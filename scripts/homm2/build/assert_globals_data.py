#!/usr/bin/env python3
"""assert_globals_data.py — hard build gate for the globals headers:
  * include/_globals.h        — every global whose identifier IS a CodeView data symbol must
    carry `DATA(0x<its exact VA>)` (VA = rva + image base); synthetic names are exempt.
  * include/_globals_model.h  — synthetic/modelling globals with NO CodeView symbol; EVERY one
    must still carry a DATA() (their VAs are pinned from the retail disassembly), and if a name
    IS a CodeView symbol its VA must match.
All DATA() VAs across BOTH files must be unique. Records each global's retail address so it
can't drift / silently lack one. Run from repo root; exits 1 on any violation."""
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
dup = []
seen_va = {}          # VA -> "file:line", enforce DATA() addresses unique across both headers

# (file, require_data_on_all): _globals_model.h globals all have known addresses -> all need DATA()
for path, require_all in (("include/_globals.h", False), ("include/_globals_model.h", True)):
    for i, line in enumerate(open(path), 1):
        dm = re.match(r'^DATA\(0x([0-9a-fA-F]+)\)', line)
        if dm:
            va = dm.group(1).lower()
            if va in seen_va:
                dup.append(("%s:%d" % (path, i), va, seen_va[va]))
            else:
                seen_va[va] = "%s:%d" % (path, i)
        s = line.rstrip('\n')
        if not s.lstrip().startswith(('extern', 'DATA(')):
            continue
        m = EXT.match(s)
        if not m:
            continue
        mm = re.search(r'([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;', s)
        name = mm.group(1) if mm else None
        if not name:
            continue
        if name in rva_of:                            # CodeView-backed -> exact VA required
            want = rva_of[name] + IMG
            if not m.group(2):
                bad.append(("%s:%d" % (path, i), name, "no DATA()", "%#010x" % want))
            elif int(m.group(2), 16) != want:
                bad.append(("%s:%d" % (path, i), name, "DATA(%#010x)" % int(m.group(2), 16), "%#010x" % want))
        elif require_all and not m.group(2):          # model global with no DATA() at all
            bad.append(("%s:%d" % (path, i), name, "no DATA()", "(pin from the retail reloc)"))

for loc, name, got, want in bad:
    print("  %s  %s  has %s  want DATA(%s)" % (loc, name, got, want))
for loc, va, first in dup:
    print("  %s  DATA(0x%s) duplicates the VA first used at %s" % (loc, va, first))
if bad or dup:
    print("\nGLOBALS-DATA FAIL: %d missing/wrong DATA(VA), %d duplicate VA(s)." % (len(bad), len(dup)))
    sys.exit(1)
print("globals-data OK: _globals.h + _globals_model.h carry DATA(their VAs); all VAs unique.")
