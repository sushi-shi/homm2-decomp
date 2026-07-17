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
        }, indent=2) + "\n")
        self.report = self.objdiff / "report.json"
        self.stamp = self.objdiff / status.REPORT_STAMP
        self.report_data = {"units": [], "measures": {"matched_code_percent": 0}}

    def tearDown(self):
        self.temporary.cleanup()

    def _identity(self, tool=None):
        return status._report_inputs_identity(self.objdiff, tool or self.tool)

    def _seed(self):
        identity = self._identity()
        self.report.write_text(json.dumps(self.report_data))
        status._store_report_stamp(self.report, self.stamp, identity, False)
        return identity

    def _cached(self, identity=None, **kwargs):
        return status._load_cached_report(
            self.report, self.stamp, identity or self._identity(), **kwargs)

    def test_unchanged_inputs_reuse_report(self):
        identity = self._seed()
        self.assertEqual(self._cached(identity), self.report_data)

    def test_config_content_change_invalidates_report(self):
        identity = self._seed()
        self.config.write_text(self.config.read_text() + "\n")
        self.assertNotEqual(self._identity(), identity)
        self.assertIsNone(self._cached())

    def test_base_object_content_change_invalidates_report(self):
        self._seed()
        self.base.write_bytes(b"new base")
        self.assertIsNone(self._cached())

    def test_target_object_content_change_invalidates_report(self):
        self._seed()
        self.target.write_bytes(b"new target")
        self.assertIsNone(self._cached())

    def test_base_only_change_is_incremental(self):
        self._seed()
        self.base.write_bytes(b"new base")
        report, units = status._trusted_incremental_base_units(
            self.report, self.stamp, self._identity())
        self.assertEqual(report, self.report_data)
        self.assertEqual(units, ["SOURCE/UNIT"])

    def test_target_change_is_not_incremental(self):
        self._seed()
        self.target.write_bytes(b"new target")
        report, units = status._trusted_incremental_base_units(
            self.report, self.stamp, self._identity())
        self.assertIsNone(report)
        self.assertIsNone(units)

    def test_executable_path_and_content_changes_invalidate_report(self):
        original = self._seed()
        second_tool = self.tool.with_name("objdiff-cli-new")
        second_tool.write_bytes(self.tool.read_bytes())
        moved = self._identity(second_tool)
        self.assertNotEqual(moved, original)
        self.assertIsNone(self._cached(moved))

        self.tool.write_bytes(b"new objdiff")
        replaced = self._identity()
        self.assertNotEqual(replaced, original)
        self.assertIsNone(self._cached(replaced))

    def test_reviewed_target_refresh_invalidates_report(self):
        identity = self._seed()
        self.assertIsNone(self._cached(identity, reviewed_targets_refreshed=True))

    def test_force_refresh_invalidates_report(self):
        identity = self._seed()
        self.assertIsNone(self._cached(identity, force_refresh=True))

    def test_missing_or_corrupt_report_invalidates_cache(self):
        identity = self._seed()
        self.report.unlink()
        self.assertIsNone(self._cached(identity))
        self.report.write_text("not json")
        self.assertIsNone(self._cached(identity))

    def test_missing_or_corrupt_stamp_invalidates_cache(self):
        identity = self._seed()
        self.stamp.unlink()
        self.assertIsNone(self._cached(identity))
        self.stamp.write_text("not json")
        self.assertIsNone(self._cached(identity))

    def test_modified_valid_report_invalidates_cache(self):
        identity = self._seed()
        self.report.write_text(json.dumps({"units": [], "measures": {}}))
        self.assertIsNone(self._cached(identity))

    def test_status_force_option_reaches_report_loader(self):
        data = {"units": [], "measures": {}}
        with mock.patch.object(status, "load_report", return_value=data) as loader, \
                mock.patch.object(status, "load_baseline", return_value={}), \
                mock.patch.object(status, "source_hashes", return_value={}):
            self.assertEqual(status.main(["--force-refresh"]), 0)
        loader.assert_called_once_with(force_refresh=True)

    def test_status_distinguishes_matched_code_bytes_from_fuzzy(self):
        data = {
            "units": [],
            "measures": {
                "matched_code_percent": 36.9699,
                "fuzzy_match_percent": 98.2670,
            },
        }
        with mock.patch.object(status, "load_baseline", return_value={}), \
                mock.patch.object(status, "source_hashes", return_value={}), \
                mock.patch("builtins.print") as output:
            self.assertEqual(status.main([], data=data), 0)
        rendered = "\n".join(" ".join(str(arg) for arg in call.args)
                             for call in output.call_args_list)
        self.assertIn("matched-code-bytes: 36.97%", rendered)
        self.assertIn("fuzzy: 98.27%", rendered)
        self.assertNotIn("overall:", rendered)

    def test_partial_report_replaces_units_and_recomputes_measures(self):
        def unit(name, total, matched, fuzzy):
            return {
                "name": name,
                "measures": {
                    "total_code": str(total),
                    "matched_code": str(matched),
                    "fuzzy_match_percent": fuzzy,
                    "total_functions": 1,
                    "matched_functions": int(total == matched),
                },
                "functions": [],
                "sections": [],
            }

        previous = {"version": 1, "units": [
            unit("SOURCE/A", 100, 10, 80.0),
            unit("SOURCE/B", 300, 30, 90.0),
        ], "measures": {}}
        replacement = unit("SOURCE/A", 100, 100, 100.0)
        partial = {"version": 1, "units": [replacement], "measures": {}}
        merged = status._merge_partial_report(
            previous, partial, ["SOURCE/A", "SOURCE/B"], ["SOURCE/A"])

        self.assertEqual([row["name"] for row in merged["units"]],
                         ["SOURCE/A", "SOURCE/B"])
        self.assertEqual(merged["measures"]["total_code"], "400")
        self.assertEqual(merged["measures"]["matched_code"], "130")
        self.assertAlmostEqual(merged["measures"]["fuzzy_match_percent"], 92.5)
        self.assertAlmostEqual(merged["measures"]["matched_code_percent"], 32.5)


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


class SourceHashBoundaryTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        (self.root / "src/SOURCE").mkdir(parents=True)
        (self.root / "include").mkdir()
        generated = self.root / "build/gen"
        generated.mkdir(parents=True)
        (generated / "symbol_names.csv").write_text(
            "0x1000,?First@@YAHXZ,SOURCE/UNIT,,func\n"
            "0x1100,?Second@@YAHXZ,SOURCE/UNIT,,func\n"
        )
        self.source = self.root / "src/SOURCE/UNIT.cpp"
        self.patch = mock.patch.object(status, "REPO", self.root)
        self.patch.start()

    def tearDown(self):
        self.patch.stop()
        self.temporary.cleanup()

    @staticmethod
    def _source(first_return="localValue + 1", second_return="2",
                between=""):
        return f'''#define BRACE_TEXT {{ ignored_by_lexer }}
VA(0x00401000, 0x20)
int First(void)
{{
    DATA(0x00500000) static int localValue = 1;
    const char *markerText = "}} DATA(0x00500004)";
    /* VA(0x00401100, 0x1) {{ }} */
    if (localValue) {{
VA(0x0040ffff, 0x1)
        localValue++;
    }}
#ifdef KEEP_BODY_BALANCED
    localValue += 0;
#endif
    return {first_return};
}}

{between}
VA(0x00401100, 0x10)
int Second(void)
{{
    return {second_return};
}}
'''

    def test_local_markers_and_nested_braces_stay_in_function_block(self):
        blocks = list(status._source_function_blocks(self._source()))

        self.assertEqual([va for va, _block in blocks], [0x00401000, 0x00401100])
        self.assertIn("DATA(0x00500000)", blocks[0][1])
        self.assertIn("#endif\n    return localValue + 1", blocks[0][1])
        self.assertIn("return localValue + 1", blocks[0][1])
        self.assertNotIn("int Second", blocks[0][1])
        self.assertIn("return 2", blocks[1][1])

    def test_neighboring_va_comments_belong_to_neither_function_hash(self):
        first = ("SOURCE/UNIT", "?First@@YAHXZ")
        second = ("SOURCE/UNIT", "?Second@@YAHXZ")
        self.source.write_text(self._source(
            between="// @semantic: first neighboring marker\n#undef LOCAL_ONLY"))
        before = status.source_hashes()
        self.source.write_text(self._source(
            between="// @early-stop\n// changed neighboring proof\n#undef OTHER"))
        after = status.source_hashes()

        self.assertEqual(before[first], after[first])
        self.assertEqual(before[second], after[second])

    def test_target_body_edit_changes_only_target_hash(self):
        first = ("SOURCE/UNIT", "?First@@YAHXZ")
        second = ("SOURCE/UNIT", "?Second@@YAHXZ")
        self.source.write_text(self._source(
            between="// @early-stop\n// target proof"))
        before = status.source_hashes()
        self.source.write_text(self._source(
            second_return="3", between="// @early-stop\n// target proof"))
        after = status.source_hashes()

        self.assertEqual(before[first], after[first])
        self.assertNotEqual(before[second], after[second])

    def test_file_scope_boundary_preserves_legacy_blank_line_surface(self):
        text = '''VA(0x00401000, 0x1)
int First(void)
{
    return 1;
}


DATA(0x00500000) int globalValue;
'''

        blocks = list(status._source_function_blocks(text))

        self.assertEqual(blocks, [(0x00401000, ''' 0x1)
int First(void)
{
    return 1;
}
''')])

    def test_edit_after_local_data_changes_only_own_hash(self):
        self.source.write_text(self._source())
        before = status.source_hashes()
        self.source.write_text(self._source(first_return="localValue + 2"))
        after_first_edit = status.source_hashes()
        self.source.write_text(self._source(
            first_return="localValue + 2", second_return="3"))
        after_second_edit = status.source_hashes()

        first = ("SOURCE/UNIT", "?First@@YAHXZ")
        second = ("SOURCE/UNIT", "?Second@@YAHXZ")
        self.assertNotEqual(before[first], after_first_edit[first])
        self.assertEqual(before[second], after_first_edit[second])
        self.assertEqual(after_first_edit[first], after_second_edit[first])
        self.assertNotEqual(after_first_edit[second], after_second_edit[second])


if __name__ == "__main__":
    unittest.main()
