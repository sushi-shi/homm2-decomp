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

    def test_readme_contains_only_live_metrics(self):
        block = status.readme_block(self.report())
        self.assertIn("1 / 2 functions exact", block)
        self.assertIn("98.20% fuzzy", block)
        self.assertNotIn("exact-max", block)
        self.assertNotIn("fuzzy-max", block)

    def test_status_prints_live_exact_count(self):
        with mock.patch("builtins.print") as output:
            self.assertEqual(status.main([], data=self.report()), 0)
        rendered = "\n".join(" ".join(str(arg) for arg in call.args)
                             for call in output.call_args_list)
        self.assertIn("matched-code-bytes: 40.00%", rendered)
        self.assertIn("functions-exact: 1/2", rendered)
        self.assertNotIn("max", rendered)

    def test_removed_ratchet_commands_are_rejected(self):
        for command in ("update", "check"):
            with self.subTest(command=command):
                self.assertEqual(status.main([command], data=self.report()), 1)

    def test_force_option_reaches_report_loader(self):
        with mock.patch.object(status, "load_report", return_value=self.report()) as loader:
            self.assertEqual(status.main(["--force-refresh"]), 0)
        loader.assert_called_once_with(force_refresh=True)


if __name__ == "__main__":
    unittest.main()
