"""Derive closed-world data allocations from candidate COFF topology and retail relocs.

Candidate objects prove symbol topology only: which symbols are definitions, their
section-relative offsets, sizes, alignment, and relocation spelling.  Retail PE bytes,
NB09 contribution ranges, and aligned code relocations remain the address authority.
An object/storage group is emitted only when every candidate definition maps through a
unique, consistent relocation proof (or an exact NB09 public-data address).
"""

import argparse
import csv
import json
import os
import struct
import tomllib
from collections import defaultdict
from dataclasses import asdict, dataclass
from pathlib import Path

from homm2.build.canonicalize_relocs import CoffFile
from homm2.build.contribution_manifest import contribution_rows


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
IMAGE_BASE = 0x400000
DIR32 = 0x0006
IMAGE_REL_BASED_HIGHLOW = 3
DATA_SECTIONS = {".rdata": "rdata", ".data": "data", ".bss": "bss"}
OUTPUT = REPO / "build/gen/candidate_delink_data.tsv"
DIAGNOSTICS_OUTPUT = REPO / "build/gen/candidate_data_diagnostics.json"


@dataclass(frozen=True)
class CandidateAllocation:
    unit: str
    object_name: str
    name: str
    storage: str
    section_offset: int
    size: int
    alignment: int
    rva: int
    proof_count: int
    scope: str
    provenance: str


@dataclass
class DerivationStats:
    candidate_definitions: int = 0
    evidenced_definitions: int = 0
    mapped_definitions: int = 0
    closed_groups: int = 0
    open_groups: int = 0
    paired_functions: int = 0
    rejected_functions: int = 0
    aligned_dir32_sites: int = 0


@dataclass(frozen=True)
class GroupDiagnostic:
    unit: str
    storage: str
    causes: tuple
    details: tuple
    evidence_ranges: tuple = ()
    proposed_allocations: tuple = ()

    @property
    def reason(self):
        return "; ".join(self.details)


def _section_alignment(characteristics):
    encoded = (characteristics >> 20) & 0xF
    if encoded == 0:
        return 1
    if not 1 <= encoded <= 14:
        raise ValueError("unsupported COFF section alignment encoding %d" % encoded)
    return 1 << (encoded - 1)


def _coff_section_characteristics(coff, section_index):
    optional_size = struct.unpack_from("<H", coff.data, 16)[0]
    offset = 20 + optional_size + (section_index - 1) * 40
    return struct.unpack_from("<I", coff.data, offset + 36)[0]


def _coff_symbol_fields(coff, symbol):
    typ, storage = struct.unpack_from("<HB", coff.data, symbol.offset + 14)
    return typ, storage


def _candidate_data_storage(section_name):
    if section_name.startswith(".CRT$"):
        return "data"
    return DATA_SECTIONS.get(section_name)


def candidate_definitions(path, unit):
    """Return candidate-defined data symbols with extents from COFF topology."""
    coff = CoffFile(path)
    definitions = []
    for section in coff.sections:
        storage = _candidate_data_storage(section.name)
        if storage is None:
            continue
        symbols = []
        for symbol in coff.symbols.values():
            if symbol.section != section.index or symbol.aux_count:
                continue
            typ, symbol_storage = _coff_symbol_fields(coff, symbol)
            if symbol.name == section.name or symbol_storage not in (2, 3) or typ != 0:
                continue
            symbols.append((symbol, symbol_storage))
        by_offset = defaultdict(list)
        for symbol, symbol_storage in symbols:
            by_offset[symbol.value].append((symbol, symbol_storage))
        ambiguous = [offset for offset, values in by_offset.items() if len(values) != 1]
        if ambiguous:
            raise ValueError("candidate data aliases at %s:%s offsets %s" % (
                unit, section.name, ", ".join("0x%x" % value for value in ambiguous)))
        ordered = [values[0] for _offset, values in sorted(by_offset.items())]
        alignment = _section_alignment(_coff_section_characteristics(coff, section.index))
        for index, (symbol, symbol_storage) in enumerate(ordered):
            end = (ordered[index + 1][0].value
                   if index + 1 < len(ordered) else section.raw_size)
            if not symbol.value <= end <= section.raw_size or end == symbol.value:
                raise ValueError("invalid candidate data extent for %s in %s" %
                                 (symbol.name, unit))
            definitions.append({
                "unit": unit,
                "name": symbol.name,
                "storage": storage,
                "section": section.index,
                "section_offset": symbol.value,
                "size": end - symbol.value,
                "alignment": (alignment if symbol.value == 0 else
                              min(alignment, symbol.value & -symbol.value)),
                "scope": "external" if symbol_storage == 2 else "local",
            })
    return definitions, coff


