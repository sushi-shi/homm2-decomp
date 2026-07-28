# Tests for the controlled TU-state experiment engine.
from __future__ import annotations

import importlib.util
import os
import sys
import tempfile
import time
import unittest
from unittest import mock
from pathlib import Path

from homm2.build.canonicalize_data_symbols import CoffObject, CompgenDataClaim
from homm2.build.test_canonicalize_data_symbols import DATA, SectionSpec, make_coff


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

    def test_top_insertion_follows_leading_preprocessor_block(self):
        original = "#include <a.h>\n\n#define VALUE 1\n\nint predecessor;\nVA(0x00401234, 0x1)\n"
        target = self.target(original)
        variant = noise.Variant(1, "typedef", "tag", "typedef int PROBE_ALIAS;\n")
        candidate = noise.insert_variant(original, target, variant, "top")
        self.assertLess(candidate.index("#define VALUE"), candidate.index("PROBE_ALIAS"))
        self.assertLess(candidate.index("PROBE_ALIAS"), candidate.index("int predecessor"))
        self.assertIn("#line 5\nint predecessor", candidate)

    def test_layer_body_precedes_generated_variant_at_same_insertion(self):
        original = "#include <a.h>\n\nint predecessor;\nVA(0x00401234, 0x1)\n"
        target = self.target(original)
        variant = noise.Variant(2, "enum", "tag", "typedef int SECOND;\n")
        candidate = noise.insert_variant(
            original,
            target,
            variant,
            "top",
            "typedef int FIRST;\n",
        )
        self.assertLess(candidate.index("FIRST"), candidate.index("SECOND"))
        self.assertLess(candidate.index("SECOND"), candidate.index("int predecessor"))
        self.assertIn("#line 3\nint predecessor", candidate)

    def test_layer_body_loads_a_matching_census_representative(self):
        original = "int predecessor;\nVA(0x00401234, 0x1)\n"
        target = self.target(original)
        summary = {
            "insertion": "top",
            "target": {
                "unit": target.unit,
                "rva": f"0x{target.rva:x}",
                "symbol": target.symbol,
            },
            "states": [{
                "state": "state-id",
                "scores": [96.5],
                "representative": {
                    "trial": 17,
                    "family": "typedef",
                    "tag": "tag",
                    "body": "typedef int LAYER;",
                },
            }],
        }
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "states.json"
            path.write_text(__import__("json").dumps(summary))
            body, metadata = noise.load_layer_body(path, 17, target, "top")
        self.assertEqual(body, "typedef int LAYER;\n")
        self.assertEqual(metadata["state"], "state-id")
        self.assertEqual(metadata["score"], 96.5)
        self.assertEqual(metadata["family"], "typedef")

    def test_resolver_ignores_va_text_in_comment_before_real_marker(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "src/BASE/unit.cpp"
            source.parent.mkdir(parents=True)
            source.write_text(
                "int predecessor; // vtable: VA(0x00401234, 0x1)\n"
                "\n// @match-note\n"
                "VA(0x00401234, 0x1)\n"
                "void Target() {}\n"
            )
            (root / "config").mkdir()
            (root / "config/units.toml").write_text(
                '[flags]\nbase = ["/c"]\n\n'
                '[[unit]]\nunit = "BASE/unit"\nsource = "src/BASE/unit.cpp"\nflags = "base"\n'
            )
            (root / "build/gen").mkdir(parents=True)
            (root / "build/gen/symbol_names.csv").write_text(
                "kind,unit,rva,name,size\n"
                "func,BASE/unit,0x1234,?Target@@YIXXZ,0x1\n"
            )

            target, _flags = noise.resolve_target(root, Path("src/BASE/unit.cpp"), 0x1234)
            original = source.read_text()
            self.assertEqual(target.marker_offset, original.rindex("VA(0x00401234"))
            candidate = noise.insert_variant(
                original,
                target,
                noise.Variant(1, "typedef", "tag", "typedef int PROBE_ALIAS;\n"),
            )
            self.assertLess(candidate.index("vtable: VA("), candidate.index("PROBE_ALIAS"))
            self.assertLess(candidate.index("PROBE_ALIAS"), candidate.index("// @match-note"))
            canonical_suffix = noise.target_suffix_digest(original, target.va)
            self.assertIsNotNone(canonical_suffix)
            self.assertEqual(noise.target_suffix_digest(candidate, target.va), canonical_suffix)
            self.assertNotEqual(
                noise.target_suffix_digest(candidate.replace("void Target", "int Target"), target.va),
                canonical_suffix,
            )

    def test_variants_are_deterministic_unique_and_parser_visible(self):
        count = 8
        left = noise.make_variants(count, noise.DEFAULT_FAMILIES, 123)
        right = noise.make_variants(count, noise.DEFAULT_FAMILIES, 123)
        self.assertEqual(left, right)
        self.assertEqual({variant.family for variant in left}, set(noise.DEFAULT_FAMILIES))
        self.assertEqual(len({variant.tag for variant in left}), count)
        for variant in left:
            self.assertNotIn("//", variant.body)
            self.assertNotIn("/*", variant.body)
            self.assertIn(variant.tag.replace("-", "_").upper(), variant.body)

        compact = noise.make_variants(
            len(noise.ALL_FAMILIES), noise.ALL_FAMILIES, 321
        )
        self.assertEqual({variant.family for variant in compact}, set(noise.ALL_FAMILIES))
        for variant in compact:
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

    def test_default_forest_has_at_least_ten_of_each_major_surface(self):
        variants = noise.make_variants(12, noise.DEFAULT_FAMILIES, 123)
        permutations = set()
        for variant in variants:
            self.assertEqual(variant.family, "forest")
            width = noise.DEFAULT_MIN_FOREST_WIDTH + variant.trial - 1
            self.assertEqual(
                sum(
                    line.startswith("class ") and "_FOREST_CLASS_" in line
                    for line in variant.body.splitlines()
                ),
                width,
            )
            self.assertGreaterEqual(variant.body.count("typedef "), width)
            self.assertEqual(
                sum(
                    "_FOREST_PROTOTYPE_" in line
                    for line in variant.body.splitlines()
                ),
                width,
            )
            self.assertEqual(
                sum(
                    "_FOREST_FUNCTION_" in line
                    for line in variant.body.splitlines()
                ),
                width,
            )
            self.assertEqual(len(variant.permutation), width * 4)
            permutations.add(variant.permutation)
        self.assertEqual(len(permutations), len(variants))

    def test_forest_refuses_a_width_below_ten(self):
        with self.assertRaisesRegex(ValueError, "at least 10"):
            noise.make_variants(1, ("forest",), 123, max_declarations=9)

    def test_only_trial_replays_original_deterministic_index(self):
        variants = noise.make_variants(12, ("forest",), 123)
        selected = noise.select_variants(variants, (4, 11), 12)
        self.assertEqual([variant.trial for variant in selected], [4, 11])
        self.assertEqual(selected[0], variants[3])
        self.assertEqual(selected[1], variants[10])
        with self.assertRaisesRegex(ValueError, "exceeds --trials"):
            noise.select_variants(variants, (13,), 12)

    def test_declaration_trains_cover_requested_range(self):
        variants = noise.make_variants(6, ("typedef",), 123, max_declarations=4)
        self.assertEqual(
            [variant.body.count("typedef ") for variant in variants],
            [1, 2, 3, 4, 1, 2],
        )

    def test_target_state_identity_covers_bytes_relocations_and_boundary(self):
        base = {
            "objdiff_size": 4,
            "text_sha": "text",
            "reloc_stream": ["00000001:0006:$SG123:00000000"],
        }
        self.assertEqual(noise.target_state_identity(base), noise.target_state_identity(dict(base)))
        for key, value in (
            ("objdiff_size", 5),
            ("text_sha", "other-text"),
            ("reloc_stream", ["00000002:0006:$SG123:00000000"]),
        ):
            changed = dict(base)
            changed[key] = value
            self.assertNotEqual(noise.target_state_identity(base), noise.target_state_identity(changed))
        renumbered = dict(base)
        renumbered["reloc_stream"] = ["00000001:0006:$SG999:00000000"]
        self.assertEqual(noise.target_state_identity(base), noise.target_state_identity(renumbered))

    def test_byte_differences_cover_changed_and_one_sided_bytes(self):
        self.assertEqual(
            noise.byte_differences("001122", "00113344"),
            [
                {"offset": 2, "candidate": "22", "retail": "33"},
                {"offset": 3, "candidate": None, "retail": "44"},
            ],
        )

    def test_cfg_topology_metrics_match_blocks_and_flow(self):
        assembly = (
            "0000: 74 01\tje\t0x3\n"
            "0002: c3\tret\n"
            "0003: c3\tret\n"
        )
        metrics = noise.cfg_topology_metrics(assembly, assembly)
        self.assertEqual(metrics["candidate_blocks"], 3)
        self.assertEqual(metrics["retail_blocks"], 3)
        self.assertEqual(metrics["exact"], 3)
        self.assertEqual(metrics["flow_kind"], 0)
        self.assertEqual(metrics["target_shift"], 0)
        self.assertTrue(metrics["flow_exact"])
        self.assertTrue(metrics["graph_exact"])
        self.assertEqual(metrics["labeled_edge_delta"], 0)
        self.assertEqual(metrics["predecessor_delta"], 0)
        self.assertEqual(metrics["leading_exact_blocks"], 3)
        self.assertEqual(metrics["leading_exact_instructions"], 3)
        self.assertIsNone(metrics["first_structural_divergence"])
        self.assertIsNone(metrics["first_structural_divergence_kind"])

    def test_canonical_cfg_signature_preserves_labeled_from_to_graph(self):
        left = [
            (0, [], "jcc B2 | fall B1"),
            (1, [], "jmp B3"),
            (2, [], "jmp B3"),
            (3, [], "ret"),
        ]
        right = [
            (0, [], "jcc B1 | fall B2"),
            (1, [], "jmp B3"),
            (2, [], "jmp B3"),
            (3, [], "ret"),
        ]
        self.assertEqual(
            noise.canonical_cfg_signature(left),
            noise.canonical_cfg_signature(right),
        )
        changed = list(right)
        changed[2] = (2, [], "ret")
        self.assertNotEqual(
            noise.canonical_cfg_signature(left),
            noise.canonical_cfg_signature(changed),
        )

    def test_topology_rank_precedes_fuzzy_score(self):
        exact_shape = {
            "block_count_delta": 0,
            "labeled_edge_delta": 0,
            "predecessor_delta": 0,
            "flow_kind": 0,
            "target_shift": 0,
            "size_only": 3,
            "exact": 7,
        }
        extra_block = {
            "block_count_delta": 1,
            "labeled_edge_delta": 0,
            "predecessor_delta": 0,
            "flow_kind": 0,
            "target_shift": 0,
            "size_only": 0,
            "exact": 20,
        }
        self.assertLess(
            noise.topology_rank(exact_shape, 50.0),
            noise.topology_rank(extra_block, 99.0),
        )
        self.assertLess(
            noise.topology_rank(exact_shape, 90.0),
            noise.topology_rank(exact_shape, 80.0),
        )

    def test_structural_frontier_rank_prefers_later_first_divergence(self):
        earlier = {
            "leading_exact_blocks": 3,
            "leading_exact_instructions": 10,
            "block_count_delta": 0,
            "labeled_edge_delta": 0,
            "predecessor_delta": 0,
            "flow_kind": 0,
            "target_shift": 0,
            "size_only": 1,
            "exact": 9,
        }
        later = dict(earlier)
        later["leading_exact_blocks"] = 4
        later["leading_exact_instructions"] = 12
        self.assertLess(
            noise.structural_frontier_rank(later, 70.0),
            noise.structural_frontier_rank(earlier, 99.0),
        )

    def test_trial_status_reports_only_target_observation_and_closure(self):
        self.assertEqual(
            noise.trial_status({
                "observed": True,
                "exact_closure_eligible": True,
            }),
            "exact-closure",
        )
        self.assertEqual(
            noise.trial_status({
                "observed": True,
                "exact_closure_eligible": False,
            }),
            "observed",
        )
        self.assertEqual(
            noise.trial_status({
                "observed": False,
                "exact_closure_eligible": False,
            }),
            "unusable",
        )

    def test_comparison_pair_applies_reviewed_compgen_data_names(self):
        semantic = "__h2cg$BASE$unit$data$message"
        payload = make_coff(
            [SectionSpec(".data", b"rb\0\0", DATA)],
            [("$SG123", 0, 1, 0, 3)],
        )
        pairing = {
            "unit": "BASE/unit",
            "names": set(),
            "public_data": {},
            "function_rvas": {},
            "function_sizes": {},
            "symbols": {},
            "data": {},
            "duplicates": {},
            "compgen": (),
            "compgen_data": (
                CompgenDataClaim(semantic, 1, 0, 3, "data", "local"),
            ),
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            candidate = root / "candidate.obj"
            retail = root / "retail.obj"
            candidate.write_bytes(payload)
            retail.write_bytes(payload)
            with mock.patch.object(noise, "canonicalize_unit"):
                normalized_candidate, normalized_retail, _generated = (
                    noise.normalize_comparison_pair(
                        candidate, retail, root / "pair", pairing
                    )
                )
            self.assertEqual(
                CoffObject(normalized_candidate.read_bytes()).symbols[0].name,
                semantic,
            )
            self.assertEqual(
                CoffObject(normalized_retail.read_bytes()).symbols[0].name,
                semantic,
            )

    def test_target_compgen_data_binding_follows_relocation_after_probe_storage(self):
        semantic = "__h2cg$BASE$unit$data$message"
        candidate_payload = make_coff(
            [
                SectionSpec(".text", bytes(4), 0x60000020, ((0, 2, 0x0006),)),
                SectionSpec(".data", b"\x01\0\0\0rb\0\0", DATA),
            ],
            [
                ("?Target@@YIXXZ", 0, 1, 0x20, 2),
                ("_probe", 0, 2, 0, 3),
                ("$SG123", 4, 2, 0, 3),
            ],
        )
        retail_payload = make_coff(
            [
                SectionSpec(".text", bytes(4), 0x60000020, ((0, 1, 0x0006),)),
                SectionSpec(".data", b"rb\0\0", DATA),
            ],
            [
                ("?Target@@YIXXZ", 0, 1, 0x20, 2),
                (semantic, 0, 2, 0, 3),
            ],
        )
        pairing = {
            "unit": "BASE/unit",
            "names": set(),
            "public_data": {},
            "function_rvas": {},
            "function_sizes": {},
            "symbols": {},
            "data": {},
            "duplicates": {},
            "compgen": (),
            "compgen_data": (
                CompgenDataClaim(semantic, 2, 0, 3, "data", "local"),
            ),
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            candidate = root / "candidate.obj"
            retail = root / "retail.obj"
            candidate.write_bytes(candidate_payload)
            retail.write_bytes(retail_payload)
            with mock.patch.object(noise, "canonicalize_unit"):
                normalized_candidate, normalized_retail, _generated = (
                    noise.normalize_comparison_pair(
                        candidate,
                        retail,
                        root / "pair",
                        pairing,
                        "?Target@@YIXXZ",
                    )
                )
            self.assertEqual(
                CoffObject(normalized_candidate.read_bytes()).symbols[2].name,
                semantic,
            )
            self.assertEqual(
                CoffObject(normalized_retail.read_bytes()).symbols[1].name,
                semantic,
            )

    def test_target_compgen_data_binding_survives_extra_control_relocation(self):
        semantic = "__h2cg$BASE$unit$data$message"
        candidate_payload = make_coff(
            [
                SectionSpec(
                    ".text",
                    bytes(8),
                    0x60000020,
                    ((0, 1, 0x0006), (4, 2, 0x0006)),
                ),
                SectionSpec(".data", b"rb\0\0", DATA),
            ],
            [
                ("?Target@@YIXXZ", 0, 1, 0x20, 2),
                ("$L123", 0, 1, 0, 3),
                ("$SG123", 0, 2, 0, 3),
                ("$L124", 4, 1, 0, 3),
            ],
        )
        retail_payload = make_coff(
            [
                SectionSpec(".text", bytes(4), 0x60000020, ((0, 1, 0x0006),)),
                SectionSpec(".data", b"rb\0\0", DATA),
            ],
            [
                ("?Target@@YIXXZ", 0, 1, 0x20, 2),
                (semantic, 0, 2, 0, 3),
            ],
        )
        pairing = {
            "unit": "BASE/unit",
            "names": set(),
            "public_data": {},
            "function_rvas": {},
            "function_sizes": {},
            "symbols": {},
            "data": {},
            "duplicates": {},
            "compgen": (),
            "compgen_data": (
                CompgenDataClaim(semantic, 2, 0, 3, "data", "local"),
            ),
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            candidate = root / "candidate.obj"
            retail = root / "retail.obj"
            candidate.write_bytes(candidate_payload)
            retail.write_bytes(retail_payload)
            with mock.patch.object(noise, "canonicalize_unit"):
                normalized_candidate, normalized_retail, _generated = (
                    noise.normalize_comparison_pair(
                        candidate,
                        retail,
                        root / "pair",
                        pairing,
                        "?Target@@YIXXZ",
                    )
                )
            self.assertEqual(
                CoffObject(normalized_candidate.read_bytes()).symbols[2].name,
                semantic,
            )
            self.assertEqual(
                CoffObject(normalized_retail.read_bytes()).symbols[1].name,
                semantic,
            )

    def test_target_compgen_data_binding_ignores_unreferenced_stale_claim(self):
        payload = make_coff(
            [
                SectionSpec(".text", bytes(4), 0x60000020),
                SectionSpec(".data", b"\x01\0\0\0", DATA),
            ],
            [
                ("?Target@@YIXXZ", 0, 1, 0x20, 2),
                ("_probe", 0, 2, 0, 3),
            ],
        )
        pairing = {
            "unit": "BASE/unit",
            "names": set(),
            "public_data": {},
            "function_rvas": {},
            "function_sizes": {},
            "symbols": {},
            "data": {},
            "duplicates": {},
            "compgen": (),
            "compgen_data": (
                CompgenDataClaim(
                    "__h2cg$BASE$unit$data$unrelated",
                    2,
                    0,
                    8,
                    "data",
                    "local",
                ),
            ),
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            candidate = root / "candidate.obj"
            retail = root / "retail.obj"
            candidate.write_bytes(payload)
            retail.write_bytes(payload)
            with mock.patch.object(noise, "canonicalize_unit"):
                normalized_candidate, normalized_retail, _generated = (
                    noise.normalize_comparison_pair(
                        candidate,
                        retail,
                        root / "pair",
                        pairing,
                        "?Target@@YIXXZ",
                    )
                )
            self.assertTrue(normalized_candidate.exists())
            self.assertTrue(normalized_retail.exists())

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

    def test_temporary_source_refuses_stale_original(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "unit.cpp"
            path.write_bytes(b"newer\n")
            with self.assertRaisesRegex(noise.SourceMutationError, "changed before probe"):
                with noise.temporary_source(path, b"older\n", b"candidate\n"):
                    pass
            self.assertEqual(path.read_bytes(), b"newer\n")

    def test_temporary_source_does_not_overwrite_concurrent_edit(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "unit.cpp"
            path.write_bytes(b"original\n")
            with self.assertRaisesRegex(noise.SourceMutationError, "refusing stale restoration"):
                with noise.temporary_source(path, b"original\n", b"candidate\n"):
                    path.write_bytes(b"concurrent\n")
            self.assertEqual(path.read_bytes(), b"concurrent\n")

    def test_source_mutation_lock_rejects_second_holder(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "unit.cpp"
            source.write_bytes(b"source\n")
            first = noise.acquire_source_mutation_lock(root, source)
            self.addCleanup(first.close)
            with self.assertRaisesRegex(noise.SourceMutationError, "another source-variant"):
                noise.acquire_source_mutation_lock(root, source)

    def test_compile_timeout_seconds_must_be_positive_and_finite(self):
        self.assertEqual(noise.positive_seconds("0.25"), 0.25)
        for value in ("0", "-1", "nan", "inf", "not-a-number"):
            with self.subTest(value=value):
                with self.assertRaises(noise.argparse.ArgumentTypeError):
                    noise.positive_seconds(value)

    @unittest.skipUnless(hasattr(os, "killpg"), "requires POSIX process groups")
    def test_timeout_kills_compiler_process_group(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            marker = root / "survived"
            child_code = (
                "import pathlib,signal,time;"
                "signal.signal(signal.SIGTERM, signal.SIG_IGN);"
                "time.sleep(0.4);"
                f"pathlib.Path({str(marker)!r}).write_text('alive')"
            )
            parent_code = (
                "import signal,subprocess,sys,time;"
                "signal.signal(signal.SIGTERM, signal.SIG_IGN);"
                f"subprocess.Popen([sys.executable, '-c', {child_code!r}]);"
                "time.sleep(10)"
            )
            with mock.patch.object(noise, "PROCESS_GROUP_TERMINATION_GRACE_SECONDS", 0.05):
                returncode, _stdout, _stderr, timed_out = noise._run_command_with_timeout(
                    [sys.executable, "-c", parent_code], root, 0.05
                )
            self.assertTrue(timed_out)
            self.assertNotEqual(returncode, 0)
            time.sleep(0.5)
            self.assertFalse(marker.exists())

    def test_compile_timeout_is_fail_closed_and_removes_partial_outputs(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            output = root / "candidate.obj"
            output.write_bytes(b"partial")
            Path(str(output) + ".d").write_bytes(b"partial dependency")
            with mock.patch.object(
                noise,
                "_run_command_with_timeout",
                return_value=(-9, "compiler stdout\n", "compiler stderr\n", True),
            ):
                ok, log, timed_out = noise.compile_object(
                    root, root / "unit.cpp", output, ["/c"], 0.125
                )
            self.assertFalse(ok)
            self.assertTrue(timed_out)
            self.assertFalse(output.exists())
            self.assertFalse(Path(str(output) + ".d").exists())
            self.assertIn("compiler stdout", log)
            self.assertIn("compiler stderr", log)
            self.assertIn("timed out after 0.125 seconds", log)
            self.assertIn("terminated compiler process group", log)

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

    def test_retained_subpath_is_written_through_staging_directory(self):
        root = Path("/tmp/tu-state-noise-test")
        final_path = root / "retained-run"
        scratch_path = root / ".retained-run.tmp"
        self.assertEqual(
            noise.staged_artifact_path(
                final_path / "states.json", scratch_path, final_path
            ),
            scratch_path / "states.json",
        )
        external = root / "summaries" / "states.json"
        self.assertEqual(
            noise.staged_artifact_path(external, scratch_path, final_path),
            external,
        )

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

    def test_record_target_max_records_higher_sub_100_observations(self):
        with tempfile.TemporaryDirectory() as directory:
            baseline = Path(directory) / "match_baseline.tsv"
            original = b"BASE/unit\t?Target@@YIXXZ\t90.0000\tabc123\n"
            baseline.write_bytes(original)
            result = noise.record_target_max(
                baseline, "BASE/unit", "?Target@@YIXXZ", "abc123", 99.991234
            )
            self.assertTrue(result["updated"])
            self.assertEqual(result["new_max"], 99.9912)
            self.assertEqual(
                baseline.read_bytes(),
                b"BASE/unit\t?Target@@YIXXZ\t99.9912\tabc123\n",
            )
            unchanged = noise.record_target_max(
                baseline, "BASE/unit", "?Target@@YIXXZ", "abc123", 99.98
            )
            self.assertFalse(unchanged["updated"])
            self.assertEqual(unchanged["reason"], "not_higher")

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
