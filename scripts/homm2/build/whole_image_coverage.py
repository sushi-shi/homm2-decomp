"""Build fail-closed retail file, loaded-image, and TU data coverage partitions."""

import csv
import json
import struct
from collections import defaultdict
from dataclasses import asdict, dataclass
from pathlib import Path

from homm2.build.candidate_data_manifest import IMAGE_REL_BASED_HIGHLOW
from homm2.build.contribution_manifest import contribution_rows


CONTRIBUTION_HEADER = (
    "object", "storage", "rva", "size", "segment", "section", "provenance",
)


@dataclass(frozen=True)
class CoverageRow:
    domain: str
    owner: str
    storage: str
    start: int
    size: int
    kind: str
    identity: str
    provenance: str


@dataclass(frozen=True)
class PaddingAllocation:
    unit: str
    name: str
    storage: str
    rva: int
    size: int


@dataclass(frozen=True)
class CoverageDiagnostic:
    cause: str
    owner: str
    storage: str
    start: int
    size: int
    detail: str


def load_contributions(path):
    """Load the exact generated contribution classes used by the delinker."""
    with Path(path).open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter="\t")
        if tuple(reader.fieldnames or ()) != CONTRIBUTION_HEADER:
            raise ValueError("invalid contribution manifest header: %s" % path)
        rows = []
        for line, row in enumerate(reader, 2):
            if row["storage"] not in ("text", "rdata", "data", "bss"):
                raise ValueError("unsupported contribution storage at %s:%d" %
                                 (path, line))
            object_name = row["object"].replace("/", "\\")
            if (not object_name or ":" in object_name or
                    any(part in ("", ".", "..")
                        for part in object_name.split("\\"))):
                raise ValueError("invalid contribution object at %s:%d" % (path, line))
            rva = int(row["rva"], 0)
            size = int(row["size"], 0)
            segment = int(row["segment"], 0)
            if (size <= 0 or rva < 0 or not 0 < segment <= 0xFFFF or
                    rva + size > 0x100000000):
                raise ValueError("invalid contribution extent at %s:%d" % (path, line))
            expected_section = {
                "text": ".text", "rdata": ".rdata",
                "data": ".data", "bss": ".data",
            }[row["storage"]]
            if row["section"] != expected_section:
                raise ValueError("contribution storage/section mismatch at %s:%d" %
                                 (path, line))
            if not row["section"] or not row["provenance"]:
                raise ValueError("empty contribution evidence at %s:%d" % (path, line))
            rows.append({
                "object": object_name, "storage": row["storage"],
                "rva": rva, "size": size, "segment": segment,
                "section": row["section"], "provenance": row["provenance"],
            })
    intervals = sorted((row["rva"], row["rva"] + row["size"], row) for row in rows)
    for previous, current in zip(intervals, intervals[1:]):
        if previous[1] > current[0]:
            raise ValueError(
                "overlapping contribution manifest rows at 0x%x" % current[0])
    return rows


def _align_up(value, alignment):
    return (value + alignment - 1) & -alignment


def _pe(path):
    data = Path(path).read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe:pe + 4] != b"PE\0\0":
        raise ValueError("not a PE file: %s" % path)
    coff = pe + 4
    count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    image_base = struct.unpack_from("<I", data, optional + 28)[0]
    section_alignment = struct.unpack_from("<I", data, optional + 32)[0]
    size_of_image = struct.unpack_from("<I", data, optional + 56)[0]
    size_of_headers = struct.unpack_from("<I", data, optional + 60)[0]
    directories = []
    directory_names = [
        "exports", "imports", "resources", "exceptions", "certificates",
        "base-relocations", "debug-directory", "architecture", "global-pointer",
        "tls", "load-config", "bound-imports", "iat", "delay-imports", "clr", "reserved",
    ]
    for index, name in enumerate(directory_names):
        rva, size = struct.unpack_from("<II", data, optional + 96 + index * 8)
        if rva and size:
            directories.append((name, rva, size))
    sections = []
    first = optional + optional_size
    for index in range(count):
        offset = first + index * 40
        name = data[offset:offset + 8].rstrip(b"\0").decode("latin-1")
        virtual_size, rva, raw_size, raw_offset = struct.unpack_from(
            "<IIII", data, offset + 8)
        sections.append({
            "name": name, "rva": rva, "virtual_size": virtual_size,
            "raw_size": raw_size, "raw_offset": raw_offset,
        })

    def raw_offset(rva):
        for section in sections:
            if section["rva"] <= rva < section["rva"] + section["raw_size"]:
                return section["raw_offset"] + rva - section["rva"]
        return None

    debug_entries = []
    debug = next((row for row in directories if row[0] == "debug-directory"), None)
    if debug is not None:
        cursor = raw_offset(debug[1])
        if cursor is None or debug[2] % 28:
            raise ValueError("invalid PE debug directory")
        debug_names = {2: "codeview-nb09", 3: "fpo", 4: "misc"}
        for offset in range(cursor, cursor + debug[2], 28):
            _characteristics, _timestamp, _major, _minor, typ, size, _rva, file_offset = \
                struct.unpack_from("<IIHHIIII", data, offset)
            if size:
                debug_entries.append((debug_names.get(typ, "debug-%d" % typ),
                                      file_offset, size))
    return {
        "data": data, "image_base": image_base, "section_alignment": section_alignment,
        "size_of_image": size_of_image, "size_of_headers": size_of_headers,
        "sections": sections, "directories": directories,
        "debug_entries": debug_entries, "raw_offset": raw_offset,
    }


