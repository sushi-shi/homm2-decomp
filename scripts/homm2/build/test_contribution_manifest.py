import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.build import contribution_manifest


class ContributionManifestTest(unittest.TestCase):
    def test_data_contribution_splits_at_raw_boundary(self):
        contribution = {
            "section_name": ".data",
            "section_rva": 0x3000,
            "section_raw_size": 0x180,
            "section_virtual_size": 0x300,
            "rva": 0x3170,
            "size": 0x30,
        }
        self.assertEqual(contribution_manifest.split_contribution(contribution), [
            ("data", 0x3170, 0x10),
            ("bss", 0x3180, 0x20),
        ])

    def test_unsupported_pe_section_is_not_claimed(self):
        contribution = {
            "section_name": ".idata",
            "section_rva": 0x5000,
            "section_raw_size": 0x100,
            "section_virtual_size": 0x100,
            "rva": 0x5000,
            "size": 0x20,
        }
        self.assertEqual(contribution_manifest.split_contribution(contribution), [])

    def test_manifest_maps_original_module_stem_to_delinker_object(self):
        with tempfile.TemporaryDirectory() as directory:
            units = Path(directory) / "units.toml"
            units.write_text(
                '[[unit]]\nunit = "SOURCE/GAME"\nsource = "src/SOURCE/GAME.cpp"\n')
            contributions = [{
                "section": 1,
                "section_name": ".text",
                "section_rva": 0x1000,
                "section_raw_size": 0x200,
                "section_virtual_size": 0x200,
                "rva": 0x1100,
                "size": 0x40,
            }, {
                "section": 3,
                "section_name": ".data",
                "section_rva": 0x3000,
                "section_raw_size": 0x100,
                "section_virtual_size": 0x300,
                "rva": 0x3200,
                "size": 0x20,
            }]
            modules = {"game": [{"module": r".\GAME.OBJ",
                                  "contributions": contributions}]}
            with mock.patch.object(
                    contribution_manifest, "read_nb09_module_contributions",
                    return_value=modules) as reader:
                rows = contribution_manifest.contribution_rows("game.exe", units)
        reader.assert_called_once_with("game.exe", executable_only=False)
        self.assertEqual(rows, [{
            "object": r"SOURCE\GAME.c",
            "storage": "text",
            "rva": 0x1100,
            "size": 0x40,
            "segment": 1,
            "section": ".text",
            "provenance": "retail-nb09-sstModule",
        }, {
            "object": r"SOURCE\GAME.c",
            "storage": "bss",
            "rva": 0x3200,
            "size": 0x20,
            "segment": 3,
            "section": ".data",
            "provenance": "retail-nb09-sstModule",
        }])

    def test_contribution_past_virtual_section_is_rejected(self):
        contribution = {
            "section_name": ".rdata",
            "section_rva": 0x2000,
            "section_raw_size": 0x100,
            "section_virtual_size": 0x100,
            "rva": 0x20f0,
            "size": 0x20,
        }
        with self.assertRaisesRegex(ValueError, "exceeds PE virtual section"):
            contribution_manifest.split_contribution(contribution)


if __name__ == "__main__":
    unittest.main()
