from __future__ import annotations

import csv
import json
import struct
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from homm2.build import coff_reloc_topology as topology
from homm2.build.annotated_data import AnnotatedDataDefinition


def synthetic_coff(path: Path, relocations: list[tuple[int, int, int, int]],
                   symbols: list[dict], section_name: str = ".data",
                   characteristics: int = topology.IMAGE_SCN_CNT_INITIALIZED_DATA) -> Path:
    raw = bytearray(16)
    for offset, _symbol, relocation_type, addend in relocations:
        width = topology.RELOCATION_WIDTHS[relocation_type]
        if width:
            raw[offset:offset + width] = int(addend).to_bytes(width, "little", signed=True)

    strings = bytearray(b"\0\0\0\0")
    string_offsets = {}

    def encoded_name(name: str) -> bytes:
        encoded = name.encode("ascii")
        if len(encoded) <= 8:
            return encoded.ljust(8, b"\0")
        if name not in string_offsets:
            string_offsets[name] = len(strings)
            strings.extend(encoded + b"\0")
        return struct.pack("<II", 0, string_offsets[name])

    symbol_rows = bytearray()
    for symbol in symbols:
        symbol_rows.extend(topology.SYMBOL.pack(
            encoded_name(symbol["name"]), symbol.get("value", 0),
            symbol.get("section", 0), symbol.get("type", 0),
            symbol.get("storage", 2), 0,
        ))
    struct.pack_into("<I", strings, 0, len(strings))

    raw_offset = topology.COFF_HEADER.size + topology.SECTION_HEADER.size
    relocation_offset = raw_offset + len(raw)
    symbol_offset = relocation_offset + len(relocations) * topology.RELOCATION.size
    header = topology.COFF_HEADER.pack(
        topology.IMAGE_FILE_MACHINE_I386, 1, 0, symbol_offset, len(symbols), 0, 0
    )
    section = topology.SECTION_HEADER.pack(
        section_name.encode("ascii").ljust(8, b"\0"), 0, 0, len(raw), raw_offset,
        relocation_offset, 0, len(relocations), 0, characteristics,
    )
    relocation_rows = b"".join(
        topology.RELOCATION.pack(offset, symbol, relocation_type)
        for offset, symbol, relocation_type, _addend in relocations
    )
    path.write_bytes(header + section + raw + relocation_rows + symbol_rows + strings)
    return path


def manifest_row(name: str, unit: str, rva: int, *, scope: str = "external",
                 provenance: str = "candidate-coff-section-translation") -> dict[str, str]:
    return {
        "name": name,
        "object": unit.replace("/", "\\") + ".c",
        "rva": f"0x{rva:x}",
        "size": "0x4",
        "storage": "data",
        "alignment": "0x4",
        "section_ordinal": "2",
        "section_offset": "0x0",
        "scope": scope,
        "provenance": provenance,
    }