def _partition(start, end, intervals, gap_factory):
    rows = []
    cursor = start
    for left, right, value in sorted(intervals):
        if left < cursor or right <= left or right > end:
            raise ValueError("coverage interval overlap/out-of-range at 0x%x" % left)
        if cursor < left:
            rows.append(gap_factory(cursor, left))
        rows.append(value)
        cursor = right
    if cursor < end:
        rows.append(gap_factory(cursor, end))
    return rows


def file_partition(pe):
    intervals = []
    header_end = min(pe["size_of_headers"], len(pe["data"]))
    intervals.append((0, header_end, CoverageRow(
        "file", "PE", "file", 0, header_end, "headers", "DOS+PE+section-table",
        "retail-pe-headers")))
    section_kinds = {
        ".text": "section-text", ".rdata": "section-rdata", ".data": "section-data",
        ".idata": "imports", ".rsrc": "resources", ".reloc": "base-relocations",
        ".edata": "exports",
    }
    for section in pe["sections"]:
        if not section["raw_size"]:
            continue
        start = section["raw_offset"]
        end = min(len(pe["data"]), start + section["raw_size"])
        intervals.append((start, end, CoverageRow(
            "file", "PE", section["name"], start, end - start,
            section_kinds.get(section["name"], "section-raw"), section["name"],
            "retail-pe-section-table")))
    for kind, start, size in pe["debug_entries"]:
        intervals.append((start, start + size, CoverageRow(
            "file", "LINK", "overlay", start, size, kind, kind,
            "retail-pe-debug-directory")))
    return _partition(0, len(pe["data"]), intervals, lambda left, right: CoverageRow(
        "file", "LINK", "file", left, right - left,
        "file-padding" if right <= pe["size_of_headers"] else "overlay",
        "-", "retail-file-gap"))


def loaded_partition(pe):
    rows = [CoverageRow(
        "loaded", "PE", "headers", 0, pe["size_of_headers"], "headers",
        "DOS+PE+section-table", "retail-pe-headers")]
    cursor = pe["size_of_headers"]
    section_kinds = {
        ".text": "executable", ".rdata": "read-only-data", ".data": "writable-data",
        ".idata": "imports", ".rsrc": "resources", ".reloc": "base-relocations",
        ".edata": "exports",
    }
    for section in sorted(pe["sections"], key=lambda row: row["rva"]):
        if cursor < section["rva"]:
            rows.append(CoverageRow(
                "loaded", "LINK", "alignment", cursor, section["rva"] - cursor,
                "virtual-padding", "-", "retail-section-alignment"))
        meaningful_end = section["rva"] + section["virtual_size"]
        aligned_end = _align_up(meaningful_end, pe["section_alignment"])
        if section["name"] == ".data" and section["raw_size"] < section["virtual_size"]:
            initialized_end = section["rva"] + section["raw_size"]
            rows.append(CoverageRow(
                "loaded", "PE", ".data", section["rva"], section["raw_size"],
                "writable-data", ".data initialized", "retail-pe-section-table"))
            rows.append(CoverageRow(
                "loaded", "PE", ".bss", initialized_end,
                meaningful_end - initialized_end, "loader-zero", ".data virtual tail",
                "retail-pe-section-table"))
        else:
            rows.append(CoverageRow(
                "loaded", "PE", section["name"], section["rva"],
                section["virtual_size"], section_kinds.get(section["name"], "section"),
                section["name"], "retail-pe-section-table"))
        if meaningful_end < aligned_end:
            rows.append(CoverageRow(
                "loaded", "LINK", section["name"], meaningful_end,
                aligned_end - meaningful_end, "virtual-padding", section["name"],
                "retail-section-alignment"))
        cursor = aligned_end
    if cursor < pe["size_of_image"]:
        rows.append(CoverageRow(
            "loaded", "LINK", "alignment", cursor, pe["size_of_image"] - cursor,
            "virtual-padding", "-", "retail-size-of-image"))
    return rows


