from __future__ import annotations

import csv
import tempfile
import unittest
from pathlib import Path

from homm2.build.assert_early_stop_bytes import (
    FunctionSpan,
    compare_masked_spans,
    is_raw_identity_claim,
    scan_claims,
)


class EarlyStopClaimTest(unittest.TestCase):
    def test_recognizes_relocation_only_instruction_claim(self):
        marker = """// @early-stop
// @early-stop-reloc-only
// All instructions and 87/87 relocation sites align; only delinked string
// literal symbol identities differ."""
        self.assertTrue(is_raw_identity_claim(marker))

    def test_recognizes_explicit_relocation_masked_bytes(self):
        self.assertTrue(is_raw_identity_claim(
            "// @early-stop-reloc-only reloc-masked: all 320 bytes are identical"))

    def test_does_not_promote_semantic_or_partial_claim(self):
        self.assertFalse(is_raw_identity_claim(
            "// @early-stop all instructions before the jump table align"))
        self.assertFalse(is_raw_identity_claim(
            "// @early-stop all instructions align; only relocations differ"))
        self.assertFalse(is_raw_identity_claim(
            "// @semantic complete CFG; first operand still differs"))

    def test_scanner_maps_explicit_claim_by_va(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            source = root / "src/SOURCE/ARMY.cpp"
            source.parent.mkdir(parents=True)
            source.write_text(
                "// @early-stop\n// @early-stop-reloc-only\n"
                "VA(0x0044aec8, 0x4a6)\nvoid body() {}\n",
                encoding="latin-1",
            )
            symbols = root / "symbols.csv"
            with symbols.open("w", encoding="latin-1", newline="") as stream:
                writer = csv.DictWriter(
                    stream,
                    fieldnames=("rva", "name", "unit", "size", "kind", "provenance"),
                )
                writer.writeheader()
                writer.writerow({
                    "rva": "0x4aec8", "name": "?LoadResources@army@@QAEXXZ",
                    "unit": "SOURCE/ARMY", "size": "0x4a6", "kind": "func",
                    "provenance": "fixture",
                })
            claims = scan_claims(root / "src", symbols)
        self.assertEqual(1, len(claims))
        self.assertEqual("SOURCE/ARMY", claims[0].unit)
        self.assertEqual(0x4AEC8, claims[0].rva)


class MaskedSpanTest(unittest.TestCase):
    def test_relocation_payload_difference_is_masked(self):
        target = FunctionSpan(b"\xa1\x20\x8d\x52\x00\xc3", ((1, 4),))
        base = FunctionSpan(b"\xa1\x50\x8d\x52\x00\xc3", ((1, 4),))
        self.assertEqual([], compare_masked_spans(base, target))

    def test_rejects_different_relocation_sites(self):
        target = FunctionSpan(b"\xa1\x20\x8d\x52\x00\xc3", ((1, 4),))
        base = FunctionSpan(b"\xa1\x50\x8d\x52\x00\xc3", ())
        with self.assertRaisesRegex(ValueError, "different relocation sites"):
            compare_masked_spans(base, target)

    def test_ordinary_member_operand_mismatch_is_not_masked(self):
        # Historical army::LoadResources regression: both instructions test bit
        # four, but the stale flag constant selected this+0xd2 instead of +0xd0.
        target = FunctionSpan(b"\xf6\x80\xd0\x00\x00\x00\x04", ())
        base = FunctionSpan(b"\xf6\x80\xd2\x00\x00\x00\x04", ())
        self.assertEqual([2], compare_masked_spans(base, target))


if __name__ == "__main__":
    unittest.main()
