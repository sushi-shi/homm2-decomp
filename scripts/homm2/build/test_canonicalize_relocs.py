import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.build.canonicalize_relocs import (
    CoffFile, Coverage, authorize_import_alias, authorize_missing_self_rel32,
    authorize_owner_alias, authorize_rel32_alias, import_iat_aliases,
    load_reviewed_rel32_aliases, record_site_coverage,
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


class ImportAliasAuthorizationTest(unittest.TestCase):
    SYMBOL = "__imp__AdrOpenSampleSource@4"

    def test_exact_iat_slot_authorizes(self):
        self.assertEqual(authorize_import_alias(
            {self.SYMBOL: 0xEA324}, "DIR32", self.SYMBOL, 0, 0xEA324),
            self.SYMBOL)

    def test_undecorated_pe_name_authorizes_exact_stdcall_slot(self):
        symbol = "__imp__CreateBitmapIndirect@4"
        self.assertEqual(authorize_import_alias(
            {("stdcall", "CreateBitmapIndirect"): 0xEA1E4},
            "DIR32", symbol, 0, 0xEA1E4), symbol)

    def test_undecorated_pe_name_authorizes_exact_cdecl_slot(self):
        symbol = "__imp__wsprintfA"
        self.assertEqual(authorize_import_alias(
            {("cdecl", "wsprintfA"): 0xEA218},
            "DIR32", symbol, 0, 0xEA218), symbol)

    def test_wrong_slot_type_addend_or_symbol_cannot_authorize(self):
        imports = {self.SYMBOL: 0xEA324}
        self.assertIsNone(authorize_import_alias(
            imports, "DIR32", self.SYMBOL, 0, 0xEA328))
        self.assertIsNone(authorize_import_alias(
            imports, "REL32", self.SYMBOL, 0, 0xEA324))
        self.assertIsNone(authorize_import_alias(
            imports, "DIR32", self.SYMBOL, 4, 0xEA324))
        self.assertIsNone(authorize_import_alias(
            imports, "DIR32", "__imp__AdrOpenDevice@8", 0, 0xEA324))
        self.assertIsNone(authorize_import_alias(
            {("stdcall", "CreateBitmapIndirect"): 0xEA1E4},
            "DIR32", "__imp__CreateIconIndirect@4", 0, 0xEA1E4))
        self.assertIsNone(authorize_import_alias(
            {("cdecl", "wsprintfA"): 0xEA218},
            "DIR32", "__imp__wvsprintfA", 0, 0xEA218))


class ImportIatAliasesTest(unittest.TestCase):
    def test_reviewed_smack_ordinal_recovers_exact_coff_name(self):
        self.assertEqual(
            import_iat_aliases(
                "SMACKW32.DLL", 0x8000000E,
                lambda _rva: self.fail("ordinal import read a name")),
            ("__imp__SmackOpen@12",),
        )

    def test_unknown_dll_or_ordinal_has_no_alias(self):
        read_string = lambda _rva: self.fail("ordinal import read a name")
        self.assertEqual(import_iat_aliases(
            "other.dll", 0x8000000E, read_string), ())
        self.assertEqual(import_iat_aliases(
            "smackw32.dll", 0x8000000F, read_string), ())

    def test_named_import_keeps_exact_and_mechanical_aliases(self):
        self.assertEqual(
            import_iat_aliases(
                "KERNEL32.dll", 0x1234,
                lambda rva: "CreateFileA" if rva == 0x1236 else "wrong"),
            ("__imp_CreateFileA", ("stdcall", "CreateFileA"),
             ("cdecl", "CreateFileA")),
        )


class Rel32AliasAuthorizationTest(unittest.TestCase):
    def test_same_unique_rva_and_addend_authorize(self):
        symbols = {"_open": 0xE0760, "__open": 0xE0760}

        self.assertEqual(authorize_rel32_alias(
            symbols, {}, {}, ("REL32", "__open", 0),
            ("REL32", "_open", 0)), "__open")

    def test_different_rva_or_addend_cannot_authorize(self):
        symbols = {"_open": 0xE0760, "__open": 0xE0770}

        self.assertIsNone(authorize_rel32_alias(
            symbols, {}, {}, ("REL32", "__open", 0),
            ("REL32", "_open", 0)))
        symbols["__open"] = symbols["_open"]
        self.assertIsNone(authorize_rel32_alias(
            symbols, {}, {}, ("REL32", "__open", 4),
            ("REL32", "_open", 0)))

    def test_ambiguous_name_cannot_authorize(self):
        symbols = {"left": 0x1000, "right": 0x1000}
        duplicates = {"right": {0x1000, 0x2000}}

        self.assertIsNone(authorize_rel32_alias(
            symbols, {}, duplicates, ("REL32", "left", 0),
            ("REL32", "right", 0)))


class ReviewedRel32AliasesTest(unittest.TestCase):
    def _manifest(self, directory, row):
        path = Path(directory) / "aliases.tsv"
        path.write_text(
            "alias\tcanonical\trva\tprovenance\n" + row + "\n",
            encoding="utf-8")
        return path

    def test_exact_canonical_rva_adds_alias(self):
        symbols = {"_open": 0xD8C6E}
        duplicates = {"_open": {0xD8C6E}}
        with TemporaryDirectory() as directory:
            load_reviewed_rel32_aliases(
                self._manifest(
                    directory,
                    "__open\t_open\t0x000d8c6e\tVC6 open.obj"),
                symbols, duplicates)
        self.assertEqual(symbols["__open"], 0xD8C6E)
        self.assertEqual(duplicates["__open"], {0xD8C6E})

    def test_wrong_canonical_or_conflicting_alias_fails_closed(self):
        with TemporaryDirectory() as directory:
            path = self._manifest(
                directory,
                "__open\t_open\t0x000d8c6e\tVC6 open.obj")
            with self.assertRaisesRegex(ValueError, "does not resolve uniquely"):
                load_reviewed_rel32_aliases(
                    path, {"_open": 0xD8C70}, {"_open": {0xD8C70}})
            with self.assertRaisesRegex(ValueError, "conflicts"):
                load_reviewed_rel32_aliases(
                    path,
                    {"_open": 0xD8C6E, "__open": 0xD8C70},
                    {"_open": {0xD8C6E}, "__open": {0xD8C70}})

    def test_missing_provenance_fails_closed(self):
        with TemporaryDirectory() as directory:
            path = self._manifest(
                directory, "__open\t_open\t0x000d8c6e\t")
            with self.assertRaisesRegex(ValueError, "lacks a name or provenance"):
                load_reviewed_rel32_aliases(
                    path, {"_open": 0xD8C6E}, {"_open": {0xD8C6E}})


class MissingSelfRel32AuthorizationTest(unittest.TestCase):
    def test_exact_linked_self_target_authorizes(self):
        function_rva = 0x1A1D9
        site = 0x10B4
        displacement = function_rva - (function_rva + site + 4)

        self.assertEqual(authorize_missing_self_rel32(
            function_rva, site + 4, "function", site,
            ("REL32", "function", 0),
            b"\xe8",
            displacement.to_bytes(4, "little", signed=True)), "function")

    def test_wrong_identity_addend_or_target_is_rejected(self):
        operand = (-0x24).to_bytes(4, "little", signed=True)

        self.assertIsNone(authorize_missing_self_rel32(
            0x1000, 0x40, "function", 0x20,
            ("REL32", "other", 0), b"\xe8", operand))
        self.assertIsNone(authorize_missing_self_rel32(
            0x1000, 0x40, "function", 0x20,
            ("REL32", "function", 4), b"\xe8", operand))
        self.assertIsNone(authorize_missing_self_rel32(
            0x1000, 0x40, "function", 0x24,
            ("REL32", "function", 0), b"\xe8", operand))

    def test_site_outside_reviewed_function_is_rejected(self):
        self.assertIsNone(authorize_missing_self_rel32(
            0x1000, 0x20, "function", 0x20,
            ("REL32", "function", 0), b"\xe8", bytes(4)))

    def test_non_call_operand_is_rejected(self):
        self.assertIsNone(authorize_missing_self_rel32(
            0x1000, 0x40, "function", 0x20,
            ("REL32", "function", 0), b"\x00",
            (-0x24).to_bytes(4, "little", signed=True)))


class RelocationInsertionTest(unittest.TestCase):
    FUNCTION_TYPE = 0x20

    def test_dir32_and_rel32_records_keep_their_types(self):
        obj = make_coff(
            [SectionSpec(".text", b"\0\0\0\0\x01\x02\x03\x04", TEXT)],
            [("function", 0, 1, self.FUNCTION_TYPE, 2)])
        with TemporaryDirectory() as directory:
            path = Path(directory) / "test.obj"
            path.write_bytes(obj)
            coff = CoffFile(path)
            function = coff.unique_text_functions()["function"]

            self.assertTrue(coff.insert_dir32(function, 0, "absolute"))
            self.assertTrue(coff.insert_rel32(function, 4, "function"))
            coff.finish()

            parsed = CoffFile(path)
            records = [(site, relocation.typ,
                        parsed.symbols[relocation.symbol_index].name)
                       for (_section, site), relocation
                       in sorted(parsed.relocations.items())]
            self.assertEqual(records, [
                (0, DIR32, "absolute"),
                (4, 0x14, "function"),
            ])
            section = parsed.sections[0]
            self.assertEqual(
                parsed.data[section.raw_offset + 4:section.raw_offset + 8],
                bytes(4))


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

    def test_rel32_alias_is_rewritten_only_at_the_paired_site(self):
        obj = make_coff([SectionSpec(
            ".text", bytes(8), TEXT,
            ((0, 1, 0x14), (4, 1, 0x14)),
        )], [
            ("function", 0, 1, 0x20, 2),
            ("_open", 0, 0, 0, 2),
        ])
        with TemporaryDirectory() as directory:
            path = Path(directory) / "test.obj"
            path.write_bytes(obj)
            coff = CoffFile(path)
            function = coff.unique_text_functions()["function"]

            self.assertTrue(coff.patch_rel32(
                function, 0, "_open", "__open"))
            coff.finish()

            parsed = CoffFile(path)
            relocation_names = [
                parsed.symbols[relocation.symbol_index].name
                for relocation in parsed.relocations.values()
            ]
            self.assertEqual(relocation_names, ["__open", "_open"])


class AlignmentBoundaryTest(unittest.TestCase):
    FUNCTION_TYPE = 0x20

    def _coff(self, text, extra_symbols=()):
        obj = make_coff(
            [SectionSpec(".text", bytes(text), TEXT)],
            [("function", 0, 1, self.FUNCTION_TYPE, 2)] + list(extra_symbols))
        directory = TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        path = Path(directory.name) / "test.obj"
        path.write_bytes(obj)
        return path

    def _boundaries(self, path):
        parsed = CoffFile(path)
        return [(symbol.name, symbol.value, symbol.section)
                for symbol in parsed.symbols.values()
                if symbol.name.startswith("$fnpad@")]

    def test_pure_nop_fill_before_next_symbol_is_bounded(self):
        path = self._coff(
            b"\xc3\x90\x90\x90" + bytes(4),
            [("next_function", 4, 1, self.FUNCTION_TYPE, 2)])
        coff = CoffFile(path)
        function = coff.unique_text_functions()["function"]

        self.assertTrue(coff.add_alignment_boundary(function, 1))
        coff.finish()

        self.assertEqual(self._boundaries(path), [("$fnpad@1", 1, 1)])

    def test_int3_fill_before_section_end_is_bounded(self):
        path = self._coff(b"\xc3\xcc\xcc\xcc")
        coff = CoffFile(path)
        function = coff.unique_text_functions()["function"]

        self.assertTrue(coff.add_alignment_boundary(function, 1))
        coff.finish()

        self.assertEqual(self._boundaries(path), [("$fnpad@1", 1, 1)])

    def test_non_fill_byte_keeps_the_span_unbounded(self):
        path = self._coff(b"\xc3\x90\x04\x90")
        coff = CoffFile(path)
        function = coff.unique_text_functions()["function"]

        self.assertFalse(coff.add_alignment_boundary(function, 1))
        coff.finish()

        self.assertEqual(self._boundaries(path), [])

    def test_fill_of_sixteen_or_more_bytes_is_not_alignment(self):
        path = self._coff(b"\xc3" + b"\x90" * 16)
        coff = CoffFile(path)
        function = coff.unique_text_functions()["function"]

        self.assertFalse(coff.add_alignment_boundary(function, 1))

    def test_exact_span_needs_no_boundary(self):
        path = self._coff(
            b"\xc3\x90\x90\x90" + bytes(4),
            [("next_function", 4, 1, self.FUNCTION_TYPE, 2)])
        coff = CoffFile(path)
        function = coff.unique_text_functions()["function"]

        self.assertFalse(coff.add_alignment_boundary(function, 4))

    def test_claim_beyond_span_is_rejected(self):
        path = self._coff(
            b"\xc3\x90\x90\x90" + bytes(4),
            [("next_function", 4, 1, self.FUNCTION_TYPE, 2)])
        coff = CoffFile(path)
        function = coff.unique_text_functions()["function"]

        self.assertFalse(coff.add_alignment_boundary(function, 5))
        self.assertFalse(coff.add_alignment_boundary(function, 0))


if __name__ == "__main__":
    unittest.main()
