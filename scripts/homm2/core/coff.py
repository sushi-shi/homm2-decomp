#!/usr/bin/env python3
"""homm2.core.coff - the one COFF object and Win32 .res record reader.

Every tool that opens a compiled object goes through `CoffObject`; every tool
that reads or writes RES32 records goes through `read_res` / `write_res`.
The reader was promoted verbatim from canonicalize_data_symbols' proven,
validating implementation.

Numbering doctrine, stated once so nobody rediscovers it the hard way:
`Section.index` and `Symbol.section` are **1-based**, exactly as COFF stores
them and exactly as `llvm-objdump -t`'s `(sec N)` prints them. A symbol with
`section <= 0` is undefined/absolute/debug. `llvm-objdump -h`'s `Idx` column
is 0-based - one off from everything here.
"""

from __future__ import annotations

import struct
from dataclasses import dataclass

SYMBOL_SIZE = 18

INITIALIZED_DATA = 0x00000040
UNINITIALIZED_DATA = 0x00000080
MEM_EXECUTE = 0x20000000
MEM_WRITE = 0x80000000
LNK_COMDAT = 0x00001000
LNK_NRELOC_OVFL = 0x01000000
LNK_INFO = 0x00000200

RELOCATION_WIDTHS = {
    0x0001: 2,  # IMAGE_REL_I386_DIR16
    0x0002: 2,  # IMAGE_REL_I386_REL16
    0x0006: 4,  # IMAGE_REL_I386_DIR32
    0x0007: 4,  # IMAGE_REL_I386_DIR32NB
    0x000A: 2,  # IMAGE_REL_I386_SECTION
    0x000B: 4,  # IMAGE_REL_I386_SECREL
    0x0014: 4,  # IMAGE_REL_I386_REL32
}
DIR32 = 0x0006
REL32 = 0x0014
FUNCTION_TYPE = 0x0020
EXTERNAL_STORAGE = 2
STATIC_STORAGE = 3
WEAK_EXTERNAL_STORAGE = 105


@dataclass(frozen=True)
class Section:
    index: int          # 1-based COFF section number
    header_offset: int
    name: str
    raw_size: int
    raw_offset: int
    reloc_offset: int
    reloc_count: int
    characteristics: int


@dataclass(frozen=True)
class Symbol:
    index: int
    offset: int
    name: str
    value: int
    section: int        # 1-based; <= 0 means undefined/absolute/debug
    typ: int
    storage_class: int
    aux_count: int


@dataclass(frozen=True)
class Relocation:
    section: int        # 1-based owner section number
    site: int
    symbol_index: int
    typ: int
    offset: int = 0


