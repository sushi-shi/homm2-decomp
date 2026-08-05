"""Bind source data claims to candidate COFF topology.

``DATA()``, ``DATA_COMPGEN()``, ``VTBL()``, and ``VTBL2()`` are the address
authorities for reconstructed storage; candidate objects provide the physical
COFF topology, and this module resolves each claim to its candidate definition.
`build_manifests` emits the delinker symbol manifests from those bindings. It is
the single map both sides read: the delinker names retail storage from it, and
`canonicalize_data_symbols` gives the compiled side the same canonical name from
the same rows, so a claim can never bind on one side only. No hand-maintained
private-data supplement participates in the build.

The section and contribution manifests the PoL 2.0 line also writes here are
absent by necessity, not omission: they derive from retail NB09 sstModule
contributions and this image has no debug stream.
"""

from __future__ import annotations

import argparse
import csv
import os
import re
import struct
import tempfile
import tomllib
import warnings
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

from homm2.build.annotated_data import (
    AnnotatedDataDefinition as SourceDefinition,
    source_definitions as annotated_source_definitions,
)
from homm2.build.annotated_compgen_data import source_compgen_data
from homm2.build.annotated_vtables import source_vtables
from homm2.build.canonicalize_relocs import CoffFile
from homm2.build.data_topology_census import (
    _is_data_section,
    _scope,
    _section_alignment,
    _section_characteristics,
    _section_definition_symbols,
    _storage,
)
from homm2.build.link_exe import classify_pe_storage, read_pe
from homm2.build.candidate_data_manifest import _pe_layout, derive_allocations


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
SOURCE_ROOT = REPO / "src"
BASE_ROOT = REPO / "build/objdiff/base"
SYMBOLS = REPO / "build/gen/symbol_names.csv"
UNITS = REPO / "config/units.toml"
EXE = REPO / "build/orig/HMM2PL.exe"
SOURCE_MANIFEST = REPO / "build/gen/delink_data_from_source.tsv"
COMBINED_MANIFEST = REPO / "build/gen/delink_data_manifest.tsv"
SYMBOL_HEADER = (
    "name", "object", "rva", "size", "storage", "alignment",
    "section_ordinal", "section_offset", "scope", "provenance",
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


def source_definitions(source_root: Path = SOURCE_ROOT,
                       base_root: Path = BASE_ROOT) -> list[SourceDefinition]:
    source_root = Path(source_root)
    if source_root.resolve() != SOURCE_ROOT.resolve():
        return annotated_source_definitions(source_root, REPO)
    return annotated_source_definitions(source_root, REPO, Path(base_root))


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
    # A symbol name may legitimately recur across units; keep the global name
    # index separate from the per-unit associations the claims bind against.
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
        if len(matches) > 1:
            source_peers = sorted(
                (row for row in definitions
                 if row.unit == source.unit and row.name == source.name
                 and row.qualified_name == source.qualified_name),
                key=lambda row: row.rva)
            candidate_peers = sorted(matches, key=lambda row: (
                row.storage, row.stream_offset, row.symbol))
            if len(source_peers) == len(candidate_peers) and source in source_peers:
                matches = [candidate_peers[source_peers.index(source)]]
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


def _candidate_bytes(coff: CoffFile, candidate: CandidateDefinition,
                     size: int) -> bytes:
    section = coff.sections[candidate.section_ordinal - 1]
    if section.raw_offset == 0:
        return b"\0" * size
    start = section.raw_offset + candidate.section_value
    return bytes(coff.data[start:start + size])


def _compgen_candidate_kind(candidate: CandidateDefinition) -> str | None:
    if candidate.symbol.startswith(("$SG", "??_C@")):
        return "STRING_LITERAL"
    # `$T<counter>` is MSVC 4.2's spelling for a floating-point literal COMDAT;
    # VC6 emits `__real@4@<bits>` / `__real@8@<bits>` instead, so a tree built
    # with the retail toolchain has essentially no `$T` float candidates
    # (SOURCE/PHILAI: 96 `__real@` against 2 `$T`). Both spellings name the same
    # thing, and the claim binds by retail RVA and payload rather than by name.
    if candidate.symbol.startswith(("$T", "__real@", "__xmm@")):
        return "FLOAT_LITERAL"
    if candidate.symbol.startswith("_$S5$"):
        return "STATIC_INIT_GUARD"
    return None


def _retail_storage_name(pe, rva: int) -> str | None:
    storage = classify_pe_storage(pe, rva)["class"]
    return {
        "rdata": "rdata",
        "data-initialized": "data",
        "data-loader-zero-tail": "bss",
    }.get(storage)


def _maximum_claim_matching(edges, claims, forbidden=None):
    """Return a deterministic matching covering ``claims``, if one exists."""
    candidate_owner = {}
    assignment = {}

    def augment(claim_index, seen):
        for candidate_index in sorted(edges[claim_index]):
            if forbidden == (claim_index, candidate_index):
                continue
            if candidate_index in seen:
                continue
            seen.add(candidate_index)
            owner = candidate_owner.get(candidate_index)
            if owner is None or augment(owner, seen):
                candidate_owner[candidate_index] = claim_index
                assignment[claim_index] = candidate_index
                return True
        return False

    for claim_index in sorted(claims, key=lambda value: (len(edges[value]), value)):
        if not augment(claim_index, set()):
            return None
    return assignment


def _bind_compgen_edges(edges, claim_order, candidate_order):
    """Resolve bipartite claim/candidate edges without arbitrary assignments.

    A non-order-derived binding must be the only matching which covers every
    claim in its connected component. Equal-sized, order-compatible components
    are the explicitly supported compiler-counter case and pair in preserved
    stream/RVA order.
    """
    claim_rank = {value: index for index, value in enumerate(claim_order)}
    candidate_rank = {value: index for index, value in enumerate(candidate_order)}
    reverse = defaultdict(set)
    for claim_index, choices in edges.items():
        for candidate_index in choices:
            reverse[candidate_index].add(claim_index)

    assignments = {}
    failures = []
    remaining = set(edges)
    while remaining:
        seed = min(remaining, key=lambda value: claim_rank[value])
        component_claims = {seed}
        component_candidates = set()
        frontier = [seed]
        while frontier:
            claim_index = frontier.pop()
            for candidate_index in edges[claim_index]:
                if candidate_index in component_candidates:
                    continue
                component_candidates.add(candidate_index)
                for peer in reverse[candidate_index]:
                    if peer in remaining and peer not in component_claims:
                        component_claims.add(peer)
                        frontier.append(peer)

        ordered_claims = sorted(
            component_claims, key=lambda value: claim_rank[value])
        ordered_candidates = sorted(
            component_candidates, key=lambda value: candidate_rank[value])
        ordered_counter_group = (
            len(ordered_claims) == len(ordered_candidates)
            and all(candidate_index in edges[claim_index]
                    for claim_index, candidate_index
                    in zip(ordered_claims, ordered_candidates)))
        if ordered_counter_group:
            assignments.update(zip(ordered_claims, ordered_candidates))
            remaining -= component_claims
            continue

        matching = _maximum_claim_matching(edges, component_claims)
        if matching is None:
            failures.append(("missing", ordered_claims, ordered_candidates))
            remaining -= component_claims
            continue
        alternative = any(
            _maximum_claim_matching(
                edges, component_claims,
                forbidden=(claim_index, candidate_index)) is not None
            for claim_index, candidate_index in matching.items())
        if alternative:
            failures.append(("ambiguous", ordered_claims, ordered_candidates))
        else:
            assignments.update(matching)
        remaining -= component_claims
    return assignments, failures


def resolve_compgen_definitions(claims, topology_by_unit, base_root: Path,
                                exe: Path, reserved=(), strict=False,
                                symbols: Path = SYMBOLS, units: Path = UNITS):
    """Bind semantic source claims to anonymous candidate COFF allocations.

    Retail RVA/type/size comes from the source claim. Candidate section bytes and
    topology identify the physical allocation. Unique payload edges are consumed
    first; indistinguishable compiler-counter siblings are paired only by the
    preserved candidate allocation order and retail RVA order.
    """
    reserved = {(row.unit, row.section_ordinal, row.section_value)
                for row in reserved}
    pe = read_pe(exe)
    _image_base, highlow, _read_u32, read_bytes = _pe_layout(exe)
    highlow = set(highlow)
    coff_by_unit = {}
    resolved = []
    diagnostics = []
    claims_by_unit = defaultdict(list)
    for claim in claims:
        claims_by_unit[claim.unit].append(claim)
    derived, _stats, derivation_diagnostics = derive_allocations(
        base_dir=base_root, exe=exe, symbols_path=symbols,
        units_path=units, reviewed_rows=())
    derived_by_rva = defaultdict(list)
    for allocation in [
            *derived,
            *(allocation for diagnostic in derivation_diagnostics
              for allocation in diagnostic.proposed_allocations)]:
        derived_by_rva[(allocation.unit, allocation.rva)].append(allocation.name)

    for unit, unit_claims in sorted(claims_by_unit.items()):
        candidates = [
            candidate
            for candidate in topology_by_unit.get(unit, ([], []))[0]
            if (unit, candidate.section_ordinal, candidate.section_value) not in reserved
            and _compgen_candidate_kind(candidate) is not None
        ]
        coff = coff_by_unit.setdefault(
            unit, CoffFile(Path(base_root) / f"{unit}.obj"))
        requested_sizes = defaultdict(set)
        for claim in unit_claims:
            expected_kind = ("FLOAT_LITERAL" if claim.kind.startswith("FLOAT")
                             else claim.kind)
            requested_sizes[(expected_kind,
                             _retail_storage_name(pe, claim.rva))].add(claim.size)
        candidates_by_payload = defaultdict(list)
        for candidate_index_value, candidate in enumerate(candidates):
            candidate_kind = _compgen_candidate_kind(candidate)
            for size in requested_sizes.get(
                    (candidate_kind, candidate.storage), ()):
                if candidate.size >= size:
                    candidates_by_payload[(
                        candidate_kind, candidate.storage, size,
                        _candidate_bytes(coff, candidate, size)
                    )].append(candidate_index_value)
        edges = {}
        for index, claim in enumerate(unit_claims):
            claim_storage = _retail_storage_name(pe, claim.rva)
            expected_kind = ("FLOAT_LITERAL" if claim.kind.startswith("FLOAT")
                             else claim.kind)
            retail_payload = bytearray(read_bytes(claim.rva, claim.size))
            for site in range(claim.rva, claim.rva + claim.size - 3):
                if site in highlow:
                    offset = site - claim.rva
                    retail_payload[offset:offset + 4] = b"\0\0\0\0"
            retail_payload = bytes(retail_payload)
            payload_edges = set(candidates_by_payload.get(
                (expected_kind, claim_storage, claim.size, retail_payload), ()))
            placed_names = set(derived_by_rva.get((unit, claim.rva), ()))
            placed_edges = {
                candidate_index_value
                for candidate_index_value in payload_edges
                if candidates[candidate_index_value].symbol in placed_names
            }
            decorated_exact_edges = {
                value for value in payload_edges
                if (candidates[value].size == claim.size
                    and candidates[value].symbol.startswith("??_C@"))
            }
            edges[index] = (decorated_exact_edges
                            if len(decorated_exact_edges) == 1
                            else placed_edges if len(placed_edges) == 1
                            else payload_edges)

        claim_order = sorted(edges, key=lambda value: unit_claims[value].rva)
        candidate_order = sorted(
            range(len(candidates)),
            key=lambda value: (candidates[value].storage,
                               candidates[value].stream_offset,
                               candidates[value].symbol))
        assignments, failures = _bind_compgen_edges(
            edges, claim_order, candidate_order)
        for cause, failed_claims, failed_candidates in failures:
            diagnostics.append(
                f"{unit}: {cause} compiler-generated binding for "
                f"{len(failed_claims)} semantic claims and "
                f"{len(failed_candidates)} anonymous candidate allocations: "
                + ", ".join(unit_claims[value].semantic_name
                            for value in failed_claims[:6]))

        for claim_index, candidate_index in sorted(assignments.items()):
            resolved.append((unit_claims[claim_index], candidates[candidate_index]))

    if diagnostics:
        detail = "\n".join(diagnostics)
        if strict:
            raise ValueError(detail)
        warnings.warn(detail, stacklevel=2)
    return resolved, diagnostics


def _compgen_row(claim, candidate: CandidateDefinition) -> dict[str, str]:
    if claim.size <= 0 or claim.size > candidate.size:
        raise ValueError(
            f"logical compiler-generated size 0x{claim.size:x} exceeds candidate "
            f"span 0x{candidate.size:x} at {claim.location}")
    semantic = "__h2cg$" + candidate.unit.replace("/", "$")
    semantic += "$data$" + claim.semantic_name
    return {
        "name": semantic,
        "object": candidate.unit.replace("/", "\\") + ".c",
        "rva": f"0x{claim.rva:x}",
        "size": f"0x{claim.size:x}",
        "storage": candidate.storage,
        "alignment": f"0x{candidate.alignment:x}",
        "section_ordinal": str(candidate.section_ordinal),
        "section_offset": f"0x{candidate.section_value:x}",
        "scope": "external" if candidate.storage_class == 2 else "local",
        "provenance": f"source-DATA_COMPGEN:{claim.location}",
    }


def resolve_vtable_definitions(claims, topology_by_unit, public_by_rva):
    """Bind each source VTBL/VTBL2 claim to one emitted candidate definition."""
    resolved = []
    public_locations = defaultdict(set)
    for (unit, rva), names in public_by_rva.items():
        for name in names:
            public_locations[name].add((unit, rva))
    for claim in claims:
        candidates = [
            row for row in topology_by_unit.get(claim.unit, ([], []))[0]
            if row.symbol == claim.mangled_name
        ]
        if len(candidates) != 1:
            raise ValueError(
                f"{claim.location}: expected one candidate {claim.mangled_name}, "
                f"found {len(candidates)}")
        candidate = candidates[0]
        if candidate.storage != "rdata" or candidate.storage_class != 2:
            raise ValueError(
                f"{claim.location}: vtable {claim.mangled_name} is not external rdata")
        public_names = set(public_by_rva.get((claim.unit, claim.rva), ()))
        if claim.base is None and claim.mangled_name not in public_names:
            raise ValueError(
                f"{claim.location}: primary vtable {claim.mangled_name} has no "
                "matching source-vtable inventory row")
        if (public_locations.get(claim.mangled_name)
                and claim.mangled_name not in public_names):
            raise ValueError(
                f"{claim.location}: the source inventory places "
                f"{claim.mangled_name} at another RVA")
        resolved.append((claim, candidate))
    return resolved


def _vtable_row(claim, candidate: CandidateDefinition) -> dict[str, str]:
    return {
        "name": candidate.symbol,
        "object": candidate.unit.replace("/", "\\") + ".c",
        "rva": f"0x{claim.rva:x}",
        "size": f"0x{candidate.size:x}",
        "storage": candidate.storage,
        "alignment": f"0x{candidate.alignment:x}",
        "section_ordinal": str(candidate.section_ordinal),
        "section_offset": f"0x{candidate.section_value:x}",
        "scope": "external",
        "provenance": f"source-{'VTBL2' if claim.base else 'VTBL'}:{claim.location}",
    }


def _mark_vtable_aliases(rows: list[dict[str, str]]) -> None:
    """Mark source vtable names which candidate COFF aliases at one allocation."""
    by_rva = defaultdict(list)
    for row in rows:
        by_rva[int(row["rva"], 0)].append(row)
    for aliases in by_rva.values():
        if len(aliases) < 2:
            continue
        for row in aliases:
            row["provenance"] += ":candidate-coff-alias"


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
            alias_fields = (
                "object", "size", "storage", "alignment", "section_ordinal",
                "section_offset", "scope")
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


def build_manifests(source_root=SOURCE_ROOT, base_root=BASE_ROOT, symbols=SYMBOLS,
                    units=UNITS, exe=EXE, strict=False):
    """Bind every source data claim to candidate COFF topology and emit the
    delinker symbol manifests.

    This is the single map both consumers read: the delinker names retail
    storage from it (`configure.py` passes `--data-manifest`), and
    `canonicalize_data_symbols.load_compgen_data_claims` reads the same rows to
    give the compiled side the same canonical name.  Neither side re-parses
    `DATA_COMPGEN()` for itself, so a claim cannot bind on one side only.

    The section and contribution manifests the PoL 2.0 line also writes here are
    deliberately absent: they are derived from retail NB09 sstModule
    contributions, and this image has no debug stream.  Symbol rows never
    depended on them - `source_rows` is complete before that work begins - so
    the claim channel does not have to wait for a substitute.
    """
    definitions = source_definitions(Path(source_root), Path(base_root))
    manifest = tomllib.loads(Path(units).read_text())
    topology_by_unit = {}
    for unit in sorted(row["unit"] for row in manifest.get("unit", [])):
        topology_by_unit[unit] = candidate_topology(
            Path(base_root) / f"{unit}.obj", unit)
    public_by_rva, _public_by_symbol, _global_public_rvas = _load_public_data(
        Path(symbols))
    resolved = resolve_source_definitions(definitions, topology_by_unit, public_by_rva)
    retail_rvas = sorted({row.rva for row in definitions})
    next_rva = {rva: (retail_rvas[index + 1]
                      if index + 1 < len(retail_rvas) else None)
                for index, rva in enumerate(retail_rvas)}
    source_rows = [_symbol_row(source, candidate, next_rva[source.rva])
                   for source, candidate in resolved]
    vtable_claims = source_vtables(Path(source_root), REPO)
    resolved_vtables = resolve_vtable_definitions(
        vtable_claims, topology_by_unit, public_by_rva)
    vtable_rows = [_vtable_row(claim, candidate)
                   for claim, candidate in resolved_vtables]
    _mark_vtable_aliases(vtable_rows)
    source_rows.extend(vtable_rows)
    compgen_claims = source_compgen_data(Path(source_root), REPO)
    resolved_compgen, compgen_diagnostics = resolve_compgen_definitions(
        compgen_claims, topology_by_unit, Path(base_root), Path(exe),
        reserved=[candidate for _source, candidate in resolved]
                 + [candidate for _claim, candidate in resolved_vtables],
        strict=strict, symbols=Path(symbols), units=Path(units))
    compgen_rows = [_compgen_row(claim, candidate)
                    for claim, candidate in resolved_compgen]
    source_rows.extend(compgen_rows)
    validate_symbol_rows(source_rows, "source annotations")
    combined = list(source_rows)
    validate_symbol_rows(combined, "combined")
    _write_tsv(SOURCE_MANIFEST, SYMBOL_HEADER, _sorted_symbol_rows(source_rows),
               "Generated from source DATA()/DATA_COMPGEN()/VTBL()/VTBL2() plus "
               "exact candidate COFF identity.")
    _write_tsv(COMBINED_MANIFEST, SYMBOL_HEADER, _sorted_symbol_rows(combined),
               "Generated Vostok symbol manifest; do not edit.")
    return {
        "definitions": len(resolved),
        "vtables": len(resolved_vtables),
        "compgen": len(resolved_compgen),
        "rows": len(combined),
        "compgen_diagnostics": list(compgen_diagnostics),
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--strict", action="store_true",
                        help="fail instead of warning on an unresolved compgen claim")
    args = parser.parse_args(argv)
    summary = build_manifests(strict=args.strict)
    print("[data-manifest] %d data + %d vtable + %d compgen = %d row(s) -> %s"
          % (summary["definitions"], summary["vtables"], summary["compgen"],
             summary["rows"], COMBINED_MANIFEST.relative_to(REPO)))
    for diagnostic in summary["compgen_diagnostics"]:
        print(f"[data-manifest] {diagnostic}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
