#!/usr/bin/env python3
"""Generate a pre-short-record named import library from reviewed ABI facts.

VC6 emits compact ``IMPORT_OBJECT_HEADER`` members.  Some retail vendor
libraries instead contain ordinary i386 COFF objects for every import.  The
linked ABI is the same, but LINK's contribution and Rich-header accounting are
not.  Retail-exact mode takes used decorated names and hints from the control
PE.  Generic mode takes its ABI from a checked-in DEF and assigns conventional
sorted hints; those advisory values do not require the retail executable.
"""

from __future__ import annotations

import argparse
import re
import struct
import sys
from pathlib import Path

from homm2.build.import_lib import imported_hints, verify_archive_hints
from homm2.build.legacy_import_lib import (
    ARCHIVE_MAGIC,
    ExportSpec,
    _member_size,
    _section,
    _symbol,
)
from homm2.build.link_exe import RETAIL_EXE


STDCALL = re.compile(r"^_[A-Za-z_][A-Za-z0-9_]*@(?P<bytes>\d+)$")


def read_definition(path: Path, expected_dll: str | None = None) -> tuple[str, list[str]]:
    """Read a strict named-export DEF whose names are already DLL-exact."""
    library = None
    in_exports = False
    exports = []
    for number, raw_line in enumerate(
        path.read_text(encoding="ascii").splitlines(), start=1
    ):
        line = raw_line.split(";", 1)[0].strip()
        if not line:
            continue
        keyword, _, remainder = line.partition(" ")
        if keyword.upper() == "LIBRARY":
            if library is not None or not remainder.strip():
                raise ValueError(f"{path}:{number}: invalid LIBRARY directive")
            library = remainder.strip()
            continue
        if keyword.upper() == "DESCRIPTION":
            continue
        if keyword.upper() == "EXPORTS" and not remainder.strip():
            in_exports = True
            continue
        if not in_exports:
            raise ValueError(f"{path}:{number}: expected LIBRARY or EXPORTS")
        if len(line.split()) != 1 or "=" in line:
            raise ValueError(
                f"{path}:{number}: expected one literal decorated named export"
            )
        match = STDCALL.fullmatch(line)
        if match is None:
            raise ValueError(f"{path}:{number}: expected _stdcall@bytes export")
        exports.append(line)

    if library is None:
        raise ValueError(f"{path}: no LIBRARY directive")
    if expected_dll is not None and library.lower() != expected_dll.lower():
        raise ValueError(f"{path}: defines {library}, expected {expected_dll}")
    if not exports:
        raise ValueError(f"{path}: no exports")
    if len(exports) != len(set(exports)):
        raise ValueError(f"{path}: duplicate exports")
    return library, exports


def _identities(dll: str) -> tuple[str, str, str, str]:
    stem = Path(dll).stem
    return (
        f"__IMPORT_DESCRIPTOR_{stem}",
        "__NULL_IMPORT_DESCRIPTOR",
        f"\x7f{stem}_NULL_THUNK_DATA",
        f"{dll}/",
    )


def _optional_header() -> bytes:
    data = bytearray(224)
    struct.pack_into("<HBB", data, 0, 0x10B, 4, 20)
    struct.pack_into("<II", data, 32, 0x1000, 0x200)
    struct.pack_into("<H", data, 40, 4)
    struct.pack_into(
        "<IIII", data, 72, 0x100000, 0x1000, 0x100000, 0x1000
    )
    struct.pack_into("<I", data, 92, 16)
    return bytes(data)


def _descriptor_object(
    import_descriptor: str,
    null_descriptor: str,
    null_thunk: str,
    dll: str,
) -> bytes:
    optional = _optional_header()
    idata2_raw = 20 + len(optional) + 80
    idata2_relocations = idata2_raw + 20
    idata6_raw = idata2_relocations + 30
    dll_bytes = dll.encode("ascii") + b"\0"
    if len(dll_bytes) & 1:
        dll_bytes += b"\0"
    symbol_offset = idata6_raw + len(dll_bytes)
    header = struct.pack(
        "<HHIIIHH", 0x14C, 2, 0, symbol_offset, 7, len(optional), 0x100
    )
    sections = _section(
        ".idata$2", 0, 20, idata2_raw, idata2_relocations, 3, 0xC0100040
    )
    sections += _section(
        ".idata$6", 0, len(dll_bytes), idata6_raw, idata2_relocations, 0,
        0xC0200040,
    )

    strings = bytearray(b"\0\0\0\0")
    symbols = bytearray()
    symbols += _symbol(import_descriptor, 0, 1, 2, 0, strings)
    symbols += _symbol(".idata$2", 0xC0000040, 1, 0x68, 0, strings)
    symbols += _symbol(".idata$6", 0, 2, 3, 0, strings)
    symbols += _symbol(".idata$4", 0xC0000040, 0, 0x68, 0, strings)
    symbols += _symbol(".idata$5", 0xC0000040, 0, 0x68, 0, strings)
    symbols += _symbol(null_descriptor, 0, 0, 2, 0, strings)
    symbols += _symbol(null_thunk, 0, 0, 2, 0, strings)
    struct.pack_into("<I", strings, 0, len(strings))

    data = bytearray(header + optional + sections)
    data += b"\0" * 20
    data += struct.pack("<IIH", 12, 2, 7)
    data += struct.pack("<IIH", 0, 3, 7)
    data += struct.pack("<IIH", 16, 4, 7)
    data += dll_bytes
    data += symbols + strings
    return bytes(data)


