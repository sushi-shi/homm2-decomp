#!/usr/bin/env python3
"""Build, provision, and launch the reconstructed Buka executable.

The Nix app supplies the exact compiler/linker tools and Wine. This script owns
the repository-local workflow so `HOMM2_DATA=/path/to/game nix run` has one
definition and never launches the executable in the compiler Wine prefix.
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


REPO = Path(__file__).resolve().parents[2]
RETAIL_EXE = REPO / "build/orig/HMM2PL.exe"
TOOLCHAIN_EXES = (
    REPO / "build/toolchain/msvc/bin/CL.EXE",
    REPO / "build/toolchain/msvc/bin/cl.exe",
)
PLAY_ENVIRONMENT = REPO / "build/game-wine"


def fail(message: str) -> None:
    print(f"[homm2-run] ERROR: {message}", file=sys.stderr)
    raise SystemExit(1)


def run(*command: str) -> None:
    print("[homm2-run] " + " ".join(command))
    subprocess.run(command, cwd=REPO, check=True)


def needs_initialization() -> bool:
    return (
        not any(path.is_file() for path in TOOLCHAIN_EXES)
        or not (REPO / "build.ninja").is_file()
        or not (REPO / "build/delink").is_dir()
    )


def install_control_executable(data: Path) -> None:
    if RETAIL_EXE.is_file():
        return

    candidates = sorted(
        (path for path in data.iterdir() if path.name.casefold() == "hmm2pl.exe"),
        key=lambda path: path.name,
    )
    if not candidates:
        fail(
            f"{data} does not contain the retail Buka HMM2PL.exe required "
            "as matching and resource-link evidence"
        )
    if len(candidates) != 1:
        fail(
            f"ambiguous case-insensitive HMM2PL.exe below {data}: "
            + ", ".join(path.name for path in candidates)
        )
    if not candidates[0].is_file():
        fail(f"retail control executable is not a regular file: {candidates[0]}")

    RETAIL_EXE.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(candidates[0], RETAIL_EXE)
    print(f"[homm2-run] installed retail control image at {RETAIL_EXE.relative_to(REPO)}")


def prepare(data: Path) -> Path:
    if not data.is_dir():
        fail(f"HOMM2_DATA is not a directory: {data}")
    install_control_executable(data)

    if needs_initialization():
        run("homm2", "init")
    run("homm2", "build")
    run("homm2", "link", "--rsrc")
    run(
        sys.executable,
        str(REPO / "scripts/toolchain/create-wine-prefix.py"),
        str(data),
        "--target",
        str(PLAY_ENVIRONMENT),
    )

    play = PLAY_ENVIRONMENT / "play.sh"
    if not play.is_file():
        fail(f"Wine environment did not produce {play}")
    run(str(play), "--setup-only")
    return play


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--prepare-only",
        action="store_true",
        help="build and provision the Wine environment without starting the game",
    )
    parser.add_argument("game_arguments", nargs=argparse.REMAINDER)
    args = parser.parse_args()

    data_text = os.environ.get("HOMM2_DATA")
    if not data_text:
        fail("set HOMM2_DATA to a legally obtained Buka game installation")
    play = prepare(Path(data_text).expanduser().resolve())
    if args.prepare_only:
        print(f"[homm2-run] ready: {play}")
        return 0

    arguments = [str(play), *args.game_arguments]
    os.execv(str(play), arguments)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