def _reviewed_candidate_allocation(unit, storage, candidate, reviewed):
    """Validate and return one reviewed candidate allocation when present."""
    object_name = unit.replace("/", "\\") + ".c"
    row = reviewed.get((object_name, candidate["name"]))
    if row is None:
        return None
    logical_size = int(row["size"], 0)
    expected = {
        "storage": storage,
        "alignment": candidate["alignment"],
        "section_ordinal": candidate["section"],
        "section_offset": candidate["section_offset"],
        "scope": candidate["scope"],
    }
    actual = {
        "storage": row["storage"],
        "alignment": int(row["alignment"], 0),
        "section_ordinal": int(row["section_ordinal"], 0),
        "section_offset": int(row["section_offset"], 0),
        "scope": row["scope"],
    }
    if actual != expected or not 0 < logical_size <= candidate["size"]:
        mismatches = {
            key: {"reviewed": actual[key], "candidate": value}
            for key, value in expected.items() if actual[key] != value
        }
        if not 0 < logical_size <= candidate["size"]:
            mismatches["size"] = {
                "reviewed": logical_size, "candidate_span": candidate["size"]}
        raise ValueError(
            "reviewed candidate topology mismatch for %s:%s: %s" %
            (unit, candidate["name"], mismatches))
    return CandidateAllocation(
        unit, object_name, candidate["name"], storage,
        candidate["section_offset"], logical_size, candidate["alignment"],
        int(row["rva"], 0), 1, candidate["scope"], row["provenance"])


def _reviewed_group_allocations(unit, storage, definitions, reviewed):
    """Return a group only when every candidate definition is reviewed exactly."""
    rows = []
    for candidate in definitions:
        allocation = _reviewed_candidate_allocation(
            unit, storage, candidate, reviewed)
        if allocation is None:
            return None
        rows.append(allocation)
    extents = sorted((row.rva, row.rva + row.size, row.name) for row in rows)
    for previous, current in zip(extents, extents[1:]):
        if previous[1] > current[0]:
            raise ValueError(
                "reviewed candidate allocations overlap: %s and %s" %
                (previous[2], current[2]))
    return rows


def _validate_constrained_reviewed_group(unit, storage, definitions, coff,
                                         allocations, intervals, read_bytes):
    """Validate a complete remaining-slot/equivalence-class reconstruction."""
    constrained = [
        row for row in allocations if row.provenance.startswith((
            "candidate-coff-remaining-slot-bijection",
            "candidate-coff-equivalence-class:",
        ))
    ]
    if not constrained:
        return
    merged = _merge_adjacent_intervals(intervals)
    if len(merged) != 1:
        raise ValueError(
            "constrained reviewed group %s:%s requires one contiguous retail interval" %
            (unit, storage))
    start, end = merged[0]
    by_name = {row["name"]: row for row in definitions}
    covered = bytearray(end - start)
    payloads = {}
    for allocation in allocations:
        if not start <= allocation.rva < allocation.rva + allocation.size <= end:
            raise ValueError(
                "constrained reviewed allocation %s:%s escapes 0x%x..0x%x" %
                (unit, allocation.name, start, end))
        first = allocation.rva - start
        last = first + allocation.size
        if any(covered[first:last]):
            raise ValueError(
                "constrained reviewed allocation %s:%s overlaps another owner" %
                (unit, allocation.name))
        covered[first:last] = b"\1" * allocation.size
        candidate = by_name[allocation.name]
        section = coff.sections[candidate["section"] - 1]
        offset = section.raw_offset + candidate["section_offset"]
        payload = bytes(coff.data[offset:offset + allocation.size])
        payloads[allocation.name] = payload
        if storage != "bss" and payload != read_bytes(
                allocation.rva, allocation.size):
            raise ValueError(
                "constrained reviewed allocation %s:%s payload differs at 0x%x" %
                (unit, allocation.name, allocation.rva))
    if storage != "bss":
        retail = read_bytes(start, end - start)
        nonzero_gap = next((index for index, value in enumerate(retail)
                            if not covered[index] and value), None)
        if nonzero_gap is not None:
            raise ValueError(
                "constrained reviewed group %s:%s leaves nonzero retail byte 0x%x uncovered" %
                (unit, storage, start + nonzero_gap))
    equivalence = defaultdict(list)
    for allocation in allocations:
        if allocation.provenance.startswith("candidate-coff-equivalence-class:"):
            equivalence[allocation.provenance].append(allocation)
    for provenance, members in equivalence.items():
        if len(members) < 2:
            raise ValueError("equivalence class %s has fewer than two owners" % provenance)
        identities = {(row.size, payloads[row.name]) for row in members}
        if len(identities) != 1:
            raise ValueError(
                "equivalence class %s does not have identical logical payloads" %
                provenance)


def _virtual_section_bytes(data, sections, rva, size):
    """Read PE section payload using loader zero-fill semantics."""
    for start, span, raw_size, raw in sections:
        if start <= rva and rva + size <= start + span:
            delta = rva - start
            raw_count = max(0, min(size, raw_size - delta))
            payload = bytes(data[raw + delta:raw + delta + raw_count])
            return payload + b"\0" * (size - raw_count)
    raise ValueError("PE RVA 0x%x has no %d-byte virtual payload" % (rva, size))


