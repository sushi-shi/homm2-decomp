#!/usr/bin/env python3
"""homm2.analysis.xref - who calls this function? (retail call/jmp-graph xrefs).

The delink/objdiff pipeline has no cross-reference DB, but for SEMANTIC navigation a
matcher wants the *callers* of a function (which method `new`s a ctor, which caller
attributes a leaf to its owning class/TU). This scans HEROES2W.EXE's .text for direct
`call`/`jmp rel32` (E8/E9) sites whose target is the queried RVA and reports each caller
resolved to its containing function + unit. It is the caller-side complement of
`sema disasm` (which shows a function's own body/callees).

Names are resolved best-first: build/gen/symbol_names.csv (the CodeView-authoritative
game functions + their units) -> build/ghidra/exports/functions.csv (Ghidra's whole-
.text boundary map, which NAMES nothing new but bounds the library/runtime functions
CodeView omits) -> a FUN_<rva> fallback.

Ghidra's functions.csv is OPTIONAL but recommended: without it, a caller sitting in a
library/runtime function (not in CodeView) is reported as `(unrecovered fn @ ~0x..)`
because we don't know that function's boundary. With it, every caller site lands inside
a known boundary. Generate it once with `homm2 ghidra`.

Unlike gruntz's GRUNTZ.EXE, HEROES2W.EXE is NOT incrementally linked - there is no ILT
jump-thunk table in front of .text (it starts at RVA 0x1000 with real code), so the
thunk-chasing machinery gruntz needs is absent here.

Attribution is SIZE-BOUNDED: a call site is credited to a function only if it lies within
[start, start+size). A site in the unrecovered GAP after a function's body is reported as
`(unrecovered fn @ ~0x..)`, never misattributed to the previous function.

Usage (reads files + $HOMM2_EXE only):
    python3 -m homm2.analysis.xref 0x00069120 0x0006e0f0   # callers of each
    python3 -m homm2.analysis.xref ?SmackManagerMain@@YIXXZ # by (mangled) name
    python3 -m homm2.analysis.xref --callees 0x00069120     # forward: its call targets
    python3 -m homm2.analysis.xref --raw 0x00069120         # every call site (no dedup)
    python3 -m homm2.analysis.xref --tree 0x00069120        # caller ancestry (depth 4)
    python3 -m homm2.analysis.xref --tree --depth 0 0x...    # unlimited (can be huge)
"""
import os, sys, struct, csv, bisect
from pathlib import Path

REPO = next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
            Path(__file__).resolve().parents[3])
EXE = Path(os.environ.get("HOMM2_EXE") or REPO / "build/orig/HEROES2W.EXE")
SYMCSV = REPO / "build/gen/symbol_names.csv"
FUNCS = REPO / "build/ghidra/exports/functions.csv"
IMAGEBASE = 0x400000


def _load():
    d = EXE.read_bytes()
    e = struct.unpack_from("<I", d, 0x3c)[0]
    nsec = struct.unpack_from("<H", d, e + 6)[0]
    optsz = struct.unpack_from("<H", d, e + 20)[0]
    opt = e + 24
    secs = []
    for i in range(nsec):
        o = opt + optsz + i * 40
        name = d[o:o + 8].rstrip(b"\0").decode("latin1")
        vsz, va, rsz, rp = struct.unpack_from("<IIII", d, o + 8)
        secs.append((name, va, vsz, rp, rsz))
    return d, secs


def _text(secs):
    return next(s for s in secs if s[0] == ".text")


def _psize(x):
    """Parse a size cell -> int bytes or None. symbol_names is hex (0x2e), Ghidra
    functions.csv is decimal (46); int(_, 0) reads both. 0/blank -> None (unknown)."""
    x = str(x).strip()
    if not x:
        return None
    try:
        return int(x, 0) or None
    except ValueError:
        return None


