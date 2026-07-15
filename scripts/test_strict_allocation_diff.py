import base64
import copy
import unittest

from strict_allocation_diff import audit


def encoded(data: bytes) -> str:
    return base64.b64encode(data).decode("ascii")


def fixture():
    target_symbols = [
        {
            "name": "target_data",
            "address": "0",
            "size": "8",
            "kind": "SYMBOL_OBJECT",
            "section": 0,
            "flags": {"size_inferred": True},
            "data_diff": [{"data": encoded(b"ABCD1234"), "size": "8"}],
            "data_relocations": [
                {
                    "relocation": {"type": 6, "target_symbol": 1, "addend": "4"},
                    "start": "4",
                    "end": "8",
                }
            ],
        },
        {"name": "target_ref", "kind": "SYMBOL_UNKNOWN"},
    ]
    base_symbols = [
        {
            "name": "base_data",
            "address": "0",
            "size": "8",
            "kind": "SYMBOL_OBJECT",
            "section": 0,
            "flags": {"size_inferred": True},
            "data_diff": [{"data": encoded(b"ABCD1234"), "size": "8"}],
            "data_relocations": [
                {
                    "relocation": {"type": 6, "target_symbol": 1, "addend": "4"},
                    "start": "4",
                    "end": "8",
                }
            ],
        },
        {"name": "base_ref", "kind": "SYMBOL_UNKNOWN"},
    ]
    diff = {
        "left": {
            "sections": [{"name": ".data", "kind": "SECTION_DATA", "size": "8"}],
            "symbols": target_symbols,
        },
        "right": {
            "sections": [{"name": ".data", "kind": "SECTION_DATA", "size": "8"}],
            "symbols": base_symbols,
        },
    }
    manifest = {
        "version": 1,
        "symbol_mappings": {"target_data": "base_data", "target_ref": "base_ref"},
        "allocations": [{"target_name": "target_data", "extent": 8}],
    }
    return diff, manifest


