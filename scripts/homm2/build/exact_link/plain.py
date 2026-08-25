#!/usr/bin/env python3
"""Link HMM2PL.exe with untouched VC6 LINK.EXE, in one of three modes.

  generic      (default)  raw compiled objects and checked-in ABI manifests:
                          no retail executable, resources, or COFF transforms.
                          One LINK pass with an ordinary PDB under
                          build/link/generic/.
  --rsrc                  generic plus reconstructed .rsrc resources; opens
                          build/orig/HMM2PL.exe for the program icon and the
                          full resource gate. Code and data still come from
                          this codebase.
                          Output under build/link/rsrc/.
  --transform             --rsrc plus four reviewed COFF transforms, the
                          historical four-pass PDB link, and the retail
                          SHA-256 assertion. Output at build/link/HMM2PL.exe,
                          byte-identical to retail.

LINK writes the final executable in every mode. Nothing in this module opens
or rewrites that executable afterward.
"""

from __future__ import annotations

import argparse
import hashlib
import os
import subprocess
import sys
from pathlib import Path

from homm2.core import wine

from . import transforms
from .crt_order import ninja_link_args


ROOT = next(
    path for path in Path(__file__).resolve().parents if (path / "flake.nix").exists()
)
LINK_ROOT = ROOT / "build/link"
TOOLCHAIN = ROOT / "build/toolchain/msvc"
LINK_EXE = TOOLCHAIN / "bin/LINK.EXE"
LIB_EXE = TOOLCHAIN / "bin/LIB.EXE"
LIBCMT = TOOLCHAIN / "lib/LIBCMT.LIB"
MSVCPRT = TOOLCHAIN / "lib/MSVCPRT.LIB"
RETAIL = ROOT / "build/orig/HMM2PL.exe"
RETAIL_SHA256 = "bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a"
GENERIC_IMPORT_LIBRARIES = {
    "build/link/audiere.lib": "build/link/generic-imports/audiere.lib",
    "build/link/mss32.lib": "build/link/generic-imports/mss32.lib",
    "build/link/smackw32.lib": "build/link/generic-imports/smackw32.lib",
    "build/link/netapi32.lib": "build/link/generic-imports/netapi32.lib",
}
PDB_WINDOWS_PATH = r"e:\Users\igorl\VSS\HMM\HMM2\temp\release\game\HMM2PL.pdb"
PDB_RELATIVE_PATH = Path("Users/igorl/VSS/HMM/HMM2/temp/release/game/HMM2PL.pdb")
LINK_TIMES = (
    "@2003-02-26 14:51:33",
    "@2003-04-04 08:19:23",
    "@2003-04-04 08:19:23",
    "@2003-04-04 08:19:23",
)


def relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def prepare_request(apply_transform: bool) -> Path:
    raw = ROOT / "build/objdiff/base/SOURCE/REQUEST.obj"
    if not raw.exists():
        raise RuntimeError(f"REQUEST final-link object is missing: {raw}")
    if not apply_transform:
        return raw

    # VC6 emits cFRDummy's file-scope empty-string cell before REQUEST's five
    # function-body empty strings. Retail carries the opposite private-cell
    # order. Keep that compiler-layout accommodation out of reconstructed C++.
    subprocess.run(
        (
            sys.executable,
            "-m",
            "homm2.build.exact_link.batch_bss",
            "--owner",
            "SOURCE",
            "--unit",
            r"SOURCE\REQUEST.c",
            "--previous-end",
            "0x133d78",
        ),
        cwd=ROOT,
        check=True,
    )
    output = LINK_ROOT / "bss-layout-all/SOURCE/REQUEST.obj"
    if not output.exists():
        raise RuntimeError(f"REQUEST transformed object was not produced: {output}")
    return output


def rebuild_library_with_transforms(source_library: Path, expect: set[str]) -> Path:
    """Rebuild one ninja archive, substituting the reviewed transformed copy
    for each member that exact_link/transforms.py names."""
    members = subprocess.check_output(
        ["llvm-ar", "t", source_library], cwd=ROOT, text=True
    ).splitlines()
    selected = []
    replaced = set()
    for member in members:
        member = member.replace("\\", "/")
        path = ROOT / member
        unit = member.removeprefix("build/objdiff/base/").removesuffix(".obj")
        if unit in transforms.TRANSFORMS:
            output = LINK_ROOT / "plain-inputs" / f"{unit}.obj"
            output.parent.mkdir(parents=True, exist_ok=True)
            output.write_bytes(transforms.apply(unit, path.read_bytes()))
            path = output
            replaced.add(unit)
        selected.append(relative(path))
    if replaced != expect:
        raise RuntimeError(
            f"{source_library.name}: transformed {sorted(replaced)}, "
            f"expected {sorted(expect)}"
        )

    output = LINK_ROOT / "plain-inputs" / source_library.name
    output.parent.mkdir(parents=True, exist_ok=True)
    output.unlink(missing_ok=True)
    response = output.with_suffix(".lib.rsp")
    response.write_text(
        "/NOLOGO /MACHINE:IX86 /OUT:"
        + relative(output)
        + " "
        + " ".join(reversed(selected))
        + "\n"
    )
    wine.run(LIB_EXE, "@" + relative(response), cwd=ROOT)
    print(f"[link] {source_library.name}: transformed members {sorted(replaced)}")
    return output