class CoffObject:
    """A validating, read-only view of one i386 COFF object."""

    def __init__(self, payload: bytes):
        self.data = bytes(payload)
        if len(self.data) < 20:
            raise ValueError("short COFF object")
        machine, section_count = struct.unpack_from("<HH", self.data, 0)
        if machine != 0x14C:
            raise ValueError(f"unsupported COFF machine 0x{machine:x}")
        self.section_count = section_count
        self.symbol_offset = struct.unpack_from("<I", self.data, 8)[0]
        self.symbol_count = struct.unpack_from("<I", self.data, 12)[0]
        optional_size = struct.unpack_from("<H", self.data, 16)[0]
        first_section = 20 + optional_size
        section_end = first_section + section_count * 40
        if section_end > len(self.data):
            raise ValueError("truncated COFF section table")
        self.string_offset = self.symbol_offset + self.symbol_count * SYMBOL_SIZE
        if self.string_offset + 4 > len(self.data):
            raise ValueError("missing COFF string table")
        self.string_size = struct.unpack_from("<I", self.data, self.string_offset)[0]
        if self.string_size < 4 or self.string_offset + self.string_size != len(self.data):
            raise ValueError("COFF string table is not final")
        self.sections = self._read_sections(first_section)
        self.symbols = self._read_symbols()
        self.relocations = self._read_relocations()

    def _string_name(self, offset: int) -> str:
        if not 4 <= offset < self.string_size:
            raise ValueError(f"invalid COFF string offset {offset}")
        start = self.string_offset + offset
        try:
            end = self.data.index(b"\0", start, self.string_offset + self.string_size)
        except ValueError as error:
            raise ValueError("unterminated COFF string") from error
        return self.data[start:end].decode("latin-1")

    def _symbol_name(self, offset: int) -> str:
        raw = self.data[offset:offset + 8]
        zero, string_offset = struct.unpack("<II", raw)
        if zero == 0:
            return self._string_name(string_offset)
        return raw.split(b"\0", 1)[0].decode("latin-1")

    def _section_name(self, offset: int) -> str:
        raw = self.data[offset:offset + 8].split(b"\0", 1)[0]
        if raw.startswith(b"/") and raw[1:].isdigit():
            return self._string_name(int(raw[1:]))
        return raw.decode("latin-1")

    def _read_sections(self, first: int) -> tuple[Section, ...]:
        rows = []
        for zero_index in range(self.section_count):
            offset = first + zero_index * 40
            raw_size, raw_offset, reloc_offset = struct.unpack_from(
                "<III", self.data, offset + 16)
            reloc_count = struct.unpack_from("<H", self.data, offset + 32)[0]
            characteristics = struct.unpack_from("<I", self.data, offset + 36)[0]
            if raw_offset and raw_offset + raw_size > len(self.data):
                raise ValueError("COFF section raw data is out of bounds")
            relocation_bytes = (10 if characteristics & LNK_NRELOC_OVFL
                                else reloc_count * 10)
            if reloc_count and reloc_offset + relocation_bytes > len(self.data):
                raise ValueError("COFF relocation table is out of bounds")
            rows.append(Section(
                zero_index + 1, offset, self._section_name(offset), raw_size,
                raw_offset, reloc_offset, reloc_count, characteristics,
            ))
        return tuple(rows)

    def _read_symbols(self) -> dict[int, Symbol]:
        if self.symbol_offset + self.symbol_count * SYMBOL_SIZE > len(self.data):
            raise ValueError("COFF symbol table is out of bounds")
        rows = {}
        index = 0
        while index < self.symbol_count:
            offset = self.symbol_offset + index * SYMBOL_SIZE
            value, section, typ, storage_class, aux_count = struct.unpack_from(
                "<IhHBB", self.data, offset + 8)
            if index + aux_count >= self.symbol_count:
                raise ValueError("COFF auxiliary symbols exceed the symbol table")
            rows[index] = Symbol(
                index, offset, self._symbol_name(offset), value, section, typ,
                storage_class, aux_count,
            )
            index += 1 + aux_count
        return rows

    def _read_relocations(self) -> tuple[Relocation, ...]:
        rows = []
        for section in self.sections:
            count = section.reloc_count
            first = 0
            if section.characteristics & LNK_NRELOC_OVFL:
                if count != 0xFFFF:
                    raise ValueError("COFF relocation overflow flag/count disagree")
                if section.reloc_offset + 10 > len(self.data):
                    raise ValueError("missing COFF relocation overflow record")
                count, symbol_index, typ = struct.unpack_from(
                    "<IIH", self.data, section.reloc_offset)
                if count < 1 or symbol_index or typ:
                    raise ValueError("invalid COFF relocation overflow record")
                first = 1
            for index in range(first, count):
                offset = section.reloc_offset + index * 10
                if offset + 10 > len(self.data):
                    raise ValueError("COFF relocation table is out of bounds")
                site, symbol_index, typ = struct.unpack_from("<IIH", self.data, offset)
                if site >= section.raw_size:
                    raise ValueError("COFF relocation site is outside its section")
                if symbol_index not in self.symbols:
                    raise ValueError("COFF relocation targets an auxiliary/missing symbol")
                rows.append(Relocation(
                    section.index, site, symbol_index, typ, offset))
        return tuple(rows)

    def section_bytes(self, section: Section) -> bytes:
        if section.raw_offset == 0:
            return bytes(section.raw_size)
        return self.data[section.raw_offset:section.raw_offset + section.raw_size]

    # -- lookup helpers ----------------------------------------------------

    def section(self, number: int) -> Section:
        """The section with 1-based COFF `number`."""
        if not 1 <= number <= self.section_count:
            raise ValueError(f"COFF section number {number} is out of range")
        return self.sections[number - 1]

    def symbol(self, name: str) -> Symbol | None:
        """The first symbol spelled `name`, or None."""
        for row in self.symbols.values():
            if row.name == name:
                return row
        return None

    def symbol_section(self, symbol: Symbol) -> Section | None:
        """The defining section of `symbol`, or None when undefined."""
        if symbol.section <= 0:
            return None
        return self.section(symbol.section)

    def symbol_payload(self, name: str) -> bytes | None:
        """Raw bytes of the section defining `name`, or None when undefined."""
        row = self.symbol(name)
        if row is None:
            return None
        section = self.symbol_section(row)
        return None if section is None else self.section_bytes(section)


