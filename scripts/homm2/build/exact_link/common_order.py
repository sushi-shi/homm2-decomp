#!/usr/bin/env python3
"""Build a COFF object that presents reviewed COMMONs in retail order."""

import argparse
import struct
from pathlib import Path


ROOT = next(path for path in Path(__file__).resolve().parents
            if (path / "flake.nix").exists())
OUTPUT = ROOT / "build/link/common-order.obj"

COMMONS = [
    ("??_B?1???id@?$ctype@G@std@@$D@@9@51", 0x1),
    ("___alternate_form", 0x4),
    ("___no_lead_zeros", 0x4),
    ("___mbcodepage", 0x4),
    ("___mbulinfo", 0xC),
    ("___ismbcodepage", 0x4),
    ("__mbcasemap", 0x100),
    ("__mbctype", 0x101),
    ("___mblcid", 0x4),
    ("___piob", 0x4),
    ("__bufin", 0x1000),
    ("__nstream", 0x4),
    ("___pioinfo", 0x100),
    ("__nhandle", 0x4),
    ("___sbh_sizeHeaderList", 0x4),
    ("___sbh_indGroupDefer", 0x4),
    ("___sbh_pHeaderScan", 0x4),
    ("___sbh_initialized", 0x4),
    ("___sbh_pHeaderDefer", 0x4),
    ("___sbh_cntHeaderList", 0x4),
    ("___sbh_pHeaderList", 0x4),
    ("___sbh_threshold", 0x4),
    ("__crtheap", 0x4),
    ("___active_heap", 0x4),
    ("___setlc_active", 0x4),
    ("___unguarded_readlc_active", 0x4),
    ("__acmdln", 0x4),
    ("___env_initialized", 0x4),
    ("___mbctype_initialized", 0x4),
    ("___onexitend", 0x4),
    ("___onexitbegin", 0x4),
]


def common_offsets():
    rows = []
    cursor = 0
    for name, size in COMMONS:
        alignment = min(32, 1 << (size - 1).bit_length())
        cursor = (cursor + alignment - 1) & -alignment
        rows.append((name, size, cursor))
        cursor += size
    return rows, cursor


def defined_section_object():
    rows, extent = common_offsets()
    symbols = bytearray()
    strings = bytearray()
    for name, _size, offset in rows:
        encoded = name.encode("ascii") + b"\0"
        name_field = struct.pack("<II", 0, 4 + len(strings))
        strings.extend(encoded)
        symbols.extend(name_field)
        symbols.extend(struct.pack("<IhHBB", offset, 1, 0, 2, 0))
    symbol_offset = 20 + 40
    header = struct.pack(
        "<HHIIIHH", 0x14C, 1, 0, symbol_offset, len(rows), 0, 0
    )
    section = struct.pack(
        "<8sIIIIIIHHI", b".bss$ff\0", 0, 0, extent, 0, 0, 0, 0, 0,
        0xC0600080,
    )
    return header + section + symbols + struct.pack("<I", 4 + len(strings)) + strings


def sectionless_common_object():
    symbols = bytearray()
    strings = bytearray()
    # LINK 6 walks sectionless COMMON symbol records in reverse table order.
    for name, size in reversed(COMMONS):
        encoded = name.encode("ascii") + b"\0"
        name_field = struct.pack("<II", 0, 4 + len(strings))
        strings.extend(encoded)
        symbols.extend(name_field)
        symbols.extend(struct.pack("<IhHBB", size, 0, 0, 2, 0))
    header = struct.pack(
        "<HHIIIHH", 0x14C, 0, 0, 20, len(COMMONS), 0, 0
    )
    return header + symbols + struct.pack("<I", 4 + len(strings)) + strings


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--defined-section", action="store_true")
    options = parser.parse_args()
    payload = (defined_section_object() if options.defined_section
               else sectionless_common_object())
    OUTPUT.write_bytes(payload)


if __name__ == "__main__":
    main()
