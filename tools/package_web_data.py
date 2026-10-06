#!/usr/bin/env python3
"""Resolve retail installation paths and package them for the browser."""

import argparse
from pathlib import Path
import subprocess
import sys


DIRECTORIES = ("DATA", "GAMES", "HELP", "HEROES2", "MAPS", "MUSIC")
FILES = ("H2CAMP.TXT", "POLCAMP.TXT", "HEROES2.CFG")


def resolve_name(directory: Path, name: str) -> Path | None:
    exact = directory / name
    if exact.exists():
        return exact
    matches = [entry for entry in directory.iterdir() if entry.name.lower() == name.lower()]
    if len(matches) > 1:
        raise ValueError(f"ambiguous case-insensitive path: {directory / name}")
    return matches[0] if matches else None


def preloads(game_data: str) -> list[str]:
    if not game_data:
        raise ValueError("set HOMM2_DATA to the installed game directory")
    root = Path(game_data).absolute()
    data = resolve_name(root, "DATA")
    archive = resolve_name(data, "HEROES2.AGG") if data and data.is_dir() else None
    if archive is None or not archive.is_file():
        raise ValueError(f"no DATA/HEROES2.AGG below {root} (matched case-insensitively)")

    options = []
    for name in (*DIRECTORIES, *FILES):
        path = resolve_name(root, name)
        if path is None:
            continue
        if name in DIRECTORIES and not path.is_dir():
            raise ValueError(f"expected a directory: {path}")
        if name in FILES and not path.is_file():
            raise ValueError(f"expected a file: {path}")
        options.extend(("--preload", f"{path}@/game/{name}"))
    return options


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--game-data", required=True)
    parser.add_argument("--check", action="store_true", help="validate paths without packaging")
    parser.add_argument("--packager", type=Path)
    parser.add_argument("--lang", type=Path)
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    try:
        options = preloads(args.game_data)
        if args.check:
            return 0
        if args.packager is None or args.lang is None or args.output is None:
            parser.error("packaging requires --packager, --lang, and --output")
        subprocess.run(
            [str(args.packager), str(args.output / "homm2.data"),
             "--preload", f"{args.lang.absolute()}@/lang", *options,
             f"--js-output={args.output / 'homm2.data.js'}"],
            check=True,
        )
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        print(f"homm2-web: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
