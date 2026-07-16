import unittest
from types import SimpleNamespace

from homm2.build.candidate_data_manifest import (
    REPO,
    _contains,
    _function_relocation_offsets_align,
    _function_relocation_proofs,
    _payload_rvas,
    _reviewed_group_allocations,
    candidate_definitions,
    diagnostics_bytes,
    derive_allocations,
)


class CandidateDataManifestTest(unittest.TestCase):
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

    def test_icondf2b_candidate_topology_and_retail_bijection(self):
        path = REPO / "build/objdiff/base/BASE/Icondf2b.obj"
        if not path.is_file():
            self.skipTest("candidate objects are not built")
        definitions, _coff = candidate_definitions(path, "BASE/Icondf2b")
        local = [row for row in definitions if row["storage"] == "bss"]
        self.assertEqual(len(local), 13)
        self.assertEqual([row["section_offset"] for row in local],
                         list(range(0, 0x34, 4)))
        self.assertTrue(all(row["size"] == 4 and row["scope"] == "local"
                            for row in local))

        allocations, _stats, diagnostics = derive_allocations()
        mapped = [row for row in allocations if row.unit == "BASE/Icondf2b"]
        self.assertEqual(len(mapped), 13, diagnostics)
        self.assertEqual({row.rva for row in mapped}, set(range(0x1381B8, 0x1381EC, 4)))
        self.assertTrue(all(row.proof_count >= 1 for row in mapped))
        self.assertTrue(all(row.provenance == "candidate-coff-section-translation"
                            for row in mapped))
        self.assertFalse(any(
            row.storage == "bss" and row.unit in ("BASE/BUTTON", "BASE/Blur")
            for row in allocations))
        missing_contribution = {
            (row.unit, row.storage) for row in diagnostics
            if "retail_contribution_missing" in row.causes
        }
        self.assertNotIn(("BASE/Blur", "bss"), missing_contribution)
        self.assertNotIn(("BASE/BUTTON", "bss"), missing_contribution)

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
