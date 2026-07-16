import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from types import SimpleNamespace

from homm2.build.whole_image_coverage import (
    CoverageRow,
    _partition,
    _validate_partition,
    data_partition,
    load_contributions,
)


class WholeImageCoverageTest(unittest.TestCase):
    def test_loads_the_exact_classified_contribution_manifest(self):
        with TemporaryDirectory() as directory:
            path = Path(directory) / "contributions.tsv"
            path.write_text(
                "# generated\n"
                "object\tstorage\trva\tsize\tsegment\tsection\tprovenance\n"
                "SOURCE/Test.c\tdata\t0x100\t0x8\t3\t.data\treviewed-data\n"
                "SOURCE/Test.c\tbss\t0x108\t0x10\t3\t.data\treviewed-bss\n")
            rows = load_contributions(path)
            self.assertEqual([(row["storage"], row["rva"], row["size"])
                              for row in rows], [
                ("data", 0x100, 8), ("bss", 0x108, 0x10),
            ])

            path.write_text(
                "object\tstorage\trva\tsize\tsegment\tsection\tprovenance\n"
                "SOURCE/Test.c\tbss\t0x100\t0x8\t3\t.rdata\twrong\n")
            with self.assertRaisesRegex(ValueError, "storage/section mismatch"):
                load_contributions(path)

    def test_partition_fills_gaps_and_rejects_overlaps(self):
        middle = CoverageRow("test", "owner", "data", 4, 4,
                             "allocation", "value", "test")
        rows = _partition(0, 12, [(4, 8, middle)], lambda left, right: CoverageRow(
            "test", "owner", "data", left, right - left,
            "padding", "-", "test"))
        self.assertEqual([(row.start, row.size) for row in rows], [(0, 4), (4, 4), (8, 4)])
        self.assertIsNone(_validate_partition(rows, "test", 0, 12))
        with self.assertRaisesRegex(ValueError, "overlap"):
            _partition(0, 12, [(2, 7, middle), (6, 8, middle)],
                       lambda left, right: middle)

    def test_data_partition_emits_only_proven_zero_padding(self):
        with TemporaryDirectory() as directory:
            symbols = Path(directory) / "symbols.csv"
            symbols.write_text(
                "rva,name,unit,size,kind,provenance\n"
                "0x100,public,SOURCE/Test,0x4,data,cv-public-data\n")
            allocations = [
                SimpleNamespace(unit="SOURCE/Test", storage="data", name="public",
                                rva=0x100, size=4, scope="external", provenance="test"),
                SimpleNamespace(unit="SOURCE/Test", storage="data", name="$SG1",
                                rva=0x108, size=4, scope="local", provenance="test"),
            ]
            contributions = [{
                "object": "SOURCE\\Test.c", "storage": "data",
                "rva": 0x100, "size": 0xC,
            }]
            pe = {
                "data": b"\x01\x02\x03\x04\0\0\0\0literal!",
                "raw_offset": lambda rva: rva - 0x100,
            }
            rows, padding, diagnostics = data_partition(
                allocations, contributions, pe, symbols)
            self.assertFalse(diagnostics)
            self.assertEqual([(row.start, row.size, row.kind) for row in rows], [
                (0x100, 4, "allocation"),
                (0x104, 4, "padding"),
                (0x108, 4, "literal"),
            ])
            self.assertEqual([(row.rva, row.size) for row in padding], [(0x104, 4)])

            pe["data"] = b"\x01\x02\x03\x04BAD!literal!"
            _rows, _padding, diagnostics = data_partition(
                allocations, contributions, pe, symbols)
            self.assertEqual([row.cause for row in diagnostics],
                             ["unclassified_nonzero_gap"])


if __name__ == "__main__":
    unittest.main()
