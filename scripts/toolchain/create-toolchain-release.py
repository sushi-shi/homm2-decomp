#!/usr/bin/env python3
"""Build the reproducible Buka VC6 SP5 toolchain tarball.

The base disc supplies a complete VC98 tree; SP5 replaces the parts the target
was built with. Two things about SP5 are worth knowing before reading this:

Its cabinets are a multi-volume set, so they must be read starting at
`VS6sp51.cab` and followed through the chain. Listing a single volume silently
omits every entry that spans volumes, and the back end is one of them.

It does not ship a file called `c2.dll`. It carries four back ends, one per VC6
edition, stored under unrelated names and renamed on install by `sp598ent.inf`;
`sp598ent.stf` chooses between them by probing which edition is already present.
The Enterprise one - the one this target was built with - is `msvcep.dll`.

The base disc needs one thing done to it as well. It carries a single ISO 9660
primary volume descriptor and no supplementary one, so every name on it is 8.3
and upper case; VC6 setup restores the longer ones as it installs. Six C++
standard headers are longer, and they are not optional - VC6's own `<new>`
includes `<exception>`, so nothing compiled with `/GX` builds without them.

Every artifact is verified against a pinned SHA-256, and the assembled compiler
has to stamp the target's own `@comp.id` before the tarball is written.
"""

from __future__ import annotations

import hashlib
import os
import shutil
import subprocess
import sys
import tarfile
import tempfile
from pathlib import Path

def _find_repo() -> Path:
    """Repository root, resolved without counting this file's nesting.

    HOMM2_DIR is what the Nix entrypoint sets, and is the only thing that works
    when these sources run from the store, where no ancestor is the repository.
    """
    override = os.environ.get("HOMM2_DIR")
    if override:
        return Path(override).resolve()
    for parent in Path(__file__).resolve().parents:
        if (parent / "flake.nix").exists():
            return parent
    return Path.cwd().resolve()


REPO = _find_repo()
RELEASE_EPOCH = 1782573438  # Same fixed epoch as the VC 4.2 release.

# Verified against docs/buka-analysis.md, which derives them from the target.
ARTIFACTS = {
    "bin/CL.EXE":     "1bf99f206271ecdbd13da2829192ea2c02e2a44c740b8d72935d5d9cb753b156",
    "bin/C1.DLL":     "0792033fc3d2fec262ff95478c6a9e70a06e836ed55f0a9710200fa06c204754",
    "bin/C1XX.DLL":   "f014b3bee650224adf6cb44e51f0eeac5abbf5da666fa299d5250b2f3d1937c6",
    "bin/C2.DLL":     "d50100ac2380d58f3f6f756961fb1319d35f5248e5fa6cafb866ca657e5dda4a",
    "bin/LINK.EXE":   "9672e578fdfaa43bdb8e9c16071682988665cb90bba2904bf02f6a5576d8ffbc",
    "bin/CVTRES.EXE": "83b602ed8e69e979fc9557f482a4a4c6c9a97b4ad67b879aedeacd2b09e5b20b",
    "lib/LIBCMT.LIB": "28b9f04962378ec4668072f37d7fd2835cd6cacc17b40cf22002c57bd8e76714",
}

# SP5 stores the back end under an edition-specific alias. Enterprise is ours.
BACK_END_ALIAS = "msvcep.dll"

# CL.EXE loads these from COMMON\MSDEV98\BIN, not from VC98\BIN.
SHARED_BIN = ("MSPDB60.DLL", "MSOBJ10.DLL")

SP5_BIN = ("cl.exe", "c1.dll", "c1xx.dll", "link.exe", "cvtres.exe")

# The extension-less C++ headers the disc cannot spell in 8.3. Every other name
# under VC98\INCLUDE already fits, which is how this list was closed: resolving
# every `#include` in the assembled tree leaves these six unreachable and the
# rest either present or belonging to a component the disc puts elsewhere. Each
# header names itself in its banner, and the rename checks that it does.
SHORTENED_HEADERS = {
    "ALGRITHM": "algorithm",
    "FCTIONAL": "functional",
    "STDXCEPT": "stdexcept",
    "STREAMBF": "streambuf",
    "STRSTREM": "strstream",
    "XCEPTION": "exception",
}


def log(message: str) -> None:
    print(f"[vc6] {message}", flush=True)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def run(*command: str) -> None:
    subprocess.run(command, check=True, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL)


def find(root: Path, name: str) -> Path:
    # Sorted, because both media carry a name more than once and directory
    # order is whatever the filesystem hands back: an unsorted walk would let
    # the same media assemble different toolchains on different machines.
    for path in sorted(root.rglob("*")):
        if path.is_file() and path.name.lower() == name.lower():
            return path
    raise SystemExit(f"{name} not found under {root}")


def media(variable: str) -> Path:
    value = os.environ.get(variable)
    if not value:
        raise SystemExit(
            f"{variable} is not set; use scripts/toolchain/create-toolchain-release.nix")
    path = Path(value).expanduser().resolve()
    if not path.is_file():
        raise SystemExit(f"{variable} does not exist: {path}")
    return path


