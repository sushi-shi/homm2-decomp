#!/usr/bin/env python3
"""Link raw reconstruction objects with the pinned VC6 LINK.EXE.

The generic link uses source and reviewed import ABI manifests. --rsrc adds
source-compiled resources and the retail-extracted icon. LINK writes the final
executable directly; neither link inputs nor its output receive layout fixes.

The selected image (`homm2 --image`, $HOMM2_IMAGE) chooses the link profile:
its configured Ninja edge, output names, PDB path, stack and historical link
clock. The game reads `build.ninja`; another image reads its own
`build/<image>/build.ninja`, whose `link_args` are already the final inputs.
"""

from __future__ import annotations

import argparse
import hashlib
import os
import shlex
from dataclasses import dataclass
from pathlib import Path

from homm2.tool import wine
from homm2.core.paths import DEFAULT_IMAGE, REPO as ROOT, image_build, image_key, retail_exe

TOOLCHAIN = ROOT / "build/toolchain/msvc"
LINK_EXE = TOOLCHAIN / "bin/LINK.EXE"
LIBCMT = TOOLCHAIN / "lib/LIBCMT.LIB"
MSVCPRT = TOOLCHAIN / "lib/MSVCPRT.LIB"


@dataclass(frozen=True)
class LinkProfile:
    """One image's link contract, every value read from its retail image."""
    exe: str
    pdb_windows_path: str
    # Each historical LINK runs with its clock frozen at one observed UTC
    # second (an absolute libfaketime spec, no '@'): the first link creates
    # the PDB (its NB10 signature), and every link ages it by one.
    link_times: tuple[str, ...]
    # Header reserve/commit options; an empty tuple keeps LINK's defaults.
    memory: tuple[str, ...]
    # The image's resource script, kept with its sources.
    resources: str

    @property
    def stem(self) -> str:
        return self.exe.rsplit(".", 1)[0]

    @property
    def pdb_relative_path(self) -> Path:
        return Path(self.pdb_windows_path.split(":", 1)[1].lstrip("\\").replace("\\", "/"))


PROFILES = {
    # HMM2PL.exe: NB10 signature 2003-02-26 14:51:33, age 4, image stamp
    # 2003-04-04 08:19:23; stack 0x10240/0x1000.
    "game": LinkProfile(
        exe="HMM2PL.exe",
        pdb_windows_path=r"e:\Users\igorl\VSS\HMM\HMM2\temp\release\game\HMM2PL.pdb",
        link_times=("2003-02-26 14:51:33",) + ("2003-04-04 08:19:23",) * 3,
        memory=("/STACK:66112,4096", "/HEAP:1048576,4096"),
        resources="src/SOURCE/HMM2PL.rc"),
    # EDT2PL.exe: a fresh PDB (age 1) whose NB10 signature equals the image
    # stamp 2003-04-04 08:21:00; LINK's default stack and heap (0x100000/0x1000).
    "editor": LinkProfile(
        exe="EDT2PL.exe",
        pdb_windows_path=r"e:\Users\igorl\VSS\HMM\HMM2\temp\release\editor\EDT2PL.pdb",
        link_times=("2003-04-04 08:21:00",),
        memory=(),
        resources="src/EDITOR/EDT2PL.rc"),
}

PROFILE = PROFILES[image_key()]
GAME = image_key() == DEFAULT_IMAGE
LINK_ROOT = image_build() / "link"
RETAIL = retail_exe()
PDB_WINDOWS_PATH = PROFILE.pdb_windows_path
PDB_RELATIVE_PATH = PROFILE.pdb_relative_path
LINK_TIMES = PROFILE.link_times
NINJA_FILE = ROOT / "build.ninja" if GAME else image_build() / "build.ninja"


def relative(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def ninja_link_args() -> list[str]:
    """Read the configured raw-input edge; this never reads an object or image."""
    text = NINJA_FILE.read_text().replace("$\n", " ")
    edge = relative(LINK_ROOT / "generic" / PROFILE.exe)
    for block in text.split("\nbuild "):
        if block.startswith(edge + " "):
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
        *expected_tail[:-2], "MSVCPRT.LIB", "LIBCMT.LIB",
        *(["build/link/HMM2PL.res"] if include_resources else []),
    ]


