#!/usr/bin/env python3
"""create-wine-prefix.py - the local HoMM2 GAME environment (play/test).

A one-shot provisioner in the mold of the gruntz sibling script: it stages a
play environment that is SEPARATE from the build wineprefix. It REQUIRES a
legally obtained Buka install's files the first time - the runtime set we
cannot distribute; once game/ is populated a bare run just refreshes the rest
(the rebuilt executable, play.sh, registry keys):

    python3 scripts/toolchain/create-wine-prefix.py [<installed-game-dir>] \\
                                                    [--target build/game-wine]

and assembles:

    <target>/game/     the retail data set (DATA/*.AGG, MAPS/, the middleware
                       DLLs audiere/mss32/smackw32/WING32) + HMM2PL.exe -
                       the ordinary rebuilt `homm2 link --rsrc` executable,
                       refreshed on every run
    <target>/cd/       mapped as the D: cdrom drive. Anim2 movies and Tracks2
                       music are staged from the supplied install (including
                       its conventional .wine-cd/ directory)
    <target>/prefix/   a dedicated wineprefix, SEPARATE from build/wineprefix

The generated play wrapper delegates to the same measured `run-game.sh` that
ships with a cleaned source tree. That one launch definition owns Wine setup:
the dedicated prefix, 640x480 virtual desktop, Russian ANSI code page, Buka
registry values, G: install mapping and D: cdrom mapping.

Idempotent: existing files (saves, HEROES2.CFG, an already-populated game/)
are never overwritten; only the rebuilt HMM2PL.exe is refreshed.
"""

from __future__ import annotations

import argparse
import filecmp
import shutil
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]

#: what the game dir must provide before play makes sense: the archives the
#: startup path opens (SetupCDDrive probes ".\\DATA\\HEROES2x.AGG") plus the
#: four middleware DLLs the executable imports.
REQUIRED = (
    "DATA/HEROES2.AGG",
    "DATA/HEROES2X.AGG",
    "audiere.dll",
    "MSS32.DLL",
    "SMACKW32.DLL",
    "WING32.DLL",
)
GAME_DIRECTORIES = ("DATA", "MAPS", "HELP")
GAME_ROOT_SUFFIXES = (".cfg", ".dll", ".txt")
CD_DIRECTORIES = ("Anim2", "Tracks2")

DEFAULT_TARGET = REPO / "build" / "game-wine"


def log(msg: str) -> None:
    print(f"[wine-prefix] {msg}")


def die(msg: str) -> None:
    print(f"[wine-prefix] ERROR: {msg}", file=sys.stderr)
    sys.exit(1)


def find_ci(root: Path, relative: str) -> Path | None:
    """Case-insensitive lookup of `relative` (may contain '/') under root."""
    current = root
    for part in relative.split("/"):
        if not current.is_dir():
            return None
        low = part.lower()
        matches = sorted(
            (path for path in current.iterdir() if path.name.lower() == low),
            key=lambda path: path.name,
        )
        if not matches:
            return None
        if len(matches) != 1:
            die(
                f"ambiguous case-insensitive path below {current}: "
                + ", ".join(path.name for path in matches)
            )
        current = matches[0]
    return current


def canonicalize_required(root: Path, relative: str) -> Path | None:
    """Give one required staged file its canonical spelling.

    Older provisioner runs could preserve the install's lowercase spelling and
    then copy a second uppercase spelling for Wine. Collapse that generated
    duplicate only when every variant is byte-identical; conflicting retail
    inputs remain an error.
    """
    current = root
    parts = relative.split("/")
    for part in parts[:-1]:
        if not current.is_dir():
            return None
        matches = sorted(
            (path for path in current.iterdir()
             if path.name.casefold() == part.casefold()),
            key=lambda path: path.name,
        )
        if not matches:
            return None
        if len(matches) != 1 or not matches[0].is_dir():
            die(
                f"ambiguous case-insensitive directory below {current}: "
                + ", ".join(path.name for path in matches)
            )
        current = matches[0]

    canonical = current / parts[-1]
    matches = sorted(
        (path for path in current.iterdir()
         if path.name.casefold() == parts[-1].casefold()),
        key=lambda path: path.name,
    )
    if not matches:
        return None
    if any(not path.is_file() for path in matches):
        die(
            f"required runtime path is not a regular file below {current}: "
            + ", ".join(path.name for path in matches)
        )

    reference = canonical if canonical in matches else matches[0]
    for path in matches:
        if path != reference and not filecmp.cmp(reference, path, shallow=False):
            die(
                f"conflicting case-insensitive runtime files below {current}: "
                + ", ".join(candidate.name for candidate in matches)
            )

    if reference != canonical:
        reference.rename(canonical)
        log(f"normalized {relative} from {reference.name}")
    for path in matches:
        if path != reference:
            path.unlink()
            log(f"removed byte-identical staged duplicate {path}")
    return canonical


