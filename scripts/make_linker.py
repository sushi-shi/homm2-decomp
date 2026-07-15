#!/usr/bin/env python3
"""Provision the pinned VC 4.0 LINK 3.00 final-link component."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import sys
import tempfile
from pathlib import Path

import make_toolchain


REPO = Path(__file__).resolve().parent.parent
DEFAULT_OUTPUT = REPO / "build" / "toolchain" / "link300"
REQUIRED_FILES = (
    "LINK.EXE",
    "CVPACK.EXE",
    "CVPACK.ERR",
    "CVTRES.EXE",
    "CVTRES.ERR",
    "MSPDB40.DLL",
)
PINNED_SHA256 = {
    "LINK.EXE": "81109c8cb534debc0c5645db7c3a1b99dd646d982b0fd545070ceb1c77f9cb6c",
    "CVPACK.EXE": "09426223bc4537ad58b46d5abc188c11fcd95f0e3701bd7fe19ce7f20a820d3b",
    "CVPACK.ERR": "190e1cd5beeb79b5736359cb3399246b829f494d7572ef330f4624bdeda56dff",
    "CVTRES.EXE": "7d66e9e5437b8d983432d8addedd7ea342bb814a34b1ffdebbc30018485004e8",
    "CVTRES.ERR": "c2d246a342f3aa9dddbb7145a2c06477a446935bc3d0e92afa31cee4d9d37fbb",
    "MSPDB40.DLL": "7c99519244e1dfe9f8cbc0b54842a757c22121c4269572c7c744d8bd7da0d419",
}


def log(message: str) -> None:
    print(f"[linker] {message}", flush=True)


def fail(message: str) -> "NoReturn":
    print(f"[linker] ERROR: {message}", file=sys.stderr)
    raise SystemExit(1)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def candidate_bin_dirs(roots: list[Path]) -> list[Path]:
    candidates = []
    for root in roots:
        directories = [root]
        directories.extend(path for path in root.rglob("*") if path.is_dir())
        for directory in directories:
            if all(make_toolchain.file_ci(directory, name) for name in REQUIRED_FILES):
                candidates.append(directory.resolve())
    return sorted(set(candidates), key=lambda path: (len(path.parts), str(path)))


def validate(root: Path, allow_unpinned: bool, source_label: str, write: bool) -> bool:
    bin_dir = root / "bin"
    hashes = {}
    for name in REQUIRED_FILES:
        path = make_toolchain.file_ci(bin_dir, name)
        if path is None:
            fail(f"required VC 4.0 linker file is missing: bin/{name}")
        hashes[f"bin/{name}"] = sha256(path)
    mismatches = [
        (name, PINNED_SHA256[name], hashes[f"bin/{name}"])
        for name in REQUIRED_FILES
        if hashes[f"bin/{name}"] != PINNED_SHA256[name]
    ]
    if mismatches:
        for name, expected, actual in mismatches:
            print(
                f"[linker] hash mismatch bin/{name}\n  expected {expected}\n  actual   {actual}",
                file=sys.stderr,
            )
        if not allow_unpinned:
            fail("linker is not the pinned VC 4.0 artifact set")
    if write:
        manifest = {
            "schema": 1,
            "toolchain": "Microsoft Visual C++ 4.0 final-link tools",
            "matching_profile": "LINK 3.00.5270 / CVPACK 3.00 / CVTRES 4.00",
            "pinned_match": not mismatches,
            "source": source_label,
            "sha256": dict(sorted(hashes.items())),
        }
        (root / "manifest.json").write_text(
            json.dumps(manifest, indent=2) + "\n", encoding="ascii")
    log("artifact hashes match the pinned LINK 3.00 component" if not mismatches else
        "accepted an explicitly unpinned linker component")
    return not mismatches


def provision(source: Path, output: Path, force: bool, allow_unpinned: bool) -> None:
    source = source.expanduser().resolve()
    output = output.expanduser().resolve()
    if not source.exists():
        fail(f"source does not exist: {source}")
    if output.exists() and not force:
        fail(f"output already exists: {output}; use --check or --force")
    output.parent.mkdir(parents=True, exist_ok=True)

    with tempfile.TemporaryDirectory(prefix="homm2-link300-") as temporary:
        temporary_root = Path(temporary)
        if source.is_dir():
            search_roots = [source]
        else:
            extracted = temporary_root / "media"
            log(f"extracting {source.name}")
            make_toolchain.archive_extract(source, extracted, prefer_unrar=False)
            search_roots = [extracted]
        candidates = candidate_bin_dirs(search_roots)
        if not candidates:
            fail("no directory containing the complete VC 4.0 LINK/CVPACK tool set was found")

        staging = Path(tempfile.mkdtemp(prefix=f".{output.name}.stage-", dir=output.parent))
        try:
            bin_dir = staging / "bin"
            bin_dir.mkdir()
            source_bin = candidates[0]
            for name in REQUIRED_FILES:
                source_file = make_toolchain.file_ci(source_bin, name)
                shutil.copy2(source_file, bin_dir / name)
            validate(staging, allow_unpinned, source.name, write=True)
            if output.exists():
                shutil.rmtree(output)
            os.replace(staging, output)
        finally:
            if staging.exists():
                shutil.rmtree(staging)
    log(f"provisioned {output}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Provision build/toolchain/link300 from verified VC 4.0 media.")
    parser.add_argument("source", nargs="?", type=Path)
    parser.add_argument("-o", "--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--force", action="store_true")
    parser.add_argument("--allow-unpinned", action="store_true")
    parser.add_argument("--check", type=Path, metavar="TREE")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if args.check is not None:
        if args.source is not None:
            fail("--check does not accept a source")
        validate(args.check.expanduser().resolve(), args.allow_unpinned,
                 args.check.name, write=False)
        return
    if args.source is None:
        fail("provide a VC 4.0 ISO or extracted MSDEV/BIN directory")
    provision(args.source, args.output, args.force, args.allow_unpinned)


if __name__ == "__main__":
    main()
