#!/usr/bin/env python3
"""own_header_pass.py — ensure every TU that defines free functions has an owner module header
declaring them, and that the .cpp #includes its own header. Generates a header only if missing
(never clobbers a hand-curated one), and inserts the self-include after `#include <va.h>`.
Run from repo root."""
import subprocess, os, re, glob, sys
sys.path.insert(0, "scripts")
from homm2.build.gen_module_header import free_decls   # reuse the extractor

made = 0; incl = 0
for cpp in sorted(glob.glob("src/**/*.cpp", recursive=True)):
    if not free_decls(cpp):
        continue                                        # pure member-fn TU: decls live in class headers
    tier = re.search(r'src/([A-Za-z]+)/', cpp).group(1)
    base = os.path.basename(cpp)[:-4]
    hdr = "include/%s/%s.h" % (tier, base)
    inc = "#include <%s/%s.h>" % (tier, base)
    if not os.path.exists(hdr):
        out = subprocess.run(["python3", "scripts/homm2/build/gen_module_header.py", cpp],
                             capture_output=True, text=True).stdout
        open(hdr, "w").write(out); made += 1
    t = open(cpp).read()
    if inc not in t:
        t = t.replace("#include <va.h>\n", "#include <va.h>\n%s\n" % inc, 1)
        open(cpp, "w").write(t); incl += 1
print("generated %d owner headers; added %d self-includes" % (made, incl))
