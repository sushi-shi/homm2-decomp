import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.match import status


class BaselineTest(unittest.TestCase):
    def test_update_excludes_functions_without_source_hash(self):
        generated = ("SOURCE/PHILAI", "_$E1")
        source_backed = ("SOURCE/PHILAI", "?EvaluateTownEvent@@")
        merged = status._updated_baseline(
            {generated: 100.0, source_backed: 98.5},
            {source_backed: (99.0, "abc123")},
            {source_backed: "abc123"},
        )
        self.assertNotIn(generated, merged)
        self.assertEqual(merged[source_backed], (99.0, "abc123"))

    def test_blank_four_column_rows_are_not_loaded(self):
        with tempfile.TemporaryDirectory() as directory:
            baseline = Path(directory) / "baseline.tsv"
            baseline.write_text(
                "SOURCE/PHILAI\t_$E1\t100.0000\t\n"
                "SOURCE/PHILAI\t?EvaluateTownEvent@@\t99.0000\tabc123\n"
                "SOURCE/legacy\t?Old@@\t88.0000\n"
            )
            with mock.patch.object(status, "BASELINE", baseline):
                loaded = status.load_baseline()
        self.assertNotIn(("SOURCE/PHILAI", "_$E1"), loaded)
        self.assertEqual(loaded[("SOURCE/PHILAI", "?EvaluateTownEvent@@")],
                         (99.0, "abc123"))
        self.assertEqual(loaded[("SOURCE/legacy", "?Old@@")], (88.0, None))

    def test_writer_never_emits_a_blank_hash_column(self):
        with tempfile.TemporaryDirectory() as directory:
            baseline = Path(directory) / "baseline.tsv"
            with mock.patch.object(status, "BASELINE", baseline):
                status.write_baseline({
                    ("SOURCE/PHILAI", "_$E1"): (100.0, None),
                    ("SOURCE/PHILAI", "?EvaluateTownEvent@@"): (99.0, "abc123"),
                })
            text = baseline.read_text()
        self.assertNotIn("_$E1", text)
        self.assertFalse(any(line.endswith("\t") for line in text.splitlines()))

    def test_source_less_function_remains_in_live_readme_counts(self):
        data = {
            "units": [{
                "name": "SOURCE/PHILAI",
                "measures": {"matched_functions": 1, "total_functions": 1},
                "functions": [{"name": "_$E1", "size": 26,
                               "fuzzy_match_percent": 100.0}],
            }]
        }
        block = status.readme_block(data, {})
        self.assertIn("Functions exact-max", block)
        self.assertEqual(block.count("1 / 1 (100.0%)"), 2)

    def test_retained_exact_is_counted_separately_from_live_exact(self):
        key = ("SOURCE/PHILAI", "?EvaluateTownEvent@@")
        data = {
            "units": [{
                "name": key[0],
                "measures": {"matched_functions": 0, "total_functions": 1},
                "functions": [{"name": key[1], "size": 100,
                               "fuzzy_match_percent": 95.0}],
            }]
        }
        block = status.readme_block(data, {key: (100.0, "abc123")})
        self.assertIn("0 / 1 (0.0%)", block)
        self.assertIn("1 / 1 (100.0%)", block)

    def test_near_exact_retained_score_is_not_counted_as_exact(self):
        key = ("SOURCE/CMBTMGR", "?CatAttack@@")
        data = {
            "units": [{
                "name": key[0],
                "measures": {"matched_functions": 0, "total_functions": 1},
                "functions": [{"name": key[1], "size": 100,
                               "fuzzy_match_percent": 99.9977}],
            }]
        }
        block = status.readme_block(data, {key: (99.9977, "abc123")})
        self.assertEqual(block.count("0 / 1 (0.0%)"), 2)


if __name__ == "__main__":
    unittest.main()
