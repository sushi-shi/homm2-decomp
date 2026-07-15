"""Derive closed-world data allocations from candidate COFF topology and retail relocs.

Candidate objects prove symbol topology only: which symbols are definitions, their
section-relative offsets, sizes, alignment, and relocation spelling.  Retail PE bytes,
NB09 contribution ranges, and aligned code relocations remain the address authority.
An object/storage group is emitted only when every candidate definition maps through a
unique, consistent relocation proof (or an exact NB09 public-data address).
"""

import argparse
import csv
import os
import struct
import tomllib
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

from homm2.build.canonicalize_relocs import CoffFile
from homm2.build.contribution_manifest import contribution_rows


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
IMAGE_BASE = 0x400000
DIR32 = 0x0006
IMAGE_REL_BASED_HIGHLOW = 3
DATA_SECTIONS = {".rdata": "rdata", ".data": "data", ".bss": "bss"}
OUTPUT = REPO / "build/gen/candidate_delink_data.tsv"


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

    return image_base, sorted(highlow), read_u32


def _symbol_inventory(path):
    public = {}
    functions = defaultdict(list)
    with Path(path).open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "data" and row.get("provenance") == "cv-public-data":
                public[row["name"]] = int(row["rva"], 0)
            elif row.get("kind") == "func":
                functions[row["unit"]].append({
                    "name": row["name"],
                    "rva": int(row["rva"], 0),
                    "size": int(row.get("size") or "0", 0),
                })
    return public, functions


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


def derive_allocations(base_dir=REPO / "build/objdiff/base",
                       exe=REPO / "build/orig/HEROES2W.EXE",
                       symbols_path=REPO / "build/gen/symbol_names.csv",
                       units_path=REPO / "config/units.toml"):
    """Return allocations only for object/storage groups proved complete."""
    public, functions = _symbol_inventory(symbols_path)
    image_base, highlow, read_u32 = _pe_layout(exe)
    referenced_targets = {
        (read_u32(site) - image_base) & 0xFFFFFFFF for site in highlow
    }
    contributions = _contribution_index(exe, units_path)
    units = tomllib.loads(Path(units_path).read_text()).get("unit", [])
    allocations = []
    diagnostics = []
    stats = DerivationStats()
    for unit_row in units:
        unit = unit_row["unit"]
        path = Path(base_dir) / (unit + ".obj")
        if not path.is_file():
            diagnostics.append((unit, "*", "candidate object missing"))
            continue
        definitions, coff = candidate_definitions(path, unit)
        stats.candidate_definitions += len(definitions)
        defined = {row["name"]: row for row in definitions}
        proofs = defaultdict(list)
        for name, row in defined.items():
            if name in public:
                proofs[name].append(public[name])

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
                elif symbol.name in public:
                    known_anchors += 1
                    if (public[symbol.name] + addend) & 0xFFFFFFFF != target_rva:
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
            group = [row for row in definitions if row["storage"] == storage]
            intervals = contributions.get((unit, storage), [])
            mapped = {}
            failures = []
            if not intervals:
                failures.append("retail contribution is missing")
            if len({row["section"] for row in group}) != 1:
                failures.append("candidate storage uses multiple COFF sections")
            if min(row["section_offset"] for row in group) != 0:
                failures.append("candidate section has unmodeled leading bytes")
            for row in group:
                values = proofs.get(row["name"], [])
                unique = sorted(set(values))
                if len(unique) != 1:
                    failures.append("%s maps to %s" % (row["name"], unique or "nothing"))
                    continue
                rva = unique[0]
                if not _contains(intervals, rva, row["size"]):
                    failures.append("%s extent 0x%x+0x%x escapes contribution" %
                                    (row["name"], rva, row["size"]))
                    continue
                mapped[row["name"]] = (rva, len(values))
            starts = [value[0] for value in mapped.values()]
            if len(starts) != len(set(starts)):
                failures.append("multiple candidate definitions map to one retail RVA")
            extents = sorted((mapped[row["name"]][0], row["size"], row["name"])
                             for row in group if row["name"] in mapped)
            for previous, current in zip(extents, extents[1:]):
                if previous[0] + previous[1] > current[0]:
                    failures.append("mapped retail extents overlap: %s and %s" %
                                    (previous[2], current[2]))
            uncovered = sorted(
                target for target in referenced_targets
                if _contains(intervals, target) and
                not any(rva <= target < rva + size for rva, size, _name in extents))
            if uncovered:
                failures.append("referenced retail RVAs lack candidate allocations: %s" %
                                ", ".join("0x%x" % value for value in uncovered[:8]))
            if failures:
                stats.open_groups += 1
                diagnostics.append((unit, storage, "; ".join(failures)))
                continue
            stats.closed_groups += 1
            stats.mapped_definitions += len(group)
            object_name = unit.replace("/", "\\") + ".c"
            for row in group:
                rva, proof_count = mapped[row["name"]]
                allocations.append(CandidateAllocation(
                    unit, object_name, row["name"], storage,
                    row["section_offset"], row["size"], row["alignment"], rva,
                    proof_count, row["scope"], "candidate-coff-reloc-bijection"))
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


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--base-dir", type=Path, default=REPO / "build/objdiff/base")
    parser.add_argument("--exe", type=Path, default=REPO / "build/orig/HEROES2W.EXE")
    parser.add_argument("--symbols", type=Path, default=REPO / "build/gen/symbol_names.csv")
    parser.add_argument("--units", type=Path, default=REPO / "config/units.toml")
    parser.add_argument("--output", type=Path, default=OUTPUT)
    parser.add_argument(
        "--require-all", action="store_true",
        help="fail without writing output unless every data-bearing group is closed")
    args = parser.parse_args(argv)
    payload, stats, diagnostics = manifest_bytes(
        base_dir=args.base_dir, exe=args.exe, symbols_path=args.symbols,
        units_path=args.units)
    print("candidate data: %d/%d definitions in %d closed groups; %d open groups; "
          "%d aligned DIR32 sites across %d functions" % (
              stats.mapped_definitions, stats.candidate_definitions,
              stats.closed_groups, stats.open_groups,
              stats.aligned_dir32_sites, stats.paired_functions))
    for unit, storage, reason in diagnostics:
        print("  OPEN %s %s: %s" % (unit, storage, reason))
    if args.require_all and diagnostics:
        print("candidate data: refusing partial manifest in --require-all mode")
        return 1
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(payload)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
