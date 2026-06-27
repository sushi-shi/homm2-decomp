#!/usr/bin/env python3
"""MSVC 4.2 /Od local stack-slot predictor, name catalog & layout solver.

Reverse-engineered from the toolchain (C1XX.EXE identifier lexer FUN_0041d1a2 and
C2.EXE per-scope symbol table FUN_0044bb5e/FUN_0044c4df). Verified byte-exact
against the retail compiler.

How /Od assigns local frame offsets:

  lexer:   unsigned h = 0;
           for each identifier char c:  h = (h >> 4) + h*4 + c;      # 32-bit
           key = (h ^ (h >> 16)) & 0xffff;                           # stored in name record
  backend: bucket = key & 0xF;          # 16-bucket per-scope table, LIFO chains
           frame layout walks bucket 0..15, each chain newest-first.

  => slot order = sort locals by (bucket ascending, declaration index DESCENDING).

Only the *name* picks the bucket; only declaration order breaks ties *within* a
bucket. To reproduce a retail layout you need names whose buckets are
non-decreasing down the stack (slot -0x4 .. deepest); this module computes that.
"""
import sys

# ---------------------------------------------------------------- the hash -----
def ident_hash(name: str) -> int:
    h = 0
    for c in name:
        h = ((h >> 4) + h * 4 + ord(c)) & 0xFFFFFFFF
    return h


def key16(name: str) -> int:
    h = ident_hash(name)
    return (h ^ (h >> 16)) & 0xFFFF


def bucket(name: str) -> int:
    """Symbol-table bucket 0..15 — picks the local's /Od frame-slot group."""
    return key16(name) & 0xF


def slot_order(names):
    """names in DECLARATION order -> names in FRAME-SLOT order (-0x4 first)."""
    idx = {n: i for i, n in enumerate(names)}
    return sorted(names, key=lambda n: (bucket(n), -idx[n]))


def predict_offsets(names, sizes=None):
    """names (decl order) -> {name: ebp offset}. sizes: optional {name: bytes}
    (default 4). Order is exact for any types; offsets need real sizes."""
    order = slot_order(names)
    off, out = 0, {}
    for n in order:
        off -= (sizes.get(n, 4) if sizes else 4)
        out[n] = off
    return out


# ----------------------------------------------------- the name catalog --------
# Readable identifiers grouped by the role a decomp would use them for, each list
# in preference order (best first). Used to pick layout-compatible names that are
# still meaningful. Extend freely.
ROLES = {
    "counter": ["i", "j", "k", "n", "m", "cnt", "idx", "nb", "ii", "jj", "iter", "count"],
    "index":   ["idx", "i", "j", "index", "pos", "at", "ix", "k"],
    "ptr":     ["ptr", "p", "cur", "node", "elem", "it", "q", "pp", "head", "obj"],
    "buf":     ["buf", "data", "block", "mem", "tmp", "buffer", "dst", "src"],
    "len":     ["len", "n", "cnt", "size", "sz", "num", "count", "nb"],
    "count":   ["cnt", "count", "num", "n", "total", "nb", "k"],
    "temp":    ["tmp", "t", "scratch", "tmp1", "tmp2", "tt", "tmpv"],
    "result":  ["res", "ret", "result", "out", "rv", "r", "ok"],
    "value":   ["val", "v", "value", "num", "data", "x"],
    "size":    ["sz", "size", "len", "cap", "n", "num"],
    "str":     ["str", "s", "name", "text", "buf", "cp"],
    "char":    ["ch", "c", "ck", "cc"],
    "flag":    ["flag", "ok", "done", "bit", "b", "f"],
    "sum":     ["sum", "acc", "total", "s", "tot"],
    "obj":     ["obj", "o", "self", "that", "item", "p"],
    "x": ["x"], "y": ["y"], "z": ["z"],
}

# Flat catalog = every catalog name, binned by bucket.
CATALOG = sorted({n for lst in ROLES.values() for n in lst} |
                 set("abcdefghijklmnopqrstuvwxyz") |
                 {a + b for a in "ijknpmtr" for b in "xyznabc"})


def by_bucket(names=CATALOG):
    """{bucket 0..15: [names...]} for any name list (default the catalog)."""
    out = {b: [] for b in range(16)}
    for n in names:
        out[bucket(n)].append(n)
    return out


BUCKETS = by_bucket()


def _distinct_name(b, used, prefer=()):
    """A name in bucket b not in `used` (prefer `prefer`, then catalog, then
    generate). Always succeeds — each bucket has unboundedly many short names."""
    import itertools, string
    for n in list(prefer) + BUCKETS[b]:
        if n not in used and bucket(n) == b:
            return n
    al = string.ascii_lowercase
    for L in (2, 3, 4):
        for combo in itertools.product(al, repeat=L):
            n = "".join(combo)
            if n not in used and bucket(n) == b:
                return n
    return None


