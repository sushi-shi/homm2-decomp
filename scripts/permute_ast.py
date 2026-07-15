#!/usr/bin/env python3
"""permute_ast.py - libclang (clang.cindex) permutation-search hill-climber.

Candidate source mutations are compiled with real MSVC 4.2, scored with objdiff, and retained
only when they improve the target without regressing siblings. The mutator walks the real clang
AST instead of regexes. Because libclang gives the exact source RANGE of each operand sub-expression (it
knows `b*c` and `(x+y)` are single operands, and knows precedence/parens), swapping two
operand ranges is CORRECT BY CONSTRUCTION - the precedence-crossing false-match class that
the regex tool needed a conservative guard for simply cannot occur here.

Transforms: commutative-operand swap (+ * == != & | ^), relational-operand swap with operator
flip, and conservative independent-statement reorder using AST read/write sets. AST ranges make
the edits syntactically reliable, but every retained mutation still needs a semantic audit.
In particular, inequality +/-1 rewrites are intentionally excluded: they are not equivalent for
floating-point operands and can differ for integral operands at overflow boundaries.

Parses with the exact flags clangd uses (build/clangd/compile_commands.json, translated to
clang-driver form). Operates on BYTES (libclang offsets are byte offsets; sources contain
multi-byte UTF-8 in comments). Usage (inside `nix develop .#build`, cd to the tree first):
    python3 scripts/permute_ast.py <src.cpp> <TU> <mangled-sym> [iters]
Edits <src.cpp> IN PLACE, leaving the best-scoring variant.
"""
import subprocess, re, sys, json, os, random, glob
import clang.cindex as ci

ROOT  = os.environ.get("HOMM2_DIR") or os.getcwd()
SRC   = sys.argv[1]
TU    = sys.argv[2]
SYM   = sys.argv[3]
ITERS = int(sys.argv[4]) if len(sys.argv) > 4 else 300
OBJ   = f"build/objdiff/base/{TU}.obj"
TGT   = f"build/delink/{TU}.c.obj"
os.chdir(ROOT)

_lib = glob.glob("/nix/store/*clang*-lib/lib/libclang.so")
if _lib:
    try: ci.Config.set_library_file(_lib[0])
    except Exception: pass


def _load_flags(tu):
    toml = open("config/units.toml").read()
    profiles = {m.group(1): re.findall(r'"([^"]+)"', m.group(2))
                for m in re.finditer(r'^(base|o2|base_oi) = \[([^\]]+)\]', toml, re.M)}
    unit_flags = {}
    for blk in toml.split("[[unit]]"):
        um = re.search(r'unit = "([^"]+)"', blk); fm = re.search(r'flags = "(\w+)"', blk)
        if um and fm: unit_flags[um.group(1)] = fm.group(1)
    return profiles.get(unit_flags.get(tu, "o2"), "/nologo /c /O2 /MT /Gr /G5 /QIfdiv".split())
FLAGS = _load_flags(TU)


def _clang_args():
    """clangd's parse flags for SRC, translated from clang-cl to clang-driver form."""
    try:
        db = json.load(open("build/clangd/compile_commands.json"))
    except Exception:
        db = []
    raw = next((e.get("arguments", []) for e in db if SRC in e.get("file", "")), [])
    out = ["-x", "c++", "-std=c++14", "-fms-compatibility", "-ferror-limit=0"]
    i = 0
    while i < len(raw):
        a = raw[i]
        if a == "/imsvc": out += ["-isystem", raw[i + 1]]; i += 2; continue
        if a == "/I":     out += ["-I", raw[i + 1]]; i += 2; continue
        if a.startswith("/D"): out.append("-D" + a[2:]); i += 1; continue
        if a.startswith(("--target=", "-fms", "-fdelayed")): out.append(a); i += 1; continue
        i += 1
    return out
_ARGS = _clang_args()
_INDEX = ci.Index.create()


