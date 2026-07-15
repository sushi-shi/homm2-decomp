#!/usr/bin/env python3
"""move_model_singletons.py — ONE-TIME/repeatable: relocate each synthetic _globals_model.h global
that the RETAIL binary references from exactly ONE module into that module's .cpp as a file-private
`static` definition (DATA on the def). Multi-module globals and unsized arrays (`T g[]` — no size
to give a static) stay queued for a manually reconstructed shared owner; the global hard gates do
not permit a DATA-bearing header declaration. Retail xref = which delinked TU objs carry a
reloc resolving to the global's VA (authoritative; the source may be incompletely matched).

The `static` def is inserted before the module's first VA() (so every function sees it), and the
module keeps `#include <_globals_model.h>` so struct element types (POINT/NCB/tag_Anchor/...) remain
available. `extern `->`static ` is width-preserving, so column alignment is kept.

    move_model_singletons.py            # dry-run: report the plan
    move_model_singletons.py --write    # apply
Run from repo root (needs build/delink + build/gen)."""
import sys, re, glob, os
sys.path.insert(0, "scripts")
from collections import defaultdict
from homm2.build.assert_relocs import load_symbols, parse_obj, resolve, _cands

WRITE = '--write' in sys.argv
IMG = 0x400000

# 1. synthetic globals: rva -> (name, raw model line)
model = {}
for ln in open("include/_globals_model.h"):
    m = re.match(r'^DATA\(0x([0-9a-fA-F]+)\)\s+extern\s+(.*)$', ln.rstrip('\n'))
    if not m:
        continue
    nm = re.search(r'([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;', m.group(2).split('//')[0])
    if nm:
        model[int(m.group(1), 16) - IMG] = (nm.group(1), ln)

# 2. retail xref: which units' delinked objs reference each model VA (dup names -> all candidates)
sym, data, dups = load_symbols()
users = defaultdict(set)
for obj in glob.glob("build/delink/**/*.c.obj", recursive=True):
    unit = obj[len("build/delink/"):-len(".c.obj")]
    for _fn, relocs in parse_obj(obj).items():
        for r in relocs:
            vs = set()
            v = resolve(sym, data, *r)
            if v is not None:
                vs.add(v)
            if r[0] != 'REL32':
                vs |= _cands(dups, r[1], r[2])
            for v in vs:
                if v in model:
                    users[v].add(unit)

# 3. plan: single-module & not-unsized-array -> move; else keep
moves = defaultdict(list)          # unit -> [(name, static line)]
remove = set()                     # model rvas to delete
keep = []                          # (name, reason)
for rva, (name, ln) in model.items():
    us = users.get(rva, set())
    if len(us) != 1:
        keep.append((name, "multi-module" if len(us) > 1 else "no retail xref")); continue
    if re.search(r'\[\s*\]', ln):
        keep.append((name, "unsized array")); continue
    unit = next(iter(us))
    moves[unit].append((name, re.sub(r'\bextern\b', 'static', ln, count=1)))
    remove.add(rva)

def find_plain_def(lines, name):
    """index of an existing file-scope plain def `<type> name<dims>;` (no static/DATA), else None."""
    for i, l in enumerate(lines):
        if l[:1] in ' \t' or l.lstrip().startswith(('DATA(', 'static', '//')):
            continue
        code = l.split('//')[0]
        if '(' in code or '=' in code:
            continue
        if re.match(r'^[A-Za-z_][\w\s\*]*?[\s\*]' + re.escape(name) + r'\s*(\[[^\]]*\])*\s*;', code):
            return i
    return None

def has_static_def(lines, name):
    return any(re.match(r'^DATA\(0x[0-9a-fA-F]+\)\s+static\b.*\b' +
                        re.escape(name) + r'\s*(?:\[[^;]*\])*\s*;', line)
               for line in lines)

# 4. apply: convert an existing plain def in place, else insert before the first VA(); the module
#    keeps #include <_globals_model.h> so struct element types stay available.
for unit in sorted(moves):
    cpp = "src/%s.cpp" % unit
    if not os.path.exists(cpp):
        print("  MISSING .cpp: %s" % cpp); continue
    lines = open(cpp).read().splitlines(keepends=True)
    converted, inserted = 0, []
    for name, staticline in moves[unit]:
        if has_static_def(lines, name):
            continue
        idx = find_plain_def(lines, name)
        if idx is not None:
            lines[idx] = staticline; converted += 1
        else:
            inserted.append(staticline)
    if inserted:
        ins = next((i for i, l in enumerate(lines) if l.startswith("VA(")), len(lines))
        # Keep the first function's durable matcher checkpoint immediately above VA().
        while ins > 0 and (not lines[ins - 1].strip() or lines[ins - 1].lstrip().startswith("//")):
            ins -= 1
        block = ["\n// ---- module-private synthetic globals (retail xref: single-module) ----\n"] \
                + sorted(inserted) + ["\n"]
        lines = lines[:ins] + block + lines[ins:]
    print("  %2d -> %s (%d converted in place, %d inserted)" %
          (len(moves[unit]), cpp, converted, len(inserted)))
    if WRITE:
        open(cpp, "w").write("".join(lines))

if WRITE:
    out = []
    for ln in open("include/_globals_model.h"):
        m = re.match(r'^DATA\(0x([0-9a-fA-F]+)\)', ln)
        if m and (int(m.group(1), 16) - IMG) in remove:
            continue
        out.append(ln)
    open("include/_globals_model.h", "w").write("".join(out))

print("\n%s: move %d synthetic globals into %d modules; %d stay in _globals_model.h" %
      ("APPLIED" if WRITE else "DRY-RUN", len(remove), len(moves), len(keep)))
for n, why in sorted(keep):
    print("    keep %-24s %s" % (n, why))
