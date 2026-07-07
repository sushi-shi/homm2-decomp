#!/usr/bin/env python3
"""gen_global_decls.py — EAGER global declarations.

Every CodeView-owned data symbol gets a `DATA(<VA>) extern <type> <name>;` in its owner header
UP FRONT, so matching a TU never requires hand-adding globals (the old lazy model). Reads
build/gen/symbol_names.csv (kind==data), demangles each via llvm-undname, reconstructs array
dims from the recorded byte size (MSVC 4.2 records array globals as *decayed pointers* + a size,
e.g. `void**`+0x180 == `void*[0x60]`, `char(*)[2]`+0xc0 == `char[0x60][2]`), and writes a managed
`// ---- globals (declarations, RVA order) ----` block into the owner header (the .cpp's own-class
include). Dedups against any already-declared name (hand-written decls win). Anything it cannot
confidently reconstruct (unknown struct element size, exotic type) is SKIPPED and logged, so it
only ever emits well-formed decls. Pair with gen_global_defs.py (definitions).

Usage: gen_global_decls.py            # dry-run: print what each header would gain
       gen_global_decls.py --write    # apply
"""
import csv, re, subprocess, os, sys
from collections import defaultdict

IMAGE_BASE = 0x400000
PTR = 4
SCALAR = {'char':1,'signed char':1,'unsigned char':1,'short':2,'unsigned short':2,'int':4,
          'unsigned int':4,'long':4,'unsigned long':4,'float':4,'double':8,'void':1,'bool':1}

def demangle(names):
    """batch-demangle; llvm-undname echoes each input then its decl on the next line."""
    out = subprocess.run(['llvm-undname'] + names, capture_output=True, text=True).stdout.splitlines()
    res, i = {}, 0
    lines = [l for l in out]
    # llvm-undname prints: <mangled>\n<demangled>\n  per input
    j = 0
    while j + 1 < len(lines):
        mangled, dem = lines[j].strip(), lines[j+1].strip()
        if mangled in names:
            res[mangled] = dem
            j += 2
        else:
            j += 1
    return res

def owner_header(src):
    if not os.path.exists(src):
        return None
    for line in open(src):
        m = re.match(r'\s*#include\s+[<"]([^>"]+\.h)[>"]', line)
        if m and not m.group(1).endswith('va.h'):
            return 'include/' + m.group(1)
    return None

