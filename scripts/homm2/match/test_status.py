import json
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.match import status


class ReportCacheTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        self.objdiff = self.root / "build/objdiff"
        self.base = self.objdiff / "base/SOURCE/UNIT.obj"
        self.target = self.root / "build/delink/SOURCE/UNIT.c.obj"
        self.tool = self.root / "bin/objdiff-cli"
        for path, contents in ((self.base, b"base"), (self.target, b"target"),
                               (self.tool, b"objdiff")):
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(contents)
        self.config = self.objdiff / "objdiff.json"
        self.config.write_text(json.dumps({
            "units": [{
                "name": "SOURCE/UNIT",
                "base_path": "./base/SOURCE/UNIT.obj",
                "target_path": "../delink/SOURCE/UNIT.c.obj",
            }]
        }))
        self.report = self.objdiff / "report.json"
        self.stamp = self.objdiff / status.REPORT_STAMP
        self.report_data = {"units": [], "measures": {"matched_code_percent": 0}}

    def tearDown(self):
        self.temporary.cleanup()

    def identity(self, tool=None):
        return status._report_inputs_identity(self.objdiff, tool or self.tool)

    def seed(self):
        identity = self.identity()
        self.report.write_text(json.dumps(self.report_data))
        status._store_report_stamp(self.report, self.stamp, identity, False)
        return identity

    def cached(self, identity=None, **kwargs):
        return status._load_cached_report(
            self.report, self.stamp, identity or self.identity(), **kwargs)

    def test_unchanged_inputs_reuse_report(self):
        identity = self.seed()
        self.assertEqual(self.cached(identity), self.report_data)

    def test_object_content_changes_invalidate_report(self):
        self.seed()
        self.base.write_bytes(b"new base")
        self.assertIsNone(self.cached())

    def test_base_only_change_is_incremental(self):
        self.seed()
        self.base.write_bytes(b"new base")
        report, units = status._trusted_incremental_base_units(
            self.report, self.stamp, self.identity())
        self.assertEqual(report, self.report_data)
        self.assertEqual(units, ["SOURCE/UNIT"])

    def test_target_change_is_not_incremental(self):
        self.seed()
        self.target.write_bytes(b"new target")
        report, units = status._trusted_incremental_base_units(
            self.report, self.stamp, self.identity())
        self.assertIsNone(report)
        self.assertIsNone(units)

    def test_executable_change_invalidates_report(self):
        self.seed()
        self.tool.write_bytes(b"new objdiff")
        self.assertIsNone(self.cached())

    def test_force_refresh_invalidates_report(self):
        identity = self.seed()
        self.assertIsNone(self.cached(identity, force_refresh=True))

    def test_modified_report_invalidates_cache(self):
        identity = self.seed()
        self.report.write_text(json.dumps({"units": [], "measures": {}}))
        self.assertIsNone(self.cached(identity))


class LiveStatusTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.maxima = Path(self.temporary.name) / "match_baseline.tsv"
        self.maxima_patch = mock.patch.object(status, "MAXIMA", self.maxima)
        self.maxima_patch.start()
        self.hashes_patch = mock.patch.object(status, "source_hashes", return_value={
            ("SOURCE/UNIT", "exact"): "hash-exact",
            ("SOURCE/UNIT", "partial"): "hash-partial",
        })
        self.hashes_patch.start()

    def tearDown(self):
        self.hashes_patch.stop()
        self.maxima_patch.stop()
        self.temporary.cleanup()

    @staticmethod
    def report():
        return {
            "units": [{
                "name": "SOURCE/UNIT",
                "measures": {
                    "total_code": "100",
                    "matched_code": "40",
                    "fuzzy_match_percent": 98.25,
                    "total_data": "20",
                    "matched_data": "20",
                    "total_functions": 2,
                    "matched_functions": 1,
                },
                "functions": [
                    {"name": "exact", "size": "40", "fuzzy_match_percent": 100.0},
                    {"name": "partial", "size": "60", "fuzzy_match_percent": 97.0},
                ],
            }],
            "measures": {
                "matched_code_percent": 40.0,
                "fuzzy_match_percent": 98.25,
                "matched_data": "20",
                "total_data": "20",
                "matched_data_percent": 100.0,
                "total_functions": 2,
                "matched_functions": 1,
            },
        }

    def test_readme_contains_live_and_retained_maxima(self):
        maxima = {
            ("SOURCE/UNIT", "exact"): (100.0, "hash-exact"),
            ("SOURCE/UNIT", "partial"): (100.0, "hash-partial"),
        }
        block = status.readme_block(self.report(), maxima)
        self.assertIn("1 / 2 functions exact", block)
        self.assertIn("2 / 2 functions exact-max", block)
        self.assertIn("98.20% fuzzy", block)
        self.assertIn("100.00% fuzzy-max", block)

    def test_status_prints_live_and_max_exact_counts(self):
        self.maxima.write_text(
            "SOURCE/UNIT\texact\t100.0000\thash-exact\n"
            "SOURCE/UNIT\tpartial\t100.0000\thash-partial\n")
        with mock.patch("builtins.print") as output:
            self.assertEqual(status.main([], data=self.report()), 0)
        rendered = "\n".join(" ".join(str(arg) for arg in call.args)
                             for call in output.call_args_list)
        self.assertIn("matched-code-bytes: 40.00%", rendered)
        self.assertIn("functions-exact: 1/2", rendered)
        self.assertIn("functions-exact-max: 2/2", rendered)
        self.assertIn("fuzzy-max: 100.00%", rendered)

    def test_status_does_not_collapse_duplicate_source_less_names(self):
        report = self.report()
        report["units"][0]["functions"].append(
            {"name": "exact", "size": "10", "fuzzy_match_percent": 100.0})
        report["units"][0]["measures"].update({
            "total_functions": 3,
            "matched_functions": 2,
        })
        report["measures"].update({"total_functions": 3, "matched_functions": 2})
        with mock.patch("builtins.print") as output:
            self.assertEqual(status.main([], data=report), 0)
        rendered = "\n".join(" ".join(str(arg) for arg in call.args)
                             for call in output.call_args_list)
        self.assertIn("functions-exact-max: 2/3", rendered)

    def test_recording_never_lowers_maximum_for_same_hash(self):
        self.maxima.write_text(
            "SOURCE/UNIT\texact\t100.0000\thash-exact\n"
            "SOURCE/UNIT\tpartial\t99.0000\thash-partial\n")
        maxima = status.record_maxima(self.report())
        self.assertEqual(maxima[("SOURCE/UNIT", "exact")], (100.0, "hash-exact"))
        self.assertEqual(maxima[("SOURCE/UNIT", "partial")], (99.0, "hash-partial"))
        rendered = self.maxima.read_text()
        self.assertIn("never an enforcement baseline", rendered)
        self.assertIn("99.0000\thash-partial", rendered)

    def test_changed_hash_resets_maximum_without_rejecting_regression(self):
        self.maxima.write_text(
            "SOURCE/UNIT\texact\t100.0000\thash-exact\n"
            "SOURCE/UNIT\tpartial\t99.0000\told-hash\n")
        maxima = status.record_maxima(self.report())
        self.assertEqual(maxima[("SOURCE/UNIT", "partial")],
                         (97.0, "hash-partial"))

    def test_recording_adds_new_observations(self):
        maxima = status.record_maxima(self.report())
        self.assertEqual(maxima[("SOURCE/UNIT", "partial")], (97.0, "hash-partial"))
        self.assertEqual(status.load_maxima(), maxima)

    def test_removed_ratchet_check_is_rejected(self):
        self.assertEqual(status.main(["check"], data=self.report()), 1)
        self.assertFalse(self.maxima.exists())

    def test_update_command_records_without_enforcement(self):
        self.assertEqual(status.main(["update"], data=self.report()), 0)
        self.assertEqual(status.load_maxima()[("SOURCE/UNIT", "partial")],
                         (97.0, "hash-partial"))

    def test_force_option_reaches_report_loader(self):
        with mock.patch.object(status, "load_report", return_value=self.report()) as loader:
            self.assertEqual(status.main(["--force-refresh"]), 0)
        loader.assert_called_once_with(force_refresh=True)


if __name__ == "__main__":
    unittest.main()
