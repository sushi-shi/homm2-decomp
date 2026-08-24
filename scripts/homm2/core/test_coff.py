"""The shared COFF/RES reader.

Objects are built by hand: the defects these tests exist for are record-layout
misreads (the 0/1-based section-number trap above all), reproducible offline.
"""
import struct
import unittest

from homm2.core.coff import CoffObject, read_res, write_res


def tiny_object() -> bytes:
    """One .text section, one external symbol at +4, one DIR32 reloc."""
    payload = b"\x90" * 8
    reloc = struct.pack("<IIH", 4, 0, 0x0006)
    strings = b"long_external_name\0"
    header = struct.pack("<HHIIIHH", 0x14C, 1, 0, 0, 1, 0, 0)
    section = (b".text\0\0\0"
               + struct.pack("<IIIIIIHHI", 0, 0, len(payload), 0, 0, 0, 1, 0,
                             0x60000020))
    blob = bytearray(header + section)
    raw_offset = len(blob)
    blob += payload
    reloc_offset = len(blob)
    blob += reloc
    symbol_offset = len(blob)
    blob += (struct.pack("<II", 0, 4)
             + struct.pack("<IhHBB", 4, 1, 0x20, 2, 0))
    blob += struct.pack("<I", 4 + len(strings)) + strings
    struct.pack_into("<I", blob, 8, symbol_offset)
    struct.pack_into("<II", blob, 20 + 16 + 4, raw_offset, reloc_offset)
    struct.pack_into("<I", blob, 20 + 16, len(payload))
    return bytes(blob)


class CoffObjectTests(unittest.TestCase):
    def test_sections_and_symbols_are_one_based(self):
        coff = CoffObject(tiny_object())
        self.assertEqual(coff.sections[0].index, 1)
        self.assertEqual(coff.section(1).name, ".text")
        with self.assertRaises(ValueError):
            coff.section(0)
        symbol = coff.symbol("long_external_name")
        self.assertIsNotNone(symbol)
        self.assertEqual(symbol.section, 1)
        self.assertEqual(coff.symbol_section(symbol).name, ".text")

    def test_relocations_carry_the_owner_section_number(self):
        coff = CoffObject(tiny_object())
        (reloc,) = coff.relocations
        self.assertEqual((reloc.section, reloc.site, reloc.typ), (1, 4, 0x0006))
        self.assertEqual(coff.symbols[reloc.symbol_index].name,
                         "long_external_name")

    def test_symbol_payload_reads_the_defining_section(self):
        coff = CoffObject(tiny_object())
        self.assertEqual(coff.symbol_payload("long_external_name"), b"\x90" * 8)

    def test_non_i386_objects_are_rejected(self):
        blob = bytearray(tiny_object())
        struct.pack_into("<H", blob, 0, 0x8664)
        with self.assertRaises(ValueError):
            CoffObject(bytes(blob))


class ResRoundTripTests(unittest.TestCase):
    RECORDS = [
        {"type": 4, "name": "MNUADV", "language": 1049, "data": b"\x01\x02\x03"},
        {"type": 16, "name": 1, "language": 1049, "data": b"\x04" * 5},
    ]

    def test_write_then_read_preserves_identity_order_and_bytes(self):
        blob = write_res(self.RECORDS)
        self.assertEqual(read_res(blob), self.RECORDS)

    def test_the_null_header_record_is_skipped(self):
        blob = write_res([])
        self.assertEqual(len(blob), 32)
        self.assertEqual(read_res(blob), [])
