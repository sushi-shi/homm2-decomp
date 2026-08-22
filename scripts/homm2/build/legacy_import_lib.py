#!/usr/bin/env python3
"""Generate the legacy WinG import library from its reviewed DEF file.

The WinG 1.0 SDK library predates short COFF import records.  VC6 LINK orders
its contributions differently when every regular archive member is named
``wing32.def``.  Current LIB.EXE, even with ``/LINK50COMPAT``, emits modern
COMDAT bookkeeping and therefore cannot reproduce the retail IAT order.

This module emits the small, documented pre-VC6 COFF form directly.  The DEF
file remains the only export ledger; no SDK binary or final-image patching is
used.  Unreferenced export members are intentional because the complete export
surface determines PE hint values and preserves the vendor declaration order.
"""

from __future__ import annotations

import argparse
import re
import struct
import sys
from dataclasses import dataclass
from pathlib import Path


ARCHIVE_MAGIC = b"!<arch>\n"
MEMBER_NAME = "wing32.def/"
STDCALL = re.compile(r"^_(?P<name>[A-Za-z_][A-Za-z0-9_]*)@(?P<bytes>\d+)$")
EXPORT = re.compile(
    r"^(?P<lookup>[A-Za-z_][A-Za-z0-9_]*)="
    r"(?P<caller>_[A-Za-z_][A-Za-z0-9_]*@\d+)\s+@(?P<ordinal>\d+)$"
)


@dataclass(frozen=True)
class ExportSpec:
    lookup: str
    caller: str
    argument_bytes: int
    ordinal: int

    @property
    def import_symbol(self) -> str:
        return "__imp_" + self.caller


def read_definition(path: Path) -> list[ExportSpec]:
    """Read the full WinG surface in declaration order."""
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
        match = EXPORT.fullmatch(line)
        if match is None:
            raise ValueError(
                f"{path}:{number}: expected lookup=_stdcall@bytes @ordinal"
            )
        caller = match.group("caller")
        stdcall = STDCALL.fullmatch(caller)
        if stdcall is None or stdcall.group("name") != match.group("lookup"):
            raise ValueError(f"{path}:{number}: inconsistent WinG alias {line!r}")
        exports.append(
            ExportSpec(
                lookup=match.group("lookup"),
                caller=caller,
                argument_bytes=int(stdcall.group("bytes")),
                ordinal=int(match.group("ordinal")),
            )
        )

    if library is None or library.lower() != "wing32.dll":
        raise ValueError(f"{path}: expected LIBRARY WING32.dll")
    if not exports:
        raise ValueError(f"{path}: no exports")
    lookups = [spec.lookup for spec in exports]
    callers = [spec.caller for spec in exports]
    if len(lookups) != len(set(lookups)) or len(callers) != len(set(callers)):
        raise ValueError(f"{path}: duplicate exports")
    expected_ordinals = {
        name: index + 1 for index, name in enumerate(sorted(lookups))
    }
    mismatches = [
        (spec.lookup, spec.ordinal, expected_ordinals[spec.lookup])
        for spec in exports
        if spec.ordinal != expected_ordinals[spec.lookup]
    ]
    if mismatches:
        details = ", ".join(
            f"{name}: {actual}, expected {expected}"
            for name, actual, expected in mismatches
        )
        raise ValueError(f"{path}: export ordinals do not reproduce hints: {details}")
    return exports


def _symbol(
    name: str,
    value: int,
    section_number: int,
    storage: int,
    auxiliary_count: int,
    strings: bytearray,
) -> bytes:
    encoded = name.encode("ascii")
    if len(encoded) <= 8:
        name_field = encoded.ljust(8, b"\0")
    else:
        offset = len(strings)
        strings.extend(encoded + b"\0")
        name_field = struct.pack("<II", 0, offset)
    return name_field + struct.pack(
        "<IhHBB", value, section_number, 0, storage, auxiliary_count
    )


def _section(
    name: str,
    virtual_address: int,
    size: int,
    raw_offset: int,
    relocation_offset: int,
    relocation_count: int,
    characteristics: int,
) -> bytes:
    return struct.pack(
        "<8sIIIIIIHHI",
        name.encode("ascii").ljust(8, b"\0"),
        0,
        virtual_address,
        size,
        raw_offset,
        relocation_offset,
        0,
        relocation_count,
        0,
        characteristics,
    )


