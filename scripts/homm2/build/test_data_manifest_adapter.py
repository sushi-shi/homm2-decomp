import unittest
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest import mock

from homm2.build.data_manifest_adapter import (
    CandidateDefinition,
    CandidateSection,
    breakpoint_report,
    candidate_topology,
    resolve_source_definitions,
    source_definitions,
    validate_symbol_rows,
    _symbol_row,
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


class DataManifestAdapterTest(unittest.TestCase):
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

    def test_breakpoint_replay_never_snaps_cursor_to_observed_anchor(self):
        sections = [
            CandidateSection("A", "A.c", 1, ".data", 4, 4, DATA_FLAGS,
                             "data", 0, None),
            CandidateSection("B", "B.c", 1, ".data", 4, 4, DATA_FLAGS,
                             "data", 0, None),
        ]
        topology = {"A": ([], [sections[0]]), "B": ([], [sections[1]])}
        section_rows = [
            {"object": "A.c", "ordinal": "1", "rva": "0x110"},
            {"object": "B.c", "ordinal": "1", "rva": "0x114"},
        ]
        order = [{"unit": "A"}, {"unit": "B"}]
        pe = {"sections": {
            ".rdata": {"rva": 0x80, "virtual_size": 0},
            ".data": {"rva": 0x100, "raw_size": 0x20, "virtual_size": 0x40},
        }}
        with (mock.patch("homm2.build.data_manifest_adapter.load_retail_order",
                         return_value=order),
              mock.patch("homm2.build.data_manifest_adapter.read_pe", return_value=pe)):
            report = breakpoint_report(topology, section_rows, [], "u", "s", "e")
        intervals = report["streams"]["data"]["intervals"]
        self.assertEqual(intervals[0]["predicted_rva"], 0x100)
        self.assertEqual(intervals[0]["drift"], 0x10)
        self.assertEqual(intervals[1]["predicted_rva"], 0x104)
        self.assertEqual(intervals[1]["drift"], 0x10)


if __name__ == "__main__":
    unittest.main()
