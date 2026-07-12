#!/usr/bin/env python3
"""assert_relocs.py — OPT-IN reloc-target audit (`homm2 relocs`; NOT a hard build gate). For every
near-EXACT function, verify each relocation's TARGET address. objdiff MASKS all relocations when
scoring, so it never checks the target — a "100% match" can silently reference the WRONG
global/const/field/function, or even call a FABRICATED function that is declared, never defined, and
(because we compile-but-don't-link) never caught as an unresolved external. This closes that blind spot.

Order-INDEPENDENT: we resolve each side's relocs to a SET of final RVAs (symbol RVA + instruction
addend; REL32 -> the symbol's own RVA) and flag any address the BASE references that retail never
does. We also flag any base '?'-mangled symbol that resolves to NEITHER CodeView (symbol_names.csv)
NOR a header DATA() global — i.e. fabricated.

Data offsets come from symbol_names.csv (authoritative CodeView RVAs). Synthetic _globals_model.h
globals carry no CodeView symbol, so their VA is read from their header DATA(0x..) annotation.

It is OPT-IN (not in `homm2 build`'s hard gates) because it also surfaces unreproducible link
artifacts — chiefly the delinker's `empty_stub`, the synthetic name for a COMDAT-folded empty (`ret`)
function that base still calls by its own CodeView name. That is a delinker-side concern, not a
source bug, so it must not break every build.

Review one function (order-independent; usable on <100% walls):
    homm2 relocs 0x<rva>
Run from repo root; exits 1 on any wrong/fabricated reloc target.
"""
import sys, os, re, csv, json, glob, subprocess
from collections import Counter

IMAGE_BASE = 0x400000

def _norm(s):   # llvm-objdump renders each non-printable mangled byte as one '_'; symbol_names.csv
    s = s.replace('\xef\xbf\xbd', '_')          # stores it as the UTF-8 replacement char (3 bytes)
    return ''.join(c if 0x20 <= ord(c) < 0x7f else '_' for c in s)
def _key(s):    # encoding-stable key for ??_C@ strings: the ASCII "??_C@_<len>@<hash>@" prefix
    m = re.match(r'(\?\?_C@_\w+@\w+@)', s); return m.group(1) if m else _norm(s)

def load_symbols():
    sym, dups = {}, {}
    for r in csv.reader(open("build/gen/symbol_names.csv", encoding="latin-1")):
        if len(r) >= 2:
            try:
                v = int(r[0], 16)
            except ValueError:
                continue
            for k in (r[1], _norm(r[1]), _key(r[1])):
                # content-hashed string names (??_C@) can COLLIDE — two distinct addresses share one
                # name (e.g. two "!" literals). Record every address per name so an ambiguous target
                # reloc can match ANY of them (they're value-identical), not just the last one loaded.
                sym.setdefault(k, v); dups.setdefault(k, set()).add(v)
    data = {}                                    # DATA() now lives on .cpp definitions (+ the
    for f in glob.glob("src/**/*.cpp", recursive=True) + glob.glob("include/**/*.h", recursive=True):
        for ln in open(f, encoding="latin-1"):   # _globals_model.h externs, for def-less synthetics)
            m = re.search(r'DATA\(0x([0-9a-fA-F]+)\).*?\b([A-Za-z_]\w*)\s*(?:\[|;|=)', ln)
            if m:
                data[m.group(2)] = int(m.group(1), 16) - IMAGE_BASE
    return sym, data, dups

def resolve(sym, data, typ, s, add):
    mc = re.match(r'const_([0-9a-fA-F]+)$', s)  # delinker names unlabeled data addrs const_<RVA>
    if mc:
        b = int(mc.group(1), 16)
    else:
        b = sym.get(s) or sym.get(_norm(s)) or sym.get(_key(s))
        if b is None:
            mm = re.match(r'\?(\w+)@@', s)      # ?<name>@@... -> a DATA()-pinned global
            if mm:
                b = data.get(mm.group(1))
    if b is None:
        return None
    return b if typ == 'REL32' else (b + add) & 0xffffffff

def is_fake(sym, data, s):
    if s.startswith('_') or s.startswith('??_C@') or s.startswith('$SG') or not s.startswith('?'):
        return False                             # CRT/import/string/local -> outside CodeView, fine
    return resolve(sym, data, 'DIR32', s, 0) is None

