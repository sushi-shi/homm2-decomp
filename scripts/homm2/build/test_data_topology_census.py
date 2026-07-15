import json
import struct
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from homm2.build.data_topology_census import build_census, main


DATA_FLAGS = 0xC0000040
BSS_FLAGS = 0xC0000080
RDATA_FLAGS = 0x40000040
COMDAT_DATA_FLAGS = 0xC0301040


def _coff(path, sections, symbols, section_definitions=False):
    """Write the small subset of i386 COFF needed by the census tests."""
    strings = bytearray(struct.pack("<I", 4))
    string_offsets = {}

    def encoded_name(name):
        raw = name.encode("latin-1")
        if len(raw) <= 8:
            return raw.ljust(8, b"\0")
        offset = string_offsets.get(raw)
        if offset is None:
            offset = len(strings)
            string_offsets[raw] = offset
            strings.extend(raw + b"\0")
        return struct.pack("<II", 0, offset)

    normalized_sections = []
    for section in sections:
        name, size, characteristics = section[:3]
        selection = section[3] if len(section) > 3 else 0
        parent = section[4] if len(section) > 4 else 0
        normalized_sections.append(
            (name, size, characteristics, selection, parent))

    section_table = bytearray()
    raw_offset = 20 + len(sections) * 40
    for name, size, characteristics, _selection, _parent in normalized_sections:
        section_table.extend(struct.pack(
            "<8sIIIIIIHHI", name.encode("ascii").ljust(8, b"\0"),
            0, 0, size, raw_offset, 0, 0, 0, 0, characteristics))
        raw_offset += size

    records = []
    if section_definitions:
        for index, (name, size, _characteristics, selection, parent) in enumerate(
                normalized_sections, 1):
            number = parent if selection == 5 else 0
            aux = struct.pack("<IHHIhBBH", size, 0, 0, 0, number, selection, 0, 0)
            records.append((name, 0, index, 0, 3, (aux,)))
    records.extend((*symbol, ()) for symbol in symbols)
    symbol_table = bytearray()
    for name, value, section, symbol_type, storage_class, aux_rows in records:
        symbol_table.extend(encoded_name(name))
        symbol_table.extend(struct.pack(
            "<IhHBB", value, section, symbol_type, storage_class, len(aux_rows)))
        for aux in aux_rows:
            symbol_table.extend(aux)
    struct.pack_into("<I", strings, 0, len(strings))
    header = struct.pack(
        "<HHIIIHH", 0x14C, len(sections), 0, raw_offset,
        sum(1 + len(record[5]) for record in records), 0, 0)
    payload = header + section_table
    payload += b"\0" * sum(section[1] for section in normalized_sections)
    payload += symbol_table + strings
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(payload)


