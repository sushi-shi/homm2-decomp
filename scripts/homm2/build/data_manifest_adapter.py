"""Bind source data claims to candidate COFF topology.

``DATA()``, ``DATA_COMPGEN()``, ``VTBL()``, and ``VTBL2()`` are the address
authorities for reconstructed storage; candidate objects provide the physical
COFF topology, and this module resolves each claim to its candidate definition.
The data campaign will assemble delinker manifests from these bindings; until
then `reviewed_data` writes the header-only stub the normalize edges read. No
hand-maintained private-data supplement participates in the build.
"""

from __future__ import annotations

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
from homm2.build.annotated_compgen_data import (
    compgen_data_symbol_name,
    source_compgen_data,
)
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
from homm2.build.reloc_owners import load_reviewed_highlow_sites


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
SOURCE_ROOT = REPO / "src"
BASE_ROOT = REPO / "build/objdiff/base"
SYMBOLS = REPO / "build/gen/symbol_names.csv"
UNITS = REPO / "config/units.toml"
EXE = REPO / "build/orig/HMM2PL.exe"
RELOC_MANIFEST = REPO / "config/delink_relocs.tsv"
IMAGE_REL_I386_DIR32 = 0x0006
SYMBOL_HEADER = (
    "name", "object", "rva", "size", "storage", "alignment",
    "section_ordinal", "section_offset", "scope", "provenance",
)
DELINK_HEADER = (
    "object", "rva", "size", "storage", "alignment",
    "section_ordinal", "section_offset", "scope",
)
SECTION_HEADER = (
    "object", "ordinal", "name", "rva", "size", "alignment",
    "characteristics", "checksum", "comdat_selection",
    "associative_ordinal", "storage",
)
COMMON_HEADER = ("object", "name", "size")
LOCAL_SUFFIX = re.compile(r"^_?(.+?)\$S[0-9]+$")
REAL_LITERAL = re.compile(r"^__real@(4|8)@[0-9a-f]{20}$")
FOLDABLE_COMDAT_SELECTIONS = frozenset((2, 3, 4, 6, 7))
VOLATILE_E_FUNCTION = re.compile(r"^_?\$E[0-9]+$")
ORDINAL_STATIC_GUARD = re.compile(r"^_?\$S[0-9]+$")
FUNCTION_TYPE = 0x0020
MEM_EXECUTE = 0x20000000


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
    checksum: int
    comdat_selection: int
    associative_ordinal: int | None


@dataclass(frozen=True)
class RetailFunction:
    rva: int
    size: int
    provenance: str


def source_definitions(source_root: Path = SOURCE_ROOT,
                       base_root: Path = BASE_ROOT) -> list[SourceDefinition]:
    source_root = Path(source_root)
    if source_root.resolve() != SOURCE_ROOT.resolve():
        return annotated_source_definitions(source_root, REPO)
    return annotated_source_definitions(source_root, REPO, Path(base_root))