def _pe_layout(path):
    data = Path(path).read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe:pe + 4] != b"PE\0\0":
        raise ValueError("not a PE file: %s" % path)
    coff = pe + 4
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    image_base = struct.unpack_from("<I", data, optional + 28)[0]
    sections = []
    first = optional + optional_size
    for index in range(section_count):
        offset = first + index * 40
        virtual_size, rva, raw_size, raw_offset = struct.unpack_from(
            "<IIII", data, offset + 8)
        sections.append((rva, max(virtual_size, raw_size), raw_size, raw_offset))

    def raw_offset(rva):
        for start, span, raw_size, raw in sections:
            if start <= rva < start + span:
                delta = rva - start
                return None if delta >= raw_size else raw + delta
        return None

    directory = optional + 96 + 5 * 8
    reloc_rva, reloc_size = struct.unpack_from("<II", data, directory)
    cursor = raw_offset(reloc_rva)
    if cursor is None:
        raise ValueError("PE has no readable base-relocation directory")
    end = cursor + reloc_size
    highlow = []
    while cursor + 8 <= end:
        page, block_size = struct.unpack_from("<II", data, cursor)
        if block_size < 8 or cursor + block_size > end:
            raise ValueError("invalid PE base-relocation block")
        for offset in range(cursor + 8, cursor + block_size, 2):
            entry = struct.unpack_from("<H", data, offset)[0]
            if entry >> 12 == IMAGE_REL_BASED_HIGHLOW:
                highlow.append(page + (entry & 0xFFF))
        cursor += block_size

    def read_u32(rva):
        offset = raw_offset(rva)
        if offset is None or offset + 4 > len(data):
            raise ValueError("PE RVA 0x%x has no four-byte payload" % rva)
        return struct.unpack_from("<I", data, offset)[0]

    def read_bytes(rva, size):
        return _virtual_section_bytes(data, sections, rva, size)

    return image_base, sorted(highlow), read_u32, read_bytes


def _symbol_inventory(path):
    public_data = {}
    public = {}
    functions = defaultdict(list)
    with Path(path).open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "data" and row.get("provenance") == "cv-public-data":
                rva = int(row["rva"], 0)
                public_data[row["name"]] = rva
                public[row["name"]] = rva
            elif row.get("kind") == "func":
                rva = int(row["rva"], 0)
                public[row["name"]] = rva
                functions[row["unit"]].append({
                    "name": row["name"],
                    "rva": rva,
                    "size": int(row.get("size") or "0", 0),
                })
    return public, public_data, functions


def _function_dir32(coff, function_name):
    functions = coff.unique_text_functions()
    function = functions.get(function_name)
    if function is None:
        return None
    peers = sorted(symbol.value for symbol in functions.values()
                   if symbol.section == function.section and symbol.value > function.value)
    section = coff.sections[function.section - 1]
    end = peers[0] if peers else section.raw_size
    rows = []
    for (section_index, site), relocation in sorted(coff.relocations.items()):
        if (section_index != function.section or relocation.typ != DIR32 or
                not function.value <= site < end):
            continue
        symbol = coff.symbols.get(relocation.symbol_index)
        if symbol is None:
            raise ValueError("relocation references an auxiliary COFF symbol")
        addend = struct.unpack_from(
            "<I", coff.data, section.raw_offset + site)[0]
        rows.append((site - function.value, symbol, addend))
    return rows


def _function_relocation_offsets_align(candidate, function_rva, retail_sites):
    """Return whether candidate and retail DIR32 sites have identical offsets."""
    return ([site for site, _symbol, _addend in candidate] ==
            [site - function_rva for site in retail_sites])


def _function_relocation_proofs(candidate, function_rva, retail_sites,
                                read_u32, image_base, defined, public):
    """Pair only identical function-relative relocation sites."""
    offsets_align = _function_relocation_offsets_align(
        candidate, function_rva, retail_sites)
    retail_by_offset = {site - function_rva: site for site in retail_sites}
    proposed = []
    known_anchors = 0
    paired_sites = 0
    valid = True
    for site, symbol, addend in candidate:
        retail_site = retail_by_offset.get(site)
        if retail_site is None:
            continue
        paired_sites += 1
        target_rva = (read_u32(retail_site) - image_base) & 0xFFFFFFFF
        if symbol.name in defined:
            proposed.append((symbol.name, (target_rva - addend) & 0xFFFFFFFF))
        elif symbol.name in public:
            if (public[symbol.name] + addend) & 0xFFFFFFFF == target_rva:
                known_anchors += 1
    return proposed, known_anchors, paired_sites, offsets_align, valid


