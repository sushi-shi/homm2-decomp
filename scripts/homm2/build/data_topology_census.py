"""Compare candidate and delinked-target COFF data-symbol topology.

The delinker can duplicate definitions, turn private compiler symbols into
externals, or synthesize names for addresses without a recovered owner.  This
census intentionally compares COFF records as multisets: matching a name once
does not hide additional copies or a different definition/scope/storage class.
"""

import argparse
import csv
import json
import os
import re
import struct
import tomllib
from collections import Counter
from dataclasses import asdict, dataclass
from pathlib import Path

from homm2.build.canonicalize_relocs import CoffFile
from homm2.build.annotated_data import source_definitions as annotated_source_definitions


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
SYNTHETIC_PREFIXES = ("const_", "string_", "data_", "bss_")
SECTION_FALLBACK = re.compile(r"^\[section-[0-9]+\]$")
MISMATCH_ATTRIBUTES = (
    ("definition_state", "state"),
    ("storage", "storage"),
    ("section", "section"),
    ("scope", "scope"),
    ("storage_class", "storage_class"),
    ("symbol_type", "symbol_type"),
)
IMAGE_SCN_CNT_INITIALIZED_DATA = 0x00000040
IMAGE_SCN_CNT_UNINITIALIZED_DATA = 0x00000080
IMAGE_SCN_LNK_INFO = 0x00000200
IMAGE_SCN_LNK_REMOVE = 0x00000800
IMAGE_SCN_LNK_COMDAT = 0x00001000
IMAGE_SCN_MEM_DISCARDABLE = 0x02000000
IMAGE_SCN_MEM_WRITE = 0x80000000

COMDAT_SELECTION_NAMES = {
    0: "none",
    1: "no-duplicates",
    2: "any",
    3: "same-size",
    4: "exact-match",
    5: "associative",
    6: "largest",
    7: "newest",
}

STORAGE_CLASS_NAMES = {
    0: "null",
    1: "automatic",
    2: "external",
    3: "static",
    4: "register",
    5: "external-definition",
    6: "label",
    7: "undefined-label",
    8: "member-of-struct",
    9: "argument",
    10: "struct-tag",
    11: "member-of-union",
    12: "union-tag",
    13: "typedef",
    14: "undefined-static",
    15: "enum-tag",
    16: "member-of-enum",
    17: "register-parameter",
    18: "bit-field",
    100: "block",
    101: "function",
    102: "end-of-struct",
    103: "file",
    104: "section",
    105: "weak-external",
    107: "clr-token",
}


@dataclass(frozen=True, order=True)
class DataSymbol:
    name: str
    state: str
    storage: str
    section: str
    scope: str
    storage_class: int
    storage_class_name: str
    symbol_type: int

    def signature(self):
        row = asdict(self)
        del row["name"]
        return tuple(sorted(row.items()))


@dataclass(frozen=True, order=True)
class SourceDefinition:
    unit: str
    name: str
    rva: int
    location: str


@dataclass(frozen=True)
class SectionDefinitionAux:
    raw_hex: str
    length: int
    relocation_count: int
    line_number_count: int
    checksum: int
    number: int
    selection: int
    selection_name: str
    associative_parent: int | None
    reserved: int
    high_number: int


@dataclass(frozen=True)
class SectionDefinitionSymbol:
    name: str
    value: int
    section: int
    symbol_type: int
    storage_class: int
    storage_class_name: str
    aux_count: int
    aux: tuple


@dataclass(frozen=True)
class DataSection:
    ordinal: int
    raw_name: str
    raw_name_hex: str
    size: int
    characteristics: int
    alignment: int
    comdat: bool
    comdat_selection: int
    comdat_selection_name: str
    associative_parent: int | None
    relocation_count: int
    definition_symbols: tuple


def _section_characteristics(coff, section_index):
    optional_size = struct.unpack_from("<H", coff.data, 16)[0]
    offset = 20 + optional_size + (section_index - 1) * 40
    return struct.unpack_from("<I", coff.data, offset + 36)[0]


