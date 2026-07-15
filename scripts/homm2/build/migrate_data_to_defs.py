#!/usr/bin/env python3
"""migrate_data_to_defs.py — ONE-TIME: move DATA(0x..) from an owner-header `extern` onto the
global's actual DEFINITION in its owner .cpp. A global that has a file-scope definition in some
.cpp gets `DATA(0x<VA>)` prepended to that definition, and the DATA() is stripped from its header
`extern` (leaving a plain `extern T g;`). The VA is preserved verbatim from the header.

    migrate_data_to_defs.py            # dry-run: report per-file counts
    migrate_data_to_defs.py --write    # apply
Run from repo root."""
import re, glob, sys, csv

WRITE = '--write' in sys.argv
IMG = 0x400000

def def_name(code):
    """The global a file-scope definition line declares, else None."""
    if not (code[:1].isalpha() or code[:1] == '_'):
        return None
    if '(' in code or '=' in code or ';' not in code:      # skip fns / initialised / non-decls
        return None
    m = re.match(r'^[A-Za-z_][\w\s\*]*?[\s\*]([A-Za-z_]\w*)\s*(\[[^\]]*\])*\s*;', code)
    return m.group(1) if m else None

# 1. every global that has a file-scope definition in a .cpp
defined = set()
for c in glob.glob("src/**/*.cpp", recursive=True):
    for ln in open(c):
        n = def_name(ln.split('//')[0])
        if n:
            defined.add(n)

# 1b. authoritative VA per DEFINED CodeView global (symbol_names.csv) — covers globals with no header
#     DATA extern (e.g. Bzip's file-scope defs), so every defined CodeView global gets DATA.
va_for = {}
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] != "data":
        continue
    m = re.match(r'\?([A-Za-z_]\w*)@@', r["name"]) or re.match(r'[_@]?([A-Za-z_]\w*)', r["name"])
    if m and m.group(1) in defined:
        va_for.setdefault(m.group(1), "%08x" % (int(r["rva"], 16) + IMG))

# 2. strip DATA from header externs whose global is DEFINED
hdr_hits = 0
for h in sorted(glob.glob("include/**/*.h", recursive=True)):
    out, changed = [], 0
    for ln in open(h):
        m = re.match(r'^(DATA\(0x([0-9a-fA-F]+)\)\s+)(extern\s+.*)$', ln.rstrip('\n'))
        if m:
            nm = re.search(r'([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;', m.group(3).split('//')[0])
            name = nm.group(1) if nm else None
            if name in defined:
                va_for.setdefault(name, m.group(2))
                out.append(m.group(3) + "\n"); changed += 1; continue
        out.append(ln)
    if changed:
        hdr_hits += changed
        print("  %2d DATA stripped <- %s" % (changed, h))
        if WRITE:
            open(h, "w").write("".join(out))

# 3. prepend DATA(VA) to the .cpp definitions of those globals
cpp_hits = 0
for c in sorted(glob.glob("src/**/*.cpp", recursive=True)):
    out, changed = [], 0
    for ln in open(c):
        n = def_name(ln.split('//')[0])
        if n and n in va_for and not ln.lstrip().startswith('DATA('):
            out.append("DATA(0x%s) %s" % (va_for[n], ln)); changed += 1; continue
        out.append(ln)
    if changed:
        cpp_hits += changed
        print("  %2d DATA added   -> %s" % (changed, c))
        if WRITE:
            open(c, "w").write("".join(out))

print("\n%s: %d header externs -> %d .cpp definitions (%s)" %
      ("APPLIED" if WRITE else "DRY-RUN", hdr_hits, cpp_hits, "written" if WRITE else "no changes"))
if hdr_hits != cpp_hits:
    print("  NOTE: header/def counts differ — some defined globals had no header-DATA, or vice versa")
