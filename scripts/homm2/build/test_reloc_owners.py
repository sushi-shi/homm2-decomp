import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.build.reloc_owners import (
    DataOwner,
    load_reviewed_highlow_sites,
    owners_from_rows,
)


def data_row(name, rva, size=0):
    return {"name": name, "kind": "data",
            "rva": "%x" % rva, "size": "%x" % size}


class RelocOwnersTest(unittest.TestCase):
    def test_source_data_row_with_definition_and_extent_is_an_owner(self):
        owners = owners_from_rows(
            [data_row("?gConfig@@3PAXA", 0x3000)],
            {"gConfig": 0x3000}, {"gConfig": (0x3000, 0x40)})
        self.assertEqual(owners, [
            DataOwner(0x3000, 0x40, "?gConfig@@3PAXA", "gConfig")])

    def test_extern_c_names_resolve_with_and_without_the_underscore(self):
        definitions = {"gTable": 0x2000}
        extents = {"gTable": (0x2000, 0x10)}
        for symbol in ("_gTable", "gTable"):
            owners = owners_from_rows(
                [data_row(symbol, 0x2000)], definitions, extents)
            self.assertEqual(len(owners), 1, symbol)
            self.assertEqual(owners[0].source_name, "gTable")

    def test_reviewed_extent_contradicting_the_source_claim_is_fatal(self):
        # Dropping the row would switch the owner-extent rule off for exactly the
        # symbol it was written for, and say nothing.
        with self.assertRaises(ValueError) as raised:
            owners_from_rows(
                [data_row("?gConfig@@3PAXA", 0x3000)],
                {"gConfig": 0x3004}, {"gConfig": (0x3000, 0x40)})
        self.assertIn("gConfig", str(raised.exception))

    def test_inventory_row_at_another_address_disqualifies_the_owner(self):
        owners = owners_from_rows(
            [data_row("?gConfig@@3PAXA", 0x3010)],
            {"gConfig": 0x3000}, {"gConfig": (0x3000, 0x40)})
        self.assertEqual(owners, [])

    def test_a_symbol_without_a_source_claim_is_not_a_contradiction(self):
        self.assertEqual(owners_from_rows(
            [data_row("?gConfig@@3PAXA", 0x3000)], {}, {"gConfig": (0x3000, 0x40)}), [])

    def test_positive_size_smaller_than_the_extent_is_a_contradiction(self):
        definitions = {"gConfig": 0x3000}
        extents = {"gConfig": (0x3000, 0x40)}
        self.assertEqual(owners_from_rows(
            [data_row("?gConfig@@3PAXA", 0x3000, size=0x20)],
            definitions, extents), [])
        # size 0 is "no claim", size >= extent agrees
        self.assertEqual(len(owners_from_rows(
            [data_row("?gConfig@@3PAXA", 0x3000, size=0)],
            definitions, extents)), 1)
        self.assertEqual(len(owners_from_rows(
            [data_row("?gConfig@@3PAXA", 0x3000, size=0x40)],
            definitions, extents)), 1)

    def test_non_data_rows_are_ignored(self):
        rows = [{"name": "?Fn@@YIXXZ", "kind": "func",
                 "rva": "1000", "size": "10"}]
        self.assertEqual(owners_from_rows(
            rows, {"Fn": 0x1000}, {"Fn": (0x1000, 0x10)}), [])

    def test_reviewed_sites_parse_comments_header_and_rows(self):
        with TemporaryDirectory() as directory:
            manifest = Path(directory) / "delink_relocs.tsv"
            manifest.write_text(
                "# reviewed sites\n"
                "site_rva\tkind\n"
                "0x2010\tdir32\n"
                "0x2004\tdir32\n")
            self.assertEqual(
                load_reviewed_highlow_sites(manifest), [0x2004, 0x2010])

    def test_reviewed_sites_empty_manifest_is_zero_sites(self):
        with TemporaryDirectory() as directory:
            manifest = Path(directory) / "delink_relocs.tsv"
            manifest.write_text("site_rva\tkind\n")
            self.assertEqual(load_reviewed_highlow_sites(manifest), [])


if __name__ == "__main__":
    unittest.main()