# name maps: rva -> (name, unit). symbol_names first (CodeView game funcs), then Ghidra.
# fstarts/fsize: recovered function starts + their byte sizes. symbol_names sizes are
# authoritative; functions.csv fills the library/runtime remainder. _owner() bounds
# attribution by size so a call in an unrecovered GAP is reported, not misattributed.
def _names():
    names, fsize, starts, byname = {}, {}, set(), {}
    if SYMCSV.exists():
        with open(SYMCSV) as f:
            for r in csv.DictReader(f):
                try:
                    rva = int(r["rva"], 16)
                except Exception:
                    continue
                if (r.get("kind") or "func") != "func":
                    continue  # data/vtable rows aren't fn starts
                names[rva] = (r["name"], r.get("unit", ""))
                starts.add(rva)
                sz = _psize(r.get("size", ""))
                if sz:
                    fsize[rva] = sz
    if FUNCS.exists():
        with open(FUNCS) as f:
            for r in csv.DictReader(f):
                try:
                    rva = int(r["entry_rva"], 16)
                except Exception:
                    continue
                starts.add(rva)
                if rva not in fsize:  # symbol_names size wins (CodeView authority)
                    sz = _psize(r.get("byte_size", ""))
                    if sz:
                        fsize[rva] = sz
                names.setdefault(rva, (r["name"], "ghidra"))
                byname.setdefault(r["name"], rva)
    for rva, (nm, _u) in names.items():
        byname.setdefault(nm, rva)
    return names, byname, sorted(starts), fsize


def _gap_start(site, fstarts, fsize):
    """Likely START of the uncarved function containing `site`: the end of the nearest
    preceding carved function, or that function's start if its size is unknown. Uncarved
    caller sites are de-duped by this gap start."""
    k = bisect.bisect_right(fstarts, site) - 1
    if k < 0:
        return site
    st = fstarts[k]
    sz = fsize.get(st)
    return st + sz if sz else st


def _owner(rva, fstarts, fsize):
    """The recovered function CONTAINING `rva`, or None if `rva` is in an unrecovered
    gap. Nearest start <= rva, bounded by that fn's known size. Size unknown -> unbounded."""
    k = bisect.bisect_right(fstarts, rva) - 1
    if k < 0:
        return None
    start = fstarts[k]
    sz = fsize.get(start)
    if sz and rva >= start + sz:
        return None
    return start


def _resolve(arg, byname):
    try:
        return int(arg, 16)
    except ValueError:
        if arg in byname:
            return byname[arg]
        sys.exit(f"[xref] '{arg}' not an RVA and not found in symbol_names/functions.csv")


def data_refs(target, d, secs, names):
    """Data words (in non-.text sections) whose value is the VA of `target`: the fn-ptr
    tables / vtable slots / command tables that hold it indirectly - references a rel32
    .text scan can't see. Returns [(data_rva, section)]. (No ILT thunks in HEROES2W, so
    the referenced value is always the direct body VA.)"""
    want = target + IMAGEBASE
    hits = []
    for (name, va, vsz, rp, rsz) in secs:
        if name == ".text":
            continue
        blob = d[rp:rp + rsz]
        for i in range(0, len(blob) - 3, 4):
            if struct.unpack_from("<I", blob, i)[0] == want:
                hits.append((va + i, name))
    return hits


def _print_data_refs(target, d, secs, names, indent="  "):
    """Print the data-side references of `target` (compact; capped)."""
    drefs = data_refs(target, d, secs, names)
    if not drefs:
        return
    print(f"{indent}-- referenced as data (fn-ptr table / vtable slot / command table):")
    for draddr, sec in drefs[:16]:
        owner = ""
        for o in sorted((a for a in names if a <= draddr), reverse=True)[:1]:
            if draddr - o < 0x400:
                owner = f"  ~{names[o][0]}+0x{draddr - o:x}"
        print(f"{indent}   @0x{draddr:08x} [{sec}]{owner}")
    if len(drefs) > 16:
        print(f"{indent}   ... (+{len(drefs) - 16} more)")


