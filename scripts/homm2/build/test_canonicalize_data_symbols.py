import hashlib
import struct
import unittest
from dataclasses import dataclass
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build.canonicalize_data_symbols import (
    CoffObject,
    canonicalize_coff,
    main,
    sidecar_bytes,
)


TEXT = 0x60000020
DATA = 0xC0000040
RDATA = 0x40000040
BSS = 0xC0000080


@dataclass(frozen=True)
class SectionSpec:
    name: str
    data: bytes
    flags: int
    relocations: tuple = ()


def make_coff(sections, symbols):
    """Build the strict i386 COFF subset used by binary normalization tests."""
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

    section_table_end = 20 + len(sections) * 40
    cursor = section_table_end
    raw_offsets = []
    for section in sections:
        if section.flags & 0x80:
            raw_offsets.append(0)
        else:
            raw_offsets.append(cursor)
            cursor += len(section.data)
    relocation_offsets = []
    for section in sections:
        relocation_offsets.append(cursor if section.relocations else 0)
        cursor += len(section.relocations) * 10
    symbol_offset = cursor

    section_table = bytearray()
    for index, section in enumerate(sections):
        section_table.extend(struct.pack(
            "<8sIIIIIIHHI", section.name.encode("ascii").ljust(8, b"\0"),
            0, 0, len(section.data), raw_offsets[index],
            relocation_offsets[index], 0, len(section.relocations), 0,
            section.flags,
        ))
    raw = b"".join(section.data for section in sections
                   if not section.flags & 0x80)
    relocations = bytearray()
    for section in sections:
        for site, target_index, typ in section.relocations:
            relocations.extend(struct.pack("<IIH", site, target_index, typ))
    symbol_table = bytearray()
    for name, value, section, typ, storage in symbols:
        symbol_table.extend(encoded_name(name))
        symbol_table.extend(struct.pack("<IhHBB", value, section, typ, storage, 0))
    struct.pack_into("<I", strings, 0, len(strings))
    header = struct.pack(
        "<HHIIIHH", 0x14C, len(sections), 0, symbol_offset,
        len(symbols), 0, 0,
    )
    return header + section_table + raw + relocations + symbol_table + strings


def names_by_offset(result):
    return {
        (row.section_ordinal, row.section_offset): row.canonical_name
        for row in result.rows if row.section_ordinal
    }


