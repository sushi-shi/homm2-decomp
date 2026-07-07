#!/usr/bin/env python3
"""verify_carcass.py — end-to-end check that every CodeView function symbol for each unit
is PRESENT in its generated object file (build/objdiff/base/<unit>.obj). Run from repo root
after a build. Reports per-unit missing symbols (the carcass is complete when 0 missing)."""
import csv, subprocess, os
from collections import defaultdict

exp = defaultdict(set)
for row in csv.DictReader(open("build/gen/symbol_names.csv")):
    if row["kind"] == "func":
        exp[row["unit"]].add(row["name"])

total_missing = 0
units_incomplete = 0
for unit in sorted(exp):
    obj = "build/objdiff/base/%s.obj" % unit
    if not os.path.exists(obj):
        print("%-24s OBJ MISSING" % unit); units_incomplete += 1; continue
    out = subprocess.run(["llvm-objdump", "-t", obj], capture_output=True, text=True).stdout
    present = set()
    for line in out.splitlines():
        parts = line.split()
        if parts:
            present.add(parts[-1])
    miss = sorted(n for n in exp[unit] if n not in present)
    if miss:
        units_incomplete += 1; total_missing += len(miss)
        print("%-24s %3d/%3d present, MISSING %d: %s" %
              (unit, len(exp[unit]) - len(miss), len(exp[unit]), len(miss), miss[:8]))

print("\n%d units incomplete, %d total missing symbols" % (units_incomplete, total_missing))
if total_missing == 0:
    print("CARCASS COMPLETE: every CodeView function symbol is present in its object.")
