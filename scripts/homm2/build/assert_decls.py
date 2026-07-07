#!/usr/bin/env python3
"""assert_decls.py — hard build gate: no TU may carry its OWN type definitions or extern
declarations. Every class/struct/extern a .cpp relies on must come from a shared header, so
two TUs can't drift by declaring the same entity differently. Run from repo root; exits 1
(with a report) if any .cpp declares locally. Allowed in a .cpp: function DEFINITIONS
(including `extern "C" T f(...) { ... }` and `extern "C" { ...definitions... }` blocks) and
`#include`s. Forbidden: `class/struct X { ... }` / `class X;` / `extern ... ;` declarations."""
import re, sys, glob, os

# a real type definition or forward decl living in a .cpp
TYPE_RE = re.compile(r'^\s*(class|struct)\s+\w+\s*(\{|:|;)')
# an extern DECLARATION (statement ending in ';' with no body) — not a linkage block/def
EXTERN_DECL_RE = re.compile(r'^\s*extern\b')
# a FILE-SCOPE function forward-declaration (no 'extern' keyword) — must live in a header too.
# Column-0 (bodies/statements are indented), `<ret> name(params);`, no body, no '=' (that's a
# definition/initializer), and not an annotation macro or control keyword.
FWD_DECL_RE = re.compile(r'^[A-Za-z_][\w\s\*&:<>]*\b[A-Za-z_]\w*\s*\([^{;=]*\)\s*(const\s*)?;')
_FWD_SKIP = re.compile(r'^\s*(VA|VAU|DATA|SIZE|SYMBOL|return|typedef|extern)\b')

def violations(path):
    out = []
    for i, line in enumerate(open(path), 1):
        s = line.rstrip('\n')
        code = s.split('//')[0].rstrip()              # ignore trailing // comment
        if TYPE_RE.match(s):
            out.append((i, s.strip()))
        elif EXTERN_DECL_RE.match(s):
            if '{' not in code and code.endswith(';'):
                out.append((i, s.strip()))
        elif code and code[0] not in ' \t}' and not _FWD_SKIP.match(code) \
                and code.endswith(';') and FWD_DECL_RE.match(code):
            out.append((i, s.strip()))                 # local function forward-declaration
    return out

if __name__ == "__main__":
    files = sys.argv[1:] or sorted(glob.glob("src/**/*.cpp", recursive=True))
    total = 0; bad_files = 0
    for f in files:
        v = violations(f)
        if v:
            bad_files += 1; total += len(v)
            print("%s: %d local declaration(s)" % (f, len(v)))
            for ln, txt in v[:12]:
                print("    %d: %s" % (ln, txt[:90]))
    if total:
        print("\nHEADER-DISCIPLINE FAIL: %d local declarations in %d file(s) — move them to headers."
              % (total, bad_files))
        sys.exit(1)
    print("header-discipline OK: no local type/extern declarations in any TU.")