def _null_descriptor_object(null_descriptor: str) -> bytes:
    symbol_offset = 20 + 40 + 20
    header = struct.pack("<HHIIIHH", 0x14C, 1, 0, symbol_offset, 1, 0, 0x100)
    sections = _section(".idata$3", 0, 20, 60, 0, 0, 0xC0100040)
    strings = bytearray(b"\0\0\0\0")
    symbols = _symbol(null_descriptor, 0, 1, 2, 0, strings)
    struct.pack_into("<I", strings, 0, len(strings))
    return header + sections + b"\0" * 20 + symbols + strings


def _null_thunk_object(null_thunk: str) -> bytes:
    symbol_offset = 20 + 80 + 8
    header = struct.pack("<HHIIIHH", 0x14C, 2, 0, symbol_offset, 1, 0, 0x100)
    sections = _section(".idata$5", 0, 4, 100, 0, 0, 0xC0300040)
    sections += _section(".idata$4", 0, 4, 104, 0, 0, 0xC0300040)
    strings = bytearray(b"\0\0\0\0")
    symbols = _symbol(null_thunk, 0, 1, 2, 0, strings)
    struct.pack_into("<I", strings, 0, len(strings))
    return header + sections + b"\0" * 8 + symbols + strings


def _section_aux(
    length: int, relocations: int, association: int, selection: int
) -> bytes:
    return struct.pack(
        "<IHHIhBBH", length, relocations, 0, 0, association, selection, 0, 14
    )


def _symbol_at(
    string_offset: int,
    value: int,
    section_number: int,
    symbol_type: int,
    storage: int,
    auxiliary_count: int,
) -> bytes:
    return struct.pack(
        "<IIIhHBB",
        0,
        string_offset,
        value,
        section_number,
        symbol_type,
        storage,
        auxiliary_count,
    )


def _function_object(
    spec: ExportSpec, hint: int, import_descriptor: str, patched: bool = True
) -> bytes:
    lookup = struct.pack("<H", hint) + spec.lookup.encode("ascii") + b"\0"
    if len(lookup) & 1:
        lookup += b"\0"

    header_size = 20 + 4 * 40
    text_raw = header_size
    text_relocation = text_raw + 6
    iat_raw = text_relocation + 10
    iat_relocation = iat_raw + 4
    int_raw = iat_relocation + 10
    int_relocation = int_raw + 4
    lookup_raw = int_relocation + 10
    symbol_offset = lookup_raw + len(lookup)

    header = struct.pack("<HHIIIHH", 0x14C, 4, 0, symbol_offset, 11, 0, 0x100)
    sections = _section(
        ".text", 0, 6, text_raw, text_relocation, 1, 0x60201020
    )
    sections += _section(
        ".idata$5", 0, 4, iat_raw, iat_relocation, 1, 0xC0301040
    )
    sections += _section(
        ".idata$4", 0, 4, int_raw, int_relocation, 1, 0xC0301040
    )
    sections += _section(
        ".idata$6", 0, len(lookup), lookup_raw, 0, 0, 0xC0201040
    )

    strings = bytearray(b"\0\0\0\0")
    import_offset = len(strings)
    strings += spec.import_symbol.encode("ascii") + b"\0"
    descriptor_offset = len(strings)
    strings += import_descriptor.encode("ascii") + (b"\0\0" if patched else b"\0")
    struct.pack_into("<I", strings, 0, len(strings))

    symbols = bytearray()
    symbols += _symbol(".text", 0, 1, 3, 1, strings)
    symbols += _section_aux(6, 1, 0, 1)
    symbols += _symbol_at(import_offset + 6, 0, 1, 0x20, 2, 0)
    symbols += _symbol(".idata$5", 0, 2, 3, 1, strings)
    symbols += _section_aux(4, 1, 0, 1)
    symbols += _symbol_at(import_offset, 0, 2, 0, 2, 0)
    symbols += _symbol(".idata$4", 0, 3, 3, 1, strings)
    symbols += _section_aux(4, 1, 2, 5)
    symbols += _symbol(".idata$6", 0, 4, 3, 1, strings)
    symbols += _section_aux(len(lookup), 0, 2, 5)
    symbols += _symbol_at(descriptor_offset, 0, 0, 0, 2, 0)

    data = bytearray(header + sections)
    data += b"\xff\x25\0\0\0\0"
    data += struct.pack("<IIH", 2, 5, 6)
    data += b"\0" * 4
    data += struct.pack("<IIH", 0, 8, 7)
    data += b"\0" * 4
    data += struct.pack("<IIH", 0, 8, 7)
    data += lookup
    data += symbols + strings
    return bytes(data)


