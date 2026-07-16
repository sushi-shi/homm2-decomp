import csv
import io
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build.reviewed_data import (
    DATA_SYMBOL_HEADER,
    REVIEW_QUEUE_HEADER,
    _expected_objects,
    _build_reviewed_canonical_manifests,
    ensure_reviewed_targets,
    propose_candidate_topology,
    promote_canonical_topology,
    reviewed_manifest_bytes,
    review_queue_bytes,
    refresh_required,
)
from homm2.build.candidate_data_manifest import (
    CandidateAllocation, DerivationStats, GroupDiagnostic,
)


def symbol_row(name, unit, rva):
    return {
        "name": name, "object": unit.replace("/", "\\") + ".c",
        "rva": "0x%x" % rva, "size": "0x4", "storage": "data",
        "alignment": "0x4", "section_ordinal": "2", "section_offset": "0x0",
        "scope": "external", "provenance": "source-DATA:fixture.cpp:1",
    }


def write_symbol_manifest(path, rows):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=DATA_SYMBOL_HEADER,
                                delimiter="\t", lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def candidate(name, rva, provenance="aligned-relocation-addend", unit="SOURCE/Test"):
    return CandidateAllocation(
        unit, unit.replace("/", "\\") + ".c", name, "data", 0, 4, 4,
        rva, 3, "local", provenance)


