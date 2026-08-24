#!/usr/bin/env python3
"""Split Misc's pooled track literal into its reviewed .data contribution."""

import argparse
import struct
from pathlib import Path

from homm2.core.coff import CoffObject


def symbol_name(data, offset, string_table):
    raw = data[offset:offset + 8]
    if raw[:4] == b"\0\0\0\0":
        start = string_table + struct.unpack_from("<I", raw, 4)[0]
        end = data.index(0, start)
        return bytes(data[start:end]).decode("ascii")
    return bytes(raw.rstrip(b"\0")).decode("ascii")


def adapt(payload):
    coff = CoffObject(payload)
    old = bytearray(payload)
    section_count = coff.section_count
    section_table = 20 + struct.unpack_from("<H", old, 16)[0]
    main_index = 2
    insert_index = 43
    new_ordinal = insert_index + 1
    if section_count != 79:
        raise RuntimeError(f"unexpected Misc section count: {section_count}")

    main = coff.section(main_index + 1)
    main_header = main.header_offset
    if main.name != ".data":
        raise RuntimeError("Misc section 3 is not .data")
    if main.raw_size != 0x3A:
        raise RuntimeError(f"unexpected Misc main .data size: 0x{main.raw_size:x}")
    literal = coff.section_bytes(main)[0x1C:0x3A]
    if literal != b"\\Tracks2\\02-AudioTrack 02.ogg\0":
        raise RuntimeError("unexpected Misc track literal payload")

    insert_at = section_table + insert_index * 40
    data = old[:insert_at] + bytearray(40) + old[insert_at:]
    struct.pack_into("<H", data, 2, section_count + 1)
    old_symbol_table = struct.unpack_from("<I", old, 8)[0]
    symbol_count = struct.unpack_from("<I", old, 12)[0]
    symbol_table = old_symbol_table + 40
    struct.pack_into("<I", data, 8, symbol_table)

    # Every original file pointer follows the enlarged section-header table.
    for old_index in range(section_count):
        new_index = old_index if old_index < insert_index else old_index + 1
        header = section_table + new_index * 40
        for field in (20, 24, 28):
            value = struct.unpack_from("<I", data, header + field)[0]
            if value:
                struct.pack_into("<I", data, header + field, value + 40)

    # The first contribution now contains only the pointer and named globals.
    main_header = section_table + main_index * 40
    struct.pack_into("<I", data, main_header + 16, 0x1C)

    string_table = symbol_table + symbol_count * 18
    found_literal = 0
    found_main_section = 0
    index = 0
    while index < symbol_count:
        offset = symbol_table + index * 18
        name = symbol_name(data, offset, string_table)
        section = struct.unpack_from("<h", data, offset + 12)[0]
        aux_count = data[offset + 17]
        if section >= new_ordinal:
            struct.pack_into("<h", data, offset + 12, section + 1)
            section += 1
        if name == "$SG56843":
            if section != 3 or struct.unpack_from("<I", data, offset + 8)[0] != 0x1C:
                raise RuntimeError("unexpected $SG56843 coordinate")
            struct.pack_into("<Ih", data, offset + 8, 0, new_ordinal)
            found_literal += 1
        if name == ".data" and section == 3 and aux_count == 1:
            struct.pack_into("<I", data, offset + 18, 0x1C)
            found_main_section += 1
        if aux_count and name.startswith("."):
            aux = offset + 18
            if data[aux + 14] == 5:
                associated = struct.unpack_from("<H", data, aux + 12)[0]
                if associated >= new_ordinal:
                    struct.pack_into("<H", data, aux + 12, associated + 1)
        index += 1 + aux_count
    if found_literal != 1 or found_main_section != 1:
        raise RuntimeError("failed to find unique Misc literal/main section symbols")

    # Give the inserted contribution its ordinary section symbol and auxiliary
    # length record. Existing long-name offsets remain relative to the string
    # table, which simply moves after these two new symbol records.
    old_string_table = symbol_table + symbol_count * 18
    section_symbol = bytearray(36)
    section_symbol[:8] = b".data\0\0\0"
    struct.pack_into("<IhHBB", section_symbol, 8, 0, new_ordinal, 0, 3, 1)
    struct.pack_into("<I", section_symbol, 18, len(literal))
    data = data[:old_string_table] + section_symbol + data[old_string_table:]
    struct.pack_into("<I", data, 12, symbol_count + 2)

    new_header = section_table + insert_index * 40
    data[new_header:new_header + 8] = b".data\0\0\0"
    raw_pointer = len(data)
    characteristics = struct.unpack_from("<I", data, main_header + 36)[0]
    struct.pack_into(
        "<IIIIIIHHI", data, new_header + 8,
        0, 0, len(literal), raw_pointer, 0, 0, 0, 0, characteristics,
    )
    data.extend(literal)
    return bytes(data)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    options = parser.parse_args()
    options.output.parent.mkdir(parents=True, exist_ok=True)
    options.output.write_bytes(adapt(options.input.read_bytes()))


if __name__ == "__main__":
    main()