class StrictAllocationDiffTests(unittest.TestCase):
    def test_exact_reviewed_inferred_allocation_passes(self):
        diff, manifest = fixture()
        self.assertEqual(audit(diff, manifest), [])

    def test_inferred_extent_requires_review(self):
        diff, manifest = fixture()
        del manifest["allocations"][0]["extent"]
        self.assertIn("is inferred", audit(diff, manifest)[0])

    def test_declared_extent_must_match_review(self):
        diff, manifest = fixture()
        diff["left"]["symbols"][0]["flags"]["size_inferred"] = False
        diff["right"]["symbols"][0]["flags"]["size_inferred"] = False
        manifest["allocations"][0]["extent"] = 4
        self.assertIn("declared extent", audit(diff, manifest)[0])

    def test_missing_allocation_fails(self):
        diff, manifest = fixture()
        diff["right"]["symbols"][0]["name"] = "missing"
        self.assertIn("missing allocation", audit(diff, manifest)[0])

    def test_payload_mismatch_fails(self):
        diff, manifest = fixture()
        diff["right"]["symbols"][0]["data_diff"][0]["data"] = encoded(b"ABCD123X")
        self.assertIn("payload differs", audit(diff, manifest)[0])

    def test_section_kind_mismatch_fails(self):
        diff, manifest = fixture()
        diff["right"]["sections"][0]["kind"] = "SECTION_BSS"
        self.assertIn("section kind differs", audit(diff, manifest)[0])

    def test_section_name_mismatch_fails_without_mapping(self):
        diff, manifest = fixture()
        diff["right"]["sections"][0]["name"] = ".other"
        self.assertIn("section name differs", audit(diff, manifest)[0])

    def test_relocation_target_mismatch_fails(self):
        diff, manifest = fixture()
        diff["right"]["symbols"][1]["name"] = "wrong_ref"
        self.assertIn("relocations differ", audit(diff, manifest)[0])

    def test_relocation_shape_mismatches_fail(self):
        diff, manifest = fixture()
        diff["right"]["symbols"][0]["data_relocations"][0]["start"] = "0"
        self.assertIn("relocations differ", audit(diff, manifest)[0])

        for field, value in (("type", 7), ("addend", "8")):
            with self.subTest(field=field):
                diff, manifest = fixture()
                relocation = diff["right"]["symbols"][0]["data_relocations"][0]
                relocation["relocation"][field] = value
                self.assertIn("relocations differ", audit(diff, manifest)[0])

    def test_explicit_relocation_mapping_checks_each_occurrence(self):
        diff, manifest = fixture()
        manifest["symbol_mappings"] = {"target_data": "base_data"}
        manifest["allocations"][0]["relocation_mappings"] = [{
            "offset": 4,
            "type": 6,
            "addend": 4,
            "target_name": "target_ref",
            "base_name": "base_ref",
        }]
        self.assertEqual(audit(diff, manifest), [])

        manifest["allocations"][0]["relocation_mappings"][0]["base_name"] = "wrong_ref"
        self.assertIn("base relocations differ from reviewed", audit(diff, manifest)[0])

    def test_explicit_relocation_mapping_must_cover_exact_offsets(self):
        diff, manifest = fixture()
        manifest["allocations"][0]["relocation_mappings"] = []
        self.assertIn("target relocations differ from reviewed", audit(diff, manifest)[0])

    def test_negative_section_index_is_rejected(self):
        diff, manifest = fixture()
        diff["left"]["symbols"][0]["section"] = -1
        self.assertIn("section index -1 is invalid", audit(diff, manifest)[0])

    def test_negative_relocation_target_index_is_rejected(self):
        diff, manifest = fixture()
        relocation = diff["left"]["symbols"][0]["data_relocations"][0]
        relocation["relocation"]["target_symbol"] = -1
        self.assertIn("invalid relocation target -1", audit(diff, manifest)[0])

    def test_data_diff_chunks_are_concatenated_without_opposite_side_segments(self):
        diff, manifest = fixture()
        for side in ("left", "right"):
            diff[side]["symbols"][0]["data_diff"] = [
                {"data": encoded(b"ABCD"), "size": 4},
                {"kind": "DIFF_INSERT", "size": 12},
                {"data": encoded(b"1234"), "size": 4},
            ]
        self.assertEqual(audit(diff, manifest), [])

    def test_data_diff_chunk_size_mismatch_is_rejected(self):
        diff, manifest = fixture()
        diff["left"]["symbols"][0]["data_diff"][0]["size"] = 7
        self.assertIn("segment payload has size", audit(diff, manifest)[0])

    def test_unknown_payload_free_data_diff_segment_is_rejected(self):
        diff, manifest = fixture()
        diff["left"]["symbols"][0]["data_diff"] = [{"kind": "UNKNOWN", "size": 8}]
        self.assertIn("neither payload nor diff-side kind", audit(diff, manifest)[0])

    def test_exact_relocation_duplicate_at_data_diff_boundary_is_collapsed(self):
        diff, manifest = fixture()
        for side in ("left", "right"):
            symbol = diff[side]["symbols"][0]
            symbol["data_diff"] = [
                {"data": encoded(b"ABCD12"), "size": 6},
                {"data": encoded(b"34"), "size": 2},
            ]
            symbol["data_relocations"].append(
                copy.deepcopy(symbol["data_relocations"][0]))
        self.assertEqual(audit(diff, manifest), [])

    def test_unexplained_exact_relocation_duplicate_is_rejected(self):
        diff, manifest = fixture()
        diff["left"]["symbols"][0]["data_relocations"].append(
            copy.deepcopy(diff["left"]["symbols"][0]["data_relocations"][0]))
        self.assertIn("repeated 2 times but crosses 0", audit(diff, manifest)[0])

    def test_duplicate_definition_fails(self):
        diff, manifest = fixture()
        diff["left"]["symbols"].insert(1, copy.deepcopy(diff["left"]["symbols"][0]))
        self.assertIn("defined 2 times", audit(diff, manifest)[0])


if __name__ == "__main__":
    unittest.main()
