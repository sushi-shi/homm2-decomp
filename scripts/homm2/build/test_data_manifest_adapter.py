import unittest
from dataclasses import replace
from pathlib import Path
from tempfile import TemporaryDirectory
from types import SimpleNamespace
from unittest import mock

from homm2.build.data_manifest_adapter import (
    CandidateDefinition,
    CandidateSection,
    _bind_compgen_edges,
    _compgen_candidate_kind,
    _folded_compgen_rows,
    _interior_compgen_aliases,
    _unique_reviewed_pointer_sequence_rva,
    candidate_topology,
    candidate_common_manifest_bytes,
    delinker_manifest_bytes,
    resolve_compgen_definitions,
    resolve_vtable_definitions,
    resolve_source_definitions,
    source_definitions,
    validate_symbol_rows,
    _mark_vtable_aliases,
    _symbol_row,
    _vtable_row,
    automatic_real_rows,
    automatic_string_rows,
)
from homm2.build.annotated_compgen_data import CompgenDataClaim
from homm2.build.annotated_vtables import AnnotatedVtable
from homm2.build.test_data_topology_census import (
    COMDAT_DATA_FLAGS,
    DATA_FLAGS,
    _coff,
)


def candidate(unit, symbol, ordinal=1, value=0):
    return CandidateDefinition(
        unit, symbol, ordinal, ".data", value, value, 4, 4, "data",
        "global" if symbol.startswith("?") else "local",
        2 if symbol.startswith("?") else 3, DATA_FLAGS, 0, None,
    )


def section(unit, ordinal, size, alignment=1, storage="data", selection=0,
            name=None):
    name = name or (".bss" if storage == "bss" else ".data")
    return CandidateSection(
        unit, unit + ".c", ordinal, name, size, alignment, DATA_FLAGS,
        storage, 0, selection, None)