def data_partition(allocations, contributions, pe, symbols_path, closed_groups=None):
    diagnostics = []
    rows = []
    padding = []
    by_group = defaultdict(list)
    for allocation in allocations:
        by_group[(allocation.unit, allocation.storage)].append(allocation)
    contribution_groups = defaultdict(list)
    for contribution in contributions:
        if contribution["storage"] == "text":
            continue
        unit = contribution["object"].replace("\\", "/").removesuffix(".c")
        contribution_groups[(unit, contribution["storage"])].append(contribution)
    configured_units = {unit for unit, _storage in contribution_groups}
    closed_groups = set(by_group) if closed_groups is None else set(closed_groups)

    claimed_allocations = set()
    for (unit, storage), group in sorted(contribution_groups.items()):
        candidates = by_group.get((unit, storage), [])
        for contribution in group:
            start = contribution["rva"]
            end = start + contribution["size"]
            owned = []
            for allocation in candidates:
                if start <= allocation.rva and allocation.rva + allocation.size <= end:
                    owned.append(allocation)
                    claimed_allocations.add((allocation.unit, allocation.storage,
                                             allocation.name, allocation.rva))
            intervals = []
            contribution_padding = []
            for allocation in owned:
                kind = ("literal" if allocation.name.startswith(("$SG", "??_C@")) else
                        "constant" if storage == "rdata" and allocation.scope == "local" else
                        "allocation")
                value = CoverageRow(
                    "tu-data", unit, storage, allocation.rva, allocation.size, kind,
                    allocation.name, allocation.provenance)
                intervals.append((allocation.rva, allocation.rva + allocation.size, value))

            def gap(left, right):
                size = right - left
                if (unit, storage) not in closed_groups:
                    diagnostics.append(CoverageDiagnostic(
                        "unresolved_group_gap", unit, storage, left, size,
                        "candidate object/storage group is not closed"))
                    return CoverageRow(
                        "tu-data", unit, storage, left, size, "unresolved", "-",
                        "candidate-topology-open")
                raw = pe["raw_offset"](left)
                body = (b"" if storage == "bss" else
                        pe["data"][raw:raw + size] if raw is not None else b"\xff")
                pad_name = "__homm2_pad_%s_%s_%08x" % (
                    unit.replace("/", "_"), storage, left)
                if storage == "bss" or body == b"\0" * size:
                    contribution_padding.append(PaddingAllocation(
                        unit, pad_name, storage, left, size))
                    return CoverageRow(
                        "tu-data", unit, storage, left, size, "padding", pad_name,
                        "explicit-retail-zero-padding")
                diagnostics.append(CoverageDiagnostic(
                    "unclassified_nonzero_gap", unit, storage, left, size,
                    "retail contribution gap has nonzero payload"))
                return CoverageRow(
                    "tu-data", unit, storage, left, size, "unclassified", "-",
                    "retail-contribution-gap")
            try:
                partition_rows = _partition(start, end, intervals, gap)
                rows.extend(partition_rows)
                padding.extend(contribution_padding)
            except ValueError as exc:
                diagnostics.append(CoverageDiagnostic(
                    "allocation_overlap", unit, storage, start, end - start, str(exc)))

    for allocation in allocations:
        key = (allocation.unit, allocation.storage, allocation.name, allocation.rva)
        if key not in claimed_allocations:
            diagnostics.append(CoverageDiagnostic(
                "allocation_without_owner_contribution", allocation.unit,
                allocation.storage, allocation.rva, allocation.size, allocation.name))

    with Path(symbols_path).open(newline="", encoding="latin-1") as stream:
        publics = [row for row in csv.DictReader(stream)
                   if row.get("kind") == "data" and
                   row.get("provenance") == "cv-public-data" and
                   row.get("unit") in configured_units]
    for public in publics:
        rva = int(public["rva"], 0)
        owners = [row for row in rows if row.start <= rva < row.start + row.size]
        if len(owners) != 1 or owners[0].kind in (
                "padding", "unclassified", "unresolved"):
            diagnostics.append(CoverageDiagnostic(
                "public_not_in_allocation", public.get("unit", "-"), "data",
                rva, 1, public["name"]))
    return rows, padding, diagnostics