def _addend(insn):
    insn = insn.split('#')[0]                    # drop llvm-objdump's "# imm = 0xNN" annotation
    m = re.search(r'\[(0x[0-9a-f]+)\]', insn)    # absolute-memory operand [0xN] -> addend N
    if m:
        return int(m.group(1), 16)
    imms = re.findall(r'(?<![\w\]])(-?0x[0-9a-f]+)', insn)   # last standalone (SIGNED) immediate/disp
    return int(imms[-1], 16) & 0xffffffff if imms else 0     # -0x4c disp -> 0xffffffb4 addend

def parse_obj(obj):
    """llvm-objdump -dr -> {func_name: [(type, symbol, addend), ...]} in order; __imp__ skipped."""
    out = subprocess.run(["llvm-objdump", "-dr", obj], capture_output=True, text=True).stdout
    funcs, cur, prev = {}, None, ""
    for ln in out.splitlines():
        m = re.match(r'^[0-9a-f]+ <(.+?)>:', ln)
        if m:
            cur = m.group(1); funcs.setdefault(cur, []); prev = ""; continue
        if cur is None:
            continue
        mr = re.search(r'IMAGE_REL_I386_(\w+)\s+(\S+)', ln)
        if mr:
            s = mr.group(2)
            if not s.startswith('__imp__'):
                funcs[cur].append((mr.group(1), s, _addend(prev)))
            continue
        mi = re.match(r'^\s*[0-9a-f]+:\s+(?:[0-9a-f]{2}\s+)+(.+)$', ln)
        if mi:
            prev = mi.group(1).replace('\t', ' ').strip()
    return funcs

# Pre-existing reloc-target discrepancies surfaced when this gate was introduced (objdiff masked
# them, so they scored ~100%). Two kinds: (A) SUSPECTED wrong VA — a base symbol that disagrees with
# where retail reads (likely a wrong synthetic-global DATA(0x..) guess, or a wrong array base — the
# gDwelling*/gHierarchyMask/gMonsterInfo cases are the same shape: base is 0x4c off retail). (B)
# factoring/codegen differences the multiset can't distinguish (base uses a static field+offset or a
# direct call where retail computes the offset at runtime / dispatches virtually — same effect).
# TODO: confirm each and fix (correct the VA / restructure the source), then delete the entry.
# Keyed (unit, function, base-symbol).
ALLOWLIST = set()

def _cands(dups, s, add):
    """Every address a target reloc name could denote, +addend. Content-hashed string names (??_C@)
    COLLIDE — one name, several value-identical addresses; the reloc may mean ANY, so accept all."""
    c = dups.get(s) or dups.get(_norm(s)) or dups.get(_key(s)) or set()
    return {(a + add) & 0xffffffff for a in c}

def _tvas(sym, data, dups, tgt_relocs):
    """Multiset of every RVA retail references. Order-independent (robust to near-100%-wall reorder).
    An ambiguous (dup) name contributes ALL its candidate addrs per occurrence, so a base ref to any
    one of them clears; a unique name contributes its single resolved addr."""
    tvas = Counter()
    for r in tgt_relocs:
        v = resolve(sym, data, *r)
        if v is None:
            continue
        cs = _cands(dups, r[1], r[2]) if r[0] != 'REL32' else set()
        if len(cs) > 1:
            for c in cs:
                tvas[c] += 1
        else:
            tvas[v] += 1
    return tvas