# ---- objdiff scoring -------------------------------------------------------------
def _symmap(d):
    return {s["name"]: s["match_percent"] for s in d.get("right", {}).get("symbols", [])
            if isinstance(s.get("name"), str) and s.get("match_percent") is not None}


def score_full(text):
    open(SRC, "w").write(text)
    r = subprocess.run(["python3", "-m", "homm2.build.cc_wrap", "--out", OBJ, "--src", SRC, "--", *FLAGS],
                       capture_output=True, text=True)
    if r.returncode != 0:
        return -1.0, {}
    r = subprocess.run(["objdiff-cli", "diff", "-1", TGT, "-2", OBJ, SYM, "-o", "-", "--format", "json"],
                       capture_output=True, text=True)
    try:
        m = _symmap(json.loads(r.stdout))
    except Exception:
        return -1.0, {}
    return (m[SYM], m) if SYM in m else (-1.0, m)


_BASE_SIBS = {}


def score(text):
    tgt, sibs = score_full(text)
    if tgt < 0:
        return tgt
    for name, base in _BASE_SIBS.items():
        if name != SYM and sibs.get(name) is not None and sibs[name] < base - 1e-4:
            return -1.0
    return tgt


# ---- AST mutation (the whole point) ----------------------------------------------
COMM = {"+", "*", "==", "!=", "&", "|", "^"}
RELFLIP = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}


def _parse(text):
    return _INDEX.parse(SRC, args=_ARGS, unsaved_files=[(SRC, text)],
                        options=ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)


def _target_fn(tu, blob):
    """The FunctionDecl/method definition whose extent sits in the target's VA-marker span."""
    va = None
    for line in open("build/gen/symbol_names.csv"):
        c = line.rstrip("\n").split(",")
        if len(c) >= 2 and c[1] == SYM and c[0].startswith("0x"):
            va = f"VA(0x{int(c[0], 16) + 0x400000:08x},".encode(); break
    if va is None:
        return None
    i = blob.find(va)
    if i < 0:
        return None
    j = blob.find(b"\nVA(0x", i + 1)
    lo, hi = i, (j if j >= 0 else len(blob))
    best = None
    KIND = (ci.CursorKind.FUNCTION_DECL, ci.CursorKind.CXX_METHOD,
            ci.CursorKind.CONSTRUCTOR, ci.CursorKind.DESTRUCTOR)
    for c in tu.cursor.walk_preorder():
        if c.kind in KIND and c.is_definition() and c.extent.start.file \
                and str(c.extent.start.file) == SRC:
            s = c.extent.start.offset
            if lo <= s < hi:
                best = c
    return best


def _range(node):
    return node.extent.start.offset, node.extent.end.offset


