import base64
import copy
import hashlib
import subprocess
import tempfile
import unittest
from unittest import mock

from homm2.build.strict_allocations import audit_units, derive_manifest


def _encoded(value):
    return base64.b64encode(value).decode("ascii")


def _symbol(name, address=0, data=None):
    symbol = {
        "name": name,
        "address": str(address),
        "size": str(max(len(data or b""), 4)),
        "kind": "SYMBOL_OBJECT",
        "section": 0,
        "flags": {"size_inferred": True},
    }
    if data is not None:
        symbol["data_diff"] = [{"data": _encoded(data), "size": str(len(data))}]
    return symbol


def _relocation(start, target):
    return {
        "relocation": {"type": 6, "target_symbol": target},
        "start": str(start),
        "end": str(start + 4),
    }


def fixture():
    target_table = _symbol("table", 0, b"\0" * 8)
    target_table["data_relocations"] = [_relocation(0, 1), _relocation(4, 2)]
    base_table = _symbol("table", 32, b"\0" * 8)
    base_table["data_relocations"] = [_relocation(32, 1), _relocation(36, 2)]
    diff = {
        "left": {
            "sections": [{"name": ".data", "kind": "SECTION_DATA"}],
            "symbols": [target_table, _symbol("const_a"), _symbol("const_b")],
        },
        "right": {
            "sections": [{"name": ".data", "kind": "SECTION_DATA"}],
            "symbols": [base_table, _symbol("$SG1", data=b"same\0"),
                        _symbol("$SG2", data=b"other\0")],
        },
    }
    row = {
        "name": "table",
        "unit": "SOURCE/UNIT",
        "size": 8,
        "retail_rva": 0x2000,
        "retail_sha256": "table-hash",
        "highlow_count": 2,
        "audit": "cstring-pointer-table",
    }
    retail = {
        "sha256": "table-hash",
        "highlow_base_relocation_count": 2,
        "highlow_relative_offsets": [0, 4],
        "cstring_targets": [
            {"rva": 0x3000, "sha256": hashlib.sha256(b"same").hexdigest()},
            {"rva": 0x3010, "sha256": hashlib.sha256(b"other").hexdigest()},
        ],
        "pointer_target_alias_pattern": [0, 1],
    }
    inventory = {
        "const_a": [{"rva": 0x3000}],
        "const_b": [{"rva": 0x3010}],
    }
    return diff, row, retail, inventory


class StrictAllocationAdapterTests(unittest.TestCase):
    def derive(self, diff, rows, retail, inventory):
        with mock.patch(
                "homm2.build.strict_allocations.read_pe_payload_evidence",
                return_value=retail):
            return derive_manifest("SOURCE/UNIT", rows, diff, inventory, "retail.exe")

    def test_derives_payload_proven_bijective_mapping(self):
        diff, row, retail, inventory = fixture()
        inventory["const_a"].append({"rva": 0x9000})
        manifest, excluded = self.derive(diff, [row], retail, inventory)
        self.assertEqual(excluded, [])
        self.assertEqual(manifest["symbol_mappings"], {
            "const_a": "$SG1",
            "const_b": "$SG2",
        })
        self.assertEqual(manifest["allocations"][0]["target_name"], "table")

    def test_rejects_rows_from_another_unit(self):
        diff, row, retail, inventory = fixture()
        row["unit"] = "SOURCE/OTHER"
        with self.assertRaisesRegex(ValueError, "contains rows from SOURCE/OTHER"):
            self.derive(diff, [row], retail, inventory)

    def test_keeps_bytes_rows_outside_literal_mapping(self):
        diff, row, retail, inventory = fixture()
        byte_row = {**row, "name": "bytes", "audit": "bytes", "size": 4}
        manifest, excluded = self.derive(diff, [byte_row], retail, inventory)
        self.assertEqual(excluded, [])
        self.assertEqual(manifest["symbol_mappings"], {})
        self.assertEqual(manifest["allocations"][0]["target_name"], "bytes")

    def test_rejects_unproved_target_rva(self):
        diff, row, retail, inventory = fixture()
        inventory["const_a"][0]["rva"] += 4
        manifest, excluded = self.derive(diff, [row], retail, inventory)
        self.assertEqual(manifest["allocations"], [])
        self.assertIn("RVA", excluded[0]["reason"])

    def test_rejects_candidate_payload_mismatch(self):
        diff, row, retail, inventory = fixture()
        diff["right"]["symbols"][1]["data_diff"][0]["data"] = _encoded(b"wrong\0")
        manifest, excluded = self.derive(diff, [row], retail, inventory)
        self.assertEqual(manifest["allocations"], [])
        self.assertIn("payload differs", excluded[0]["reason"])

    def test_rejects_duplicate_candidate_literal_identity(self):
        diff, row, retail, inventory = fixture()
        diff["right"]["symbols"].append(copy.deepcopy(diff["right"]["symbols"][1]))
        manifest, excluded = self.derive(diff, [row], retail, inventory)
        self.assertEqual(manifest["allocations"], [])
        self.assertIn("2 defined object identities", excluded[0]["reason"])

    def test_rejects_identity_partition_mismatch(self):
        diff, row, retail, inventory = fixture()
        diff["left"]["symbols"][0]["data_relocations"][1]["relocation"][
            "target_symbol"] = 1
        diff["right"]["symbols"][2]["data_diff"][0]["data"] = _encoded(b"same\0")
        retail["cstring_targets"][1] = dict(retail["cstring_targets"][0])
        manifest, excluded = self.derive(diff, [row], retail, inventory)
        self.assertEqual(manifest["allocations"], [])
        self.assertIn("equality partition differs", excluded[0]["reason"])

    def test_rejects_non_bijective_cross_table_mapping(self):
        diff, row, retail, inventory = fixture()
        diff["right"]["symbols"][2]["data_diff"][0]["data"] = _encoded(b"same\0")
        retail["cstring_targets"][1]["sha256"] = hashlib.sha256(b"same").hexdigest()
        second = copy.deepcopy(row)
        second["name"] = "table_two"
        second["retail_rva"] += 0x20
        target_two = copy.deepcopy(diff["left"]["symbols"][0])
        target_two["name"] = "table_two"
        target_two["address"] = "16"
        target_two["data_relocations"] = [_relocation(16, 1), _relocation(20, 2)]
        base_two = copy.deepcopy(diff["right"]["symbols"][0])
        base_two["name"] = "table_two"
        base_two["address"] = "48"
        base_two["data_relocations"] = [_relocation(48, 2), _relocation(52, 1)]
        diff["left"]["symbols"].append(target_two)
        diff["right"]["symbols"].append(base_two)
        manifest, excluded = self.derive(diff, [row, second], retail, inventory)
        self.assertEqual(manifest["allocations"], [])
        self.assertEqual({item["name"] for item in excluded}, {"table", "table_two"})
        self.assertTrue(all("globally bijective" in item["reason"] for item in excluded))

    def test_audit_rejects_stale_candidate_objects_before_objdiff(self):
        calls = []

        def runner(command, cwd=None):
            calls.append(command)
            return subprocess.CompletedProcess(command, 0, "[1/1] cl SOURCE/UNIT\n", "")

        with tempfile.TemporaryDirectory() as root, mock.patch(
                "homm2.build.strict_allocations.ensure_reviewed_targets"):
            with self.assertRaisesRegex(RuntimeError, "candidate objects are stale"):
                audit_units(output=root, runner=runner)
        self.assertEqual(calls, [["ninja", "-n"]])


if __name__ == "__main__":
    unittest.main()