def _decoded_symbol_names(symbol: str) -> set[str]:
    undecorated = symbol.removeprefix("_")
    names = {symbol, undecorated}
    local = LOCAL_SUFFIX.match(symbol)
    if local:
        names.add(local.group(1).removeprefix("_"))
    # VC6 prefixes block-scope static data symbols with an ordinary COFF
    # underscore before their C++ decoration, for example
    # ``_?value@?BB@??function@@...``.  Clang reports the source VarDecl as
    # ``?value@?1??function@@...``; the lexical-scope code is compiler-local,
    # but the leading identifier is still the real source identity.
    if undecorated.startswith("?") and "@@" in undecorated:
        spelling = undecorated[1:undecorated.index("@@")]
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
        checksum = definitions[0].aux[0].checksum if definitions else 0
        storage = _storage(section.name, characteristics) if _is_data_section(
            section.name, characteristics) else None
        if storage is not None:
            cursor = _align_up(stream_cursors[storage], alignment)
            section_bases[section.index] = cursor
            stream_cursors[storage] = cursor + section.raw_size
        sections.append(CandidateSection(
            unit, unit.replace("/", "\\") + ".c", section.index, section.name,
            section.raw_size, alignment, characteristics, storage, checksum,
            selection, associative,
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


def _load_function_claims(path: Path = SYMBOLS):
    """Return reviewed function claims by identity and by owning compiland.

    CRT input sections point at compiler-generated initializer functions.  The
    candidate relocation supplies the owning compiland and physical function
    extent; this inventory supplies possible retail placements.  Compiler
    counter spellings are never treated as semantic identity.
    """
    by_identity = defaultdict(list)
    by_unit = defaultdict(list)
    with Path(path).open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "func":
                claim = RetailFunction(
                    int(row["rva"], 0), int(row.get("size") or "0", 0),
                    row.get("provenance") or "")
                by_identity[(row["unit"], row["name"])].append(claim)
                by_unit[row["unit"]].append(claim)
    return (
        {identity: tuple(sorted(rows, key=lambda row: row.rva))
         for identity, rows in by_identity.items()},
        {unit: tuple(sorted(rows, key=lambda row: row.rva))
         for unit, rows in by_unit.items()},
    )


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
    if candidate.symbol.startswith("$T") or REAL_LITERAL.fullmatch(
            candidate.symbol):
        return "FLOAT_LITERAL"
    if candidate.symbol.startswith("_$S5$"):
        return "STATIC_INIT_GUARD"
    if (candidate.storage == "bss"
            and ORDINAL_STATIC_GUARD.fullmatch(candidate.symbol)):
        return "STATIC_INIT_GUARD"
    return None


def _retail_storage_name(pe, rva: int) -> str | None:
    storage = classify_pe_storage(pe, rva)["class"]
    return {
        "rdata": "rdata",
        "data-initialized": "data",
        "data-loader-zero-tail": "bss",
    }.get(storage)


def _interior_compgen_aliases(claims):
    """Return claims whose retail extent is contained by another claim.

    A stripped PE can prove that two relocation targets share bytes, but one
    candidate definition cannot be invented for an interior alias.  Keep the
    owning allocation and leave the alias unresolved until source models the
    shared object and owner-relative addend explicitly.
    """
    aliases = {}
    ordered = sorted(claims, key=lambda row: (row.rva, -row.size,
                                               row.semantic_name))
    for index, claim in enumerate(ordered):
        start, end = claim.rva, claim.rva + claim.size
        owners = [
            owner for owner in ordered[:index]
            if owner.rva <= start and end <= owner.rva + owner.size
            and (owner.rva, owner.size) != (claim.rva, claim.size)
        ]
        if owners:
            aliases[claim] = min(
                owners, key=lambda owner: (owner.size, owner.rva,
                                            owner.semantic_name))
    return aliases


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
        interior_aliases = _interior_compgen_aliases(unit_claims)
        for claim, owner in sorted(
                interior_aliases.items(), key=lambda item: item[0].rva):
            diagnostics.append(
                f"{unit}: compiler-generated retail claim "
                f"{claim.semantic_name} at 0x{claim.rva:x}+0x{claim.size:x} "
                f"aliases the interior of {owner.semantic_name} at "
                f"0x{owner.rva:x}+0x{owner.size:x}; source must model one "
                "shared object and an owner-relative addend")
        active_claims = [
            claim for claim in unit_claims if claim not in interior_aliases
        ]
        candidates = [
            candidate
            for candidate in topology_by_unit.get(unit, ([], []))[0]
            if (unit, candidate.section_ordinal, candidate.section_value) not in reserved
            and _compgen_candidate_kind(candidate) is not None
        ]
        coff = coff_by_unit.setdefault(
            unit, CoffFile(Path(base_root) / f"{unit}.obj"))
        requested_sizes = defaultdict(set)
        for claim in active_claims:
            expected_kind = ("FLOAT_LITERAL" if claim.kind.startswith("FLOAT")
                             else claim.kind)
            retail_storage = _retail_storage_name(pe, claim.rva)
            requested_sizes[(expected_kind, retail_storage)].add(claim.size)
            # A PE merges input .data and .bss into one output section.  A
            # zero-filled .bss contribution can therefore lie inside the
            # output section's raw span when initialized contributions follow
            # it.  The final image cannot distinguish that case; candidate
            # COFF retains the physical input-section evidence.
            if retail_storage == "data":
                requested_sizes[(expected_kind, "bss")].add(claim.size)
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
        active_indexes = [
            index for index, claim in enumerate(unit_claims)
            if claim not in interior_aliases
        ]
        for index in active_indexes:
            claim = unit_claims[index]
            claim_storage = _retail_storage_name(pe, claim.rva)
            expected_kind = ("FLOAT_LITERAL" if claim.kind.startswith("FLOAT")
                             else claim.kind)
            retail_payload = bytearray(read_bytes(claim.rva, claim.size))
            for site in range(claim.rva, claim.rva + claim.size - 3):
                if site in highlow:
                    offset = site - claim.rva
                    retail_payload[offset:offset + 4] = b"\0\0\0\0"
            retail_payload = bytes(retail_payload)
            storage_options = [claim_storage]
            if claim_storage == "data" and not any(retail_payload):
                storage_options.append("bss")
            payload_edges = {
                candidate_index_value
                for storage in storage_options
                for candidate_index_value in candidates_by_payload.get(
                    (expected_kind, storage, claim.size, retail_payload), ())
            }
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


def source_manifest_rows(source_root: Path = SOURCE_ROOT,
                         base_root: Path = BASE_ROOT,
                         exe: Path = EXE, strict=False):
    """Assemble source DATA/DATA_COMPGEN/vtable claims over candidate COFF."""
    source_root = Path(source_root)
    base_root = Path(base_root)
    repo = source_root.parent
    definitions = source_definitions(source_root, base_root)
    compgen = source_compgen_data(source_root, repo)
    vtables = source_vtables(source_root, repo)
    claim_units = {row.unit for row in [*definitions, *compgen, *vtables]}
    document = tomllib.loads(Path(UNITS).read_text())
    configured_units = {
        row["unit"] for row in document.get("unit", ())
        if (base_root / f'{row["unit"]}.obj').is_file()
    }
    units = sorted(claim_units | configured_units)
    missing = [base_root / f"{unit}.obj" for unit in claim_units
               if not (base_root / f"{unit}.obj").is_file()]
    if missing:
        raise FileNotFoundError(
            "candidate objects are missing: "
            + ", ".join(str(path) for path in missing[:8]))
    topology = {
        unit: candidate_topology(base_root / f"{unit}.obj", unit)
        for unit in units
    }
    public_by_rva, _public_by_symbol, _global_rvas = _load_public_data()
    ordinary = resolve_source_definitions(
        definitions, topology, public_by_rva)
    compiler_generated, diagnostics = resolve_compgen_definitions(
        compgen, topology, base_root, Path(exe), strict=strict)
    table_rows = resolve_vtable_definitions(
        vtables, topology, public_by_rva)
    compiler_rows = _folded_compgen_rows(compiler_generated, topology)
    rows = [
        *(_symbol_row(source, candidate, None)
          for source, candidate in ordinary),
        *compiler_rows,
        *(_vtable_row(source, candidate)
          for source, candidate in table_rows),
    ]
    _mark_vtable_aliases(rows)
    validate_symbol_rows(rows, "source data manifest")
    return _sorted_symbol_rows(rows), diagnostics


def render_source_manifest(rows):
    """Serialize semantic source identities used by comparison tooling."""
    lines = [
        "# Source DATA, DATA_COMPGEN, VTBL, and VTBL2 identities bound to "
        "candidate COFF topology.",
        "\t".join(SYMBOL_HEADER),
    ]
    for row in rows:
        lines.append("\t".join(row[key] for key in SYMBOL_HEADER))
    return ("\n".join(lines) + "\n").encode("utf-8")


def delinker_manifest_bytes(rows):
    """Serialize Vostok's physical placement schema.

    Vostok obtains symbol identities from the synthetic PDB. Semantic
    DATA_COMPGEN names remain in the source manifest consumed by the comparison
    normalizer, while reviewed candidate section coordinates select the exact
    physical COFF allocation that Vostok must emit.
    """
    lines = [
        "# Physical placements projected from the reviewed source data model.",
        "\t".join(DELINK_HEADER),
    ]
    for row in rows:
        lines.append("\t".join(row[key] for key in DELINK_HEADER))
    return ("\n".join(lines) + "\n").encode("utf-8")


def source_manifest_bytes(source_root: Path = SOURCE_ROOT,
                          base_root: Path = BASE_ROOT,
                          exe: Path = EXE, strict=False):
    rows, diagnostics = source_manifest_rows(
        source_root, base_root, exe, strict=strict)
    return render_source_manifest(rows), diagnostics


def _unique_reviewed_pointer_sequence_rva(values, reviewed_cells,
                                          alignment: int) -> int | None:
    """Locate one complete pointer sequence in reviewed retail DIR32 cells.

    Every dword must be a reviewed relocation site.  Matching pointer-looking
    bytes outside that inventory are not placement evidence, and two matching
    sequences are deliberately left ambiguous.
    """
    if not values or alignment <= 0 or alignment & (alignment - 1):
        return None
    choices = tuple(
        value if isinstance(value, (tuple, list, set, frozenset)) else (value,)
        for value in values)
    matches = []
    for start, first_value in reviewed_cells.items():
        if start % alignment or first_value not in choices[0]:
            continue
        if all(reviewed_cells.get(start + offset * 4) in value_choices
               for offset, value_choices in enumerate(choices)):
            matches.append(start)
            if len(matches) > 1:
                return None
    return matches[0] if matches else None


def _candidate_function_claims(coff: CoffFile, symbol, unit: str,
                               functions_by_identity, functions_by_unit):
    """Return retail claims compatible with one candidate function extent."""
    if symbol.section <= 0:
        return ()
    typ, _storage_class = _coff_symbol_fields(coff, symbol)
    section = coff.sections[symbol.section - 1]
    characteristics = _section_characteristics(coff, symbol.section)
    if typ != FUNCTION_TYPE or not characteristics & MEM_EXECUTE:
        return ()
    peers = []
    for other in coff.symbols.values():
        if other.section != symbol.section or other.value <= symbol.value:
            continue
        other_type, _other_storage = _coff_symbol_fields(coff, other)
        if other_type == FUNCTION_TYPE:
            peers.append(other.value)
    end = min(peers, default=section.raw_size)
    if not symbol.value < end <= section.raw_size or section.raw_offset == 0:
        return ()
    physical = bytes(coff.data[
        section.raw_offset + symbol.value:section.raw_offset + end])

    def compatible(claim):
        return (0 < claim.size <= len(physical)
                and all(byte in (0x90, 0xCC) for byte in physical[claim.size:]))

    exact = tuple(
        claim for claim in functions_by_identity.get((unit, symbol.name), ())
        if compatible(claim))
    if not VOLATILE_E_FUNCTION.fullmatch(symbol.name):
        return exact
    # $E counters are unstable across otherwise equivalent TUs.  The set is
    # therefore based on owner, reviewed compiler-function provenance, and
    # physical extent; the complete CRT pointer sequence resolves the member.
    compiler_claims = tuple(
        claim for claim in functions_by_unit.get(unit, ())
        if claim.provenance.startswith(("reviewed-compgen", "source-VA_COMPGEN:"))
        and compatible(claim))
    return tuple(sorted(set((*exact, *compiler_claims)), key=lambda row: row.rva))


def _candidate_crt_pointer_values(coff: CoffFile, section,
                                  unit: str, functions_by_identity,
                                  functions_by_unit,
                                  image_base: int):
    """Resolve each candidate CRT cell to reviewed linked-pointer choices."""
    if (section.raw_size <= 0 or section.raw_size % 4
            or section.raw_offset == 0):
        return None
    section_relocs = {
        site: relocation
        for (section_index, site), relocation in coff.relocations.items()
        if section_index == section.index
    }
    if set(section_relocs) != set(range(0, section.raw_size, 4)):
        return None
    values = []
    for offset in range(0, section.raw_size, 4):
        relocation = section_relocs[offset]
        if relocation.typ != IMAGE_REL_I386_DIR32:
            return None
        symbol = coff.symbols.get(relocation.symbol_index)
        if symbol is None:
            return None
        claims = _candidate_function_claims(
            coff, symbol, unit, functions_by_identity, functions_by_unit)
        if not claims:
            return None
        addend = struct.unpack_from(
            "<I", coff.data, section.raw_offset + offset)[0]
        values.append(frozenset(
            (image_base + claim.rva + addend) & 0xFFFFFFFF
            for claim in claims))
    return tuple(values)


def _derive_crt_section_rvas(base_root: Path, unit_names,
                             symbols: Path, exe: Path,
                             reloc_manifest: Path):
    """Bind compiler-emitted CRT contributions to unique retail spans.

    This is physical section topology, not a source-level data identity: the
    compiler creates each pointer cell for a global initializer.  Candidate
    COFF proves owner/order/relocation spelling, while the reviewed function
    inventory and reviewed retail DIR32 sites prove its final placement.
    """
    pe = read_pe(exe)
    image_base, _highlow, read_u32, _read_bytes = _pe_layout(exe)
    functions_by_identity, functions_by_unit = _load_function_claims(symbols)
    reviewed_cells = {}
    for site in load_reviewed_highlow_sites(reloc_manifest):
        if classify_pe_storage(pe, site)["class"] != "data-initialized":
            continue
        reviewed_cells[site] = read_u32(site)

    placements = {}
    occupied = {}
    for unit in unit_names:
        coff = CoffFile(Path(base_root) / f"{unit}.obj")
        for section in coff.sections:
            if not section.name.startswith(".CRT$"):
                continue
            values = _candidate_crt_pointer_values(
                coff, section, unit, functions_by_identity,
                functions_by_unit, image_base)
            if values is None:
                continue
            characteristics = _section_characteristics(coff, section.index)
            alignment = _section_alignment(characteristics)
            rva = _unique_reviewed_pointer_sequence_rva(
                values, reviewed_cells, alignment)
            if rva is None:
                continue
            if classify_pe_storage(
                    pe, rva + section.raw_size - 1)["class"] != "data-initialized":
                continue
            extent = (rva, rva + section.raw_size)
            previous = occupied.get(extent)
            if previous is not None:
                raise ValueError(
                    f"candidate CRT sections {previous} and {unit}:{section.index} "
                    f"both map to retail extent 0x{rva:x}+0x{section.raw_size:x}")
            occupied[extent] = f"{unit}:{section.index}"
            placements[(unit, section.index)] = rva
    return placements


def candidate_section_manifest_bytes(base_root: Path = BASE_ROOT,
                                     units: Path = UNITS,
                                     symbols: Path = SYMBOLS,
                                     exe: Path = EXE,
                                     reloc_manifest: Path = RELOC_MANIFEST):
    """Serialize candidate data-section and COMDAT topology for Vostok."""
    base_root = Path(base_root)
    document = tomllib.loads(Path(units).read_text())
    unit_names = [row["unit"] for row in document.get("unit", ())]
    if len(unit_names) != len(set(unit_names)):
        raise ValueError("duplicate units in candidate section manifest")
    crt_rvas = _derive_crt_section_rvas(
        base_root, unit_names, Path(symbols), Path(exe), Path(reloc_manifest))
    rows = []
    for unit in unit_names:
        path = base_root / f"{unit}.obj"
        if not path.is_file():
            raise FileNotFoundError(
                f"candidate object is missing for section manifest: {path}")
        _definitions, sections = candidate_topology(path, unit)
        for section in sections:
            # Vostok's reviewed data topology covers the linker output domains
            # .data/.rdata/.bss and sorted .CRT contributions.  Associative
            # .xdata$x COMDATs are exception metadata owned by code sections,
            # not reconstructed C++ data objects.  Every section still appears
            # so candidate ordinals and associative COMDAT parents remain exact.
            modeled_storage = (
                section.storage
                if (section.name in (".data", ".rdata", ".bss")
                    or section.name.startswith(".CRT$"))
                else None
            )
            rows.append({
                "object": section.object_name,
                "ordinal": str(section.ordinal),
                "name": section.name,
                # Definitions carry independently reviewed retail RVAs.  A
                # non-affine section is replayed in candidate shape and gaps
                # remain padding, never synthetic symbols.
                "rva": (
                    f"0x{crt_rvas[(unit, section.ordinal)]:x}"
                    if (unit, section.ordinal) in crt_rvas else "-"),
                "size": f"0x{section.size:x}",
                "alignment": f"0x{section.alignment:x}",
                "characteristics": f"0x{section.characteristics:x}",
                "checksum": f"0x{section.checksum:x}",
                "comdat_selection": str(section.comdat_selection),
                "associative_ordinal": (
                    "-" if section.associative_ordinal is None
                    else str(section.associative_ordinal)),
                "storage": modeled_storage or "-",
            })
    identities = [(row["object"], row["ordinal"]) for row in rows]
    if len(identities) != len(set(identities)):
        raise ValueError("duplicate candidate data section identity")
    lines = [
        "# Candidate COFF data-section topology; definition RVAs come from "
        "the source data manifest.",
        "\t".join(SECTION_HEADER),
    ]
    for row in rows:
        lines.append("\t".join(row[key] for key in SECTION_HEADER))
    return ("\n".join(lines) + "\n").encode("utf-8")


def candidate_common_manifest_bytes(base_root: Path = BASE_ROOT,
                                    units: Path = UNITS):
    """Serialize candidate COFF COMMON definitions for Vostok.

    COMMON symbols have no section allocation or retail RVA: their non-zero
    COFF value is the requested allocation size.  Their externally linked
    spelling and per-object multiplicity are physical candidate topology, not
    anonymous compiler-counter identities.
    """
    base_root = Path(base_root)
    document = tomllib.loads(Path(units).read_text())
    unit_names = [row["unit"] for row in document.get("unit", ())]
    if len(unit_names) != len(set(unit_names)):
        raise ValueError("duplicate units in candidate COMMON manifest")
    rows = []
    for unit in unit_names:
        path = base_root / f"{unit}.obj"
        if not path.is_file():
            raise FileNotFoundError(
                f"candidate object is missing for COMMON manifest: {path}")
        coff = CoffFile(path)
        for symbol in coff.symbols.values():
            symbol_type, storage_class = _coff_symbol_fields(coff, symbol)
            if (symbol.section != 0 or symbol.value == 0 or
                    symbol_type != 0 or storage_class != 2):
                continue
            rows.append({
                "object": unit.replace("/", "\\") + ".c",
                "name": symbol.name,
                "size": f"0x{symbol.value:x}",
            })
    identities = [(row["object"], row["name"]) for row in rows]
    if len(identities) != len(set(identities)):
        raise ValueError("duplicate candidate COMMON identity in one object")
    lines = [
        "# Candidate COFF COMMON definitions; value is allocation size.",
        "\t".join(COMMON_HEADER),
    ]
    for row in rows:
        lines.append("\t".join(row[key] for key in COMMON_HEADER))
    return ("\n".join(lines) + "\n").encode("utf-8")


def _compgen_row(claim, candidate: CandidateDefinition) -> dict[str, str]:
    if claim.size <= 0 or claim.size > candidate.size:
        raise ValueError(
            f"logical compiler-generated size 0x{claim.size:x} exceeds candidate "
            f"span 0x{candidate.size:x} at {claim.location}")
    semantic = compgen_data_symbol_name(candidate.unit, claim.semantic_name)
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


def _folded_compgen_rows(bindings, topology_by_unit):
    """Project one reviewed external COMDAT identity into every emitter.

    A string or real-literal COMDAT is present in every candidate object which
    emits it, while the linker folds those copies onto one retail RVA.  The
    source use-site claim establishes the retail identity once.  Candidate COFF
    then proves the additional emitters, but only when the raw symbol and the
    complete section topology/checksum agree with the claimed copy.
    """
    rows = []
    candidates_by_symbol = defaultdict(list)
    sections_by_location = {}
    for unit, (definitions, sections) in topology_by_unit.items():
        for section in sections:
            sections_by_location[(unit, section.ordinal)] = section
        for candidate in definitions:
            if (candidate.storage_class == 2
                    and candidate.comdat_selection in FOLDABLE_COMDAT_SELECTIONS
                    and _compgen_candidate_kind(candidate) in (
                        "STRING_LITERAL", "FLOAT_LITERAL")):
                candidates_by_symbol[candidate.symbol].append(candidate)

    def signature(candidate):
        section = sections_by_location[
            (candidate.unit, candidate.section_ordinal)]
        return (
            candidate.symbol,
            candidate.section_name,
            candidate.section_value,
            candidate.size,
            candidate.alignment,
            candidate.storage,
            candidate.storage_class,
            candidate.characteristics,
            candidate.comdat_selection,
            candidate.associative_ordinal,
            section.name,
            section.size,
            section.alignment,
            section.characteristics,
            section.storage,
            section.checksum,
            section.comdat_selection,
            section.associative_ordinal,
        )

    identities = set()
    for claim, owner in bindings:
        owner_row = _compgen_row(claim, owner)
        peers = [
            candidate
            for candidate in candidates_by_symbol.get(owner.symbol, ())
            if signature(candidate) == signature(owner)
        ]
        if not peers:
            peers = [owner]
        peer_units = {candidate.unit for candidate in peers}
        if len(peer_units) != len(peers):
            raise ValueError(
                f"{owner.symbol} has duplicate folded COMDAT definitions in one object")
        if len(peers) > 1:
            owner_row["provenance"] += ":candidate-coff-folded-comdat"
        for peer in sorted(peers, key=lambda row: (
                row.unit, row.section_ordinal, row.section_value)):
            row = dict(owner_row)
            row.update({
                "object": peer.unit.replace("/", "\\") + ".c",
                "alignment": f"0x{peer.alignment:x}",
                "section_ordinal": str(peer.section_ordinal),
                "section_offset": f"0x{peer.section_value:x}",
                "scope": (
                    "external" if peer.storage_class == 2 else "local"),
            })
            identity = (row["object"], row["name"])
            if identity in identities:
                continue
            identities.add(identity)
            rows.append(row)
    return rows


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
    by_allocation = defaultdict(list)
    for row in rows:
        by_allocation[(row["object"], int(row["rva"], 0))].append(row)
    for aliases in by_allocation.values():
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
            fold_fields = (
                "name", "size", "storage", "alignment", "section_offset",
                "scope")
            proved_fold = (
                row["object"] != previous_row["object"]
                and row["scope"] == "external"
                and "candidate-coff-folded-comdat" in row["provenance"]
                and "candidate-coff-folded-comdat" in previous_row["provenance"]
                and all(row[key] == previous_row[key] for key in fold_fields))
            if not (proved_alias or proved_fold):
                raise ValueError(
                    f"{label} duplicate RVA 0x{rva:x}: {identity} and {previous_identity}")
        else:
            rvas[rva] = (identity, row)
        intervals.append((rva, rva + size, identity))
    for previous, current in zip(sorted(intervals), sorted(intervals)[1:]):
        if previous[1] > current[0] and previous[0] != current[0]:
            raise ValueError(f"{label} overlapping allocations {previous} and {current}")
