#!/usr/bin/env python3
"""Provision the pinned Visual C++ 4.2 matching toolchain.

The original repository scaffold referenced make-toolchain.sh without ever
committing it. This implementation accepts either an installed/extracted MSDEV
tree or one or more VC 4.2 disc/archive images. Archive extraction is deliberately
best effort because some editions use an installer database that 7z cannot lay
out as an installed tree; in that case install/extract the media separately and
pass the resulting MSDEV directory.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


REPO = Path(__file__).resolve().parent.parent
DEFAULT_OUTPUT = REPO / "build" / "toolchain" / "msvc"

REQUIRED_BIN = (
    "CL.EXE",
    "C1.EXE",
    "C1XX.EXE",
    "C2.EXE",
    "LINK.EXE",
    "CVPACK.EXE",
    "CVTRES.EXE",
    "MSPDB41.DLL",
)
REQUIRED_INCLUDE = ("STDIO.H", "STDDEF.H", "IO.H", "STRING.H")
REQUIRED_LIB = (
    "LIBCMT.LIB",
    "OLDNAMES.LIB",
    "KERNEL32.LIB",
    "USER32.LIB",
    "GDI32.LIB",
    "WSOCK32.LIB",
    "NETAPI32.LIB",
    "WINMM.LIB",
    "ADVAPI32.LIB",
)

# These are the byte-matching VC 4.2 artifacts currently used by the project.
# Pin code-generating tools, the CRT, and the core headers; import libraries are
# inventoried in the manifest because their ordering/layout is audited at link time.
PINNED_SHA256 = {
    "bin/CL.EXE": "c5bf7ad84482e8a54d5753fcbd3e648d8a1192f5ca8b8cf1f5d23b651750585f",
    "bin/C1.EXE": "c5a62937d806fbd8663b05f15bd02670a43bdf983a50ee4080bcfd90a7643b90",
    "bin/C1XX.EXE": "9e0782ec157b30a387ca855374bc4c1b8a605dfb12364425497ba431541a5bf9",
    "bin/C2.EXE": "674fb9e410481378c6980c3f21914e513128c85001e8337aca73b587b6273ae9",
    "bin/LINK.EXE": "6ca5a19155e4170e8df08247769b4586fa951743f09f1d8fcec838fc4eb9750e",
    "bin/CVTRES.EXE": "7d66e9e5437b8d983432d8addedd7ea342bb814a34b1ffdebbc30018485004e8",
    "bin/MSPDB41.DLL": "6cab17cfcbc5a6317ab030a0db99164cafdfd1f360baa36186849237ffb25858",
    "include/STDIO.H": "53f3025c95bf31f671313239f7ce6f9b640293933233fc71c319f3629abed60d",
    "include/STDDEF.H": "f3a4d3f7bd77f47e1c3958539a958a499df3eda8b32416544962b8ec62151bce",
    "include/IO.H": "57a249eff9a7a17ffcf17dc166fe96823bc51c865fdcc0d10c7dac36b18ad28d",
    "include/STRING.H": "95a5c5eab27c280cefeb51cddf4ee9442515faac69accd2ff3761ae1d628cbae",
    "lib/LIBCMT.LIB": "1b42eb48c0824798e2585573069f85c9c0b08df45db62ff2cc7a7aa5dd1b5ba2",
    "lib/OLDNAMES.LIB": "2593c14f0b284df06d588bbfbd43d92b77d2ffb82708c026913108e1da0dbf9c",
}


def log(message: str) -> None:
    print(f"[toolchain] {message}", flush=True)


def fail(message: str) -> "NoReturn":
    print(f"[toolchain] ERROR: {message}", file=sys.stderr)
    raise SystemExit(1)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def child_ci(directory: Path, name: str) -> Path | None:
    if not directory.is_dir():
        return None
    wanted = name.casefold()
    return next((entry for entry in directory.iterdir() if entry.name.casefold() == wanted), None)


def file_ci(directory: Path, name: str) -> Path | None:
    path = child_ci(directory, name)
    return path if path is not None and path.is_file() else None


def archive_extract(source: Path, destination: Path, *, prefer_unrar: bool = True) -> None:
    if prefer_unrar and source.suffix.casefold() == ".exe":
        unrar = shutil.which("unrar")
        if unrar is not None:
            listing = subprocess.run(
                [unrar, "lb", str(source)],
                stdout=subprocess.PIPE,
                stderr=subprocess.DEVNULL,
                text=True,
                check=False,
            )
            if listing.stdout.strip():
                destination.mkdir(parents=True, exist_ok=True)
                result = subprocess.run(
                    [unrar, "x", "-idq", "-o+", str(source), f"{destination}{os.sep}"],
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.PIPE,
                    text=True,
                    check=False,
                )
                # The preserved disc1 SFX reports an unexpected end after extracting
                # its complete MSDEV tree. Let structural/hash validation decide.
                if any(destination.iterdir()):
                    if result.returncode != 0:
                        log("unrar returned nonzero after producing files; validating the extracted tree")
                    return

    seven_zip = shutil.which("7z")
    if seven_zip is None:
        fail("7z is required for disc/archive inputs; run inside `nix develop .#build`")
    destination.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(
        [seven_zip, "x", "-y", f"-o{destination}", str(source)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        detail = result.stderr.strip().splitlines()[-1] if result.stderr.strip() else "unknown error"
        fail(f"could not extract {source}: {detail}")


def expand_nested_cabs(root: Path) -> None:
    expanded: set[Path] = set()
    for _round in range(5):
        cabs = [
            path
            for path in root.rglob("*")
            if path.is_file() and path.suffix.casefold() == ".cab" and path not in expanded
        ]
        if not cabs:
            return
        for cab in cabs:
            expanded.add(cab)
            archive_extract(cab, cab.parent / f"{cab.name}.extracted", prefer_unrar=False)


def candidate_roots(search_roots: list[Path]) -> list[Path]:
    candidates: dict[Path, int] = {}
    for search_root in search_roots:
        directories = [search_root]
        directories.extend(path for path in search_root.rglob("*") if path.is_dir())
        for root in directories:
            bin_dir = child_ci(root, "bin")
            include_dir = child_ci(root, "include")
            lib_dir = child_ci(root, "lib")
            if not bin_dir or not include_dir or not lib_dir:
                continue
            if not file_ci(bin_dir, "CL.EXE"):
                continue
            present = sum(file_ci(bin_dir, name) is not None for name in REQUIRED_BIN)
            present += sum(file_ci(include_dir, name) is not None for name in REQUIRED_INCLUDE)
            present += sum(file_ci(lib_dir, name) is not None for name in REQUIRED_LIB)
            candidates[root.resolve()] = present
    return sorted(candidates, key=lambda path: (-candidates[path], len(path.parts), str(path)))


def copy_tree(source_root: Path, destination: Path) -> None:
    for name in ("bin", "include", "lib"):
        source = child_ci(source_root, name)
        if source is None or not source.is_dir():
            fail(f"{source_root} has no {name}/ directory")
        shutil.copytree(source, destination / name)


def required_paths(root: Path) -> dict[str, Path]:
    result: dict[str, Path] = {}
    for directory_name, names in (
        ("bin", REQUIRED_BIN),
        ("include", REQUIRED_INCLUDE),
        ("lib", REQUIRED_LIB),
    ):
        directory = root / directory_name
        for name in names:
            path = file_ci(directory, name)
            if path is None:
                fail(f"required VC 4.2 file is missing: {directory_name}/{name}")
            result[f"{directory_name}/{name}"] = path
    return result


def pinned_mismatches(root: Path) -> list[tuple[str, str, str]]:
    mismatches = []
    for relative, expected in PINNED_SHA256.items():
        directory_name, filename = relative.split("/", 1)
        path = file_ci(root / directory_name, filename)
        actual = sha256(path) if path is not None else "missing"
        if actual != expected:
            mismatches.append((relative, expected, actual))
    return mismatches


def write_manifest(root: Path, source_label: str, pinned: bool) -> None:
    required = required_paths(root)
    file_counts = {
        name: sum(path.is_file() for path in (root / name).rglob("*"))
        for name in ("bin", "include", "lib")
    }
    hashes = {relative: sha256(path) for relative, path in sorted(required.items())}
    for relative in PINNED_SHA256:
        if relative not in hashes:
            directory_name, filename = relative.split("/", 1)
            path = file_ci(root / directory_name, filename)
            if path is not None:
                hashes[relative] = sha256(path)
    manifest = {
        "schema": 1,
        "toolchain": "Microsoft Visual C++ 4.2",
        "matching_profile": "CL 10.20.6166 / LINK 4.20.6164 / CVTRES 4.00",
        "pinned_match": pinned,
        "source": source_label,
        "file_counts": file_counts,
        "sha256": dict(sorted(hashes.items())),
    }
    (root / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="ascii")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Provision build/toolchain/msvc from VC 4.2 media or an installed MSDEV tree.",
        epilog=(
            "Examples:\n"
            "  scripts/make-toolchain.sh /mnt/vc42/MSDEV\n"
            "  scripts/make-toolchain.sh /images/en_vc42ent_disc1.iso /images/en_vc42ent_disc2.iso\n"
            "  scripts/make-toolchain.sh --check build/toolchain/msvc"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("sources", nargs="*", type=Path, help="installed tree, directory, ISO, or archive")
    parser.add_argument("-o", "--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--force", action="store_true", help="replace an existing output tree")
    parser.add_argument(
        "--allow-unpinned",
        action="store_true",
        help="permit a different VC 4.2 artifact set for an explicit A/B experiment",
    )
    parser.add_argument("--check", type=Path, metavar="TREE", help="validate an existing tree without copying")
    return parser.parse_args()


def validate(root: Path, allow_unpinned: bool, source_label: str, write: bool) -> bool:
    required_paths(root)
    mismatches = pinned_mismatches(root)
    if mismatches:
        for relative, expected, actual in mismatches:
            print(f"[toolchain] hash mismatch {relative}\n  expected {expected}\n  actual   {actual}", file=sys.stderr)
        if not allow_unpinned:
            fail("toolchain is not the pinned matching artifact set; use --allow-unpinned only for A/B work")
    if write:
        write_manifest(root, source_label, not mismatches)
    log("validated required compiler, linker, headers, CRT, and system import libraries")
    log("artifact hashes match the pinned toolchain" if not mismatches else "accepted an explicitly unpinned toolchain")
    return not mismatches


def main() -> None:
    args = parse_args()
    if args.check is not None:
        if args.sources:
            fail("--check does not accept source arguments")
        root = args.check.expanduser().resolve()
        validate(root, args.allow_unpinned, root.name, write=False)
        return

    if not args.sources:
        fail("provide an installed MSDEV tree or one or more VC 4.2 disc/archive images")

    sources = [path.expanduser().resolve() for path in args.sources]
    for source in sources:
        if not source.exists():
            fail(f"source does not exist: {source}")

    output = args.output.expanduser().resolve()
    if output.exists() and not args.force:
        fail(f"output already exists: {output}; validate it with --check or replace it with --force")
    output.parent.mkdir(parents=True, exist_ok=True)

    with tempfile.TemporaryDirectory(prefix="homm2-vc42-") as temporary:
        temporary_root = Path(temporary)
        search_roots = []
        for index, source in enumerate(sources):
            if source.is_dir():
                search_roots.append(source)
                continue
            extracted = temporary_root / f"media-{index:02d}"
            log(f"extracting {source.name}")
            archive_extract(source, extracted)
            expand_nested_cabs(extracted)
            search_roots.append(extracted)

        candidates = candidate_roots(search_roots)
        if not candidates:
            cl_locations = []
            for root in search_roots:
                cl_locations.extend(
                    str(path) for path in root.rglob("*") if path.is_file() and path.name.casefold() == "cl.exe"
                )
            detail = "\n  ".join(cl_locations[:10]) or "none"
            fail(
                "no complete BIN/INCLUDE/LIB tree containing CL.EXE was found. "
                "This media may require its original installer; install it into a temporary "
                f"prefix and pass the resulting MSDEV directory. CL.EXE locations:\n  {detail}"
            )

        source_root = candidates[0]
        log(f"using VC tree {source_root}")
        staging = Path(tempfile.mkdtemp(prefix=f".{output.name}.stage-", dir=output.parent))
        try:
            copy_tree(source_root, staging)
            validate(staging, args.allow_unpinned, source_root.name, write=True)
            if output.exists():
                shutil.rmtree(output)
            os.replace(staging, output)
        finally:
            if staging.exists():
                shutil.rmtree(staging)

    log(f"provisioned {output}")


if __name__ == "__main__":
    main()
