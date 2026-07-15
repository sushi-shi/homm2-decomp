#!/usr/bin/env python3
"""carcass_fill.py — turn `// signature;` placeholder comments into compilable stub
definitions so every symbol is PRESENT in the object file (the "carcass" phase; real
byte-matching is a later step). SURGICAL & SAFE: only rewrites a `// <sig>;` comment that
sits *immediately* after a `VA(...)` line and contains `(` (a real demangled signature).
Already-reconstructed bodies, `@early-stop` blocks (their reason line precedes the VA, and
their post-VA line is real code), DATA()/vtable comments, and bare-name extern-C stubs
(no `(`) are left byte-for-byte untouched. Run from repo root. Usage: carcass_fill.py FILE...

Stub bodies are DISPOSABLE: void->{}, pointer/scalar->return 0, else->return *(T*)0.
Deleting destructors (??_E/??_G, demangled scalar_dtor/vector_dtor) are compiler-GENERATED
from a single `Class::~Class(){}` — so per class we emit exactly one dtor definition and
leave the other dtor-family placeholders as comments (their symbols come free)."""
import sys, re, csv, os

IMG = 0x400000
sym = {}
with open("build/gen/symbol_names.csv") as f:
    for row in csv.DictReader(f):
        sym.setdefault(int(row["rva"], 16), row["name"])

VA_RE = re.compile(r'^VA\(0x([0-9a-fA-F]+),\s*0x[0-9a-fA-F]+\)\s*$')
PH_RE = re.compile(r'^//\s*(.+?);')          # placeholder sig, up to the first ';'
SCALAR = re.compile(r'\b(void|char|short|int|long|float|double|bool|unsigned|signed|__int64|wchar_t)\b')
DTOR_KINDS = {'~destructor', 'scalar_dtor', 'vector_dtor', 'vbase_dtor'}

def conv_kw(mangled):
    if not mangled or not mangled.startswith("?"):
        return ""
    m = re.search(r'@@Y([A-Z])', mangled)
    return {"A": "__cdecl ", "G": "__stdcall ", "C": "__pascal "}.get(m.group(1) if m else "", "")

def body_for(ret):
    r = ret.strip()
    if r == "" or r == "void":
        return "{}"
    if r.endswith("*") or SCALAR.search(r):
        return "{ return 0; }"
    return "{ return *(%s *)0; }" % r.rstrip(' &')   # class/enum by value (needs complete type)

DROP = "__DROP__"   # sentinel: remove this VA()+comment block entirely (no source function)

def make_def(sig, va_hex):
    """Return a definition string, DROP to delete the block, or None to leave it a comment."""
    mangled = sym.get(int(va_hex, 16) - IMG, "")
    # `??_*` are COMPILER-GENERATED helpers with NO source function: deleting destructors
    # (??_E/??_G, demangled scalar_dtor/vector_dtor), vector ctor/dtor iterators (??_H/??_I),
    # etc. They are emitted from the class's real dtor + vtable, not written by hand — so drop
    # the placeholder rather than inventing a def or leaving a bare VA().
    if mangled.startswith("??_"):
        return DROP
    par = sig.find('(')
    if par < 0:
        return None
    head = sig[:par].rstrip()
    rest = sig[par:].strip()                 # "(params)" possibly with trailing " const"
    sp = head.rfind(' ')
    ret = head[:sp].strip() if sp >= 0 else ""
    name = head[sp + 1:] if sp >= 0 else head
    ret = re.sub(r'^(static|virtual|inline)\s+', '', ret)   # out-of-line defs drop these
    ret = re.sub(r'^(static|virtual|inline)\s+', '', ret)
    comps = name.split('::')
    last = comps[-1]
    if last == 'constructor':
        cls = comps[-2]; qual = '::'.join(comps[:-1])
        return "%s::%s%s {}" % (qual, cls, rest)
    if last in DTOR_KINDS:                    # only ~destructor (??1, the real base dtor) reaches here
        cls = comps[-2]; qual = '::'.join(comps[:-1])
        return "%s::~%s() {}" % (qual, cls)
    kw = conv_kw(mangled) if '::' not in name else ""
    retpart = (ret + ' ') if ret else ''
    return "%s%s%s%s %s" % (retpart, kw, name, rest, body_for(ret))

def transform(path):
    lines = open(path).read().split('\n')
    out = []; i = 0; filled = 0; bare = 0; dropped = 0
    while i < len(lines):
        line = lines[i]
        m = VA_RE.match(line)
        if m and i + 1 < len(lines):
            pm = PH_RE.match(lines[i + 1])
            if pm and '(' in pm.group(1):
                d = make_def(pm.group(1), m.group(1))
                if d == DROP:                    # compiler-generated ??_* : remove the whole block
                    i += 2
                    if i < len(lines) and lines[i].strip() == '':
                        i += 1                   # and its trailing blank line
                    dropped += 1; continue
                if d:
                    out.append(line); out.append(d)
                    filled += 1; i += 2; continue
            elif pm:
                bare += 1
        out.append(line); i += 1
    text = '\n'.join(out)
    # Type includes are intentionally not added here. Reconstructed TUs own their explicit
    # module/class includes; adding the removed <_all.h> umbrella would perturb TU state.
    return text, filled, bare, dropped

if __name__ == "__main__":
    for path in sys.argv[1:]:
        text, filled, bare, dropped = transform(path)
        if filled or dropped:
            open(path, "w").write(text)
        note = (" (%d bare-name left for hand)" % bare) if bare else ""
        note += (" (%d compiler-gen dropped)" % dropped) if dropped else ""
        print("%-32s filled %3d%s" % (os.path.basename(path), filled, note))