def _function_sequence_relocation_proofs(candidate, retail_sites, read_u32,
                                         image_base, defined, public):
    """Pair equal relocation sequences after validating every known target."""
    if len(candidate) != len(retail_sites):
        return [], 0, False
    proposed = []
    known_anchors = 0
    for (_site, symbol, addend), retail_site in zip(candidate, retail_sites):
        target_rva = (read_u32(retail_site) - image_base) & 0xFFFFFFFF
        if symbol.name in defined:
            proposed.append((symbol.name, (target_rva - addend) & 0xFFFFFFFF))
        elif symbol.name in public:
            known_anchors += 1
            if (public[symbol.name] + addend) & 0xFFFFFFFF != target_rva:
                return [], known_anchors, False
    return proposed, known_anchors, True


def _contribution_index(exe, units):
    result = defaultdict(list)
    for row in contribution_rows(exe, units):
        if row["storage"] != "text":
            unit = row["object"].replace("\\", "/").removesuffix(".c")
            result[(unit, row["storage"])].append((row["rva"], row["rva"] + row["size"]))
    return result


def _contains(intervals, rva, size=1):
    return any(start <= rva and rva + size <= end for start, end in intervals)


def _align_up(value, alignment):
    return (value + alignment - 1) & -alignment


def _merge_adjacent_intervals(intervals):
    merged = []
    for start, end in sorted(intervals):
        if merged and merged[-1][1] == start:
            merged[-1] = (merged[-1][0], end)
        else:
            merged.append((start, end))
    return merged


def _layout_group(group, coff):
    """Replay candidate same-class sections into one aligned output stream."""
    bases = {}
    cursor = 0
    for section_index in sorted({row["section"] for row in group}):
        section = coff.sections[section_index - 1]
        alignment = _section_alignment(
            _coff_section_characteristics(coff, section_index))
        cursor = _align_up(cursor, alignment)
        bases[section_index] = cursor
        cursor += section.raw_size
    laid_out = []
    for row in group:
        translated = dict(row)
        translated["symbol_offset"] = row["section_offset"]
        translated["section_offset"] = bases[row["section"]] + row["section_offset"]
        laid_out.append(translated)
    return laid_out, bases, cursor


def _section_stream_translation(group, coff, intervals, public, image_base,
                                highlow, read_u32, read_bytes):
    """Translate the candidate section stream into its retail contribution."""
    intervals = _merge_adjacent_intervals(intervals)
    if len(intervals) != 1:
        return None, ("section_translation_shape",), (
            "section stream requires one contiguous retail interval",)
    start, end = intervals[0]
    group, section_bases, stream_size = _layout_group(group, coff)
    causes = set()
    details = []

    def fail(cause, detail):
        causes.add(cause)
        details.append(detail)

    if stream_size > end - start:
        fail("section_translation_size",
             "candidate section stream size 0x%x differs from retail contribution 0x%x" %
             (stream_size, end - start))
    elif stream_size < end - start and group[0]["storage"] != "bss":
        tail = read_bytes(start + stream_size, end - start - stream_size)
        if any(tail):
            fail("section_translation_tail_payload",
                 "retail contribution has nonzero payload after candidate stream offset 0x%x" %
                 stream_size)
    for section_index in section_bases:
        rows = [row for row in group if row["section"] == section_index]
        if min(row["symbol_offset"] for row in rows) != 0:
            fail("section_translation_leading_bytes",
                 "candidate section %d has unmodeled leading bytes" % section_index)
    for row in group:
        expected = start + row["section_offset"]
        public_rva = public.get(row["name"])
        if public_rva is not None and public_rva != expected:
            fail("section_translation_public_anchor",
                 "public %s is 0x%x, translated RVA is 0x%x" %
                 (row["name"], public_rva, expected))

    candidate_relocs = []
    for (reloc_section, site), relocation in sorted(coff.relocations.items()):
        if reloc_section not in section_bases:
            continue
        symbol = coff.symbols.get(relocation.symbol_index)
        if symbol is None:
            fail("section_translation_relocation",
                 "candidate relocation references an auxiliary symbol")
            continue
        if relocation.typ != DIR32:
            fail("section_translation_relocation_type",
                 "candidate section uses unsupported relocation type 0x%x" % relocation.typ)
            continue
        section = coff.sections[reloc_section - 1]
        addend = struct.unpack_from("<I", coff.data, section.raw_offset + site)[0]
        candidate_relocs.append(
            (section_bases[reloc_section] + site, symbol, addend))
    retail_sites = [site - start for site in highlow if start <= site < end]
    candidate_sites = [site for site, _symbol, _addend in candidate_relocs]
    if candidate_sites != retail_sites:
        fail("section_translation_relocation_sites",
             "candidate DIR32 sites %s differ from retail HIGHLOW sites %s" %
             (candidate_sites, retail_sites))

    if group[0]["storage"] != "bss" and stream_size <= end - start:
        candidate_bytes = bytearray(stream_size)
        for section_index, section_base in section_bases.items():
            section = coff.sections[section_index - 1]
            candidate_bytes[section_base:section_base + section.raw_size] = \
                coff.data[section.raw_offset:section.raw_offset + section.raw_size]
        retail_bytes = read_bytes(start, stream_size)
        masked = set()
        for site in candidate_sites:
            masked.update(range(site, site + 4))
        for row in group:
            if not (row["name"].startswith("$SG") or
                    row["name"].startswith("??_C@")):
                continue
            first = row["section_offset"]
            last = first + row["size"]
            mismatch = next((offset for offset in range(first, last)
                             if offset not in masked and
                             candidate_bytes[offset] != retail_bytes[offset]), None)
            if mismatch is not None:
                fail("section_translation_literal_payload",
                     "%s payload differs at stream offset 0x%x" %
                     (row["name"], mismatch))

    for site, symbol, addend in candidate_relocs:
        if start + site + 4 > end:
            continue
        retail_target = (read_u32(start + site) - image_base) & 0xFFFFFFFF
        if symbol.section in section_bases:
            expected_target = (start + section_bases[symbol.section] +
                               symbol.value + addend) & 0xFFFFFFFF
        elif symbol.name in public:
            expected_target = (public[symbol.name] + addend) & 0xFFFFFFFF
        else:
            fail("section_translation_relocation_target",
                 "candidate relocation at 0x%x has unproved target %s" %
                 (site, symbol.name))
            continue
        if retail_target != expected_target:
            fail("section_translation_relocation_target",
                 "candidate relocation at 0x%x targets 0x%x, retail targets 0x%x" %
                 (site, expected_target, retail_target))

    mapped = {row["name"]: (start + row["section_offset"], 1) for row in group}
    return mapped, tuple(sorted(causes)), tuple(details)