def _ordinal_function_object(spec: ExportSpec, import_descriptor: str) -> bytes:
    header_size = 20 + 3 * 40
    text_raw = header_size
    text_relocation = text_raw + 6
    iat_raw = text_relocation + 10
    int_raw = iat_raw + 4
    symbol_offset = int_raw + 4
    ordinal = 0x80000000 | spec.ordinal

    header = struct.pack("<HHIIIHH", 0x14C, 3, 0, symbol_offset, 9, 0, 0x100)
    sections = _section(
        ".text", 0, 6, text_raw, text_relocation, 1, 0x60201020
    )
    sections += _section(".idata$5", 0, 4, iat_raw, 0, 0, 0xC0301040)
    sections += _section(".idata$4", 0, 4, int_raw, 0, 0, 0xC0301040)

    strings = bytearray(b"\0\0\0\0")
    import_offset = len(strings)
    strings += spec.import_symbol.encode("ascii") + b"\0"
    descriptor_offset = len(strings)
    strings += import_descriptor.encode("ascii") + b"\0"
    struct.pack_into("<I", strings, 0, len(strings))

    symbols = bytearray()
    symbols += _symbol(".text", 0, 1, 3, 1, strings)
    symbols += _section_aux(6, 1, 0, 1)
    symbols += _symbol_at(import_offset + 6, 0, 1, 0x20, 2, 0)
    symbols += _symbol(".idata$5", 0, 2, 3, 1, strings)
    symbols += _section_aux(4, 0, 0, 1)
    symbols += _symbol_at(import_offset, 0, 2, 0, 2, 0)
    symbols += _symbol(".idata$4", 0, 3, 3, 1, strings)
    symbols += _section_aux(4, 0, 2, 5)
    symbols += _symbol_at(descriptor_offset, 0, 0, 0, 2, 0)

    data = bytearray(header + sections)
    data += b"\xff\x25\0\0\0\0"
    data += struct.pack("<IIH", 2, 5, 6)
    data += struct.pack("<II", ordinal, ordinal)
    data += symbols + strings
    return bytes(data)


def _archive_header(size: int, member_name: str) -> bytes:
    return b"".join(
        (
            member_name.encode("ascii").ljust(16),
            b"0".ljust(12),
            b"".ljust(6),
            b"".ljust(6),
            b"0".ljust(8),
            str(size).encode("ascii").ljust(10),
            b"`\n",
        )
    )


def _linker_header(size: int) -> bytes:
    return _archive_header(size, "/")


