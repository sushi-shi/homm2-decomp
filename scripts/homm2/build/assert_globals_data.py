#!/usr/bin/env python3
"""assert_globals_data.py — hard build gate: every global whose identifier IS a CodeView data
symbol must be declared in include/_globals.h with a `DATA(0x<its exact VA>)` annotation (VA =
rva + image base). Catches missing DATA() and wrong addresses, so a global's retail address is
recorded and can't drift. Globals with NO CodeView data symbol (synthetic modelling aliases,
e.g. gRVWeightOre) are auto-exempt. Run from repo root; exits 1 on any violation."""
import csv, re, sys

IMG = 0x400000
rva_of = {}
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] != "data":
        continue
    m = re.match(r'\?([A-Za-z_]\w*)@@', r["name"]) or re.match(r'[_@]?([A-Za-z_]\w*)', r["name"])
    if m:
        rva_of.setdefault(m.group(1), int(r["rva"], 16))

EXT = re.compile(r'^(DATA\(0x([0-9a-fA-F]+)\)\s+)?extern\b')
bad = []
seen_va = {}          # VA -> first line, to enforce DATA() addresses are unique in _globals.h
dup = []
for i, line in enumerate(open("include/_globals.h"), 1):
    dm = re.match(r'^DATA\(0x([0-9a-fA-F]+)\)', line)
    if dm:
        va = dm.group(1).lower()
        if va in seen_va:
            dup.append((i, va, seen_va[va]))
        else:
            seen_va[va] = i
for i, line in enumerate(open("include/_globals.h"), 1):
    s = line.rstrip('\n')
    if not s.lstrip().startswith(('extern', 'DATA(')):
        continue
    m = EXT.match(s)
    if not m:
        continue
    ids = re.findall(r'[A-Za-z_]\w*', s.split('//')[0].split('(extern')[-1].split('[')[0])
    # variable name = last identifier before []/;  (skip DATA/extern/type tokens)
    name = None
    mm = re.search(r'([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;', s)
    if mm:
        name = mm.group(1)
    if not name or name not in rva_of:
        continue                                  # synthetic / no CodeView data symbol -> exempt
    want = rva_of[name] + IMG
    if not m.group(2):
        bad.append((i, name, "no DATA()", "%#010x" % want))
    elif int(m.group(2), 16) != want:
        bad.append((i, name, "DATA(%#010x)" % int(m.group(2), 16), "%#010x" % want))

for ln, name, got, want in bad:
    print("  _globals.h:%d  %s  has %s  want DATA(%s)" % (ln, name, got, want))
for ln, va, first in dup:
    print("  _globals.h:%d  DATA(0x%s) duplicates the VA first used at line %d" % (ln, va, first))
if bad or dup:
    print("\nGLOBALS-DATA FAIL: %d missing/wrong DATA(VA), %d duplicate VA(s)." % (len(bad), len(dup)))
    sys.exit(1)
print("globals-data OK: every CodeView global in _globals.h carries DATA(its VA); all VAs unique.")