def _section_name_hex(coff, section_index):
    optional_size = struct.unpack_from("<H", coff.data, 16)[0]
    offset = 20 + optional_size + (section_index - 1) * 40
    return bytes(coff.data[offset:offset + 8]).hex()


def _section_alignment(characteristics):
    encoded = (characteristics >> 20) & 0xF
    if encoded == 0:
        return 1
    if not 1 <= encoded <= 14:
        raise ValueError("unsupported COFF section alignment encoding %d" % encoded)
    return 1 << (encoded - 1)


def _is_data_section(name, characteristics):
    if name.startswith(".debug"):
        return False
    if characteristics & (IMAGE_SCN_LNK_INFO | IMAGE_SCN_LNK_REMOVE |
                          IMAGE_SCN_MEM_DISCARDABLE):
        return False
    return bool(characteristics & (IMAGE_SCN_CNT_INITIALIZED_DATA |
                                   IMAGE_SCN_CNT_UNINITIALIZED_DATA))


def _storage(name, characteristics):
    if (name.startswith(".bss") or
            characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA):
        return "bss"
    if name.startswith(".rdata"):
        return "rdata"
    if name.startswith(".data"):
        return "data"
    return "data" if characteristics & IMAGE_SCN_MEM_WRITE else "rdata"


def _scope(storage_class):
    if storage_class in (2, 105):
        return "global"
    if storage_class in (3, 6, 7, 14):
        return "local"
    return "other"


def data_symbols(path):
    """Return meaningful defined, common, and undefined data COFF symbols."""
    coff = CoffFile(path)
    sections = {}
    for section in coff.sections:
        characteristics = _section_characteristics(coff, section.index)
        if _is_data_section(section.name, characteristics):
            sections[section.index] = (section.name, characteristics)

    rows = []
    for symbol in coff.symbols.values():
        symbol_type, storage_class = struct.unpack_from(
            "<HB", coff.data, symbol.offset + 14)
        storage_class_name = STORAGE_CLASS_NAMES.get(
            storage_class, "class-%d" % storage_class)
        if symbol.section in sections:
            section_name, characteristics = sections[symbol.section]
            if (storage_class == 104 or
                    (symbol.aux_count and symbol.name == section_name)):
                continue
            rows.append(DataSymbol(
                symbol.name, "defined", _storage(section_name, characteristics),
                section_name, _scope(storage_class), storage_class,
                storage_class_name, symbol_type))
            continue

        # COFF type 0 externals are the only undefined records that can be
        # identified as data without guessing from a function relocation.
        if symbol.section == 0 and symbol_type == 0 and storage_class in (2, 105):
            state = "common" if symbol.value else "undefined"
            rows.append(DataSymbol(
                symbol.name, state, state, "", _scope(storage_class),
                storage_class, storage_class_name, symbol_type))
    return rows


def _section_definition_symbols(coff, section):
    rows = []
    for symbol in coff.symbols.values():
        symbol_type, storage_class = struct.unpack_from(
            "<HB", coff.data, symbol.offset + 14)
        if not (symbol.section == section.index and symbol.value == 0 and
                symbol_type == 0 and storage_class == 3 and symbol.aux_count):
            continue
        aux_rows = []
        for index in range(symbol.aux_count):
            offset = symbol.offset + 18 * (index + 1)
            raw = bytes(coff.data[offset:offset + 18])
            if len(raw) != 18:
                raise ValueError("short section-definition auxiliary record in %s" %
                                 coff.path)
            length, relocations, lines, checksum, number, selection, reserved, high = (
                struct.unpack("<IHHIhBBH", raw))
            associative_parent = None
            if selection == 5:
                associative_parent = (high << 16) | (number & 0xFFFF)
            aux_rows.append(SectionDefinitionAux(
                raw.hex(), length, relocations, lines, checksum, number,
                selection, COMDAT_SELECTION_NAMES.get(
                    selection, "selection-%d" % selection),
                associative_parent, reserved, high))
        if (not aux_rows or aux_rows[0].length != section.raw_size or
                aux_rows[0].relocation_count != section.reloc_count):
            continue
        rows.append(SectionDefinitionSymbol(
            symbol.name, symbol.value, symbol.section, symbol_type, storage_class,
            STORAGE_CLASS_NAMES[storage_class], symbol.aux_count, tuple(aux_rows)))
    return tuple(rows)