# --------------------------------------------------------- the layout solver ---
def _candidates(slot):
    """Normalize a slot spec -> ([(name, bucket, rank)], preferred_set).
    slot may be a role name in ROLES, a bare name (pinned), or a list of names.
    Role/explicit names come first (preferred); then one catalog name per
    otherwise-uncovered bucket, so *any* target order is always feasible (those
    extras are flagged as fallbacks if chosen)."""
    if isinstance(slot, str):
        pref = ROLES[slot] if slot in ROLES else [slot]
    else:
        pref = list(slot)
    pref = list(dict.fromkeys(pref))
    names, seen, have = list(pref), set(pref), {bucket(n) for n in pref}
    for b in range(16):
        if b not in have:
            cn = _distinct_name(b, seen)
            if cn:
                names.append(cn); seen.add(cn)
    return [(nm, bucket(nm), r) for r, nm in enumerate(names)], set(pref)


def solve_layout(slots):
    """slots: list in STACK order (slot -0x4 first); each entry a role/name/list of
    candidate names (preference order). Returns the cheapest assignment whose
    buckets are non-decreasing down the stack (== reproduces this exact order).

    -> {ok, names (stack order), decl_order, buckets, fallbacks} or {ok:False,...}.
    'fallbacks' lists slots that couldn't use their 1st-choice name."""
    parsed = [_candidates(s) for s in slots]
    cand = [c for c, _ in parsed]
    pref = [p for _, p in parsed]
    if any(not c for c in cand):
        return {"ok": False, "error": "a slot has no candidate names"}
    N, INF = len(cand), float("inf")
    # choice[i][b] = (min_total_rank, name, prev_bucket)
    choice = [dict() for _ in range(N)]
    for nm, bk, rk in cand[0]:
        if rk < choice[0].get(bk, (INF,))[0]:
            choice[0][bk] = (rk, nm, None)
    for i in range(1, N):
        # prefix-min over previous buckets <= b
        pm, run = [], (INF, None)
        for b in range(16):
            if b in choice[i - 1] and choice[i - 1][b][0] < run[0]:
                run = (choice[i - 1][b][0], b)
            pm.append(run)
        for nm, bk, rk in cand[i]:
            base_cost, base_b = pm[bk]
            if base_cost < INF and base_cost + rk < choice[i].get(bk, (INF,))[0]:
                choice[i][bk] = (base_cost + rk, nm, base_b)
    if not choice[-1]:
        return {"ok": False, "error": "infeasible: candidate buckets can't be "
                "made non-decreasing down the stack — add more candidate names"}
    b = min(choice[-1], key=lambda bb: choice[-1][bb][0])
    names = [None] * N
    for i in range(N - 1, -1, -1):
        _, names[i], b = choice[i][b]
    # enforce distinct names (duplicates can only occur within one bucket; swap in
    # another name of the same bucket so the order is preserved)
    used = set()
    for i in range(N):
        if names[i] in used:
            bk = bucket(names[i])
            prefer = [nm for nm, cb, _ in cand[i] if cb == bk]
            rep = _distinct_name(bk, used, prefer)
            if rep is None:
                return {"ok": False, "error": f"bucket {bk} ran out of distinct names"}
            names[i] = rep
        used.add(names[i])
    fb = [i for i in range(N) if names[i] not in pref[i]]
    decl = [names[i] for i in sorted(range(N), key=lambda i: (bucket(names[i]), -i))]
    return {"ok": True, "names": names, "decl_order": decl,
            "buckets": [bucket(n) for n in names], "fallbacks": fb}


def solve(n_slots):
    """Canonical guaranteed solution: single chars hitting buckets 0..n-1."""
    return [chr(0x70 | (k & 0xF)) for k in range(n_slots)]


# -------------------------------------------------------------------- CLI ------
def _main(argv):
    if not argv:
        print(__doc__); return 0
    cmd, rest = argv[0], argv[1:]
    if cmd == "order":
        for i, n in enumerate(slot_order(rest)):
            print(f"  -0x{4*(i+1):x}  {n}")
    elif cmd == "bucket":
        for n in rest:
            print(f"  {n:16s} H=0x{ident_hash(n):08x} key=0x{key16(n):04x} bucket={bucket(n):2d}")
    elif cmd == "buckets":
        for b in range(16):
            print(f"  bucket {b:2d}: {' '.join(BUCKETS[b])}")
    elif cmd == "layout":           # od_slots.py layout counter ptr index len ...
        r = solve_layout(rest)
        if not r["ok"]:
            print("  " + r["error"]); return 1
        for i, n in enumerate(r["names"]):
            tag = "  (fallback)" if i in r["fallbacks"] else ""
            print(f"  -0x{4*(i+1):x}  {n:10s} bucket={r['buckets'][i]:2d}  [{rest[i]}]{tag}")
        print("  declare in this order:", " ".join(r["decl_order"]))
    elif cmd == "solve":
        print(" ".join(solve(int(rest[0]))))
    else:
        print(__doc__); return 1
    return 0


if __name__ == "__main__":
    sys.exit(_main(sys.argv[1:]))
