import struct
import tempfile
import unittest
from pathlib import Path

from homm2.build.link_diff import compare, main, read_ceiling, regions


def image(text: bytes, data: bytes, overlay: bytes = b"") -> bytes:
    """A minimal PE layout: headers to 0x200, then .text and .data raw data."""
    header = bytearray(0x200)
    header[0:2] = b"MZ"
    struct.pack_into("<I", header, 0x3C, 0x40)
    header[0x40:0x44] = b"PE\0\0"
    struct.pack_into("<H", header, 0x46, 2)       # NumberOfSections
    struct.pack_into("<H", header, 0x54, 0xE0)    # SizeOfOptionalHeader
    table = 0x40 + 24 + 0xE0
    for index, (name, pointer, size) in enumerate(
            ((b".text", 0x200, len(text)), (b".data", 0x200 + len(text), len(data)))):
        entry = table + 40 * index
        header[entry:entry + 8] = name.ljust(8, b"\0")
        struct.pack_into("<II", header, entry + 16, size, pointer)
    return bytes(header) + text + data + overlay


class LinkDiffTests(unittest.TestCase):
    def test_regions_attribute_bytes_to_headers_sections_and_overlay(self):
        retail = image(b"\x90" * 16, b"\0" * 16, b"tail")
        candidate = bytearray(image(b"\x90" * 16, b"\0" * 16, b"tail"))
        candidate[0x10] ^= 1           # header
        candidate[0x200 + 3] ^= 1      # .text
        candidate[0x210 + 1] ^= 1      # .data
        candidate[0x210 + 2] ^= 1
        counts = regions(retail, bytes(candidate) + b"!")
        self.assertEqual(counts, {"size": 1, "headers": 1, ".text": 1, ".data": 2,
                                  "overlay": 1})

    def test_a_rise_or_an_unbanked_region_fails_and_a_drop_is_bankable(self):
        regressions, bankable = compare({"headers": 0, ".text": 5, ".data": 1},
                                        {"headers": 2, ".text": 4})
        self.assertEqual(regressions, [".text: 5 differing bytes > ceiling 4",
                                       ".data: 1 differing bytes; no ceiling banked"])
        self.assertEqual(bankable, ["headers: 0 < ceiling 2"])

    def test_update_banks_counts_and_the_gate_then_passes(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            retail, candidate = root / "retail.exe", root / "candidate.exe"
            retail.write_bytes(image(b"\x90" * 16, b"\0" * 16))
            changed = bytearray(retail.read_bytes())
            changed[0x205] = 0xCC
            candidate.write_bytes(bytes(changed))
            ceiling, stamp = root / "ceiling.tsv", root / "stamp.tsv"
            arguments = ["--retail", str(retail), "--candidate", str(candidate),
                         "--ceiling", str(ceiling), "--stamp", str(stamp)]
            self.assertEqual(main(arguments), 1)
            self.assertFalse(stamp.exists())
            self.assertEqual(main(arguments + ["--update"]), 0)
            self.assertEqual(read_ceiling(ceiling)[".text"], 1)
            self.assertEqual(main(arguments), 0)
            self.assertTrue(stamp.exists())


if __name__ == "__main__":
    unittest.main()