def data_sections(path):
    """Return raw COFF data-section topology, retaining duplicate section names."""
    coff = CoffFile(path)
    rows = []
    for section in coff.sections:
        characteristics = _section_characteristics(coff, section.index)
        if not _is_data_section(section.name, characteristics):
            continue
        definitions = _section_definition_symbols(coff, section)
        selection = definitions[0].aux[0].selection if definitions else 0
        associative_parent = (
            definitions[0].aux[0].associative_parent if definitions else None)
        rows.append(DataSection(
            section.index, section.name, _section_name_hex(coff, section.index),
            section.raw_size, characteristics,
            _section_alignment(characteristics),
            bool(characteristics & IMAGE_SCN_LNK_COMDAT), selection,
            COMDAT_SELECTION_NAMES.get(selection, "selection-%d" % selection),
            associative_parent, section.reloc_count, definitions))
    return rows


def _fallback_kind(name):
    for prefix in SYNTHETIC_PREFIXES:
        if name.startswith(prefix):
            return prefix.removesuffix("_")
    if "empty_stub" in name:
        return "empty_stub"
    if SECTION_FALLBACK.fullmatch(name):
        return "section"
    return None


def fallback_identities(path):
    """Inventory all fallback names, including non-data ``empty_stub`` symbols."""
    coff = CoffFile(path)
    counts = Counter()
    for symbol in coff.symbols.values():
        kind = _fallback_kind(symbol.name)
        if kind is not None:
            counts[(kind, symbol.name)] += 1
    return [{"kind": kind, "name": name, "count": count}
            for (kind, name), count in sorted(counts.items())]


def source_definitions(source_root):
    return [SourceDefinition(row.unit, row.name, row.rva, row.location)
            for row in annotated_source_definitions(Path(source_root), REPO)]


def _public_source_names(path):
    names = {}
    if path is None or not path.is_file():
        return names
    with path.open(encoding="latin-1", newline="") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") != "data":
                continue
            match = (re.match(r"\?([A-Za-z_]\w*)@@", row["name"]) or
                     re.match(r"[_@]?([A-Za-z_]\w*)", row["name"]))
            if match:
                names[row["name"]] = match.group(1)
    return names


def _source_spelling(symbol_name, public_names):
    if symbol_name in public_names:
        return public_names[symbol_name]
    match = re.match(r"_?([A-Za-z_]\w*?)\$S[0-9]+$", symbol_name)
    if match:
        return match.group(1)
    match = re.match(r"\?([A-Za-z_]\w*)@@", symbol_name)
    if match:
        return match.group(1)
    return symbol_name.removeprefix("_")


def _supplemental_rows(path):
    if path is None or not path.is_file():
        return []
    with path.open(encoding="utf-8", newline="") as stream:
        reader = csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter="\t")
        rows = []
        for row in reader:
            normalized = dict(row)
            normalized["unit"] = row["object"].replace("\\", "/").removesuffix(".c")
            normalized["rva"] = int(row["rva"], 0)
            normalized["size"] = int(row["size"], 0)
            rows.append(normalized)
        return rows


