#!/usr/bin/env python3
"""Reproduce reviewed VC6 COMDAT contribution order for final linking.

The comparison objects remain untouched.  For the final link, two recovered
objects need a different section-table order than the surviving reconstructed
source naturally emits.  Retail placement proves the order, while each moved
COMDAT's contents, relocations, symbol spelling, selection kind, and alignment
already match.  This adapter only permutes COFF section headers and remaps COFF
section numbers; it does not change section payloads or relocations.
"""

from __future__ import annotations

import argparse
import dataclasses
import struct
from pathlib import Path


COFF_I386 = 0x14C
COFF_HEADER_SIZE = 20
SECTION_HEADER_SIZE = 40
SYMBOL_SIZE = 18
IMAGE_SCN_LNK_COMDAT = 0x00001000
IMAGE_SYM_CLASS_STATIC = 3


@dataclasses.dataclass(frozen=True)
class Move:
    symbol: str
    before: str | None = None
    after: str | None = None


REVIEWED_MOVES = {
    "BASE/AudiereEffects": Move(
        symbol="??1AudiereSampleNode@@QAE@XZ",
        after="_$E20",
    ),
    "BASE/DIMMER": Move(
        symbol="??_GdimmerWidget@@UAEPAXI@Z",
        before="??1dimmerWidget@@UAE@XZ",
    ),
}


class CoffError(ValueError):
    pass


def _symbol_name(blob: bytes, offset: int, string_offset: int, string_end: int) -> str:
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


def adapt(blob: bytes, unit: str) -> bytes:
    move = REVIEWED_MOVES.get(unit)
    if move is None:
        raise CoffError(f"{unit}: not a reviewed COMDAT-order unit")
    if (move.before is None) == (move.after is None):
        raise CoffError(f"{unit}: reviewed move must have exactly one anchor direction")
    if len(blob) < COFF_HEADER_SIZE:
        raise CoffError(f"{unit}: truncated COFF header")

    machine, section_count, _, symbol_offset, symbol_count, optional_size, _ = (
        struct.unpack_from("<HHIIIHH", blob, 0)
    )
    if machine != COFF_I386:
        raise CoffError(f"{unit}: expected i386 COFF, got machine {machine:#x}")
    if optional_size:
        raise CoffError(f"{unit}: object unexpectedly has an optional header")

    section_offset = COFF_HEADER_SIZE
    section_end = section_offset + section_count * SECTION_HEADER_SIZE
    symbol_end = symbol_offset + symbol_count * SYMBOL_SIZE
    if section_end > len(blob) or symbol_end + 4 > len(blob):
        raise CoffError(f"{unit}: truncated section or symbol table")
    string_size = struct.unpack_from("<I", blob, symbol_end)[0]
    string_end = symbol_end + string_size
    if string_size < 4 or string_end > len(blob):
        raise CoffError(f"{unit}: truncated COFF string table")

    wanted = {move.symbol, move.before, move.after} - {None}
    symbol_sections: dict[str, int] = {}
    section_aux_offsets: list[int] = []
    index = 0
    while index < symbol_count:
        offset = symbol_offset + index * SYMBOL_SIZE
        name = _symbol_name(blob, offset, symbol_end, string_end)
        value, section_number, symbol_type = struct.unpack_from("<IhH", blob, offset + 8)
        storage_class = blob[offset + 16]
        auxiliary_count = blob[offset + 17]
        if name in wanted and section_number > 0:
            if name in symbol_sections:
                raise CoffError(f"{unit}: duplicate defined anchor symbol {name}")
            symbol_sections[name] = section_number
        if (
            section_number > 0
            and storage_class == IMAGE_SYM_CLASS_STATIC
            and symbol_type == 0
            and value == 0
            and auxiliary_count == 1
        ):
            section_aux_offsets.append(offset + SYMBOL_SIZE)
        index += 1 + auxiliary_count

    missing = wanted - symbol_sections.keys()
    if missing:
        raise CoffError(f"{unit}: missing anchor symbol(s): {', '.join(sorted(missing))}")

    moved_section = symbol_sections[move.symbol]
    anchor_symbol = move.before if move.before is not None else move.after
    assert anchor_symbol is not None
    anchor_section = symbol_sections[anchor_symbol]
    if moved_section == anchor_section:
        raise CoffError(f"{unit}: moved symbol and anchor share a section")

    moved_header_offset = section_offset + (moved_section - 1) * SECTION_HEADER_SIZE
    moved_header = blob[moved_header_offset:moved_header_offset + SECTION_HEADER_SIZE]
    moved_name = moved_header[:8].split(b"\0", 1)[0]
    moved_characteristics = struct.unpack_from("<I", moved_header, 36)[0]
    if moved_name != b".text" or not (moved_characteristics & IMAGE_SCN_LNK_COMDAT):
        raise CoffError(f"{unit}: reviewed source section is not a .text COMDAT")

    old_order = list(range(1, section_count + 1))
    new_order = [number for number in old_order if number != moved_section]
    anchor_index = new_order.index(anchor_section)
    insertion = anchor_index if move.before is not None else anchor_index + 1
    new_order.insert(insertion, moved_section)
    if new_order == old_order:
        raise CoffError(f"{unit}: reviewed move no longer changes section order")

    old_to_new = {old: new for new, old in enumerate(new_order, 1)}
    result = bytearray(blob)
    headers = [
        blob[section_offset + (old - 1) * SECTION_HEADER_SIZE:
             section_offset + old * SECTION_HEADER_SIZE]
        for old in new_order
    ]
    result[section_offset:section_end] = b"".join(headers)

    index = 0
    while index < symbol_count:
        offset = symbol_offset + index * SYMBOL_SIZE
        section_number = struct.unpack_from("<h", result, offset + 12)[0]
        auxiliary_count = result[offset + 17]
        if section_number > 0:
            struct.pack_into("<h", result, offset + 12, old_to_new[section_number])
        index += 1 + auxiliary_count

    for auxiliary_offset in section_aux_offsets:
        associated = struct.unpack_from("<h", result, auxiliary_offset + 12)[0]
        if associated > 0:
            struct.pack_into("<h", result, auxiliary_offset + 12, old_to_new[associated])

    return bytes(result)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--unit", choices=sorted(REVIEWED_MOVES), required=True)
    args = parser.parse_args()

    output = adapt(args.input.read_bytes(), args.unit)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(output)


if __name__ == "__main__":
    main()
