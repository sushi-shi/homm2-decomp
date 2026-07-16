import unittest
from dataclasses import replace
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build.data_manifest_adapter import (
    CandidateDefinition,
    CandidateSection,
    candidate_topology,
    resolve_source_definitions,
    source_definitions,
    validate_symbol_rows,
    _classified_contribution_rows,
    _normalize_symbol_row,
    _section_rows,
    _symbol_row,
    _validate_supplemental_row,
)
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
        storage, selection, None)


def contribution(unit, rva, size):
    return {
        "object": unit + ".c", "storage": "data", "rva": rva, "size": size,
        "segment": 3, "section": ".data", "provenance": "fixture-NB09",
    }


def anchor(unit, ordinal, rva, offset=0, name="anchor"):
    return {
        "object": unit + ".c", "name": name, "rva": hex(rva), "size": "0x4",
        "storage": "data", "alignment": "0x4", "section_ordinal": str(ordinal),
        "section_offset": hex(offset), "scope": "local", "provenance": "fixture",
    }


class DataManifestAdapterTest(unittest.TestCase):
    def test_stale_local_identity_migrates_by_section_local_position(self):
        topology = {"A": ([
            candidate("A", "$Tnew", ordinal=7, value=0),
        ], [])}
        row = anchor("A", 7, 0x100, name="$Told")
        row.update({
            "size": "0x4", "storage": "data", "alignment": "0x4",
            "scope": "local", "provenance": "reviewed",
        })
        normalized = _normalize_symbol_row(row, topology)
        self.assertEqual(normalized["name"], "$Tnew")
        self.assertEqual(normalized["section_ordinal"], "7")
        self.assertEqual(normalized["section_offset"], "0x0")

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
                "DATA(0x00400108) int Owner::member;\n")
            definitions = source_definitions(root)
        topology = {"A": ([
            candidate("A", "?global@@3HA", value=0),
            candidate("A", "_local$S12", value=4),
            candidate("A", "?member@Owner@@2HA", value=8),
        ], [])}
        resolved = resolve_source_definitions(definitions, topology, {})
        self.assertEqual([row.symbol for _source, row in resolved], [
            "?global@@3HA", "_local$S12", "?member@Owner@@2HA",
        ])

    def test_normal_assembly_rejects_implicit_supplemental_identity_migration(self):
        current = candidate("A", "_private$S2")
        topology = {"A": ([current], [])}
        row = {
            "name": "_private$S1", "object": "A.c", "rva": "0x100",
            "size": "0x4", "storage": "data", "alignment": "0x4",
            "section_ordinal": "1", "section_offset": "0x0",
            "scope": "local", "provenance": "candidate-coff-reloc-bijection",
        }
        with self.assertRaisesRegex(ValueError, "stale reviewed supplemental"):
            _validate_supplemental_row(row, topology)
        migrated = _normalize_symbol_row(row, topology)
        self.assertEqual(migrated["name"], "_private$S2")

    def test_normal_assembly_preserves_exact_reviewed_supplemental_row(self):
        current = candidate("A", "_private$S2")
        topology = {"A": ([current], [])}
        row = {
            "name": "_private$S2", "object": "A.c", "rva": "0x100",
            "size": "0x4", "storage": "data", "alignment": "0x4",
            "section_ordinal": "1", "section_offset": "0x0",
            "scope": "local", "provenance": "candidate-coff-reloc-bijection",
        }
        self.assertEqual(_validate_supplemental_row(row, topology), row)

    def test_normal_assembly_preserves_logical_size_below_candidate_span(self):
        current = replace(candidate("A", "_private$S2"), size=8)
        topology = {"A": ([current], [])}
        row = {
            "name": "_private$S2", "object": "A.c", "rva": "0x100",
            "size": "0x3", "storage": "data", "alignment": "0x4",
            "section_ordinal": "1", "section_offset": "0x0",
            "scope": "local", "provenance": "candidate-coff-reloc-bijection",
        }
        self.assertEqual(_validate_supplemental_row(row, topology), row)
        row["size"] = "0x9"
        with self.assertRaisesRegex(ValueError, "exceeds candidate span"):
            _validate_supplemental_row(row, topology)

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

    def test_multiple_sections_pack_in_candidate_order(self):
        topology = {"A": ([], [section("A", 2, 4, 4), section("A", 5, 8, 4)])}
        rows, diagnostics, _physical, _classifications = _section_rows(
            topology, [], {}, {}, [contribution("A", 0x100, 0x0c)])
        self.assertEqual([row["rva"] for row in rows], ["0x100", "0x104"])
        self.assertEqual(diagnostics, [])

    def test_alignment_gap_is_implicit_not_a_padding_symbol(self):
        topology = {"A": ([], [section("A", 1, 3), section("A", 2, 4, 4)])}
        rows, diagnostics, _physical, _classifications = _section_rows(
            topology, [], {}, {}, [contribution("A", 0x100, 8)])
        self.assertEqual([row["rva"] for row in rows], ["0x100", "0x104"])
        self.assertEqual(len(rows), 2)
        self.assertEqual(diagnostics, [])

    def test_breakpoint_drift_never_snaps_replay_cursor(self):
        topology = {"A": ([], [section("A", 1, 4), section("A", 2, 4)])}
        rows, diagnostics, _physical, _classifications = _section_rows(
            topology, [anchor("A", 1, 0x110)], {}, {},
            [contribution("A", 0x100, 8)])
        self.assertEqual([row["rva"] for row in rows], ["0x100", "0x104"])
        self.assertEqual([row["cause"] for row in diagnostics], ["breakpoint-drift"])
        self.assertEqual(diagnostics[0]["predicted_base"], 0x100)

    def test_unique_reviewed_base_overrides_replay_and_need_not_be_pe_aligned(self):
        definition = candidate("A", "reviewed", ordinal=1, value=0)
        topology = {"A": ([definition], [section("A", 1, 8, 8)])}
        rows, diagnostics, _physical, classifications = _section_rows(
            topology, [anchor("A", 1, 0x102, name="reviewed")], {}, {},
            [contribution("A", 0x102, 8)])
        self.assertEqual(rows[0]["rva"], "0x102")
        self.assertEqual(rows[0]["provenance"],
                         "reviewed-definition-affine-section+validated-anchor")
        self.assertEqual(diagnostics, [])
        self.assertEqual(classifications, [])

    def test_fully_reviewed_nonaffine_section_is_an_exact_classification(self):
        definitions = [
            candidate("A", "first", ordinal=1, value=0),
            candidate("A", "second", ordinal=1, value=4),
        ]
        topology = {"A": (definitions, [section("A", 1, 8, 4)])}
        reviewed = [
            anchor("A", 1, 0x100, name="first"),
            anchor("A", 1, 0x120, offset=4, name="second"),
        ]
        rows, diagnostics, physical, classifications = _section_rows(
            topology, reviewed, {}, {}, [contribution("A", 0x100, 0x30)])
        self.assertEqual(rows[0]["rva"], "-")
        self.assertEqual(rows[0]["storage"], "data")
        self.assertEqual(rows[0]["provenance"],
                         "reviewed-definition-nonaffine-section")
        self.assertEqual(diagnostics, [])
        self.assertEqual(classifications[0]["anchor_bases"], [0x100, 0x11c])
        classified = _classified_contribution_rows(physical, rows)
        self.assertEqual(classified[0]["storage"], "data")
        self.assertEqual(classified[0]["size"], 0x30)

    def test_partial_nonaffine_section_remains_a_blocking_diagnostic(self):
        definitions = [
            candidate("A", "first", ordinal=1, value=0),
            candidate("A", "missing", ordinal=1, value=4),
        ]
        topology = {"A": (definitions, [section("A", 1, 8, 4)])}
        rows, diagnostics, _physical, classifications = _section_rows(
            topology, [anchor("A", 1, 0x120, name="first")], {}, {},
            [contribution("A", 0x100, 8)])
        self.assertEqual(rows[0]["rva"], "0x100")
        self.assertEqual([row["cause"] for row in diagnostics],
                         ["breakpoint-drift"])
        self.assertEqual(classifications, [])

    def test_multiple_contribution_chunks_restart_cursor(self):
        topology = {"A": ([], [section("A", 1, 4), section("A", 2, 8, 8)])}
        rows, diagnostics, _physical, _classifications = _section_rows(
            topology, [], {}, {}, [
                contribution("A", 0x100, 4), contribution("A", 0x200, 8),
            ])
        self.assertEqual([row["rva"] for row in rows], ["0x100", "0x200"])
        self.assertEqual(diagnostics, [])

    def test_adjacent_nb09_chunks_remain_distinct(self):
        topology = {"A": ([], [section("A", 1, 4), section("A", 2, 4)])}
        rows, diagnostics, physical, _classifications = _section_rows(
            topology, [], {}, {}, [
                contribution("A", 0x100, 4), contribution("A", 0x104, 4),
            ])
        self.assertEqual(len(physical), 2)
        self.assertEqual([row["rva"] for row in rows], ["0x100", "0x104"])
        self.assertEqual(diagnostics, [])

    def test_linker_sorted_subsection_uses_isolated_owner_contribution(self):
        topology = {"A": ([], [
            section("A", 1, 8, 4),
            section("A", 2, 4, 4, name=".CRT$XCU"),
        ])}
        rows, diagnostics, _physical, _classifications = _section_rows(
            topology, [], {}, {}, [
                contribution("A", 0x100, 4),
                contribution("A", 0x200, 8),
            ])
        by_ordinal = {int(row["ordinal"]): row for row in rows}
        self.assertEqual(by_ordinal[1]["rva"], "0x200")
        self.assertEqual(by_ordinal[2]["rva"], "0x100")
        self.assertEqual(by_ordinal[2]["provenance"],
                         "retail-linker-subsection-contribution")
        self.assertEqual(diagnostics, [])

    def test_ambiguous_linker_sorted_subsection_remains_unassigned(self):
        topology = {"A": ([], [
            section("A", 1, 8, 4),
            section("A", 2, 4, 4, name=".CRT$XCU"),
        ])}
        rows, diagnostics, _physical, _classifications = _section_rows(
            topology, [], {}, {}, [
                contribution("A", 0x100, 4),
                contribution("A", 0x104, 4),
                contribution("A", 0x200, 8),
            ])
        by_ordinal = {int(row["ordinal"]): row for row in rows}
        self.assertEqual(by_ordinal[1]["rva"], "0x200")
        self.assertEqual(by_ordinal[2]["rva"], "-")
        self.assertIn("ambiguous-linker-subsection-contribution",
                      {row["cause"] for row in diagnostics})

    def test_comdat_without_contribution_requires_explicit_anchor(self):
        topology = {"A": ([], [
            section("A", 1, 4, selection=2),
            section("A", 2, 4, selection=2),
        ])}
        rows, diagnostics, physical, _classifications = _section_rows(
            topology, [anchor("A", 1, 0x180)], {}, {}, [])
        self.assertEqual(rows[0]["rva"], "0x180")
        self.assertEqual(rows[0]["provenance"], "explicit-comdat-anchor+validated-anchor")
        self.assertEqual(rows[1]["rva"], "-")
        self.assertEqual(diagnostics[0]["cause"],
                         "unanchored-comdat-without-contribution")
        classified = _classified_contribution_rows(physical, rows)
        self.assertEqual(classified, [])

    def test_unique_global_comdat_identity_is_an_explicit_anchor(self):
        definition = replace(candidate("A", "literal", ordinal=1),
                             comdat_selection=2)
        topology = {"A": ([definition], [section("A", 1, 4, selection=2)])}
        rows, diagnostics, _physical, _classifications = _section_rows(
            topology, [], {}, {"literal": (0x180,)}, [])
        self.assertEqual(rows[0]["rva"], "0x180")
        self.assertEqual(diagnostics, [])

    def test_duplicate_global_comdat_identity_remains_ambiguous(self):
        definition = replace(candidate("A", "literal", ordinal=1),
                             comdat_selection=2)
        topology = {"A": ([definition], [section("A", 1, 4, selection=2)])}
        rows, diagnostics, _physical, _classifications = _section_rows(
            topology, [], {}, {"literal": (0x180, 0x1a0)}, [])
        self.assertEqual(rows[0]["rva"], "-")
        self.assertEqual(diagnostics[0]["cause"],
                         "ambiguous-comdat-identity-without-contribution")

    def test_global_comdat_identity_classifies_chunk_without_steering_cursor(self):
        definition = replace(candidate("A", "literal", ordinal=1),
                             storage="bss", comdat_selection=2)
        topology = {"A": ([definition], [
            section("A", 1, 4, storage="bss", selection=2),
            section("A", 2, 4, storage="data"),
        ])}
        rows, diagnostics, physical, _classifications = _section_rows(
            topology, [], {}, {"literal": (0x104,)},
            [contribution("A", 0x100, 8)])
        by_ordinal = {int(row["ordinal"]): row for row in rows}
        self.assertEqual(physical[0]["candidate_storage"], "bss")
        self.assertEqual(by_ordinal[1]["rva"], "0x100")
        self.assertEqual(by_ordinal[1]["provenance"],
                         "retail-contribution-replay")
        self.assertNotIn(1, {row["section_ordinal"] for row in diagnostics})

    def test_candidate_bss_classifies_physical_writable_contribution(self):
        topology = {"A": ([], [section("A", 1, 8, 4, storage="bss")])}
        rows, diagnostics, physical, _classifications = _section_rows(
            topology, [], {}, {}, [contribution("A", 0x122f20, 8)])
        classified = _classified_contribution_rows(physical, rows)
        self.assertEqual(diagnostics, [])
        self.assertEqual(classified[0]["storage"], "bss")
        self.assertEqual(classified[0]["rva"], 0x122f20)

    def test_raw_boundary_fragments_rejoin_before_candidate_classification(self):
        topology = {"A": ([], [section("A", 1, 8, 4, storage="bss")])}
        first = contribution("A", 0x122f20, 4)
        second = contribution("A", 0x122f24, 4)
        second["storage"] = "bss"
        rows, diagnostics, physical, _classifications = _section_rows(
            topology, [], {}, {}, [first, second])
        self.assertEqual(len(physical), 1)
        classified = _classified_contribution_rows(physical, rows)
        self.assertEqual(diagnostics, [])
        self.assertEqual(classified[0]["storage"], "bss")
        self.assertEqual(classified[0]["size"], 8)


if __name__ == "__main__":
    unittest.main()
