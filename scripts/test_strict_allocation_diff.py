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

    def test_duplicate_definition_fails(self):
        diff, manifest = fixture()
        diff["left"]["symbols"].insert(1, copy.deepcopy(diff["left"]["symbols"][0]))
        self.assertIn("defined 2 times", audit(diff, manifest)[0])


if __name__ == "__main__":
    unittest.main()
