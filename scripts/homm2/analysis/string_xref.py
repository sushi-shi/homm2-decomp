#!/usr/bin/env python3
"""homm2.analysis.string_xref - which string literals does a function reference?

Mechanically recovers, per function, the .rdata/.data string literals it directly loads
(a 4-byte LE immediate in .text equal to a string start - a `push offset str` /
`mov reg, offset str` operand). Two directions:

    --rva 0x0000126d      the string set of a function (its literal evidence)
    --find TEXT           reverse: every function referencing a string containing TEXT

Function boundaries + names come from build/gen/symbol_names.csv (CodeView-authoritative);
Ghidra's build/ghidra/exports/functions.csv, if present, adds the library/runtime funcs.

Usage (reads $HOMM2_EXE + the CSVs only):
    python3 -m homm2.analysis.string_xref --rva 0x0000126d
    python3 -m homm2.analysis.string_xref --find "too many bits"
"""
import bisect, csv, os, re, struct, sys
from pathlib import Path

REPO = Path(os.environ.get("HOMM2_DIR")) if os.environ.get("HOMM2_DIR") else \
    next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
         Path(__file__).resolve().parents[3])
EXE = Path(os.environ.get("HOMM2_EXE") or REPO / "build/orig/HEROES2W.EXE")
SYMCSV = REPO / "build/gen/symbol_names.csv"
FUNCS = REPO / "build/ghidra/exports/functions.csv"
IMAGE_BASE = 0x400000


def _funcs():
    """(sorted starts, {rva:name}, {rva:size}) from symbol_names.csv (+ optional Ghidra)."""
    fname, fsize = {}, {}
    if SYMCSV.is_file():
        for r in csv.DictReader(SYMCSV.open()):
            if (r.get("kind") or "func") != "func":
                continue
            try:
                rva = int(r["rva"], 16)
            except ValueError:
                continue
            fname[rva] = r["name"]
            try:
                fsize[rva] = int(r["size"], 0)
            except (ValueError, KeyError):
                pass
    if FUNCS.is_file():
        for r in csv.DictReader(FUNCS.open()):
            try:
                rva = int(r["entry_rva"], 16)
            except (ValueError, KeyError):
                continue
            fname.setdefault(rva, r["name"])
            if rva not in fsize:
                try:
                    fsize[rva] = int(r["byte_size"])
                except (ValueError, KeyError):
                    pass
    return sorted(fname), fname, fsize


def load():
    data = EXE.read_bytes()
    pe = struct.unpack_from("<I", data, 0x3c)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    sec_off = pe + 24 + opt_size
    secs = []
    for i in range(nsec):
        o = sec_off + i * 40
        name = data[o:o + 8].rstrip(b"\0").decode("latin1")
        vsize, vaddr, rawsz, rawp = struct.unpack_from("<IIII", data, o + 8)
        secs.append((name, vaddr, vsize, rawp, rawsz))
    text = next(s for s in secs if s[0] == ".text")
    tbytes = data[text[3]:text[3] + text[4]]
    trva = text[1]
    # strings (start VA -> text) from non-.text sections
    str_at = {}
    pat = re.compile(rb"[\x20-\x7e]{4,}")
    for name, vaddr, vsize, rawp, rawsz in secs:
        if name == ".text":
            continue
        blob = data[rawp:rawp + rawsz]
        for m in pat.finditer(blob):
            str_at[IMAGE_BASE + vaddr + m.start()] = m.group().decode("latin1")

    starts, fname, fsize = _funcs()

    def func_of(rva):
        i = bisect.bisect_right(starts, rva) - 1
        if i < 0:
            return None
        st = starts[i]
        sz = fsize.get(st)
        return st if (sz is None or rva < st + sz) else None

    func_strs = {}
    if str_at:
        lo, hi = min(str_at), max(str_at)
        n = len(tbytes)
        for o in range(n - 3):
            v = tbytes[o] | (tbytes[o + 1] << 8) | (tbytes[o + 2] << 16) | (tbytes[o + 3] << 24)
            if v < lo or v > hi:
                continue
            s = str_at.get(v)
            if s is None:
                continue
            fn = func_of(trva + o)
            if fn is not None:
                func_strs.setdefault(fn, set()).add(s)
    return func_strs, fname, fsize


def main():
    args = sys.argv[1:]
    if not EXE.is_file():
        sys.exit(f"[string_xref] target EXE not found: {EXE} (set $HOMM2_EXE)")
    func_strs, fname, fsize = load()

    if "--rva" in args:
        for tok in args[args.index("--rva") + 1:]:
            if tok.startswith("--"):
                break
            rva = int(tok, 16)
            print(f"### 0x{rva:08x} {fname.get(rva, '?')} sz={fsize.get(rva, 0):#x}")
            hits = sorted(func_strs.get(rva, []))
            if not hits:
                print("   (no directly-referenced .rdata/.data string literal)")
            for s in hits:
                print("   " + repr(s))
        return

    if "--find" in args:                       # reverse: literal -> referencing fns
        needle = args[args.index("--find") + 1].lower()
        any_hit = False
        for rva in sorted(func_strs):
            hits = [s for s in func_strs[rva] if needle in s.lower()]
            if hits:
                any_hit = True
                print(f"### 0x{rva:08x} {fname.get(rva, '?')}")
                for s in sorted(hits):
                    print("   " + repr(s))
        if not any_hit:
            print(f"(no function references a string containing {needle!r})")
        return

    sys.exit(__doc__)


if __name__ == "__main__":
    main()