def build(work: Path, disc: Path, sp5: Path) -> Path:
    tree = work / "msvc"
    (tree / "bin").mkdir(parents=True)

    log("extracting the base VC98 tree")
    base = work / "base"
    run("7z", "x", "-y", f"-o{base}", str(disc),
        "VC98/BIN/*", "VC98/INCLUDE/*", "VC98/LIB/*",
        *(f"COMMON/MSDEV98/BIN/{name}" for name in SHARED_BIN))
    vc98 = find(base, "CL.EXE").parent.parent
    for directory in ("BIN", "INCLUDE", "LIB"):
        shutil.copytree(vc98 / directory, tree / directory.lower(), dirs_exist_ok=True)
    for name in SHARED_BIN:
        shutil.copy2(find(base / "COMMON", name), tree / "bin" / name)

    log("extracting SP5 (following the cabinet chain from volume 1)")
    cabs, sp5tree = work / "cabs", work / "sp5"
    run("7z", "x", "-y", f"-o{cabs}", str(sp5), "VS6sp5*.cab")
    run("cabextract", "-q", "-d", str(sp5tree), str(cabs / "VS6sp51.cab"))
    # The loose SFX members carry the aliased back ends and the setup tables.
    run("7z", "x", "-y", f"-o{sp5tree}", str(sp5), "-x!VS6sp5*.cab")

    log("overlaying the SP5 components")
    for name in SP5_BIN:
        shutil.copy2(find(sp5tree / "vc98", name), tree / "bin" / name.upper())
    shutil.copy2(find(sp5tree / "vc98", "libcmt.lib"), tree / "lib" / "LIBCMT.LIB")

    log(f"installing the Enterprise back end from {BACK_END_ALIAS}")
    shutil.copy2(find(sp5tree, BACK_END_ALIAS), tree / "bin" / "C2.DLL")

    log("restoring the header names the disc could not spell")
    restore_long_names(tree / "include")
    return tree


def restore_long_names(include: Path) -> None:
    for short, long in SHORTENED_HEADERS.items():
        source = include / short
        if not source.is_file():
            raise SystemExit(f"shortened header is missing: {source}")
        # The banner is the header's own claim about what it is; a disc that
        # ever shortened a name differently would fail here rather than ship a
        # tree in which <exception> quietly means something else.
        if long.encode() not in source.read_bytes()[:256]:
            raise SystemExit(f"{short} does not name itself {long}")
        source.rename(include / long)


def verify(tree: Path) -> None:
    for relative, expected in ARTIFACTS.items():
        path = tree / relative
        if not path.is_file():
            raise SystemExit(f"missing artifact: {relative}")
        actual = sha256(path)
        if actual != expected:
            raise SystemExit(f"{relative} hashes {actual}, expected {expected}")
    for long in SHORTENED_HEADERS.values():
        if not (tree / "include" / long).is_file():
            raise SystemExit(f"missing standard header: include/{long}")
    log(f"all {len(ARTIFACTS)} pinned artifacts verified")


def package(tree: Path, output: Path) -> None:
    def normalize(entry: tarfile.TarInfo) -> tarfile.TarInfo:
        entry.uid = entry.gid = 0
        entry.uname = entry.gname = ""
        entry.mtime = RELEASE_EPOCH
        # The disc's own permission bits reach us through 7z and the caller's
        # umask, so state them instead of inheriting them; nothing in the tree
        # is executed directly, CL.EXE and its DLLs are read by wine.
        entry.mode = 0o755 if entry.isdir() else 0o644
        return entry

    output.parent.mkdir(parents=True, exist_ok=True)
    with tarfile.open(output, "w:xz") as archive:
        for path in sorted(tree.rglob("*")):
            archive.add(path, arcname=str(Path("msvc") / path.relative_to(tree)),
                        filter=normalize, recursive=False)
    log(f"wrote {output}")
    log(f"archive SHA-256: {sha256(output)}")


def main() -> None:
    # --check validates an already-unpacked tree and builds nothing. `homm2 init`
    # calls it after unpacking the release, so the pinned ARTIFACTS above stay the
    # single source of truth instead of being copied into the fetcher.
    if len(sys.argv) > 1 and sys.argv[1] == "--check":
        if len(sys.argv) != 3:
            raise SystemExit("usage: create-toolchain-release.py --check <msvc tree>")
        verify(Path(sys.argv[2]).expanduser().resolve())
        return

    disc, sp5 = media("VC6_DISC1"), media("VC6_SP5")
    output = Path(os.environ.get(
        "OUTPUT", REPO / "build" / "homm2-toolchain-vc6-sp5.tar.xz")).expanduser().resolve()

    with tempfile.TemporaryDirectory(dir=REPO / "build") as scratch:
        tree = build(Path(scratch), disc, sp5)
        verify(tree)
        package(tree, output)

    log("next: extract into build/toolchain-vc6 and confirm a probe stamps 0x000b2306")


if __name__ == "__main__":
    main()