def copy_missing_tree(source: Path, destination: Path) -> int:
    """Copy everything from source that destination lacks; never overwrite."""
    copied = 0
    for path in source.rglob("*"):
        relative = path.relative_to(source)
        out = destination / relative
        if path.is_dir():
            out.mkdir(parents=True, exist_ok=True)
        elif not out.exists():
            out.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(path, out)
            copied += 1
    return copied


def copy_game_files(source: Path, destination: Path) -> int:
    """Copy the retail runtime, excluding build/test state beside it."""
    copied = 0
    for name in GAME_DIRECTORIES:
        directory = find_ci(source, name)
        if directory is not None and directory.is_dir():
            copied += copy_missing_tree(directory, destination / name)
    for path in source.iterdir():
        if not path.is_file() or path.name.startswith("."):
            continue
        if path.suffix.lower() not in GAME_ROOT_SUFFIXES:
            continue
        output = destination / path.name
        if not output.exists():
            shutil.copy2(path, output)
            copied += 1
    return copied


def install_rebuilt_exe(game: Path) -> None:
    candidate = REPO / "build/link/rsrc/HMM2PL.exe"
    if not candidate.is_file():
        die("rebuilt executable missing - run `homm2 link --rsrc` and rerun this")
    shutil.copy2(candidate, game / "HMM2PL.exe")
    log(f"installed HMM2PL.exe from {candidate.relative_to(REPO)}")


def setup(resources: Path | None, target: Path) -> None:
    # idempotency is decided at the DESTINATION: only what game/ lacks is
    # demanded of the install folder.
    game = target / "game"
    game.mkdir(parents=True, exist_ok=True)
    needed = [name for name in REQUIRED
              if canonicalize_required(game, name) is None]
    if needed:
        if resources is None:
            die(f"{game} lacks {', '.join(needed)} and no game folder was "
                "given (point this script at a legally obtained Buka install)")
        if not resources.is_dir():
            die(f"install folder missing: {resources}")
        copied = copy_game_files(resources, game)
        log(f"copied {copied} files from {resources}")
        still = [name for name in REQUIRED
                 if canonicalize_required(game, name) is None]
        if still:
            die("the install folder lacks the retail runtime set: "
                + ", ".join(still))

    # The canonical runner checks these exact names before Wine starts.
    for relative in REQUIRED:
        if canonicalize_required(game, relative) is None:
            die(f"staged game unexpectedly lacks {relative}")

    install_rebuilt_exe(game)

    cd = target / "cd"
    cd.mkdir(parents=True, exist_ok=True)
    if resources is not None:
        hidden_cd = find_ci(resources, ".wine-cd")
        cd_source = hidden_cd if hidden_cd is not None and hidden_cd.is_dir() else resources
        copied = 0
        for name in CD_DIRECTORIES:
            source_directory = find_ci(cd_source, name)
            if source_directory is not None and source_directory.is_dir():
                copied += copy_missing_tree(
                    source_directory.resolve(),
                    cd / name,
                )
        if copied:
            log(f"copied {copied} CD runtime files from {cd_source}")

    canonical_runner = REPO / "scripts/homm2/clean/project/run-game.sh"
    staged_runner = game / "run-game.sh"
    shutil.copy2(canonical_runner, staged_runner)
    staged_runner.chmod(0o755)

    play = target / "play.sh"
    play.write_text(
        "#!/usr/bin/env bash\n"
        "# generated adapter; game/run-game.sh is the launch definition\n"
        'environment_root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)\n'
        'export HOMM2_WINEPREFIX="$environment_root/prefix"\n'
        'export HOMM2_CD_DIR="$environment_root/cd"\n'
        'export HOMM2_EXE=HMM2PL.exe\n'
        'exec "$environment_root/game/run-game.sh" "$@"\n'
    )
    play.chmod(0o755)
    log(f"ready: {target} (run {play})")


def main() -> int:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        "resources", nargs="?",
        help="a legally obtained Buka install folder (DATA/, the middleware "
             "DLLs); optional once game/ is populated")
    parser.add_argument("--target", default=str(DEFAULT_TARGET),
                        help=f"environment root (default {DEFAULT_TARGET})")
    args = parser.parse_args()
    setup(Path(args.resources) if args.resources else None, Path(args.target))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
