import struct
import tempfile
import unittest
from dataclasses import dataclass
from pathlib import Path

from homm2.build.retopologize_data import (
    AllocationMove,
    IMAGE_SCN_CNT_INITIALIZED_DATA,
    IMAGE_SCN_CNT_UNINITIALIZED_DATA,
    SectionLayout,
    _coff,
    rewrite_coff_data_topology,
)


TEXT = 0x60500020
DATA = 0xC0300040
BSS = 0xC0300080
DIR32 = 0x0006


@dataclass(frozen=True)
class SectionSpec:
    name: str
    data: bytes
    flags: int
    relocations: tuple = ()


def make_coff(sections, symbols):
    strings = bytearray(struct.pack("<I", 4))
    string_offsets = {}

    def encoded_name(name):
        raw = name.encode("latin-1")
        if len(raw) <= 8:
            return raw.ljust(8, b"\0")
        offset = string_offsets.get(raw)
        if offset is None:
            offset = len(strings)
            string_offsets[raw] = offset
            strings.extend(raw + b"\0")
        return struct.pack("<II", 0, offset)

    cursor = 20 + len(sections) * 40
    raw_offsets = []
    for section in sections:
        raw_offsets.append(
            0 if section.flags & IMAGE_SCN_CNT_UNINITIALIZED_DATA else cursor)
        if raw_offsets[-1]:
            cursor += len(section.data)
    relocation_offsets = []
    for section in sections:
        relocation_offsets.append(cursor if section.relocations else 0)
        cursor += len(section.relocations) * 10

    section_table = bytearray()
    for index, section in enumerate(sections):
        section_table.extend(struct.pack(
            "<8sIIIIIIHHI", section.name.encode("ascii").ljust(8, b"\0"),
            0, 0, len(section.data), raw_offsets[index], relocation_offsets[index],
            0, len(section.relocations), 0, section.flags))
    raw = b"".join(section.data for index, section in enumerate(sections)
                   if raw_offsets[index])
    relocations = b"".join(
        struct.pack("<IIH", site, target, kind)
        for section in sections for site, target, kind in section.relocations)

    records = []
    for index, section in enumerate(sections, 1):
        aux = struct.pack("<IHHIhBBH", len(section.data), 0, 0, 0, 0, 0, 0, 0)
        records.append((section.name, 0, index, 0, 3, (aux,)))
    records.extend((*symbol, ()) for symbol in symbols)
    symbol_table = bytearray()
    for name, value, section, symbol_type, storage, auxiliary in records:
        symbol_table.extend(encoded_name(name))
        symbol_table.extend(struct.pack(
            "<IhHBB", value, section, symbol_type, storage, len(auxiliary)))
        for record in auxiliary:
            symbol_table.extend(record)
    struct.pack_into("<I", strings, 0, len(strings))
    header = struct.pack(
        "<HHIIIHH", 0x14C, len(sections), 0, cursor,
        sum(1 + len(row[5]) for row in records), 0, 0)
    return header + section_table + raw + relocations + symbol_table + strings


class RetopologizeDataTest(unittest.TestCase):
    def test_reorders_bytes_symbols_and_relocation_coordinates(self):
        text = bytearray(8)
        struct.pack_into("<i", text, 0, -4)
        struct.pack_into("<I", text, 4, 4)
        payload = make_coff([
            SectionSpec(
                ".text", bytes(text), TEXT,
                ((0, 4, DIR32), (4, 2, DIR32))),
            SectionSpec(
                ".data", b"ABCD" + struct.pack("<I", 0x11223344), DATA,
                ((4, 6, DIR32),)),
        ], [
            ("first", 0, 2, 0, 3),
            ("second", 4, 2, 0, 3),
            ("external", 0, 0, 0, 2),
        ])
        layout = SectionLayout(
            2, ".data", "data", 0x2000, 12, 4, True,
            (AllocationMove("first", 0, 8, 4),
             AllocationMove("second", 4, 0, 4)))

        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "source.obj"
            output = Path(directory) / "output.obj"
            source.write_bytes(payload)
            self.assertEqual(
                rewrite_coff_data_topology(source, output, (layout,)), 1)
            result = bytearray(output.read_bytes())

        sections, symbols, _ = _coff(result)
        data = sections[1]
        self.assertEqual(
            bytes(result[data.raw:data.raw + data.size]),
            struct.pack("<I", 0x11223344) + bytes(4) + b"ABCD")
        self.assertEqual((symbols[4].value, symbols[5].value), (8, 0))
        self.assertEqual(
            struct.unpack_from("<I", result, symbols[2].offset + 18)[0], 12)
        self.assertEqual(
            struct.unpack_from("<I", result, sections[0].raw)[0], 0xFFFFFFFC)
        self.assertEqual(
            struct.unpack_from("<I", result, sections[0].raw + 4)[0], 0)
        self.assertEqual(struct.unpack_from("<I", result, data.relocations)[0], 0)

    def test_materializes_reviewed_raw_backed_zero_contribution(self):
        payload = make_coff([
            SectionSpec(".bss", bytes(8), BSS),
        ], [
            ("late", 0, 1, 0, 3),
            ("early", 4, 1, 0, 3),
        ])
        layout = SectionLayout(
            1, ".bss", "bss", 0x3000, 8, 4, True,
            (AllocationMove("late", 0, 4, 4),
             AllocationMove("early", 4, 0, 4)))

        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "source.obj"
            output = Path(directory) / "output.obj"
            source.write_bytes(payload)
            rewrite_coff_data_topology(source, output, (layout,))
            result = bytearray(output.read_bytes())

        sections, symbols, _ = _coff(result)
        section = sections[0]
        self.assertEqual(section.name, ".data")
        self.assertEqual(
            bytes(result[section.raw:section.raw + section.size]), bytes(8))
        self.assertTrue(section.characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
        self.assertFalse(section.characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
        self.assertEqual((symbols[2].value, symbols[3].value), (4, 0))
        self.assertEqual(
            result[symbols[0].offset:symbols[0].offset + 8], b".data\0\0\0")


if __name__ == "__main__":
    unittest.main()