def _optional_header() -> bytes:
    data = bytearray(224)
    struct.pack_into("<HBB", data, 0, 0x10B, 2, 23)
    struct.pack_into("<II", data, 32, 0x1000, 0x200)
    struct.pack_into("<H", data, 40, 1)
    struct.pack_into(
        "<IIII", data, 72, 0x100000, 0x1000, 0x100000, 0x1000
    )
    struct.pack_into("<I", data, 92, 16)
    return bytes(data)


def _descriptor_object(
    import_descriptor: str = "WING32_IMPORT_DESCRIPTOR",
    null_descriptor: str = "NULL_IMPORT_DESCRIPTOR",
    null_thunk: str = "\x7fWING32_NULL_THUNK_DATA",
    dll: str = "WING32.dll",
) -> bytes:
    optional = _optional_header()
    idata2_raw = 20 + len(optional) + 80
    idata2_relocations = idata2_raw + 20
    idata6_raw = idata2_relocations + 30
    symbol_offset = idata6_raw + 12
    header = struct.pack(
        "<HHIIIHH", 0x14C, 2, 0, symbol_offset, 7, len(optional), 0x8180
    )
    sections = _section(
        ".idata$2", 0, 20, idata2_raw, idata2_relocations, 3, 0xC0000048
    )
    sections += _section(".idata$6", 20, 12, idata6_raw, 0, 0, 0x48)

    strings = bytearray(b"\0\0\0\0")
    symbols = bytearray()
    symbols += _symbol(import_descriptor, 0, 1, 2, 0, strings)
    symbols += _symbol(".idata$2", 0x48, 1, 0x68, 0, strings)
    symbols += _symbol(".idata$6", 0, 2, 3, 0, strings)
    symbols += _symbol(".idata$4", 0x48, 0, 0x68, 0, strings)
    symbols += _symbol(".idata$5", 0x48, 0, 0x68, 0, strings)
    symbols += _symbol(null_descriptor, 0, 0, 2, 0, strings)
    symbols += _symbol(null_thunk, 0, 0, 2, 0, strings)
    struct.pack_into("<I", strings, 0, len(strings))

    data = bytearray(header + optional + sections)
    data += b"\0" * 20
    data += struct.pack("<IIH", 12, 2, 7)
    data += struct.pack("<IIH", 0, 3, 7)
    data += struct.pack("<IIH", 16, 4, 7)
    dll_bytes = dll.encode("ascii") + b"\0"
    if len(dll_bytes) > 12:
        raise ValueError(f"DLL name {dll!r} exceeds legacy descriptor capacity")
    data += dll_bytes.ljust(12, b"\0")
    data += symbols + strings
    return bytes(data)


def _null_descriptor_object(null_descriptor: str = "NULL_IMPORT_DESCRIPTOR") -> bytes:
    symbol_offset = 20 + 40 + 20
    header = struct.pack("<HHIIIHH", 0x14C, 1, 0, symbol_offset, 1, 0, 0x8180)
    sections = _section(".idata$3", 0, 20, 60, 0, 0, 0xC0000048)
    strings = bytearray(b"\0\0\0\0")
    symbols = _symbol(null_descriptor, 0, 1, 2, 0, strings)
    struct.pack_into("<I", strings, 0, len(strings))
    return header + sections + b"\0" * 20 + symbols + strings


def _null_thunk_object(null_thunk: str = "\x7fWING32_NULL_THUNK_DATA") -> bytes:
    symbol_offset = 20 + 80 + 8
    header = struct.pack("<HHIIIHH", 0x14C, 2, 0, symbol_offset, 1, 0, 0x8180)
    sections = _section(".idata$5", 0, 4, 100, 0, 0, 0x48)
    sections += _section(".idata$4", 0, 4, 104, 0, 0, 0x48)
    strings = bytearray(b"\0\0\0\0")
    symbols = _symbol(null_thunk, 0, 1, 2, 0, strings)
    struct.pack_into("<I", strings, 0, len(strings))
    return header + sections + b"\0" * 8 + symbols + strings


