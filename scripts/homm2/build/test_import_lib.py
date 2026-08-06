import unittest
from pathlib import Path
from unittest import mock

from homm2.build.import_lib import (
    export_names,
    imported_names,
    stub_source,
    validate_export_hints,
)


class ImportLibraryTests(unittest.TestCase):
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


if __name__ == "__main__":
    unittest.main()
