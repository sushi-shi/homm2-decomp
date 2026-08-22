#!/usr/bin/env python3
"""Adapt reconstructed assembly TUs to their reviewed CVTOMF link topology.

BITS and TILE are reconstructed as C++ so objdiff can compare their functions and
data normally.  Retail linked those two units from OMF-produced objects, however:
their Rich records name CVTOMF 6.00 and their linked starts prove four-byte text
alignment.  This link-only adapter preserves section contents and relocations,
changes the first .text contribution to four-byte alignment, and records the
reviewed CVTOMF producer id.
"""

from __future__ import annotations

import argparse
import struct
from pathlib import Path


COFF_I386 = 0x14C
COFF_HEADER_SIZE = 20
SECTION_HEADER_SIZE = 40
SYMBOL_SIZE = 18
IMAGE_SCN_ALIGN_MASK = 0x00F00000
IMAGE_SCN_ALIGN_4BYTES = 0x00300000
VC6_CPP_COMP_ID = 0x000B2306
CVTOMF_600_COMP_ID = 0x000520FF
REVIEWED_UNITS = {"BASE/BITS", "BASE/TILE"}


class CoffError(ValueError):
    pass


def adapt(blob: bytes, unit: str) -> bytes:
    if unit not in REVIEWED_UNITS:
        raise CoffError(f"{unit}: not a reviewed OMF-produced unit")
    if len(blob) < COFF_HEADER_SIZE:
        raise CoffError(f"{unit}: truncated COFF header")

    machine, section_count, _, symbol_offset, symbol_count, optional_size, _ = (
        struct.unpack_from("<HHIIIHH", blob, 0)
    )
    if machine != COFF_I386:
        raise CoffError(f"{unit}: expected i386 COFF, got machine {machine:#x}")
    if optional_size:
        raise CoffError(f"{unit}: object unexpectedly has an optional header")

    section_end = COFF_HEADER_SIZE + section_count * SECTION_HEADER_SIZE
    symbol_end = symbol_offset + symbol_count * SYMBOL_SIZE
    if section_end > len(blob) or symbol_end > len(blob):
        raise CoffError(f"{unit}: truncated section or symbol table")

    result = bytearray(blob)
    first_text = None
    for index in range(section_count):
        offset = COFF_HEADER_SIZE + index * SECTION_HEADER_SIZE
        name = bytes(result[offset:offset + 8]).split(b"\0", 1)[0]
        if name == b".text":
            first_text = offset
            break
    if first_text is None:
        raise CoffError(f"{unit}: no .text section")

    characteristics_offset = first_text + 36
    characteristics = struct.unpack_from("<I", result, characteristics_offset)[0]
    characteristics &= ~IMAGE_SCN_ALIGN_MASK
    characteristics |= IMAGE_SCN_ALIGN_4BYTES
    struct.pack_into("<I", result, characteristics_offset, characteristics)

    comp_id_offset = None
    index = 0
    while index < symbol_count:
        offset = symbol_offset + index * SYMBOL_SIZE
        name = bytes(result[offset:offset + 8]).split(b"\0", 1)[0]
        auxiliary_count = result[offset + 17]
        if name == b"@comp.id":
            if comp_id_offset is not None:
                raise CoffError(f"{unit}: duplicate @comp.id symbol")
            value, section_number = struct.unpack_from("<Ih", result, offset + 8)
            if value != VC6_CPP_COMP_ID or section_number != -1:
                raise CoffError(
                    f"{unit}: unexpected @comp.id {value:#010x}/{section_number}"
                )
            comp_id_offset = offset + 8
        index += 1 + auxiliary_count

    if comp_id_offset is None:
        raise CoffError(f"{unit}: missing @comp.id symbol")
    struct.pack_into("<I", result, comp_id_offset, CVTOMF_600_COMP_ID)
    return bytes(result)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--unit", choices=sorted(REVIEWED_UNITS), required=True)
    args = parser.parse_args()

    output = adapt(args.input.read_bytes(), args.unit)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(output)


if __name__ == "__main__":
    main()