def callers_of(targets, d, secs, names, fstarts, fsize, raw=False):
    tname, tva, tvsz, trp, trsz = _text(secs)
    tb = d[trp:trp + trsz]
    tset = set(targets)
    found = {t: [] for t in targets}
    n = len(tb) - 4  # -4: an E8/E9 in the last 5 bytes still counts
    i = 0
    while i < n:
        op = tb[i]
        if op == 0xE8 or op == 0xE9:
            rel = struct.unpack_from("<i", tb, i + 1)[0]
            src = tva + i
            tgt = src + 5 + rel
            if tgt in tset:
                found[tgt].append((src, op))
        i += 1

    for t in targets:
        tn = names.get(t, (f"FUN_{t:x}", "?"))[0]
        print(f"\n==== callers of 0x{t:08x}  {tn} ====")
        if not found[t]:
            print("  (no direct call/jmp rel32 caller in .text)")
            _print_data_refs(t, d, secs, names)
            continue
        seen = set()
        for src, op in found[t]:
            o = _owner(src, fstarts, fsize)
            kind = "call" if op == 0xE8 else "jmp "
            if o is None:  # site sits in an unrecovered gap - roll it up to the gap start
                gap = _gap_start(src, fstarts, fsize)
                if raw:
                    print(f"  {kind} @0x{src:08x}  in (unrecovered fn ~0x{gap:08x})")
                elif ("gap", gap) not in seen:
                    seen.add(("gap", gap))
                    print(f"  {kind} in (unrecovered fn @ ~0x{gap:08x})")
                continue
            nm, unit = names.get(o, (f"FUN_{o:x}", "?"))
            if raw:
                print(f"  {kind} @0x{src:08x}  in 0x{o:08x} {nm} [{unit}]")
            elif o not in seen:
                seen.add(o)
                print(f"  {kind} in 0x{o:08x} {nm} [{unit}]")
        _print_data_refs(t, d, secs, names)


def _is_known(rva, names):
    """A CodeView-named game function (a real symbol_names unit), vs a Ghidra-only /
    FUN_ library body. The tree expands THROUGH known game functions and STOPS on
    library functions - the attribution frontier."""
    nm, unit = names.get(rva, (f"FUN_{rva:x}", "?"))
    return unit not in ("", "?", "ghidra") and not nm.startswith("FUN_")


def caller_tree(targets, d, secs, names, fstarts, fsize, depth_cap=0):
    """Recursive caller ancestry. Expansion continues through KNOWN game functions
    (CodeView-named) and stops at each library function (Ghidra-only FUN_ - the frontier),
    as well as at roots and unrecovered gaps. Dedup: an already-expanded function prints
    as (*seen). depth_cap=0 means unlimited; the default is 4."""
    tname, tva, tvsz, trp, trsz = _text(secs)
    tb = d[trp:trp + trsz]
    idx = {}  # callee entry-rva -> [(site, op)] over the WHOLE .text, one scan
    n = len(tb) - 4
    i = 0
    while i < n:
        op = tb[i]
        if op == 0xE8 or op == 0xE9:
            rel = struct.unpack_from("<i", tb, i + 1)[0]
            tgt = tva + i + 5 + rel
            if tva <= tgt < tva + tvsz:
                idx.setdefault(tgt, []).append((tva + i, op))
        i += 1

    def label(rva):
        nm, unit = names.get(rva, (f"FUN_{rva:x}", "?"))
        return f"0x{rva:08x} {nm} [{unit}]"

    def effective_callers(rva):
        for site, op in idx.get(rva, []):
            o = _owner(site, fstarts, fsize)
            if o == rva:
                continue  # intra-fn / self
            yield (o, op, site)

    for t in targets:
        print(f"\n==== caller tree of {label(t)} ====")
        seen = set()

        def walk(rva, depth):
            if depth_cap and depth > depth_cap:
                print("  " * (depth + 1) + "... (--depth cap)")
                return
            owners, uniq = [], set()
            for o, op, site in effective_callers(rva):
                key = (o, op) if o is not None else ("gap", _gap_start(site, fstarts, fsize))
                if key in uniq:
                    continue
                uniq.add(key)
                owners.append((o, op, site))
            if not owners:
                if depth == 0:
                    print("  (no direct call/jmp rel32 caller in .text)")
                return
            for o, op, site in owners:
                kind = "call" if op == 0xE8 else "jmp "
                pad = "  " * (depth + 1)
                if o is None:  # a genuine unrecovered gap - a real leaf
                    gap = _gap_start(site, fstarts, fsize)
                    print(pad + f"<- {kind} (unrecovered fn @ ~0x{gap:08x})")
                    continue
                if o in seen:
                    print(pad + f"<- {kind} {label(o)} (*seen)")
                    continue
                seen.add(o)
                if _is_known(o, names):
                    print(pad + f"<- {kind} {label(o)}")
                    walk(o, depth + 1)  # go THROUGH known game callers
                else:
                    print(pad + f"<- {kind} {label(o)}  [library - frontier]")
        walk(t, 0)
        _print_data_refs(t, d, secs, names)


