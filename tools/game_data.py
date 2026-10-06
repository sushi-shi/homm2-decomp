#!/usr/bin/env python3
"""Check a player's copy of Heroes II and lay out the native game's read-only data.

    game_data.py --game PATH --out DIR [--program DIR] [--work DIR]

PATH is an installed game folder (GOG's, a Windows or DOS installation), the
Buka disc's unpacked files, or a .zip/.7z/.iso of one of them, or a folder
holding only such an archive. The installation is the shallowest folder below
PATH with DATA/HEROES2.AGG; names are matched case-insensitively, as the game
matches them. Both archives must be Heroes II resource archives (read with
agg_manifest.parse); one of the tested editions is named, another is accepted.
The installation is copied as

    DIR/game/DATA, MAPS, GAMES, HELP, HEROES2, MUSIC, TRACKS2,
             H2CAMP.TXT, POLCAMP.TXT, HEROES2.CFG       (the names upper-cased)

which is what the game reads (package_web_data's set, plus the Buka disc's
music). Nothing else is taken. With --program, the copy's Windows program
(HMM2PL.exe or HEROES2W.EXE) is copied there, for its icon. Used by the
flake's game package (nix/game.nix) at install time.
"""

from __future__ import annotations

import argparse
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

from agg_manifest import AggError, parse
from package_web_data import DIRECTORIES, FILES, resolve_name


#: The game's folders and files: the browser package's, and the Buka disc's
#: music, which the browser build does not play.
GAME_DIRECTORIES = (*DIRECTORIES, "TRACKS2")
GAME_FILES = FILES
ARCHIVES = ("HEROES2.AGG", "HEROES2X.AGG")

#: Tested archives (README, "Play on Linux").
KNOWN = {
    "7a11c86db8ec8fbf19d810c1f7ebf9d8520f63fbdc42d6f51f7538654004315d": "HEROES2.AGG, GOG, English",
    "1f3edad1bb88052da50b3fae5d00e8f7e73dd977f356463022aab132b54f4d41": "HEROES2X.AGG, GOG, English",
    "da08a14cc545f6708bd2b746edd7ea7fa0eb7d0ed26f6e54ab8e6ccb2e735e19": "HEROES2.AGG, Buka, Russian",
    "68f12a2ca2dd1a000e1136ad70f38b19116686c2a7a0e9a0860528d998b52afd": "HEROES2X.AGG, Buka, Russian",
}

#: The Windows programs whose icon the desktop entry shows, preferred first.
PROGRAMS = ("HMM2PL.EXE", "HEROES2W.EXE")

#: Extensions 7z unpacks for us.
PACKED = (".zip", ".7z", ".iso")

#: How deep below PATH an installation or a program is looked for.
DEPTH = 4


class GameDataError(Exception):
    pass


def say(message: str) -> None:
    print(f"[import] {message}", file=sys.stderr)


def holds_game(directory: Path) -> bool:
    data = resolve_name(directory, "DATA")
    if data is None or not data.is_dir():
        return False
    archive = resolve_name(data, "HEROES2.AGG")
    return archive is not None and archive.is_file()


def directories(root: Path, depth: int):
    """root and the folders below it, shallowest first."""
    level = [root]
    for _ in range(depth + 1):
        following = []
        for directory in level:
            yield directory
            try:
                following.extend(sorted(p for p in directory.iterdir() if p.is_dir()))
            except OSError:
                continue
        level = following


def find_installation(root: Path) -> Path | None:
    return next((d for d in directories(root, DEPTH) if holds_game(d)), None)


def find_program(root: Path, installation: Path) -> Path | None:
    """The copy's Windows program: beside the game first, else anywhere."""
    for directory in (installation, *directories(root, DEPTH)):
        for name in PROGRAMS:
            try:
                program = resolve_name(directory, name)
            except (OSError, ValueError):
                continue
            if program is not None and program.is_file():
                return program
    return None