def _payload_rvas(row, coff, intervals, highlow, read_bytes, cache):
    """Return exact payload occurrences for a relocation-free allocation."""
    if row["storage"] == "bss":
        return []
    section = coff.sections[row["section"] - 1]
    first = row["symbol_offset"]
    last = first + row["size"]
    if any(reloc_section == row["section"] and first <= site < last
           for reloc_section, site in coff.relocations):
        return []
    needle = bytes(coff.data[section.raw_offset + first:section.raw_offset + last])
    if not needle:
        return []
    if needle in cache:
        return cache[needle]
    matches = []
    for start, end in intervals:
        haystack = read_bytes(start, end - start)
        offset = haystack.find(needle)
        while offset >= 0:
            rva = start + offset
            if not any(rva <= site < rva + len(needle) for site in highlow):
                matches.append(rva)
                if len(matches) > 1:
                    cache[needle] = matches
                    return matches
            offset = haystack.find(needle, offset + 1)
    result = sorted(set(matches))
    cache[needle] = result
    return result


def _payload_matches_at(row, coff, rva, highlow, read_bytes):
    """Check a relocation-free initialized allocation against retail bytes."""
    if row["storage"] == "bss":
        return True
    first = row["symbol_offset"]
    last = first + row["size"]
    if any(reloc_section == row["section"] and first <= site < last
           for reloc_section, site in coff.relocations):
        return True
    section = coff.sections[row["section"] - 1]
    candidate = bytes(
        coff.data[section.raw_offset + first:section.raw_offset + last])
    return candidate == read_bytes(rva, row["size"])


def _allocation_has_relocation(row, coff):
    first = row["symbol_offset"]
    last = first + row["size"]
    return any(
        section == row["section"] and first <= site < last
        for section, site in coff.relocations
    )


def _bounded_section_delta_proofs(group, mapped, evidence, coff, intervals,
                                  highlow, read_bytes):
    """Map relocation-free runs bracketed by one proved section translation.

    Reconstructed source owners can move relative to compiler-generated literals,
    so a whole candidate section is not always a valid retail translation.  A run
    between two already-proved owners is still exact when both endpoints imply the
    same retail-minus-candidate delta and every projected payload byte agrees.
    """
    index = 0
    while index < len(group):
        if group[index]["name"] in mapped:
            index += 1
            continue
        first = index
        while index < len(group) and group[index]["name"] not in mapped:
            index += 1
        last = index
        if first == 0 or last == len(group):
            continue
        before = group[first - 1]
        after = group[last]
        before_delta = mapped[before["name"]][0] - before["section_offset"]
        after_delta = mapped[after["name"]][0] - after["section_offset"]
        if before_delta != after_delta:
            continue

        projected = []
        for row in group[first:last]:
            rva = before_delta + row["section_offset"]
            if (_allocation_has_relocation(row, coff)
                    or any(rva <= site < rva + row["size"] for site in highlow)
                    or not _contains(intervals, rva, row["size"])
                    or not _payload_matches_at(
                        row, coff, rva, highlow, read_bytes)):
                projected = []
                break
            projected.append((row, rva))
        for row, rva in projected:
            mapped[row["name"]] = (rva, 2)
            evidence[row["name"]] = "bounded-section-delta"


