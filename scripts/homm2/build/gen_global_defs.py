#!/usr/bin/env python3
"""gen_global_defs.py — give every CodeView-owned global a DEFINITION in its owner .cpp, so the
project links (headers only DECLARE them: `DATA(VA) extern T g;`). The definition is plain
`T g;` (no DATA — the header carries the VA, so the RVA isn't repeated), emitted in retail-RVA
order in a data section at the owner .cpp's tail. Scalars/pointers/sized-arrays are the header
decl minus `extern`; unsized `T g[]` gets its outer dim from the CodeView byte size. Synthetic
globals (no CodeView symbol -> _globals_model.h) alias real storage and get no definition.
Idempotent: replaces the generated block. Run from repo root."""
import csv, re, os, glob
from collections import defaultdict

size_of, owner, rva_of = {}, {}, {}
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] != "data":
        continue
    m = re.match(r'\?([A-Za-z_]\w*)@@', r["name"]) or re.match(r'[_@]?([A-Za-z_]\w*)', r["name"])
    if m:
        n = m.group(1)
        size_of.setdefault(n, int(r["size"], 0)); owner.setdefault(n, r["unit"]); rva_of.setdefault(n, int(r["rva"], 0))

BASE = {'char':1,'signed char':1,'unsigned char':1,'short':2,'unsigned short':2,'int':4,
        'unsigned int':4,'unsigned':4,'long':4,'unsigned long':4,'float':4,'double':8}
# struct element sizes (from _types.h layouts)
STRUCT = {'tag_monsterInfo':26,'SSpellInfo':22,'SWinSetup':7,'tag_tilePoint':4,
          'SNetPlayerInfo':0xcc,'monsterRV':26,'configStruct':0x1a0,'SCreatureInfo':26}

def leaf_size(typ):
    t = typ.replace('class','').replace('struct','').strip()
    if '*' in t:
        return 4
    return BASE.get(t) or STRUCT.get(t)

def make_def(rest, name):
    """rest = 'TYPE name DIMS' (no extern/;). Return a valid definition or None if unresolved."""
    rest = rest.rstrip(';').strip()
    m = re.match(r'^(.*?)([A-Za-z_]\w*)\s*((?:\[[^\]]*\])*)$', rest)
    if not m:
        return rest + ";"                              # ptr-to-array / fn-ptr: the decl (sans extern) IS the def
    typ, nm, dims = m.group(1), m.group(2), m.group(3)
    if nm != name or '[]' not in dims:
        return rest + ";"                              # scalar/ptr/sized -> already a definition
    inner = re.findall(r'\[([^\]]*)\]', dims)
    prod = 1
    for d in inner[1:]:
        prod *= int(d, 0)
    ls = leaf_size(typ)
    if not ls or name not in size_of:
        return None
    outer = size_of[name] // (prod * ls)
    return "%s%s[%d]%s;" % (typ, nm, outer, "".join("[%s]" % d for d in inner[1:]))

BLOCK = re.compile(r'\n// ---- globals \(definitions, RVA order\) ----\n(?:.*\n)*?(?=\Z)', re.M)
by_cpp = defaultdict(list)
for h in glob.glob("include/**/*.h", recursive=True):
    if os.path.basename(h) == "_globals_model.h":
        continue                                       # synthetic aliases: no definition
    for line in open(h):
        m = re.match(r'^DATA\(0x[0-9a-fA-F]+\)\s+extern\s+(.*;)', line)
        if not m:
            continue
        nm = re.search(r'([A-Za-z_]\w*)\s*(\[[^;]*\])*\s*;', m.group(1))
        if not nm:
            continue
        name = nm.group(1)
        u = owner.get(name)
        if not u or u == "_const":
            continue
        d = make_def(m.group(1), name)
        if d:
            by_cpp["src/%s.cpp" % u].append((rva_of[name], d))

made = 0
for cpp, defs in sorted(by_cpp.items()):
    if not os.path.exists(cpp):
        print("  MISSING .cpp: %s" % cpp); continue
    defs.sort()
    block = "\n// ---- globals (definitions, RVA order) ----\n" + "\n".join(d for _, d in defs) + "\n"
    t = open(cpp).read()
    t = BLOCK.sub("", t).rstrip() + "\n" + block
    open(cpp, "w").write(t); made += len(defs)
    print("  %2d defs -> %s" % (len(defs), cpp))
print("emitted %d global definitions across %d TUs" % (made, len(by_cpp)))