def provenance_census(units, base_root, source_root, supplemental_path,
                      symbols_path, base_suffix=".obj"):
    """Separate canonical DATA definitions from derived and supplemental rows."""
    public_names = _public_source_names(symbols_path)
    definitions = source_definitions(source_root) if source_root is not None else []
    definitions_by_unit_name = {
        (row.unit, row.name): row for row in definitions
    }
    definitions_by_rva = {}
    definitions_by_name = {}
    for row in definitions:
        definitions_by_rva.setdefault(row.rva, []).append(row)
        definitions_by_name.setdefault(row.name, []).append(row)

    object_rows = {}
    candidate_class = {}
    for unit in units:
        path = _path(base_root, unit, base_suffix)
        if not path.is_file():
            continue
        covered = Counter()
        private = Counter()
        storage_by_name = {}
        try:
            symbols = data_symbols(path)
        except (OSError, ValueError, struct.error):
            continue
        for symbol in symbols:
            if symbol.state != "defined":
                continue
            source_name = _source_spelling(symbol.name, public_names)
            definition = definitions_by_unit_name.get((unit, source_name))
            if definition is not None:
                covered[(symbol.name, source_name)] += 1
                candidate_class[(unit, symbol.name)] = "source-data"
            else:
                private[symbol.name] += 1
                candidate_class[(unit, symbol.name)] = "compiler-private"
            storage_by_name.setdefault(symbol.name, set()).add(symbol.storage)
        source_rows = [row for row in definitions if row.unit == unit]
        object_rows[unit] = {
            "source_data": [asdict(row) for row in source_rows],
            "candidate_data_covered": [
                {"symbol": symbol, "source_name": source_name, "count": count}
                for (symbol, source_name), count in sorted(covered.items())
            ],
            "candidate_compiler_private": _name_rows(private),
            "candidate_storage": storage_by_name,
            "supplemental_linker_metadata": [],
        }

    supplemental = _supplemental_rows(supplemental_path)
    duplicate_rows = []
    disagreement_rows = []
    supplemental_counts = Counter()
    supplemental_classes = Counter()
    for row in supplemental:
        unit = row["unit"]
        source_name = _source_spelling(row["name"], public_names)
        named_definition = definitions_by_unit_name.get((unit, source_name))
        if named_definition is None:
            same_name = definitions_by_name.get(source_name, [])
            if len(same_name) == 1:
                named_definition = same_name[0]
        rva_definitions = definitions_by_rva.get(row["rva"], [])
        duplicate_definitions = list(rva_definitions)
        if named_definition is not None and named_definition not in duplicate_definitions:
            duplicate_definitions.append(named_definition)
        candidate_provenance = candidate_class.get((unit, row["name"]), "unclassified")
        supplemental_counts[row.get("provenance") or "unknown"] += 1
        supplemental_classes[candidate_provenance] += 1
        if unit in object_rows:
            object_rows[unit]["supplemental_linker_metadata"].append({
                "name": row["name"],
                "rva": row["rva"],
                "size": row["size"],
                "storage": row["storage"],
                "scope": row.get("scope") or "",
                "provenance": row.get("provenance") or "",
                "candidate_class": candidate_provenance,
            })
        if duplicate_definitions:
            duplicate_rows.append({
                "supplemental_name": row["name"],
                "supplemental_unit": unit,
                "supplemental_rva": row["rva"],
                "supplemental_storage": row["storage"],
                "provenance": row.get("provenance") or "",
                "source_definitions": [asdict(value)
                                       for value in sorted(duplicate_definitions)],
            })
        if named_definition is not None:
            differences = []
            if unit != named_definition.unit:
                differences.append("unit")
            if row["rva"] != named_definition.rva:
                differences.append("rva")
            expected_storage = object_rows.get(named_definition.unit, {}).get(
                "candidate_storage", {}).get(row["name"], set())
            if expected_storage and row["storage"] not in expected_storage:
                differences.append("storage")
            if differences:
                disagreement_rows.append({
                    "supplemental_name": row["name"],
                    "source_name": named_definition.name,
                    "supplemental_unit": unit,
                    "source_unit": named_definition.unit,
                    "supplemental_rva": row["rva"],
                    "source_rva": named_definition.rva,
                    "supplemental_storage": row["storage"],
                    "candidate_storage": sorted(expected_storage),
                    "differences": differences,
                    "source_location": named_definition.location,
                })

    # Sets are retained above for comparisons only; JSON gets stable lists.
    for row in object_rows.values():
        row["candidate_storage"] = {
            name: sorted(values) for name, values in sorted(row["candidate_storage"].items())
        }
    source_duplicate_rvas = [
        {"rva": rva, "definitions": [asdict(row) for row in sorted(rows)]}
        for rva, rows in sorted(definitions_by_rva.items()) if len(rows) > 1
    ]
    summary = {
        "source_data_definitions": len(definitions),
        "candidate_data_covered": sum(
            item["count"] for row in object_rows.values()
            for item in row["candidate_data_covered"]),
        "candidate_compiler_private": sum(
            item["count"] for row in object_rows.values()
            for item in row["candidate_compiler_private"]),
        "supplemental_rows": len(supplemental),
        "supplemental_by_provenance": dict(sorted(supplemental_counts.items())),
        "supplemental_by_candidate_class": dict(sorted(supplemental_classes.items())),
        "source_data_duplicates_in_supplemental": len(duplicate_rows),
        "source_data_supplemental_disagreements": len(disagreement_rows),
        "duplicate_source_rvas": len(source_duplicate_rvas),
    }
    return {
        "summary": summary,
        "objects": object_rows,
        "supplemental_duplicates": duplicate_rows,
        "supplemental_disagreements": disagreement_rows,
        "duplicate_source_rvas": source_duplicate_rvas,
    }


