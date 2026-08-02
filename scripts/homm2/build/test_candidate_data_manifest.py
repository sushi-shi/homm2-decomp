import unittest
from types import SimpleNamespace

from homm2.build.candidate_data_manifest import (
    CandidateAllocation,
    REPO,
    _bounded_section_delta_proofs,
    _candidate_data_storage,
    _contains,
    _function_relocation_offsets_align,
    _function_relocation_proofs,
    _function_sequence_relocation_proofs,
    _payload_matches_at,
    _reviewed_candidate_allocation,
    _payload_rvas,
    _reviewed_group_allocations,
    _validate_constrained_reviewed_group,
    _virtual_section_bytes,
    candidate_definitions,
    diagnostics_bytes,
    derive_allocations,
)


class CandidateDataManifestTest(unittest.TestCase):
    def test_linker_sorted_crt_subsection_is_initialized_data(self):
        self.assertEqual(_candidate_data_storage(".CRT$XCU"), "data")
        self.assertEqual(_candidate_data_storage(".CRT$XTX"), "data")
        self.assertEqual(_candidate_data_storage(".data"), "data")
        self.assertIsNone(_candidate_data_storage(".text"))

    def test_constrained_group_accepts_zero_padding_and_equivalent_payloads(self):
        definitions = [
            {"name": "$T1", "section": 1, "section_offset": 0},
            {"name": "$T2", "section": 1, "section_offset": 4},
            {"name": "$T3", "section": 1, "section_offset": 8},
        ]
        coff = SimpleNamespace(
            data=b"AAAABBBBBBBB",
            sections=[SimpleNamespace(raw_offset=0)],
        )
        allocations = [
            CandidateAllocation("A", "A.c", "$T1", "rdata", 0, 4, 4,
                                0x100, 1, "local",
                                "candidate-coff-remaining-slot-bijection"),
            CandidateAllocation("A", "A.c", "$T2", "rdata", 4, 4, 4,
                                0x108, 1, "local",
                                "candidate-coff-equivalence-class:A-one"),
            CandidateAllocation("A", "A.c", "$T3", "rdata", 8, 4, 4,
                                0x10c, 1, "local",
                                "candidate-coff-equivalence-class:A-one"),
        ]
        retail = b"AAAA\0\0\0\0BBBBBBBB"
        _validate_constrained_reviewed_group(
            "A", "rdata", definitions, coff, allocations, [(0x100, 0x110)],
            lambda rva, size: retail[rva - 0x100:rva - 0x100 + size])

    def test_constrained_group_rejects_uncovered_nonzero_payload(self):
        definitions = [{"name": "$T1", "section": 1, "section_offset": 0}]
        coff = SimpleNamespace(
            data=b"AAAA", sections=[SimpleNamespace(raw_offset=0)])
        allocations = [CandidateAllocation(
            "A", "A.c", "$T1", "rdata", 0, 4, 4, 0x100, 1, "local",
            "candidate-coff-remaining-slot-bijection")]
        retail = b"AAAAXXXX"
        with self.assertRaisesRegex(ValueError, "leaves nonzero retail byte"):
            _validate_constrained_reviewed_group(
                "A", "rdata", definitions, coff, allocations,
                [(0x100, 0x108)],
                lambda rva, size: retail[rva - 0x100:rva - 0x100 + size])

    def test_constrained_group_rejects_singleton_equivalence_class(self):
        definitions = [{"name": "$T1", "section": 1, "section_offset": 0}]
        coff = SimpleNamespace(
            data=b"AAAA", sections=[SimpleNamespace(raw_offset=0)])
        allocations = [CandidateAllocation(
            "A", "A.c", "$T1", "rdata", 0, 4, 4, 0x100, 1, "local",
            "candidate-coff-equivalence-class:A-one")]
        with self.assertRaisesRegex(ValueError, "fewer than two owners"):
            _validate_constrained_reviewed_group(
                "A", "rdata", definitions, coff, allocations,
                [(0x100, 0x104)], lambda _rva, _size: b"AAAA")

    def test_constrained_group_validates_initialized_relocations(self):
        definitions = [
            {"name": "pointer", "section": 1, "section_offset": 0},
            {"name": "target", "section": 1, "section_offset": 4},
        ]
        coff = SimpleNamespace(
            data=b"\0\0\0\0DATA",
            sections=[SimpleNamespace(raw_offset=0)],
            relocations={(1, 0): SimpleNamespace(typ=0x0006, symbol_index=1)},
            symbols={1: SimpleNamespace(name="target")},
        )
        allocations = [
            CandidateAllocation(
                "A", "A.c", "pointer", "data", 0, 4, 4, 0x100, 1,
                "local", "candidate-coff-remaining-slot-bijection"),
            CandidateAllocation(
                "A", "A.c", "target", "data", 4, 4, 4, 0x104, 1,
                "local", "source-DATA:test:1"),
        ]
        retail = b"\x04\x01\x40\x00DATA"
        _validate_constrained_reviewed_group(
            "A", "data", definitions, coff, allocations, [(0x100, 0x108)],
            lambda rva, size: retail[rva - 0x100:rva - 0x100 + size],
            0x400000, {0x100},
            lambda rva: int.from_bytes(
                retail[rva - 0x100:rva - 0x100 + 4], "little"),
            {},
        )
        wrong_target = b"\x08\x01\x40\x00DATA"
        with self.assertRaisesRegex(ValueError, "relocation target differs"):
            _validate_constrained_reviewed_group(
                "A", "data", definitions, coff, allocations,
                [(0x100, 0x108)],
                lambda rva, size: wrong_target[
                    rva - 0x100:rva - 0x100 + size],
                0x400000, {0x100},
                lambda rva: int.from_bytes(
                    wrong_target[rva - 0x100:rva - 0x100 + 4], "little"),
                {},
            )

    def test_virtual_section_reader_zero_fills_beyond_raw_payload(self):
        data = b"xxxxABCD"
        sections = [(0x100, 8, 4, 4)]
        self.assertEqual(
            _virtual_section_bytes(data, sections, 0x102, 6),
            b"CD\0\0\0\0",
        )

    def test_partial_reviewed_owner_preserves_logical_extent(self):
        candidate = {
            "name": "_line", "storage": "data", "section": 2,
            "section_offset": 8, "size": 8, "alignment": 8,
            "scope": "local",
        }
        reviewed = {("A.c", "_line"): {
            "name": "_line", "object": "A.c", "rva": "0x120",
            "size": "0x4", "storage": "data", "alignment": "0x8",
            "section_ordinal": "2", "section_offset": "0x8",
            "scope": "local", "provenance": "source-DATA:test:1",
        }}
        allocation = _reviewed_candidate_allocation(
            "A", "data", candidate, reviewed)
        self.assertEqual(allocation.rva, 0x120)
        self.assertEqual(allocation.size, 4)

    def test_private_counter_drift_is_not_a_reviewed_identity(self):
        candidate = {
            "name": "$SG2", "storage": "data", "section": 2,
            "section_offset": 8, "size": 8, "alignment": 8,
            "scope": "local",
        }
        row = {
            "name": "$SG1", "object": "A.c", "rva": "0x120",
            "size": "0x4", "storage": "data", "alignment": "0x8",
            "section_ordinal": "2", "section_offset": "0x8",
            "scope": "local", "provenance": "reviewed",
        }
        allocation = _reviewed_candidate_allocation(
            "A", "data", candidate, {}, {("A.c", 2, 8): row})
        self.assertIsNone(allocation)

    def test_exact_raw_name_does_not_override_wrong_position(self):
        candidate = {
            "name": "$SG2", "storage": "data", "section": 2,
            "section_offset": 8, "size": 4, "alignment": 4,
            "scope": "local",
        }
        wrong_raw = {
            "name": "$SG2", "object": "A.c", "rva": "0x140",
            "size": "0x4", "storage": "data", "alignment": "0x4",
            "section_ordinal": "2", "section_offset": "0xc",
            "scope": "local", "provenance": "reviewed",
        }
        correct_position = dict(
            wrong_raw, name="$SG1", rva="0x120", section_offset="0x8")
        with self.assertRaisesRegex(ValueError, "topology mismatch"):
            _reviewed_candidate_allocation(
                "A", "data", candidate, {("A.c", "$SG2"): wrong_raw},
                {("A.c", 2, 8): correct_position,
                 ("A.c", 2, 12): wrong_raw})

    def test_data_compgen_semantic_identity_binds_by_exact_position(self):
        candidate = {
            "name": "$SG2", "storage": "data", "section": 2,
            "section_offset": 8, "size": 4, "alignment": 4,
            "scope": "local",
        }
        semantic = {
            "name": "meaningful", "object": "A.c", "rva": "0x120",
            "size": "0x4", "storage": "data", "alignment": "0x4",
            "section_ordinal": "2", "section_offset": "0x8",
            "scope": "local", "provenance": "source-DATA_COMPGEN:A.cpp:1",
        }
        allocation = _reviewed_candidate_allocation(
            "A", "data", candidate, {}, {("A.c", 2, 8): semantic})
        self.assertEqual(allocation.name, "meaningful")
        self.assertEqual(allocation.rva, 0x120)

        wrong_topology = dict(semantic, storage="rdata")
        with self.assertRaisesRegex(ValueError, "topology mismatch"):
            _reviewed_candidate_allocation(
                "A", "data", candidate, {}, {("A.c", 2, 8): wrong_topology})

    def test_function_relocation_offsets_must_align_exactly(self):
        candidate = [
            (0x10, object(), 0),
            (0x28, object(), 0),
        ]
        self.assertTrue(_function_relocation_offsets_align(
            candidate, 0x4000, [0x4010, 0x4028]))
        self.assertFalse(_function_relocation_offsets_align(
            candidate, 0x4000, [0x4010, 0x402C]))

    def test_partial_function_pairs_only_identical_relocation_sites(self):
        from types import SimpleNamespace

        local_one = SimpleNamespace(name="$SG1")
        anchor = SimpleNamespace(name="?known@@3HA")
        shifted_local = SimpleNamespace(name="$SG2")
        candidate = [
            (0x10, local_one, 0),
            (0x20, anchor, 4),
            (0x30, shifted_local, 0),
        ]
        retail_values = {
            0x4010: 0x400000 + 0x700,
            0x4020: 0x400000 + 0x204,
            0x4034: 0x400000 + 0x900,
        }
        result = _function_relocation_proofs(
            candidate, 0x4000, sorted(retail_values), retail_values.__getitem__,
            0x400000, {"$SG1": {}, "$SG2": {}}, {"?known@@3HA": 0x200})
        proposed, anchors, paired, offsets_align, valid = result
        self.assertEqual(proposed, [("$SG1", 0x700)])
        self.assertEqual(anchors, 1)
        self.assertEqual(paired, 2)
        self.assertFalse(offsets_align)
        self.assertTrue(valid)

    def test_mismatching_anchor_does_not_discard_other_exact_sites(self):
        private = SimpleNamespace(name="$SG1")
        wrong_anchor = SimpleNamespace(name="?wrong@@3HA")
        good_anchor = SimpleNamespace(name="?good@@3HA")
        candidate = [
            (0x10, private, 0),
            (0x20, wrong_anchor, 0),
            (0x30, good_anchor, 0),
        ]
        values = {
            0x4010: 0x400000 + 0x700,
            0x4020: 0x400000 + 0x999,
            0x4030: 0x400000 + 0x300,
        }
        proposed, anchors, paired, _aligned, valid = \
            _function_relocation_proofs(
                candidate, 0x4000, sorted(values), values.__getitem__,
                0x400000, {"$SG1": {}},
                {"?wrong@@3HA": 0x200, "?good@@3HA": 0x300})
        self.assertEqual(proposed, [("$SG1", 0x700)])
        self.assertEqual(anchors, 1)
        self.assertEqual(paired, 3)
        self.assertTrue(valid)

    def test_validated_sequence_pairs_shifted_private_relocation(self):
        private = SimpleNamespace(name="$SG1")
        anchor = SimpleNamespace(name="?known@@3HA")
        candidate = [(0x11, private, 0), (0x21, anchor, 4)]
        retail_sites = [0x4010, 0x4020]
        values = {
            0x4010: 0x400000 + 0x700,
            0x4020: 0x400000 + 0x204,
        }
        proposed, anchors, valid = _function_sequence_relocation_proofs(
            candidate, retail_sites, values.__getitem__, 0x400000,
            {"$SG1": {}}, {"?known@@3HA": 0x200})
        self.assertEqual(proposed, [("$SG1", 0x700)])
        self.assertEqual(anchors, 1)
        self.assertTrue(valid)

    def test_relocation_proof_cannot_override_initialized_payload(self):
        from types import SimpleNamespace

        coff = SimpleNamespace(
            data=b"retail payload\0",
            sections=[SimpleNamespace(raw_offset=0)],
            relocations={},
        )
        row = {
            "storage": "data",
            "section": 1,
            "symbol_offset": 0,
            "size": len(coff.data),
        }
        self.assertTrue(_payload_matches_at(
            row, coff, 0x100, [], lambda _rva, _size: coff.data))
        self.assertFalse(_payload_matches_at(
            row, coff, 0x100, [], lambda _rva, size: b"wrong".ljust(size, b"\0")))

    def test_replay_payload_requires_a_unique_retail_occurrence(self):
        coff = SimpleNamespace(
            data=b"same",
            sections=[SimpleNamespace(raw_offset=0)],
            relocations={},
        )
        row = {
            "storage": "data",
            "section": 1,
            "symbol_offset": 0,
            "size": 4,
        }
        retail = b"same----same"

        def read_bytes(rva, size):
            return retail[rva - 0x100:rva - 0x100 + size]

        self.assertEqual(
            _payload_rvas(row, coff, [(0x100, 0x10C)], [], read_bytes, {}),
            [0x100, 0x108],
        )
        self.assertEqual(
            _payload_rvas(row, coff, [(0x100, 0x108)], [], read_bytes, {}),
            [0x100],
        )

    def test_bounded_section_delta_maps_duplicate_payloads_in_order(self):
        coff = SimpleNamespace(
            data=b"leftsamesameright",
            sections=[SimpleNamespace(raw_offset=0)],
            relocations={},
        )
        group = [
            {"name": "left", "storage": "data", "section": 1,
             "symbol_offset": 0, "section_offset": 0, "size": 4},
            {"name": "same_one", "storage": "data", "section": 1,
             "symbol_offset": 4, "section_offset": 4, "size": 4},
            {"name": "same_two", "storage": "data", "section": 1,
             "symbol_offset": 8, "section_offset": 8, "size": 4},
            {"name": "right", "storage": "data", "section": 1,
             "symbol_offset": 12, "section_offset": 12, "size": 5},
        ]
        retail = b"xxxxleftsamesamerightyyyy"

        def read_bytes(rva, size):
            return retail[rva - 0x100:rva - 0x100 + size]

        mapped = {"left": (0x104, 1), "right": (0x110, 1)}
        evidence = {"left": "anchor", "right": "anchor"}
        _bounded_section_delta_proofs(
            group, mapped, evidence, coff, [(0x104, 0x115)], [], read_bytes)
        self.assertEqual(mapped["same_one"], (0x108, 2))
        self.assertEqual(mapped["same_two"], (0x10C, 2))
        self.assertEqual(evidence["same_one"], "bounded-section-delta")

    def test_bounded_section_delta_rejects_disagreeing_anchors(self):
        coff = SimpleNamespace(
            data=b"leftsameright",
            sections=[SimpleNamespace(raw_offset=0)],
            relocations={},
        )
        group = [
            {"name": "left", "storage": "data", "section": 1,
             "symbol_offset": 0, "section_offset": 0, "size": 4},
            {"name": "same", "storage": "data", "section": 1,
             "symbol_offset": 4, "section_offset": 4, "size": 4},
            {"name": "right", "storage": "data", "section": 1,
             "symbol_offset": 8, "section_offset": 8, "size": 5},
        ]
        mapped = {"left": (0x104, 1), "right": (0x114, 1)}
        evidence = {"left": "anchor", "right": "anchor"}
        _bounded_section_delta_proofs(
            group, mapped, evidence, coff, [(0x100, 0x120)], [],
            lambda _rva, size: b"same"[:size])
        self.assertNotIn("same", mapped)

    def test_fully_reviewed_group_closes_with_reordered_retail_owners(self):
        definitions = [
            {"name": "_row", "storage": "bss", "section": 3,
             "section_offset": 0, "size": 4, "alignment": 4,
             "scope": "local"},
            {"name": "_mode", "storage": "bss", "section": 3,
             "section_offset": 4, "size": 4, "alignment": 4,
             "scope": "local"},
        ]
        reviewed = {
            ("BASE\\TILE.c", "_row"): {
                "name": "_row", "object": "BASE\\TILE.c", "rva": "0x104",
                "size": "0x4", "storage": "bss", "alignment": "0x4",
                "section_ordinal": "3", "section_offset": "0x0",
                "scope": "local", "provenance": "source-DATA:test:1",
            },
            ("BASE\\TILE.c", "_mode"): {
                "name": "_mode", "object": "BASE\\TILE.c", "rva": "0x100",
                "size": "0x4", "storage": "bss", "alignment": "0x4",
                "section_ordinal": "3", "section_offset": "0x4",
                "scope": "local", "provenance": "source-DATA:test:2",
            },
        }
        allocations = _reviewed_group_allocations(
            "BASE/TILE", "bss", definitions, reviewed)
        self.assertEqual([(row.name, row.rva) for row in allocations],
                         [("_row", 0x104), ("_mode", 0x100)])

    def test_partially_reviewed_group_remains_open(self):
        definitions = [
            {"name": "_one", "storage": "data", "section": 2,
             "section_offset": 0, "size": 4, "alignment": 4,
             "scope": "local"},
            {"name": "_two", "storage": "data", "section": 2,
             "section_offset": 4, "size": 4, "alignment": 4,
             "scope": "local"},
        ]
        reviewed = {("A.c", "_one"): {
            "name": "_one", "object": "A.c", "rva": "0x100",
            "size": "0x4", "storage": "data", "alignment": "0x4",
            "section_ordinal": "2", "section_offset": "0x0",
            "scope": "local", "provenance": "reviewed",
        }}
        self.assertIsNone(_reviewed_group_allocations(
            "A", "data", definitions, reviewed))

    def test_interval_containment_is_extent_aware(self):
        intervals = [(0x100, 0x120)]
        self.assertTrue(_contains(intervals, 0x100, 0x20))
        self.assertTrue(_contains(intervals, 0x11F))
        self.assertFalse(_contains(intervals, 0x0FF))
        self.assertFalse(_contains(intervals, 0x11F, 2))

    def test_diagnostics_are_machine_readable_and_count_overlapping_causes(self):
        from homm2.build.candidate_data_manifest import DerivationStats, GroupDiagnostic
        import json

        rows = [
            GroupDiagnostic("A", "data", ("unmapped", "uncovered"), ("first",)),
            GroupDiagnostic("B", "data", ("uncovered",), ("second",)),
        ]
        payload = json.loads(diagnostics_bytes(DerivationStats(open_groups=2), rows))
        self.assertEqual(payload["open_by_storage"], {"data": 2})
        self.assertEqual(payload["open_by_cause"], {"uncovered": 2, "unmapped": 1})

    def test_diagnostics_separate_evidenced_from_closed_definitions(self):
        from homm2.build.candidate_data_manifest import DerivationStats
        import json

        stats = DerivationStats(
            candidate_definitions=10, evidenced_definitions=8,
            mapped_definitions=3, closed_groups=1, open_groups=1,
        )
        payload = json.loads(diagnostics_bytes(stats, []))
        self.assertEqual(payload["stats"]["candidate_definitions"], 10)
        self.assertEqual(payload["stats"]["evidenced_definitions"], 8)
        self.assertEqual(payload["stats"]["mapped_definitions"], 3)

    def test_philai_crt_initializer_is_a_real_candidate_definition(self):
        path = REPO / "build/objdiff/base/SOURCE/PHILAI.obj"
        if not path.is_file():
            self.skipTest("candidate objects are not built")
        definitions, _coff = candidate_definitions(path, "SOURCE/PHILAI")
        crt = [row for row in definitions if row["name"].startswith("_$S")]
        self.assertEqual(len(crt), 1)
        self.assertEqual(crt[0]["storage"], "data")
        self.assertEqual(crt[0]["size"], 4)
        self.assertEqual(crt[0]["scope"], "local")

    def _fixture_pe(self, root):
        import struct
        data_rva, data_size = 0x2000, 0x40
        dos = b"MZ" + b"\0" * 58 + struct.pack("<I", 0x40)
        coff = struct.pack("<HHIIIHH", 0x14C, 1, 0, 0, 0, 224, 0x102)
        optional = bytearray(224)
        struct.pack_into("<H", optional, 0, 0x10B)
        struct.pack_into("<I", optional, 28, 0x400000)      # image base
        struct.pack_into("<I", optional, 92, 16)            # directory count
        # every data directory zero, including base relocations (index 5)
        section = struct.pack(
            "<8sIIIIIIHHI", b".data\0\0\0", data_size, data_rva, data_size,
            0x200, 0, 0, 0, 0, 0xC0000040)
        payload = bytes(range(data_size))
        pe = root / "fixture.exe"
        blob = dos + b"PE\0\0" + coff + bytes(optional) + section
        pe.write_bytes(blob + b"\0" * (0x200 - len(blob)) + payload)
        return pe

    def test_pe_layout_reads_sites_from_the_reviewed_manifest(self):
        from tempfile import TemporaryDirectory
        from pathlib import Path
        from unittest import mock
        from homm2.build.candidate_data_manifest import _pe_layout

        with TemporaryDirectory() as directory:
            root = Path(directory)
            pe = self._fixture_pe(root)
            manifest = root / "delink_relocs.tsv"
            manifest.write_text(
                "# reviewed sites\nsite_rva\tkind\n0x2010\tdir32\n0x2004\tdir32\n")
            with mock.patch(
                    "homm2.build.candidate_data_manifest.RELOC_MANIFEST", manifest):
                image_base, highlow, read_u32, _read_bytes = _pe_layout(pe)
        self.assertEqual(image_base, 0x400000)
        self.assertEqual(highlow, [0x2004, 0x2010])
        self.assertEqual(read_u32(0x2000), 0x03020100)

    def test_pe_layout_empty_manifest_means_zero_sites(self):
        from tempfile import TemporaryDirectory
        from pathlib import Path
        from unittest import mock
        from homm2.build.candidate_data_manifest import _pe_layout

        with TemporaryDirectory() as directory:
            root = Path(directory)
            pe = self._fixture_pe(root)
            manifest = root / "delink_relocs.tsv"
            manifest.write_text("site_rva\tkind\n")
            with mock.patch(
                    "homm2.build.candidate_data_manifest.RELOC_MANIFEST", manifest):
                _image_base, highlow, _read_u32, _read_bytes = _pe_layout(pe)
        self.assertEqual(highlow, [])

    def test_candidate_scope_is_read_per_symbol(self):
        path = REPO / "build/objdiff/base/BASE/Bzip.obj"
        if not path.is_file():
            self.skipTest("candidate objects are not built")
        definitions, _coff = candidate_definitions(path, "BASE/Bzip")
        by_name = {row["name"]: row for row in definitions}
        self.assertEqual(by_name["?crc32Table@@3PAIA"]["scope"], "external")
        self.assertTrue(any(
            row["name"].startswith("$SG") and row["scope"] == "local"
            for row in definitions))


if __name__ == "__main__":
    unittest.main()