def unpack(given: Path, work: Path) -> Path:
    """A folder as it is; an archive, or a folder holding only one, unpacked."""
    if given.is_dir():
        if find_installation(given) is not None:
            return given
        packed = [p for p in given.iterdir() if p.is_file() and p.suffix.lower() in PACKED]
        if len(packed) != 1:
            return given
        given = packed[0]
    if not given.is_file() or given.suffix.lower() not in PACKED:
        raise GameDataError(f"{given}: not a folder or a .zip/.7z/.iso archive")
    target = work / "unpacked"
    say(f"unpacking {given}")
    result = subprocess.run(["7z", "x", "-y", f"-o{target}", str(given)],
                            stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, text=True)
    if result.returncode != 0:
        raise GameDataError(f"{given}: 7z could not unpack it: {result.stderr.strip()}")
    return target


def check(installation: Path) -> list[str]:
    """The archives' problems; notes about the optional parts."""
    errors = []
    data = resolve_name(installation, "DATA")
    for name in ARCHIVES:
        archive = resolve_name(data, name)
        if archive is None or not archive.is_file():
            errors.append(f"missing DATA/{name}")
            continue
        try:
            manifest = parse(archive)
        except (AggError, UnicodeDecodeError) as error:
            errors.append(f"DATA/{name} is not a Heroes II archive: {error}")
            continue
        edition = KNOWN.get(manifest["archive_sha256"])
        if edition is None:
            say(f"DATA/{name}: {manifest['entry_count']} entries, not one of the tested "
                f"archives (it may still work)")
        else:
            say(f"DATA/{name}: {edition}")
    if resolve_name(installation, "MAPS") is None:
        say("warning: no MAPS folder; there will be no scenarios to play")
    if resolve_name(installation, "MUSIC") is None and resolve_name(installation, "TRACKS2") is None:
        say("note: no MUSIC or TRACKS2 folder; the game will be silent")
    if resolve_name(installation, "HEROES2") is None:
        say("note: no HEROES2/ANIM folder; the movies will be skipped")
    return errors


def lay_out(installation: Path, game: Path) -> None:
    game.mkdir(parents=True)
    for name in (*GAME_DIRECTORIES, *GAME_FILES):
        source = resolve_name(installation, name)
        if source is None:
            continue
        if name in GAME_DIRECTORIES:
            if not source.is_dir():
                raise GameDataError(f"expected a folder: {source}")
            for link in source.rglob("*"):
                if link.is_symlink() and not link.exists():
                    say(f"warning: {link} links to nothing; left out")
            shutil.copytree(source, game / name, ignore_dangling_symlinks=True)
        else:
            if not source.is_file():
                raise GameDataError(f"expected a file: {source}")
            shutil.copyfile(source, game / name)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("--game", type=Path, required=True, metavar="PATH")
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--program", type=Path, metavar="DIR",
                        help="where to copy the copy's Windows program, for its icon")
    parser.add_argument("--work", type=Path, help="where to unpack (default: $TMPDIR)")
    args = parser.parse_args()
    if args.out.exists():
        say(f"{args.out} already exists")
        return 1
    try:
        given = args.game.expanduser().resolve(strict=True)
        with tempfile.TemporaryDirectory(prefix=".import-", dir=args.work) as work:
            root = unpack(given, Path(work))
            installation = find_installation(root)
            if installation is None:
                raise GameDataError(f"no DATA/HEROES2.AGG in {args.game}: pass the installed "
                                    "game folder, the disc's files or an archive of either")
            errors = check(installation)
            if errors:
                raise GameDataError(f"{installation}: " + "; ".join(errors))
            say(f"checked the game in {installation}")
            lay_out(installation, args.out / "game")
            if args.program is not None:
                program = find_program(root, installation)
                if program is None:
                    say("note: no HMM2PL.exe or HEROES2W.EXE; the menu entry has no icon")
                else:
                    args.program.mkdir(parents=True, exist_ok=True)
                    shutil.copyfile(program, args.program / program.name.upper())
    except (GameDataError, OSError, ValueError) as error:
        say(str(error))
        return 1
    say(f"game data laid out in {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
