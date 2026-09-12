"""Negative controls for whole-object readability refactoring proofs."""
import struct
import tempfile
import unittest
from pathlib import Path

from homm2.audit.object_equivalence import compare, compare_directories
from homm2.core.coff import CoffObject
from homm2.core.test_coff import tiny_object


def edited(offset, fmt, value):
    blob = bytearray(tiny_object())
    struct.pack_into(fmt, blob, offset, value)
    return bytes(blob)


def local_object(name, value=4, storage=3):
    blob = bytearray(tiny_object())
    coff = CoffObject(blob)
    symbol = next(iter(coff.symbols.values()))
    blob[symbol.offset:symbol.offset + 8] = name.encode().ljust(8, b'\0')
    struct.pack_into('<I', blob, symbol.offset + 8, value)
    struct.pack_into('<H', blob, symbol.offset + 14, 0)
    blob[symbol.offset + 16] = storage
    return bytes(blob)


class ObjectEquivalenceTests(unittest.TestCase):
    def setUp(self):
        self.original = tiny_object()
        self.coff = CoffObject(self.original)

    def test_identical_object_passes(self):
        self.assertEqual(compare(self.original, self.original)['errors'], [])

    def test_timestamp_is_not_codegen(self):
        self.assertEqual(compare(self.original, edited(4, '<I', 123))['errors'], [])

    def test_instruction_byte_change_fails(self):
        change = edited(self.coff.sections[0].raw_offset, '<B', 0xCC)
        self.assertIn('section bytes: .text', compare(self.original, change)['errors'])

    def test_relocation_addend_byte_change_fails(self):
        change = edited(self.coff.sections[0].raw_offset + 4, '<I', 8)
        self.assertIn('section bytes: .text', compare(self.original, change)['errors'])

    def test_relocation_type_change_fails(self):
        change = edited(self.coff.relocations[0].offset + 8, '<H', 0x14)
        self.assertIn('ordered relocations', compare(self.original, change)['errors'])

    def test_relocation_site_change_fails(self):
        change = edited(self.coff.relocations[0].offset, '<I', 0)
        self.assertIn('ordered relocations', compare(self.original, change)['errors'])

    def test_named_target_change_fails(self):
        change = bytearray(self.original)
        change[self.coff.string_offset + 4] = ord('X')
        errors = compare(self.original, bytes(change))['errors']
        self.assertIn('ordered relocations', errors)
        self.assertIn('named symbols', errors)

    def test_section_flags_change_fails(self):
        change = edited(self.coff.sections[0].header_offset + 36, '<I', 0x40000040)
        self.assertIn('section layout: .text', compare(self.original, change)['errors'])

    def test_relocation_order_is_not_replaced_by_a_multiset(self):
        blob = bytearray(self.original)
        second = struct.pack('<IIH', 0, 0, 0x0006)
        at = self.coff.relocations[0].offset
        blob[at + 10:at + 10] = second
        struct.pack_into('<I', blob, 8, self.coff.symbol_offset + 10)
        struct.pack_into('<H', blob, self.coff.sections[0].header_offset + 32, 2)
        changed = blob[:]
        changed[at:at + 20] = blob[at + 10:at + 20] + blob[at:at + 10]
        self.assertIn('ordered relocations', compare(bytes(blob), bytes(changed))['errors'])

    def test_unreferenced_named_symbol_is_still_checked(self):
        blob = bytearray(self.original)
        struct.pack_into('<H', blob, self.coff.sections[0].header_offset + 32, 0)
        changed = blob[:]
        struct.pack_into('<I', changed, self.coff.symbol_offset + 8, 0)
        self.assertIn('named symbols', compare(bytes(blob), bytes(changed))['errors'])

    def test_counter_renumbering_retains_exact_destination(self):
        for prefix in ('$SG', '$T', '$L'):
            with self.subTest(prefix=prefix):
                storage = 6 if prefix == '$L' else 3
                self.assertEqual(compare(local_object(prefix + '1', storage=storage),
                                         local_object(prefix + '999', storage=storage))['errors'], [])

    def test_counter_renumbering_cannot_hide_changed_destination(self):
        errors = compare(local_object('$SG1'), local_object('$SG2', value=0))['errors']
        self.assertIn('ordered relocations', errors)

    def test_external_counter_like_names_are_not_normalized(self):
        errors = compare(local_object('$SG1', storage=2),
                         local_object('$SG2', storage=2))['errors']
        self.assertIn('ordered relocations', errors)
        self.assertIn('named symbols', errors)

    def test_undefined_counter_like_names_are_not_normalized(self):
        first = bytearray(local_object('$SG1'))
        second = bytearray(local_object('$SG2'))
        for blob in (first, second):
            struct.pack_into('<h', blob, self.coff.symbol_offset + 12, 0)
        self.assertIn('ordered relocations', compare(bytes(first), bytes(second))['errors'])

    def test_empty_directories_do_not_prove_equivalence(self):
        with tempfile.TemporaryDirectory() as folder:
            self.assertFalse(compare_directories(Path(folder), Path(folder))['passed'])

    def test_directory_census_detects_missing_and_added_objects(self):
        with tempfile.TemporaryDirectory() as folder:
            before = Path(folder) / 'before'
            after = Path(folder) / 'after'
            before.mkdir()
            after.mkdir()
            (before / 'a.obj').write_bytes(self.original)
            (after / 'b.obj').write_bytes(self.original)
            report = compare_directories(before, after)
            self.assertFalse(report['passed'])
            self.assertEqual(report['objects']['a.obj']['errors'], ['missing object'])
            self.assertEqual(report['added'], ['b.obj'])


if __name__ == '__main__':
    unittest.main()
