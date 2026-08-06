#!/usr/bin/env python3
"""Synthesize vendor import libraries through throwaway stub DLLs.

``LIB /DEF`` cannot represent an already-decorated DLL export faithfully: using
the retail spelling adds an extra underscore to the public import symbol, while
dropping that underscore changes the name stored in ``.idata$6``.  Compile real
stub exports from reviewed module-definition files instead and keep the import
library emitted by VC6 LINK.EXE.  The stub DLL, EXP, object, and generated C
source are disposable build products.
"""

from __future__ import annotations

import argparse
import os
import re
import sys
import tempfile
from pathlib import Path

from homm2.build.cc_wrap import (
    _run_cl,
    ensure_wineserver,
    find_ci,
    msvc_dir,
    winepath_w,
)
from homm2.build.link_exe import RETAIL_EXE, read_imports


STDCALL = re.compile(r"^_(?P<name>[A-Za-z_][A-Za-z0-9_]*)@(?P<bytes>\d+)$")
PLAIN = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def imported_names(exe: Path, dll: str) -> list[str]:
    """Return the exact named imports for ``dll`` in retail IAT order."""
    imports = read_imports(exe)
    entry = next((row for row in imports if row["dll"].lower() == dll.lower()), None)
    if entry is None:
        raise ValueError(f"{dll} is absent from the retail import table")
    ordinal_only = [row["ordinal"] for row in entry["symbols"] if "ordinal" in row]
    if ordinal_only:
        raise ValueError(
            f"{dll} contains ordinal-only imports {ordinal_only}; "
            "a C stub cannot recover their source export names"
        )
    return [row["name"] for row in entry["symbols"]]


def export_names(path: Path, dll: str | None = None) -> list[str]:
    """Read the complete named-export surface from a strict module definition."""
    library = None
    in_exports = False
    names = []
    for number, raw_line in enumerate(
        path.read_text(encoding="ascii").splitlines(), start=1
    ):
        line = raw_line.split(";", 1)[0].strip()
        if not line:
            continue
        keyword, _, value = line.partition(" ")
        if keyword.upper() == "LIBRARY":
            if library is not None or not value.strip():
                raise ValueError(f"{path}:{number}: invalid LIBRARY directive")
            library = value.strip()
            continue
        if keyword.upper() == "EXPORTS" and not value.strip():
            in_exports = True
            continue
        if not in_exports:
            raise ValueError(f"{path}:{number}: expected LIBRARY or EXPORTS")
        if len(line.split()) != 1 or "=" in line:
            raise ValueError(
                f"{path}:{number}: only one literal export name is supported"
            )
        names.append(line)

    if library is None:
        raise ValueError(f"{path} has no LIBRARY directive")
    if dll is not None and library.lower() != dll.lower():
        raise ValueError(f"{path} defines {library}, expected {dll}")
    if not in_exports or not names:
        raise ValueError(f"{path} has no named EXPORTS")
    duplicates = sorted({name for name in names if names.count(name) > 1})
    if duplicates:
        raise ValueError(f"{path} contains duplicate exports: {duplicates}")
    return names


def validate_export_hints(exe: Path, dll: str, exports: list[str]) -> None:
    """Prove that a complete export table reproduces retail's named hints."""
    imports = read_imports(exe)
    entry = next((row for row in imports if row["dll"].lower() == dll.lower()), None)
    if entry is None:
        raise ValueError(f"{dll} is absent from the retail import table")
    indices = {name: index for index, name in enumerate(sorted(exports))}
    mismatches = []
    for row in entry["symbols"]:
        if "name" not in row:
            continue
        actual = indices.get(row["name"])
        if actual != row["hint"]:
            mismatches.append((row["name"], row["hint"], actual))
    if mismatches:
        details = ", ".join(
            f"{name}: retail {expected}, table {actual}"
            for name, expected, actual in mismatches
        )
        raise ValueError(f"{dll} export table does not reproduce retail hints: {details}")


