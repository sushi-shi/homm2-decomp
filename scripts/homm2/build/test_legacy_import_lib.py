import struct
import tempfile
import unittest
from pathlib import Path

from homm2.build.legacy_import_lib import (
    MEMBER_NAME,
    build_archive,
    generate,
    read_definition,
)


ROOT = Path(__file__).resolve().parents[3]
DEFINITION = ROOT / "imports/wing32.def"


def archive_entries(data: bytes):
    offset = 8
    while offset < len(data):
        header = data[offset : offset + 60]
        size = int(header[48:58].decode("ascii").strip())
        start = offset + 60
        end = start + size
        yield header[:16].decode("ascii").rstrip(), data[start:end]
        offset = end + (size & 1)


class LegacyImportLibraryTests(unittest.TestCase):
    def test_definition_preserves_vendor_order_and_true_ordinals(self):
        exports = read_definition(DEFINITION)
        self.assertEqual(
            [spec.lookup for spec in exports],
            [
                "WinGCreateDC",
                "WinGRecommendDIBFormat",
                "WinGCreateBitmap",
                "WinGGetDIBPointer",
                "WinGGetDIBColorTable",
                "WinGSetDIBColorTable",
                "WinGCreateHalftonePalette",
                "WinGCreateHalftoneBrush",
                "WinGStretchBlt",
                "WinGBitBlt",
            ],
        )
        self.assertEqual(
            {spec.lookup: spec.ordinal for spec in exports},
            {
                name: hint + 1
                for hint, name in enumerate(sorted(spec.lookup for spec in exports))
            },
        )

    def test_archive_uses_legacy_member_name_and_regular_coff(self):
        archive = build_archive(read_definition(DEFINITION))
        entries = list(archive_entries(archive))
        self.assertEqual([name for name, _ in entries[:2]], ["/", "/"])
        objects = entries[2:]
        self.assertEqual(len(objects), 13)
        self.assertTrue(all(name == MEMBER_NAME for name, _ in objects))

        descriptor = objects[0][1]
        machine, sections, _, _, symbols, optional_size, characteristics = (
            struct.unpack_from("<HHIIIHH", descriptor, 0)
        )
        self.assertEqual(
            (machine, sections, symbols, optional_size, characteristics),
            (0x14C, 2, 7, 224, 0x8180),
        )
        self.assertIn(b"WING32_IMPORT_DESCRIPTOR\0", descriptor)
        self.assertIn(b"NULL_IMPORT_DESCRIPTOR\0", descriptor)

        bitblt = objects[3][1]
        self.assertEqual(struct.unpack_from("<H", bitblt, 2)[0], 4)
        self.assertIn(b"\0\0WinGBitBlt\0", bitblt)
        self.assertIn(b"_WinGBitBlt@32\0", bitblt)
        self.assertIn(b"__imp__WinGBitBlt@32\0", bitblt)

    def test_generate_writes_only_the_requested_build_output(self):
        with tempfile.TemporaryDirectory() as temp_name:
            output = Path(temp_name) / "wing32.lib"
            self.assertEqual(generate(DEFINITION, output), output)
            self.assertTrue(output.read_bytes().startswith(b"!<arch>\n"))
            self.assertEqual(list(output.parent.iterdir()), [output])


if __name__ == "__main__":
    unittest.main()