def reconstruct(dem, name, size):
    """The demangled decl IS the declared type — it round-trips to the CodeView mangled name, so
    the linker/asserts see the exact symbol. Emit it verbatim as `<type> <name>;`. `size` is
    UNRELIABLE (often the span to the next symbol, e.g. a scalar `int` with size 0x248), so it is
    NOT used to synthesise array dims; the demangled form already carries the true dims (MSVC decays
    a global array's outer dim, keeping inner dims: `char[0x60][2]` -> `char (*x)[2]`).

    Skip only user-defined-type VALUES and UDT arrays (defining one needs the *complete* struct,
    which the owner header may not have); a plain pointer to a UDT is fine (forward-decl)."""
    dem = dem.strip()
    # Decayed ARRAY forms are dense (size == true storage), so reconstruct the real array — it
    # mangles identically to the decayed pointer (verified: void*[0x60] and void** both -> PAPAXA)
    # but is faithful + usable (a ctor can write name[i]).
    #   PAY: 'T (*name)[inner...]'  -> 'T name[outer][inner...]'   (only for scalar element T)
    m = re.match(r'^(.*?)\s*\(\*' + re.escape(name) + r'\)((?:\[\d+\])+)$', dem)
    if m and m.group(1).strip() in SCALAR:
        base, dims = m.group(1).strip(), m.group(2)
        stride = SCALAR[base]
        for d in re.findall(r'\[(\d+)\]', dims):
            stride *= int(d)
        if size % stride == 0 and size > stride:
            return "%s %s[%d]%s;" % (base, name, size // stride, dims)
    #   PAPAX etc.: 'T **...name' (>=2 stars) -> 'T *...name[outer]'   (1D array of pointers)
    m = re.match(r'^(.*?)(\*{2,})\s*' + re.escape(name) + r'$', dem)
    if m and size > PTR and size % PTR == 0:
        return "%s %s%s[%d];" % (m.group(1).strip(), '*' * (len(m.group(2)) - 1), name, size // PTR)
    # everything else verbatim (scalars, single-ptr handles, struct ptrs); skip UDT values/arrays
    if re.match(r'^(struct|class|union|enum)\b', dem):
        if re.match(r'^(struct|class|union|enum)\s+[\w:]+\s*\*+\s*' + re.escape(name) + r'$', dem):
            return dem + ";"
        return None
    return dem + ";"

def main():
    write = '--write' in sys.argv
    units = {}
    for m in re.finditer(r'unit\s*=\s*"([^"]+)"\s*\n\s*source\s*=\s*"([^"]+)"', open('config/units.toml').read()):
        units[m.group(1)] = m.group(2)

    # already-declared names anywhere (dedup; hand-written decls win)
    declared = set()
    for h in glob_headers():
        in_block = False                                  # skip OUR managed block (idempotent re-runs)
        for line in open(h):
            if 'globals (declarations' in line:
                in_block = True; continue
            if in_block:
                if line.startswith('DATA(') or not line.strip():
                    continue
                in_block = False
            mm = re.search(r'\bextern\b.*?([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;', line.split('//')[0])
            if mm:
                declared.add(mm.group(1))
    # also skip globals already DEFINED file-scope in a .cpp (else gen_global_defs redefines them)
    for src in set(units.values()):
        if not os.path.exists(src):
            continue
        for line in open(src):
            if 'globals (definitions' in line:                # gen_global_defs block runs to EOF
                break
            if not (line[:1].isalpha() or line[:1] == '_'):   # file-scope defs begin at col 0 w/ a type
                continue
            code = line.split('//')[0]
            if '(' in code or '=' in code or ';' not in code:  # skip functions / initialised / non-decls
                continue
            dm = re.match(r'^[A-Za-z_][\w\s\*]*?[\s\*]([A-Za-z_]\w*)\s*(\[[^\]]*\])*\s*;', code)
            if dm:
                declared.add(dm.group(1))

    rows = [r for r in csv.DictReader(open('build/gen/symbol_names.csv')) if r['kind'] == 'data']
    mangled = sorted({r['name'] for r in rows if r['name'].startswith('?') and '@@3' in r['name']})
    dem = demangle(mangled)

    by_header, skipped = defaultdict(list), []
    for r in rows:
        nm = r['name']
        m = re.match(r'\?([A-Za-z_]\w*)@@3', nm)
        if not m:
            continue
        name = m.group(1)
        if name in declared or nm not in dem:
            continue
        hdr = owner_header(units.get(r['unit'], ''))
        if not hdr:
            skipped.append((name, 'no owner header for %s' % r['unit'])); continue
        decl = reconstruct(dem[nm], name, int(r['size'], 0))
        if not decl:
            skipped.append((name, 'unresolved: %s (size 0x%x)' % (dem[nm], int(r['size'], 0)))); continue
        va = int(r['rva'], 0) + IMAGE_BASE
        by_header[hdr].append((int(r['rva'], 0), "DATA(0x%08x) extern %s" % (va, decl)))
        declared.add(name)

    BLOCK = re.compile(r'\n// ---- globals \(declarations, RVA order\) ----\n(?:DATA\(.*\n)*', re.M)
    for hdr, decls in sorted(by_header.items()):
        decls.sort()
        block = "\n// ---- globals (declarations, RVA order) ----\n" + "\n".join(d for _, d in decls) + "\n"
        print("  %2d decls -> %s" % (len(decls), hdr))
        if write and os.path.exists(hdr):
            t = open(hdr).read()
            t = BLOCK.sub("", t)
            t = re.sub(r'\n(#endif[^\n]*\n?)\Z', block + r'\n\1', t) if re.search(r'\n#endif[^\n]*\n?\Z', t) else t.rstrip() + block
            open(hdr, 'w').write(t)
    print("emitted %d decls across %d headers; skipped %d" % (sum(len(v) for v in by_header.values()), len(by_header), len(skipped)))
    for n, why in skipped[:40]:
        print("    skip %-24s %s" % (n, why))

import glob as _glob
def glob_headers():
    return _glob.glob('include/**/*.h', recursive=True)

if __name__ == '__main__':
    main()
