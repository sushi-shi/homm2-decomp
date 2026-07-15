#!/usr/bin/env python3
"""Build the reproducible HoMM2 VC 4.2 toolchain release tarball.

This is the Gruntz create-toolchain-release.py pattern adapted to the actual
en_vc42ent disc1 layout. The RAR SFX already contains a complete installed
MSDEV/{BIN,INCLUDE,LIB} tree; make_toolchain performs extraction and strict
artifact validation, then this script normalizes the release archive metadata.
"""

from __future__ import annotations

import hashlib
import os
import shutil
import subprocess
import tempfile
from pathlib import Path

import make_toolchain


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parent.parent)).resolve()
RELEASE_EPOCH = 1782573438  # v0.1.0 publication day, 2026-06-27 UTC.


def log(message: str) -> None:
    print(f"[release] {message}", flush=True)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main() -> None:
    media_value = os.environ.get("VC42_DISC1")
    if not media_value:
        raise SystemExit("VC42_DISC1 is not set; use scripts/create-toolchain-release.nix")
    media = Path(media_value).expanduser().resolve()
    if not media.is_file():
        raise SystemExit(f"VC42_DISC1 does not exist: {media}")

    output = Path(
        os.environ.get("OUTPUT", REPO / "build" / "homm2-toolchain-vc42.tar.xz")
    ).expanduser().resolve()
    output.parent.mkdir(parents=True, exist_ok=True)

    work_parent = Path(os.environ.get("WORK_DIR", REPO / "build")).expanduser().resolve()
    work_parent.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix=".vc42-release-", dir=work_parent))
    try:
        extracted = work / "media"
        stage = work / "stage"
        stage_msvc = stage / "msvc"

        log(f"extracting {media.name}")
        make_toolchain.archive_extract(media, extracted)
        candidates = make_toolchain.candidate_roots([extracted])
        if not candidates:
            raise SystemExit("disc1 did not produce a complete MSDEV/BIN,INCLUDE,LIB tree")

        source_root = candidates[0]
        log(f"staging {source_root}")
        make_toolchain.copy_tree(source_root, stage_msvc)
        make_toolchain.validate(stage_msvc, False, "en_vc42ent_disc1.exe", write=True)

        log(f"packaging {output}")
        subprocess.run(
            [
                "tar",
                "--sort=name",
                "--format=gnu",
                "--owner=0",
                "--group=0",
                "--numeric-owner",
                f"--mtime=@{RELEASE_EPOCH}",
                "--transform",
                r"s|^\.|homm2-toolchain-vc42|",
                "-C",
                str(stage),
                "-cJf",
                str(output),
                ".",
            ],
            check=True,
        )

        digest = sha256(output)
        print()
        log("done")
        print(f"  Output: {output}")
        print(f"  SHA256: {digest}")
        print()
        print("Publish as a dedicated toolchain asset, not as source code:")
        print(
            f"  gh release upload toolchain-vc42 {output} "
            "--repo homm2-decomp/homm2-decomp --clobber"
        )
    finally:
        shutil.rmtree(work, ignore_errors=True)


if __name__ == "__main__":
    main()