def _literal_rvas(row, coff, intervals, highlow, read_bytes, cache):
    if not (row["name"].startswith("$SG") or row["name"].startswith("??_C@")):
        return []
    return _payload_rvas(row, coff, intervals, highlow, read_bytes, cache)


def derive_allocations(base_dir=REPO / "build/objdiff/base",
                       exe=REPO / "build/orig/HEROES2W.EXE",
                       symbols_path=REPO / "build/gen/symbol_names.csv",
                       units_path=REPO / "config/units.toml",
                       reviewed_rows=()):
    """Return allocations only for object/storage groups proved complete."""
    reviewed = {}
    for row in reviewed_rows:
        key = (row["object"], row["name"])
        if key in reviewed:
            raise ValueError("duplicate reviewed candidate identity %s:%s" % key)
        reviewed[key] = row
    public, public_data, functions = _symbol_inventory(symbols_path)
    image_base, highlow, read_u32, read_bytes = _pe_layout(exe)
    referenced_targets = {
        (read_u32(site) - image_base) & 0xFFFFFFFF for site in highlow
    }
    contributions = _contribution_index(exe, units_path)
    storage_intervals = defaultdict(list)
    for (_unit, storage), intervals in contributions.items():
        storage_intervals[storage].extend(intervals)
    units = tomllib.loads(Path(units_path).read_text()).get("unit", [])
    allocations = []
    diagnostics = []
    stats = DerivationStats()
    for unit_row in units:
        unit = unit_row["unit"]
        path = Path(base_dir) / (unit + ".obj")
        if not path.is_file():
            diagnostics.append(GroupDiagnostic(
                unit, "*", ("candidate_object_missing",),
                ("candidate object missing",)))
            continue
        definitions, coff = candidate_definitions(path, unit)
        stats.candidate_definitions += len(definitions)
        defined = {row["name"]: row for row in definitions}
        proofs = defaultdict(list)
        for name, row in defined.items():
            if name in public_data:
                proofs[name].append(public_data[name])

        for function in functions.get(unit, []):
            candidate = _function_dir32(coff, function["name"])
            if candidate is None:
                continue
            retail_sites = [rva for rva in highlow
                            if function["rva"] <= rva < function["rva"] + function["size"]]
            proposed, known_anchors, paired_sites, offsets_align, valid = \
                _function_relocation_proofs(
                    candidate, function["rva"], retail_sites, read_u32,
                    image_base, defined, public)
            if valid and not offsets_align:
                sequence, sequence_anchors, sequence_valid = \
                    _function_sequence_relocation_proofs(
                        candidate, retail_sites, read_u32, image_base,
                        defined, public)
                if sequence_valid and sequence_anchors:
                    proposed.extend(sequence)
                    known_anchors += sequence_anchors
            if not valid or (proposed and known_anchors == 0 and not offsets_align):
                stats.rejected_functions += 1
                continue
            stats.paired_functions += 1
            stats.aligned_dir32_sites += paired_sites
            for symbol, rva in proposed:
                proofs[symbol].append(rva)

        for storage in sorted({row["storage"] for row in definitions}):
            source_group = [row for row in definitions if row["storage"] == storage]
            intervals = contributions.get((unit, storage), [])
            reviewed_group = _reviewed_group_allocations(
                unit, storage, source_group, reviewed)
            if reviewed_group is not None:
                _validate_constrained_reviewed_group(
                    unit, storage, source_group, coff, reviewed_group,
                    intervals, read_bytes)
                allocations.extend(reviewed_group)
                stats.closed_groups += 1
                stats.evidenced_definitions += len(reviewed_group)
                stats.mapped_definitions += len(reviewed_group)
                continue
            partial_reviewed = {}
            for source_row in source_group:
                allocation = _reviewed_candidate_allocation(
                    unit, storage, source_row, reviewed)
                if allocation is not None:
                    partial_reviewed[source_row["name"]] = allocation
            analysis_group = []
            for source_row in source_group:
                row = dict(source_row)
                if source_row["name"] in partial_reviewed:
                    row["size"] = partial_reviewed[source_row["name"]].size
                analysis_group.append(row)
            if partial_reviewed:
                mapped, translation_causes, translation_details = (
                    None, ("partial_reviewed_group",),
                    ("section replay disabled by reviewed allocation anchors",))
            else:
                mapped, translation_causes, translation_details = \
                    _section_stream_translation(
                        analysis_group, coff, intervals, public, image_base,
                        highlow, read_u32, read_bytes)
            replay_mapped = dict(mapped or {})
            translated = mapped is not None and not translation_causes
            evidence = ({row["name"]: "candidate-section-replay" for row in analysis_group}
                        if mapped is not None else {})
            group, _section_bases, _stream_size = _layout_group(analysis_group, coff)
            if not translated:
                mapped = {}
                evidence = {}
                literal_cache = {}
                for row in group:
                    reviewed_allocation = partial_reviewed.get(row["name"])
                    if reviewed_allocation is not None:
                        mapped[row["name"]] = (reviewed_allocation.rva, 1)
                        evidence[row["name"]] = reviewed_allocation.provenance
                        continue
                    if row["name"] in public_data:
                        mapped[row["name"]] = (public_data[row["name"]], 1)
                        evidence[row["name"]] = "retail-public-rva"
                        continue
                    unique = sorted(set(proofs.get(row["name"], [])))
                    if len(unique) == 1:
                        if _payload_matches_at(
                                row, coff, unique[0], highlow, read_bytes):
                            mapped[row["name"]] = (
                                unique[0], len(proofs[row["name"]]))
                            evidence[row["name"]] = "aligned-relocation-addend"
                            continue
                    literal_rvas = _literal_rvas(
                        row, coff, intervals, highlow, read_bytes, literal_cache)
                    if len(literal_rvas) == 1:
                        mapped[row["name"]] = (literal_rvas[0], 1)
                        evidence[row["name"]] = "unique-literal-payload"
                _bounded_section_delta_proofs(
                    group, mapped, evidence, coff, intervals,
                    highlow, read_bytes)
            mapped = mapped or {}
            failures = []
            causes = set()

            def fail(cause, detail):
                causes.add(cause)
                failures.append(detail)

            if translated:
                causes.update(translation_causes)
                failures.extend(translation_details)
            if not translated:
                owner_intervals = intervals or storage_intervals[storage]
                for row in group:
                    if row["name"] in mapped:
                        rva = mapped[row["name"]][0]
                        if not _contains(owner_intervals, rva, row["size"]):
                            fail("extent_escapes_contribution",
                                 "%s extent 0x%x+0x%x escapes retail storage" %
                                 (row["name"], rva, row["size"]))
                            del mapped[row["name"]]
                            evidence.pop(row["name"], None)
                        continue
                    values = proofs.get(row["name"], [])
                    unique = sorted(set(values))
                    if len(unique) != 1:
                        fail("unmapped_definition" if not unique else "ambiguous_mapping",
                             "%s maps to %s" % (row["name"], unique or "nothing"))
                        continue
                    rva = unique[0]
                    if not _payload_matches_at(
                            row, coff, rva, highlow, read_bytes):
                        fail("relocation_payload_mismatch",
                             "%s relocation proof at 0x%x contradicts payload" %
                             (row["name"], rva))
                        continue
                    if not _contains(owner_intervals, rva, row["size"]):
                        fail("extent_escapes_contribution",
                             "%s extent 0x%x+0x%x escapes contribution" %
                             (row["name"], rva, row["size"]))
                        continue
                    mapped[row["name"]] = (rva, len(values))
                    evidence[row["name"]] = "aligned-relocation-addend"
            if not intervals:
                fail("retail_contribution_missing", "retail contribution is missing")
            starts = [value[0] for value in mapped.values()]
            if len(starts) != len(set(starts)):
                fail("non_bijective_mapping",
                     "multiple candidate definitions map to one retail RVA")
            extents = sorted((mapped[row["name"]][0], row["size"], row["name"])
                             for row in group if row["name"] in mapped)
            for previous, current in zip(extents, extents[1:]):
                if previous[0] + previous[1] > current[0]:
                    fail("overlapping_extents",
                         "mapped retail extents overlap: %s and %s" %
                         (previous[2], current[2]))
            coverage_intervals = intervals
            if not coverage_intervals and mapped:
                coverage_intervals = [(
                    min(value[0] for value in mapped.values()),
                    max(mapped[row["name"]][0] + row["size"]
                        for row in group if row["name"] in mapped))]
            uncovered = sorted(
                target for target in referenced_targets
                if _contains(coverage_intervals, target) and
                not any(rva <= target < rva + size for rva, size, _name in extents))
            if uncovered:
                fail("uncovered_retail_reference",
                     "referenced retail RVAs lack candidate allocations: %s" %
                     ", ".join("0x%x" % value for value in uncovered[:8]))
            uncovered_public = sorted(
                (name, rva) for name, rva in public_data.items()
                if _contains(coverage_intervals, rva) and
                not any(start <= rva < start + size
                        for start, size, _name in extents))
            if uncovered_public:
                fail("uncovered_public_definition",
                     "retail public definitions lack candidate allocations: %s" %
                     ", ".join("%s=0x%x" % value
                               for value in uncovered_public[:8]))
            if failures:
                if not mapped:
                    causes.update(translation_causes)
                    failures[:0] = translation_details
                stats.open_groups += 1
                object_name = unit.replace("/", "\\") + ".c"
                replay_payload_cache = {}
                proposal_mappings = {}
                for row in group:
                    value = replay_mapped.get(row["name"])
                    if value is None or not _contains(intervals, value[0], row["size"]):
                        continue
                    payload_rvas = _payload_rvas(
                        row, coff, intervals, highlow, read_bytes,
                        replay_payload_cache)
                    if payload_rvas == [value[0]]:
                        proposal_mappings[row["name"]] = value
                proposal_evidence = {
                    name: "candidate-section-replay" for name in proposal_mappings
                }
                proposal_mappings.update(mapped)
                proposal_evidence.update(evidence)
                proposed = tuple(CandidateAllocation(
                    unit, object_name, row["name"], storage,
                    row["section_offset"], row["size"], row["alignment"],
                    proposal_mappings[row["name"]][0],
                    proposal_mappings[row["name"]][1], row["scope"],
                    proposal_evidence[row["name"]])
                    for row in group if row["name"] in proposal_mappings)
                stats.evidenced_definitions += len(proposed)
                diagnostics.append(GroupDiagnostic(
                    unit, storage, tuple(sorted(causes)), tuple(failures),
                    tuple((rva, size, name) for rva, size, name in extents),
                    proposed))
                continue
            stats.closed_groups += 1
            stats.evidenced_definitions += len(group)
            stats.mapped_definitions += len(group)
            object_name = unit.replace("/", "\\") + ".c"
            for row in group:
                rva, proof_count = mapped[row["name"]]
                allocations.append(CandidateAllocation(
                    unit, object_name, row["name"], storage,
                    row["section_offset"], row["size"], row["alignment"], rva,
                    proof_count, row["scope"],
                    ("candidate-coff-section-translation" if translated
                     else evidence[row["name"]])))
    return allocations, stats, diagnostics