def final_inputs(
    configured: list[str],
    request: Path,
    base_prefix: str,
    base_suffix: str,
    include_resources: bool,
    use_generic_imports: bool,
) -> list[str]:
    first_source = configured.index("build/objdiff/base/SOURCE/ADVMGR.obj")
    first_base = configured.index("build/link/BASE-prefix.lib")
    expected_tail = [
        "build/link/BASE-prefix.lib",
        "build/link/Midi.lib",
        "build/link/BASE-suffix.lib",
        "LIBCMT.LIB",
        "build/link/HMM2PL.res",
    ]
    if configured[first_base:] != expected_tail:
        raise RuntimeError(f"unexpected configured final-link tail: {configured[first_base:]}")

    libraries = configured[:first_source]
    if use_generic_imports:
        libraries = [GENERIC_IMPORT_LIBRARIES.get(path, path) for path in libraries]
    sources = configured[first_source:first_base]
    request_raw = "build/objdiff/base/SOURCE/REQUEST.obj"
    if sources.count(request_raw) != 1:
        raise RuntimeError("configured link does not contain one raw REQUEST object")
    sources[sources.index(request_raw)] = relative(request)
    return [
        "/NODEFAULTLIB:LIBCMT",
        "/NODEFAULTLIB:LIBCPMT",
        "/NODEFAULTLIB:OLDNAMES",
        *sources,
        "OLDNAMES.LIB",
        *libraries,
        base_prefix,
        "build/link/Midi.lib",
        base_suffix,
        "MSVCPRT.LIB",
        "LIBCMT.LIB",
        *(["build/link/HMM2PL.res"] if include_resources else []),
    ]


def prepare_historical_pdb() -> Path:
    wineprefix = Path(os.environ.get("WINEPREFIX", ROOT / "build/wineprefix"))
    drive = wineprefix / "dosdevices/e:"
    target = LINK_ROOT / "historical-drive-e"
    target.mkdir(parents=True, exist_ok=True)
    if drive.is_symlink():
        if drive.resolve() != target.resolve():
            raise RuntimeError(f"Wine E: already maps to {drive.resolve()}, expected {target}")
    elif drive.exists():
        raise RuntimeError(f"Wine E: exists and is not a symlink: {drive}")
    else:
        drive.parent.mkdir(parents=True, exist_ok=True)
        drive.symlink_to(target)
    pdb = target / PDB_RELATIVE_PATH
    pdb.parent.mkdir(parents=True, exist_ok=True)
    pdb.unlink(missing_ok=True)
    return pdb


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


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--rsrc", action="store_true",
        help="link reconstructed .rsrc resources plus the retail program icon",
    )
    parser.add_argument(
        "--transform", action="store_true",
        help="apply the reviewed COFF transforms and assert the retail SHA-256",
    )
    args = parser.parse_args()
    mode = "transform" if args.transform else ("rsrc" if args.rsrc else "generic")

    required = (LINK_EXE, LIB_EXE, LIBCMT, MSVCPRT)
    if mode in ("rsrc", "transform"):
        required += (RETAIL,)
    for tool in required:
        if not tool.exists():
            raise RuntimeError(f"required {mode}-link input is missing: {tool}")
    if mode in ("rsrc", "transform") and (
        hashlib.sha256(RETAIL.read_bytes()).hexdigest() != RETAIL_SHA256
    ):
        raise RuntimeError("build/orig/HMM2PL.exe is not the supported Buka retail image")

    request = prepare_request(mode == "transform")
    if mode == "transform":
        base_prefix = relative(rebuild_library_with_transforms(
            LINK_ROOT / "BASE-prefix.lib", {"BASE/Misc"}))
        base_suffix = relative(rebuild_library_with_transforms(
            LINK_ROOT / "BASE-suffix.lib", {"BASE/AudiereEffects", "BASE/DIMMER"}))
    else:
        base_prefix = "build/link/BASE-prefix.lib"
        base_suffix = "build/link/BASE-suffix.lib"
    inputs = final_inputs(
        ninja_link_args(), request, base_prefix, base_suffix,
        include_resources=mode in ("rsrc", "transform"),
        use_generic_imports=mode != "transform",
    )

    if mode == "transform":
        prepare_historical_pdb()
        output = LINK_ROOT / "HMM2PL.exe"
        map_path = LINK_ROOT / "HMM2PL.map"
        response = LINK_ROOT / "HMM2PL.rsp"
        prefix = link_prefix(output, map_path, PDB_WINDOWS_PATH)
        response.write_text(" ".join(prefix + inputs) + "\n")
        output.unlink(missing_ok=True)
        map_path.unlink(missing_ok=True)
        for iteration, timestamp in enumerate(LINK_TIMES, 1):
            wine.run(
                LINK_EXE, "@" + relative(response),
                cwd=ROOT, faketime_spec=timestamp,
                log=LINK_ROOT / f"HMM2PL.link-{iteration}.log",
            )
        digest = hashlib.sha256(output.read_bytes()).hexdigest()
        if digest != RETAIL_SHA256:
            raise RuntimeError(
                f"plain LINK output differs from retail: sha256={digest}, "
                f"expected={RETAIL_SHA256}"
            )
        print(f"plain LINK.EXE output is retail-exact: {digest}")
        return 0

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
    print(
        f"{mode} LINK.EXE output: {relative(output)} "
        f"({output.stat().st_size} bytes, sha256={digest})"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
