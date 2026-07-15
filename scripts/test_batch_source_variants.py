import json
import tempfile
import unittest
from pathlib import Path

from batch_source_variants import iter_variants, load_manifest, render_candidate, render_variant


class BatchSourceVariantManifestTests(unittest.TestCase):
    def write_case(self, source_text, axes):
        temporary = tempfile.TemporaryDirectory()
        root = Path(temporary.name)
        source = root / "sample.cpp"
        source.write_text(source_text)
        manifest = root / "variants.json"
        manifest.write_text(json.dumps({
            "schema": 1,
            "source": "sample.cpp",
            "rva": "0x1234",
            "axes": axes,
        }))
        self.addCleanup(temporary.cleanup)
        return root, manifest

    def test_exact_spans_render_without_regex_rewrites(self):
        root, manifest = self.write_case("alpha beta gamma\n", [
            {
                "name": "first",
                "find": "alpha",
                "options": [{"name": "base"}, {"name": "upper", "replace": "ALPHA"}],
            },
            {
                "name": "last",
                "find": "gamma",
                "options": [{"name": "short", "replace": "g"}],
            },
        ])
        _payload, _source, original, axes, _candidates, _rva = load_manifest(manifest, root)
        choices = (axes[0].options[1], axes[1].options[0])
        self.assertEqual(render_variant(original, axes, choices), b"ALPHA beta g\n")

    def test_find_span_must_be_unique(self):
        root, manifest = self.write_case("same same\n", [{
            "name": "ambiguous",
            "find": "same",
            "options": [{"name": "base"}],
        }])
        with self.assertRaisesRegex(ValueError, "occurs 2 times"):
            load_manifest(manifest, root)

    def test_axes_may_not_overlap(self):
        root, manifest = self.write_case("abcdef\n", [
            {"name": "wide", "find": "abcd", "options": [{"name": "base"}]},
            {"name": "inside", "find": "bc", "options": [{"name": "base"}]},
        ])
        with self.assertRaisesRegex(ValueError, "axes overlap"):
            load_manifest(manifest, root)

    def test_explicit_candidate_edits_use_validated_byte_offsets(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        root = Path(temporary.name)
        (root / "sample.cpp").write_text("alpha beta gamma\n")
        manifest = root / "variants.json"
        manifest.write_text(json.dumps({
            "schema": 1,
            "source": "sample.cpp",
            "rva": "0x1234",
            "candidates": [{
                "name": "two-edits",
                "edits": [
                    {"start": 0, "end": 5, "find": "alpha", "replace": "A"},
                    {"start": 11, "end": 16, "find": "gamma", "replace": "G"},
                ],
            }],
        }))
        _payload, _source, original, axes, candidates, _rva = load_manifest(manifest, root)
        self.assertFalse(axes)
        self.assertEqual(render_candidate(original, candidates[0]), b"A beta G\n")

    def test_candidate_offset_must_match_original_source(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        root = Path(temporary.name)
        (root / "sample.cpp").write_text("alpha\n")
        manifest = root / "variants.json"
        manifest.write_text(json.dumps({
            "schema": 1,
            "source": "sample.cpp",
            "rva": "0x1234",
            "candidates": [{
                "name": "stale",
                "edits": [{"start": 1, "end": 5, "find": "alpha", "replace": "A"}],
            }],
        }))
        with self.assertRaisesRegex(ValueError, "does not equal find"):
            load_manifest(manifest, root)

    def test_candidate_can_insert_at_an_exact_byte_offset(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        root = Path(temporary.name)
        (root / "sample.cpp").write_text("body\n")
        manifest = root / "variants.json"
        manifest.write_text(json.dumps({
            "schema": 1,
            "source": "sample.cpp",
            "rva": "0x1234",
            "candidates": [{
                "name": "insert",
                "edits": [{"start": 0, "end": 0, "find": "", "replace": "helper\n"}],
            }],
        }))
        _payload, _source, original, _axes, candidates, _rva = load_manifest(manifest, root)
        self.assertEqual(render_candidate(original, candidates[0]), b"helper\nbody\n")

    def test_axes_and_ast_candidates_form_one_cartesian_product(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        root = Path(temporary.name)
        (root / "sample.cpp").write_text("left middle right\n")
        manifest = root / "variants.json"
        manifest.write_text(json.dumps({
            "schema": 1,
            "source": "sample.cpp",
            "rva": "0x1234",
            "axes": [{
                "name": "left",
                "find": "left",
                "options": [{"name": "base"}, {"name": "upper", "replace": "LEFT"}],
            }],
            "candidates": [{
                "name": "right",
                "edits": [{"start": 12, "end": 17, "find": "right", "replace": "R"}],
            }],
        }))
        _payload, _source, original, axes, candidates, _rva = load_manifest(manifest, root)
        rendered = list(iter_variants(original, axes, candidates))
        self.assertEqual([item[0] for item in rendered], [
            b"left middle R\n", b"LEFT middle R\n",
        ])


if __name__ == "__main__":
    unittest.main()
