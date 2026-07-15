#!/usr/bin/env python3
"""assert_globals_defined.py — hard build gate: every global DECLARED `extern` in a header must
have a DEFINITION in its owner TU's object, so the project links (no unresolved externals). For
each CodeView-owned global declared in a header, checks its symbol is DEFINED (section > 0) in
build/objdiff/base/<owner-unit>.obj. Synthetic globals in _globals_model.h must be defined in
some reconstruction object. Only the _const pseudo-unit is exempt. Runs after
ninja (needs the built objs + llvm-objdump). Run from repo root; exits 1 on any violation."""
import csv, re, sys, glob, os, subprocess

# CodeView data symbol: demangled identifier -> (owner unit, mangled name)
info = {}
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] != "data":
        continue
    m = re.match(r'\?([A-Za-z_]\w*)@@', r["name"]) or re.match(r'[_@]?([A-Za-z_]\w*)', r["name"])
    if m:
        info.setdefault(m.group(1), (r["unit"], r["name"]))

# Globals declared `extern` in a header. Model globals have no CodeView owner, so their definitions
# are checked across all reconstruction objects below.
declared = {}
model_declared = {}
for h in glob.glob("include/**/*.h", recursive=True):
    is_model = os.path.basename(h) == "_globals_model.h"
    for line in open(h):
        if not re.match(r'^\s*(DATA\(0x[0-9a-fA-F]+\)\s+)?extern\b', line):
            continue
        nm = re.search(r'([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;', line.split('//')[0])
        if not nm:
            continue
        if is_model:
            model_declared[nm.group(1)] = os.path.relpath(h, "include")
        elif nm.group(1) in info:
            declared[nm.group(1)] = os.path.relpath(h, "include")

# defined symbols (section > 0) per owner object
_defcache = {}
def defined_in(unit):
    if unit not in _defcache:
        obj = "build/objdiff/base/%s.obj" % unit
        syms = set()
        if os.path.exists(obj):
            out = subprocess.run(["llvm-objdump", "-t", obj], capture_output=True, text=True).stdout
            for line in out.splitlines():
                m = re.match(r'\[\s*\d+\]\(sec\s+(-?\d+)\).*?\)\s+0x[0-9a-f]+\s+(\S+)\s*$', line)
                if m and int(m.group(1)) > 0:
                    syms.add(m.group(2))
        _defcache[unit] = syms
    return _defcache[unit]

bad = []
for name, hdr in sorted(declared.items()):
    unit, mangled = info[name]
    if not unit or unit == "_const":
        continue                                       # no real owning TU -> exempt
    if mangled not in defined_in(unit):
        bad.append((name, hdr, unit))

all_defined = set()
for obj in glob.glob("build/objdiff/base/**/*.obj", recursive=True):
    unit = obj[len("build/objdiff/base/"):-len(".obj")]
    all_defined.update(defined_in(unit))
for name, hdr in sorted(model_declared.items()):
    if not any(symbol.startswith("?%s@@" % name) or symbol == "_" + name
               for symbol in all_defined):
        bad.append((name, hdr, "any reconstruction unit"))

for name, hdr, unit in bad:
    print("  %s (declared in %s) has NO definition in its owner %s.obj" % (name, hdr, unit))
if bad:
    print("\nGLOBALS-DEFINED FAIL: %d extern global(s) with no definition — would not link." % len(bad))
    sys.exit(1)
print("globals-defined OK: every extern global has a definition in its owner TU.")
