#!/usr/bin/env python3
"""assert_vtables.py — hard build gate for vtables. Vtables (CodeView ??_7Class@@6B@ data
symbols) are compiler-emitted, one per class, attributed to an owning TU. Each owner TU carries
a `VTBL(Class, 0xVA);` census marker at its tail. This gate cross-checks three ways so the set
can't drift and no fake class can slip in:

  A. COVERAGE      every CodeView vtable has a VTBL() marker in its owner TU, with matching VA.
  B. NO FAKE CLAIM every VTBL() marker names a real CodeView vtable, in its correct owner TU.
  C. NO FAKE VTBL  every vtable EMITTED (defined) in a compiled object has a VTBL() marker —
                   so a reconstructed class with virtuals can't emit a vtable that isn't
                   accounted for (an invented class would trip this or B).

Runs after ninja (C needs the objs + llvm-objdump). Run from repo root; exits 1 on violation."""
import csv, re, sys, glob, os, subprocess
IMG = 0x400000

# CodeView vtables: class -> (va, owner unit)
cv = {}
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] != "data":
        continue
    m = re.match(r'\?\?_7(\w+)@@6B@$', r["name"])
    if m:
        cv[m.group(1)] = (int(r["rva"], 0) + IMG, r["unit"])

# VTBL() markers in source: class -> (va, cpp)
VT = re.compile(r'^VTBL\((\w+),\s*0x([0-9a-fA-F]+)\)')
src = {}; dups = []
for cpp in sorted(glob.glob("src/**/*.cpp", recursive=True)):
    for line in open(cpp):
        m = VT.match(line)
        if m:
            cls, va = m.group(1), int(m.group(2), 16)
            if cls in src:
                dups.append(cls)
            src[cls] = (va, cpp)

bad = []
# A. every CodeView vtable is claimed, in its owner TU, at the right VA
for cls, (va, unit) in sorted(cv.items()):
    if cls not in src:
        bad.append("A: CodeView vtable %s (owner %s) has NO VTBL() marker" % (cls, unit)); continue
    sva, scpp = src[cls]
    if sva != va:
        bad.append("A: VTBL(%s) VA 0x%08x != CodeView 0x%08x" % (cls, sva, va))
    if scpp != "src/%s.cpp" % unit:
        bad.append("A: VTBL(%s) is in %s but CodeView owner is %s" % (cls, scpp, unit))
# B. every marker names a real CodeView vtable
for cls, (va, cpp) in sorted(src.items()):
    if cls not in cv:
        bad.append("B: VTBL(%s) in %s is not a CodeView vtable (fake class?)" % (cls, cpp))
for cls in dups:
    bad.append("B: VTBL(%s) declared in more than one TU" % cls)

# C. every vtable emitted (defined) in a compiled obj has a marker
SYM = re.compile(r'\[\s*\d+\]\(sec\s+(-?\d+)\).*?\)\s+0x[0-9a-f]+\s+\?\?_7(\w+)@@6B@\s*$')
for obj in glob.glob("build/objdiff/base/**/*.obj", recursive=True):
    out = subprocess.run(["llvm-objdump", "-t", obj], capture_output=True, text=True).stdout
    for line in out.splitlines():
        m = SYM.match(line)
        if m and int(m.group(1)) > 0 and m.group(2) not in src:
            bad.append("C: %s emits vtable ??_7%s@@6B@ with no VTBL() marker" % (obj, m.group(2)))

for b in bad:
    print("  " + b)
if bad:
    print("\nVTABLES FAIL: %d issue(s)." % len(bad)); sys.exit(1)
print("vtables OK: %d CodeView vtables, all claimed by VTBL() in their owner TU; none fake/undeclared." % len(cv))
