#!/usr/bin/env python3
"""Constrained /Od frame-layout solver over existing semantic names.

Input spec file (TSV): name<TAB>size<TAB>target_slot(hex, e.g. 0x14)
Rows with name "-" are dead locals to invent (candidates from --dead-pool).
The walk model: sort by (bucket asc, decl index DESC); slots assigned
shallow (-0x4) downward by size. The solver searches numeric-suffix
variants of each name (stem preserved) plus declaration order to hit the
target slots exactly. Prints the chosen names in declaration order.

Usage: od_layout_solve.py spec.tsv [--suffix-max 60] [--dead-pool a,b,c]
"""
import sys, itertools, argparse
sys.path.insert(0, __file__.rsplit('/', 1)[0])
from od_slots import bucket

def variants(name, nmax):
    stem = name.rstrip('0123456789')
    yield name
    if stem != name:
        yield stem
    for i in range(nmax):
        yield f"{stem}{i}"

def solve(spec, nmax, dead_pool):
    # spec: list of (name, size, target) shallow-first by target
    spec = sorted(spec, key=lambda r: r[2])
    order_target = [r[2] for r in spec]
    # candidate (name, bucket) per row
    cands = []
    for name, size, tgt in spec:
        pool = dead_pool if name == "-" else list(dict.fromkeys(variants(name, nmax)))
        cands.append([(n, bucket(n)) for n in pool])
    # walk constraint: shallow row i, deeper row i+1 must satisfy
    #   b_i < b_{i+1}  OR  (b_i == b_{i+1} and decl(i) AFTER decl(i+1))
    # With free decl order, any non-decreasing bucket sequence works:
    # ties resolved by declaring deeper-first within the tie group.
    best = None
    def rec(i, prev_b, acc, used):
        nonlocal best
        if best: return
        if i == len(cands):
            best = list(acc); return
        for n, b in cands[i]:
            if n in used or b < prev_b: continue
            acc.append((n, b)); used.add(n)
            rec(i + 1, b, acc, used)
            acc.pop(); used.discard(n)
            if best: return
    rec(0, -1, [], set())
    if not best:
        return None
    # declaration order: bucket groups ascending, within group DEEPEST first
    decl = []
    for b in sorted(set(b for _, b in best)):
        group = [(n, bb) for (n, bb) in best if bb == b]
        decl.extend(n for n, _ in reversed(group))
    return best, decl

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("spec")
    ap.add_argument("--suffix-max", type=int, default=60)
    ap.add_argument("--dead-pool", default="unused,cost,month,size,result,value,extra,spare")
    a = ap.parse_args()
    spec = []
    for ln in open(a.spec):
        ln = ln.strip()
        if not ln or ln.startswith('#'): continue
        name, size, tgt = ln.split('\t')
        spec.append((name, int(size, 0), int(tgt, 16)))
    r = solve(spec, a.suffix_max, a.dead_pool.split(','))
    if not r:
        print("NO SOLUTION with given pools; widen --suffix-max or pools"); return 1
    best, decl = r
    print("slot assignment (shallow->deep):")
    for (n, b), (_, sz, tgt) in zip(best, sorted(spec, key=lambda x: x[2])):
        print(f"  -{tgt:#x}  {n:24s} bucket={b:2d} size={sz}")
    print("declare in this order:")
    for n in decl: print(f"  {n}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