def _name_rows(counter):
    return [{"name": name, "count": count}
            for name, count in sorted(counter.items()) if count]


def _signature_dict(signature, count):
    row = dict(signature)
    row["count"] = count
    return row


def _signature_rows(symbols):
    counts = Counter(symbol.signature() for symbol in symbols)
    return [_signature_dict(signature, count)
            for signature, count in sorted(counts.items())]


def _data_section_rows(counter):
    rows = []
    for section, count in sorted(
            counter.items(), key=lambda item: (
                item[0].ordinal, item[0].raw_name, item[0].size,
                item[0].characteristics)):
        row = asdict(section)
        row["count"] = count
        rows.append(row)
    return rows


def _attribute_distance(base, target, attribute):
    base_counts = Counter(getattr(symbol, attribute) for symbol in base)
    target_counts = Counter(getattr(symbol, attribute) for symbol in target)
    common = min(len(base), len(target))
    return common - sum((base_counts & target_counts).values())


def _mismatch_rows(base_symbols, target_symbols):
    base_by_name = {}
    target_by_name = {}
    for symbol in base_symbols:
        base_by_name.setdefault(symbol.name, []).append(symbol)
    for symbol in target_symbols:
        target_by_name.setdefault(symbol.name, []).append(symbol)

    rows = []
    totals = Counter()
    for name in sorted(set(base_by_name) & set(target_by_name)):
        base = base_by_name[name]
        target = target_by_name[name]
        if Counter(symbol.signature() for symbol in base) == Counter(
                symbol.signature() for symbol in target):
            continue
        category_counts = {}
        for category, attribute in MISMATCH_ATTRIBUTES:
            count = _attribute_distance(base, target, attribute)
            if count:
                category_counts[category] = count
                totals[category] += count
        rows.append({
            "name": name,
            "common_multiplicity": min(len(base), len(target)),
            "categories": sorted(category_counts),
            "category_counts": category_counts,
            "base": _signature_rows(base),
            "target": _signature_rows(target),
        })
    return rows, {category: totals[category]
                  for category, _attribute in MISMATCH_ATTRIBUTES}


def _synthetic_inventory(symbols):
    result = {}
    for prefix in SYNTHETIC_PREFIXES:
        names = Counter(symbol.name for symbol in symbols
                        if symbol.name.startswith(prefix))
        result[prefix] = {
            "count": sum(names.values()),
            "names": _name_rows(names),
        }
    return result


