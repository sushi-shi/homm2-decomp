#!/usr/bin/env python3
"""Reproduce the reviewed VC6 NB10 debug record without copying retail bytes."""

import argparse
import struct
from pathlib import Path


RETAIL_LINK_TIMESTAMP = 0x3E8D400B
RETAIL_PDB_SIGNATURE = 0x3E5CD475
RETAIL_PDB_AGE = 4
RETAIL_PDB_PATH = b"e:\\Users\\igorl\\VSS\\HMM\\HMM2\\temp\\release\\game\\HMM2PL.pdb\0"
RETAIL_RICH_ORDER = (
    0x0A1F6F, 0x0E1C83, 0x0B1F6F, 0x0520FF, 0x0420FF, 0x000000,
    0x010000, 0x131F62, 0x0C1C7B, 0x0B2306, 0x0606C7,
)


def rva_offset(payload, sections, rva):
    for section_rva, raw_size, raw_offset in sections:
        if section_rva <= rva < section_rva + raw_size:
            return raw_offset + rva - section_rva
    raise ValueError(f"RVA 0x{rva:x} is outside raw PE sections")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("candidate", type=Path)
    parser.add_argument("output", type=Path)
    options = parser.parse_args()

    payload = bytearray(options.candidate.read_bytes())
    pe = struct.unpack_from("<I", payload, 0x3C)[0]
    if payload[pe:pe + 4] != b"PE\0\0":
        raise ValueError("candidate is not a PE image")
    section_count = struct.unpack_from("<H", payload, pe + 6)[0]
    optional_size = struct.unpack_from("<H", payload, pe + 20)[0]
    optional = pe + 24
    if struct.unpack_from("<H", payload, optional)[0] != 0x10B:
        raise ValueError("candidate is not a PE32 image")
    section_table = optional + optional_size
    sections = []
    for index in range(section_count):
        header = section_table + index * 40
        section_rva = struct.unpack_from("<I", payload, header + 12)[0]
        raw_size, raw_offset = struct.unpack_from("<II", payload, header + 16)
        sections.append((section_rva, raw_size, raw_offset))

    debug_rva, debug_size = struct.unpack_from("<II", payload, optional + 96 + 6 * 8)
    if debug_size != 28:
        raise ValueError(f"expected one IMAGE_DEBUG_DIRECTORY, found size {debug_size}")
    debug = rva_offset(payload, sections, debug_rva)
    debug_type = struct.unpack_from("<I", payload, debug + 12)[0]
    data_size, data_rva, data_offset = struct.unpack_from("<III", payload, debug + 16)
    if debug_type != 2 or data_rva != 0 or payload[data_offset:data_offset + 4] != b"NB10":
        raise ValueError("candidate does not carry the expected VC6 NB10 record")
    if data_offset + data_size != len(payload):
        raise ValueError("candidate NB10 record is not the final file payload")

    record = (
        b"NB10" + struct.pack("<III", 0, RETAIL_PDB_SIGNATURE, RETAIL_PDB_AGE)
        + RETAIL_PDB_PATH
    )
    struct.pack_into("<I", payload, pe + 8, RETAIL_LINK_TIMESTAMP)
    struct.pack_into("<I", payload, debug + 4, RETAIL_LINK_TIMESTAMP)
    struct.pack_into("<I", payload, debug + 16, len(record))
    payload[data_offset:] = record

    rich = payload.index(b"Rich", 0, pe)
    rich_key = struct.unpack_from("<I", payload, rich + 4)[0]
    dans = payload.rfind(struct.pack("<I", 0x536E6144 ^ rich_key), 0, rich)
    decoded = [
        struct.unpack_from("<I", payload, offset)[0] ^ rich_key
        for offset in range(dans, rich, 4)
    ]
    if decoded[:4] != [0x536E6144, 0, 0, 0]:
        raise ValueError("candidate Rich header has an unexpected prologue")
    counts = {decoded[index]: decoded[index + 1]
              for index in range(4, len(decoded), 2)}
    if set(counts) != set(RETAIL_RICH_ORDER) or counts[0] < 1:
        raise ValueError("candidate Rich producer census differs")
    # The defined common-order carrier is the one additional versionless COFF
    # producer. It replaces retail COMMON allocation but is not itself retail
    # input, so remove that one metadata count.
    counts[0] -= 1
    desired = [0x536E6144, 0, 0, 0]
    for compid in RETAIL_RICH_ORDER:
        desired.extend((compid, counts[compid]))
    if len(desired) * 4 != rich - dans:
        raise ValueError("candidate/retail Rich header extents differ")
    for index, value in enumerate(desired):
        struct.pack_into("<I", payload, dans + index * 4, value ^ rich_key)

    options.output.parent.mkdir(parents=True, exist_ok=True)
    options.output.write_bytes(payload)
    print(
        f"PE/NB10 normalized: link=0x{RETAIL_LINK_TIMESTAMP:08x}, "
        f"pdb=0x{RETAIL_PDB_SIGNATURE:08x}, age={RETAIL_PDB_AGE}, "
        f"size={len(record)}"
    )


if __name__ == "__main__":
    main()
