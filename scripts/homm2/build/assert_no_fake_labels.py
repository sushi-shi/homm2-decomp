#!/usr/bin/env python3
"""assert_no_fake_labels.py — soundness gate: every FUNCTION symbol a TU's object defines
must actually exist in the original CodeView/PDB symbol set (build/gen/symbol_names.csv).
Catches invented labels and wrong signatures that mangle to a name the retail binary
never had. Run from repo root after a build; exits 1 if any object defines a function
symbol absent from CodeView.

A defined, external, function-typed COFF symbol (llvm-objdump: sec>0, ty 20, scl 2) is a
"label". FAKE = not in CodeView at all. MISATTRIBUTED = in CodeView but under a different
unit (usually a wrong-TU definition)."""
import csv, subprocess, os, sys, glob, re

rows = list(csv.DictReader(open("build/gen/symbol_names.csv")))
global_funcs = {r["name"] for r in rows if r["kind"] == "func"}
unit_funcs = {}
for r in rows:
    if r["kind"] == "func":
        unit_funcs.setdefault(r["unit"], set()).add(r["name"])

# symbols the compiler may synthesize that CodeView legitimately lists as funcs anyway are
# already in global_funcs (??_E/??_G/??_H...). Nothing extra to whitelist.

SYM_RE = re.compile(r'\(sec\s+(\d+)\).*\(ty\s+([0-9a-fA-F]+)\).*\(scl\s+(\d+)\).*?\s(\S+)$')

def defined_funcs(obj):
    out = subprocess.run(["llvm-objdump", "-t", obj], capture_output=True, text=True).stdout
    names = set()
    for line in out.splitlines():
        m = SYM_RE.search(line)
        if not m:
            continue
        sec, ty, scl, name = int(m.group(1)), m.group(2), int(m.group(3)), m.group(4)
        if sec > 0 and ty == "20" and scl == 2:      # defined, function, external
            names.add(name)
    return names

fake = {}; misattr = {}
objs = sorted(glob.glob("build/objdiff/base/**/*.obj", recursive=True))
for obj in objs:
    unit = os.path.relpath(obj, "build/objdiff/base")[:-4]   # e.g. BASE/BITS
    for s in defined_funcs(obj):
        if s not in global_funcs:
            fake.setdefault(unit, []).append(s)
        elif s not in unit_funcs.get(unit, set()):
            misattr.setdefault(unit, []).append(s)

for unit in sorted(fake):
    print("FAKE   %-24s %s" % (unit, sorted(fake[unit])[:10]))
for unit in sorted(misattr):
    print("MISATTR %-23s %s" % (unit, sorted(misattr[unit])[:10]))

nf = sum(len(v) for v in fake.values())
nm = sum(len(v) for v in misattr.values())
print("\n%d fake label(s), %d misattributed, across %d objects" % (nf, nm, len(objs)))
if nf:
    print("FAIL: objects define function symbols that do not exist in CodeView.")
    sys.exit(1)
print("no-fake-labels OK: every emitted function symbol exists in CodeView.")
