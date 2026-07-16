"""Build HoMM2 data manifests from source annotations and candidate COFF topology.

``DATA()`` is the address authority for reconstructed storage.  Candidate objects
provide the exact decorated identity, section ordinal/value, extent, alignment, and
storage class.  The only versioned supplement contains compiler/linker allocations
which have no source annotation; combined Vostok inputs are generated under
``build/gen``.
"""

from __future__ import annotations

import argparse
import csv
import json
import os
import re
import struct
import tempfile
import tomllib
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

from homm2.build.annotated_data import (
    AnnotatedDataDefinition as SourceDefinition,
    source_definitions as annotated_source_definitions,
)
from homm2.build.canonicalize_relocs import CoffFile
from homm2.build.data_topology_census import (
    _is_data_section,
    _scope,
    _section_alignment,
    _section_characteristics,
    _section_definition_symbols,
    _storage,
)
from homm2.build.contribution_manifest import contribution_rows
from homm2.build.link_exe import read_pe


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
SOURCE_ROOT = REPO / "src"
BASE_ROOT = REPO / "build/objdiff/base"
SYMBOLS = REPO / "build/gen/symbol_names.csv"
UNITS = REPO / "config/units.toml"
EXE = REPO / "build/orig/HEROES2W.EXE"
SUPPLEMENTAL = REPO / "config/delink_data_supplemental.tsv"
SOURCE_MANIFEST = REPO / "build/gen/delink_data_from_source.tsv"
COMBINED_MANIFEST = REPO / "build/gen/delink_data_manifest.tsv"
SECTION_MANIFEST = REPO / "build/gen/delink_data_sections.tsv"
BREAKPOINTS = REPO / "build/gen/delink_data_breakpoints.json"
CONTRIBUTION_MANIFEST = REPO / "build/gen/delink_contributions.tsv"

SYMBOL_HEADER = (
    "name", "object", "rva", "size", "storage", "alignment",
    "section_ordinal", "section_offset", "scope", "provenance",
)
SECTION_HEADER = (
    "object", "ordinal", "name", "rva", "size", "alignment",
    "characteristics", "comdat_selection", "associative_ordinal",
    "storage", "provenance",
)
CONTRIBUTION_HEADER = (
    "object", "storage", "rva", "size", "segment", "section", "provenance",
)
LOCAL_SUFFIX = re.compile(r"^_?(.+?)\$S[0-9]+$")


@dataclass(frozen=True)
class CandidateDefinition:
    unit: str
    symbol: str
    section_ordinal: int
    section_name: str
    section_value: int
    stream_offset: int
    size: int
    alignment: int
    storage: str
    scope: str
    storage_class: int
    characteristics: int
    comdat_selection: int
    associative_ordinal: int | None


@dataclass(frozen=True)
class CandidateSection:
    unit: str
    object_name: str
    ordinal: int
    name: str
    size: int
    alignment: int
    characteristics: int
    storage: str | None
    comdat_selection: int
    associative_ordinal: int | None


def source_definitions(source_root: Path = SOURCE_ROOT) -> list[SourceDefinition]:
    return annotated_source_definitions(Path(source_root), REPO)


def _decoded_symbol_names(symbol: str) -> set[str]:
    names = {symbol, symbol.removeprefix("_")}
    local = LOCAL_SUFFIX.match(symbol)
    if local:
        names.add(local.group(1).removeprefix("_"))
    if symbol.startswith("?") and "@@" in symbol:
        spelling = symbol[1:symbol.index("@@")]
        pieces = spelling.split("@")
        simple = pieces[0]
        names.add(simple)
        if len(pieces) > 1:
            names.add("::".join([*reversed(pieces[1:]), simple]))
    return names


def _coff_symbol_fields(coff: CoffFile, symbol) -> tuple[int, int]:
    return struct.unpack_from("<HB", coff.data, symbol.offset + 14)


