from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock


SCRIPT = Path(__file__).with_name("scan_bitfield_residuals.py")
SPEC = importlib.util.spec_from_file_location("scan_bitfield_residuals", SCRIPT)
scanner = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = scanner
SPEC.loader.exec_module(scanner)


def instruction(text: str | None, address: int = 0):
    if text is None:
        return {"diff_kind": "DIFF_INSERT"}
    return {
        "instruction": {
            "address": str(address),
            "size": 1,
            "formatted": text,
        }
    }


def symbol(name: str, rows, size: int, score: float):
    return {
        "name": name,
        "size": str(size),
        "match_percent": score,
        "instructions": rows,
    }


def diff_fixture(retail_rows, candidate_rows, name="?Draw@@QAEXXZ"):
    return {
        "left": {"symbols": [symbol(name, retail_rows, 20, 92.5)]},
        "right": {"symbols": [symbol(name, candidate_rows, 17, 92.5)]},
    }


class BitfieldResidualScannerTests(unittest.TestCase):
    def test_detects_drawcell_masked_retail_against_plain_candidate(self):
        retail = [
            instruction(None),
            instruction("mov al, byte ptr [eax+0x8]", 0x100),
            instruction("and ax, 0x1f", 0x102),
            instruction("and eax, 0xffff", 0x106),
            instruction("mov [length], eax", 0x10B),
        ]
        candidate = [
            instruction("xor ecx, ecx", 0x200),
            instruction("mov cl, [eax+0x8]", 0x202),
            instruction(None),
            instruction(None),
            instruction("mov [length], ecx", 0x204),
        ]

        occurrences = scanner.detect_occurrences(retail, candidate)

        self.assertEqual(len(occurrences), 1)
        self.assertEqual(occurrences[0].direction, "retail_masked_candidate_plain")
        self.assertEqual(occurrences[0].mask, "0x1f")
        self.assertEqual(occurrences[0].memory, "[eax+0x8]")
        self.assertEqual(occurrences[0].retail_address, "0x100")
        self.assertEqual(occurrences[0].candidate_address, "0x200")

    def test_detects_reverse_direction_and_size_suffix_spellings(self):
        retail = [
            instruction("xorl %edx, %edx"),
            instruction("movb 0x6(%eax), %dl"),
            instruction(None),
            instruction(None),
        ]
        candidate = [
            instruction(None),
            instruction("movb 0x6(%eax), %al"),
            instruction("andw $0x3f, %ax"),
            instruction("andl $0xffff, %eax"),
        ]

        occurrences = scanner.detect_occurrences(retail, candidate)

        self.assertEqual(len(occurrences), 1)
        self.assertEqual(occurrences[0].direction, "retail_plain_candidate_masked")
        self.assertEqual(occurrences[0].mask, "0x3f")
        self.assertEqual(occurrences[0].memory, "0x6(%eax)")

    def test_rejects_non_bitfield_masks_sources_and_incomplete_sequences(self):
        candidate = [
            instruction("xor ecx, ecx"),
            instruction("mov cl, [eax+0x8]"),
            instruction(None),
        ]
        bad_retail_rows = (
            [
                instruction("mov al, [eax+0x8]"),
                instruction("and ax, 0xff"),
                instruction("and eax, 0xffff"),
            ],
            [
                instruction("mov al, [eax+0x9]"),
                instruction("and ax, 0x1f"),
                instruction("and eax, 0xffff"),
            ],
            [
                instruction("mov al, [eax+0x8]"),
                instruction("and ax, 0x1f"),
                instruction("mov [length], eax"),
            ],
        )
        for retail in bad_retail_rows:
            with self.subTest(retail=retail):
                self.assertEqual(scanner.detect_occurrences(retail, candidate), [])

    def test_analysis_reports_manifest_identity_scores_sizes_and_counts(self):
        name = "?Draw@@QAEXXZ"
        retail = [
            instruction(None),
            instruction("mov al, [eax+0x8]"),
            instruction("and ax, 0x1f"),
            instruction("and eax, 0xffff"),
        ]
        candidate = [
            instruction("xor ecx, ecx"),
            instruction("mov cl, [eax+0x8]"),
            instruction(None),
            instruction(None),
        ]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            artifact_dir = root / "retained"
            artifact_dir.mkdir()
            (artifact_dir / "best.candidate.obj").write_bytes(b"candidate")
            (artifact_dir / "best.retail.obj").write_bytes(b"retail")
            (artifact_dir / "manifest.json").write_text(json.dumps({
                "target": {
                    "unit": "SOURCE/ADVMGR",
                    "symbol": name,
                    "rva": "0x5bb7c",
                },
                "baseline": {"score": 89.54572},
            }))
            (artifact_dir / "best.objdiff.json").write_text(
                json.dumps(diff_fixture(retail, candidate, name))
            )

            report = scanner.scan([root], executable="unused", cwd=root)

        self.assertEqual(report["scanned_pairs"], 1)
        self.assertEqual(report["finding_count"], 1)
        finding = report["findings"][0]
        self.assertEqual(finding["artifact"], "retained")
        self.assertEqual(finding["unit"], "SOURCE/ADVMGR")
        self.assertEqual(finding["function"], name)
        self.assertEqual(finding["rva"], "0x5bb7c")
        self.assertEqual(finding["masks"], {"0x1f": 1})
        self.assertEqual(finding["score"], 92.5)
        self.assertEqual(finding["baseline_score"], 89.54572)
        self.assertEqual(finding["retail_size"], 20)
        self.assertEqual(finding["candidate_size"], 17)

    def test_missing_json_runs_relocation_aware_objdiff_without_writing_cache(self):
        name = "?Draw@@QAEXXZ"
        retail = [
            instruction(None),
            instruction("mov al, [eax+0x8]"),
            instruction("and ax, 0x7"),
            instruction("and eax, 0xffff"),
        ]
        candidate = [
            instruction("xor ecx, ecx"),
            instruction("mov cl, [eax+0x8]"),
            instruction(None),
            instruction(None),
        ]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "best.candidate.obj").write_bytes(b"candidate")
            (root / "best.retail.obj").write_bytes(b"retail")
            (root / "manifest.json").write_text(json.dumps({
                "target": {"unit": "BASE/TU", "symbol": name, "rva": "0x1234"},
            }))
            completed = mock.Mock(
                returncode=0,
                stdout=json.dumps(diff_fixture(retail, candidate, name)),
                stderr="",
            )
            with mock.patch.object(scanner.subprocess, "run", return_value=completed) as run:
                report = scanner.scan([root], executable="objdiff-test", cwd=root)

            command = run.call_args.args[0]
            self.assertIn(scanner.RELOCATION_CONFIG, command)
            self.assertEqual(command[command.index("-o") + 1], "-")
            self.assertFalse((root / scanner.DEFAULT_JSON_NAME).exists())
            self.assertEqual(report["finding_count"], 1)
            self.assertEqual(report["findings"][0]["objdiff_json"], "generated-on-stdout")

    def test_discovery_ignores_unfinalized_hidden_staging_directory(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            retained = root / "retained"
            staging = root / ".retained.tmp-1234"
            for artifact_dir in (retained, staging):
                artifact_dir.mkdir()
                (artifact_dir / "best.candidate.obj").write_bytes(b"candidate")
                (artifact_dir / "best.retail.obj").write_bytes(b"retail")

            artifacts = scanner.discover_artifacts([root])

        self.assertEqual([artifact.directory.name for artifact in artifacts], ["retained"])

    def test_ranking_prefers_more_occurrences_then_lower_score(self):
        reports = [
            {"occurrence_count": 1, "score": 80.0, "rva": "0x2", "artifact": "b"},
            {"occurrence_count": 2, "score": 99.0, "rva": "0x1", "artifact": "a"},
            {"occurrence_count": 1, "score": 70.0, "rva": "0x3", "artifact": "c"},
        ]
        reports.sort(key=scanner._finding_sort_key)
        self.assertEqual([report["artifact"] for report in reports], ["a", "c", "b"])


if __name__ == "__main__":
    unittest.main()