def callees_of(targets, d, secs, names, fstarts, fsize):
    tname, tva, tvsz, trp, trsz = _text(secs)
    for t in targets:
        k = bisect.bisect_right(fstarts, t) - 1
        start = fstarts[k] if k >= 0 and fstarts[k] == t else t
        sz = fsize.get(start)
        if sz:
            end = start + sz
        else:
            j = bisect.bisect_right(fstarts, start)
            end = fstarts[j] if j < len(fstarts) else start + 0x400
        tn = names.get(t, (f"FUN_{t:x}", "?"))[0]
        print(f"\n==== callees of 0x{t:08x}  {tn}  (span 0x{start:x}..0x{end:x}) ====")
        b = d[trp + (start - tva):trp + (end - tva)]
        seen = set()
        indirect = 0
        for i in range(max(0, len(b) - 4)):
            if b[i] in (0xE8, 0xE9):
                rel = struct.unpack_from("<i", b, i + 1)[0]
                tgt = start + i + 5 + rel
                if tva <= tgt < tva + tvsz and tgt not in seen:
                    seen.add(tgt)
                    nm, unit = names.get(tgt, (f"FUN_{tgt:x}", "?"))
                    print(f"  -> 0x{tgt:08x} {nm} [{unit}]")
            elif b[i] == 0xFF and i + 1 < len(b) and ((b[i + 1] >> 3) & 7) in (2, 4):
                indirect += 1  # call/jmp r/m32 - vtable/IAT/fn-ptr dispatch
        if not seen:
            print("  (no direct call/jmp rel32 callee)")
        if indirect:
            print(f"  (~{indirect} indirect call/jmp site(s) - vtable/IAT/fn-ptr; "
                  "invisible to rel32 xref)")


def main():
    args = sys.argv[1:]
    mode = "callers"
    raw = False
    depth = 4  # --depth 0 = unlimited
    rest = []
    it = iter(args)
    for a in it:
        if a == "--callees":
            mode = "callees"
        elif a == "--raw":
            raw = True
        elif a == "--tree":
            mode = "tree"
        elif a == "--depth":
            depth = int(next(it, "4"))
        else:
            rest.append(a)
    if not rest:
        sys.exit(__doc__)
    if not EXE.is_file():
        sys.exit(f"[xref] target EXE not found: {EXE} (set $HOMM2_EXE)")
    d, secs = _load()
    names, byname, fstarts, fsize = _names()
    targets = [_resolve(a, byname) for a in rest]
    if mode == "callees":
        callees_of(targets, d, secs, names, fstarts, fsize)
    elif mode == "tree":
        caller_tree(targets, d, secs, names, fstarts, fsize, depth_cap=depth)
    else:
        callers_of(targets, d, secs, names, fstarts, fsize, raw=raw)


if __name__ == "__main__":
    main()
