from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


SCRIPT = Path(__file__).with_name("recover_residual_functions.py")
SPEC = importlib.util.spec_from_file_location("recover_residual_functions", SCRIPT)
residual = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = residual
SPEC.loader.exec_module(residual)


class RecoverResidualFunctionsTests(unittest.TestCase):
    def test_normalized_queue_joins_current_hash_and_maximum(self):
        raw = [
            {
                "rank": "1",
                "fuzzy": "99.500000",
                "unit": "SOURCE/ONE",
                "rva": "0x00001234",
                "size": "100",
                "name": "?One@@YAXXZ",
                "demangled": "void One()",
            }
        ]
        key = ("SOURCE/ONE", "?One@@YAXXZ")
        rows, stale = residual.normalized_queue_rows(
            raw, {key: "abc123"}, {key: (99.75, "abc123")}, {}
        )
        self.assertEqual(stale, [])
        self.assertEqual(
            rows[0],
            {
                "rank": "1",
                "priority": "",
                "kind": "source",
                "unit": "SOURCE/ONE",
                "rva": "0x00001234",
                "symbol": "?One@@YAXXZ",
                "current_hash": "abc123",
                "current_max": "99.7500",
                "live_score": "99.500000",
                "size": "100",
                "demangled": "void One()",
            },
        )

    def test_normalized_queue_rejects_stale_ledger_epoch(self):
        raw = [
            {
                "rank": "1",
                "fuzzy": "90.0",
                "unit": "BASE/ONE",
                "rva": "0x10",
                "size": "4",
                "name": "?One@@YAXXZ",
                "demangled": "void One()",
            }
        ]
        key = ("BASE/ONE", "?One@@YAXXZ")
        rows, stale = residual.normalized_queue_rows(
            raw, {key: "new"}, {key: (100.0, "old")}, {}
        )
        self.assertEqual(rows, [])
        self.assertEqual(stale, [key])

    def test_generated_residual_uses_normalized_object_epoch(self):
        raw = [
            {
                "rank": "9",
                "fuzzy": "0.0",
                "unit": "BASE/ONE",
                "rva": "0x20",
                "size": "8",
                "name": "??_GOne@@UAEPAXI@Z",
                "demangled": "deleting destructor",
            }
        ]
        rows, stale = residual.normalized_queue_rows(
            raw, {}, {}, {"BASE/ONE": "deadbeef"}
        )
        self.assertEqual(stale, [])
        self.assertEqual(rows[0]["kind"], "generated")
        self.assertEqual(rows[0]["current_hash"], "object-deadbeef")

    def test_order_prioritizes_lost_exact_then_93_percent_then_rest(self):
        rows = [
            {
                "unit": "U",
                "symbol": "low",
                "live_score": "50",
                "rva": "0x30",
            },
            {
                "unit": "U",
                "symbol": "high",
                "live_score": "99",
                "rva": "0x20",
            },
            {
                "unit": "U",
                "symbol": "lost",
                "live_score": "80",
                "rva": "0x10",
            },
        ]
        ordered = residual.order_queue(rows, {("U", "lost")})
        self.assertEqual([row["symbol"] for row in ordered], ["lost", "high", "low"])
        self.assertEqual([row["priority"] for row in ordered], ["1", "2", "3"])

    def test_data_totals_cover_every_report_unit(self):
        report = {
            "units": [
                {"measures": {"matched_data": "4", "total_data": "4"}},
                {"measures": {"matched_data": "7", "total_data": "9"}},
                {"measures": {}},
            ]
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            path = root / "build/objdiff/report.json"
            path.parent.mkdir(parents=True)
            path.write_text(json.dumps(report))
            self.assertEqual(residual.data_totals(root), (11, 13))


if __name__ == "__main__":
    unittest.main()
