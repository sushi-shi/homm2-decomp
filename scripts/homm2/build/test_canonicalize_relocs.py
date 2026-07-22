import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.build.canonicalize_relocs import (
    CoffFile, Coverage, authorize_owner_alias, record_site_coverage,
)
from homm2.build.test_canonicalize_data_symbols import (
    DIR32, TEXT, SectionSpec, make_coff,
)


class OwnerAliasAuthorizationTest(unittest.TestCase):
    def test_exact_public_owner_and_addend_authorize(self):
        public = {"?gConfig@@3UconfigStruct@@A": 0x128D20}

        self.assertEqual(
            authorize_owner_alias(
                public, "DIR32", "?gConfig@@3UconfigStruct@@A", 0x30,
                0x128D50),
            "?gConfig@@3UconfigStruct@@A")

    def test_wrong_addend_cannot_authorize_target_rewrite(self):
        public = {"?gConfig@@3UconfigStruct@@A": 0x128D20}

        self.assertIsNone(authorize_owner_alias(
            public, "DIR32", "?gConfig@@3UconfigStruct@@A", 0x1C,
            0x128D50))

    def test_owner_name_mismatch_cannot_authorize_target_rewrite(self):
        public = {"?gConfig@@3UconfigStruct@@A": 0x128D20}

        self.assertIsNone(authorize_owner_alias(
            public, "DIR32", "?notConfig@@3UconfigStruct@@A", 0x30,
            0x128D50))


class CoverageTest(unittest.TestCase):
    def test_site_outcomes_are_counted_explicitly(self):
        coverage = Coverage()
        symbols = {"owner": 0x1000, "duplicate": 0x2000}
        data = {}
        duplicates = {"duplicate": {0x2000, 0x3000}}

        self.assertTrue(record_site_coverage(
            coverage, ("DIR32", "owner", 0x10),
            ("DIR32", "const_00001010", 0), symbols, data, duplicates))
        self.assertFalse(record_site_coverage(
            coverage, ("DIR32", "owner", 0), None,
            symbols, data, duplicates))
        self.assertFalse(record_site_coverage(
            coverage, ("DIR32", "owner", 0),
            ("REL32", "owner", 0), symbols, data, duplicates))
        self.assertTrue(record_site_coverage(
            coverage, ("DIR32", "duplicate", 0),
            ("DIR32", "const_00002000", 0), symbols, data, duplicates))
        self.assertTrue(record_site_coverage(
            coverage, ("DIR32", "_compilerLocal$S1", 0),
            ("DIR32", "const_00004000", 0), symbols, data, duplicates))

        self.assertEqual(coverage.same_site_same_type, 3)
        self.assertEqual(coverage.missing_target_site, 1)
        self.assertEqual(coverage.type_mismatch, 1)
        self.assertEqual(coverage.duplicate_string_ambiguity, 1)
        self.assertEqual(coverage.unresolved_base, 1)
        self.assertEqual(coverage.unresolved_target, 0)
        self.assertEqual(coverage.unknown_compiler_local, 1)


class CoffOwnerRewriteTest(unittest.TestCase):
    def test_shared_synthetic_owner_is_not_renamed_globally(self):
        obj = make_coff([SectionSpec(
            ".text", bytes(8), TEXT,
            ((0, 1, DIR32), (4, 1, DIR32)),
        )], [
            ("function", 0, 1, 0x20, 2),
            ("synthetic_owner", 0, 0, 0, 2),
        ])
        with TemporaryDirectory() as directory:
            path = Path(directory) / "test.obj"
            path.write_bytes(obj)
            coff = CoffFile(path)
            function = coff.unique_text_functions()["function"]

            self.assertTrue(coff.patch_dir32(
                function, 0, "synthetic_owner", "public_owner_one", 0))
            self.assertTrue(coff.patch_dir32(
                function, 4, "synthetic_owner", "public_owner_two", 4))
            coff.finish()

            parsed = CoffFile(path)
            relocation_names = [
                parsed.symbols[relocation.symbol_index].name
                for relocation in parsed.relocations.values()
            ]
            self.assertEqual(
                relocation_names, ["public_owner_one", "public_owner_two"])
            self.assertIn(
                "synthetic_owner", {symbol.name for symbol in parsed.symbols.values()})


if __name__ == "__main__":
    unittest.main()
