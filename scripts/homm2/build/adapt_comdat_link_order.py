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

from homm2.core.coff import (
    LNK_COMDAT as IMAGE_SCN_LNK_COMDAT,
    STATIC_STORAGE as IMAGE_SYM_CLASS_STATIC,
    SYMBOL_SIZE,
    CoffObject,
)

COFF_HEADER_SIZE = 20
SECTION_HEADER_SIZE = 40


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


def adapt(blob: bytes, unit: str) -> bytes:
    move = REVIEWED_MOVES.get(unit)
    if move is None:
        raise CoffError(f"{unit}: not a reviewed COMDAT-order unit")
    if (move.before is None) == (move.after is None):
        raise CoffError(f"{unit}: reviewed move must have exactly one anchor direction")

    try:
        coff = CoffObject(blob)
    except ValueError as error:
        raise CoffError(f"{unit}: {error}") from error
    if struct.unpack_from("<H", blob, 16)[0]:
        raise CoffError(f"{unit}: object unexpectedly has an optional header")

    section_count = coff.section_count
    symbol_offset = coff.symbol_offset
    symbol_count = coff.symbol_count
    section_offset = COFF_HEADER_SIZE
    section_end = section_offset + section_count * SECTION_HEADER_SIZE

    wanted = {move.symbol, move.before, move.after} - {None}
    symbol_sections: dict[str, int] = {}
    section_aux_offsets: list[int] = []
    for symbol in coff.symbols.values():
        if symbol.name in wanted and symbol.section > 0:
            if symbol.name in symbol_sections:
                raise CoffError(f"{unit}: duplicate defined anchor symbol {symbol.name}")
            symbol_sections[symbol.name] = symbol.section
        if (
            symbol.section > 0
            and symbol.storage_class == IMAGE_SYM_CLASS_STATIC
            and symbol.typ == 0
            and symbol.value == 0
            and symbol.aux_count == 1
        ):
            section_aux_offsets.append(symbol.offset + SYMBOL_SIZE)

    missing = wanted - symbol_sections.keys()
    if missing:
        raise CoffError(f"{unit}: missing anchor symbol(s): {', '.join(sorted(missing))}")

    moved_section = symbol_sections[move.symbol]
    anchor_symbol = move.before if move.before is not None else move.after
    assert anchor_symbol is not None
    anchor_section = symbol_sections[anchor_symbol]
    if moved_section == anchor_section:
        raise CoffError(f"{unit}: moved symbol and anchor share a section")

    moved = coff.section(moved_section)
    if moved.name != ".text" or not (moved.characteristics & IMAGE_SCN_LNK_COMDAT):
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
