#!/usr/bin/env python3
"""Apply reviewed DATA-marker placement to a final-link COFF .bss copy."""

from __future__ import annotations

import argparse
import json
import struct
from pathlib import Path


COFF_I386 = 0x14C
COFF_HEADER_SIZE = 20
SECTION_HEADER_SIZE = 40
SYMBOL_SIZE = 18
IMAGE_SCN_ALIGN_MASK = 0x00F00000
IMAGE_SCN_ALIGN_SHIFT = 20


class CoffError(ValueError):
    pass


def walk(value):
    if isinstance(value, dict):
        yield value
        for child in value.values():
            yield from walk(child)
    elif isinstance(value, list):
        for child in value:
            yield from walk(child)


def symbol_name(blob: bytes, offset: int, string_offset: int, string_end: int) -> str:
    raw = blob[offset:offset + 8]
    if raw[:4] != b"\0\0\0\0":
        return raw.split(b"\0", 1)[0].decode("ascii")
    relative = struct.unpack_from("<I", raw, 4)[0]
    absolute = string_offset + relative
    if relative < 4 or absolute >= string_end:
        raise CoffError(f"invalid COFF string-table offset {relative}")
    end = blob.find(b"\0", absolute, string_end)
    if end < 0:
        raise CoffError("unterminated COFF symbol name")
    return blob[absolute:end].decode("ascii")


def reviewed_offsets(census_path: Path, unit: str, retail_start: int, size: int):
    census = json.loads(census_path.read_text())
    mappings: dict[str, set[str]] = {}
    markers: dict[str, set[int]] = {}
    for item in walk(census):
        if "source_name" in item and "symbol" in item:
            mappings.setdefault(item["source_name"], set()).add(item["symbol"])
        if (
            item.get("unit") == unit
            and isinstance(item.get("name"), str)
            and isinstance(item.get("rva"), int)
            and retail_start <= item["rva"] < retail_start + size
        ):
            markers.setdefault(item["name"], set()).add(item["rva"])

    result = {}
    for source_name, rvas in markers.items():
        if len(rvas) != 1:
            raise CoffError(f"{unit}: ambiguous marker RVA for {source_name}: {rvas}")
        symbols = mappings.get(source_name, set())
        if len(symbols) != 1:
            raise CoffError(f"{unit}: ambiguous symbol mapping for {source_name}: {symbols}")
        symbol = next(iter(symbols))
        result[symbol] = next(iter(rvas)) - retail_start
    return result


def adapt(
    blob: bytes,
    unit: str,
    offsets: dict[str, int],
    alignment: int = 16,
    section_size: int | None = None,
) -> bytes:
    if len(blob) < COFF_HEADER_SIZE:
        raise CoffError(f"{unit}: truncated COFF header")
    machine, section_count, _, symbol_offset, symbol_count, optional_size, _ = (
        struct.unpack_from("<HHIIIHH", blob, 0)
    )
    if machine != COFF_I386 or optional_size:
        raise CoffError(f"{unit}: expected an i386 COFF object")

    section_offset = COFF_HEADER_SIZE
    section_end = section_offset + section_count * SECTION_HEADER_SIZE
    symbol_end = symbol_offset + symbol_count * SYMBOL_SIZE
    if section_end > len(blob) or symbol_end + 4 > len(blob):
        raise CoffError(f"{unit}: truncated section or symbol table")
    string_size = struct.unpack_from("<I", blob, symbol_end)[0]
    string_end = symbol_end + string_size
    if string_size < 4 or string_end > len(blob):
        raise CoffError(f"{unit}: truncated COFF string table")

    bss_sections = []
    for number in range(1, section_count + 1):
        offset = section_offset + (number - 1) * SECTION_HEADER_SIZE
        name = blob[offset:offset + 8].split(b"\0", 1)[0]
        if name == b".bss":
            bss_sections.append((number, offset))
    if len(bss_sections) != 1:
        raise CoffError(f"{unit}: expected exactly one .bss section")
    bss_number, bss_header_offset = bss_sections[0]

    result = bytearray(blob)
    original_size = struct.unpack_from("<I", result, bss_header_offset + 16)[0]
    if section_size is None:
        section_size = original_size
    if section_size <= 0:
        raise CoffError(f"{unit}: invalid .bss section size {section_size}")
    struct.pack_into("<I", result, bss_header_offset + 16, section_size)
    if alignment <= 0 or alignment & (alignment - 1) or alignment > 8192:
        raise CoffError(f"{unit}: invalid COFF section alignment {alignment}")
    alignment_bits = (alignment.bit_length() << IMAGE_SCN_ALIGN_SHIFT)
    characteristics = struct.unpack_from("<I", result, bss_header_offset + 36)[0]
    characteristics &= ~IMAGE_SCN_ALIGN_MASK
    characteristics |= alignment_bits
    struct.pack_into("<I", result, bss_header_offset + 36, characteristics)

    defined = {}
    bss_aux_offset = None
    index = 0
    while index < symbol_count:
        offset = symbol_offset + index * SYMBOL_SIZE
        name = symbol_name(blob, offset, symbol_end, string_end)
        section_number = struct.unpack_from("<h", blob, offset + 12)[0]
        auxiliary_count = blob[offset + 17]
        if section_number == bss_number:
            if name == ".bss" and auxiliary_count == 1:
                bss_aux_offset = offset + SYMBOL_SIZE
            elif name != ".bss":
                if name in defined:
                    raise CoffError(f"{unit}: duplicate .bss symbol {name}")
                defined[name] = offset
        index += 1 + auxiliary_count

    if bss_aux_offset is None:
        raise CoffError(f"{unit}: .bss section symbol lacks its auxiliary record")
    struct.pack_into("<I", result, bss_aux_offset, section_size)

    missing = set(defined) - set(offsets)
    extra = set(offsets) - set(defined)
    if missing or extra:
        raise CoffError(
            f"{unit}: marker coverage differs: missing={sorted(missing)}, extra={sorted(extra)}"
        )
    for name, offset in defined.items():
        if offsets[name] >= section_size:
            raise CoffError(f"{unit}: {name} lies beyond the reviewed .bss extent")
        struct.pack_into("<I", result, offset + 8, offsets[name])
    return bytes(result)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--census", type=Path, default=Path("build/gen/data_topology_census.json"))
    parser.add_argument("--unit", default="SOURCE/ADVMGR")
    parser.add_argument("--retail-start", type=lambda value: int(value, 0), default=0x123DE0)
    args = parser.parse_args()

    blob = args.input.read_bytes()
    offsets = reviewed_offsets(args.census, args.unit, args.retail_start, 600)
    output = adapt(blob, args.unit, offsets)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(output)
    print(f"{args.unit}: placed {len(offsets)} .bss symbols, align=16")


if __name__ == "__main__":
    main()
