#!/usr/bin/env python3
"""Assemble a source file to ordinary OMF with the pinned MASM 6.11 ML.EXE."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

from homm2.build.cc_wrap import (
    ensure_wineserver,
    find_ci,
    msvc_dir,
    winepath_w,
)


def assemble(src: Path, out: Path) -> None:
    toolchain = msvc_dir()
    assembler = find_ci(toolchain / "bin", "ml.exe")
    if assembler is None:
        raise RuntimeError(
            f"ML.EXE not found under {toolchain}/bin; rebuild or fetch the pinned toolchain")
    if shutil.which("wine") is None:
        raise RuntimeError("wine not found; run inside `nix develop .#build`")

    src = src.resolve()
    out = out.resolve()
    if not src.is_file():
        raise RuntimeError(f"assembly source is missing: {src}")
    out.parent.mkdir(parents=True, exist_ok=True)
    out.unlink(missing_ok=True)
    ensure_wineserver()
    result = subprocess.run(
        ["wine", str(assembler), "/nologo", "/c",
         f"/Fo{winepath_w(out)}", winepath_w(src)],
        cwd=out.parent,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        encoding="latin1",
    )
    if result.returncode or not out.is_file():
        detail = "\n".join(result.stdout.strip().splitlines()[-15:])
        raise RuntimeError(
            f"MASM 6.11 failed for {src.name} (exit {result.returncode})\n{detail}")


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