def manifest_bytes(**kwargs):
    allocations, stats, diagnostics = derive_allocations(**kwargs)
    lines = [
        "# Closed candidate COFF allocation groups proved against retail relocations.",
        "name\tobject\trva\tsize\tstorage\talignment\tsection_offset\tscope\tprovenance",
    ]
    for row in sorted(allocations, key=lambda value: (
            value.object_name, value.storage, value.section_offset, value.name)):
        lines.append("%s\t%s\t0x%x\t0x%x\t%s\t0x%x\t0x%x\t%s\t%s" % (
            row.name, row.object_name, row.rva, row.size, row.storage,
            row.alignment, row.section_offset, row.scope, row.provenance))
    return ("\n".join(lines) + "\n").encode(), stats, diagnostics


def diagnostics_bytes(stats, diagnostics):
    cause_counts = defaultdict(int)
    storage_counts = defaultdict(int)
    for diagnostic in diagnostics:
        storage_counts[diagnostic.storage] += 1
        for cause in diagnostic.causes:
            cause_counts[cause] += 1
    payload = {
        "schema": 1,
        "stats": asdict(stats),
        "open_by_storage": dict(sorted(storage_counts.items())),
        "open_by_cause": dict(sorted(cause_counts.items())),
        "groups": [asdict(row) for row in diagnostics],
    }
    return (json.dumps(payload, indent=2, sort_keys=True) + "\n").encode()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--base-dir", type=Path, default=REPO / "build/objdiff/base")
    parser.add_argument("--exe", type=Path, default=REPO / "build/orig/HEROES2W.EXE")
    parser.add_argument("--symbols", type=Path, default=REPO / "build/gen/symbol_names.csv")
    parser.add_argument("--units", type=Path, default=REPO / "config/units.toml")
    parser.add_argument("--output", type=Path, default=OUTPUT)
    parser.add_argument("--diagnostics-output", type=Path, default=DIAGNOSTICS_OUTPUT)
    parser.add_argument(
        "--require-all", action="store_true",
        help="fail without writing output unless every data-bearing group is closed")
    args = parser.parse_args(argv)
    payload, stats, diagnostics = manifest_bytes(
        base_dir=args.base_dir, exe=args.exe, symbols_path=args.symbols,
        units_path=args.units)
    args.diagnostics_output.parent.mkdir(parents=True, exist_ok=True)
    args.diagnostics_output.write_bytes(diagnostics_bytes(stats, diagnostics))
    print("candidate data: %d/%d individually evidenced; %d/%d definitions in "
          "%d closed groups; %d open groups; %d aligned DIR32 sites across "
          "%d functions" % (
              stats.evidenced_definitions, stats.candidate_definitions,
              stats.mapped_definitions, stats.candidate_definitions,
              stats.closed_groups, stats.open_groups,
              stats.aligned_dir32_sites, stats.paired_functions))
    for diagnostic in diagnostics:
        print("  OPEN %s %s: %s" % (
            diagnostic.unit, diagnostic.storage, diagnostic.reason))
    if args.require_all and diagnostics:
        print("candidate data: refusing partial manifest in --require-all mode")
        return 1
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(payload)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
