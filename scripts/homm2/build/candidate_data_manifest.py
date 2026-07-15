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


def candidate_definitions(path, unit):
    """Return candidate-defined data symbols with extents from COFF topology."""
    coff = CoffFile(path)
    definitions = []
    for section in coff.sections:
        storage = DATA_SECTIONS.get(section.name)
        if storage is None:
            continue
        symbols = []
        for symbol in coff.symbols.values():
            if symbol.section != section.index or symbol.aux_count:
                continue
            typ, symbol_storage = _coff_symbol_fields(coff, symbol)
            if symbol.name == section.name or symbol_storage not in (2, 3) or typ != 0:
                continue
            symbols.append(symbol)
        by_offset = defaultdict(list)
        for symbol in symbols:
            by_offset[symbol.value].append(symbol)
        ambiguous = [offset for offset, values in by_offset.items() if len(values) != 1]
        if ambiguous:
            raise ValueError("candidate data aliases at %s:%s offsets %s" % (
                unit, section.name, ", ".join("0x%x" % value for value in ambiguous)))
        ordered = [values[0] for _offset, values in sorted(by_offset.items())]
        alignment = _section_alignment(_coff_section_characteristics(coff, section.index))
        for index, symbol in enumerate(ordered):
            end = ordered[index + 1].value if index + 1 < len(ordered) else section.raw_size
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
        offset = raw_offset(rva)
        if offset is None or offset + size > len(data):
            raise ValueError("PE RVA 0x%x has no %d-byte payload" % (rva, size))
        return bytes(data[offset:offset + size])

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


def _literal_rvas(row, coff, intervals, highlow, read_bytes, cache):
    if not (row["name"].startswith("$SG") or row["name"].startswith("??_C@")):
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


def derive_allocations(base_dir=REPO / "build/objdiff/base",
                       exe=REPO / "build/orig/HEROES2W.EXE",
                       symbols_path=REPO / "build/gen/symbol_names.csv",
                       units_path=REPO / "config/units.toml"):
    """Return allocations only for object/storage groups proved complete."""
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
            if len(candidate) != len(retail_sites):
                stats.rejected_functions += 1
                continue
            proposed = []
            known_anchors = 0
            valid = True
            for (_site, symbol, addend), retail_site in zip(candidate, retail_sites):
                target_rva = (read_u32(retail_site) - image_base) & 0xFFFFFFFF
                if symbol.name in defined:
                    proposed.append((symbol.name, (target_rva - addend) & 0xFFFFFFFF))
                elif symbol.name in public_data:
                    known_anchors += 1
                    if (public_data[symbol.name] + addend) & 0xFFFFFFFF != target_rva:
                        valid = False
                        break
            if not valid or (proposed and known_anchors == 0):
                stats.rejected_functions += 1
                continue
            stats.paired_functions += 1
            stats.aligned_dir32_sites += len(candidate)
            for symbol, rva in proposed:
                proofs[symbol].append(rva)

        for storage in sorted({row["storage"] for row in definitions}):
            source_group = [row for row in definitions if row["storage"] == storage]
            intervals = contributions.get((unit, storage), [])
            mapped, translation_causes, translation_details = _section_stream_translation(
                source_group, coff, intervals, public, image_base, highlow,
                read_u32, read_bytes)
            translated = mapped is not None and not translation_causes
            group, _section_bases, _stream_size = _layout_group(source_group, coff)
            if not translated:
                mapped = {}
                literal_cache = {}
                for row in group:
                    if row["name"] in public_data:
                        mapped[row["name"]] = (public_data[row["name"]], 1)
                        continue
                    unique = sorted(set(proofs.get(row["name"], [])))
                    if len(unique) == 1:
                        mapped[row["name"]] = (unique[0], len(proofs[row["name"]]))
                        continue
                    literal_rvas = _literal_rvas(
                        row, coff, intervals, highlow, read_bytes, literal_cache)
                    if len(literal_rvas) == 1:
                        mapped[row["name"]] = (literal_rvas[0], 1)
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
                        continue
                    values = proofs.get(row["name"], [])
                    unique = sorted(set(values))
                    if len(unique) != 1:
                        fail("unmapped_definition" if not unique else "ambiguous_mapping",
                             "%s maps to %s" % (row["name"], unique or "nothing"))
                        continue
                    rva = unique[0]
                    if not _contains(owner_intervals, rva, row["size"]):
                        fail("extent_escapes_contribution",
                             "%s extent 0x%x+0x%x escapes contribution" %
                             (row["name"], rva, row["size"]))
                        continue
                    mapped[row["name"]] = (rva, len(values))
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
                diagnostics.append(GroupDiagnostic(
                    unit, storage, tuple(sorted(causes)), tuple(failures),
                    tuple((rva, size, name) for rva, size, name in extents)))
                continue
            stats.closed_groups += 1
            stats.mapped_definitions += len(group)
            object_name = unit.replace("/", "\\") + ".c"
            for row in group:
                rva, proof_count = mapped[row["name"]]
                allocations.append(CandidateAllocation(
                    unit, object_name, row["name"], storage,
                    row["section_offset"], row["size"], row["alignment"], rva,
                    proof_count, row["scope"],
                    ("candidate-coff-section-translation" if translated
                     else "candidate-coff-reloc-bijection")))
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
    print("candidate data: %d/%d definitions in %d closed groups; %d open groups; "
          "%d aligned DIR32 sites across %d functions" % (
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