# -- Win32 .res records ----------------------------------------------------
#
# The interchange shape is the plain dict {"type", "name", "language", "data"}
# (type/name: int ordinal or str), shared with extract_resources' PE walker
# and the JSON reports.

RES_ORDINAL_MARKER = 0xFFFF
RES_MEMORY_FLAGS = 0x1030


def _align(value: int, alignment: int = 4) -> int:
    return (value + alignment - 1) & ~(alignment - 1)


def read_res(blob: bytes) -> list[dict]:
    """RES32 records in file order, skipping the leading null record."""

    def name_or_ordinal(data: bytes, offset: int):
        value = struct.unpack_from("<H", data, offset)[0]
        if value == RES_ORDINAL_MARKER:
            return struct.unpack_from("<H", data, offset + 2)[0], offset + 4
        end = offset
        while struct.unpack_from("<H", data, end)[0] != 0:
            end += 2
        return data[offset:end].decode("utf-16-le"), end + 2

    records = []
    offset = 0
    while offset < len(blob):
        data_size, header_size = struct.unpack_from("<II", blob, offset)
        cursor = offset + 8
        rtype, cursor = name_or_ordinal(blob, cursor)
        rname, cursor = name_or_ordinal(blob, cursor)
        cursor = _align(cursor)
        _data_version, _memory, language = struct.unpack_from("<IHH", blob, cursor)
        data = blob[offset + header_size: offset + header_size + data_size]
        if not (rtype == 0 and rname == 0):
            records.append(
                {"type": rtype, "name": rname, "language": language, "data": data}
            )
        offset = _align(offset + header_size + data_size)
    return records


def _encode_identifier(value) -> bytes:
    if isinstance(value, int):
        if not 0 <= value <= 0xFFFF:
            raise ValueError(f"resource ordinal is outside WORD range: {value}")
        return struct.pack("<HH", RES_ORDINAL_MARKER, value)
    return value.encode("utf-16le") + b"\0\0"


def encode_res_record(resource: dict) -> bytes:
    identifiers = (_encode_identifier(resource["type"])
                   + _encode_identifier(resource["name"]))
    identifiers += b"\0" * (_align(len(identifiers)) - len(identifiers))
    header_tail = struct.pack(
        "<IHHII", 0, RES_MEMORY_FLAGS, resource["language"], 0, 0)
    header_size = 8 + len(identifiers) + len(header_tail)
    payload = resource["data"]
    record = (struct.pack("<II", len(payload), header_size)
              + identifiers + header_tail + payload)
    return record + b"\0" * (_align(len(record)) - len(record))


def write_res(resources: list[dict]) -> bytes:
    """A complete .res: the null header record, then `resources` in order."""
    null_record = {"type": 0, "name": 0, "language": 0, "data": b""}
    return encode_res_record(null_record) + b"".join(
        encode_res_record(resource) for resource in resources)
