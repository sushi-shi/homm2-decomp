from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest import mock


SCRIPT = Path(__file__).with_name("recover_historical_exact.py")
SPEC = importlib.util.spec_from_file_location("recover_historical_exact", SCRIPT)
recovery = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = recovery
SPEC.loader.exec_module(recovery)


class RecoverHistoricalExactTests(unittest.TestCase):
    def test_cleanup_is_limited_to_target_va_block(self):
        text = (
            "VA(0x00401000, 1)\n"
            "void Before() { OD_STEER(before); }\n"
            "VA(0x00402000, 1)\n"
            "void Target() { OD_STEER(target); }\n"
            "VA(0x00403000, 1)\n"
            "void After() { OD_STEER(after); }\n"
        )
        marker = text.index("VA(0x00402000")
        cleaned, count = recovery.clean_target_od_steer(text, marker)
        self.assertEqual(count, 1)
        self.assertIn("OD_STEER(before)", cleaned)
        self.assertIn("void Target() { target; }", cleaned)
        self.assertIn("OD_STEER(after)", cleaned)

    def test_cleanup_handles_nested_and_multiline_expressions(self):
        block = (
            "OD_STEER(\n"
            "    value(call(1, 2), OD_STEER(other))\n"
            ")"
        )
        cleaned, count = recovery.unwrap_od_steer(block)
        self.assertEqual(count, 2)
        self.assertEqual(
            cleaned,
            "\n    value(call(1, 2), other)\n",
        )

    def test_cleanup_ignores_comments_strings_and_longer_identifiers(self):
        block = (
            "// OD_STEER(comment)\n"
            'const char *s = "OD_STEER(string)";\n'
            "int MY_OD_STEER_NAME = 1;\n"
            "int value = OD_STEER(real);\n"
        )
        cleaned, count = recovery.unwrap_od_steer(block)
        self.assertEqual(count, 1)
        self.assertIn("// OD_STEER(comment)", cleaned)
        self.assertIn('"OD_STEER(string)"', cleaned)
        self.assertIn("MY_OD_STEER_NAME", cleaned)
        self.assertIn("int value = real;", cleaned)

    def test_cleanup_fails_closed_on_malformed_invocation(self):
        with self.assertRaisesRegex(ValueError, "unterminated OD_STEER"):
            recovery.unwrap_od_steer("int value = OD_STEER(call(1);")

    def cleanup_fixture(self, root: Path):
        source = root / "src/BASE/unit.cpp"
        source.parent.mkdir(parents=True)
        original = b"VA(0x00401234, 1)\nvoid Target() { OD_STEER(value); }\n"
        clean = b"VA(0x00401234, 1)\nvoid Target() { value; }\n"
        source.write_bytes(original)
        baseline = root / "config/match_baseline.tsv"
        baseline.parent.mkdir()
        baseline.write_bytes(b"BASE/unit\t?Target@@YAXXZ\t99.0\toldhash\n")
        output = root / "build/evidence"
        output.mkdir(parents=True)
        row = {
            "unit": "BASE/unit",
            "rva": "0x1234",
            "symbol": "?Target@@YAXXZ",
            "current_hash": "oldhash",
        }
        args = SimpleNamespace(
            trials=8,
            seed=1,
            insertion="target",
            families="forest",
            max_declarations=64,
            compile_timeout_seconds=120,
        )
        return source, original, clean, output, row, args

    def test_failed_clean_search_restores_source_and_baseline(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source, original, clean, output, row, args = self.cleanup_fixture(root)
            baseline = (root / "config/match_baseline.tsv").read_bytes()
            with (
                mock.patch.object(recovery, "noise_command", return_value=["probe"]),
                mock.patch.object(
                    recovery, "run_logged_process", return_value=(0, 0.1)
                ),
                mock.patch.object(recovery, "load_manifest", return_value={}),
                mock.patch.object(recovery, "rebuild_normalized_unit"),
            ):
                with self.assertRaisesRegex(
                    recovery.RecoveryError, "did not reproduce"
                ):
                    recovery.retire_target_od_steer(
                        root,
                        row,
                        args,
                        root / "probe.log",
                        output,
                        original,
                        clean,
                        1,
                        float("inf"),
                    )
            self.assertEqual(source.read_bytes(), original)
            self.assertEqual(
                (root / "config/match_baseline.tsv").read_bytes(), baseline
            )
            state = __import__("json").loads(
                (output / "od-steer-cleanup.json").read_text()
            )
            self.assertTrue(state["rollback_complete"])
            self.assertFalse(state["source_retained"])

    def test_clean_exact_replay_keeps_clean_source(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source, original, clean, output, row, args = self.cleanup_fixture(root)
            manifests = [
                {"exact_closure": {"trial": 3}},
                {
                    "exact_closure": {"trial": 3},
                    "record_max": {"new_max": 100.0, "source_hash": "newhash"},
                },
            ]
            with (
                mock.patch.object(recovery, "noise_command", return_value=["probe"]),
                mock.patch.object(
                    recovery, "run_logged_process", return_value=(0, 0.1)
                ),
                mock.patch.object(
                    recovery, "load_manifest", side_effect=manifests
                ),
                mock.patch.object(recovery, "rebuild_normalized_unit"),
            ):
                artifact = recovery.retire_target_od_steer(
                    root,
                    row,
                    args,
                    root / "probe.log",
                    output,
                    original,
                    clean,
                    1,
                    float("inf"),
                )
            self.assertEqual(source.read_bytes(), clean)
            self.assertTrue(artifact.name.endswith("-record"))
            state = __import__("json").loads(
                (output / "od-steer-cleanup.json").read_text()
            )
            self.assertTrue(state["source_retained"])
            self.assertEqual(state["exact_trial"], 3)


if __name__ == "__main__":
    unittest.main()
