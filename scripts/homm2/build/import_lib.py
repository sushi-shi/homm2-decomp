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
import struct
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


def imported_hints(exe: Path, dll: str) -> dict[str, int]:
    """Return the exact named imports and hints for ``dll`` in retail IAT order."""
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
    return {row["name"]: row["hint"] for row in entry["symbols"]}


def imported_names(exe: Path, dll: str) -> list[str]:
    """Return the exact named imports for ``dll`` in retail IAT order."""
    return list(imported_hints(exe, dll))


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


def verify_archive_hints(path: Path, expected: dict[str, int]) -> None:
    """Prove the produced archive's ``.idata$6`` hint/name records are retail-exact."""
    data = path.read_bytes()
    if data[:8] != b"!<arch>\n":
        raise ValueError(f"{path}: generated import library is not an archive")

    actual: dict[str, int] = {}
    offset = 8
    while offset + 60 <= len(data):
        try:
            size = int(data[offset + 48:offset + 58].decode().strip() or "0")
        except (UnicodeDecodeError, ValueError) as error:
            raise ValueError(f"{path}: invalid archive member header") from error
        body_offset = offset + 60
        member = data[body_offset:body_offset + size]
        if len(member) != size:
            raise ValueError(f"{path}: truncated archive member")
        if len(member) >= 20 and member[:4] == b"\0\0\xff\xff":
            # VC6 stores each named import as an IMPORT_OBJECT_HEADER followed
            # by public-symbol and DLL strings, rather than a full COFF object
            # with a materialized .idata$6 section. The hint in this header is
            # exactly what LINK later writes to the image's .idata$6 record.
            try:
                _sig1, _sig2, _version, _machine, _stamp, data_size, hint, flags = (
                    struct.unpack_from("<HHHHIIHH", member)
                )
            except struct.error as error:
                raise ValueError(f"{path}: malformed short-import member") from error
            payload = member[20:20 + data_size]
            strings = payload.split(b"\0")
            if len(strings) < 3:
                raise ValueError(f"{path}: malformed short-import strings")
            public_name = strings[0].decode("latin-1")
            name_type = (flags >> 2) & 0x7
            if name_type == 1:  # IMPORT_OBJECT_NAME
                export_name = public_name
            elif name_type == 2:  # IMPORT_OBJECT_NAME_NO_PREFIX
                export_name = public_name[1:] if public_name[:1] in "_@?" else public_name
            elif name_type == 3:  # IMPORT_OBJECT_NAME_UNDECORATE
                export_name = public_name[1:] if public_name[:1] in "_@?" else public_name
                export_name = export_name.split("@", 1)[0]
            elif name_type == 4 and len(strings) >= 4:  # IMPORT_OBJECT_NAME_EXPORTAS
                export_name = strings[2].decode("latin-1")
            else:
                export_name = ""
            if export_name in expected:
                actual[export_name] = hint
        # Traditional import-library members materialize .idata$6 in a normal
        # COFF section. Keep supporting that VC5-era representation as well.
        elif len(member) > 20 and member[:4] != b"\xff\xff\0\0":
            try:
                section_count = struct.unpack_from("<H", member, 2)[0]
                for index in range(section_count):
                    header = member[20 + 40 * index:20 + 40 * (index + 1)]
                    if len(header) != 40 or header[:8].rstrip(b"\0") != b".idata$6":
                        continue
                    raw_size, raw_offset = struct.unpack_from("<II", header, 16)
                    blob = member[raw_offset:raw_offset + raw_size]
                    nul = blob.find(b"\0", 2)
                    if len(blob) <= 3 or nul < 0:
                        continue
                    hint = struct.unpack_from("<H", blob, 0)[0]
                    name = blob[2:nul].decode("latin-1")
                    if name in expected:
                        actual[name] = hint
            except struct.error as error:
                raise ValueError(f"{path}: malformed COFF archive member") from error
        offset = body_offset + size + (size & 1)

    mismatches = {
        name: (hint, actual.get(name))
        for name, hint in expected.items()
        if actual.get(name) != hint
    }
    if mismatches:
        raise ValueError(
            f"{path.name}: hint mismatch after synthesis: {mismatches}"
        )


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
    hints = imported_hints(exe, dll)
    imports = list(hints)
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
        verify_archive_hints(implib, hints)
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
