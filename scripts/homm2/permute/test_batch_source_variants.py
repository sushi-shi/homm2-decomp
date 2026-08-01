# Tests for the source-variant runner.
import json
import tempfile
import unittest
from pathlib import Path

from homm2.permute.batch_source_variants import (
    cfg_metrics,
    iter_variants,
    load_manifest,
    render_candidate,
    render_edits,
    render_variant,
    result_rank,
    structural_frontier_result_rank,
    topology_result_rank,
)


class BatchSourceVariantManifestTests(unittest.TestCase):
    def test_cfg_metrics_separates_size_shift_flow_and_missing(self):
        candidate = [
            (0, ["mov", "je"], "jcc B2 | fall B1"),
            (2, ["ret"], "ret"),
            (3, ["jmp"], "jmp B1^"),
            (4, ["nop"], "fall B3^"),
        ]
        retail = [
            (0, ["mov", "je"], "jcc B2 | fall B1"),
            (2, ["mov", "ret"], "ret"),
            (4, ["jmp"], "jmp B2^"),
            (5, ["nop"], "ret"),
            (6, ["ret"], "ret"),
        ]
        self.assertEqual(
            cfg_metrics(candidate, retail),
            {
                "candidate_blocks": 4,
                "retail_blocks": 5,
                "block_count_delta": 1,
                "graph_exact": False,
                "labeled_edge_delta": 3,
                "predecessor_delta": 4,
                "candidate_graph_sha": "ea7536d6200f2374",
                "retail_graph_sha": "162f06bfddc7a93a",
                "leading_exact_blocks": 1,
                "leading_exact_instructions": 2,
                "first_structural_divergence": 1,
                "first_structural_divergence_kind": "size_only",
                "exact": 1,
                "size_only": 1,
                "target_shift": 1,
                "flow_kind": 1,
                "missing": 1,
                "flow_exact": False,
            },
        )

    def test_result_rank_prefers_score_then_size_relocs_and_trial(self):
        rows = [
            {"score": 99.0, "candidate_size": 101, "candidate_relocs": 4, "trial": 4},
            {"score": 99.0, "candidate_size": 100, "candidate_relocs": 5, "trial": 3},
            {"score": 99.0, "candidate_size": 100, "candidate_relocs": 4, "trial": 2},
            {"score": 100.0, "candidate_size": 90, "candidate_relocs": 1, "trial": 1},
        ]
        ranked = sorted(rows, key=lambda row: result_rank(row, 100, 4))
        self.assertEqual([row["trial"] for row in ranked], [1, 2, 3, 4])

    def test_topology_result_rank_can_select_lower_fuzzy_shape(self):
        exact_graph = {
            "score": 80.0,
            "trial": 2,
            "blocks": {
                "block_count_delta": 0,
                "labeled_edge_delta": 0,
                "predecessor_delta": 0,
                "flow_kind": 0,
                "target_shift": 0,
                "size_only": 3,
                "exact": 7,
            },
        }
        high_fuzzy_extra_block = {
            "score": 99.0,
            "trial": 1,
            "blocks": {
                "block_count_delta": 1,
                "labeled_edge_delta": 0,
                "predecessor_delta": 0,
                "flow_kind": 0,
                "target_shift": 0,
                "size_only": 0,
                "exact": 20,
            },
        }
        self.assertLess(
            topology_result_rank(exact_graph),
            topology_result_rank(high_fuzzy_extra_block),
        )

    def test_frontier_result_rank_can_select_lower_fuzzy_shape(self):
        later = {
            "score": 80.0,
            "trial": 2,
            "blocks": {
                "leading_exact_blocks": 10,
                "leading_exact_instructions": 50,
                "block_count_delta": 0,
                "labeled_edge_delta": 0,
                "predecessor_delta": 0,
                "flow_kind": 0,
                "target_shift": 0,
                "size_only": 2,
                "exact": 20,
            },
        }
        earlier = {
            "score": 99.0,
            "trial": 1,
            "blocks": {
                "leading_exact_blocks": 9,
                "leading_exact_instructions": 49,
                "block_count_delta": 0,
                "labeled_edge_delta": 0,
                "predecessor_delta": 0,
                "flow_kind": 0,
                "target_shift": 0,
                "size_only": 1,
                "exact": 30,
            },
        }
        self.assertLess(
            structural_frontier_result_rank(later),
            structural_frontier_result_rank(earlier),
        )

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

    def test_named_baseline_candidate_may_have_no_edits(self):
        root, manifest = self.write_case("alpha\n", [{
            "name": "word",
            "find": "alpha",
            "options": [{"name": "base"}, {"name": "upper", "replace": "ALPHA"}],
        }])
        payload = json.loads(manifest.read_text())
        payload["candidates"] = [{"name": "baseline", "edits": []}]
        manifest.write_text(json.dumps(payload))
        _payload, _source, original, axes, candidates, _rva = load_manifest(manifest, root)
        rendered = list(iter_variants(original, axes, candidates))
        self.assertEqual([variant for variant, _labels in rendered], [b"alpha\n", b"ALPHA\n"])

    def test_axis_option_can_insert_inline_helper_and_replace_call_site(self):
        root, manifest = self.write_case("MARK\nvoid f() { value++; }\n", [{
            "name": "increment",
            "find": "value++;",
            "options": [
                {"name": "postfix"},
                {
                    "name": "inline_inc",
                    "replace": "Inc(value);",
                    "extra_edits": [{
                        "insert_before": "MARK\n",
                        "text": "inline void Inc(int &value) { ++value; }\n",
                    }],
                },
            ],
        }])
        _payload, _source, original, axes, _candidates, _rva = load_manifest(manifest, root)
        self.assertEqual(
            render_variant(original, axes, (axes[0].options[1],)),
            b"inline void Inc(int &value) { ++value; }\nMARK\nvoid f() { Inc(value); }\n",
        )

    def test_same_point_insertions_preserve_dimension_order(self):
        self.assertEqual(
            render_edits(b"MARK\n", [
                (0, 0, b"state\n"),
                (0, 0, b"generated helper\n"),
                (0, 0, b"authored helper\n"),
            ]),
            b"state\ngenerated helper\nauthored helper\nMARK\n",
        )

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
