import struct
import tempfile
import unittest
from pathlib import Path

from homm2.build.regular_import_lib import build_archive, read_definition


def archive_entries(data: bytes):
    offset = 8
    while offset < len(data):
        header = data[offset : offset + 60]
        size = int(header[48:58].decode("ascii").strip())
        start = offset + 60
        end = start + size
        yield header[:16].decode("ascii").rstrip(), data[start:end]
        offset = end + (size & 1)


class RegularImportLibraryTests(unittest.TestCase):
    def test_definition_requires_literal_decorated_named_exports(self):
        with tempfile.TemporaryDirectory() as temp_name:
            definition = Path(temp_name) / "vendor.def"
            definition.write_text(
                "LIBRARY vendor.dll\n"
                "EXPORTS\n"
                "    _Beta@4\n"
                "    _Alpha@0\n",
                encoding="ascii",
            )
            self.assertEqual(
                read_definition(definition, "VENDOR.DLL"),
                ("vendor.dll", ["_Beta@4", "_Alpha@0"]),
            )

    def test_archive_uses_only_regular_coff_and_sorted_hints(self):
        archive = build_archive(
            "vendor.dll", ["_Beta@4", "_Alpha@0"], {"_Alpha@0"}
        )
        entries = list(archive_entries(archive))
        self.assertEqual([name for name, _ in entries[:2]], ["/", "/"])
        objects = entries[2:]
        self.assertEqual(len(objects), 5)
        self.assertTrue(all(name == "vendor.dll/" for name, _ in objects))
        self.assertTrue(all(obj[:4] != b"\0\0\xff\xff" for _, obj in objects))
        self.assertTrue(all(struct.unpack_from("<H", obj)[0] == 0x14C
                            for _, obj in objects))

        descriptor = objects[0][1]
        self.assertIn(b"__IMPORT_DESCRIPTOR_vendor\0", descriptor)
        self.assertIn(b"__NULL_IMPORT_DESCRIPTOR\0", descriptor)
        self.assertIn(b"\x7fvendor_NULL_THUNK_DATA\0", descriptor)

        alpha = objects[3][1]
        beta = objects[4][1]
        self.assertIn(b"\0\0_Alpha@0\0", alpha)
        self.assertIn(b"\x01\0_Beta@4\0", beta)
        self.assertIn(b"__imp__Alpha@0\0", alpha)
        self.assertIn(b"__Beta@4\0", beta)
        self.assertIn(b"__imp___Beta@4\0", beta)


if __name__ == "__main__":
    unittest.main()