class DataTopologyCensusTest(unittest.TestCase):
    def test_reports_multiplicity_and_topology_mismatch_classes(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            base = root / "base"
            target = root / "target"
            sections = [(".data", 32, DATA_FLAGS), (".bss", 32, BSS_FLAGS),
                        (".rdata", 32, RDATA_FLAGS)]
            _coff(base / "SOURCE/Test.obj", sections, [
                ("same", 0, 1, 0, 2),
                ("state", 0, 0, 0, 2),
                ("scope", 4, 1, 0, 3),
                ("klass", 8, 1, 0, 6),
                ("candidate_only", 0, 3, 0, 3),
                ("candidate_only", 4, 3, 0, 3),
                ("const_001", 8, 3, 0, 3),
            ])
            _coff(target / "SOURCE/Test.c.obj", sections, [
                ("same", 0, 1, 0, 2),
                ("state", 0, 2, 0, 2),
                ("scope", 4, 1, 0, 2),
                ("klass", 8, 1, 0, 2),
                ("target_only", 0, 0, 0, 2),
                ("target_only", 0, 0, 0, 2),
                ("const_001", 8, 3, 0, 2),
                ("string_001", 12, 3, 0, 2),
            ])

            payload = build_census(["SOURCE/Test"], base, target)
            row = payload["objects"][0]
            self.assertEqual(row["counts"]["name_common"], 5)
            self.assertEqual(row["counts"]["name_union"], 10)
            self.assertEqual(row["counts"]["topology_common"], 1)
            self.assertEqual(row["extra_names"], [
                {"name": "string_001", "count": 1},
                {"name": "target_only", "count": 2},
            ])
            self.assertEqual(row["missing_names"], [
                {"name": "candidate_only", "count": 2},
            ])
            self.assertEqual(row["mismatch_counts"]["definition_state"], 1)
            self.assertEqual(row["mismatch_counts"]["scope"], 3)
            self.assertEqual(row["mismatch_counts"]["storage_class"], 3)
            self.assertEqual(row["mismatch_counts"]["symbol_type"], 0)
            self.assertEqual(row["counts"]["local_global_mismatches"], 3)
            self.assertEqual(row["synthetic"]["target"]["string_"]["count"], 1)
            self.assertEqual(row["synthetic"]["base"]["const_"]["count"], 1)
            self.assertEqual(row["counts"]["proved_mappings"], 1)
            self.assertEqual(row["counts"]["provisional_mappings"], 3)
            self.assertEqual(row["counts"]["target_fallback_identities"], 2)

    def test_duplicate_comdat_data_sections_are_not_flattened(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            base = root / "base/Midi.obj"
            target = root / "target/Midi.c.obj"
            sections = [
                (".data", 8, DATA_FLAGS, 0, 0),
                (".data", 5, COMDAT_DATA_FLAGS, 2, 0),
                (".data", 9, COMDAT_DATA_FLAGS, 2, 0),
            ]
            symbols = [
                ("global", 0, 1, 0, 2),
                ("??_C@first", 0, 2, 0, 2),
                ("??_C@second", 0, 3, 0, 2),
            ]
            _coff(base, sections, symbols, section_definitions=True)
            _coff(target, [(".data", 22, DATA_FLAGS, 0, 0)], [
                ("global", 0, 1, 0, 2),
                ("??_C@first", 8, 1, 0, 2),
                ("??_C@second", 13, 1, 0, 2),
            ], section_definitions=True)

            flattened = build_census(["Midi"], root / "base", root / "target")
            row = flattened["objects"][0]
            self.assertTrue(row["symbol_topology_exact"])
            self.assertFalse(row["data_section_topology_exact"])
            self.assertFalse(row["exact"])
            self.assertEqual(row["counts"]["missing_data_sections"], 3)
            self.assertEqual(row["counts"]["extra_data_sections"], 1)
            comdat = row["data_section_topology"]["base"][1]
            self.assertEqual(comdat["ordinal"], 2)
            self.assertEqual(comdat["raw_name"], ".data")
            self.assertEqual(comdat["raw_name_hex"], "2e64617461000000")
            self.assertEqual(comdat["alignment"], 4)
            self.assertTrue(comdat["comdat"])
            self.assertEqual(comdat["comdat_selection_name"], "any")
            self.assertEqual(
                comdat["definition_symbols"][0]["aux"][0]["selection"], 2)

            _coff(target, sections, symbols, section_definitions=True)
            exact = build_census(["Midi"], root / "base", root / "target")
            self.assertTrue(exact["objects"][0]["symbol_topology_exact"])
            self.assertTrue(exact["objects"][0]["data_section_topology_exact"])
            self.assertTrue(exact["objects"][0]["exact"])

    def test_iterative_mode_succeeds_and_strict_mode_fails(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            units = root / "units.toml"
            units.write_text('[[unit]]\nunit = "SOURCE/Test"\n')
            _coff(root / "base/SOURCE/Test.obj", [(".data", 4, DATA_FLAGS)], [
                ("candidate_only", 0, 1, 0, 2),
            ])
            _coff(root / "target/SOURCE/Test.c.obj", [(".data", 4, DATA_FLAGS)], [
                ("target_only", 0, 1, 0, 2),
            ])
            output = root / "census.json"
            args = [
                "--units", str(units),
                "--base-root", str(root / "base"),
                "--target-root", str(root / "target"),
                "--source-root", str(root / "src"),
                "--supplemental", str(root / "missing.tsv"),
                "--symbols", str(root / "missing.csv"),
                "--output", str(output),
            ]
            self.assertEqual(main(args), 0)
            self.assertEqual(main(args + ["--strict"]), 1)
            payload = json.loads(output.read_text())
            self.assertEqual(payload["summary"]["exact_objects"], 0)
            self.assertEqual(payload["objects"][0]["missing_names"], [
                {"name": "candidate_only", "count": 1},
            ])
            self.assertEqual(payload["objects"][0]["extra_names"], [
                {"name": "target_only", "count": 1},
            ])

    def test_strict_allows_provisional_real_mapping_but_rejects_fallback(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            units = root / "units.toml"
            units.write_text('[[unit]]\nunit = "A"\n')
            sections = [(".data", 8, DATA_FLAGS)]
            _coff(root / "base/A.obj", sections, [
                ("shared", 0, 0, 0, 2),
            ])
            _coff(root / "target/A.c.obj", sections, [
                ("shared", 0, 1, 0, 2),
            ])
            args = [
                "--units", str(units),
                "--base-root", str(root / "base"),
                "--target-root", str(root / "target"),
                "--source-root", str(root / "src"),
                "--supplemental", str(root / "missing.tsv"),
                "--symbols", str(root / "missing.csv"),
                "--output", str(root / "census.json"),
                "--strict",
            ]
            self.assertEqual(main(args), 0)
            _coff(root / "target/A.c.obj", sections, [
                ("shared", 0, 1, 0, 2),
                ("empty_stub", 0, 0, 0x20, 2),
                ("[section-1]", 4, 1, 0, 2),
            ])
            self.assertEqual(main(args), 1)
            payload = json.loads((root / "census.json").read_text())
            self.assertEqual(payload["summary"]["target_fallback_identities"], 2)
            self.assertEqual(payload["summary"]["fallback_by_kind"],
                             {"empty_stub": 1, "section": 1})
            self.assertEqual(
                [row["kind"]
                 for row in payload["objects"][0]["fallback_identities"]["target"]],
                ["empty_stub", "section"])

    def test_separates_source_private_and_supplemental_provenance(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            sections = [(".data", 16, DATA_FLAGS)]
            symbols = [
                ("?global@@3HA", 0, 1, 0, 2),
                ("_local$S12", 4, 1, 0, 3),
                ("compiler_private", 8, 1, 0, 3),
            ]
            _coff(root / "base/A.obj", sections, symbols)
            _coff(root / "target/A.c.obj", sections, symbols)
            (root / "src").mkdir()
            (root / "src/A.cpp").write_text(
                "DATA(0x00400100) int global;\n"
                "DATA(0x00400104) static int\n"
                "local;\n")
            (root / "symbols.csv").write_text(
                "rva,name,unit,size,kind\n"
                "0x100,?global@@3HA,A,0x4,data\n")
            (root / "supplemental.tsv").write_text(
                "name\tobject\trva\tsize\tstorage\talignment\tsection_offset\t"
                "scope\tprovenance\n"
                "?global@@3HA\tA.c\t0x100\t0x4\tdata\t0x4\t0x0\texternal\ttest\n"
                "_local$S12\tA.c\t0x108\t0x4\trdata\t0x4\t0x4\tlocal\ttest\n"
                "compiler_private\tA.c\t0x200\t0x4\tdata\t0x4\t0x8\tlocal\ttest\n")

            payload = build_census(
                ["A"], root / "base", root / "target",
                source_root=root / "src",
                supplemental_path=root / "supplemental.tsv",
                symbols_path=root / "symbols.csv")
            provenance = payload["provenance"]
            self.assertEqual(provenance["summary"]["source_data_definitions"], 2)
            self.assertEqual(provenance["summary"]["candidate_data_covered"], 2)
            self.assertEqual(provenance["summary"]["candidate_compiler_private"], 1)
            self.assertEqual(
                provenance["summary"]["supplemental_by_candidate_class"],
                {"compiler-private": 1, "source-data": 2})
            self.assertEqual(
                provenance["summary"]["source_data_duplicates_in_supplemental"], 2)
            self.assertEqual(
                provenance["summary"]["source_data_supplemental_disagreements"], 1)
            self.assertEqual(
                provenance["supplemental_disagreements"][0]["differences"],
                ["rva", "storage"])

    def test_exact_and_missing_objects_are_counted_separately(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            symbols = [("shared", 0, 1, 0, 2)]
            sections = [(".rdata", 4, RDATA_FLAGS)]
            _coff(root / "base/A.obj", sections, symbols)
            _coff(root / "target/A.c.obj", sections, symbols)
            payload = build_census(["A", "B"], root / "base", root / "target")
            self.assertEqual(payload["summary"]["exact_objects"], 1)
            self.assertEqual(payload["summary"]["error_objects"], 1)
            self.assertEqual(payload["objects"][1]["status"], "missing-object")
            self.assertEqual(payload["objects"][1]["missing"], ["base", "target"])


if __name__ == "__main__":
    unittest.main()
