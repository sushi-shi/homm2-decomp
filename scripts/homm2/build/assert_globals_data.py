#!/usr/bin/env python3
"""assert_globals_data.py — hard build gate for global declarations. Since _globals.h was
dissolved, globals now live in their owner-TU headers (CodeView-attributed) + _globals_model.h
(globals with NO CodeView symbol, which can't be attributed). Enforces:
  * every `extern` global that IS a CodeView data symbol carries DATA(0x<its exact VA>);
  * a global with no CodeView symbol lives ONLY in _globals_model.h (and still carries DATA);
  * every DATA() VA is UNIQUE across all headers (one VA == one global);
  * NO DATA() appears in a .cpp — the annotation belongs on the header declaration.
Run from repo root; exits 1 on any violation."""
import csv, re, sys, glob, os

IMG = 0x400000
rva_of = {}
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] != "data":
        continue
    m = re.match(r'\?([A-Za-z_]\w*)@@', r["name"]) or re.match(r'[_@]?([A-Za-z_]\w*)', r["name"])
    if m:
        rva_of.setdefault(m.group(1), int(r["rva"], 16))

DATA_RE = re.compile(r'^\s*DATA\(0x([0-9a-fA-F]+)\)')
EXT = re.compile(r'^(DATA\(0x([0-9a-fA-F]+)\)\s+)?extern\b')
NAME = re.compile(r'([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;')

bad = []; dup = []; in_cpp = []; seen = {}

# (1) DATA() belongs in headers only — flag any in a .cpp.
for c in sorted(glob.glob("src/**/*.cpp", recursive=True)):
    for i, line in enumerate(open(c), 1):
        if DATA_RE.match(line):
            in_cpp.append("%s:%d" % (c, i))

# (2) scan every header: VA uniqueness + per-global DATA(VA) correctness.
for h in sorted(glob.glob("include/**/*.h", recursive=True)):
    is_model = os.path.basename(h) == "_globals_model.h"
    for i, line in enumerate(open(h), 1):
        loc = "%s:%d" % (h, i)
        dm = DATA_RE.match(line)
        if dm:
            va = dm.group(1).lower()
            if va in seen:
                dup.append((loc, va, seen[va]))
            else:
                seen[va] = loc
        m = EXT.match(line)
        if not m:
            continue
        nm = NAME.search(line.split('//')[0])
        name = nm.group(1) if nm else None
        if not name:
            continue                                  # e.g. `extern "C" T f(...);` — a function
        if name in rva_of:                            # CodeView global -> exact DATA(VA) required
            want = rva_of[name] + IMG
            if not m.group(2):
                bad.append((loc, name, "no DATA()", "%#010x" % want))
            elif int(m.group(2), 16) != want:
                bad.append((loc, name, "DATA(%#010x)" % int(m.group(2), 16), "%#010x" % want))
        elif not is_model:                            # no CodeView symbol -> only _globals_model.h
            bad.append((loc, name, "no CodeView symbol -> belongs in _globals_model.h", "—"))
        elif not m.group(2):
            bad.append((loc, name, "no DATA()", "(pin from the retail reloc)"))

for loc, name, got, want in bad:
    print("  %s  %s  %s  (want DATA %s)" % (loc, name, got, want))
for loc, va, first in dup:
    print("  %s  DATA(0x%s) repeats a VA first used at %s" % (loc, va, first))
for loc in in_cpp:
    print("  %s  DATA() in a .cpp — move it to the global's header declaration" % loc)
if bad or dup or in_cpp:
    print("\nGLOBALS-DATA FAIL: %d decl issue(s), %d duplicate VA(s), %d in-.cpp." % (len(bad), len(dup), len(in_cpp)))
    sys.exit(1)
print("globals-data OK: every CodeView global carries DATA(its VA); VAs unique across headers; none in .cpp.")