def _validate_partition(rows, domain, start, end):
    selected = sorted((row.start, row.start + row.size) for row in rows
                      if row.domain == domain)
    cursor = start
    for left, right in selected:
        if left != cursor or right <= left:
            return "partition %s gap/overlap at 0x%x" % (domain, cursor)
        cursor = right
    if cursor != end:
        return "partition %s ends at 0x%x, expected 0x%x" % (domain, cursor, end)
    return None


def build_coverage(allocations, exe, symbols_path, units_path, closed_groups=None,
                   contributions_path=None):
    pe = _pe(exe)
    file_rows = file_partition(pe)
    loaded_rows = loaded_partition(pe)
    data_rows, padding, diagnostics = data_partition(
        allocations,
        (load_contributions(contributions_path) if contributions_path is not None
         else contribution_rows(exe, units_path)),
        pe, symbols_path,
        closed_groups=closed_groups)
    for domain, rows, start, end in (
            ("file", file_rows, 0, len(pe["data"])),
            ("loaded", loaded_rows, 0, pe["size_of_image"])):
        problem = _validate_partition(rows, domain, start, end)
        if problem:
            diagnostics.append(CoverageDiagnostic(
                "partition_failure", "PE", domain, start, end - start, problem))

    # Every HIGHLOW site and in-image target must land in the loaded partition.
    reloc = next((row for row in pe["directories"] if row[0] == "base-relocations"), None)
    if reloc is not None:
        cursor = pe["raw_offset"](reloc[1])
        limit = cursor + reloc[2]
        while cursor + 8 <= limit:
            page, block_size = struct.unpack_from("<II", pe["data"], cursor)
            if block_size < 8 or cursor + block_size > limit:
                diagnostics.append(CoverageDiagnostic(
                    "invalid_base_relocation_block", "PE", "reloc", page,
                    max(block_size, 1), "malformed block"))
                break
            for offset in range(cursor + 8, cursor + block_size, 2):
                entry = struct.unpack_from("<H", pe["data"], offset)[0]
                if entry >> 12 != IMAGE_REL_BASED_HIGHLOW:
                    continue
                site = page + (entry & 0xFFF)
                raw = pe["raw_offset"](site)
                if raw is None:
                    diagnostics.append(CoverageDiagnostic(
                        "highlow_site_not_raw_backed", "PE", "reloc", site, 4,
                        "HIGHLOW site has no stored payload"))
                    continue
                target = struct.unpack_from("<I", pe["data"], raw)[0] - pe["image_base"]
                for value, identity in ((site, "site"), (target, "target")):
                    if 0 <= value < pe["size_of_image"] and not any(
                            row.start <= value < row.start + row.size for row in loaded_rows):
                        diagnostics.append(CoverageDiagnostic(
                            "highlow_%s_uncovered" % identity, "PE", "reloc",
                            value, 1, "HIGHLOW %s" % identity))
            cursor += block_size
    return file_rows + loaded_rows + data_rows, padding, diagnostics


def manifest_bytes(rows):
    lines = ["domain\towner\tstorage\tstart\tsize\tkind\tidentity\tprovenance"]
    for row in sorted(rows, key=lambda value: (value.domain, value.start, value.owner)):
        lines.append("%s\t%s\t%s\t0x%x\t0x%x\t%s\t%s\t%s" % (
            row.domain, row.owner, row.storage, row.start, row.size, row.kind,
            row.identity.replace("\t", " "), row.provenance.replace("\t", " ")))
    return ("\n".join(lines) + "\n").encode("utf-8")


def diagnostics_bytes(diagnostics):
    counts = defaultdict(int)
    for diagnostic in diagnostics:
        counts[diagnostic.cause] += 1
    return (json.dumps({
        "schema": 1,
        "counts": dict(sorted(counts.items())),
        "diagnostics": [asdict(row) for row in diagnostics],
    }, indent=2, sort_keys=True) + "\n").encode("utf-8")