def candidate_topology(path: Path, unit: str) -> tuple[list[CandidateDefinition], list[CandidateSection]]:
    coff = CoffFile(path)
    sections = []
    section_bases = {}
    stream_cursors = defaultdict(int)
    for section in coff.sections:
        characteristics = _section_characteristics(coff, section.index)
        alignment = _section_alignment(characteristics)
        definitions = _section_definition_symbols(coff, section)
        selection = definitions[0].aux[0].selection if definitions else 0
        associative = definitions[0].aux[0].associative_parent if definitions else None
        storage = _storage(section.name, characteristics) if _is_data_section(
            section.name, characteristics) else None
        if storage is not None:
            cursor = _align_up(stream_cursors[storage], alignment)
            section_bases[section.index] = cursor
            stream_cursors[storage] = cursor + section.raw_size
        sections.append(CandidateSection(
            unit, unit.replace("/", "\\") + ".c", section.index, section.name,
            section.raw_size, alignment, characteristics, storage, selection, associative,
        ))

    rows = []
    for section in coff.sections:
        if section.index not in section_bases:
            continue
        metadata = sections[section.index - 1]
        symbols = []
        for symbol in coff.symbols.values():
            if symbol.section != section.index or symbol.aux_count:
                continue
            typ, storage_class = _coff_symbol_fields(coff, symbol)
            if storage_class not in (2, 3) or typ != 0:
                continue
            symbols.append((symbol, storage_class))
        symbols.sort(key=lambda item: (item[0].value, item[0].name))
        offsets = sorted({symbol.value for symbol, _storage_class in symbols})
        next_offsets = {value: (offsets[index + 1] if index + 1 < len(offsets)
                                else section.raw_size)
                        for index, value in enumerate(offsets)}
        for symbol, storage_class in symbols:
            end = next_offsets[symbol.value]
            if not 0 <= symbol.value < end <= section.raw_size:
                raise ValueError(f"invalid extent for {unit}:{symbol.name}")
            rows.append(CandidateDefinition(
                unit, symbol.name, section.index, section.name, symbol.value,
                section_bases[section.index] + symbol.value, end - symbol.value,
                (metadata.alignment if symbol.value == 0 else
                 min(metadata.alignment, symbol.value & -symbol.value)),
                metadata.storage or "", _scope(storage_class), storage_class,
                metadata.characteristics, metadata.comdat_selection,
                metadata.associative_ordinal,
            ))
    return rows, sections


def _align_up(value: int, alignment: int) -> int:
    return (value + alignment - 1) & -alignment


def _load_public_data(path: Path = SYMBOLS) -> tuple[
        dict[tuple[str, int], list[str]],
        dict[tuple[str, str], int],
        dict[str, tuple[int, ...]]]:
    by_rva = defaultdict(list)
    by_symbol = {}
    global_by_symbol = defaultdict(set)
    with Path(path).open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") != "data":
                continue
            rva = int(row["rva"], 0)
            by_rva[(row["unit"], rva)].append(row["name"])
            by_symbol[(row["unit"], row["name"])] = rva
            global_by_symbol[row["name"]].add(rva)
    # The shipping NB09 has S_PUB32 identities but no compiland ownership.
    # Keep its global name index separate from recovered per-unit associations.
    global_rvas = {
        name: tuple(sorted(rvas)) for name, rvas in global_by_symbol.items()
    }
    return by_rva, by_symbol, global_rvas


def resolve_source_definitions(definitions, topology_by_unit, public_by_rva) -> list[tuple[SourceDefinition, CandidateDefinition]]:
    resolved = []
    for source in definitions:
        candidates = topology_by_unit.get(source.unit, ([], []))[0]
        public_names = set(public_by_rva.get((source.unit, source.rva), ()))
        exact_public = [row for row in candidates if row.symbol in public_names]
        if len(exact_public) == 1:
            matches = exact_public
        else:
            matches = [row for row in candidates
                       if source.qualified_name in _decoded_symbol_names(row.symbol)]
            if not matches and source.qualified_name == source.name:
                matches = [row for row in candidates
                           if source.name in _decoded_symbol_names(row.symbol)]
        if len(matches) != 1:
            identities = [f"{row.symbol}@{row.section_ordinal}:{row.section_value:#x}"
                          for row in matches]
            raise ValueError(
                f"{source.location}: expected one candidate for {source.qualified_name}, "
                f"found {len(matches)}: {identities}")
        resolved.append((source, matches[0]))
    return resolved


