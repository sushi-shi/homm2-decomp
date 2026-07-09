#!/usr/bin/env python3
"""permute.py - decomp-permuter-style hill-climber for the MSVC 4.2 /O2 register-alloc wall.

On /O2 TUs the match plateaus on register coloring / instruction scheduling that C source
cannot directly pin. This applies SEMANTICS-PRESERVING text mutations to the C++ source
(commutative-operand swaps, independent-line reorders, additive reassociation, decl splits),
recompiles each with the REAL MSVC 4.2 (the compiler - not a C parser - handles the C++),
scores the COFF with objdiff-cli, and keeps improvements. Every mutation is value-preserving
by construction, so a higher byte-score can never come from a semantically-wrong program.
Unlike the classic pycparser-based decomp-permuter, it never parses the language, so C++
(class / reinterpret_cast / templates) is fine.

ONLY useful on /O2 TUs - /Od functions have no regalloc wall (use scripts/od_slots.py).
Gains are incremental: a mutation helps only when it flips a key lea base/index or a
callee-saved coloring; the core residue (which reg holds a given value) is out of reach.

Usage (inside `nix develop .#build`, run from the repo/worktree root - HOMM2_DIR is pinned
at shell entry, so cd to the tree FIRST):
    python3 scripts/permute.py <src.cpp> <TU> <mangled-sym> [iters]
  e.g.
    python3 scripts/permute.py src/BASE/Icondf2b.cpp BASE/Icondf2b \\
        '?FlipDimIconToBitmap@@YIXPAVicon@@PAVbitmap@@HHHHHHHHH@Z' 400

Edits <src.cpp> IN PLACE, leaving the best-scoring variant (prints start/FINAL scores).
Inspect/commit the result yourself.
"""
import subprocess, re, sys, json, os, random

ROOT  = os.environ.get("HOMM2_DIR") or os.getcwd()
SRC   = sys.argv[1]                # e.g. src/BASE/Icondf2b.cpp
TU    = sys.argv[2]                # e.g. BASE/Icondf2b
SYM   = sys.argv[3]                # mangled symbol
ITERS = int(sys.argv[4]) if len(sys.argv) > 4 else 400
FLAGS = "/nologo /c /O2 /MT /Gr /G5 /QIfdiv".split()
OBJ   = f"build/objdiff/base/{TU}.obj"
TGT   = f"build/delink/{TU}.c.obj"
os.chdir(ROOT)


def score(text):
    open(SRC, "w").write(text)
    r = subprocess.run(["python3", "-m", "homm2.build.cc_wrap", "--out", OBJ, "--src", SRC, "--", *FLAGS],
                       capture_output=True, text=True)
    if r.returncode != 0:
        return -1.0
    r = subprocess.run(["objdiff-cli", "diff", "-1", TGT, "-2", OBJ, SYM, "-o", "-", "--format", "json"],
                       capture_output=True, text=True)
    try:
        d = json.loads(r.stdout)
    except Exception:
        return -1.0
    vals = []
    def walk(o):
        if isinstance(o, dict):
            for k, v in o.items():
                if k == "match_percent": vals.append(v)
                walk(v)
        elif isinstance(o, list):
            for x in o: walk(x)
    walk(d)
    return max(vals) if vals else -1.0


OPD = r'(?:0x[0-9a-fA-F]+|\d+|[A-Za-z_]\w*(?:\[[A-Za-z0-9_ +*-]+\]|->\w+|\.\w+)*)'
# commutative ops: swapping operands preserves the computed value
COMM = ['+', '*', '==', '!=', '&', '|', '^']
ASSIGN = re.compile(r'^(\s*)([A-Za-z_]\w*) = (.+);\s*$')


def idents(s):
    return set(re.findall(r'[A-Za-z_]\w*', s))