class DataManifestAdapterTest(unittest.TestCase):
    def test_automatic_reals_preserve_width_and_fold_exact_comdats(self):
        symbol = "__real@8@4002c800000000000000"
        owner = CandidateDefinition(
            "A", symbol, 1, ".rdata", 0, 0, 8, 8, "rdata", "global",
            2, COMDAT_DATA_FLAGS, 2, None)
        peer = replace(owner, unit="B")
        owner_section = CandidateSection(
            "A", "A.c", 1, ".rdata", 8, 8, COMDAT_DATA_FLAGS,
            "rdata", 0x12345678, 2, None)
        peer_section = replace(owner_section, unit="B", object_name="B.c")
        payload = bytes.fromhex("0000000000002940")
        placement = [SimpleNamespace(
            unit="A", name=symbol, storage="rdata", section_offset=0,
            rva=0x100, provenance="aligned-relocation-addend",
        )]
        pe = {
            "sections": {
                ".rdata": {
                    "rva": 0x100, "virtual_size": 8, "raw_size": 8,
                },
            },
        }
        with mock.patch(
                "homm2.build.data_manifest_adapter.CoffFile"), mock.patch(
                "homm2.build.data_manifest_adapter._candidate_bytes",
                return_value=payload), mock.patch(
                    "homm2.build.data_manifest_adapter.read_pe",
                    return_value=pe), mock.patch(
                    "homm2.build.data_manifest_adapter._pe_layout",
                    return_value=(
                        0x400000, (0x10,), lambda _site: 0x400100,
                        lambda _rva, size: payload[:size],
                    )):
            rows = automatic_real_rows(
                {"A": ([owner], [owner_section]),
                 "B": ([peer], [peer_section])},
                Path("base"), Path("game.exe"), placement)

        self.assertEqual(
            [(row["object"], row["name"], row["rva"], row["size"])
             for row in rows],
            [("A.c", symbol, "0x100", "0x8"),
             ("B.c", symbol, "0x100", "0x8")])
        self.assertTrue(all(
            row["provenance"] ==
            "candidate-COFF-real:aligned-relocation-addend:"
            "candidate-coff-folded-comdat"
            for row in rows))

    def test_automatic_real_reserves_source_claimed_symbol(self):
        symbol = "__real@4@40008000000000000000"
        real = CandidateDefinition(
            "A", symbol, 1, ".rdata", 0, 0, 4, 4, "rdata", "global",
            2, COMDAT_DATA_FLAGS, 2, None)
        real_section = CandidateSection(
            "A", "A.c", 1, ".rdata", 4, 4, COMDAT_DATA_FLAGS,
            "rdata", 0x12345678, 2, None)
        with mock.patch("homm2.build.data_manifest_adapter.CoffFile"):
            rows = automatic_real_rows(
                {"A": ([real], [real_section])}, Path("base"),
                Path("game.exe"), (), reserved_symbols=(symbol,))
        self.assertEqual(rows, [])

    def test_automatic_strings_use_direct_proof_then_unique_content(self):
        direct = replace(candidate("A", "$SG1", value=0), size=8)
        collision = replace(candidate("A", "$SG2", value=8), size=8)
        unique = replace(candidate("A", "$SG3", value=16), size=8)
        topology = {"A": ([direct, collision, unique], [])}
        payloads = {
            "$SG1": b"same\0\0\0\0",
            "$SG2": b"same\0\0\0\0",
            "$SG3": b"unique\0\0",
        }
        retail = {
            0x100: b"same\0\0\0\0",
            0x200: b"same\0\0\0\0",
            0x300: b"unique\0\0",
        }

        def read_bytes(rva, size):
            return retail[rva][:size]

        pe = {
            "sections": {
                ".data": {
                    "rva": 0x100, "virtual_size": 0x300,
                    "raw_size": 0x300,
                },
            },
        }
        placement = [SimpleNamespace(
            unit="A", name="$SG1", storage="data", section_offset=0,
            rva=0x100, provenance="aligned-relocation-addend",
        )]
        with mock.patch(
                "homm2.build.data_manifest_adapter.CoffFile"), mock.patch(
                "homm2.build.data_manifest_adapter._candidate_bytes",
                side_effect=lambda _coff, row, size: payloads[row.symbol][:size]), \
                mock.patch(
                    "homm2.build.data_manifest_adapter.read_pe",
                    return_value=pe), mock.patch(
                    "homm2.build.data_manifest_adapter._pe_layout",
                    return_value=(
                        0x400000, (0x10, 0x14, 0x18),
                        lambda site: 0x400000 + {
                            0x10: 0x100, 0x14: 0x200, 0x18: 0x300,
                        }[site],
                        read_bytes,
                    )):
            rows = automatic_string_rows(
                topology, Path("base"), Path("game.exe"), placement)

        self.assertEqual([(row["name"], row["rva"], row["provenance"])
                          for row in rows], [
            ("$SG1", "0x100",
             "candidate-COFF-string:aligned-relocation-addend"),
            ("$SG3", "0x300",
             "candidate-COFF-string:unique-relocation-target-payload"),
        ])

    def test_automatic_string_reservation_keeps_collision_ambiguous(self):
        claimed = replace(candidate("A", "$SG1", value=0), size=8)
        peer = replace(candidate("A", "$SG2", value=8), size=8)
        topology = {"A": ([claimed, peer], [])}
        pe = {
            "sections": {
                ".data": {
                    "rva": 0x100, "virtual_size": 0x100,
                    "raw_size": 0x100,
                },
            },
        }
        with mock.patch(
                "homm2.build.data_manifest_adapter.CoffFile"), mock.patch(
                "homm2.build.data_manifest_adapter._candidate_bytes",
                return_value=b"same\0\0\0\0"), mock.patch(
                    "homm2.build.data_manifest_adapter.read_pe",
                    return_value=pe), mock.patch(
                    "homm2.build.data_manifest_adapter._pe_layout",
                    return_value=(
                        0x400000, (0x10,), lambda _site: 0x400100,
                        lambda _rva, size: b"same\0\0\0\0"[:size],
                    )):
            rows = automatic_string_rows(
                topology, Path("base"), Path("game.exe"), (),
                reserved=(claimed,))
        self.assertEqual(rows, [])

    def test_crt_pointer_sequence_requires_one_complete_reviewed_span(self):
        values = (0x401000, 0x402000)
        reviewed = {
            0x100: 0x401000,
            0x104: 0x402000,
            # A matching first pointer without a reviewed adjacent relocation
            # is not a second sequence.
            0x200: 0x401000,
        }
        self.assertEqual(
            _unique_reviewed_pointer_sequence_rva(values, reviewed, 4), 0x100)

        reviewed[0x204] = 0x402000
        self.assertIsNone(
            _unique_reviewed_pointer_sequence_rva(values, reviewed, 4))

    def test_crt_pointer_sequence_respects_candidate_alignment(self):
        self.assertEqual(
            _unique_reviewed_pointer_sequence_rva(
                (0x401000,), {0x104: 0x401000, 0x108: 0x402000}, 8),
            None)

    def test_crt_whole_sequence_resolves_unstable_function_counters(self):
        # Either 15-byte dispatcher is physically compatible with the first
        # cell, but only one complete ordered sequence exists in retail.
        values = ({0x401000, 0x402000}, {0x403000}, {0x404000})
        reviewed = {
            0x100: 0x402000,
            0x104: 0x403000,
            0x108: 0x404000,
            0x200: 0x401000,
            0x204: 0x405000,
            0x208: 0x404000,
        }
        self.assertEqual(
            _unique_reviewed_pointer_sequence_rva(values, reviewed, 4), 0x100)

    def test_delinker_manifest_retains_candidate_section_coordinates(self):
        row = {
            "object": "SOURCE\\Test.c",
            "rva": "0x1234",
            "size": "0x4",
            "storage": "rdata",
            "alignment": "0x4",
            "section_ordinal": "7",
            "section_offset": "0xc",
            "scope": "local",
        }
        fields = delinker_manifest_bytes([row]).decode().splitlines()[-1].split("\t")
        self.assertEqual(fields[5:7], ["7", "0xc"])

    def test_candidate_common_manifest_retains_per_object_size(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            base = root / "base"
            units = root / "units.toml"
            units.write_text('[[unit]]\nunit = "SOURCE/Test"\n')
            _coff(base / "SOURCE/Test.obj", [(".data", 4, DATA_FLAGS)], [
                ("?ordinary@@3HA", 0, 1, 0, 2),
                ("??_Bguard", 1, 0, 0, 2),
                ("undefined", 0, 0, 0, 2),
            ])
            rows = candidate_common_manifest_bytes(base, units).decode().splitlines()
        self.assertEqual(rows[-1], "SOURCE\\Test.c\t??_Bguard\t0x1")

    def test_compgen_recognizes_vc6_real_literal_symbols(self):
        self.assertEqual(
            _compgen_candidate_kind(candidate(
                "A", "__real@4@4012f424000000000000")),
            "FLOAT_LITERAL")
        self.assertEqual(
            _compgen_candidate_kind(candidate(
                "A", "__real@8@3ff8a3d70a3d70a3d800")),
            "FLOAT_LITERAL")
        self.assertIsNone(
            _compgen_candidate_kind(candidate("A", "__real@8@invalid")))

    def test_compgen_recognizes_plain_vc6_ordinal_bss_guard(self):
        guard = replace(
            candidate("A", "_$S18"), section_name=".bss", storage="bss"
        )
        self.assertEqual(_compgen_candidate_kind(guard), "STATIC_INIT_GUARD")
        self.assertIsNone(_compgen_candidate_kind(candidate("A", "_$S18")))

    def test_external_compgen_comdat_is_projected_into_every_exact_emitter(self):
        symbol = "__real@8@3ffec000000000000000"
        owner = CandidateDefinition(
            "A", symbol, 1, ".rdata", 0, 0, 8, 8, "rdata", "global",
            2, 0x40401040, 2, None)
        peer = replace(owner, unit="B", section_ordinal=3)
        owner_section = CandidateSection(
            "A", "A.c", 1, ".rdata", 8, 8, 0x40401040, "rdata",
            0x12345678, 2, None)
        peer_sections = [
            CandidateSection(
                "B", "B.c", ordinal, ".text", 0, 1, 0x60300020,
                None, 0, 0, None)
            for ordinal in (1, 2)
        ] + [replace(owner_section, unit="B", object_name="B.c", ordinal=3)]
        claim = CompgenDataClaim(
            "A", 0x100, "sharedScale", "FLOAT_LITERAL", 8,
            "A.cpp:1", "A.cpp:1", ())
        rows = _folded_compgen_rows(
            [(claim, owner)], {
                "A": ([owner], [owner_section]),
                "B": ([peer], peer_sections),
            })
        self.assertEqual([row["object"] for row in rows], ["A.c", "B.c"])
        self.assertEqual(len({row["name"] for row in rows}), 1)
        self.assertTrue(all(
            "candidate-coff-folded-comdat" in row["provenance"]
            for row in rows))
        validate_symbol_rows(rows, "fixture")

        different = replace(peer_sections[-1], checksum=0x87654321)
        rows = _folded_compgen_rows(
            [(claim, owner)], {
                "A": ([owner], [owner_section]),
                "B": ([peer], [*peer_sections[:-1], different]),
            })
        self.assertEqual([row["object"] for row in rows], ["A.c"])

        local = replace(owner, storage_class=3, scope="local",
                        comdat_selection=0)
        rows = _folded_compgen_rows(
            [(claim, local)], {"A": ([local], [owner_section])})
        self.assertEqual([row["object"] for row in rows], ["A.c"])
        self.assertNotIn("candidate-coff-folded-comdat", rows[0]["provenance"])
        self.assertEqual(rows[0]["scope"], "local")

    def test_compgen_interior_alias_keeps_the_owning_allocation(self):
        owner = CompgenDataClaim(
            "A", 0x100, "sourceFile", "STRING_LITERAL", 8,
            "A.cpp:1", "A.cpp:1", ())
        alias = CompgenDataClaim(
            "A", 0x107, "emptySuffix", "STRING_LITERAL", 1,
            "A.cpp:2", "A.cpp:2", ())
        self.assertEqual(_interior_compgen_aliases([alias, owner]), {
            alias: owner,
        })

    def test_compgen_binder_accepts_only_unique_or_ordered_bindings(self):
        assignments, failures = _bind_compgen_edges(
            {0: {0, 1}}, [0], [0, 1])
        self.assertEqual(assignments, {})
        self.assertEqual(failures, [("ambiguous", [0], [0, 1])])

        assignments, failures = _bind_compgen_edges(
            {0: {0, 1}, 1: {1}}, [0, 1], [0, 1])
        self.assertEqual(assignments, {0: 0, 1: 1})
        self.assertEqual(failures, [])

        assignments, failures = _bind_compgen_edges(
            {0: {0, 1}, 1: {0, 1}}, [1, 0], [1, 0])
        self.assertEqual(assignments, {1: 1, 0: 0})
        self.assertEqual(failures, [])

    def test_ambiguous_compgen_binding_warns_and_strict_fails(self):
        claim = CompgenDataClaim(
            "A", 0x100, "message", "STRING_LITERAL", 4,
            "A.cpp:1", "A.cpp:1", ())
        definitions = [
            replace(candidate("A", "$SG1", value=0), storage="rdata"),
            replace(candidate("A", "$SG2", value=4), storage="rdata"),
        ]
        topology = {"A": (definitions, [])}
        patches = (
            mock.patch("homm2.build.data_manifest_adapter.read_pe", return_value={}),
            mock.patch("homm2.build.data_manifest_adapter._pe_layout",
                       return_value=(0, set(), None,
                                     lambda _rva, size: b"same"[:size])),
            mock.patch("homm2.build.data_manifest_adapter._retail_storage_name",
                       return_value="rdata"),
            mock.patch("homm2.build.data_manifest_adapter._candidate_bytes",
                       return_value=b"same"),
            mock.patch("homm2.build.data_manifest_adapter.CoffFile"),
            mock.patch("homm2.build.data_manifest_adapter.derive_allocations",
                       return_value=([], mock.Mock(), [])),
        )
        with patches[0], patches[1], patches[2], patches[3], patches[4], patches[5]:
            with self.assertWarnsRegex(UserWarning, "ambiguous.*message"):
                resolved, diagnostics = resolve_compgen_definitions(
                    [claim], topology, Path("base"), Path("game.exe"))
            self.assertEqual(resolved, [])
            self.assertEqual(len(diagnostics), 1)
            with self.assertRaisesRegex(ValueError, "ambiguous.*message"):
                resolve_compgen_definitions(
                    [claim], topology, Path("base"), Path("game.exe"), strict=True)

    def test_zero_payload_in_raw_pe_data_can_bind_candidate_bss(self):
        claim = CompgenDataClaim(
            "A", 0x100, "empty", "STRING_LITERAL", 1,
            "A.cpp:1", "A.cpp:1", ())
        definition = replace(
            candidate("A", "$SG1"), section_name=".bss", storage="bss")
        topology = {"A": ([definition], [])}
        patches = (
            mock.patch("homm2.build.data_manifest_adapter.read_pe", return_value={}),
            mock.patch("homm2.build.data_manifest_adapter._pe_layout",
                       return_value=(0, set(), None,
                                     lambda _rva, size: b"\0" * size)),
            mock.patch("homm2.build.data_manifest_adapter._retail_storage_name",
                       return_value="data"),
            mock.patch("homm2.build.data_manifest_adapter._candidate_bytes",
                       return_value=b"\0"),
            mock.patch("homm2.build.data_manifest_adapter.CoffFile"),
            mock.patch("homm2.build.data_manifest_adapter.derive_allocations",
                       return_value=([], mock.Mock(), [])),
        )
        with patches[0], patches[1], patches[2], patches[3], patches[4], patches[5]:
            resolved, diagnostics = resolve_compgen_definitions(
                [claim], topology, Path("base"), Path("game.exe"))
        self.assertEqual(resolved, [(claim, definition)])
        self.assertEqual(diagnostics, [])

    def test_source_vtables_bind_exact_candidate_and_allow_proved_aliases(self):
        primary = AnnotatedVtable(
            "A", 0x100, "Derived", None, "??_7Derived@@6B@", "src/A.cpp:1")
        secondary = AnnotatedVtable(
            "A", 0x100, "Derived", "Base", "??_7Derived@@6BBase@@@",
            "src/A.cpp:2")
        definitions = [
            replace(candidate("A", primary.mangled_name, ordinal=2),
                    section_name=".rdata", storage="rdata"),
            replace(candidate("A", secondary.mangled_name, ordinal=2),
                    section_name=".rdata", storage="rdata"),
        ]
        resolved = resolve_vtable_definitions(
            [primary, secondary], {"A": (definitions, [])},
            {("A", 0x100): [primary.mangled_name]})
        rows = [_vtable_row(claim, definition)
                for claim, definition in resolved]
        _mark_vtable_aliases(rows)
        validate_symbol_rows(rows, "fixture")
        self.assertTrue(all("candidate-coff-alias" in row["provenance"]
                            for row in rows))

    def test_alias_marker_does_not_conflate_cross_object_folds(self):
        rows = [
            {
                "object": "A.c", "rva": "0x100",
                "provenance": "source-DATA_COMPGEN:test:fold-a",
            },
            {
                "object": "B.c", "rva": "0x100",
                "provenance": "source-DATA_COMPGEN:test:fold-b",
            },
        ]
        _mark_vtable_aliases(rows)
        self.assertTrue(all("candidate-coff-alias" not in row["provenance"]
                            for row in rows))

    def test_primary_vtable_requires_matching_inventory_row(self):
        claim = AnnotatedVtable(
            "A", 0x100, "Derived", None, "??_7Derived@@6B@", "src/A.cpp:1")
        definition = replace(candidate("A", claim.mangled_name),
                             section_name=".rdata", storage="rdata")
        with self.assertRaisesRegex(
                ValueError, "no matching source-vtable inventory row"):
            resolve_vtable_definitions([claim], {"A": ([definition], [])}, {})

    def test_source_scanner_handles_multiline_arrays_and_initializers(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "SOURCE").mkdir()
            (root / "SOURCE/Test.cpp").write_text(
                '#define DATA(x)\nstruct Owner { static int m_value; };\n'
                'int make_value(int, int);\n'
                '// DATA(0x00400000) int ignored;\n'
                'DATA(\n  0x00400100\n) static const char\n'
                'gNames[2][8] = {\n  "one", // ; DATA(0x00400004)\n  "two"\n};\n'
                'DATA(0x00400110) int Owner::m_value =\n'
                '    make_value(1, 2);\n'
            )
            rows = source_definitions(root)
        self.assertEqual(len(rows), 2)
        self.assertEqual(rows[0].qualified_name, "gNames")
        self.assertTrue(rows[0].is_static)
        self.assertEqual(rows[1].qualified_name, "Owner::m_value")
        self.assertEqual(rows[1].location, "SOURCE/Test.cpp:12")

    def test_resolves_external_local_and_class_static_identities(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "A.cpp").write_text(
                "#define DATA(x)\nstruct Owner { static int member; };\n"
                "DATA(0x00400100) int global;\n"
                "DATA(0x00400104) static int local;\n"
                "DATA(0x00400108) int Owner::member;\n"
                "void function() {\n"
                "  DATA(0x0040010c) static int blockLocal;\n"
                "}\n")
            definitions = source_definitions(root)
        topology = {"A": ([
            candidate("A", "?global@@3HA", value=0),
            candidate("A", "_local$S12", value=4),
            candidate("A", "?member@Owner@@2HA", value=8),
            candidate(
                "A", "_?blockLocal@?BB@??function@@YAXXZ@4HA", value=12),
        ], [])}
        resolved = resolve_source_definitions(definitions, topology, {})
        self.assertEqual([row.symbol for _source, row in resolved], [
            "?global@@3HA", "_local$S12", "?member@Owner@@2HA",
            "_?blockLocal@?BB@??function@@YAXXZ@4HA",
        ])

    def test_duplicate_raw_comdat_section_names_keep_distinct_ordinals(self):
        with TemporaryDirectory() as directory:
            path = Path(directory) / "A.obj"
            _coff(path, [
                (".data", 4, DATA_FLAGS, 0, 0),
                (".data", 5, COMDAT_DATA_FLAGS, 2, 0),
                (".data", 9, COMDAT_DATA_FLAGS, 2, 0),
            ], [
                ("?global@@3HA", 0, 1, 0, 2),
                ("??_C@first", 0, 2, 0, 2),
                ("??_C@second", 0, 3, 0, 2),
            ], section_definitions=True)
            definitions, sections = candidate_topology(path, "A")
        self.assertEqual([row.ordinal for row in sections if row.storage], [1, 2, 3])
        self.assertEqual([row.name for row in sections if row.storage],
                         [".data", ".data", ".data"])
        self.assertEqual([row.section_ordinal for row in definitions], [1, 2, 3])
        self.assertEqual([row.comdat_selection for row in definitions], [0, 2, 2])

    def test_symbol_validation_rejects_logical_duplicates_and_overlaps(self):
        base = {
            "name": "a", "object": "A.c", "rva": "0x100", "size": "0x4",
            "storage": "data", "alignment": "0x4", "section_ordinal": "1",
            "section_offset": "0x0", "scope": "external", "provenance": "test",
        }
        with self.assertRaisesRegex(ValueError, "duplicate identity"):
            validate_symbol_rows([base, dict(base)], "fixture")
        overlap = dict(base, name="b", rva="0x102")
        with self.assertRaisesRegex(ValueError, "overlapping"):
            validate_symbol_rows([base, overlap], "fixture")

    def test_explicit_same_section_candidate_alias_is_allowed(self):
        first = {
            "name": "a", "object": "A.c", "rva": "0x100", "size": "0x4",
            "storage": "rdata", "alignment": "0x4", "section_ordinal": "2",
            "section_offset": "0x0", "scope": "local",
            "provenance": "candidate-coff-alias:test",
        }
        second = dict(first, name="b")
        validate_symbol_rows([first, second], "fixture")

    def test_logical_size_does_not_include_candidate_padding(self):
        source = mock.Mock(
            size=5, rva=0x100, location="A.cpp:1", unit="A", name="uRedBeam",
            qualified_name="uRedBeam", is_static=True)
        padded = CandidateDefinition(
            "A", "_uRedBeam$S1", 1, ".data", 0, 0, 8, 4, "data", "local",
            3, DATA_FLAGS, 0, None)
        row = _symbol_row(source, padded, 0x108)
        self.assertEqual(row["size"], "0x5")
        self.assertEqual(row["scope"], "local")


if __name__ == "__main__":
    unittest.main()
