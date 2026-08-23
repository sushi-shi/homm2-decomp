#!/usr/bin/env python3
"""Produce the retail image directly with untouched VC6 LINK.EXE.

LINK writes the final executable.  Nothing in this module opens or rewrites that
executable afterward.  The remaining prepared COFF inputs are deliberately
isolated here so each can be removed as its original source/library ownership is
recovered.
"""

from __future__ import annotations

import hashlib
import os
import shutil
import subprocess
from pathlib import Path

from .adapt_misc_data import adapt as adapt_misc_data
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


def run(command: list[str], *, log: Path | None = None) -> None:
    # The historical LINK_TIMES strings are UTC; faketime reads them in the
    # ambient timezone, so pin TZ or the PE/PDB stamps shift with the machine.
    completed = subprocess.run(
        command,
        cwd=ROOT,
        text=True,
        env={**os.environ, "TZ": "UTC0"},
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    if log is not None:
        log.write_text(completed.stdout)
    elif completed.stdout:
        print(completed.stdout, end="")
    if completed.returncode:
        raise RuntimeError(
            f"command failed ({completed.returncode}): {' '.join(command)}"
        )


def prepare_request() -> Path:
    # The cFRDummy backing byte is owned by SEARCH's selectany COMDAT, so the
    # compiled REQUEST object links unmodified.
    output = ROOT / "build/objdiff/base/SOURCE/REQUEST.obj"
    if not output.exists():
        raise RuntimeError(f"REQUEST final-link object is missing: {output}")
    return output


def prepare_misc_prefix_library() -> Path:
    source_library = LINK_ROOT / "BASE-prefix.lib"
    members = subprocess.check_output(
        ["llvm-ar", "t", source_library], cwd=ROOT, text=True
    ).splitlines()
    selected = []
    seen_misc = 0
    for member in members:
        path = ROOT / member
        if member.replace("\\", "/").endswith("/BASE/Misc.obj"):
            output = LINK_ROOT / "plain-inputs/BASE/Misc.obj"
            output.parent.mkdir(parents=True, exist_ok=True)
            output.write_bytes(adapt_misc_data(path.read_bytes()))
            path = output
            seen_misc += 1
        selected.append(relative(path))
    if seen_misc != 1:
        raise RuntimeError(f"expected one Misc member in {source_library}, got {seen_misc}")

    output = LINK_ROOT / "plain-inputs/BASE-prefix.lib"
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
    run(["wine", str(LIB_EXE), "@" + relative(response)])
    return output


def final_inputs(
    configured: list[str], request: Path, base_prefix: Path
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
        relative(base_prefix),
        "build/link/Midi.lib",
        "build/link/BASE-suffix.lib",
        "MSVCPRT.LIB",
        "LIBCMT.LIB",
        "build/link/HMM2PL.res",
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


def main() -> int:
    for tool in (LINK_EXE, LIB_EXE, LIBCMT, MSVCPRT, RETAIL):
        if not tool.exists():
            raise RuntimeError(f"required exact-link input is missing: {tool}")
    if hashlib.sha256(RETAIL.read_bytes()).hexdigest() != RETAIL_SHA256:
        raise RuntimeError("build/orig/HMM2PL.exe is not the supported Buka retail image")
    faketime = shutil.which("faketime")
    if faketime is None:
        raise RuntimeError("faketime is required; enter `nix develop .#build`")

    request = prepare_request()
    base_prefix = prepare_misc_prefix_library()
    inputs = final_inputs(ninja_link_args(), request, base_prefix)
    prepare_historical_pdb()

    output = LINK_ROOT / "HMM2PL.exe"
    map_path = LINK_ROOT / "HMM2PL.map"
    response = LINK_ROOT / "HMM2PL.rsp"
    prefix = [
        "/NOLOGO",
        "/MACHINE:IX86",
        "/BASE:0x400000",
        "/SUBSYSTEM:WINDOWS,4.0",
        "/STACK:66112,4096",
        "/HEAP:1048576,4096",
        "/INCREMENTAL:NO",
        "/OPT:NOREF",
        "/DEBUG",
        "/PDB:" + PDB_WINDOWS_PATH,
        "/LIBPATH:build/toolchain/msvc/lib",
        "/MAP:" + relative(map_path),
        "/OUT:" + relative(output),
    ]
    response.write_text(" ".join(prefix + inputs) + "\n")
    output.unlink(missing_ok=True)
    map_path.unlink(missing_ok=True)
    for iteration, timestamp in enumerate(LINK_TIMES, 1):
        run(
            [
                faketime,
                "-f",
                timestamp,
                "wine",
                str(LINK_EXE),
                "@" + relative(response),
            ],
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


if __name__ == "__main__":
    raise SystemExit(main())
