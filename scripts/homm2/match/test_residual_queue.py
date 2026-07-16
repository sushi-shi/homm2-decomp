import tempfile
import unittest
from pathlib import Path

from homm2.match.residual_queue import residual_rows, source_markers


class ResidualQueueTest(unittest.TestCase):
    def test_orders_every_nonexact_function_closest_first(self):
        report = {"units": [
            {"name": "SOURCE/A", "functions": [
                {"name": "a", "size": "10", "fuzzy_match_percent": 98.0,
                 "metadata": {"demangled_name": "A"}},
                {"name": "b", "size": "20", "fuzzy_match_percent": 100.0},
            ]},
            {"name": "BASE/B", "functions": [
                {"name": "c", "size": "30", "fuzzy_match_percent": 99.5},
            ]},
        ]}
        symbols = {
            ("SOURCE/A", "a"): {"rva": "0x200"},
            ("SOURCE/A", "b"): {"rva": "0x210"},
            ("BASE/B", "c"): {"rva": "0x100"},
        }
        rows = residual_rows(report, symbols, {0x100: "early-stop"})
        self.assertEqual([row["name"] for row in rows], ["c", "a"])
        self.assertEqual(rows[0]["marker"], "early-stop")
        self.assertEqual([row["rank"] for row in rows], [1, 2])

    def test_prior_markers_are_annotations_not_filters(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "A.cpp").write_text(
                "// @semantic\nVA(0x00400100, 0x4)\nvoid a() {}\n"
                "// @early-stop\nVA(0x00400200, 0x4)\nvoid b() {}\n",
                encoding="latin-1")
            markers = source_markers(root)
        self.assertEqual(markers, {0x100: "semantic", 0x200: "early-stop"})

    def test_missing_rva_inventory_fails_closed(self):
        report = {"units": [{"name": "SOURCE/A", "functions": [
            {"name": "a", "size": "10", "fuzzy_match_percent": 99.0},
        ]}]}
        with self.assertRaisesRegex(ValueError, "lack an RVA inventory"):
            residual_rows(report, {}, {})


if __name__ == "__main__":
    unittest.main()
