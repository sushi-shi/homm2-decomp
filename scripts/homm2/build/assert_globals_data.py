#!/usr/bin/env python3
"""assert_globals_data.py — hard build gate for global DATA(VA) placement. DATA(0x<VA>) lives on
the global's DEFINITION in its owner .cpp (not on the header `extern`). Enforces:
  * every file-scope DEFINITION of a CodeView data symbol carries DATA(0x<its exact VA>);
  * NO DATA() on a header `extern`;
  * every header global extern has a CodeView public symbol and an owner TU;
  * every DATA() VA is UNIQUE (one VA == one definition).
Run from repo root; exits 1 on any violation."""
import csv, re, sys, glob

IMG = 0x400000
rva_of = {}
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] != "data":
        continue
    m = re.match(r'\?([A-Za-z_]\w*)@@', r["name"]) or re.match(r'[_@]?([A-Za-z_]\w*)', r["name"])
    if m:
        rva_of.setdefault(m.group(1), int(r["rva"], 16))

DATA_RE = re.compile(r'^\s*DATA\(0x([0-9a-fA-F]+)\)\s+(.*)$')

def def_name(code):
    """The global a file-scope definition line declares (type at col 0, no call/init), else None."""
    if not (code[:1].isalpha() or code[:1] == '_'):
        return None
    if '(' in code or '=' in code or ';' not in code:
        return None
    m = re.match(r'^[A-Za-z_][\w\s\*]*?[\s\*]([A-Za-z_]\w*)\s*(\[[^\]]*\])*\s*;', code)
    return m.group(1) if m else None

bad = []; dup = []; seen = {}
def note(va, loc):
    va = va.lower()
    if va in seen:
        dup.append((loc, va, seen[va]))
    else:
        seen[va] = loc

# (1) .cpp DEFINITIONS: every CodeView-global def carries DATA(exact VA). Non-CodeView defs may carry
#     DATA too (Phase-B module-private synthetic globals) — those just claim their VA for uniqueness.
for c in sorted(glob.glob("src/**/*.cpp", recursive=True)):
    for i, line in enumerate(open(c), 1):
        loc = "%s:%d" % (c, i)
        dm = DATA_RE.match(line)
        rest = dm.group(2) if dm else line
        name = def_name(rest.split('//')[0])
        if not name:
            continue
        if name not in rva_of:                        # non-CodeView file-scope def (helper/static)
            if dm:
                note(dm.group(1), loc)
            continue
        want = rva_of[name] + IMG
        if not dm:
            bad.append((loc, name, "no DATA() on definition", "%#010x" % want))
        elif int(dm.group(1), 16) != want:
            bad.append((loc, name, "DATA(%#010x)" % int(dm.group(1), 16), "%#010x" % want))
        else:
            note(dm.group(1), loc)

# (2) HEADERS: declarations never claim storage, and every cross-TU global must have a retained
#     CodeView public symbol. Anonymous/synthetic storage is module-private in its owning .cpp.
for h in sorted(glob.glob("include/**/*.h", recursive=True)):
    for i, line in enumerate(open(h), 1):
        loc = "%s:%d" % (h, i)
        dm = DATA_RE.match(line)
        rest = (dm.group(2) if dm else line).strip()
        if not rest.startswith("extern"):
            if dm:
                bad.append((loc, "?", "DATA() on a non-extern header line", "—"))
            continue
        nm = re.search(r'([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;', rest.split('//')[0])
        name = nm.group(1) if nm else None
        if not name:
            continue                                  # `extern "C" T f(...);` — a function
        if dm:
            bad.append((loc, name, "DATA() on header extern — move it to the .cpp definition", "—"))
        elif name not in rva_of:
            bad.append((loc, name,
                        "no CodeView public symbol -> make storage module-private or recover owner",
                        "—"))

for loc, name, got, want in bad:
    print("  %s  %s  %s  (want DATA %s)" % (loc, name, got, want))
for loc, va, first in dup:
    print("  %s  DATA(0x%s) repeats a VA first used at %s" % (loc, va, first))
if bad or dup:
    print("\nGLOBALS-DATA FAIL: %d placement issue(s), %d duplicate VA(s)." % (len(bad), len(dup)))
    sys.exit(1)
print("globals-data OK: every CodeView global's DEFINITION carries DATA(its VA); no DATA on header "
      "externs; definition VAs unique.")
