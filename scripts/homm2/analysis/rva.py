#!/usr/bin/env python3
"""homm2.analysis.rva - one-shot address dossier.

Joins the pure lookups a matcher wants when they land on an address: the CodeView src
claim (name/unit/kind), where it is bodied in src/ (the VA(...) macro line), the Ghidra
boundary (if `homm2 ghidra` has run), and its objdiff fuzzy %. No analysis - just joins.

Accepts an RVA (0x0004a3c0) or a full VA (0x004a43c0, as written in the src VA() macros);
a value >= the image base is treated as a full VA and reduced to its RVA.

Usage:
    python3 -m homm2.analysis.rva 0x0004a3c0
"""
import csv, json, os, re, sys
from pathlib import Path

REPO = Path(os.environ.get("HOMM2_DIR")) if os.environ.get("HOMM2_DIR") else \
    next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
         Path(__file__).resolve().parents[3])
SYMCSV = REPO / "build/gen/symbol_names.csv"
GHIDRA_FUNCS = REPO / "build/ghidra/exports/functions.csv"
REPORT = REPO / "build/objdiff/report.json"
IMAGEBASE = 0x400000


def _csv_find(path: Path, val: int, key: str):
    if not path.is_file():
        return None
    for r in csv.DictReader(path.open()):
        try:
            if int(r[key], 16) == val:
                return r
        except (ValueError, KeyError):
            pass
    return None


def _src_loc_of(rva: int):
    """(relpath, lineno) of the VA(0x..)/DATA(0x..) macro defining the fn at `rva`,
    scanning src/ + include/. The macros carry the FULL VA (image base + rva), padding-
    agnostic so 0x004a43c0 and 0x4a43c0 both match."""
    full = rva + IMAGEBASE
    pat = re.compile(r"\b(?:VA|DATA)\s*\(\s*0x0*%x\s*[,)]" % full, re.IGNORECASE)
    for sub in ("src", "include"):
        base = REPO / sub
        if not base.is_dir():
            continue
        for f in sorted(base.rglob("*")):
            if f.suffix not in (".c", ".cpp", ".cc", ".cxx", ".h", ".hpp"):
                continue
            try:
                for i, line in enumerate(f.read_text(errors="ignore").splitlines(), 1):
                    if pat.search(line):
                        return (f.relative_to(REPO).as_posix(), i)
            except OSError:
                continue
    return None


def _fuzzy(name: str, unit: str):
    if not REPORT.is_file():
        return None
    rep = json.loads(REPORT.read_text())
    for u in rep.get("units", []):
        if u.get("name") != unit:
            continue
        for fn in u.get("functions") or []:
            if fn.get("name") == name:
                return fn.get("fuzzy_match_percent")
    return None


def main():
    args = sys.argv[1:]
    if not args:
        sys.exit(__doc__)
    try:
        n = int(args[0], 16)
    except ValueError:
        sys.exit(f"[rva] '{args[0]}' is not a hex address (e.g. 0x0004a3c0)")
    rva = n - IMAGEBASE if n >= IMAGEBASE else n
    print(f"RVA 0x{rva:08x}  (VA 0x{rva + IMAGEBASE:08x})")

    claim = _csv_find(SYMCSV, rva, "rva")
    if claim:
        print(f"  src claim : {claim['name']}  [{claim.get('unit', '?')}] "
              f"({claim.get('kind', '?')})")
        loc = _src_loc_of(rva)
        if loc:
            print(f"  src loc   : {loc[0]}:{loc[1]}")
        else:
            print("  src loc   : (not yet bodied under src/ - placeholder or un-started)")
    else:
        print("  src claim : (none at this RVA in symbol_names.csv)")

    grow = _csv_find(GHIDRA_FUNCS, rva, "entry_rva")
    if grow:
        print(f"  ghidra    : {grow['name']}  size {grow.get('byte_size', '?')} B")
    elif GHIDRA_FUNCS.is_file():
        print("  ghidra    : (no function start at this RVA in the export)")
    else:
        print("  ghidra    : (no export - run `homm2 ghidra`)")

    if claim and (claim.get("kind") or "func") == "func":
        pct = _fuzzy(claim["name"], claim.get("unit", ""))
        if pct is not None:
            print(f"  match     : {pct:.2f}% fuzzy" + ("  (EXACT)" if pct >= 100.0 else ""))
        elif REPORT.is_file():
            print("  match     : -- (not yet in the diff report - 0%, not started)")


if __name__ == "__main__":
    main()