def check_fn(sym, data, dups, unit, name, base_relocs, tgt_relocs):
    """Order-INDEPENDENT MULTISET reloc-target check. We flag `only_base`: an address the BASE
    references MORE times than retail does — a wrong global/const/field/fn (multiset, not set, so an
    injected wrong ref is caught even when that address is ALSO referenced elsewhere in the same fn).
    Plus a FAKE check for unresolvable base '?'-symbols. Factoring/near-wall reorder no longer false-
    positive here: they're fixed at the resolve layer (signed addend, const_<rva>, dup names)."""
    probs = []
    for r in base_relocs:                        # (1) fabricated function/data symbol
        if is_fake(sym, data, r[1]) and (unit, name, r[1]) not in ALLOWLIST:
            probs.append("FAKE call/ref '%s' — exists in neither CodeView nor a DATA() global" % r[1])
    tvas = _tvas(sym, data, dups, tgt_relocs)
    bvas, va_sym = Counter(), {}
    for r in base_relocs:
        v = resolve(sym, data, *r)
        if v is not None:
            bvas[v] += 1; va_sym.setdefault(v, r[1])
    for v, n in (bvas - tvas).items():            # (2) base references an addr retail never does (or fewer)
        bs = va_sym[v]
        if '_00A@' in bs or (unit, name, bs) in ALLOWLIST:
            continue
        probs.append("WRONG: base references 0x%x (%s) x%d — retail never references it (or fewer)" % (v, bs, n))
    return probs

def review(rva):
    """Single-function multiset review (order-independent; usable on <100% walls)."""
    sym, data, dups = load_symbols()
    def disasm(side):
        out = subprocess.run(["homm2", "sema", "disasm", rva, side], capture_output=True, text=True).stdout
        rel, prev = [], ""
        for ln in out.splitlines():
            mr = re.search(r'IMAGE_REL_I386_(\w+)\s+(\S+)', ln)
            if mr:
                s = mr.group(2)
                if not s.startswith('__imp__'):
                    rel.append((mr.group(1), s, _addend(prev)))
                continue
            mi = re.match(r'^\s*[0-9a-f]+:\s+(?:[0-9a-f]{2}\s+)+(.+)$', ln)
            if mi:
                prev = mi.group(1).replace('\t', ' ').strip()
        return rel
    B, T = disasm("--base"), disasm("--target")
    for s in sorted({r[1] for r in B if is_fake(sym, data, r[1])}):
        print("  !! FAKE base references '%s'" % s)
    def bvas(rs):
        c, sy = Counter(), {}
        for r in rs:
            if '_00A@' in r[1]:
                continue
            v = resolve(sym, data, *r)
            if v is not None:
                c[v] += 1; sy.setdefault(v, r[1])
        return c, sy
    bc, va_sym = bvas(B)
    tvas = _tvas(sym, data, dups, [r for r in T if '_00A@' not in r[1]])
    diff = bc - tvas
    for v, n in sorted(diff.items()):
        print("  base references 0x%x (%s) x%d that retail never does (wrong/extra global/const/fn)"
              % (v, va_sym[v], n))
    print("base relocs=%d target relocs=%d  only-base=%d" % (len(B), len(T), sum(diff.values())))

def main():
    if len(sys.argv) > 1:                        # single-function review mode
        review(sys.argv[1]); return 0
    sym, data, dups = load_symbols()
    report = json.load(open("build/objdiff/report.json"))
    bad = []
    # A WRONG reloc doesn't drop objdiff to a low %, it costs a TINY fraction (~0.005%/reloc) that
    # rounds to "100.00%" in the display — so we must audit near-exact fns, not just == 100. The
    # multiset check (check_fn) is order-independent, so 99.5% (with its minor reordering) is safe.
    THRESHOLD = 99.5
    for u in report["units"]:
        unit = u["name"]
        exact = {f["name"] for f in u.get("functions", []) if f.get("fuzzy_match_percent", 0) >= THRESHOLD}
        if not exact:
            continue
        base_obj = "build/objdiff/base/%s.obj" % unit
        tgt_obj = "build/delink/%s.c.obj" % unit
        if not (os.path.exists(base_obj) and os.path.exists(tgt_obj)):
            continue
        bf, tf = parse_obj(base_obj), parse_obj(tgt_obj)
        for name in sorted(exact):
            if name not in bf or name not in tf:
                continue
            for p in check_fn(sym, data, dups, unit, name, bf[name], tf[name]):
                bad.append((unit, name, p))
    for unit, name, p in bad:
        print("  %s  %s: %s" % (unit, name, p))
    if bad:
        print("\nRELOCS FAIL: %d exact function(s) reference a wrong/fabricated target "
              "(objdiff masks relocs, so it can't see this)." % len(bad))
        return 1
    print("relocs OK: every 100%-exact function's reloc targets resolve to the retail address.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
