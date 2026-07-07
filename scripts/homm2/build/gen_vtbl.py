#!/usr/bin/env python3
"""gen_vtbl.py — emit a VTBL(Class, 0xVA); census marker at the tail of each TU that owns a
vtable (CodeView ??_7Class@@6B@ data symbols, attributed to a compiland). Vtables are
compiler-emitted from the class's virtual methods; VTBL just records the claim so the vtable
gate can cross-check CodeView <-> source <-> emitted objects. Idempotent. Run from repo root."""
import csv, re, os, glob
from collections import defaultdict
IMG = 0x400000
by_cpp = defaultdict(list)
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] != "data":
        continue
    m = re.match(r'\?\?_7(\w+)@@6B@$', r["name"])
    if m:
        by_cpp["src/%s.cpp" % r["unit"]].append((int(r["rva"], 0) + IMG, m.group(1)))
BLOCK = re.compile(r'\n// ---- vtables \(compiler-emitted; census\) ----\n(?:VTBL\([^\n]*\n)*', re.M)
made = 0
for cpp, vts in sorted(by_cpp.items()):
    if not os.path.exists(cpp):
        print("  MISSING .cpp: %s (owns %d vtable(s))" % (cpp, len(vts))); continue
    vts.sort()
    block = "\n// ---- vtables (compiler-emitted; census) ----\n" + "".join("VTBL(%s, 0x%08x);\n" % (c, v) for v, c in vts)
    t = BLOCK.sub("", open(cpp).read()).rstrip() + "\n" + block
    open(cpp, "w").write(t); made += len(vts)
    print("  %d -> %s" % (len(vts), cpp))
print("emitted %d VTBL markers across %d TUs" % (made, len(by_cpp)))