def compare_object(unit, base_path, target_path):
    base_symbols = data_symbols(base_path)
    target_symbols = data_symbols(target_path)
    base_sections = data_sections(base_path)
    target_sections = data_sections(target_path)
    base_topology = Counter(base_symbols)
    target_topology = Counter(target_symbols)
    base_section_topology = Counter(base_sections)
    target_section_topology = Counter(target_sections)
    base_names = Counter(symbol.name for symbol in base_symbols)
    target_names = Counter(symbol.name for symbol in target_symbols)
    mismatch_rows, mismatch_counts = _mismatch_rows(base_symbols, target_symbols)
    missing = base_names - target_names
    extra = target_names - base_names
    topology_common = sum((base_topology & target_topology).values())
    topology_union = sum((base_topology | target_topology).values())
    section_common = sum((base_section_topology & target_section_topology).values())
    section_union = sum((base_section_topology | target_section_topology).values())
    missing_sections = base_section_topology - target_section_topology
    extra_sections = target_section_topology - base_section_topology
    name_common = sum((base_names & target_names).values())
    name_union = sum((base_names | target_names).values())
    fallback = {
        "base": fallback_identities(base_path),
        "target": fallback_identities(target_path),
    }
    target_fallback_names = {row["name"] for row in fallback["target"]}
    real_base_names = Counter({
        name: count for name, count in base_names.items()
        if _fallback_kind(name) is None
    })
    real_target_names = Counter({
        name: count for name, count in target_names.items()
        if name not in target_fallback_names
    })
    real_base_topology = Counter({
        symbol: count for symbol, count in base_topology.items()
        if _fallback_kind(symbol.name) is None
    })
    real_target_topology = Counter({
        symbol: count for symbol, count in target_topology.items()
        if symbol.name not in target_fallback_names
    })
    proved_mappings = sum((real_base_topology & real_target_topology).values())
    common_real_names = sum((real_base_names & real_target_names).values())
    provisional_mappings = common_real_names - proved_mappings
    unmapped_target = real_target_names - real_base_names
    unmapped_base = real_base_names - real_target_names
    fallback_count = sum(row["count"] for row in fallback["target"])
    object_hard_errors = (fallback_count + sum(unmapped_target.values()) +
                          sum(unmapped_base.values()) +
                          sum(missing_sections.values()) +
                          sum(extra_sections.values()))
    symbol_exact = base_topology == target_topology
    section_exact = base_section_topology == target_section_topology
    exact = symbol_exact and section_exact
    return {
        "unit": unit,
        "status": "exact" if exact else "mismatch",
        "exact": exact,
        "symbol_topology_exact": symbol_exact,
        "data_section_topology_exact": section_exact,
        "base_path": str(base_path),
        "target_path": str(target_path),
        "counts": {
            "base_symbols": len(base_symbols),
            "target_symbols": len(target_symbols),
            "exact_topology": topology_common,
            "topology_common": topology_common,
            "topology_union": topology_union,
            "name_common": name_common,
            "name_union": name_union,
            "missing_names": sum(missing.values()),
            "extra_names": sum(extra.values()),
            "mismatched_names": len(mismatch_rows),
            "base_data_sections": len(base_sections),
            "target_data_sections": len(target_sections),
            "data_section_common": section_common,
            "data_section_union": section_union,
            "missing_data_sections": sum(missing_sections.values()),
            "extra_data_sections": sum(extra_sections.values()),
            "proved_mappings": proved_mappings,
            "provisional_mappings": provisional_mappings,
            "unmapped_target_identities": sum(unmapped_target.values()),
            "unmapped_base_identities": sum(unmapped_base.values()),
            "target_fallback_identities": fallback_count,
            "defined_vs_undefined_mismatches": mismatch_counts["definition_state"],
            "local_global_mismatches": mismatch_counts["scope"],
            "storage_class_mismatches": mismatch_counts["storage_class"],
        },
        "missing_names": _name_rows(missing),
        "extra_names": _name_rows(extra),
        "topology_mismatches": mismatch_rows,
        "mismatch_counts": dict(sorted(mismatch_counts.items())),
        "data_section_topology": {
            "exact": section_exact,
            "common": section_common,
            "union": section_union,
            "base": _data_section_rows(base_section_topology),
            "target": _data_section_rows(target_section_topology),
            "missing": _data_section_rows(missing_sections),
            "extra": _data_section_rows(extra_sections),
        },
        "mappings": {
            "proved": proved_mappings,
            "provisional": provisional_mappings,
            "unmapped_target": _name_rows(unmapped_target),
            "unmapped_base": _name_rows(unmapped_base),
        },
        "fallback_identities": fallback,
        "canonical_status": "error" if object_hard_errors else "clean",
        "canonical_hard_errors": object_hard_errors,
        "synthetic": {
            "base": _synthetic_inventory(base_symbols),
            "target": _synthetic_inventory(target_symbols),
        },
    }


