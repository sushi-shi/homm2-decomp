import unittest
import struct
from pathlib import Path
from unittest import mock

from homm2.build.import_lib import (
    export_names,
    imported_names,
    stub_source,
    validate_export_hints,
    verify_archive_hints,
)


class ImportLibraryTests(unittest.TestCase):
    @staticmethod
    def archive_with_hint(name: str, hint: int) -> bytes:
        blob = struct.pack("<H", hint) + name.encode("ascii") + b"\0"
        coff_header = struct.pack("<HHIIIHH", 0x14C, 1, 0, 0, 0, 0, 0)
        section = struct.pack(
            "<8sIIIIIIHHI",
            b".idata$6", 0, 0, len(blob), 60, 0, 0, 0, 0, 0,
        )
        member = coff_header + section + blob
        archive_header = (
            b"member.obj/     "
            + b"0           "
            + b"0     "
            + b"0     "
            + b"100644  "
            + f"{len(member):<10}".encode("ascii")
            + b"`\n"
        )
        padding = b"\n" if len(member) & 1 else b""
        return b"!<arch>\n" + archive_header + member + padding

    @staticmethod
    def short_import_archive(name: str, hint: int) -> bytes:
        payload = name.encode("ascii") + b"\0mss32.dll\0"
        member = struct.pack(
            "<HHHHIIHH",
            0, 0xFFFF, 0, 0x14C, 0, len(payload), hint, 1 << 2,
        ) + payload
        archive_header = (
            b"mss32.dll/      "
            + b"0           "
            + b"0     "
            + b"0     "
            + b"100644  "
            + f"{len(member):<10}".encode("ascii")
            + b"`\n"
        )
        padding = b"\n" if len(member) & 1 else b""
        return b"!<arch>\n" + archive_header + member + padding

    def test_retail_names_drive_stdcall_stub_decorations(self):
        source = stub_source(
            "mss32.dll",
            ["_AIL_startup@0", "_AIL_stop_sequence@4"],
        )
        self.assertIn("void __stdcall AIL_startup(void) {}", source)
        self.assertIn("void __stdcall AIL_stop_sequence(int a0) {}", source)

    def test_bare_export_is_cdecl(self):
        source = stub_source("example.dll", ["PlainExport"])
        self.assertIn("void PlainExport(void) {}", source)
        self.assertNotIn("__stdcall PlainExport", source)

    def test_imported_names_preserve_retail_decoration(self):
        imports = [{
            "dll": "mss32.dll",
            "symbols": [{"name": "_AIL_startup@0", "hint": 126}],
        }]
        with mock.patch("homm2.build.import_lib.read_imports", return_value=imports):
            self.assertEqual(
                imported_names(Path("retail.exe"), "MSS32.DLL"),
                ["_AIL_startup@0"],
            )

    def test_complete_export_table_reproduces_retail_hints(self):
        imports = [{
            "dll": "audiere.dll",
            "symbols": [
                {"name": "_AdrOpenDevice@8", "hint": 1},
                {"name": "_AdrOpenSampleSource@4", "hint": 2},
            ],
        }]
        exports = [
            "_AdrGetVersion@0",
            "_AdrOpenDevice@8",
            "_AdrOpenSampleSource@4",
        ]
        with mock.patch("homm2.build.import_lib.read_imports", return_value=imports):
            validate_export_hints(Path("retail.exe"), "audiere.dll", exports)

    def test_reads_module_definition(self):
        definition = mock.mock_open(read_data=(
            "; complete table\n"
            "LIBRARY audiere.dll\n"
            "EXPORTS\n"
            "    _AdrOpenDevice@8\n"
            "    _AdrOpenSampleSource@4 ; recovered name\n"
        ))
        with mock.patch.object(Path, "open", definition):
            self.assertEqual(
                export_names(Path("audiere.def"), "AUDIERE.DLL"),
                ["_AdrOpenDevice@8", "_AdrOpenSampleSource@4"],
            )

    def test_incorrect_export_hint_is_rejected(self):
        imports = [{
            "dll": "audiere.dll",
            "symbols": [{"name": "_AdrOpenDevice@8", "hint": 10}],
        }]
        with mock.patch("homm2.build.import_lib.read_imports", return_value=imports):
            with self.assertRaisesRegex(ValueError, "does not reproduce retail hints"):
                validate_export_hints(
                    Path("retail.exe"),
                    "audiere.dll",
                    ["_AdrOpenDevice@8"],
                )

    def test_produced_archive_hint_is_verified(self):
        archive = self.archive_with_hint("_AIL_startup@0", 126)
        with mock.patch.object(Path, "read_bytes", return_value=archive):
            verify_archive_hints(Path("mss32.lib"), {"_AIL_startup@0": 126})

    def test_produced_archive_hint_mismatch_is_rejected(self):
        archive = self.archive_with_hint("_AIL_startup@0", 125)
        with mock.patch.object(Path, "read_bytes", return_value=archive):
            with self.assertRaisesRegex(ValueError, "hint mismatch after synthesis"):
                verify_archive_hints(
                    Path("mss32.lib"), {"_AIL_startup@0": 126}
                )

    def test_vc6_short_import_hint_is_verified(self):
        archive = self.short_import_archive("_AIL_startup@0", 126)
        with mock.patch.object(Path, "read_bytes", return_value=archive):
            verify_archive_hints(Path("mss32.lib"), {"_AIL_startup@0": 126})


if __name__ == "__main__":
    unittest.main()