def _symbol_row(source: SourceDefinition, candidate: CandidateDefinition,
                _next_retail_rva: int | None) -> dict[str, str]:
    size = source.size
    if size <= 0 or size > candidate.size:
        raise ValueError(
            f"logical sizeof 0x{size:x} exceeds candidate span 0x{candidate.size:x} "
            f"at {source.location}")
    return {
        "name": candidate.symbol,
        "object": candidate.unit.replace("/", "\\") + ".c",
        "rva": f"0x{source.rva:x}",
        # Clang supplies logical sizeof. Candidate and section extents retain
        # padding separately; padding is never emitted as a symbol.
        "size": f"0x{size:x}",
        "storage": candidate.storage,
        "alignment": f"0x{candidate.alignment:x}",
        "section_ordinal": str(candidate.section_ordinal),
        "section_offset": f"0x{candidate.section_value:x}",
        "scope": "external" if candidate.storage_class == 2 else "local",
        "provenance": f"source-DATA:{source.location}",
    }


def _read_tsv(path: Path) -> list[dict[str, str]]:
    if not Path(path).is_file():
        return []
    with Path(path).open(newline="", encoding="utf-8") as stream:
        return list(csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter="\t"))


def _write_tsv(path: Path, header, rows, comment: str | None = None) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile("w", newline="", encoding="utf-8",
                                     prefix=f".{path.name}.", dir=path.parent,
                                     delete=False) as stream:
        temporary = Path(stream.name)
        if comment:
            stream.write(f"# {comment}\n")
        writer = csv.DictWriter(stream, fieldnames=header, delimiter="\t",
                                lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
    os.replace(temporary, path)


def _normalize_symbol_row(row: dict[str, str], topology_by_unit) -> dict[str, str]:
    unit = row["object"].replace("\\", "/").removesuffix(".c")
    definitions = topology_by_unit.get(unit, ([], []))[0]
    matches = [value for value in definitions if value.symbol == row["name"]]
    if matches and row.get("section_ordinal"):
        matches = [value for value in matches
                   if value.section_ordinal == int(row["section_ordinal"], 0)]
    if matches and row.get("section_offset") not in (None, "", "-"):
        offset = int(row["section_offset"], 0)
        matches = [value for value in matches
                   if value.stream_offset == offset or value.section_value == offset]
    if not matches and row.get("section_offset") not in (None, "", "-"):
        # MSVC's compiler-local $T number is TU-state-sensitive.  The reviewed
        # stream offset/storage/scope identifies the current exact COFF symbol
        # without carrying the stale generated spelling forward.
        offset = int(row["section_offset"], 0)
        expected_scope = "global" if row.get("scope") == "external" else "local"
        matches = [value for value in definitions
                   if value.storage == row.get("storage")
                   and value.scope == expected_scope
                   and value.stream_offset == offset]
    if len(matches) != 1:
        raise ValueError(f"supplemental {unit}:{row['name']} has {len(matches)} candidate definitions")
    candidate = matches[0]
    normalized = dict(row)
    normalized.update({
        "name": candidate.symbol,
        "object": unit.replace("/", "\\") + ".c",
        "storage": candidate.storage,
        "alignment": f"0x{candidate.alignment:x}",
        "section_ordinal": str(candidate.section_ordinal),
        "section_offset": f"0x{candidate.section_value:x}",
        "scope": "external" if candidate.storage_class == 2 else "local",
    })
    return {key: normalized.get(key, "") for key in SYMBOL_HEADER}


def migrate_supplemental(legacy: Path, output: Path, source_rows, topology_by_unit) -> list[dict[str, str]]:
    source_identity = {(row["object"], row["name"]) for row in source_rows}
    source_rvas = {int(row["rva"], 0) for row in source_rows}
    rows = []
    for legacy_row in _read_tsv(legacy):
        identity = (legacy_row["object"], legacy_row["name"])
        if identity in source_identity or int(legacy_row["rva"], 0) in source_rvas:
            continue
        rows.append(_normalize_symbol_row(legacy_row, topology_by_unit))
    validate_symbol_rows(rows, "supplemental")
    _write_tsv(output, SYMBOL_HEADER, _sorted_symbol_rows(rows),
               "Compiler/linker allocations without a source DATA() definition.")
    return rows


def _sorted_symbol_rows(rows):
    return sorted(rows, key=lambda row: (
        row["object"], int(row["section_ordinal"], 0),
        int(row["section_offset"], 0), row["name"]))


def validate_symbol_rows(rows, label: str) -> None:
    identities = {}
    rvas = {}
    intervals = []
    for row in rows:
        missing = [key for key in SYMBOL_HEADER if key not in row or row[key] == ""]
        if missing:
            raise ValueError(f"{label} row missing {missing}: {row}")
        identity = (row["object"], row["name"])
        if identity in identities:
            raise ValueError(f"{label} duplicate identity {identity}")
        identities[identity] = row
        rva = int(row["rva"], 0)
        size = int(row["size"], 0)
        alignment = int(row["alignment"], 0)
        if size <= 0 or alignment <= 0 or alignment & (alignment - 1):
            raise ValueError(f"{label} invalid size/alignment for {identity}")
        if rva in rvas:
            previous_identity, previous_row = rvas[rva]
            alias_fields = ("object", "size", "storage", "section_ordinal", "section_offset")
            proved_alias = (
                "candidate-coff-alias" in row["provenance"]
                and "candidate-coff-alias" in previous_row["provenance"]
                and all(row[key] == previous_row[key] for key in alias_fields))
            if not proved_alias:
                raise ValueError(
                    f"{label} duplicate RVA 0x{rva:x}: {identity} and {previous_identity}")
        else:
            rvas[rva] = (identity, row)
        intervals.append((rva, rva + size, identity))
    for previous, current in zip(sorted(intervals), sorted(intervals)[1:]):
        if previous[1] > current[0] and previous[0] != current[0]:
            raise ValueError(f"{label} overlapping allocations {previous} and {current}")


def _physical_contributions(rows):
    """Return unsplit NB09 owner chunks and defer writable class to COFF replay."""
    physical = []
    for row in sorted(rows, key=lambda value: (
            value["object"], int(value["rva"]), int(value["segment"]))):
        if row["storage"] == "text":
            continue
        domain = "writable" if row["section"] == ".data" else row["storage"]
        value = dict(row)
        value["domain"] = domain
        value["rva"] = int(value["rva"])
        value["size"] = int(value["size"])
        if (physical and physical[-1]["object"] == value["object"]
                and physical[-1]["domain"] == domain
                and physical[-1]["segment"] == value["segment"]
                and physical[-1]["section"] == value["section"]
                and physical[-1]["provenance"] == value["provenance"]
                and physical[-1]["storage"] == "data"
                and value["storage"] == "bss"
                and physical[-1]["rva"] + physical[-1]["size"] == value["rva"]):
            physical[-1]["size"] += value["size"]
        else:
            physical.append(value)
    return physical


def _section_domain(section):
    if section.storage in ("data", "bss"):
        return "writable"
    return section.storage


def _section_rows(topology_by_unit, symbol_rows, public_by_symbol,
                  global_public_rvas, contributions):
    anchors = defaultdict(list)
    comdat_identity_anchors = defaultdict(list)
    for row in symbol_rows:
        unit = row["object"].replace("\\", "/").removesuffix(".c")
        ordinal = int(row["section_ordinal"], 0)
        base = int(row["rva"], 0) - int(row["section_offset"], 0)
        anchors[(unit, ordinal)].append(
            (base, row["name"], int(row["rva"], 0), row["provenance"]))
    for unit, (definitions, _sections) in topology_by_unit.items():
        for definition in definitions:
            public_rva = public_by_symbol.get((unit, definition.symbol))
            if public_rva is not None:
                anchors[(unit, definition.section_ordinal)].append((
                    public_rva - definition.section_value, definition.symbol,
                    public_rva, "retail-S_PUB32+recovered-owner"))
                continue
            if not definition.comdat_selection:
                continue
            for global_rva in global_public_rvas.get(definition.symbol, ()):
                comdat_identity_anchors[(unit, definition.section_ordinal)].append((
                    global_rva - definition.section_value, definition.symbol,
                    global_rva, "retail-S_PUB32-global-COMDAT-identity"))
    physical = _physical_contributions(contributions)
    sections_by_unit = {
        unit: [section for section in sections if section.storage is not None]
        for unit, (_definitions, sections) in topology_by_unit.items()
    }
    for index, contribution in enumerate(physical):
        contribution["index"] = index
        unit = contribution["object"].replace("\\", "/").removesuffix(".c")
        if contribution["domain"] != "writable":
            contribution["candidate_storage"] = contribution["domain"]
            continue
        start = contribution["rva"]
        end = start + contribution["size"]
        hit_storages = set()
        for section in sections_by_unit.get(unit, []):
            if _section_domain(section) != "writable":
                continue
            if any(start <= rva < end
                   for _base, _name, rva, _provenance in
                   [*anchors.get((unit, section.ordinal), []),
                    *comdat_identity_anchors.get((unit, section.ordinal), [])]):
                hit_storages.add(section.storage)
        candidate_storages = {
            section.storage for section in sections_by_unit.get(unit, [])
            if _section_domain(section) == "writable"
        }
        if len(hit_storages) == 1:
            contribution["candidate_storage"] = next(iter(hit_storages))
        elif not hit_storages and len(candidate_storages) == 1:
            contribution["candidate_storage"] = next(iter(candidate_storages))
        else:
            contribution["candidate_storage"] = None
    contribution_groups = defaultdict(list)
    for contribution in physical:
        unit = contribution["object"].replace("\\", "/").removesuffix(".c")
        storage = contribution["candidate_storage"]
        if storage is not None:
            contribution_groups[(unit, storage)].append(contribution)

    result = []
    diagnostics = []
    assignments = {}
    used_contributions = set()
    for contribution in physical:
        if contribution["domain"] != "writable":
            continue
        unit = contribution["object"].replace("\\", "/").removesuffix(".c")
        start = contribution["rva"]
        end = start + contribution["size"]
        storages = sorted({
            section.storage
            for section in sections_by_unit.get(unit, [])
            if _section_domain(section) == "writable"
            and any(start <= rva < end
                    for _base, _name, rva, _provenance in
                    [*anchors.get((unit, section.ordinal), []),
                     *comdat_identity_anchors.get((unit, section.ordinal), [])])
        })
        if len(storages) > 1:
            diagnostics.append({
                "unit": unit,
                "section_ordinal": None,
                "section_name": None,
                "storage": "writable",
                "cause": "contribution-storage-conflict",
                "contribution_rva": start,
                "contribution_size": contribution["size"],
                "candidate_storages": storages,
            })
    for unit, (_definitions, sections) in sorted(topology_by_unit.items()):
        data_sections = [section for section in sections if section.storage is not None]
        grouped_sections = defaultdict(list)
        for section in data_sections:
            grouped_sections[section.storage].append(section)
        for storage, group in sorted(grouped_sections.items()):
            intervals = contribution_groups.get((unit, storage), [])
            interval_index = 0
            cursor = intervals[0]["rva"] if intervals else None
            for section in group:
                values = anchors.get((unit, section.ordinal), [])
                bases = {value[0] for value in values}
                predicted = None
                owner_index = None
                while interval_index < len(intervals):
                    interval = intervals[interval_index]
                    candidate = _align_up(cursor, section.alignment)
                    if candidate + section.size <= interval["rva"] + interval["size"]:
                        predicted = candidate
                        owner_index = interval["index"]
                        cursor = candidate + section.size
                        used_contributions.add(interval["index"])
                        break
                    interval_index += 1
                    if interval_index < len(intervals):
                        cursor = intervals[interval_index]["rva"]

                provenance = ("retail-contribution-replay"
                              if predicted is not None else None)
                if predicted is None and section.comdat_selection:
                    fallback_values = values or comdat_identity_anchors.get(
                        (unit, section.ordinal), [])
                    fallback_bases = {value[0] for value in fallback_values}
                    if len(fallback_bases) == 1:
                        predicted = next(iter(fallback_bases))
                        values = fallback_values
                        bases = fallback_bases
                        provenance = "explicit-comdat-anchor"
                if predicted is None:
                    fallback_values = comdat_identity_anchors.get(
                        (unit, section.ordinal), [])
                    fallback_bases = {value[0] for value in fallback_values}
                    if fallback_bases and not bases:
                        values = fallback_values
                        bases = fallback_bases
                    if section.comdat_selection:
                        cause = ("ambiguous-comdat-identity-without-contribution"
                                 if bases else
                                 "unanchored-comdat-without-contribution")
                    else:
                        cause = "section-outside-contributions"
                    diagnostics.append({
                        "unit": unit,
                        "section_ordinal": section.ordinal,
                        "section_name": section.name,
                        "storage": section.storage,
                        "cause": cause,
                        "anchor_bases": sorted(bases),
                        "anchors": [
                            {"base": base, "name": name, "rva": rva,
                             "provenance": anchor_provenance}
                            for base, name, rva, anchor_provenance in values
                        ],
                    })
                    continue

                assignments[(unit, section.ordinal)] = (predicted, owner_index)
                if len(bases) > 1:
                    cause = "inconsistent-anchor-bases"
                elif bases and predicted not in bases:
                    cause = "breakpoint-drift"
                else:
                    cause = None
                if cause:
                    diagnostics.append({
                        "unit": unit,
                        "section_ordinal": section.ordinal,
                        "section_name": section.name,
                        "storage": section.storage,
                        "cause": cause,
                        "predicted_base": predicted,
                        "anchor_bases": sorted(bases),
                        "anchors": [
                            {"base": base, "name": name, "rva": rva,
                             "provenance": anchor_provenance}
                            for base, name, rva, anchor_provenance in values
                        ],
                    })
                elif bases:
                    provenance += "+validated-anchor"

                result.append({
                    "object": section.object_name,
                    "ordinal": str(section.ordinal),
                    "name": section.name,
                    "rva": f"0x{predicted:x}",
                    "size": f"0x{section.size:x}",
                    "alignment": f"0x{section.alignment:x}",
                    "characteristics": f"0x{section.characteristics:08x}",
                    "comdat_selection": str(section.comdat_selection),
                    "associative_ordinal": (str(section.associative_ordinal)
                                            if section.associative_ordinal is not None else "-"),
                    "storage": section.storage,
                    "provenance": provenance,
                })
        assigned_ordinals = {section.ordinal for section in data_sections}
        for section in sections:
            if section.ordinal in assigned_ordinals:
                if (unit, section.ordinal) not in assignments:
                    result.append({
                        "object": section.object_name,
                        "ordinal": str(section.ordinal),
                        "name": section.name,
                        "rva": "-",
                        "size": f"0x{section.size:x}",
                        "alignment": f"0x{section.alignment:x}",
                        "characteristics": f"0x{section.characteristics:08x}",
                        "comdat_selection": str(section.comdat_selection),
                        "associative_ordinal": (str(section.associative_ordinal)
                                                if section.associative_ordinal is not None else "-"),
                        "storage": section.storage,
                        "provenance": "candidate-unassigned-data-section",
                    })
                continue
            result.append({
                "object": section.object_name,
                "ordinal": str(section.ordinal),
                "name": section.name,
                "rva": "-",
                "size": f"0x{section.size:x}",
                "alignment": f"0x{section.alignment:x}",
                "characteristics": f"0x{section.characteristics:08x}",
                "comdat_selection": str(section.comdat_selection),
                "associative_ordinal": (str(section.associative_ordinal)
                                        if section.associative_ordinal is not None else "-"),
                "storage": "-",
                "provenance": "candidate-nondata-placeholder",
            })
    for contribution in physical:
        if contribution["index"] in used_contributions:
            continue
        unit = contribution["object"].replace("\\", "/").removesuffix(".c")
        diagnostics.append({
            "unit": unit,
            "section_ordinal": None,
            "section_name": None,
            "storage": contribution["candidate_storage"] or contribution["domain"],
            "cause": "unconsumed-contribution",
            "contribution_rva": contribution["rva"],
            "contribution_size": contribution["size"],
        })
    result.sort(key=lambda row: (row["object"], int(row["ordinal"])))
    return result, diagnostics, physical


def _stream_bounds(pe, storage: str) -> tuple[int, int]:
    if storage == "rdata":
        section = pe["sections"][".rdata"]
        return section["rva"], section["rva"] + section["virtual_size"]
    section = pe["sections"][".data"]
    # PE raw file alignment does not preserve the linker's initialized/common
    # distinction. Both candidate classes occupy the writable virtual stream.
    return section["rva"], section["rva"] + section["virtual_size"]


def _classified_contribution_rows(physical, section_rows):
    assigned = []
    for row in section_rows:
        if row["rva"] == "-" or row["storage"] == "-":
            continue
        assigned.append((
            row["object"], int(row["rva"], 0),
            int(row["rva"], 0) + int(row["size"], 0), row["storage"],
            int(row["ordinal"], 0),
        ))

    result = []
    for contribution in physical:
        start = contribution["rva"]
        end = start + contribution["size"]
        owned = sorted(row for row in assigned
                       if row[0] == contribution["object"]
                       and start <= row[1] and row[2] <= end)
        if not owned:
            continue
        piece_start = start
        current_storage = owned[0][3]
        for _object, section_start, _section_end, storage, _ordinal in owned[1:]:
            if storage != current_storage:
                result.append({
                    "object": contribution["object"],
                    "storage": current_storage,
                    "rva": piece_start,
                    "size": section_start - piece_start,
                    "segment": contribution["segment"],
                    "section": contribution["section"],
                    "provenance": "retail-nb09-sstModule+candidate-class",
                })
                piece_start = section_start
                current_storage = storage
        result.append({
            "object": contribution["object"],
            "storage": current_storage,
            "rva": piece_start,
            "size": end - piece_start,
            "segment": contribution["segment"],
            "section": contribution["section"],
            "provenance": "retail-nb09-sstModule+candidate-class",
        })
    return [row for row in result if row["size"] > 0]


def breakpoint_report(topology_by_unit, section_rows, section_diagnostics,
                      physical, exe=EXE) -> dict:
    assigned = {(row["object"].replace("\\", "/").removesuffix(".c"),
                 int(row["ordinal"], 0)): int(row["rva"], 0)
                for row in section_rows if row["rva"] != "-"}
    pe = read_pe(exe)
    streams = {}
    for storage in ("rdata", "data", "bss"):
        start, retail_end = _stream_bounds(pe, storage)
        intervals = []
        for unit, (_definitions, sections) in sorted(topology_by_unit.items()):
            for section in sections:
                if section.storage != storage:
                    continue
                actual = assigned.get((unit, section.ordinal))
                if actual is not None and not start <= actual <= retail_end - section.size:
                    section_diagnostics.append({
                        "unit": unit,
                        "section_ordinal": section.ordinal,
                        "section_name": section.name,
                        "storage": storage,
                        "cause": "section-outside-pe-stream",
                        "section_rva": actual,
                        "section_size": section.size,
                        "stream_start": start,
                        "stream_end": retail_end,
                    })
                intervals.append({
                    "unit": unit,
                    "object": section.object_name,
                    "section_ordinal": section.ordinal,
                    "section_name": section.name,
                    "alignment": section.alignment,
                    "size": section.size,
                    "actual_rva": actual,
                })
        evidence = sorted(
            (row["rva"], row["rva"] + row["size"])
            for row in physical
            if ((storage == "rdata" and row["domain"] == "rdata") or
                (storage in ("data", "bss") and row["domain"] == "writable")))
        streams[storage] = {
            "retail_start": start,
            "retail_end": retail_end,
            "evidence_start": evidence[0][0] if evidence else None,
            "evidence_end": evidence[-1][1] if evidence else None,
            "terminal_drift": retail_end - evidence[-1][1] if evidence else None,
            "exact_sections": sum(row["actual_rva"] is not None for row in intervals),
            "unassigned_sections": sum(row["actual_rva"] is None for row in intervals),
            "section_count": len(intervals),
            "intervals": intervals,
        }
    return {
        "schema": 1,
        "policy": ("candidate-order replay within exact NB09 contribution chunks; "
                   "observed anchors validate and never move the cursor"),
        "section_assignment_diagnostics": section_diagnostics,
        "streams": streams,
    }


def build_manifests(source_root=SOURCE_ROOT, base_root=BASE_ROOT, symbols=SYMBOLS,
                    units=UNITS, exe=EXE, supplemental=SUPPLEMENTAL,
                    migrate_from: Path | None = None, strict=False):
    definitions = source_definitions(Path(source_root))
    manifest = tomllib.loads(Path(units).read_text())
    topology_by_unit = {}
    for unit in sorted(row["unit"] for row in manifest.get("unit", [])):
        topology_by_unit[unit] = candidate_topology(
            Path(base_root) / f"{unit}.obj", unit)
    public_by_rva, public_by_symbol, global_public_rvas = _load_public_data(
        Path(symbols))
    resolved = resolve_source_definitions(definitions, topology_by_unit, public_by_rva)
    retail_rvas = sorted({row.rva for row in definitions})
    next_rva = {rva: (retail_rvas[index + 1]
                      if index + 1 < len(retail_rvas) else None)
                for index, rva in enumerate(retail_rvas)}
    source_rows = [_symbol_row(source, candidate, next_rva[source.rva])
                   for source, candidate in resolved]
    validate_symbol_rows(source_rows, "source DATA")
    if migrate_from is not None:
        supplemental_rows = migrate_supplemental(
            Path(migrate_from), Path(supplemental), source_rows, topology_by_unit)
    else:
        supplemental_rows = [_normalize_symbol_row(row, topology_by_unit)
                             for row in _read_tsv(Path(supplemental))]
        validate_symbol_rows(supplemental_rows, "supplemental")
    combined = [*source_rows, *supplemental_rows]
    validate_symbol_rows(combined, "combined")
    retail_contributions = contribution_rows(exe, units)
    section_rows, section_diagnostics, physical = _section_rows(
        topology_by_unit, combined, public_by_symbol, global_public_rvas,
        retail_contributions)
    report = breakpoint_report(
        topology_by_unit, section_rows, section_diagnostics, physical, exe)
    classified_contributions = [
        dict(row) for row in retail_contributions if row["storage"] == "text"
    ]
    classified_contributions.extend(
        _classified_contribution_rows(physical, section_rows))
    classified_contributions.sort(key=lambda row: (
        int(row["rva"]), row["object"], row["storage"]))
    _write_tsv(SOURCE_MANIFEST, SYMBOL_HEADER, _sorted_symbol_rows(source_rows),
               "Generated from source DATA() plus exact candidate COFF identity.")
    _write_tsv(COMBINED_MANIFEST, SYMBOL_HEADER, _sorted_symbol_rows(combined),
               "Generated Vostok symbol manifest; do not edit.")
    _write_tsv(SECTION_MANIFEST, SECTION_HEADER, section_rows,
               "Generated exact candidate COFF section topology; do not edit.")
    _write_tsv(CONTRIBUTION_MANIFEST, CONTRIBUTION_HEADER,
               [{key: (f"0x{row[key]:x}" if key in ("rva", "size") else row[key])
                 for key in CONTRIBUTION_HEADER}
                for row in classified_contributions],
               "NB09 owner ranges classified by reconstructed candidate section storage.")
    BREAKPOINTS.parent.mkdir(parents=True, exist_ok=True)
    BREAKPOINTS.write_text(json.dumps(report, indent=2) + "\n")
    summary = {
        "source_definitions": len(source_rows),
        "supplemental_definitions": len(supplemental_rows),
        "combined_definitions": len(combined),
        "sections": len(section_rows),
        "section_assignment_diagnostics": len(section_diagnostics),
        "breakpoints": {name: {
            "exact": row["exact_sections"], "total": row["section_count"],
            "terminal_drift": row["terminal_drift"],
        } for name, row in report["streams"].items()},
    }
    if strict and section_diagnostics:
        raise ValueError(
            f"{len(section_diagnostics)} section replay diagnostics remain; "
            f"see {BREAKPOINTS}")
    return summary


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-root", type=Path, default=SOURCE_ROOT)
    parser.add_argument("--base-root", type=Path, default=BASE_ROOT)
    parser.add_argument("--symbols", type=Path, default=SYMBOLS)
    parser.add_argument("--units", type=Path, default=UNITS)
    parser.add_argument("--exe", type=Path, default=EXE)
    parser.add_argument("--supplemental", type=Path, default=SUPPLEMENTAL)
    parser.add_argument("--migrate-from", type=Path)
    parser.add_argument("--strict", action="store_true")
    args = parser.parse_args(argv)
    try:
        summary = build_manifests(
            args.source_root, args.base_root, args.symbols, args.units, args.exe,
            args.supplemental, args.migrate_from, args.strict)
    except (OSError, KeyError, ValueError, struct.error) as exc:
        parser.error(str(exc))
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