def _load_units(path):
    data = tomllib.loads(path.read_text())
    units = [row["unit"] for row in data.get("unit", [])]
    if not units:
        raise ValueError("unit manifest has no [[unit]] entries: %s" % path)
    duplicates = [name for name, count in Counter(units).items() if count > 1]
    if duplicates:
        raise ValueError("duplicate units: %s" % ", ".join(sorted(duplicates)))
    return units


def _path(root, unit, suffix):
    return root / (unit + suffix)


def build_census(units, base_root, target_root, base_suffix=".obj",
                 target_suffix=".c.obj", source_root=None,
                 supplemental_path=None, symbols_path=None):
    objects = []
    aggregate_counts = Counter()
    aggregate_mismatches = Counter()
    aggregate_fallback = Counter()
    aggregate_synthetic = {
        side: {prefix: Counter() for prefix in SYNTHETIC_PREFIXES}
        for side in ("base", "target")
    }
    exact_objects = 0
    symbol_exact_objects = 0
    section_exact_objects = 0
    error_objects = 0
    for unit in units:
        base_path = _path(base_root, unit, base_suffix)
        target_path = _path(target_root, unit, target_suffix)
        missing = [side for side, path in (("base", base_path), ("target", target_path))
                   if not path.is_file()]
        if missing:
            error_objects += 1
            objects.append({
                "unit": unit,
                "status": "missing-object",
                "exact": False,
                "base_path": str(base_path),
                "target_path": str(target_path),
                "missing": missing,
            })
            continue
        try:
            row = compare_object(unit, base_path, target_path)
        except (OSError, ValueError, struct.error) as error:
            error_objects += 1
            objects.append({
                "unit": unit,
                "status": "invalid-object",
                "exact": False,
                "base_path": str(base_path),
                "target_path": str(target_path),
                "error": str(error),
            })
            continue
        objects.append(row)
        exact_objects += int(row["exact"])
        symbol_exact_objects += int(row["symbol_topology_exact"])
        section_exact_objects += int(row["data_section_topology_exact"])
        aggregate_counts.update(row["counts"])
        aggregate_mismatches.update(row["mismatch_counts"])
        for fallback in row["fallback_identities"]["target"]:
            aggregate_fallback[fallback["kind"]] += fallback["count"]
        for side in ("base", "target"):
            for prefix in SYNTHETIC_PREFIXES:
                for name in row["synthetic"][side][prefix]["names"]:
                    aggregate_synthetic[side][prefix][name["name"]] += name["count"]

    synthetic = {}
    for side in ("base", "target"):
        synthetic[side] = {}
        for prefix in SYNTHETIC_PREFIXES:
            names = aggregate_synthetic[side][prefix]
            synthetic[side][prefix] = {
                "count": sum(names.values()),
                "names": _name_rows(names),
            }
    summary = {
        "objects": len(units),
        "compared_objects": len(units) - error_objects,
        "exact_objects": exact_objects,
        "symbol_exact_objects": symbol_exact_objects,
        "data_section_exact_objects": section_exact_objects,
        "mismatched_objects": len(units) - error_objects - exact_objects,
        "error_objects": error_objects,
        **dict(sorted(aggregate_counts.items())),
        "mismatch_counts": dict(sorted(aggregate_mismatches.items())),
        "fallback_by_kind": dict(sorted(aggregate_fallback.items())),
        "synthetic": synthetic,
    }
    canonical_hard_errors = (
        summary["error_objects"] +
        summary.get("target_fallback_identities", 0) +
        summary.get("unmapped_target_identities", 0) +
        summary.get("unmapped_base_identities", 0) +
        summary.get("missing_data_sections", 0) +
        summary.get("extra_data_sections", 0))
    payload = {
        "schema": 1,
        "comparison": "COFF data-symbol topology multisets",
        "summary": summary,
        "objects": objects,
    }
    if source_root is not None:
        provenance = provenance_census(
            units, base_root, source_root, supplemental_path, symbols_path,
            base_suffix)
        payload["provenance"] = provenance
        summary["provenance"] = provenance["summary"]
        canonical_hard_errors += (
            provenance["summary"]["source_data_duplicates_in_supplemental"] +
            provenance["summary"]["source_data_supplemental_disagreements"] +
            provenance["summary"]["duplicate_source_rvas"])
    summary["canonical_hard_errors"] = canonical_hard_errors
    return payload


