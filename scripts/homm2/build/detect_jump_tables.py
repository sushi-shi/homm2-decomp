#!/usr/bin/env python3
"""detect_jump_tables.py — find MSVC switch jump-tables in the retail binary that the delinker
folds into their owning function (CodeView has no `$L` label for the compiler-internal table).
Each is an indirect `jmp dword ptr [reg*4 + disp32]` (`ff 24 <SIB base=101> <disp32>`); the delinker
relocates both the jmp and every table entry as `<func>+<off>` (DIR32), so a jump-table is a run of
4-byte-spaced DIR32 self-relocs starting at the jmp's disp32 (= addend K, the table's offset in the
function). We COUNT that run to get the true entry count (robust to mid-function / multi-table cases).
Emits build/gen/jump_tables.csv: func_rva,func_name,table_rva,table_size

synth_pdb.py reads this to give each table its own symbol + shrink the owning function, so the
delinked target stops folding the table in and matches our MSVC-compiled `$L`-split layout. Ghidra
does not surface these, so we recover them ourselves. Run from repo root (needs build/delink)."""
import csv, re, glob, subprocess, struct, os

IMG = 0x400000
name_rva, rva_size = {}, {}
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] == "func":
        try:
            rva = int(r["rva"], 16)
        except ValueError:
            continue
        name_rva[r["name"]] = rva; rva_size[rva] = int(r["size"], 16)

def norm(s):
    return s.replace('\xef\xbf\xbd', '_')

tables = []
for obj in sorted(glob.glob("build/delink/**/*.c.obj", recursive=True)):
    out = subprocess.run(["llvm-objdump", "-dr", obj], capture_output=True, text=True).stdout
    # pass 1: per-defined-function section offset; and every DIR32 reloc: offset -> target name
    func_off, dir32, jmps, cur_off = {}, {}, [], None
    prev_bytes = prev_off = None
    for ln in out.splitlines():
        m = re.match(r'^([0-9a-f]+) <(.+?)>:', ln)
        if m:
            cur_off = int(m.group(1), 16); func_off.setdefault(norm(m.group(2)), cur_off); continue
        mi = re.match(r'^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s+(\S.*)$', ln)
        if mi:
            b = mi.group(2).split()
            if len(b) >= 7 and b[0] == 'ff' and b[1] == '24' and (int(b[2], 16) & 0xc7) == 0x85:
                disp = struct.unpack('<I', bytes(int(x, 16) for x in b[3:7]))[0]
                jmps.append((cur_off, disp))         # (owning-func section off, addend K)
            continue
        mr = re.match(r'^\s*([0-9a-f]+):\s+IMAGE_REL_I386_DIR32\s+(\S+)', ln)   # reloc's OWN offset
        if mr:
            dir32[int(mr.group(1), 16)] = norm(mr.group(2))
    # pass 2: for each jmp, count the 4-spaced DIR32 self-reloc run at func_off + K
    for foff, K in jmps:
        fname = next((n for n, o in func_off.items() if o == foff), None)
        if fname is None:
            continue
        frva = name_rva.get(fname)
        if frva is None or frva not in rva_size:
            continue
        start = foff + K
        n = 0
        while dir32.get(start + 4 * n) == fname:      # entry points back into this same function
            n += 1
        if n == 0 or K <= 0 or K + 4 * n > rva_size[frva]:
            continue
        tables.append((frva, fname, frva + K, 4 * n))

tables = sorted(set(tables))
os.makedirs("build/gen", exist_ok=True)
with open("build/gen/jump_tables.csv", "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["func_rva", "func_name", "table_rva", "table_size"])
    for frva, fname, trva, tsz in tables:
        w.writerow(["0x%x" % frva, fname, "0x%x" % trva, "0x%x" % tsz])
tail = sum(1 for frva, _, trva, tsz in tables if trva + tsz == frva + rva_size[frva])
print("detected %d jump-tables (%d at function tail) -> build/gen/jump_tables.csv" % (len(tables), tail))
for frva, fname, trva, tsz in tables[:14]:
    at_tail = "TAIL" if trva + tsz == frva + rva_size[frva] else "mid "
    print("  %s func 0x%05x  table func+0x%-4x  size 0x%-4x (%d entries)  %s"
          % (at_tail, frva, trva - frva, tsz, tsz // 4, fname[:44]))
