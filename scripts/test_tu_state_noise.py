from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path


SCRIPT = Path(__file__).with_name("tu_state_noise.py")
SPEC = importlib.util.spec_from_file_location("tu_state_noise", SCRIPT)
noise = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = noise
SPEC.loader.exec_module(noise)


class TuStateNoiseTests(unittest.TestCase):
    def target(self, text: str, marker: str = "VA(0x00401234,"):
        marker_offset = text.index(marker)
        insertion = noise._leading_metadata_offset(text, marker_offset)
        return noise.Target(
            source=Path("unit.cpp"), unit="BASE/unit", rva=0x1234, va=0x401234,
            symbol="?Target@@YIXXZ", retail_size=1, marker_offset=marker_offset,
            insertion_offset=insertion, logical_line=noise.logical_line_at(text, insertion),
        )

    def test_variant_is_before_metadata_and_restores_line(self):
        original = "int predecessor;\n\n// @match-note\n// evidence\nVA(0x00401234, 0x1)\nvoid f() {}\n"
        target = self.target(original)
        variant = noise.Variant(1, "typedef", "tag", "typedef int PROBE_ALIAS;\n")
        candidate = noise.insert_variant(original, target, variant)
        self.assertLess(candidate.index("typedef int PROBE_ALIAS"), candidate.index("// @match-note"))
        self.assertIn(f"#line {target.logical_line}\n\n// @match-note", candidate)
        self.assertEqual(noise.logical_line_at(original, target.insertion_offset), 2)

    def test_existing_line_directive_is_preserved_logically(self):
        original = "#line 80 \"retail.cpp\"\nint predecessor;\nVA(0x00401234, 0x1)\nvoid f() {}\n"
        target = self.target(original)
        self.assertEqual(target.logical_line, 81)
        candidate = noise.insert_variant(
            original, target, noise.Variant(1, "typedef", "tag", "typedef int PROBE_ALIAS;\n")
        )
        self.assertIn("#line 81\nVA(0x00401234", candidate)

    def test_variants_are_deterministic_unique_and_parser_visible(self):
        count = len(noise.DEFAULT_FAMILIES) * 2
        left = noise.make_variants(count, noise.DEFAULT_FAMILIES, 123)
        right = noise.make_variants(count, noise.DEFAULT_FAMILIES, 123)
        self.assertEqual(left, right)
        self.assertEqual({variant.family for variant in left}, set(noise.DEFAULT_FAMILIES))
        self.assertEqual(len({variant.tag for variant in left}), count)
        for variant in left:
            self.assertNotIn("//", variant.body)
            self.assertNotIn("/*", variant.body)
            self.assertIn(variant.tag.replace("-", "_").upper(), variant.body)
            if variant.family == "packed":
                self.assertEqual(variant.body.count("#pragma pack(push"), 1)
                self.assertEqual(variant.body.count("#pragma pack(pop)"), 1)
            if variant.family == "include":
                headers = [
                    line.removeprefix("#include ")
                    for line in variant.body.splitlines()
                    if line.startswith("#include ")
                ]
                self.assertTrue(headers)
                self.assertTrue(set(headers) <= set(noise.CURATED_INCLUDES))

    def test_temporary_source_restores_byte_identically_after_exception(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "unit.cpp"
            original = b"original\n"
            path.write_bytes(original)
            with self.assertRaises(RuntimeError):
                with noise.temporary_source(path, original, b"candidate\n"):
                    self.assertEqual(path.read_bytes(), b"candidate\n")
                    raise RuntimeError("stop")
            self.assertEqual(path.read_bytes(), original)

    def test_compiled_sub_100_artifacts_are_removed(self):
        with tempfile.TemporaryDirectory() as directory:
            parent = Path(directory)
            scratch = tempfile.TemporaryDirectory(dir=parent)
            scratch_path = Path(scratch.name)
            (scratch_path / "manifest.json").write_text('{"best": 99.99}\n')
            (scratch_path / "trial-0001.obj").write_bytes(b"candidate")
            final_path = parent / "retained-run"
            retained = noise.finalize_compiled_artifacts(
                scratch, scratch_path, final_path, False
            )
            self.assertIsNone(retained)
            self.assertFalse(scratch_path.exists())
            self.assertFalse(final_path.exists())

    def test_compiled_exact_100_artifact_retains_seed_tag_and_body(self):
        with tempfile.TemporaryDirectory() as directory:
            parent = Path(directory)
            scratch = tempfile.TemporaryDirectory(dir=parent)
            scratch_path = Path(scratch.name)
            manifest = (
                '{"seed": 123, "exact_closure": '
                '{"tag": "0000007b-0001", "body": "typedef int PROBE;\\n"}}\n'
            )
            (scratch_path / "manifest.json").write_text(manifest)
            final_path = parent / "retained-run"
            retained = noise.finalize_compiled_artifacts(
                scratch, scratch_path, final_path, True
            )
            self.assertEqual(retained, final_path)
            self.assertFalse(scratch_path.exists())
            self.assertEqual((final_path / "manifest.json").read_text(), manifest)

    def test_include_macro_guard_is_transitive_and_fail_closed(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            include = root / "include"
            include.mkdir()
            (include / "stddef.h").write_text("#include <inner.h>\n#define SAFE_MACRO 1\n")
            (include / "inner.h").write_text("#define TARGET_MACRO(value) (value)\n")
            rejected = noise.include_macro_guard(
                root, "#include <stddef.h>\n", {"TARGET_MACRO", "value"}
            )
            self.assertFalse(rejected["passed"])
            self.assertEqual(rejected["macro_conflicts"], ["TARGET_MACRO"])
            self.assertEqual(rejected["transitive_header_count"], 2)

            accepted = noise.include_macro_guard(
                root, "#include <stddef.h>\n", {"unrelated_identifier"}
            )
            self.assertTrue(accepted["passed"])
            self.assertEqual(accepted["macro_conflicts"], [])

            unresolved = noise.include_macro_guard(
                root, "#include <limits.h>\n", {"anything"}
            )
            self.assertFalse(unresolved["passed"])
            self.assertEqual(unresolved["unresolved_headers"], ["limits.h"])

    def test_record_target_max_exact_100_updates_only_target_max_field(self):
        with tempfile.TemporaryDirectory() as directory:
            baseline = Path(directory) / "match_baseline.tsv"
            original = (
                b"# header\n"
                b"BASE/other\t?Other@@\t100.0000\totherhash\n"
                b"BASE/unit\t?Target@@YIXXZ\t90.0000\tabc123\textra-field\r\n"
                b"BASE/tail\t?Tail@@\t88.0000\ttailhash"
            )
            baseline.write_bytes(original)
            result = noise.record_target_max(
                baseline, "BASE/unit", "?Target@@YIXXZ", "abc123", 100.0
            )
            self.assertTrue(result["updated"])
            self.assertEqual(result["old_max"], 90.0)
            self.assertEqual(result["new_max"], 100.0)
            self.assertEqual(
                baseline.read_bytes(),
                original.replace(b"\t90.0000\tabc123\t", b"\t100.0000\tabc123\t"),
            )

    def test_record_target_max_sub_100_above_old_max_is_byte_identical(self):
        with tempfile.TemporaryDirectory() as directory:
            baseline = Path(directory) / "match_baseline.tsv"
            original = b"BASE/unit\t?Target@@YIXXZ\t90.0000\tabc123\n"
            baseline.write_bytes(original)
            for score, reason in (
                (99.99, "sub_100_is_disposable"),
                (99.999999, "sub_100_is_disposable"),
                (None, "no_exact_closure"),
            ):
                result = noise.record_target_max(
                    baseline, "BASE/unit", "?Target@@YIXXZ", "abc123", score
                )
                self.assertFalse(result["updated"])
                self.assertEqual(result["reason"], reason)
                self.assertEqual(baseline.read_bytes(), original)

    def test_record_target_max_refuses_hash_mismatch_without_write(self):
        with tempfile.TemporaryDirectory() as directory:
            baseline = Path(directory) / "match_baseline.tsv"
            original = b"BASE/unit\t?Target@@YIXXZ\t90.0000\toldhash\n"
            baseline.write_bytes(original)
            with self.assertRaisesRegex(noise.BaselineUpdateError, "source hash mismatch"):
                noise.record_target_max(
                    baseline, "BASE/unit", "?Target@@YIXXZ", "newhash", 100.0
                )
            self.assertEqual(baseline.read_bytes(), original)

    def test_record_target_max_refuses_missing_and_duplicate_rows(self):
        with tempfile.TemporaryDirectory() as directory:
            baseline = Path(directory) / "match_baseline.tsv"
            cases = (
                (b"BASE/other\t?Other@@\t90.0000\tabc123\n", "missing baseline row"),
                (
                    b"BASE/unit\t?Target@@YIXXZ\t90.0000\tabc123\n"
                    b"BASE/unit\t?Target@@YIXXZ\t91.0000\tabc123\n",
                    "duplicate baseline rows",
                ),
            )
            for original, message in cases:
                baseline.write_bytes(original)
                with self.assertRaisesRegex(noise.BaselineUpdateError, message):
                    noise.record_target_max(
                        baseline, "BASE/unit", "?Target@@YIXXZ", "abc123", 100.0
                    )
                self.assertEqual(baseline.read_bytes(), original)

    def test_exact_closure_requires_unrounded_100_exact_size_and_relocation_stream(self):
        retail = {
            "reloc_stream_complete": True,
            "reloc_stream": ["00000004:0006:_Global:00000000"],
        }
        candidate = dict(retail)
        self.assertEqual(noise.exact_closure_rejections(100.0, 42, 42, candidate, retail), [])
        self.assertIn(
            "unrounded objdiff score is not exactly 100.0",
            noise.exact_closure_rejections(99.999999, 42, 42, candidate, retail),
        )
        self.assertIn(
            "target size is not exact: candidate 41, retail 42",
            noise.exact_closure_rejections(100.0, 41, 42, candidate, retail),
        )
        wrong_target = {
            "reloc_stream_complete": True,
            "reloc_stream": ["00000004:0006:_Wrong:00000000"],
        }
        self.assertIn(
            "ordered relocation offsets/types/identities/addends differ from retail",
            noise.exact_closure_rejections(100.0, 42, 42, wrong_target, retail),
        )


if __name__ == "__main__":
    unittest.main()