class CanonicalizeDataSymbolsTest(unittest.TestCase):
    def test_sg_renumber_is_identical_and_input_is_not_mutated(self):
        sections = [SectionSpec(".data", b"hello\0\0\0", DATA)]
        old = make_coff(sections, [("$SG10", 0, 1, 0, 3)])
        renamed = make_coff(sections, [("$SG99999", 0, 1, 0, 3)])
        before = bytes(old)
        first = canonicalize_coff(old)
        second = canonicalize_coff(renamed)
        self.assertEqual(first.data, second.data)
        self.assertEqual(old, before)
        digest = hashlib.sha256(b"hello\0").hexdigest()
        self.assertEqual(first.rows[0].canonical_name, f"$anon_str_{digest}_0")
        self.assertEqual(canonicalize_coff(first.data).data, first.data)

    def test_empty_and_padded_empty_strings_hash_through_first_nul(self):
        sections = [SectionSpec(".data", b"\0\0\0\0x\0\0\0", DATA)]
        result = canonicalize_coff(make_coff(sections, [
            ("$SG1", 0, 1, 0, 3), ("$SG2", 4, 1, 0, 3),
        ]))
        empty = hashlib.sha256(b"\0").hexdigest()
        self.assertEqual(names_by_offset(result)[(1, 0)], f"$anon_str_{empty}_0")
        self.assertEqual(result.rows[0].meaningful_size, 1)

    def test_duplicate_writable_strings_use_offset_order_not_symbol_order(self):
        sections = [SectionSpec(".data", b"same\0\0\0\0same\0\0\0\0", DATA)]
        result = canonicalize_coff(make_coff(sections, [
            ("$SG20", 8, 1, 0, 3), ("$SG10", 0, 1, 0, 3),
        ]))
        digest = hashlib.sha256(b"same\0").hexdigest()
        self.assertEqual(names_by_offset(result), {
            (1, 0): f"$anon_str_{digest}_0",
            (1, 8): f"$anon_str_{digest}_1",
        })

    def test_private_extent_stops_at_following_external_data_definition(self):
        result = canonicalize_coff(make_coff(
            [SectionSpec(".data", b"a\0\0\0WXYZ", DATA)],
            [("$SG1", 0, 1, 0, 3), ("public_data", 4, 1, 0, 2)],
        ))
        self.assertTrue(result.rows[0].canonical_name.startswith("$anon_str_"))
        self.assertEqual(result.rows[0].physical_size, 4)
        self.assertEqual(result.rows[0].meaningful_size, 2)

    def test_wrong_duplicate_string_relocation_remains_distinguishable(self):
        text = b"\xA1\0\0\0\0\xA1\0\0\0\0"
        data = b"same\0\0\0\0same\0\0\0\0"
        symbols = [
            ("fn", 0, 1, 0x20, 2),
            ("$SG1", 0, 2, 0, 3),
            ("$SG2", 8, 2, 0, 3),
        ]
        correct = make_coff([
            SectionSpec(".text", text, TEXT, ((1, 1, 6), (6, 2, 6))),
            SectionSpec(".data", data, DATA),
        ], symbols)
        wrong = make_coff([
            SectionSpec(".text", text, TEXT, ((1, 1, 6), (6, 1, 6))),
            SectionSpec(".data", data, DATA),
        ], symbols)
        normalized_correct = canonicalize_coff(correct).data
        normalized_wrong = canonicalize_coff(wrong).data
        self.assertNotEqual(normalized_correct, normalized_wrong)
        parsed = CoffObject(normalized_correct)
        targets = [parsed.symbols[row.symbol_index].name for row in parsed.relocations]
        self.assertNotEqual(targets[0], targets[1])

    def test_only_names_and_string_table_change(self):
        obj = make_coff([
            SectionSpec(".text", b"\xA1\0\0\0\0", TEXT, ((1, 1, 6),)),
            SectionSpec(".data", b"value\0\0\0", DATA),
        ], [("function", 0, 1, 0x20, 2), ("$SG1", 0, 2, 0, 3)])
        result = canonicalize_coff(obj)
        before, after = CoffObject(obj), CoffObject(result.data)
        self.assertEqual(before.relocations, after.relocations)
        self.assertEqual(
            [before.section_bytes(row) for row in before.sections],
            [after.section_bytes(row) for row in after.sections],
        )
        self.assertEqual(
            [(row.index, row.value, row.section, row.typ,
              row.storage_class, row.aux_count) for row in before.symbols.values()],
            [(row.index, row.value, row.section, row.typ,
              row.storage_class, row.aux_count) for row in after.symbols.values()],
        )

    def test_named_static_preserves_semantic_prefix(self):
        sections = [SectionSpec(".bss", bytes(4), BSS)]
        first = canonicalize_coff(make_coff(
            sections, [("_gColor$S12", 0, 1, 0, 3)]))
        second = canonicalize_coff(make_coff(
            sections, [("_gColor$S900", 0, 1, 0, 3)]))
        self.assertEqual(first.data, second.data)
        self.assertTrue(first.rows[0].canonical_name.startswith("_gColor$Sdata_bss_"))

    def test_padded_t_stays_anon_data_despite_x87_use(self):
        bits = struct.pack("<I", 0x43F00000) + bytes(4)
        obj = make_coff([
            SectionSpec(".text", b"\xD8\x05\0\0\0\0", TEXT, ((2, 0, 6),)),
            SectionSpec(".rdata", bits, RDATA),
        ], [("$T42489", 0, 2, 0, 3)])
        result = canonicalize_coff(obj)
        self.assertTrue(result.rows[0].canonical_name.startswith("$anon_data_"))
        self.assertEqual(result.rows[0].proof, "ambiguous-content-width")

    def test_f64_high_dword_addend_is_ignored_as_evidence_and_preserved(self):
        bits = struct.pack("<Q", 0x3FF0000000000000)
        text = b"\xDD\x05\0\0\0\0\xA1\x04\0\0\0"
        obj = make_coff([
            SectionSpec(".text", text, TEXT, ((2, 0, 6), (7, 0, 6))),
            SectionSpec(".rdata", bits, RDATA),
        ], [("$T1", 0, 2, 0, 3)])
        result = canonicalize_coff(obj)
        self.assertEqual(result.rows[0].canonical_name,
                         "$anon_f64_3ff0000000000000_0")
        before = CoffObject(obj).section_bytes(CoffObject(obj).sections[0])
        after_coff = CoffObject(result.data)
        self.assertEqual(after_coff.section_bytes(after_coff.sections[0]), before)

    def test_t_name_is_independent_of_presence_or_kind_of_code_uses(self):
        bits = struct.pack("<I", 0x3F800000) + bytes(4)
        cases = [
            [SectionSpec(".rdata", bits, RDATA)],
            [SectionSpec(".text", b"\x68\0\0\0\0", TEXT, ((1, 0, 6),)),
             SectionSpec(".rdata", bits, RDATA)],
            [SectionSpec(".text", b"\xD8\x05\0\0\0\0\xDD\x05\0\0\0\0", TEXT,
                         ((2, 0, 6), (8, 0, 6))),
             SectionSpec(".rdata", bits, RDATA)],
        ]
        names = []
        for sections in cases:
            target_section = len(sections)
            result = canonicalize_coff(make_coff(
                sections, [("$T2", 0, target_section, 0, 3)]))
            self.assertTrue(result.rows[0].canonical_name.startswith("$anon_data_"))
            names.append(result.rows[0].canonical_name)
        self.assertEqual(len(set(names)), 1)

    def test_storage_is_distinguishable_for_identical_strings(self):
        payload = b"same\0\0\0\0"
        result = canonicalize_coff(make_coff([
            SectionSpec(".data", payload, DATA),
            SectionSpec(".rdata", payload, RDATA),
        ], [("$SG1", 0, 1, 0, 3), ("$SG2", 0, 2, 0, 3)]))
        self.assertNotEqual(result.rows[0].canonical_name,
                            result.rows[1].canonical_name)
        self.assertEqual([row.storage for row in result.rows], ["data", "rdata"])

    def test_anon_data_relocation_target_changes_hash(self):
        data = bytes(4)
        sections_a = [SectionSpec(".data", data, DATA, ((0, 1, 6),))]
        sections_b = [SectionSpec(".data", data, DATA, ((0, 2, 6),))]
        symbols = [
            ("$SG1", 0, 1, 0, 3),
            ("semantic_a", 0, 0, 0, 2),
            ("semantic_b", 0, 0, 0, 2),
        ]
        first = canonicalize_coff(make_coff(sections_a, symbols)).rows[0]
        second = canonicalize_coff(make_coff(sections_b, symbols)).rows[0]
        self.assertNotEqual(first.canonical_name, second.canonical_name)

    def test_named_pointer_hash_includes_duplicate_target_occurrence(self):
        payload = b"same\0\0\0\0same\0\0\0\0" + bytes(4)
        symbols = [
            ("$SG1", 0, 1, 0, 3),
            ("$SG2", 8, 1, 0, 3),
            ("gPointer$S12", 16, 1, 0, 3),
        ]
        first = canonicalize_coff(make_coff([
            SectionSpec(".data", payload, DATA, ((16, 0, 6),)),
        ], symbols))
        second = canonicalize_coff(make_coff([
            SectionSpec(".data", payload, DATA, ((16, 1, 6),)),
        ], symbols))
        first_pointer = names_by_offset(first)[(1, 16)]
        second_pointer = names_by_offset(second)[(1, 16)]
        self.assertNotEqual(first_pointer, second_pointer)

    def test_cycle_and_same_offset_alias_fail_closed(self):
        cycle = make_coff([
            SectionSpec(".data", bytes(8), DATA, ((0, 1, 6), (4, 0, 6))),
        ], [("$SG1", 0, 1, 0, 3), ("$SG2", 4, 1, 0, 3)])
        with self.assertRaisesRegex(ValueError, "cyclic"):
            canonicalize_coff(cycle)
        alias = make_coff([SectionSpec(".data", bytes(4), DATA)], [
            ("$SG1", 0, 1, 0, 3), ("semantic", 0, 1, 0, 2),
        ])
        with self.assertRaisesRegex(ValueError, "same-offset"):
            canonicalize_coff(alias)

    def test_undefined_named_static_is_unchanged_and_diagnosed(self):
        obj = make_coff([SectionSpec(".text", b"\xC3", TEXT)], [
            ("crt_value$S12", 0, 0, 0, 2),
        ])
        result = canonicalize_coff(obj)
        self.assertEqual(CoffObject(result.data).symbols[0].name, "crt_value$S12")
        self.assertEqual(result.rows[0].proof, "skipped-undefined")

    def test_cli_refuses_to_overwrite_input_object(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            path = root / "link.obj"
            original = make_coff(
                [SectionSpec(".data", b"x\0\0\0", DATA)],
                [("$SG1", 0, 1, 0, 3)])
            path.write_bytes(original)
            with self.assertRaises(SystemExit):
                main([
                    "--input", str(path), "--output", str(path),
                    "--sidecar", str(root / "sidecar.tsv"),
                ])
            self.assertEqual(path.read_bytes(), original)

    def test_huge_string_has_fixed_name_and_bounded_escaped_preview(self):
        payload = b"A" * 20000 + b"\0"
        result = canonicalize_coff(make_coff(
            [SectionSpec(".data", payload, DATA)], [("$SG1", 0, 1, 0, 3)]))
        row = result.rows[0]
        self.assertLess(len(row.canonical_name), 100)
        self.assertLessEqual(len(row.preview), 51)
        self.assertLess(len(sidecar_bytes(result.rows)), 1000)

    def test_full_hash_and_canonical_name_collisions_fail(self):
        class FixedHash:
            def hexdigest(self):
                return "a" * 64

        obj = make_coff([SectionSpec(".data", b"a\0\0\0b\0\0\0", DATA)], [
            ("$SG1", 0, 1, 0, 3), ("$SG2", 4, 1, 0, 3),
        ])
        with mock.patch(
                "homm2.build.canonicalize_data_symbols.hashlib.sha256",
                return_value=FixedHash()):
            with self.assertRaisesRegex(ValueError, "SHA-256 collision"):
                canonicalize_coff(obj)


if __name__ == "__main__":
    unittest.main()
