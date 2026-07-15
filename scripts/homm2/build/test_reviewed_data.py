import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build.reviewed_data import (
    _expected_objects,
    ensure_reviewed_targets,
    promote_canonical_topology,
    refresh_required,
)
from homm2.build.candidate_data_manifest import DerivationStats, GroupDiagnostic


class ReviewedDataTest(unittest.TestCase):
    def test_freshness_requires_exact_identity(self):
        expected = {"schema": 1, "manifest_sha256": "abc"}
        self.assertFalse(refresh_required(dict(expected), expected))
        self.assertFalse(refresh_required({**expected, "extra": "retained"}, expected))
        self.assertTrue(refresh_required(
            {"schema": 1, "manifest_sha256": "def"}, expected))
        self.assertTrue(refresh_required(None, expected))

    def test_manifest_object_paths_map_to_delinker_outputs(self):
        manifest = (
            b"name\tobject\trva\tsize\tstorage\talignment\tsection_offset\t"
            b"scope\tprovenance\n"
            b"table\tengine\\world.c\t0x100\t4\tdata\t4\t-\texternal\ttest\n")
        self.assertEqual(_expected_objects(manifest), {"engine/world.c.obj"})

    def test_refresh_fails_when_required_target_inputs_are_missing(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            with mock.patch("homm2.build.reviewed_data.TARGET", root / "missing"):
                with self.assertRaisesRegex(RuntimeError, "run `homm2 init`"):
                    ensure_reviewed_targets()

    def test_normal_ensure_does_not_derive_or_regenerate_bootstrap_target(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            target = root / "delink"
            target.mkdir()
            stamp = target / ".reviewed-data-stamp.json"
            stamp.write_text('{"mode": "bootstrap"}\n')
            with (mock.patch("homm2.build.reviewed_data.TARGET", target),
                  mock.patch("homm2.build.reviewed_data.STAMP", stamp),
                  mock.patch("homm2.build.reviewed_data.derive_allocations",
                             side_effect=AssertionError("must not derive")),
                  mock.patch("homm2.build.reviewed_data.subprocess.run",
                             side_effect=AssertionError("must not regenerate")),
                  mock.patch("homm2.build.reviewed_data._refresh_objdiff_targets")):
                self.assertFalse(ensure_reviewed_targets())

    def test_normal_ensure_refuses_stale_canonical_target(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            target = root / "delink"
            target.mkdir()
            stamp = target / ".reviewed-data-stamp.json"
            stamp.write_text('{"mode": "canonical", "digest": "old"}\n')
            inputs = [root / name for name in (
                "data.tsv", "contrib.tsv", "coverage.tsv", "coverage.json",
                "unresolved.tsv", "game.exe", "game.pdb")]
            for path in inputs:
                path.write_bytes(b"input")
            delinker = root / "vostok-delinker"
            delinker.write_bytes(b"tool")
            with (mock.patch("homm2.build.reviewed_data.TARGET", target),
                  mock.patch("homm2.build.reviewed_data.STAMP", stamp),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_MANIFEST", inputs[0]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_CONTRIBUTION_MANIFEST", inputs[1]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_COVERAGE", inputs[2]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_COVERAGE_DIAGNOSTICS",
                             inputs[3]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_UNRESOLVED", inputs[4]),
                  mock.patch("homm2.build.reviewed_data.EXE", inputs[5]),
                  mock.patch("homm2.build.reviewed_data.PDB", inputs[6]),
                  mock.patch("homm2.build.reviewed_data._canonical_identity_inputs",
                             return_value={"mode": "canonical", "digest": "new"})):
                with self.assertRaisesRegex(RuntimeError, "regenerate.*explicitly"):
                    ensure_reviewed_targets(delinker=delinker)

    def test_promotion_versions_partial_topology_and_open_inventory(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            data_config = root / "data.tsv"
            contribution_config = root / "contrib.tsv"
            coverage_config = root / "coverage.tsv"
            coverage_diagnostics = root / "coverage.json"
            unresolved_config = root / "unresolved.tsv"
            diagnostics = root / "diagnostics.json"
            stats = DerivationStats(candidate_definitions=1, open_groups=1)
            open_group = GroupDiagnostic("SOURCE/Test", "data", ("unmapped_definition",),
                                         ("local maps to nothing",))
            with (mock.patch("homm2.build.reviewed_data.derive_allocations",
                             return_value=([], stats, [open_group])),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_MANIFEST", data_config),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_CONTRIBUTION_MANIFEST",
                             contribution_config),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_COVERAGE",
                             coverage_config),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_COVERAGE_DIAGNOSTICS",
                             coverage_diagnostics),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_UNRESOLVED",
                             unresolved_config),
                  mock.patch("homm2.build.reviewed_data.CANDIDATE_DIAGNOSTICS", diagnostics),
                  mock.patch("homm2.build.reviewed_data._build_coverage_proposal",
                             return_value=(b"coverage\n", [], [])),
                  mock.patch("homm2.build.reviewed_data.reviewed_manifest_bytes",
                             return_value=b"data\n"),
                  mock.patch("homm2.build.reviewed_data.contribution_manifest_bytes",
                             return_value=b"contributions\n"),
                  mock.patch("homm2.build.reviewed_data.unresolved_manifest_bytes",
                             return_value=b"unresolved\n")):
                _stats, open_groups = promote_canonical_topology()
            self.assertEqual(open_groups, [open_group])
            self.assertTrue(diagnostics.is_file())
            self.assertEqual(data_config.read_bytes(), b"data\n")
            self.assertEqual(contribution_config.read_bytes(), b"contributions\n")
            self.assertEqual(coverage_config.read_bytes(), b"coverage\n")
            self.assertEqual(unresolved_config.read_bytes(), b"unresolved\n")

    def test_finalization_refuses_open_groups_before_writing_configs(self):
        stats = DerivationStats(candidate_definitions=1, open_groups=1)
        open_group = GroupDiagnostic("SOURCE/Test", "data", ("unmapped_definition",),
                                     ("local maps to nothing",))
        with (mock.patch("homm2.build.reviewed_data.derive_allocations",
                         return_value=([], stats, [open_group])),
              mock.patch("homm2.build.reviewed_data._build_coverage_proposal",
                         return_value=(b"coverage\n", [], [])),
              mock.patch("homm2.build.reviewed_data._atomic_write") as write):
            _stats, blockers = promote_canonical_topology(require_all=True)
        self.assertEqual(blockers, [open_group])
        self.assertEqual(write.call_count, 1)  # Disposable diagnostics only.


if __name__ == "__main__":
    unittest.main()
