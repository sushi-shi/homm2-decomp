#!/usr/bin/env python3
"""Assemble a source file to ordinary OMF with the pinned MASM 6.11 ML.EXE."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from homm2.core import wine


def assemble(src: Path, out: Path) -> None:
    assembler = wine.tool("ml.exe")
    src = src.resolve()
    out = out.resolve()
    if not src.is_file():
        raise RuntimeError(f"assembly source is missing: {src}")
    out.parent.mkdir(parents=True, exist_ok=True)
    out.unlink(missing_ok=True)
    try:
        wine.run(assembler, "/nologo", "/c",
                 f"/Fo{wine.winepath_w(out)}", wine.winepath_w(src),
                 cwd=out.parent, quiet=True)
    except RuntimeError as error:
        raise RuntimeError(f"MASM 6.11 failed for {src.name}: {error}") from error
    if not out.is_file():
        raise RuntimeError(f"MASM 6.11 produced no object for {src.name}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--src", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()
    try:
        assemble(args.src, args.out)
    except RuntimeError as error:
        print(f"[ml_wrap] ERROR: {error}", file=sys.stderr)
        raise SystemExit(1) from error


if __name__ == "__main__":
    main()
