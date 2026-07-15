#!/usr/bin/env python3
"""homm2.analysis.decomp - Ghidra decompiler C for a function.

Reopens the cached Ghidra project (build/ghidra, built by `homm2 ghidra`) with NO
re-analysis, runs decomp_export.py over the requested RVAs, and prints the decompiler C
plus caller/callee xrefs. Names read from our CodeView seeding (apply_names.py).

Accepts RVAs or full VAs (a value >= the image base is reduced to its RVA).
This command is currently exposed through the module entry point, not `homm2 sema`.

    python3 -m homm2.analysis.decomp 0x0004a3c0
    python3 -m homm2.analysis.decomp 0x0004a3c0 --force   # create the fn first if Ghidra missed it
"""
import os
import sys
from pathlib import Path

REPO = Path(os.environ.get("HOMM2_DIR")) if os.environ.get("HOMM2_DIR") else \
    next((p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists()),
         Path(__file__).resolve().parents[3])
IMAGEBASE = 0x400000


def main():
    args = sys.argv[1:]
    force = "--force" in args
    rvas = [a for a in args if not a.startswith("--")]
    if not rvas:
        sys.exit(__doc__)

    from homm2.ghidra.driver import DECOMP_EXPORT, PROJ_DIR, PROJ_NAME, run_scripts
    if not (PROJ_DIR / f"{PROJ_NAME}.rep").is_dir():
        sys.exit("[decomp] no Ghidra project - run `homm2 ghidra` first (one-time, minutes)")

    token = "%d" % os.getpid()
    targets = Path("/tmp/homm2_decomp_%s_targets.txt" % token)
    out = Path("/tmp/homm2_decomp_%s_out.txt" % token)
    lines = []
    for a in rvas:
        try:
            n = int(a, 16)
        except ValueError:
            sys.exit(f"[decomp] '{a}' is not a hex address")
        lines.append("0x%x" % (n - IMAGEBASE if n >= IMAGEBASE else n))
    targets.write_text("\n".join(lines) + "\n")
    if out.exists():
        out.unlink()
    os.environ["HOMM2_DECOMP_TARGETS"] = str(targets)
    os.environ["HOMM2_DECOMP_OUT"] = str(out)
    if force:
        os.environ["HOMM2_DECOMP_FORCE"] = "1"

    print("[decomp] opening Ghidra project (no re-analysis) ...", file=sys.stderr, flush=True)
    try:
        run_scripts([DECOMP_EXPORT], analyze=False)
        if not out.is_file():
            sys.exit("[decomp] Ghidra produced no output (see log above)")
        sys.stdout.write(out.read_text())
    finally:
        targets.unlink(missing_ok=True)
        out.unlink(missing_ok=True)


if __name__ == "__main__":
    main()