def write_manifest(path: Path, rows: list[dict[str, str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=topology.SYMBOL_HEADER,
                                delimiter="\t", lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


class CoffRelocationTopologyTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)

    @staticmethod
    def mappings(symbols=None):
        return {"symbol_mappings": symbols or {}, "section_mappings": {}}

    def test_exact_data_relocation_census_matches(self):
        symbols = [{"name": "external", "section": 0}]
        relocations = [(0, 0, 0x0006, 4)]
        target = synthetic_coff(self.root / "target.obj", relocations, symbols)
        base = synthetic_coff(self.root / "base.obj", relocations, symbols)

        result = topology.compare_pair("fixture", base, target, self.mappings())

        self.assertEqual([], result["residuals"])
        self.assertEqual([], result["policy_errors"])
        self.assertEqual(1, result["summary"]["matched_relocations"])
        row = result["target"]["relocations"][0]
        self.assertEqual("undefined", row["symbol_status"])
        self.assertEqual(4, row["owner"]["offset"])
        self.assertEqual("initialized-data", row["section_class"])

    def test_accepts_symbol_free_coff_without_string_table(self):
        raw_offset = topology.COFF_HEADER.size + topology.SECTION_HEADER.size
        path = self.root / "empty.obj"
        path.write_bytes(
            topology.COFF_HEADER.pack(
                topology.IMAGE_FILE_MACHINE_I386, 1, 0, 0, 0, 0, 0
            )
            + topology.SECTION_HEADER.pack(
                b".data\0\0\0", 0, 0, 4, raw_offset, 0, 0, 0, 0,
                topology.IMAGE_SCN_CNT_INITIALIZED_DATA,
            )
            + b"\0" * 4
        )

        census = topology.CoffObject(path).census()

        self.assertEqual([], census["symbols"])
        self.assertEqual([], census["relocations"])

    def test_reports_property_and_multiplicity_residuals(self):
        symbols = [{"name": "external", "section": 0}]
        target = synthetic_coff(
            self.root / "target.obj",
            [(0, 0, 0x0006, 4), (8, 0, 0x0006, 0), (8, 0, 0x0006, 0)],
            symbols,
        )
        base = synthetic_coff(
            self.root / "base.obj",
            [(0, 0, 0x0006, 8), (8, 0, 0x0006, 0)],
            symbols,
        )

        result = topology.compare_pair("fixture", base, target, self.mappings())
        residuals = {row["offset"]: row for row in result["residuals"]}

        self.assertEqual("property-mismatch", residuals[0]["kind"])
        self.assertIn("addend", residuals[0]["differences"])
        self.assertEqual("multiplicity-mismatch", residuals[8]["kind"])

    def test_compares_identity_status_type_owner_addend_and_section_class(self):
        target_symbols = [
            {"name": "target", "section": 0},
            {"name": "state", "section": 0},
            {"name": "owner", "section": 1, "value": 0},
            {"name": "alias", "section": 1, "value": 4, "storage": 3},
        ]
        base_symbols = [
            {"name": "base", "section": 0},
            {"name": "state", "section": 1, "value": 0},
            {"name": "owner", "section": 1, "value": 0},
        ]
        target = synthetic_coff(
            self.root / "target.obj",
            [(0, 0, 0x0006, 0), (4, 1, 0x0006, 0),
             (8, 2, 0x0006, 0), (12, 3, 0x0006, 0)],
            target_symbols,
            characteristics=topology.IMAGE_SCN_CNT_UNINITIALIZED_DATA,
        )
        base = synthetic_coff(
            self.root / "base.obj",
            [(0, 0, 0x0006, 0), (4, 1, 0x0006, 0),
             (8, 2, 0x0014, 0), (12, 2, 0x0006, 8)],
            base_symbols,
        )

        result = topology.compare_pair("fixture", base, target, self.mappings())
        residuals = {row["offset"]: row for row in result["residuals"]}

        self.assertIn("symbol", residuals[0]["differences"])
        self.assertIn("symbol_status", residuals[4]["differences"])
        self.assertIn("target_section_class", residuals[4]["differences"])
        self.assertIn("type", residuals[8]["differences"])
        self.assertIn("owner_offset", residuals[12]["differences"])
        self.assertIn("addend", residuals[12]["differences"])
        self.assertTrue(all("section_class" in row["differences"]
                            for row in residuals.values()))

    def test_provisional_real_mapping_remains_visible(self):
        target_symbols = [
            {"name": "owner", "section": 1, "value": 0},
            {"name": "candidate", "section": 1, "value": 4, "storage": 3},
        ]
        base_symbols = [{"name": "owner", "section": 1, "value": 0}]
        target = synthetic_coff(
            self.root / "target.obj", [(0, 1, 0x0006, 0)], target_symbols
        )
        base = synthetic_coff(
            self.root / "base.obj", [(0, 0, 0x0006, 4)], base_symbols
        )
        mappings = self.mappings({
            "candidate": {
                "base_name": "owner", "status": "provisional",
                "addend_adjustment": 4, "provenance": "candidate-COFF",
            },
        })

        result = topology.compare_pair("fixture", base, target, mappings)

        self.assertEqual(1, len(result["residuals"]))
        self.assertEqual("candidate", result["residuals"][0][
            "provisional_mapping"]["target_name"])
        self.assertIn("symbol", result["residuals"][0]["differences"])

    def test_synthetic_target_identity_is_unconditional_error(self):
        for name in ("const_1234", "empty_stub"):
            with self.subTest(name=name):
                symbols = [{"name": name, "section": 0}]
                target = synthetic_coff(
                    self.root / f"target-{name}.obj", [(0, 0, 0x0006, 0)], symbols
                )
                base = synthetic_coff(
                    self.root / f"base-{name}.obj", [(0, 0, 0x0006, 0)], symbols
                )

                result = topology.compare_pair("fixture", base, target, self.mappings())

                self.assertEqual(
                    {"forbidden-synthetic-symbol", "forbidden-synthetic-identity"},
                    {row["kind"] for row in result["policy_errors"]},
                )

    def test_generated_manifest_is_exact_union_and_anchors_are_tu_aware(self):
        source = manifest_row(
            "?gValue@@3HA", "SOURCE/A", 0x1000,
            provenance="source-DATA:src/SOURCE/A.cpp:10",
        )
        supplemental = [
            manifest_row("private$S1", "SOURCE/A", 0x1100, scope="local"),
            manifest_row("private$S1", "SOURCE/B", 0x1200, scope="local"),
            manifest_row("?gUnique@@3HA", "SOURCE/B", 0x1300),
            manifest_row("??_C@shared", "SOURCE/A", 0x1400),
            manifest_row("??_C@shared", "SOURCE/B", 0x1500),
        ]
        write_manifest(self.root / "build/gen/delink_data_from_source.tsv", [source])
        write_manifest(self.root / "config/delink_data_supplemental.tsv", supplemental)
        write_manifest(self.root / "build/gen/delink_data_manifest.tsv",
                       [*supplemental, source])
        definitions = [AnnotatedDataDefinition(
            "SOURCE/A", "gValue", "gValue", 0x1000, 4,
            "src/SOURCE/A.cpp:10", False,
        )]

        with mock.patch.object(topology, "annotated_source_definitions",
                               return_value=definitions):
            provenance = topology.load_homm2_provenance(self.root)

        self.assertEqual([], provenance["diagnostics"])
        self.assertEqual(1, provenance["source_DATA_count"])
        self.assertEqual(1, provenance["source_manifest_count"])
        self.assertEqual(5, provenance["supplemental_count"])
        self.assertEqual(6, provenance["merged_count"])
        self.assertEqual(0x1100, topology._anchor_for(
            provenance, "SOURCE/A", "private$S1")["rva"])
        self.assertEqual(0x1200, topology._anchor_for(
            provenance, "SOURCE/B", "private$S1")["rva"])
        self.assertIsNone(topology._anchor_for(
            provenance, "SOURCE/C", "private$S1"))
        self.assertEqual(0x1300, topology._anchor_for(
            provenance, "SOURCE/C", "?gUnique@@3HA")["rva"])
        self.assertEqual(0x1400, topology._anchor_for(
            provenance, "SOURCE/A", "??_C@shared")["rva"])
        self.assertIsNone(topology._anchor_for(
            provenance, "SOURCE/C", "??_C@shared"))

        symbols = [{"name": "private$S1", "section": 0}]
        target = synthetic_coff(
            self.root / "tu-aware-target.obj", [(0, 0, 0x0006, 0)], symbols)
        base = synthetic_coff(
            self.root / "tu-aware-base.obj", [(0, 0, 0x0006, 0)], symbols)
        self.assertEqual([], topology.compare_pair(
            "SOURCE/A", base, target, self.mappings(), provenance=provenance
        )["policy_errors"])
        self.assertEqual("ambiguous-data-anchor", topology.compare_pair(
            "SOURCE/C", base, target, self.mappings(), provenance=provenance
        )["policy_errors"][0]["kind"])

    def test_generated_manifest_must_be_exact_conflict_free_union(self):
        source = manifest_row(
            "?gValue@@3HA", "SOURCE/A", 0x1000,
            provenance="source-DATA:src/SOURCE/A.cpp:10",
        )
        supplemental = manifest_row("private$S1", "SOURCE/A", 0x1100, scope="local")
        write_manifest(self.root / "build/gen/delink_data_from_source.tsv", [source])
        write_manifest(self.root / "config/delink_data_supplemental.tsv", [supplemental])
        write_manifest(self.root / "build/gen/delink_data_manifest.tsv", [source])
        definitions = [AnnotatedDataDefinition(
            "SOURCE/A", "gValue", "gValue", 0x1000, 4,
            "src/SOURCE/A.cpp:10", False,
        )]

        with mock.patch.object(topology, "annotated_source_definitions",
                               return_value=definitions):
            provenance = topology.load_homm2_provenance(self.root)

        diagnostic = next(row for row in provenance["diagnostics"]
                          if row["kind"] == "merged-manifest-not-exact-union")
        self.assertEqual(supplemental["name"], diagnostic["missing"][0][0])

    def test_generated_manifest_rejects_union_with_overlapping_allocations(self):
        source = manifest_row(
            "?gValue@@3HA", "SOURCE/A", 0x1000,
            provenance="source-DATA:src/SOURCE/A.cpp:10",
        )
        supplemental = manifest_row("private$S1", "SOURCE/A", 0x1002, scope="local")
        write_manifest(self.root / "build/gen/delink_data_from_source.tsv", [source])
        write_manifest(self.root / "config/delink_data_supplemental.tsv", [supplemental])
        write_manifest(self.root / "build/gen/delink_data_manifest.tsv",
                       [source, supplemental])
        definitions = [AnnotatedDataDefinition(
            "SOURCE/A", "gValue", "gValue", 0x1000, 4,
            "src/SOURCE/A.cpp:10", False,
        )]

        with mock.patch.object(topology, "annotated_source_definitions",
                               return_value=definitions):
            provenance = topology.load_homm2_provenance(self.root)

        self.assertIn("invalid-data-manifest",
                      {row["kind"] for row in provenance["diagnostics"]})

    def test_mapping_loader_rejects_synthetic_identity(self):
        path = self.root / "mappings.json"
        path.write_text(json.dumps({
            "schema": 1,
            "symbol_mappings": {"const_1234": "owner"},
        }))
        with self.assertRaisesRegex(topology.CoffError, "synthetic"):
            topology._load_mappings(path)


if __name__ == "__main__":
    unittest.main()
