#!/usr/bin/env python3
# make_compilable.py - turn the declaration-only carcass into compilable empty stubs.
#
# Each src/<tier>/<unit>.cpp gets `#include <rva.h>` (so RVA()/DATA()/OVERRIDE expand
# to nothing under MSVC) and its bare function DECLARATIONS commented out - a member/
# free function declaration ending in ';' at namespace scope is the only thing that
# breaks the MSVC compile. RVA()/DATA() lines stay as live annotations + reference.
# A matcher reconstructs a function by uncommenting its declaration and turning it into
# a real definition with a body. Idempotent.
from pathlib import Path
REPO = next(p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists())

n = 0
for cpp in sorted((REPO / "src").rglob("*.cpp")):
    if "/_external/" in str(cpp):           # CRT/lib objs - not reconstruction units
        continue
    lines = cpp.read_text(errors="replace").splitlines()
    have_inc = any("rva.h" in l and "include" in l for l in lines)
    out, inserted = [], have_inc
    for l in lines:
        s = l.rstrip(); st = s.lstrip()
        if not inserted and st and not st.startswith("//"):
            out.append("#include <rva.h>"); inserted = True
        code = s.split("//", 1)[0].rstrip()   # drop any trailing // comment
        if st and not st.startswith("//") and not st.startswith("#") and code.endswith(";"):
            out.append("// " + s)           # comment the bare declaration
        else:
            out.append(s)
    if not inserted:
        out.append("#include <rva.h>")
    cpp.write_text("\n".join(out) + "\n")
    n += 1
print(f"make_compilable: processed {n} src files")