def stub_source(dll: str, names: list[str]) -> str:
    """Generate C definitions whose VC6 export decoration equals retail."""
    lines = [
        f"/* Generated throwaway exports for {dll}. */",
        "/* The retail PE import names are authoritative. */",
    ]
    for name in sorted(names):
        match = STDCALL.fullmatch(name)
        if match:
            nargs, remainder = divmod(int(match.group("bytes")), 4)
            if remainder:
                raise ValueError(
                    f"{dll}: {name} has a non-dword stdcall argument size"
                )
            arguments = ", ".join(f"int a{index}" for index in range(nargs)) or "void"
            lines.append(
                "__declspec(dllexport) void __stdcall "
                f"{match.group('name')}({arguments}) {{}}"
            )
        elif PLAIN.fullmatch(name):
            lines.append(f"__declspec(dllexport) void {name}(void) {{}}")
        else:
            raise ValueError(f"{dll}: cannot synthesize export {name!r}")
    return "\n".join(lines) + "\n"


def _run(command: list[str], expected: Path, label: str) -> None:
    output, returncode, _timed_out = _run_cl(command, expected)
    if returncode or not expected.exists():
        tail = "\n".join(output.splitlines()[-12:])
        raise RuntimeError(f"{label} failed\n{tail}")


def synthesize(
    exe: Path,
    dll: str,
    output: Path,
    definition_path: Path | None = None,
) -> Path:
    """Create ``output`` via VC6's linker-generated import library."""
    exe = exe.resolve()
    output = output.resolve()
    imports = imported_names(exe, dll)
    names = (
        export_names(definition_path, dll)
        if definition_path is not None
        else imports
    )
    if definition_path is not None:
        validate_export_hints(exe, dll, names)
    toolchain = msvc_dir()
    compiler = find_ci(toolchain / "bin", "cl.exe")
    linker = find_ci(toolchain / "bin", "link.exe")
    if compiler is None or linker is None:
        raise RuntimeError(
            f"CL.EXE/LINK.EXE not found under {toolchain}/bin; "
            "run inside `nix develop .#build`"
        )

    output.parent.mkdir(parents=True, exist_ok=True)
    os.environ.setdefault("WINEDEBUG", "fixme-all,err-all")
    ensure_wineserver()
    with tempfile.TemporaryDirectory(prefix="homm2-import-lib-") as temp_name:
        temp = Path(temp_name)
        stem = Path(dll).stem
        source = temp / f"{stem}_stub.c"
        obj = temp / f"{stem}_stub.obj"
        stub_dll = temp / dll
        implib = temp / f"{stem}.lib"
        source.write_text(stub_source(dll, names), encoding="ascii")

        _run(
            [
                "wine",
                str(compiler),
                "/nologo",
                "/c",
                f"/Fo{winepath_w(obj)}",
                winepath_w(source),
            ],
            obj,
            f"{dll} stub compile",
        )
        _run(
            [
                "wine",
                str(linker),
                "/NOLOGO",
                "/DLL",
                "/NOENTRY",
                "/NODEFAULTLIB",
                f"/OUT:{winepath_w(stub_dll)}",
                f"/IMPLIB:{winepath_w(implib)}",
                winepath_w(obj),
            ],
            implib,
            f"{dll} stub link",
        )
        output.write_bytes(implib.read_bytes())
    print(
        f"[import-lib] {dll}: {len(imports)} retail imports, "
        f"{len(names)} stub exports -> {output}"
    )
    return output


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path, default=RETAIL_EXE)
    parser.add_argument("--dll", required=True)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument(
        "--definition",
        type=Path,
        help="reviewed complete module definition used to reproduce retail hints",
    )
    args = parser.parse_args(argv)
    try:
        synthesize(args.exe, args.dll, args.out, args.definition)
    except (OSError, RuntimeError, ValueError) as error:
        print(f"[import-lib] ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
