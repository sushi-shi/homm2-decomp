#!/usr/bin/env python3
"""Populate build/toolchain from the pinned release, so first init just works.

`homm2 init` cannot get anywhere without the compiler - redelink's name_strings
step runs `cl` under wine - so a missing toolchain used to stop a new checkout
dead, with the dev shell pointing at the hard path: find the VS6 Enterprise media,
extract it, validate it. A published release has existed since 2026-07-18; nothing
used it.

The sibling Gruntz decomp does this one level up: the toolchain is a Nix
derivation, `pkgs.fetchurl` on its release asset, with MSVC_DIR pointing into the
store. That is the better endgame here too, because a store path cannot drift,
and it is a small change once the release is anonymously fetchable. Until then
the fetch happens at init, where it runs in your shell with your credentials
rather than inside a Nix sandbox without them.

Reproducing the tarball from the preserved VS6 Enterprise disc and SP5 media
stays supported: scripts/toolchain/create-toolchain-release.nix. This just means
you no longer need the media to get started.

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
RELEASE_TAG = "toolchain-vc6-sp5"
ASSET = "homm2-toolchain-vc6-sp5.tar.xz"
# The gate: a tarball that does not hash to this is not the toolchain, whatever
# the release page says.
ASSET_SHA256 = "b243b68a1df8c2c54f7830a14221fa0a73fd3bbca78343fd1dd81a815a32ecec"

# One component, and the archive has no wrapping directory - its top level IS
# msvc/, so it unpacks with no --strip-components. An older toolchain release shipped two
# components under a wrapper; do not carry that shape over.
COMPONENTS = ("msvc",)
# The release builder owns the pinned per-artifact hashes; --check re-runs its
# verify() over an unpacked tree, so they are not duplicated here.
CHECKER = "create-toolchain-release.py"


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


def validate(root: Path) -> bool:
    """Run each component's own pinned-hash checker over an existing tree."""
    ok = True
    for component in COMPONENTS:
        path = root / component
        if not path.is_dir():
            log(f"{component}: MISSING at {path}")
            ok = False
            continue
        command = [sys.executable, str(REPO / "scripts/toolchain" / CHECKER),
                   "--check", str(path)]
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
            "              nix-shell scripts/toolchain/create-toolchain-release.nix")
    log(f"fetching {ASSET} from {REPOSITORY} {RELEASE_TAG}")
    result = subprocess.run(
        ["gh", "release", "download", RELEASE_TAG, "--repo", REPOSITORY,
         "--pattern", ASSET, "--dir", str(destination)])
    if result.returncode != 0:
        raise SystemExit(
            "[toolchain] download failed. If gh is not logged in, run `gh auth login`;\n"
            "            otherwise provision from media with\n"
            "              nix-shell scripts/toolchain/create-toolchain-release.nix")
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
        subprocess.run(["tar", "xf", str(archive), "-C", str(staging)], check=True)
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
    args = parser.parse_args(argv)

    root = toolchain_dir()

    if args.check:
        return 0 if validate(root) else 1

    if is_provisioned(root) and not args.force:
        log(f"present at {root} (--force to refetch, --check to validate)")
        return 0

    with tempfile.TemporaryDirectory(prefix="homm2-toolchain-") as scratch:
        install(download(Path(scratch)), root)

    if not validate(root):
        log("the installed tree did not pass its pinned-artifact checks")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