def _function_object(
    spec: ExportSpec,
    hint: int,
    import_descriptor: str = "WING32_IMPORT_DESCRIPTOR",
) -> bytes:
    lookup = struct.pack("<H", hint) + spec.lookup.encode("ascii") + b"\0"
    if len(lookup) & 1:
        lookup += b"\0"

    header_size = 20 + 4 * 40
    text_raw = header_size
    text_relocation = text_raw + 6
    lookup_raw = text_relocation + 10
    iat_raw = lookup_raw + len(lookup)
    iat_relocation = iat_raw + 4
    int_raw = iat_relocation + 10
    int_relocation = int_raw + 4
    symbol_offset = int_relocation + 10
    lookup_va = 6
    iat_va = lookup_va + len(lookup)
    int_va = iat_va + 4

    header = struct.pack("<HHIIIHH", 0x14C, 4, 0, symbol_offset, 6, 0, 0x8180)
    sections = _section(".text", 0, 6, text_raw, text_relocation, 1, 0x1028)
    sections += _section(".idata$6", lookup_va, len(lookup), lookup_raw, 0, 0, 0x48)
    sections += _section(".idata$5", iat_va, 4, iat_raw, iat_relocation, 1, 0x48)
    sections += _section(".idata$4", int_va, 4, int_raw, int_relocation, 1, 0x48)

    strings = bytearray(b"\0\0\0\0")
    symbols = bytearray()
    symbols += _symbol(".text", 0, 1, 3, 1, strings)
    symbols += struct.pack("<IHHIhBBH", 6, 1, 0, 0, 0, 1, 0, 0)
    symbols += _symbol(spec.caller, 0, 1, 2, 0, strings)
    symbols += _symbol(".idata$6", 0, 2, 3, 0, strings)
    symbols += _symbol(import_descriptor, 0, 0, 2, 0, strings)
    symbols += _symbol(spec.import_symbol, 0, 3, 2, 0, strings)
    struct.pack_into("<I", strings, 0, len(strings))

    data = bytearray(header + sections)
    data += b"\xff\x25\0\0\0\0"
    data += struct.pack("<IIH", 2, 5, 6)
    data += lookup
    data += b"\0" * 4
    data += struct.pack("<IIH", iat_va, 3, 7)
    data += b"\0" * 4
    data += struct.pack("<IIH", int_va, 3, 7)
    data += symbols + strings
    return bytes(data)


def _archive_header(size: int, member_name: str = MEMBER_NAME) -> bytes:
    return b"".join(
        (
            member_name.encode("ascii").ljust(16),
            b"0".ljust(12),
            b"0".ljust(6),
            b"0".ljust(6),
            b"0".ljust(8),
            str(size).encode("ascii").ljust(10),
            b"`\n",
        )
    )


def _linker_header(size: int, member_name: str = MEMBER_NAME) -> bytes:
    return _archive_header(size, member_name).replace(
        member_name.encode("ascii").ljust(16), b"/".ljust(16), 1
    )


def _member_size(payload: bytes) -> int:
    return 60 + len(payload) + (len(payload) & 1)


def build_archive(exports: list[ExportSpec]) -> bytes:
    hints = {
        name: index for index, name in enumerate(sorted(spec.lookup for spec in exports))
    }
    physical_specs = sorted(exports, key=lambda spec: spec.lookup)
    members = [_descriptor_object(), _null_descriptor_object(), _null_thunk_object()]
    members.extend(_function_object(spec, hints[spec.lookup]) for spec in physical_specs)
    physical_index = {spec.lookup: index + 3 for index, spec in enumerate(physical_specs)}

    first_entries = [
        ("WING32_IMPORT_DESCRIPTOR", 0),
        ("NULL_IMPORT_DESCRIPTOR", 1),
        ("\x7fWING32_NULL_THUNK_DATA", 2),
    ]
    for spec in exports:
        index = physical_index[spec.lookup]
        first_entries.extend(((spec.caller, index), (spec.import_symbol, index)))

    logical_members = [0, 1, 2] + [physical_index[spec.lookup] for spec in exports]
    first_names = b"".join(name.encode("ascii") + b"\0" for name, _ in first_entries)
    second_entries = sorted(first_entries, key=lambda entry: entry[0])
    second_names = b"".join(name.encode("ascii") + b"\0" for name, _ in second_entries)
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
    first += b"".join(struct.pack(">I", offsets[index]) for _, index in first_entries)
    first += first_names

    logical_ranks = {
        physical: logical + 1 for logical, physical in enumerate(logical_members)
    }
    second = struct.pack("<I", len(logical_members))
    second += b"".join(struct.pack("<I", offsets[index]) for index in logical_members)
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
        archive += _archive_header(len(member)) + member
        if len(member) & 1:
            archive += b"\n"
    return bytes(archive)


def generate(definition: Path, output: Path) -> Path:
    exports = read_definition(definition)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(build_archive(exports))
    print(f"[legacy-implib] WING32.dll: {len(exports)} exports -> {output}")
    return output


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--definition", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args(argv)
    try:
        generate(args.definition, args.out)
    except (OSError, ValueError) as error:
        print(f"[legacy-implib] ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