def gen_variants(text):
    out = []
    # 1) commutative-operand swaps
    for op in COMM:
        esc = re.escape(op)
        pat = re.compile(r'(?<![\w>&|^=!<+*-])(' + OPD + r') ' + esc + r' (' + OPD + r')(?![\w])')
        for m in pat.finditer(text):
            a, b = m.group(1), m.group(2)
            if a == b:
                continue
            out.append(text[:m.start()] + f"{b} {op} {a}" + text[m.end():])
    # 2) reorder adjacent independent scalar/global assignment lines (value-preserving)
    lines = text.split("\n")
    for i in range(len(lines) - 1):
        m1, m2 = ASSIGN.match(lines[i]), ASSIGN.match(lines[i + 1])
        if not (m1 and m2):
            continue
        # skip lines with side effects / memory writes / calls / increments
        if any(t in lines[i] + lines[i + 1] for t in ("*", "++", "--", "(", "[")):
            continue
        l1, r1 = m1.group(2), m1.group(3)
        l2, r2 = m2.group(2), m2.group(3)
        if l1 == l2:            # write-after-write to same lhs: order matters
            continue
        # independent iff neither lhs is used by the other statement
        if l1 in idents(r2) or l2 in idents(r1):
            continue
        nl = lines[:]
        nl[i], nl[i + 1] = lines[i + 1], lines[i]
        out.append("\n".join(nl))
    # 3) reassociation of 3-term additive chains  a + b + c  (int + is assoc mod 2^32)
    for m in re.finditer(r'(?<![\w>])(' + OPD + r') \+ (' + OPD + r') \+ (' + OPD + r')(?![\w])', text):
        a, b, c = m.groups()
        for perm in (f"{a} + {c} + {b}", f"{b} + {a} + {c}", f"{c} + {b} + {a}"):
            out.append(text[:m.start()] + perm + text[m.end():])
    # 4) declaration split: `TYPE V = E;` -> `TYPE V;` + `V = E;` (moves materialisation point)
    DECL = re.compile(r'^(\s*)((?:unsigned )?(?:int|short|char)\**|IconEntry \*) (\w+) = (.+);\s*$')
    for i, ln in enumerate(lines):
        m = DECL.match(ln)
        if not m or any(t in ln for t in ("*=", "new ", "[", "(")):
            continue
        ind, ty, v, e = m.groups()
        nl = lines[:i] + [f"{ind}{ty} {v};", f"{ind}{v} = {e};"] + lines[i + 1:]
        out.append("\n".join(nl))
    return out


def main():
    best = open(SRC).read()
    orig = best
    bscore = score(best)
    print(f"start {bscore:.3f}", flush=True)
    improved = True
    seen = set()
    rounds = 0
    while improved and rounds < 12:
        improved = False
        rounds += 1
        variants = gen_variants(best)
        random.shuffle(variants)
        for i, v in enumerate(variants):
            if v in seen:
                continue
            seen.add(v)
            s = score(v)
            if s > bscore + 1e-6:
                print(f"  round {rounds} site {i}: {bscore:.3f} -> {s:.3f}", flush=True)
                best, bscore = v, s
                improved = True
                break
    # phase 2: random multi-mutation walk to escape single-step local optima
    for attempt in range(ITERS):
        cur = best
        for _ in range(random.randint(2, 6)):
            vs = gen_variants(cur)
            if not vs:
                break
            cur = random.choice(vs)
        if cur in seen:
            continue
        seen.add(cur)
        s = score(cur)
        if s > bscore + 1e-6:
            print(f"  walk {attempt}: {bscore:.3f} -> {s:.3f}", flush=True)
            best, bscore = cur, s
            # greedy re-climb from the new best
            again = True
            while again:
                again = False
                for v in gen_variants(best):
                    if v in seen:
                        continue
                    seen.add(v)
                    sv = score(v)
                    if sv > bscore + 1e-6:
                        best, bscore = v, sv
                        print(f"    reclimb: -> {bscore:.3f}", flush=True)
                        again = True
                        break
    open(SRC, "w").write(best)
    score(best)
    print(f"FINAL {bscore:.3f} ({'improved' if best != orig else 'no change'})", flush=True)


if __name__ == "__main__":
    main()
