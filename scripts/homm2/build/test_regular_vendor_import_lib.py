import struct
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.build.regular_vendor_import_lib import (
    generate_named_alias,
    generate_ordinal,
)


def archive_objects(data: bytes):
    objects = []
    offset = 8
    while offset < len(data):
        header = data[offset : offset + 60]
        name = header[:16].decode("ascii").rstrip()
        size = int(header[48:58].decode("ascii").strip())
        start = offset + 60
        if name != "/":
            objects.append(data[start : start + size])
        offset = start + size + (size & 1)
    return objects


def coff_section_data(data: bytes, expected_name: str) -> bytes:
    section_count = struct.unpack_from("<H", data, 2)[0]
    optional_size = struct.unpack_from("<H", data, 16)[0]
    section_offset = 20 + optional_size
    for index in range(section_count):
        offset = section_offset + index * 40
        name = data[offset : offset + 8].rstrip(b"\0").decode("ascii")
        if name == expected_name:
            size, raw_offset = struct.unpack_from("<II", data, offset + 16)
            return data[raw_offset : raw_offset + size]
    raise AssertionError(f"missing COFF section {expected_name}")


class RegularVendorImportLibraryTests(unittest.TestCase):
    def test_ordinal_definition_needs_no_retail_exe(self):
        with tempfile.TemporaryDirectory() as temp_name:
            root = Path(temp_name)
            definition = root / "smack.def"
            output = root / "smack.lib"
            definition.write_text(
                "LIBRARY smackw32.DLL\n"
                "EXPORTS\n"
                "    SmackOpen@12 @14 NONAME\n",
                encoding="ascii",
            )
            with mock.patch(
                "homm2.build.regular_vendor_import_lib.retail_symbols"
            ) as retail:
                generate_ordinal(
                    None, "smackw32.DLL", definition, output
                )
            retail.assert_not_called()
            thunk = archive_objects(output.read_bytes())[3]
            self.assertIn(struct.pack("<I", 0x8000000E) * 2, thunk)

    def test_ordinal_definition_emits_immediate_ordinal_thunks(self):
        with tempfile.TemporaryDirectory() as temp_name:
            root = Path(temp_name)
            definition = root / "smack.def"
            output = root / "smack.lib"
            definition.write_text(
                "LIBRARY smackw32.DLL\n"
                "EXPORTS\n"
                "    SmackOpen@12 @14 NONAME\n",
                encoding="ascii",
            )
            with mock.patch(
                "homm2.build.regular_vendor_import_lib.retail_symbols",
                return_value=[{"ordinal": 14}],
            ):
                generate_ordinal(
                    Path("retail.exe"), "smackw32.DLL", definition, output
                )
            thunk = archive_objects(output.read_bytes())[3]
            self.assertEqual(struct.unpack_from("<H", thunk, 2)[0], 3)
            self.assertIn(struct.pack("<I", 0x8000000E) * 2, thunk)
            self.assertIn(b"_SmackOpen@12\0", thunk)
            self.assertIn(b"__imp__SmackOpen@12\0", thunk)

    def test_named_alias_preserves_caller_lookup_and_hint(self):
        with tempfile.TemporaryDirectory() as temp_name:
            root = Path(temp_name)
            definition = root / "netapi32.def"
            output = root / "netapi32.lib"
            definition.write_text(
                "LIBRARY NETAPI32.dll\nEXPORTS\n    _Netbios@4\n",
                encoding="ascii",
            )
            with mock.patch(
                "homm2.build.regular_vendor_import_lib.retail_symbols",
                return_value=[{"name": "Netbios", "hint": 180}],
            ):
                generate_named_alias(
                    Path("retail.exe"),
                    "NETAPI32.dll",
                    definition,
                    output,
                    "_Netbios@4",
                    "Netbios",
                    180,
                )
            thunk = archive_objects(output.read_bytes())[3]
            self.assertEqual(
                coff_section_data(thunk, ".idata$6"), b"\xb4\0Netbios\0"
            )
            self.assertIn(b"_Netbios@4\0", thunk)
            self.assertIn(b"__imp__Netbios@4\0", thunk)


if __name__ == "__main__":
    unittest.main()
