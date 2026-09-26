import unittest

from homm2.build.gen_vendor_imports import (
    ADVAPI_IMPORTS,
    MSS_IMPORTS,
    SMACK_IMPORTS,
    WING_IMPORTS,
    import_specs,
)


class VendorImportTests(unittest.TestCase):
    def test_retail_import_inventory(self):
        self.assertEqual(len(MSS_IMPORTS), 33)
        self.assertEqual({ordinal for _, ordinal in SMACK_IMPORTS},
                         {14, 18, 19, 20, 21, 23, 28, 32, 33, 38})
        self.assertEqual(len(WING_IMPORTS), 6)
        self.assertEqual(
            [lookup for _, _, lookup in ADVAPI_IMPORTS],
            ["RegOpenKeyExA", "RegSetValueExA", "RegCreateKeyA",
             "RegQueryValueExA", "RegCloseKey"],
        )

    def test_each_dll_uses_its_retail_import_form(self):
        specs = import_specs()
        self.assertTrue(all(not spec.noname and spec.lookup_name is None
                            for spec in specs if spec.dll == "mss32.dll"))
        self.assertTrue(all(spec.noname
                            for spec in specs if spec.dll == "smackw32.DLL"))
        self.assertTrue(all(not spec.noname and spec.lookup_name
                            for spec in specs if spec.dll == "WING32.dll"))
        self.assertTrue(all(not spec.noname and spec.lookup_name
                            for spec in specs if spec.dll == "ADVAPI32.dll"))


if __name__ == "__main__":
    unittest.main()
