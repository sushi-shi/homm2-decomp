import unittest
from pathlib import Path
from unittest import mock

from homm2.build.import_lib import imported_names, stub_source


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


if __name__ == "__main__":
    unittest.main()
