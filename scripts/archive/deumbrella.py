#!/usr/bin/env python3
"""deumbrella.py — remove the <_all.h> umbrella so each TU/header pulls only what it uses.
Step 1 (headers): every header that #includes <_all.h> gets it replaced by forward declarations
  of the class/struct types it REFERENCES (function-decl params etc. are pointers/refs, so a
  fwd-decl suffices; the compiler flags any that truly need the full type).
Step 2 (.cpp): strip the <_all.h> include (the compile-driven resolver adds specific class
  headers afterwards).
Codegen-neutral: used types stay fully defined via their specific headers.
Run from repo root: deumbrella.py headers | deumbrella.py cpp"""
import re, glob, os, sys

# class/struct NAME -> defining header (scan real definitions, not fwd-decls)
DEF = re.compile(r'^\s*(class|struct)\s+(\w+)\s*(:|\{)')
def strip_c(s): return re.sub(r'/\*.*?\*/', '', s)   # a class def may be `class X /* abstract */ {`
kind_of = {}                              # class/struct NAME -> 'class'|'struct' (from its definition)
for h in glob.glob("include/**/*.h", recursive=True):
    for line in open(h):
        m = DEF.match(strip_c(line))
        if m:
            kind_of.setdefault(m.group(2), m.group(1))

MARK = "// forward declarations (was <_all.h>):\n"
BLOCK_RE = re.compile(r'// forward declarations \(was <_all\.h>\):\n(?:(?:class|struct) \w+;\n)*')

def declify_header(h):
    text = open(h).read()
    if '_all.h' not in text and MARK not in text:
        return False
    defined = {m.group(2) for line in text.splitlines() if (m := DEF.match(strip_c(line)))}
    body = BLOCK_RE.sub("", text)         # ignore any prior fwd block when scanning for refs
    body = re.sub(r'#include [<"]_all\.h[>"]\n', "", body)
    words = set(re.findall(r'\b([A-Za-z_]\w*)\b', body))
    refs = sorted(nm for nm in words if nm in kind_of and nm not in defined)
    fwds = "".join("%s %s;\n" % (kind_of[nm], nm) for nm in refs)
    block = MARK + fwds
    if MARK in text:
        text = BLOCK_RE.sub(block, text, count=1)
    else:
        text = re.sub(r'#include [<"]_all\.h[>"]\n', block, text, count=1)
    open(h, "w").write(text)
    return True

if __name__ == "__main__":
    what = sys.argv[1] if len(sys.argv) > 1 else "headers"
    if what == "headers":
        n = sum(declify_header(h) for h in glob.glob("include/**/*.h", recursive=True))
        print("forward-declified %d headers (removed their <_all.h>)" % n)
    elif what == "cpp":
        n = 0
        for c in glob.glob("src/**/*.cpp", recursive=True):
            t = open(c).read()
            t2 = re.sub(r'#include <_all\.h>\n', '', t, count=1)
            if t2 != t:
                open(c, "w").write(t2); n += 1
        print("stripped <_all.h> from %d .cpp (resolver adds specific headers next)" % n)
