import json
import io
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path

from homm2.match.breadth_audit import (
    AuditError, _load_batch, classify, comparison_epoch, exact_max_rows,
    load_state, main, record_audits, write_state,
)


EPOCH_A = "a" * 64
EPOCH_B = "b" * 64


def function(name, fuzzy, size=10):
    return {
        "name": name,
        "size": str(size),
        "fuzzy_match_percent": fuzzy,
        "metadata": {"demangled_name": name + "()"},
    }


class ComparisonEpochTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        self.objdiff = self.root / "build/objdiff"
        self.base = self.objdiff / "base/SOURCE/A.obj"
        self.target = self.objdiff / "target/SOURCE/A.obj"
        self.tool = self.root / "bin/objdiff-cli"
        self.helpers = [
            self.root / "scripts/homm2/build/canonicalize_data_symbols.py",
            self.root / "scripts/homm2/build/assert_relocs.py",
            self.root / "scripts/homm2/build/assert_early_stop_bytes.py",
        ]
        for path, contents in (
                (self.base, b"base-a"), (self.target, b"target-a"),
                (self.tool, b"objdiff-a"),
                (self.helpers[0], b"canonicalizer-a"),
                (self.helpers[1], b"relocs-a"),
                (self.helpers[2], b"early-stop-a")):
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(contents)
        self.config = self.objdiff / "objdiff.json"
        self._write_config()

    def tearDown(self):
        self.temporary.cleanup()

    def _write_config(self, scratch="msvc4.2", base_path="./base/SOURCE/A.obj"):
        self.config.write_text(json.dumps({
            "units": [{
                "name": "SOURCE/A",
                "base_path": base_path,
                "target_path": "./target/SOURCE/A.obj",
                "scratch": {"compiler": scratch},
            }]
        }) + "\n")

    def _epoch(self):
        return comparison_epoch(self.root, self.tool)

    def test_base_object_and_base_path_do_not_change_epoch(self):
        original = self._epoch()
        self.base.write_bytes(b"base-b")
        self._write_config(base_path="./somewhere/else.obj")
        self.assertEqual(self._epoch(), original)

    def test_target_object_changes_epoch(self):
        original = self._epoch()
        self.target.write_bytes(b"target-b")
        self.assertNotEqual(self._epoch(), original)

    def test_comparison_config_changes_epoch(self):
        original = self._epoch()
        self._write_config(scratch="different")
        self.assertNotEqual(self._epoch(), original)

    def test_tool_and_comparison_helpers_change_epoch(self):
        original = self._epoch()
        self.tool.write_bytes(b"objdiff-b")
        tool_changed = self._epoch()
        self.assertNotEqual(tool_changed, original)
        previous = tool_changed
        for index, helper in enumerate(self.helpers):
            helper.write_bytes(("helper-%d-b" % index).encode("ascii"))
            changed = self._epoch()
            self.assertNotEqual(changed, previous)
            previous = changed

    def test_missing_target_fails_closed(self):
        self.target.unlink()
        with self.assertRaisesRegex(AuditError, "target is missing"):
            self._epoch()


