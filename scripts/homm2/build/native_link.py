#!/usr/bin/env python3
"""Link raw reconstruction objects with the pinned VC6 LINK.EXE.

The generic link uses source and reviewed import ABI manifests. --rsrc adds
source-compiled resources and the retail-extracted icon. LINK writes the final
executable directly; neither link inputs nor its output receive layout fixes.
"""

from __future__ import annotations

import argparse
import hashlib
import shlex
from pathlib import Path

from homm2.core import wine
from homm2.core.paths import REPO as ROOT

LINK_ROOT = ROOT / "build/link"
TOOLCHAIN = ROOT / "build/toolchain/msvc"
LINK_EXE = TOOLCHAIN / "bin/LINK.EXE"
LIBCMT = TOOLCHAIN / "lib/LIBCMT.LIB"
MSVCPRT = TOOLCHAIN / "lib/MSVCPRT.LIB"
RETAIL = ROOT / "build/orig/HMM2PL.exe"
RETAIL_SHA256 = "bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a"


def relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def ninja_link_args() -> list[str]:
    """Read the configured raw-input edge; this never reads an object or image."""
    text = (ROOT / "build.ninja").read_text().replace("$\n", " ")
    for block in text.split("\nbuild "):
        if block.startswith("build/link/generic/HMM2PL.exe "):
            for line in block.splitlines():
                if line.startswith("  link_args = "):
                    return shlex.split(line.removeprefix("  link_args = "))
    raise RuntimeError("configured generic link edge has no link_args")


def final_inputs(configured: list[str], *, include_resources: bool) -> list[str]:
    first_source = next((i for i, path in enumerate(configured)
                         if path.startswith("build/objdiff/base/")), None)
    first_base = configured.index("build/link/BASE-prefix.lib")
    expected_tail = [
        "build/link/BASE-prefix.lib", "build/link/Midi.lib",
        "build/link/BASE-suffix.lib", "LIBCMT.LIB", "build/link/HMM2PL.res",
    ]
    if configured[first_base:] != expected_tail:
        raise RuntimeError("unexpected configured final-link tail")
    if first_source is None or first_source >= first_base:
        raise RuntimeError("configured link has no raw project objects")
    sources = configured[first_source:first_base]
    if any(not path.startswith("build/objdiff/base/") or not path.endswith(".obj")
           for path in sources):
        raise RuntimeError("configured link must use raw project objects")
    libraries = configured[:first_source]
    if any(not path.lower().endswith(".lib") for path in libraries):
        raise RuntimeError("configured link must use raw project objects")
    return [
        "/NODEFAULTLIB:LIBCMT", "/NODEFAULTLIB:LIBCPMT", "/NODEFAULTLIB:OLDNAMES",
        *sources, "OLDNAMES.LIB", *libraries,
        *expected_tail[:3], "MSVCPRT.LIB", "LIBCMT.LIB",
        *(["build/link/HMM2PL.res"] if include_resources else []),
    ]


def link_prefix(output: Path, map_path: Path, pdb: str) -> list[str]:
    return [
        "/NOLOGO",
        "/MACHINE:IX86",
        "/BASE:0x400000",
        "/SUBSYSTEM:WINDOWS,4.0",
        "/STACK:66112,4096",
        "/HEAP:1048576,4096",
        "/INCREMENTAL:NO",
        "/OPT:NOREF",
        "/DEBUG",
        "/PDB:" + pdb,
        "/LIBPATH:build/toolchain/msvc/lib",
        "/MAP:" + relative(map_path),
        "/OUT:" + relative(output),
    ]


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rsrc", action="store_true",
                        help="add reconstructed resources and the retail-extracted icon")
    args = parser.parse_args(argv)
    mode = "rsrc" if args.rsrc else "generic"
    required = (LINK_EXE, LIBCMT, MSVCPRT)
    if args.rsrc:
        required += (RETAIL,)
    for tool in required:
        if not tool.exists():
            raise RuntimeError(f"required {mode}-link input is missing: {tool}")
    if args.rsrc and hashlib.sha256(RETAIL.read_bytes()).hexdigest() != RETAIL_SHA256:
        raise RuntimeError("build/orig/HMM2PL.exe is not the supported Buka retail image")
    inputs = final_inputs(ninja_link_args(), include_resources=args.rsrc)
    mode_root = LINK_ROOT / mode
    mode_root.mkdir(parents=True, exist_ok=True)
    output = mode_root / "HMM2PL.exe"
    map_path = mode_root / "HMM2PL.map"
    response = mode_root / "HMM2PL.rsp"
    prefix = link_prefix(output, map_path, relative(mode_root / "HMM2PL.pdb"))
    response.write_text(" ".join(prefix + inputs) + "\n")
    output.unlink(missing_ok=True)
    map_path.unlink(missing_ok=True)
    wine.run(LINK_EXE, "@" + relative(response),
             cwd=ROOT, log=mode_root / "HMM2PL.link.log")
    if not output.exists():
        raise RuntimeError(f"{mode} LINK produced no executable; see {mode_root}/HMM2PL.link.log")
    digest = hashlib.sha256(output.read_bytes()).hexdigest()
    print(f"{mode} LINK.EXE output: {relative(output)} "
          f"({output.stat().st_size} bytes, sha256={digest})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