class ReviewedDataTest(unittest.TestCase):
    def test_initialized_ledger_rejects_raw_padding_boundary_crossing(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            symbols = root / "symbols.csv"
            symbols.write_text(
                "name,unit,rva,kind,provenance\n"
                "global,SOURCE/Test,0x300c,data,cv-public-data\n")
            ledger = root / "ledger.tsv"
            ledger.write_text(
                "name\tunit\tsize\tretail_sha256\thighlow_count\taudit\n"
                "global\tSOURCE/Test\t0x8\tdeadbeef\t0\tbytes\n")
            pe = {"sections": {
                ".data": {"rva": 0x3000, "raw_size": 0x10,
                          "virtual_size": 0x30},
            }}
            with (mock.patch("homm2.build.reviewed_data.read_pe", return_value=pe),
                  mock.patch("homm2.build.reviewed_data.read_pe_payload_evidence",
                             side_effect=AssertionError(
                                 "crossing allocation must fail before payload review"))):
                with self.assertRaisesRegex(
                        RuntimeError,
                        "crosses retail storage data-initialized -> "
                        "data-loader-zero-tail"):
                    reviewed_manifest_bytes(
                        symbols=symbols, ledger=ledger, exe=root / "game.exe")

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
                             side_effect=AssertionError("must not regenerate"))):
                self.assertFalse(ensure_reviewed_targets())

    def test_normal_ensure_does_not_rewrite_objdiff_comparison_paths(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            target = root / "delink"
            target.mkdir()
            stamp = target / ".reviewed-data-stamp.json"
            stamp.write_text('{"mode": "bootstrap"}\n')
            config = root / "build/objdiff/objdiff.json"
            config.parent.mkdir(parents=True)
            expected = (
                '{"units":[{"name":"SOURCE/Test",'
                '"target_path":"./normalized/target/SOURCE/Test.c.obj"}]}\n')
            config.write_text(expected)
            with (mock.patch("homm2.build.reviewed_data.REPO", root),
                  mock.patch("homm2.build.reviewed_data.TARGET", target),
                  mock.patch("homm2.build.reviewed_data.STAMP", stamp)):
                self.assertFalse(ensure_reviewed_targets())
            self.assertEqual(config.read_text(), expected)

    def test_normal_ensure_refuses_stale_canonical_target(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            target = root / "delink"
            target.mkdir()
            stamp = target / ".reviewed-data-stamp.json"
            stamp.write_text('{"mode": "canonical", "digest": "old"}\n')
            inputs = [root / name for name in (
                "source.tsv", "supplement.tsv", "data.tsv", "sections.tsv",
                "breakpoints.json", "contrib.tsv", "coverage.tsv", "coverage.json",
                "game.exe", "game.pdb")]
            for path in inputs:
                path.write_bytes(b"input")
            delinker = root / "vostok-delinker"
            delinker.write_bytes(b"tool")
            with (mock.patch("homm2.build.reviewed_data.TARGET", target),
                  mock.patch("homm2.build.reviewed_data.STAMP", stamp),
                  mock.patch("homm2.build.reviewed_data.DATA_SOURCE_MANIFEST", inputs[0]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_SUPPLEMENTAL", inputs[1]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_MANIFEST", inputs[2]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_SECTION_MANIFEST", inputs[3]),
                  mock.patch("homm2.build.reviewed_data.DATA_BREAKPOINTS", inputs[4]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_CONTRIBUTION_MANIFEST", inputs[5]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_COVERAGE", inputs[6]),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_COVERAGE_DIAGNOSTICS",
                             inputs[7]),
                  mock.patch("homm2.build.reviewed_data.EXE", inputs[8]),
                  mock.patch("homm2.build.reviewed_data.PDB", inputs[9]),
                  mock.patch("homm2.build.reviewed_data._canonical_identity_inputs",
                             return_value={"mode": "canonical", "digest": "new"})):
                with self.assertRaisesRegex(RuntimeError, "regenerate.*explicitly"):
                    ensure_reviewed_targets(delinker=delinker)

    def test_promotion_refreshes_generated_evidence_without_versioning_topology(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            data_manifest = root / "data.tsv"
            data_manifest.write_bytes(b"data\n")
            contribution_manifest = root / "contrib.tsv"
            supplemental = root / "reviewed-supplemental.tsv"
            write_symbol_manifest(supplemental, [])
            diagnostics = root / "diagnostics.json"
            stats = DerivationStats(candidate_definitions=1, open_groups=1)
            open_group = GroupDiagnostic("SOURCE/Test", "data", ("unmapped_definition",),
                                         ("local maps to nothing",))
            with (mock.patch("homm2.build.reviewed_data.derive_allocations",
                             return_value=([], stats, [open_group])),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_MANIFEST", data_manifest),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_SUPPLEMENTAL",
                             supplemental),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_CONTRIBUTION_MANIFEST",
                             contribution_manifest),
                  mock.patch("homm2.build.reviewed_data.CANDIDATE_DIAGNOSTICS", diagnostics),
                  mock.patch("homm2.build.reviewed_data._canonical_union_rows",
                             return_value=[]),
                  mock.patch("homm2.build.reviewed_data._build_coverage_proposal",
                             return_value=(b"coverage\n", [], [])),
                  mock.patch("homm2.build.reviewed_data.build_data_manifests") as build,
                  mock.patch("homm2.build.reviewed_data.contribution_manifest_bytes",
                             side_effect=AssertionError(
                                 "must preserve section-replay contributions"))):
                _stats, open_groups = promote_canonical_topology()
            self.assertEqual(open_groups, [open_group])
            self.assertTrue(diagnostics.is_file())
            build.assert_called_once_with(
                supplemental=supplemental, migrate_from=None, strict=False)
            self.assertFalse(contribution_manifest.exists())

    def test_review_queue_contains_only_noncanonical_evidenced_real_rows(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source.tsv"
            supplemental = root / "supplemental.tsv"
            combined = root / "combined.tsv"
            canonical = symbol_row("?gCanonical@@3HA", "SOURCE/Test", 0x1000)
            write_symbol_manifest(source, [canonical])
            write_symbol_manifest(supplemental, [])
            write_symbol_manifest(combined, [canonical])
            open_candidate = candidate("private$S1", 0x1100)
            diagnostic = GroupDiagnostic(
                "SOURCE/Test", "data", ("uncovered_retail_reference",),
                ("retail RVA 0x1110 has no candidate owner",), (),
                (open_candidate,))

            payload, stats = review_queue_bytes(
                [candidate("?gCanonical@@3HA", 0x1000),
                 candidate("const_00110000", 0x1200)],
                [diagnostic], source, supplemental, combined)

            lines = [line for line in payload.decode().splitlines()
                     if line and not line.startswith("#")]
            rows = list(csv.DictReader(io.StringIO("\n".join(lines)), delimiter="\t"))
            self.assertEqual(len(rows), 1)
            self.assertEqual(rows[0]["row_kind"], "allocation-symbol")
            self.assertEqual(rows[0]["name"], "private$S1")
            self.assertEqual(rows[0]["evidence"], "aligned-relocation-addend")
            self.assertEqual(rows[0]["proof_count"], "3")
            self.assertEqual(
                rows[0]["group_blockers"], '["uncovered_retail_reference"]')
            self.assertIn("no candidate owner", rows[0]["group_contradictions"])
            self.assertEqual(tuple(rows[0]), REVIEW_QUEUE_HEADER)
            self.assertEqual(stats["already_canonical"], 1)
            self.assertEqual(stats["synthetic_filtered"], 1)
            self.assertEqual(stats["review_queue_rows"], 1)

    def test_review_queue_refuses_stale_canonical_union(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source.tsv"
            supplemental = root / "supplemental.tsv"
            combined = root / "combined.tsv"
            write_symbol_manifest(
                source, [symbol_row("?gCanonical@@3HA", "SOURCE/Test", 0x1000)])
            write_symbol_manifest(supplemental, [])
            write_symbol_manifest(combined, [])

            with self.assertRaisesRegex(RuntimeError, "not the exact source DATA"):
                review_queue_bytes(
                    [candidate("private$S1", 0x1100)], [],
                    source, supplemental, combined)

    def test_propose_writes_only_review_queue_and_diagnostics(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source.tsv"
            supplemental = root / "config" / "supplemental.tsv"
            combined = root / "combined.tsv"
            queue = root / "review-queue.tsv"
            diagnostics_path = root / "candidate-diagnostics.json"
            coverage = root / "canonical-coverage.tsv"
            target = root / "delink" / "SOURCE" / "Test.c.obj"
            canonical = symbol_row("?gCanonical@@3HA", "SOURCE/Test", 0x1000)
            write_symbol_manifest(source, [canonical])
            write_symbol_manifest(supplemental, [])
            write_symbol_manifest(combined, [canonical])
            coverage.parent.mkdir(parents=True, exist_ok=True)
            coverage.write_bytes(b"canonical coverage\n")
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(b"fixed target")
            protected = [source, supplemental, combined, coverage, target]
            before = {path: path.read_bytes() for path in protected}
            stats = DerivationStats(candidate_definitions=1, mapped_definitions=1,
                                    closed_groups=1)

            with (mock.patch("homm2.build.reviewed_data.derive_allocations",
                             return_value=([candidate("private$S1", 0x1100)], stats, [])),
                  mock.patch("homm2.build.reviewed_data.DATA_SOURCE_MANIFEST", source),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_SUPPLEMENTAL", supplemental),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_MANIFEST", combined),
                  mock.patch("homm2.build.reviewed_data.REVIEW_QUEUE", queue),
                  mock.patch("homm2.build.reviewed_data.CANDIDATE_DIAGNOSTICS",
                             diagnostics_path),
                  mock.patch("homm2.build.reviewed_data.CANONICAL_COVERAGE", coverage),
                  mock.patch("homm2.build.reviewed_data.TARGET", target.parent.parent),
                  mock.patch("homm2.build.reviewed_data._build_coverage_proposal",
                             side_effect=AssertionError("proposal must not refresh coverage"))):
                _stats, blockers, queue_stats = propose_candidate_topology()

            self.assertEqual(blockers, [])
            self.assertEqual(queue_stats["review_queue_rows"], 1)
            self.assertTrue(queue.is_file())
            self.assertTrue(diagnostics_path.is_file())
            self.assertEqual(before, {path: path.read_bytes() for path in protected})

    def test_canonical_builder_accepts_only_reviewed_supplemental_input(self):
        with TemporaryDirectory() as directory:
            reviewed = Path(directory) / "config" / "reviewed.tsv"
            write_symbol_manifest(reviewed, [])
            with (mock.patch("homm2.build.reviewed_data.CANONICAL_SUPPLEMENTAL", reviewed),
                  mock.patch("homm2.build.reviewed_data.build_data_manifests",
                             return_value={}) as build):
                _build_reviewed_canonical_manifests(strict=True)
            build.assert_called_once_with(
                supplemental=reviewed, migrate_from=None, strict=True)

            write_symbol_manifest(
                reviewed, [symbol_row("const_00110000", "SOURCE/Test", 0x1100)])
            with (mock.patch("homm2.build.reviewed_data.CANONICAL_SUPPLEMENTAL", reviewed),
                  mock.patch("homm2.build.reviewed_data.build_data_manifests") as build):
                with self.assertRaisesRegex(RuntimeError, "forbidden synthetic"):
                    _build_reviewed_canonical_manifests(strict=False)
            build.assert_not_called()

    def test_finalization_refuses_open_groups_before_writing_configs(self):
        stats = DerivationStats(candidate_definitions=1, open_groups=1)
        open_group = GroupDiagnostic("SOURCE/Test", "data", ("unmapped_definition",),
                                     ("local maps to nothing",))
        with (mock.patch("homm2.build.reviewed_data.derive_allocations",
                         return_value=([], stats, [open_group])),
              mock.patch("homm2.build.reviewed_data._build_coverage_proposal",
                         return_value=(b"coverage\n", [], [])),
              mock.patch("homm2.build.reviewed_data._canonical_union_rows",
                         return_value=[]),
              mock.patch("homm2.build.reviewed_data._atomic_write") as write):
            _stats, blockers = promote_canonical_topology(require_all=True)
        self.assertEqual(blockers, [open_group])
        self.assertEqual(write.call_count, 1)  # Disposable diagnostics only.


if __name__ == "__main__":
    unittest.main()
