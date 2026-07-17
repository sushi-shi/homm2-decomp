import struct
import tempfile
import unittest
from pathlib import Path

from homm2.build.link_layout_audit import (
    LayoutAuditError, _anchor_status, _coff_section_payload, build_queue,
    record_outcome,
)


def _unit(unit, retail, candidate, retail_size, raw_size, aligned_size, anchor):
    return {
        "unit": unit,
        "rva_anchor": anchor,
        "retail_rva": hex(retail),
        "contribution_rva": hex(retail),
        "contribution_size": hex(retail_size),
        "candidate_text_raw_size": hex(raw_size),
        "candidate_text_alignment": "0x10",
        "candidate_text_aligned_size": hex(aligned_size),
        "candidate_text_aligned_size_delta": aligned_size - retail_size,
        "candidate_rva": hex(candidate),
        "delta": candidate - retail,
    }


def _symbols(*rows):
    return [
        {"rva": hex(rva), "name": name, "unit": unit, "size": "0x10",
         "kind": "func", "provenance": provenance}
        for unit, name, rva, provenance in rows
    ]


def _map(name, va, owner):
    return {"segment": 1, "offset": va - 0x400000, "name": name, "va": va,
            "flag": "f", "object": owner}


class LinkLayoutAuditTest(unittest.TestCase):
    def setUp(self):
        self.report = {
            "status": "linked",
            "candidate": {"image_base": 0x400000, "section_alignment": 0x1000,
                          "file_alignment": 0x200},
            "linker": {"sha256": "a" * 64},
            "link_flags": ["/MACHINE:IX86"],
            "order_source": "test order",
            "link_input_order": {},
            "units": [
                _unit("SOURCE/A", 0x1000, 0x1000, 0x10, 0x0F, 0x10, "a"),
                _unit("SOURCE/B", 0x1010, 0x1010, 0x10, 0x19, 0x20, "b"),
                _unit("SOURCE/C", 0x1020, 0x1030, 0x10, 0x0F, 0x10, "c"),
            ],
        }
        self.symbols = _symbols(
            ("SOURCE/A", "a", 0x1000, "cv-public-gap"),
            ("SOURCE/B", "b", 0x1010, "cv-public-gap"),
            ("SOURCE/C", "c", 0x1020, "cv-public-gap"),
        )
        self.map_records = [
            _map("a", 0x401000, "A.obj"),
            _map("b", 0x401010, "B.obj"),
            _map("c", 0x401030, "C.obj"),
        ]

    def test_queue_attributes_downstream_shift_to_first_local_root(self):
        rows = build_queue(self.report, self.symbols, self.map_records)
        self.assertEqual([row["kind"] for row in rows],
                         ["exact", "root", "downstream"])
        self.assertTrue(rows[1]["actionable"])
        self.assertEqual(rows[2]["blocker"], "SOURCE/B")
        self.assertEqual(rows[1]["local_delta"], 0x10)
        self.assertEqual(rows[2]["upstream_delta"], 0x10)

    def test_duplicate_map_anchor_fails_closed_instead_of_choosing_first(self):
        self.map_records.append(_map("b", 0x402000, "OTHER.obj"))
        rows = build_queue(self.report, self.symbols, self.map_records)
        self.assertEqual(rows[1]["kind"], "invalid-anchor")
        self.assertEqual(rows[1]["candidate_start"], None)
        self.assertIn("2 spellings", rows[1]["anchor_note"])

    def test_nonpublic_static_thunk_is_not_accepted_as_anchor(self):
        symbols = list(self.symbols)
        symbols[1] = dict(symbols[1], provenance="validated-static-init-thunk")
        status, note = _anchor_status(
            self.report["units"][1], symbols, self.map_records, 0x400000)
        self.assertEqual(status, "invalid")
        self.assertIn("retained public", note)

    def test_exact_record_refuses_a_nonexact_contribution(self):
        rows = build_queue(self.report, self.symbols, self.map_records)
        with self.assertRaisesRegex(LayoutAuditError, "exact outcome requires"):
            record_outcome(
                "SOURCE/B", "exact", "not exact", {"version": 1, "records": []},
                rows, self.report)

    def test_coff_payload_reader_hashes_only_named_section_bytes(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "sample.obj"
            payload = b"code-bytes"
            header = bytearray(20 + 40)
            struct.pack_into("<H", header, 2, 1)
            header[20:28] = b".text\0\0\0"
            struct.pack_into("<II", header, 20 + 16, len(payload), len(header))
            path.write_bytes(header + payload + b"not-section")
            self.assertEqual(_coff_section_payload(path, ".text"), payload)


if __name__ == "__main__":
    unittest.main()