def gen_variants(text):
    blob = text.encode("utf-8")
    tu = _parse(text)
    fn = _target_fn(tu, blob)
    if fn is None:
        return []
    out = []

    def swap(a, b):                       # a before b: value-preserving text-range swap
        (a0, a1), (b0, b1) = a, b
        if not (a0 < a1 <= b0 < b1):
            return
        v = blob[:a0] + blob[b0:b1] + blob[a1:b0] + blob[a0:a1] + blob[b1:]
        out.append(v.decode("utf-8"))

    def splice(lo, hi, new):
        out.append((blob[:lo] + new.encode() + blob[hi:]).decode("utf-8"))

    binops, compounds, decls = [], [], []
    for c in fn.walk_preorder():
        if c.kind == ci.CursorKind.BINARY_OPERATOR:
            binops.append(c)
        elif c.kind == ci.CursorKind.COMPOUND_STMT:
            compounds.append(c)
        elif c.kind == ci.CursorKind.DECL_STMT:
            decls.append(c)

    for node in binops:
        kids = list(node.get_children())
        if len(kids) != 2:
            continue
        (l0, l1), (r0, r1) = _range(kids[0]), _range(kids[1])
        if not (l0 < l1 <= r0 < r1):
            continue
        op = blob[l1:r0].decode("utf-8", "ignore").strip()
        ltxt, rtxt = blob[l0:l1].decode(), blob[r0:r1].decode()
        if op in COMM and ltxt != rtxt:
            swap((l0, l1), (r0, r1))                         # a OP b -> b OP a (always valid)
        elif op in RELFLIP:
            splice(l0, r1, f"{rtxt} {RELFLIP[op]} {ltxt}")   # a < b -> b > a

    # independent adjacent-statement reorder (real read/write sets from the AST)
    def rw(stmt):
        writes, reads, sideff = set(), set(), False
        ch = list(stmt.get_children())
        # assignment `lhs = rhs` or compound-assign
        if stmt.kind == ci.CursorKind.BINARY_OPERATOR and len(ch) == 2:
            op = blob[ch[0].extent.end.offset:ch[1].extent.start.offset].decode("utf-8", "ignore").strip()
            if op == "=" and ch[0].kind in (ci.CursorKind.DECL_REF_EXPR, ci.CursorKind.MEMBER_REF_EXPR):
                writes.add(ch[0].spelling or blob[slice(*_range(ch[0]))].decode())
        for d in stmt.walk_preorder():
            if d.kind == ci.CursorKind.CALL_EXPR:
                sideff = True
            if d.kind == ci.CursorKind.UNARY_OPERATOR and any(t.spelling in ("++", "--", "*") for t in d.get_tokens()):
                sideff = True
            if d.kind in (ci.CursorKind.DECL_REF_EXPR, ci.CursorKind.MEMBER_REF_EXPR):
                reads.add(d.spelling)
        return writes, reads, sideff

    for comp in compounds:
        stmts = [c for c in comp.get_children()]
        for k in range(len(stmts) - 1):
            s1, s2 = stmts[k], stmts[k + 1]
            if s1.kind != ci.CursorKind.BINARY_OPERATOR or s2.kind != ci.CursorKind.BINARY_OPERATOR:
                continue
            w1, r1, sf1 = rw(s1); w2, r2, sf2 = rw(s2)
            if sf1 or sf2 or not w1 or not w2:
                continue
            if w1 & r2 or w2 & r1 or w1 & w2:            # RAW / WAR / WAW -> order matters
                continue
            (a0, a1), (b0, b1) = _range(s1), _range(s2)
            v = blob[:a0] + blob[b0:b1] + blob[a1:b0] + blob[a0:a1] + blob[b1:]
            out.append(v.decode("utf-8"))

    return out


def main():
    global _BASE_SIBS
    best = open(SRC).read()
    orig = best
    t0, sibs0 = score_full(best)
    _BASE_SIBS = dict(sibs0)
    bscore = t0
    print(f"start {bscore:.3f} (target {SYM}); {len(_BASE_SIBS)} symbols pinned; "
          f"{len(gen_variants(best))} variants", flush=True)
    seen = set()
    improved = True
    rounds = 0
    while improved and rounds < 12:
        improved = False; rounds += 1
        vs = gen_variants(best); random.shuffle(vs)
        for v in vs:
            if v in seen: continue
            seen.add(v)
            s = score(v)
            if s > bscore + 1e-6:
                print(f"  round {rounds}: {bscore:.3f} -> {s:.3f}", flush=True)
                best, bscore = v, s; improved = True; break
    for attempt in range(ITERS):
        cur = best
        for _ in range(random.randint(2, 5)):
            vs = gen_variants(cur)
            if not vs: break
            cur = random.choice(vs)
        if cur in seen: continue
        seen.add(cur)
        s = score(cur)
        if s > bscore + 1e-6:
            print(f"  walk {attempt}: {bscore:.3f} -> {s:.3f}", flush=True)
            best, bscore = cur, s
    open(SRC, "w").write(best)
    score(best)
    print(f"FINAL {bscore:.3f} ({'improved' if best != orig else 'no change'})", flush=True)


if __name__ == "__main__":
    main()
