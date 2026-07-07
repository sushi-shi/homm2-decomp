#!/usr/bin/env python3
"""gen_module_header.py — bootstrap a TU's module header from the FREE-function definitions
in its .cpp, so those functions are declared in exactly ONE place (their owner's header) and
callers just #include it (no drifting local externs). Emits declarations for free functions
only (members live in class headers; ctors/dtors excluded). Run from repo root:
  gen_module_header.py src/BASE/Misc.cpp include/BASE/Misc.h
Prints the decls to stdout (review) — does not overwrite an existing header unless --write."""
import sys, re, os

VA_RE = re.compile(r'^VA\(0x[0-9a-fA-F]+,')
HEAD_RE = re.compile(r'\)(\s*const)?$')          # a function head ends with ')' or ') const'

def free_decls(path):
    lines = open(path).read().split('\n')
    out = []
    for i in range(len(lines) - 1):
        if not VA_RE.match(lines[i]):
            continue
        line = lines[i + 1]
        if '(' not in line:
            continue
        brace = line.find('{')
        sig = (line[:brace] if brace >= 0 else line).strip()   # body may be on next line
        if '(' not in sig or not HEAD_RE.search(sig):
            continue
        namepart = sig[:sig.index('(')]
        # free functions only: members (Class::name) live in class headers; skip ctor/dtor
        if '::' in namepart or '~' in namepart:
            continue
        out.append(sig + ';')
    return out

if __name__ == "__main__":
    cpp = sys.argv[1]
    decls = free_decls(cpp)
    guard = "HOMM2_" + re.sub(r'\W', '_', os.path.basename(cpp)[:-4]).upper() + "_H"
    print("#ifndef %s" % guard)
    print("#define %s" % guard)
    print("// Declarations of the free functions DEFINED in %s — the single home for these" % os.path.basename(cpp))
    print("// symbols. Other TUs call them by including this header (no local externs).")
    print('#include <_all.h>')
    print()
    for d in decls:
        print(d)
    print()
    print("#endif // %s" % guard)