def image_inputs(configured: list[str], *, include_resources: bool) -> list[str]:
    """Another image's graph writes its final input list; only the resource
    script (always last) depends on the mode."""
    resources = [path for path in configured if path.lower().endswith(".res")]
    if resources != configured[-1:]:
        raise RuntimeError("configured image link must end with its one .res")
    if not any(path.endswith(".obj") for path in configured):
        raise RuntimeError("configured link has no raw project objects")
    return configured[:-1] + (resources if include_resources else [])


def link_prefix(output: Path, map_path: Path, pdb: str) -> list[str]:
    return [
        "/NOLOGO",
        "/MACHINE:IX86",
        "/BASE:0x400000",
        "/SUBSYSTEM:WINDOWS,4.0",
        *PROFILE.memory,
        "/INCREMENTAL:NO",
        "/OPT:NOREF",
        "/DEBUG",
        "/PDB:" + pdb,
        "/LIBPATH:build/toolchain/msvc/lib",
        "/MAP:" + relative(map_path),
        "/OUT:" + relative(output),
    ]


def prepare_historical_pdb() -> Path:
    """Start the observed four-link PDB history using ordinary linker inputs."""
    wineprefix = Path(os.environ.get("WINEPREFIX", ROOT / "build/wineprefix"))
    drive = wineprefix / "dosdevices/e:"
    # One E: for every image (they share the Wine prefix); each image's PDB
    # has its own directory below it.
    target = ROOT / "build/link/historical-drive-e"
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


from homm2.core.usage import logged


@logged
def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rsrc", action="store_true",
                        help="add reconstructed resources and the retail-extracted icon")
    parser.add_argument("--historical", action="store_true",
                        help="include resources and reproduce the observed PDB path and link times")
    args = parser.parse_args(argv)
    mode = "historical" if args.historical else ("rsrc" if args.rsrc else "generic")
    include_resources = args.rsrc or args.historical
    required = (LINK_EXE, LIBCMT, MSVCPRT)
    if include_resources:
        required += (RETAIL,)
    for tool in required:
        if not tool.exists():
            raise RuntimeError(f"required {mode}-link input is missing: {tool}")
    if include_resources:
        from homm2.core.retail import verify_retail
        verify_retail(RETAIL, image_key())
    select = final_inputs if GAME else image_inputs
    inputs = select(ninja_link_args(), include_resources=include_resources)
    mode_root = LINK_ROOT / mode
    mode_root.mkdir(parents=True, exist_ok=True)
    stem = PROFILE.stem
    output = mode_root / PROFILE.exe
    map_path = mode_root / f"{stem}.map"
    response = mode_root / f"{stem}.rsp"
    if args.historical:
        prepare_historical_pdb()
    pdb = PDB_WINDOWS_PATH if args.historical else relative(mode_root / f"{stem}.pdb")
    prefix = link_prefix(output, map_path, pdb)
    response.write_text(" ".join(prefix + inputs) + "\n")
    output.unlink(missing_ok=True)
    map_path.unlink(missing_ok=True)
    if args.historical:
        for iteration, timestamp in enumerate(LINK_TIMES, 1):
            wine.run(LINK_EXE, "@" + relative(response), cwd=ROOT,
                     faketime_spec=timestamp, log=mode_root / f"{stem}.link-{iteration}.log")
    else:
        wine.run(LINK_EXE, "@" + relative(response),
                 cwd=ROOT, log=mode_root / f"{stem}.link.log")
    if not output.exists():
        raise RuntimeError(f"{mode} LINK produced no executable; see {mode_root}/{stem}.link.log")
    digest = hashlib.sha256(output.read_bytes()).hexdigest()
    print(f"{mode} LINK.EXE output: {relative(output)} "
          f"({output.stat().st_size} bytes, sha256={digest})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