class AuditStateTest(unittest.TestCase):
    def setUp(self):
        self.report = {"units": [{
            "name": "SOURCE/A",
            "functions": [
                function("exact", 100.0),
                function("near", 99.9, 20),
                function("far", 90.0, 30),
            ],
        }]}
        self.hashes = {
            ("SOURCE/A", "exact"): "111111111111",
            ("SOURCE/A", "near"): "222222222222",
            ("SOURCE/A", "far"): "333333333333",
        }
        self.empty = {"version": 1, "records": []}

    def _record(self, name="near", **overrides):
        values = {
            "unit": "SOURCE/A", "function": name, "outcome": "parked",
            "iterations": 3, "before": 99.8,
            "after": 99.9 if name == "near" else 90.0,
            "note": "residual reviewed",
        }
        values.update(overrides)
        return values

    def test_exact_needs_no_record_and_queue_is_closest_first(self):
        result = classify(self.report, self.hashes, self.empty, EPOCH_A)
        self.assertEqual([row["function"] for row in result["exact"]], ["exact"])
        self.assertEqual([row["function"] for row in result["pending"]],
                         ["near", "far"])
        self.assertEqual(len(result["checked"]), 0)

    def test_live_score_not_historical_max_controls_queue(self):
        report = {"units": [{"name": "SOURCE/A", "functions": [
            function("formerly-exact", 99.99),
        ]}]}
        hashes = {("SOURCE/A", "formerly-exact"): "444444444444"}
        result = classify(report, hashes, self.empty, EPOCH_A)
        self.assertEqual([], result["exact"])
        self.assertEqual(["formerly-exact"],
                         [row["function"] for row in result["pending"]])

    def test_exact_max_fast_lane_requires_same_source_hash(self):
        result = classify(self.report, self.hashes, self.empty, EPOCH_A)
        baseline = {
            ("SOURCE/A", "near"): (100.0, "222222222222"),
            ("SOURCE/A", "far"): (100.0, "oldoldoldold"),
        }
        self.assertEqual(
            ["near"],
            [row["function"] for row in exact_max_rows(result["pending"], baseline)],
        )

    def test_matching_hash_and_epoch_are_the_only_nonexact_exclusion(self):
        state = record_audits(
            [self._record()], self.report, self.hashes, self.empty, EPOCH_A)
        result = classify(self.report, self.hashes, state, EPOCH_A)
        self.assertEqual([row["function"] for row, _ in result["checked"]], ["near"])
        self.assertEqual([row["function"] for row in result["pending"]], ["far"])

    def test_source_edit_invalidates_only_that_function(self):
        state = record_audits([
            self._record(),
            self._record("far", before=89.0, after=90.0, outcome="improved"),
        ], self.report, self.hashes, self.empty, EPOCH_A)
        changed = dict(self.hashes)
        changed[("SOURCE/A", "near")] = "444444444444"
        result = classify(self.report, changed, state, EPOCH_A)
        self.assertEqual([row["function"] for row, _ in result["checked"]], ["far"])
        self.assertEqual([row["function"] for row in result["pending"]], ["near"])
        self.assertEqual(len(result["stale"]), 1)

    def test_epoch_change_invalidates_every_nonexact_record(self):
        state = record_audits([
            self._record(), self._record("far", before=90.0),
        ], self.report, self.hashes, self.empty, EPOCH_A)
        result = classify(self.report, self.hashes, state, EPOCH_B)
        self.assertEqual(len(result["checked"]), 0)
        self.assertEqual(len(result["pending"]), 2)
        self.assertEqual(len(result["stale"]), 2)

    def test_record_replaces_only_the_same_function(self):
        first = record_audits([
            self._record(), self._record("far", before=90.0),
        ], self.report, self.hashes, self.empty, EPOCH_A)
        changed_hashes = dict(self.hashes)
        changed_hashes[("SOURCE/A", "near")] = "444444444444"
        second = record_audits([
            self._record(source_hash="444444444444")
        ], self.report, changed_hashes, first, EPOCH_A)
        by_name = {row["function"]: row for row in second["records"]}
        self.assertEqual(by_name["near"]["source_hash"], "444444444444")
        self.assertEqual(by_name["far"],
                         {row["function"]: row for row in first["records"]}["far"])

    def test_stale_handoff_fails_closed(self):
        with self.assertRaisesRegex(AuditError, "source hash changed"):
            record_audits([
                self._record(source_hash="999999999999")
            ], self.report, self.hashes, self.empty, EPOCH_A)

    def test_historical_after_survives_tu_state_score_change(self):
        state = record_audits([
            self._record(after=99.8)
        ], self.report, self.hashes, self.empty, EPOCH_A)
        self.assertEqual(state["records"][0]["after"], 99.8)

    def test_outcome_invariants_fail_closed(self):
        with self.assertRaisesRegex(AuditError, "after score of 100%"):
            record_audits([
                self._record(outcome="exact")
            ], self.report, self.hashes, self.empty, EPOCH_A)
        with self.assertRaisesRegex(AuditError, "after > before"):
            record_audits([
                self._record(outcome="improved", before=99.9)
            ], self.report, self.hashes, self.empty, EPOCH_A)

    def test_unknown_state_function_fails_closed(self):
        state = {"version": 1, "records": [{
            "unit": "SOURCE/GONE", "function": "gone",
            "source_hash": "111111111111", "epoch": EPOCH_A,
            "outcome": "parked", "iterations": 0,
            "before": 1.0, "after": 1.0, "note": "gone",
        }]}
        with self.assertRaisesRegex(AuditError, "absent from source/report"):
            classify(self.report, self.hashes, state, EPOCH_A)

    def test_state_round_trip_is_sorted_and_malformed_state_fails(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "state.json"
            state = record_audits([
                self._record("far", before=90.0), self._record(),
            ], self.report, self.hashes, self.empty, EPOCH_A)
            write_state(state, path)
            loaded = load_state(path)
            self.assertEqual([row["function"] for row in loaded["records"]],
                             ["far", "near"])
            path.write_text('{"version":1,"records":[{}]}')
            with self.assertRaisesRegex(AuditError, "fields must be exactly"):
                load_state(path)

    def test_batch_tsv_loads_required_and_optional_assertions(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "batch.tsv"
            path.write_text(
                "unit\tfunction\toutcome\titerations\tbefore\tafter\tnote\t"
                "source_hash\tepoch\n"
                "SOURCE/A\tnear\tparked\t3\t99.8\t99.9\tresidual\t"
                "222222222222\t%s\n" % EPOCH_A)
            rows = _load_batch(path)
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0]["source_hash"], "222222222222")
        self.assertEqual(rows[0]["epoch"], EPOCH_A)

    def test_empty_or_malformed_batch_fails_closed(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "batch.tsv"
            path.write_text("unit\tfunction\nSOURCE/A\tnear\n")
            with self.assertRaisesRegex(AuditError, "batch TSV requires"):
                _load_batch(path)


class BreadthAuditCliTest(unittest.TestCase):
    def test_queue_help_formats_literal_percent(self):
        output = io.StringIO()
        with redirect_stdout(output), self.assertRaises(SystemExit) as exit_context:
            main(["queue", "--help"])
        self.assertEqual(exit_context.exception.code, 0)
        self.assertIn("previously reached 100%", output.getvalue())


if __name__ == "__main__":
    unittest.main()