def _resolve(path):
    return path if path.is_absolute() else REPO / path


def _print_summary(payload, output):
    summary = payload["summary"]
    print("data topology: %d/%d objects exact; %d invalid or missing" % (
        summary["exact_objects"], summary["objects"], summary["error_objects"]))
    print("symbol/section exact objects: %d/%d; data sections common/union: %d/%d" % (
        summary["symbol_exact_objects"], summary["data_section_exact_objects"],
        summary.get("data_section_common", 0),
        summary.get("data_section_union", 0)))
    print("topology common/union: %d/%d; names common/union: %d/%d" % (
        summary.get("topology_common", 0), summary.get("topology_union", 0),
        summary.get("name_common", 0), summary.get("name_union", 0)))
    print("missing/extra name records: %d/%d; diagnostics: %s" % (
        summary.get("missing_names", 0), summary.get("extra_names", 0), output))
    print("proved/provisional real mappings: %d/%d; canonical hard errors: %d" % (
        summary.get("proved_mappings", 0), summary.get("provisional_mappings", 0),
        summary["canonical_hard_errors"]))
    provenance = summary.get("provenance")
    if provenance is not None:
        print(
            "provenance DATA/private/supplemental: %d/%d/%d; "
            "duplicates/disagreements: %d/%d" % (
                provenance["candidate_data_covered"],
                provenance["candidate_compiler_private"],
                provenance["supplemental_rows"],
                provenance["source_data_duplicates_in_supplemental"],
                provenance["source_data_supplemental_disagreements"]))


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="compare candidate and delinked-target COFF data topology")
    parser.add_argument("--units", type=Path, default=Path("config/units.toml"))
    parser.add_argument("--base-root", type=Path,
                        default=Path("build/objdiff/base"))
    parser.add_argument("--target-root", type=Path, default=Path("build/delink"))
    parser.add_argument("--source-root", type=Path, default=Path("src"))
    parser.add_argument("--supplemental", type=Path,
                        default=Path("config/delink_data_supplemental.tsv"))
    parser.add_argument("--symbols", type=Path,
                        default=Path("build/gen/symbol_names.csv"))
    parser.add_argument("--base-suffix", default=".obj")
    parser.add_argument("--target-suffix", default=".c.obj")
    parser.add_argument("--output", type=Path,
                        default=Path("build/gen/data_topology_census.json"))
    parser.add_argument("--strict", action="store_true",
                        help=("fail on missing objects, fallback identities, raw data-section "
                              "drift, or unmapped real identities; provisional symbol "
                              "mappings are allowed"))
    args = parser.parse_args(argv)

    try:
        units_path = _resolve(args.units)
        payload = build_census(
            _load_units(units_path), _resolve(args.base_root),
            _resolve(args.target_root), args.base_suffix, args.target_suffix,
            _resolve(args.source_root), _resolve(args.supplemental),
            _resolve(args.symbols))
    except (OSError, ValueError, tomllib.TOMLDecodeError) as error:
        parser.error(str(error))

    output = _resolve(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")
    _print_summary(payload, output)
    return 1 if args.strict and payload["summary"]["canonical_hard_errors"] else 0


if __name__ == "__main__":
    raise SystemExit(main())
