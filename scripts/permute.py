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

Auto-detects the TU's build profile (base=/Od, o2=/O2, base_oi=/Od+/Oi) from
config/units.toml, so it works on BOTH tiers:
  - /O2: nudges register allocation / scheduling; gains are usually incremental (the
    optimizer resists source steering, and the core register-coloring residue - which
    reg holds a given value - is out of reach) and stay below byte-identical.
  - /Od: lowering is literal, so the order mutations map DIRECTLY to codegen; the
    commutative-operand-swap targets the documented /Od operand-load-order residual, and
    completions here are TRUE byte-identical 100%. It will NOT fix a /Od stack-slot miss
    (that's scripts/od_slots.py) or a control-flow-shape mismatch.

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
OBJ   = f"build/objdiff/base/{TU}.obj"
TGT   = f"build/delink/{TU}.c.obj"
os.chdir(ROOT)


def _load_flags(tu):
    """Compile flags for a TU = its config/units.toml profile (base=/Od, o2=/O2, base_oi)."""
    toml = open("config/units.toml").read()
    profiles = {m.group(1): re.findall(r'"([^"]+)"', m.group(2))
                for m in re.finditer(r'^(base|o2|base_oi) = \[([^\]]+)\]', toml, re.M)}
    unit_flags = {}
    for blk in toml.split("[[unit]]"):
        um = re.search(r'unit = "([^"]+)"', blk)
        fm = re.search(r'flags = "(\w+)"', blk)
        if um and fm:
            unit_flags[um.group(1)] = fm.group(1)
    prof = unit_flags.get(tu, "o2")
    return profiles.get(prof, "/nologo /c /O2 /MT /Gr /G5 /QIfdiv".split())


FLAGS = _load_flags(TU)


def _symmap(d):
    """{symbol_name: match_percent} for every named function symbol on the right (our build)."""
    res = {}
    for sym in d.get("right", {}).get("symbols", []):
        n, mp = sym.get("name"), sym.get("match_percent")
        if isinstance(n, str) and mp is not None:
            res[n] = mp
    return res


def score_full(text):
    """(target match_percent, {sym: match_percent}) or (-1.0, {}) on failure.
    objdiff-cli IGNORES its <symbol> arg for JSON output, so we key the target by SYM name
    ourselves — taking the global max would saturate at 100 on any already-matched sibling."""
    open(SRC, "w").write(text)
    r = subprocess.run(["python3", "-m", "homm2.build.cc_wrap", "--out", OBJ, "--src", SRC, "--", *FLAGS],
                       capture_output=True, text=True)
    if r.returncode != 0:
        return -1.0, {}
    r = subprocess.run(["objdiff-cli", "diff", "-1", TGT, "-2", OBJ, SYM, "-o", "-", "--format", "json"],
                       capture_output=True, text=True)
    try:
        d = json.loads(r.stdout)
    except Exception:
        return -1.0, {}
    m = _symmap(d)
    return (m[SYM], m) if SYM in m else (-1.0, m)


_BASE_SIBS = {}   # sibling baseline (from the original source); no sibling may drop below these


def score(text):
    """Target symbol match_percent, or -1 if the compile failed OR any sibling regressed."""
    tgt, sibs = score_full(text)
    if tgt < 0:
        return tgt
    for name, base_mp in _BASE_SIBS.items():
        if name != SYM and sibs.get(name) is not None and sibs[name] < base_mp - 1e-4:
            return -1.0
    return tgt


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
    global _BASE_SIBS
    best = open(SRC).read()
    orig = best
    t0, sibs0 = score_full(best)          # baseline: target % + all sibling %s (before any mutation)
    _BASE_SIBS = dict(sibs0)
    bscore = t0
    print(f"start {bscore:.3f} (target {SYM}); {len(_BASE_SIBS)} symbols pinned", flush=True)
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
    _, sibs_final = score_full(best)
    dump = os.environ.get("PERMUTE_DUMP")   # optional before/after sibling maps for external re-check
    if dump:
        json.dump({"sym": SYM, "changed": best != orig, "target_before": t0,
                   "target_after": bscore, "before": sibs0, "after": sibs_final},
                  open(dump, "w"), indent=0)
    print(f"FINAL {bscore:.3f} ({'improved' if best != orig else 'no change'})", flush=True)


if __name__ == "__main__":
    main()
