#!/usr/bin/env python3
"""resolve_includes.py — after <_all.h> is stripped, add each .cpp the SPECIFIC class headers
it actually needs, driven by the compiler. Loops: build (-k 0), parse 'undefined type / not a
class / undeclared' errors, map the named type -> its defining header, insert the #include,
repeat until clean or no progress. Run INSIDE the build shell (calls ninja directly)."""
import subprocess, re, os, glob, csv
from collections import defaultdict

DEF = re.compile(r'^\s*(class|struct)\s+(\w+)\s*(:|\{)')
def strip_c(s): return re.sub(r'/\*.*?\*/', '', s)   # `class X /* abstract */ {`
name2hdr = {}
for h in glob.glob("include/**/*.h", recursive=True):
    rel = os.path.relpath(h, "include")
    for line in open(h):
        m = DEF.match(strip_c(line))
        if m:
            name2hdr.setdefault(m.group(2), rel)
# GLOBAL identifiers -> their owner TU's header (CodeView), so an undefined-global error
# (C2065) resolves to the owning header just like an undefined type does. Loaded after the
# class map (setdefault) so a class never gets shadowed by a same-named global.
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    if r["kind"] != "data":
        continue
    gm = re.match(r'\?([A-Za-z_]\w*)@@', r["name"]) or re.match(r'[_@]?([A-Za-z_]\w*)', r["name"])
    if gm:
        u = r["unit"]
        name2hdr.setdefault(gm.group(1), "_globals_model.h" if u in (None, "_const") else "%s.h" % u)

cpp_of = {os.path.basename(c)[:-4]: c for c in glob.glob("src/**/*.cpp", recursive=True)}
# type named by either a quote-first error ('X' : is not a class / undeclared) or C2027
ERR = re.compile(r"([A-Za-z0-9_]+)\.cpp\(\d+\) : error C\d+: "
                 r"(?:use of undefined type '(\w+)'|'(\w+)' : (?:is not a|undeclared))")

for it in range(12):
    log = subprocess.run("ninja -k 0 -f build.ninja 2>&1 || true", shell=True,
                         capture_output=True, text=True).stdout
    fails = log.count("FAILED:")
    need = defaultdict(set)
    for m in ERR.finditer(log):
        base, typ = m.group(1), (m.group(2) or m.group(3))
        if typ in name2hdr and base in cpp_of:
            need[cpp_of[base]].add(name2hdr[typ])
    if not need:
        print("iter %d: %d failed TU(s); no more resolvable includes" % (it, fails)); break
    added = 0
    for cpp, hdrs in need.items():
        t = open(cpp).read()
        ins = "".join("#include <%s>\n" % h for h in sorted(hdrs) if "#include <%s>" % h not in t)
        if ins:
            t = t.replace("#include <va.h>\n", "#include <va.h>\n" + ins, 1)
            open(cpp, "w").write(t); added += ins.count("#include")
    print("iter %d: %d failed TU(s); added %d include(s)" % (it, fails, added))
    if not added:
        break
