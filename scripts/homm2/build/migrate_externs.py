#!/usr/bin/env python3
"""migrate_externs.py — route a TU's local `extern` declarations to the right shared header
and rewrite the includes, enforcing the ownership rule. For each `extern ...;` line:
  * Win32 dllimport            -> <win/windows.h>
  * extern "C" _crtfn          -> <io.h>   (CRT low-level I/O)
  * a FUNCTION (has '(')        -> its owning unit's module header <TIER/Unit.h> (per CodeView)
  * a VARIABLE                  -> <_globals.h>
Removes the extern lines and inserts the needed #includes after `#include <va.h>`. Prints
the owner headers it needs (generate any missing with gen_module_header.py). Struct/class
local defs are NOT touched (handle per-TU). Run from repo root: migrate_externs.py <cpp>"""
import sys, re, csv, os

IMG = 0x400000
unit_of = {}; globals_set = set()
for r in csv.DictReader(open("build/gen/symbol_names.csv")):
    dem = r["name"]
    m = re.match(r'\?([A-Za-z_]\w*)@@', dem) or re.match(r'[_@]([A-Za-z_]\w*)', dem)
    if not m:
        continue
    nm = m.group(1)
    if r["kind"] == "func":
        unit_of.setdefault(nm, r["unit"])
    else:
        globals_set.add(nm)

def ident(decl):
    m = re.search(r'([A-Za-z_]\w*)\s*(\(|\[|;|$)', decl[decl.rfind('*') + 1:] if '*' in decl else decl)
    # last identifier before ( / [ / ;
    ids = re.findall(r'[A-Za-z_]\w*', decl.split('(')[0].split('[')[0])
    return ids[-1] if ids else None

CRT = {'memset': '<string.h>', 'memcpy': '<string.h>', 'memmove': '<string.h>',
       'strlen': '<string.h>', 'strcpy': '<string.h>', 'strcat': '<string.h>',
       'strcmp': '<string.h>', 'strncmp': '<string.h>', 'strncpy': '<string.h>',
       '_open': '<io.h>', '_close': '<io.h>', '_read': '<io.h>', '_write': '<io.h>',
       '_lseek': '<io.h>', 'malloc': '<stdlib.h>', 'free': '<stdlib.h>',
       'rand': '<stdlib.h>', 'srand': '<stdlib.h>', 'sprintf': '<stdio.h>'}

def route(decl):
    if '__declspec(dllimport)' in decl:
        return '<win/windows.h>'
    nm = ident(decl)
    if '(' in decl:                              # a function
        if nm in CRT:
            return CRT[nm]
        if nm in unit_of:
            return '<%s.h>' % unit_of[nm]
        if re.search(r'extern "C".*\b_[a-z]', decl):
            return '<io.h>'
    return '<_globals.h>'                          # variable (or unknown -> globals staging)

def main(path):
    lines = open(path).read().split('\n')
    out = []; needed = set(); removed = 0
    for ln in lines:
        if re.match(r'^\s*extern\b', ln) and '{' not in ln and ln.rstrip().endswith(';'):
            needed.add(route(ln)); removed += 1
            continue
        out.append(ln)
    text = '\n'.join(out)
    inc = ''.join('#include %s\n' % h for h in sorted(needed))
    text = text.replace('#include <va.h>\n', '#include <va.h>\n' + inc, 1)
    open(path, 'w').write(text)
    print("%s: removed %d externs; added includes: %s" % (path, removed, ', '.join(sorted(needed))))

if __name__ == "__main__":
    main(sys.argv[1])