def _assemble_archive(
    dll: str,
    specs: list[ExportSpec],
    function_object,
    linker_padding_count: int = 0,
) -> bytes:
    import_descriptor, null_descriptor, null_thunk, member_name = _identities(dll)
    physical_specs = sorted(specs, key=lambda spec: spec.lookup)
    members = [
        _descriptor_object(
            import_descriptor, null_descriptor, null_thunk, dll
        ),
        _null_descriptor_object(null_descriptor),
        _null_thunk_object(null_thunk),
    ]
    members.extend(function_object(spec, import_descriptor) for spec in physical_specs)
    physical_index = {
        spec.lookup: index + 3 for index, spec in enumerate(physical_specs)
    }

    first_entries = [
        (spec.caller, physical_index[spec.lookup]) for spec in physical_specs
    ]
    first_entries.extend(((import_descriptor, 0), (null_descriptor, 1)))
    first_entries.extend(
        (spec.import_symbol, physical_index[spec.lookup])
        for spec in physical_specs
    )
    first_entries.append((null_thunk, 2))

    logical_members = [0, 1, 2] + [physical_index[spec.lookup] for spec in specs]
    linker_padding = b"\0" * (2 * linker_padding_count)
    first_names = (
        b"".join(name.encode("ascii") + b"\0" for name, _ in first_entries)
        + linker_padding
    )
    second_entries = sorted(first_entries, key=lambda entry: entry[0])
    second_names = (
        b"".join(name.encode("ascii") + b"\0" for name, _ in second_entries)
        + linker_padding
    )
    first_size = 4 + 4 * len(first_entries) + len(first_names)
    second_size = (
        4 + 4 * len(logical_members) + 4 + 2 * len(second_entries) + len(second_names)
    )
    regular_start = len(ARCHIVE_MAGIC) + _member_size(bytes(first_size))
    regular_start += _member_size(bytes(second_size))
    offsets = []
    cursor = regular_start
    for member in members:
        offsets.append(cursor)
        cursor += _member_size(member)

    first = struct.pack(">I", len(first_entries))
    first += b"".join(
        struct.pack(">I", offsets[index]) for _, index in first_entries
    )
    first += first_names

    logical_ranks = {
        physical: logical + 1 for logical, physical in enumerate(logical_members)
    }
    second = struct.pack("<I", len(logical_members))
    second += b"".join(
        struct.pack("<I", offsets[index]) for index in logical_members
    )
    second += struct.pack("<I", len(second_entries))
    second += b"".join(
        struct.pack("<H", logical_ranks[index]) for _, index in second_entries
    )
    second += second_names

    archive = bytearray(ARCHIVE_MAGIC)
    for linker in (first, second):
        archive += _linker_header(len(linker)) + linker
        if len(linker) & 1:
            archive += b"\n"
    for member in members:
        archive += _archive_header(len(member), member_name) + member
        if len(member) & 1:
            archive += b"\n"
    return bytes(archive)


def build_named_specs_archive(
    dll: str,
    specs: list[ExportSpec],
    hints: dict[str, int],
    patched_lookups: set[str] | None = None,
) -> bytes:
    patched_lookups = set() if patched_lookups is None else patched_lookups
    return _assemble_archive(
        dll,
        specs,
        lambda spec, descriptor: _function_object(
            spec,
            hints[spec.lookup],
            descriptor,
            spec.lookup in patched_lookups,
        ),
        len(patched_lookups),
    )


def build_ordinal_specs_archive(dll: str, specs: list[ExportSpec]) -> bytes:
    return _assemble_archive(dll, specs, _ordinal_function_object)


def build_archive(
    dll: str, exports: list[str], exact_public: set[str] | None = None
) -> bytes:
    """Build a Microsoft archive containing conventional regular COFF imports."""
    hints = {name: index for index, name in enumerate(sorted(exports))}
    exact_public = set(exports) if exact_public is None else exact_public
    specs = [
        ExportSpec(
            lookup=name,
            caller=name if name in exact_public else "_" + name,
            argument_bytes=int(STDCALL.fullmatch(name).group("bytes")),
            ordinal=hints[name] + 1,
        )
        for name in exports
    ]
    return build_named_specs_archive(dll, specs, hints, exact_public)


def generate(exe: Path, dll: str, output: Path) -> Path:
    """Generate the measured regular-COFF form from retail-used imports only."""
    expected = imported_hints(exe, dll)
    specs = []
    for name in sorted(expected):
        match = STDCALL.fullmatch(name)
        if match is None:
            raise ValueError(f"{dll}: expected _stdcall@bytes import, got {name!r}")
        specs.append(
            ExportSpec(
                lookup=name,
                caller=name,
                argument_bytes=int(match.group("bytes")),
                ordinal=expected[name] + 1,
            )
        )

    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(
        build_named_specs_archive(dll, specs, expected, set(expected))
    )
    verify_archive_hints(output, expected)
    print(
        f"[regular-implib] {dll}: {len(expected)} retail imports, "
        f"retail hints embedded directly -> {output}"
    )
    return output


def generate_from_definition(definition: Path, dll: str, output: Path) -> Path:
    """Generate a conventional named import archive without a retail image."""
    declared_dll, exports = read_definition(definition, dll)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(build_archive(declared_dll, exports))
    print(
        f"[regular-implib] {declared_dll}: {len(exports)} reviewed DEF exports "
        f"-> {output}"
    )
    return output


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    source = parser.add_mutually_exclusive_group()
    source.add_argument("--exe", type=Path)
    source.add_argument("--definition", type=Path)
    parser.add_argument("--dll", required=True)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args(argv)
    try:
        if args.definition is not None:
            generate_from_definition(args.definition, args.dll, args.out)
        else:
            generate(args.exe or RETAIL_EXE, args.dll, args.out)
    except (OSError, ValueError) as error:
        print(f"[regular-implib] ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
