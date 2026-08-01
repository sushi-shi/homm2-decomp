#!/usr/bin/env python3
"""Populate build/toolchain from the pinned release, so first init just works.

`homm2 init` cannot get anywhere without the compiler - redelink's name_strings
step runs `cl` under wine - so a missing toolchain used to stop a new checkout
dead, with the dev shell pointing at the hard path: find the en_vc42ent media,
extract it, validate it. A published release has existed since 2026-07-18; nothing
used it.

The sibling Gruntz decomp does this one level up: the toolchain is a Nix
derivation, `pkgs.fetchurl` on its release asset, with MSVC_DIR pointing into the
store. That is the better endgame here too, because a store path cannot drift,
and it is a small change once the release is anonymously fetchable. Until then
the fetch happens at init, where it runs in your shell with your credentials
rather than inside a Nix sandbox without them.

Provisioning from media stays supported and is still the only option for a
different edition; see scripts/toolchain/make_toolchain.py. This just means you
no longer need it to get started.

    python3 -m homm2.init.toolchain           # fetch if absent
    python3 -m homm2.init.toolchain --force   # refetch over an existing tree
    python3 -m homm2.init.toolchain --check   # validate what is there, fetch nothing
"""
from __future__ import annotations

import argparse
import hashlib
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

from homm2.core.paths import REPO

REPOSITORY = "sushi-shi/homm2-decomp"
RELEASE_TAG = "toolchain-vc42-link300"
ASSET = "homm2-toolchain-vc42-link300.tar.xz"
# docs/toolchain-vc42.md records this as the output of two independent packaging
# runs. It is the gate: a tarball that does not hash to it is not the toolchain,
# whatever the release page says.
ASSET_SHA256 = "37f04afbb8cf8005878e010608620c51f16f1a945e7cb76145fd0f52d09617a3"

COMPONENTS = ("msvc", "link300")
CHECKERS = {"msvc": "make_toolchain.py", "link300": "make_linker.py"}


def toolchain_dir() -> Path:
    return Path(os.environ.get("HOMM2_TOOLCHAIN", REPO / "build" / "toolchain"))


def log(message: str) -> None:
    print(f"[toolchain] {message}", flush=True)


def is_provisioned(root: Path) -> bool:
    """True when a compiler is already in place.

    Deliberately shallow - it decides whether to DOWNLOAD, not whether the tree is
    sound. Depth is the job of the pinned-hash checkers, which run after any fetch
    and on demand via --check.
    """
    binaries = root / "msvc" / "bin"
    return any((binaries / name).exists() for name in ("CL.EXE", "cl.exe"))


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def validate(root: Path, allow_unpinned: bool = False) -> bool:
    """Run each component's own pinned-hash checker over an existing tree."""
    ok = True
    for component in COMPONENTS:
        path = root / component
        if not path.is_dir():
            log(f"{component}: MISSING at {path}")
            ok = False
            continue
        command = [sys.executable, str(REPO / "scripts/toolchain" / CHECKERS[component]),
                   "--check", str(path)]
        if allow_unpinned:
            command.append("--allow-unpinned")
        if subprocess.run(command, cwd=REPO).returncode != 0:
            log(f"{component}: FAILED its pinned-artifact check")
            ok = False
        else:
            log(f"{component}: verified")
    return ok


def download(destination: Path) -> Path:
    """Fetch the release asset with the caller's own gh credentials."""
    if shutil.which("gh") is None:
        raise SystemExit(
            "[toolchain] gh is not on PATH; it is what fetches the release asset.\n"
            "            Enter `nix develop .#build`, or provision from media:\n"
            "              scripts/toolchain/make_toolchain.py /path/to/en_vc42ent_disc1.exe")
    log(f"fetching {ASSET} from {REPOSITORY} {RELEASE_TAG}")
    result = subprocess.run(
        ["gh", "release", "download", RELEASE_TAG, "--repo", REPOSITORY,
         "--pattern", ASSET, "--dir", str(destination)])
    if result.returncode != 0:
        raise SystemExit(
            "[toolchain] download failed. If gh is not logged in, run `gh auth login`;\n"
            "            otherwise provision from media with\n"
            "              scripts/toolchain/make_toolchain.py /path/to/en_vc42ent_disc1.exe")
    archive = destination / ASSET
    actual = sha256(archive)
    if actual != ASSET_SHA256:
        # Fail closed and keep nothing: an unexpected tarball must never reach the
        # tree, where it would silently become "the compiler".
        archive.unlink(missing_ok=True)
        raise SystemExit(
            f"[toolchain] REFUSING the download: sha256 {actual}\n"
            f"            expected                  {ASSET_SHA256}")
    log(f"verified sha256 {actual[:16]}...")
    return archive


def install(archive: Path, root: Path) -> None:
    """Unpack beside the target, then swap it in, so a failure leaves the old tree."""
    root.parent.mkdir(parents=True, exist_ok=True)
    staging = Path(tempfile.mkdtemp(prefix=".toolchain-", dir=root.parent))
    try:
        subprocess.run(["tar", "xf", str(archive), "-C", str(staging),
                        "--strip-components=1"], check=True)
        missing = [c for c in COMPONENTS if not (staging / c).is_dir()]
        if missing:
            raise SystemExit(f"[toolchain] archive lacks {', '.join(missing)}")
        previous = root.with_name(root.name + ".previous")
        shutil.rmtree(previous, ignore_errors=True)
        if root.exists():
            os.replace(root, previous)
        os.replace(staging, root)
        staging = None
        shutil.rmtree(previous, ignore_errors=True)
    finally:
        if staging is not None:
            shutil.rmtree(staging, ignore_errors=True)
    log(f"installed {'/'.join(COMPONENTS)} into {root}")


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--force", action="store_true",
                        help="refetch even when a toolchain is already present")
    parser.add_argument("--check", action="store_true",
                        help="validate the existing tree; never download")
    parser.add_argument("--allow-unpinned", action="store_true",
                        help="accept a tree whose artifacts are not the pinned ones")
    args = parser.parse_args(argv)

    root = toolchain_dir()

    if args.check:
        return 0 if validate(root, args.allow_unpinned) else 1

    if is_provisioned(root) and not args.force:
        log(f"present at {root} (--force to refetch, --check to validate)")
        return 0

    with tempfile.TemporaryDirectory(prefix="homm2-toolchain-") as scratch:
        install(download(Path(scratch)), root)

    if not validate(root, args.allow_unpinned):
        log("the installed tree did not pass its pinned-artifact checks")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
