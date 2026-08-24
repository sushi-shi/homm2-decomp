#!/usr/bin/env python3
"""create-wine-prefix.py - the local HoMM2 GAME environment (play/test).

A one-shot provisioner in the mold of the gruntz sibling script: it builds a
play environment that is SEPARATE from the build wineprefix. It REQUIRES a
legally obtained Buka install's files the first time - the runtime set we
cannot distribute; once game/ is populated a bare run just refreshes the rest
(the rebuilt executable, play.sh, registry keys):

    python3 scripts/toolchain/create-wine-prefix.py [<installed-game-dir>] \\
                                                    [--target build/game-wine]

and assembles:

    <target>/game/     the retail data set (DATA/*.AGG, MAPS/, the middleware
                       DLLs audiere/mss32/smackw32/WING32) + HMM2PL.exe -
                       the freshest rebuilt executable (rsrc mode preferred,
                       transform mode as fallback), refreshed on every run
    <target>/cd/       mapped as the D: cdrom drive. Drop the Buka CD's
                       Tracks2/*.ogg here for in-game music; SetupCDDrive
                       reads "<CDDrive>\\Tracks2\\..." through the registry
    <target>/prefix/   a dedicated wineprefix, SEPARATE from build/wineprefix

Prefix doctrine (adapted from the measured gruntz rules):
  * default windows version - no Version=win98 pin;
  * no audio driver pin - auto-probe follows the current default sink;
  * a 1280x1024 virtual desktop, so the game's DirectDraw mode switches
    (640x480 up to 1280x1024 from the display menu) never touch the host
    video mode;
  * D: maps to <target>/cd as a cdrom drive, and the retail registry key
    (HKLM\\SOFTWARE\\Buka\\3DO\\Heroes of Might and Magic Platinum\\1.000)
    gets PathPL2 -> C:\\hmm2 (the game dir) and "HMM2POL CDDrive" -> D:\\.

Idempotent: existing files (saves, HEROES2.CFG, an already-populated game/)
are never overwritten; only the rebuilt HMM2PL.exe is refreshed.
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
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
    "mss32.dll",
    "smackw32.dll",
    "WING32.DLL",
)

DEFAULT_TARGET = REPO / "build" / "game-wine"
REGISTRY_KEY = r"HKLM\SOFTWARE\Buka\3DO\Heroes of Might and Magic Platinum\1.000"


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
        current = next(
            (p for p in current.iterdir() if p.name.lower() == low), None)
        if current is None:
            return None
    return current


def _wine(prefix: Path, *args: str) -> None:
    env = dict(os.environ, WINEPREFIX=str(prefix),
               WINEDLLOVERRIDES="mscoree,mshtml=", WINEDEBUG="fixme-all")
    subprocess.run(["wine", *args], check=False, env=env,
                   stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL)


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


def install_rebuilt_exe(game: Path) -> None:
    for candidate in (REPO / "build/link/rsrc/HMM2PL.exe",
                      REPO / "build/link/HMM2PL.exe"):
        if candidate.is_file():
            shutil.copy2(candidate, game / "HMM2PL.exe")
            log(f"installed HMM2PL.exe from {candidate.relative_to(REPO)}")
            return
    log("no rebuilt executable yet - run `homm2 link --rsrc` and rerun this")


def setup(resources: Path | None, target: Path) -> None:
    # idempotency is decided at the DESTINATION: only what game/ lacks is
    # demanded of the install folder.
    game = target / "game"
    game.mkdir(parents=True, exist_ok=True)
    needed = [name for name in REQUIRED if find_ci(game, name) is None]
    if needed:
        if resources is None:
            die(f"{game} lacks {', '.join(needed)} and no game folder was "
                "given (point this script at a legally obtained Buka install)")
        if not resources.is_dir():
            die(f"install folder missing: {resources}")
        copied = copy_missing_tree(resources, game)
        log(f"copied {copied} files from {resources}")
        still = [name for name in REQUIRED if find_ci(game, name) is None]
        if still:
            die("the install folder lacks the retail runtime set: "
                + ", ".join(still))

    install_rebuilt_exe(game)

    (target / "cd" / "Tracks2").mkdir(parents=True, exist_ok=True)

    prefix = target / "prefix"
    if not (prefix / "drive_c").is_dir():
        log("creating game wineprefix (default windows version) ...")
        prefix.mkdir(parents=True, exist_ok=True)
        env = dict(os.environ, WINEPREFIX=str(prefix),
                   WINEDLLOVERRIDES="mscoree,mshtml=")
        subprocess.run(["wineboot", "-u"], check=False, env=env,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        _wine(prefix, "reg", "add", r"HKCU\Software\Wine\Explorer",
              "/v", "Desktop", "/d", "Default", "/f")
        _wine(prefix, "reg", "add", r"HKCU\Software\Wine\Explorer\Desktops",
              "/v", "Default", "/d", "1280x1024", "/f")

    c_hmm2 = prefix / "drive_c" / "hmm2"
    if not c_hmm2.is_symlink():
        if c_hmm2.exists():
            die(f"{c_hmm2} exists and is not the expected symlink")
        c_hmm2.symlink_to(game)
        log("mapped C:\\hmm2 -> game/")

    dos_d = prefix / "dosdevices" / "d:"
    if not dos_d.is_symlink():
        dos_d.parent.mkdir(parents=True, exist_ok=True)
        if dos_d.exists():
            dos_d.unlink()
        dos_d.symlink_to(target / "cd")
        log(f"mapped D: -> {target / 'cd'}")
    _wine(prefix, "reg", "add", r"HKLM\Software\Wine\Drives",
          "/v", "D:", "/d", "cdrom", "/f")
    _wine(prefix, "reg", "add", REGISTRY_KEY,
          "/v", "PathPL2", "/d", r"C:\hmm2", "/f")
    _wine(prefix, "reg", "add", REGISTRY_KEY,
          "/v", "HMM2POL CDDrive", "/d", "D:\\", "/f")
    subprocess.run(["wineserver", "-w"], check=False,
                   env=dict(os.environ, WINEPREFIX=str(prefix)))

    play = target / "play.sh"
    play.write_text(
        "#!/usr/bin/env bash\n"
        "# generated by scripts/create-wine-prefix.py - the one launch definition\n"
        f'cd "$(dirname "$0")/game"\n'
        f'export WINEPREFIX="$(dirname "$(pwd)")/prefix"\n'
        'export WINEDLLOVERRIDES="mscoree,mshtml="\n'
        'export WINEDEBUG="${WINEDEBUG:-fixme-all}"\n'
        'exec wine HMM2PL.exe "$@"\n'
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
