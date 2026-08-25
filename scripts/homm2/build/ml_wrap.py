#!/usr/bin/env python3
"""Assemble source with pinned MASM 6.11 as OMF or comparison-only COFF."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from homm2.core import wine


def assemble(src: Path, out: Path, *, coff: bool = False) -> None:
    # Path conversion must see the repository-owned prefix before it checks the
    # verified Z: mapping. ``wine.run`` prepares the same environment later,
    # but its call would otherwise come too late for the /Fo argument.
    wine.prepare_env()
    assembler = wine.tool("ml.exe")
    src = src.resolve()
    out = out.resolve()
    if not src.is_file():
        raise RuntimeError(f"assembly source is missing: {src}")
    out.parent.mkdir(parents=True, exist_ok=True)
    out.unlink(missing_ok=True)
    try:
        flags = ["/nologo", "/c"]
        if coff:
            flags.append("/coff")
        wine.run(assembler, *flags, f"/Fo{wine.winepath_w(out)}",
                 wine.winepath_w(src),
                 cwd=out.parent, quiet=True)
    except RuntimeError as error:
        raise RuntimeError(f"MASM 6.11 failed for {src.name}: {error}") from error
    if not out.is_file():
        raise RuntimeError(f"MASM 6.11 produced no object for {src.name}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--src", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument(
        "--coff", action="store_true",
        help="emit COFF for objdiff; the retail link continues to use OMF")
    args = parser.parse_args()
    try:
        assemble(args.src, args.out, coff=args.coff)
    except RuntimeError as error:
        print(f"[ml_wrap] ERROR: {error}", file=sys.stderr)
        raise SystemExit(1) from error


if __name__ == "__main__":
    main()
