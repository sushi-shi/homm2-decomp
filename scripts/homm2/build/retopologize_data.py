"""Project reviewed COFF writable allocations into retail final-link order.

The compiler can flatten definitions which the retail object emitted as several
writable contributions.  The reviewed data manifest retains both coordinates:
the current COFF section offset and the allocation's retail RVA.  For final-link
copies only, this module projects those allocations back into their NB09 owner
range and updates symbols and relocations to preserve their identities.
"""

from __future__ import annotations

import csv
import struct
from dataclasses import dataclass
from pathlib import Path


COFF_HEADER_SIZE = 20
SECTION_HEADER_SIZE = 40
SYMBOL_SIZE = 18
RELOCATION_SIZE = 10
IMAGE_SCN_ALIGN_MASK = 0x00F00000
IMAGE_SCN_CNT_INITIALIZED_DATA = 0x00000040
IMAGE_SCN_CNT_UNINITIALIZED_DATA = 0x00000080
IMAGE_REL_I386_DIR32 = 0x0006


@dataclass(frozen=True)
class AllocationMove:
    name: str
    old_offset: int
    new_offset: int
    size: int


@dataclass(frozen=True)
class SectionLayout:
    ordinal: int
    name: str
    storage: str
    retail_rva: int
    size: int
    alignment: int
    raw_backed: bool
    allocations: tuple[AllocationMove, ...]

    def map_offset(self, offset: int, width: int = 1) -> int:
        matches = [
            move for move in self.allocations
            if move.old_offset <= offset
            and offset + width <= move.old_offset + move.size
        ]
        if len(matches) != 1:
            raise ValueError(
                "%s section %d retail owner 0x%x: offset 0x%x..0x%x is not "
                "owned by one reviewed allocation"
                % (self.name, self.ordinal, self.retail_rva, offset, offset + width))
        move = matches[0]
        return move.new_offset + offset - move.old_offset


@dataclass(frozen=True)
class _Section:
    ordinal: int
    header: int
    name: str
    size: int
    raw: int
    relocations: int
    relocation_count: int
    characteristics: int


@dataclass(frozen=True)
class _Symbol:
    index: int
    offset: int
    name: str
    value: int
    section: int
    storage_class: int
    aux_count: int


def _read_tsv(path: Path) -> list[dict[str, str]]:
    with Path(path).open(newline="", encoding="utf-8") as stream:
        return list(csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter="\t"))


def _coff(payload: bytearray) -> tuple[list[_Section], dict[int, _Symbol], int]:
    if len(payload) < COFF_HEADER_SIZE:
        raise ValueError("short COFF object")
    section_count = struct.unpack_from("<H", payload, 2)[0]
    symbol_table, symbol_count = struct.unpack_from("<II", payload, 8)
    optional_size = struct.unpack_from("<H", payload, 16)[0]
    section_table = COFF_HEADER_SIZE + optional_size
    if section_table + section_count * SECTION_HEADER_SIZE > len(payload):
        raise ValueError("truncated COFF section table")
    if symbol_table + symbol_count * SYMBOL_SIZE + 4 > len(payload):
        raise ValueError("truncated COFF symbol table")
    strings = symbol_table + symbol_count * SYMBOL_SIZE
    string_size = struct.unpack_from("<I", payload, strings)[0]
    if strings + string_size > len(payload):
        raise ValueError("truncated COFF string table")

    def name_at(offset: int) -> str:
        first, second = struct.unpack_from("<II", payload, offset)
        if first == 0:
            start = strings + second
            if not strings + 4 <= start < strings + string_size:
                raise ValueError("invalid COFF string-table offset")
            end = payload.find(0, start, strings + string_size)
            if end < 0:
                raise ValueError("unterminated COFF symbol name")
            return bytes(payload[start:end]).decode("latin-1")
        return bytes(payload[offset:offset + 8]).split(b"\0", 1)[0].decode("latin-1")

    sections = []
    for index in range(section_count):
        header = section_table + index * SECTION_HEADER_SIZE
        name = bytes(payload[header:header + 8]).split(b"\0", 1)[0].decode("latin-1")
        size, raw, relocations = struct.unpack_from("<III", payload, header + 16)
        relocation_count = struct.unpack_from("<H", payload, header + 32)[0]
        characteristics = struct.unpack_from("<I", payload, header + 36)[0]
        sections.append(_Section(
            index + 1, header, name, size, raw, relocations,
            relocation_count, characteristics))

    symbols = {}
    index = 0
    while index < symbol_count:
        offset = symbol_table + index * SYMBOL_SIZE
        value = struct.unpack_from("<I", payload, offset + 8)[0]
        section = struct.unpack_from("<h", payload, offset + 12)[0]
        storage_class, aux_count = struct.unpack_from("<BB", payload, offset + 16)
        symbols[index] = _Symbol(
            index, offset, name_at(offset), value, section, storage_class, aux_count)
        index += 1 + aux_count
    return sections, symbols, symbol_table


def _shift_file_pointer(value: int, boundary: int, delta: int) -> int:
    if value == 0:
        return 0
    if value < boundary:
        return value
    return value + delta


def _alignment_bits(alignment: int) -> int:
    if alignment <= 0 or alignment & (alignment - 1) or alignment > 8192:
        raise ValueError("invalid COFF section alignment: %d" % alignment)
    return (alignment.bit_length()) << 20


def _is_section_symbol(symbol: _Symbol, section: _Section) -> bool:
    return (
        symbol.section == section.ordinal
        and symbol.value == 0
        and symbol.storage_class == 3
        and symbol.aux_count > 0
        and symbol.name == section.name
    )


def _rewrite_one(payload: bytearray, layout: SectionLayout) -> bytearray:
    sections, old_symbols, old_symbol_table = _coff(payload)
    if not 1 <= layout.ordinal <= len(sections):
        raise ValueError("missing COFF section ordinal %d" % layout.ordinal)
    section = sections[layout.ordinal - 1]
    if section.name != layout.name:
        raise ValueError(
            "section %d name changed: expected %s, found %s"
            % (layout.ordinal, layout.name, section.name))

    moves = sorted(layout.allocations, key=lambda move: move.old_offset)
    for previous, current in zip(moves, moves[1:]):
        if previous.old_offset + previous.size > current.old_offset:
            raise ValueError("overlapping candidate allocation moves")
    by_destination = sorted(moves, key=lambda move: move.new_offset)
    for previous, current in zip(by_destination, by_destination[1:]):
        if previous.new_offset + previous.size > current.new_offset:
            raise ValueError("overlapping retail allocation moves")
    if any(move.old_offset < 0 or move.old_offset + move.size > section.size
           for move in moves):
        raise ValueError("allocation move exceeds candidate section")
    if any(move.new_offset < 0 or move.new_offset + move.size > layout.size
           for move in moves):
        raise ValueError("allocation move exceeds retail contribution")

    converted_bss = layout.storage == "bss" and layout.raw_backed
    if layout.storage == "data":
        if section.raw == 0 or section.raw + section.size > len(payload):
            raise ValueError("initialized COFF section has invalid raw data")
        old_data = bytes(payload[section.raw:section.raw + section.size])
        covered = bytearray(section.size)
        new_data = bytearray(layout.size)
        for move in moves:
            old_start = move.old_offset
            new_start = move.new_offset
            new_data[new_start:new_start + move.size] = old_data[
                old_start:old_start + move.size]
            covered[old_start:old_start + move.size] = b"\1" * move.size
        unexplained = [
            index for index, (byte, owner) in enumerate(zip(old_data, covered))
            if byte != 0 and owner == 0
        ]
        if unexplained:
            raise ValueError(
                "unreviewed nonzero candidate bytes begin at section offset 0x%x"
                % unexplained[0])
        boundary = section.raw + section.size
        delta = layout.size - section.size
        payload[section.raw:boundary] = new_data
    elif converted_bss:
        if section.raw != 0:
            raise ValueError("raw-backed zero section already has candidate payload")
        boundary = old_symbol_table
        delta = layout.size
        payload[boundary:boundary] = bytes(layout.size)
    elif layout.storage == "bss":
        if section.raw != 0:
            raise ValueError("loader-zero COFF section unexpectedly has raw data")
        boundary = len(payload) + 1
        delta = 0
    else:
        raise ValueError("unsupported writable storage: %s" % layout.storage)

    # File offsets live in the COFF header and every section header.  Raw-size
    # replacement happens after the section table, so the headers themselves do
    # not move.
    if delta:
        struct.pack_into(
            "<I", payload, 8,
            _shift_file_pointer(old_symbol_table, boundary, delta))
        for item in sections:
            for field in (20, 24, 28):
                value = struct.unpack_from("<I", payload, item.header + field)[0]
                struct.pack_into(
                    "<I", payload, item.header + field,
                    _shift_file_pointer(value, boundary, delta))

    struct.pack_into("<I", payload, section.header + 16, layout.size)
    if converted_bss:
        payload[section.header:section.header + 8] = b".data\0\0\0"
        struct.pack_into("<I", payload, section.header + 20, boundary)
    characteristics = (
        section.characteristics & ~IMAGE_SCN_ALIGN_MASK
        | _alignment_bits(layout.alignment))
    if converted_bss:
        characteristics = (
            characteristics & ~IMAGE_SCN_CNT_UNINITIALIZED_DATA
            | IMAGE_SCN_CNT_INITIALIZED_DATA)
    struct.pack_into("<I", payload, section.header + 36, characteristics)

    # Reparse after the splice so symbol and relocation file offsets are current.
    current_sections, current_symbols, _ = _coff(payload)
    new_symbol_values = {}
    for index, old_symbol in old_symbols.items():
        current = current_symbols[index]
        if old_symbol.section != layout.ordinal or _is_section_symbol(old_symbol, section):
            new_symbol_values[index] = old_symbol.value
            continue
        try:
            new_value = layout.map_offset(old_symbol.value)
        except ValueError as exc:
            raise ValueError("%s symbol %s: %s" % (
                layout.storage, old_symbol.name, exc)) from exc
        struct.pack_into("<I", payload, current.offset + 8, new_value)
        new_symbol_values[index] = new_value

    # The section-definition auxiliary record owns the section length.
    for index, old_symbol in old_symbols.items():
        if not _is_section_symbol(old_symbol, section):
            continue
        current = current_symbols[index]
        if converted_bss:
            payload[current.offset:current.offset + 8] = b".data\0\0\0"
        struct.pack_into("<I", payload, current.offset + SYMBOL_SIZE, layout.size)

    # Move relocation source sites inside the rebuilt section first.
    for item in current_sections:
        if item.relocation_count == 0:
            continue
        if item.relocations == 0 or (
                item.relocations + item.relocation_count * RELOCATION_SIZE > len(payload)):
            raise ValueError("invalid COFF relocation table")
        for relocation_index in range(item.relocation_count):
            record = item.relocations + relocation_index * RELOCATION_SIZE
            source_offset, target_index, relocation_type = struct.unpack_from(
                "<IIH", payload, record)
            if item.ordinal == layout.ordinal:
                source_offset = layout.map_offset(source_offset, 4)
                struct.pack_into("<I", payload, record, source_offset)
            target = old_symbols.get(target_index)
            if target is None or target.section != layout.ordinal:
                continue
            if relocation_type != IMAGE_REL_I386_DIR32:
                raise ValueError(
                    "unsupported relocation type 0x%x targeting rebuilt section"
                    % relocation_type)
            # A relocation naming a real allocation keeps its owner-relative
            # addend when that allocation moves.  This includes evidenced
            # base-minus-N expressions.  Only a relocation against the section
            # symbol encodes the old section coordinate in its addend.
            if not _is_section_symbol(target, section):
                continue
            if item.raw == 0:
                raise ValueError("relocation source section has no raw data")
            field = item.raw + source_offset
            if field + 4 > len(payload):
                raise ValueError("relocation field exceeds COFF payload")
            addend = struct.unpack_from("<i", payload, field)[0]
            old_resolved = target.value + addend
            new_resolved = layout.map_offset(old_resolved)
            new_addend = new_resolved - new_symbol_values[target_index]
            struct.pack_into("<i", payload, field, new_addend)

    # A final parse catches stale file pointers and malformed symbol-table shifts.
    _coff(payload)
    return payload


def rewrite_coff_data_topology(source: Path, destination: Path,
                               layouts: tuple[SectionLayout, ...]) -> int:
    payload = bytearray(Path(source).read_bytes())
    changed = 0
    for layout in sorted(layouts, key=lambda item: item.ordinal):
        payload = _rewrite_one(payload, layout)
        changed += 1
    destination = Path(destination)
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(payload)
    return changed


class RetailDataTopology:
    def __init__(self, manifest: Path, sections: Path, contributions: Path,
                 retail_exe: Path):
        self._layouts = self._load(
            Path(manifest), Path(sections), Path(contributions), Path(retail_exe))

    @staticmethod
    def _retail_data(path: Path) -> tuple[bytes, int, int]:
        payload = path.read_bytes()
        pe = struct.unpack_from("<I", payload, 0x3C)[0]
        section_count = struct.unpack_from("<H", payload, pe + 6)[0]
        optional_size = struct.unpack_from("<H", payload, pe + 20)[0]
        table = pe + 24 + optional_size
        for index in range(section_count):
            header = table + index * SECTION_HEADER_SIZE
            name = bytes(payload[header:header + 8]).split(b"\0", 1)[0]
            if name != b".data":
                continue
            virtual_size, rva, raw_size, raw = struct.unpack_from(
                "<IIII", payload, header + 8)
            return bytes(payload[raw:raw + raw_size]), rva, virtual_size
        raise ValueError("retail executable has no .data section")

    @classmethod
    def _load(cls, manifest_path: Path, section_path: Path,
              contribution_path: Path, retail_exe: Path):
        manifest = _read_tsv(manifest_path)
        sections = _read_tsv(section_path)
        contributions = _read_tsv(contribution_path)
        retail_data, data_rva, data_virtual_size = cls._retail_data(retail_exe)
        reviewed_sections = [
            row for row in sections
            if row["storage"] in ("data", "bss")
            and row["provenance"].startswith((
                "reviewed-definition-affine-section",
                "reviewed-definition-nonaffine-section",
            ))
        ]
        last_reviewed_initialized_end = max(
            int(row["rva"], 0) + int(row["size"], 0)
            for row in contributions if row["storage"] == "data")
        layouts = {}
        for reviewed_section in reviewed_sections:
            object_name = reviewed_section["object"]
            unit = object_name.replace("\\", "/").removesuffix(".c")
            ordinal = int(reviewed_section["ordinal"])
            rows = [
                row for row in manifest
                if row["object"] == object_name
                and int(row["section_ordinal"]) == ordinal
            ]
            if not rows:
                raise ValueError("reviewed section has no allocations: %s:%d"
                                 % (unit, ordinal))
            storage = reviewed_section["storage"]
            owners = []
            for owner in contributions:
                if owner["object"] != object_name or owner["storage"] != storage:
                    continue
                start = int(owner["rva"], 0)
                end = start + int(owner["size"], 0)
                if all(start <= int(row["rva"], 0)
                       and int(row["rva"], 0) + int(row["size"], 0) <= end
                       for row in rows):
                    owners.append(owner)
            if len(owners) != 1:
                raise ValueError(
                    "expected one retail owner for %s:%d, found %d"
                    % (unit, ordinal, len(owners)))
            owner = owners[0]
            owner_rva = int(owner["rva"], 0)
            owner_size = int(owner["size"], 0)
            if reviewed_section["rva"] == "-":
                layout_rva = owner_rva
                layout_size = owner_size
            else:
                layout_rva = int(reviewed_section["rva"], 0)
                layout_size = int(reviewed_section["size"], 0)
            moves = tuple(AllocationMove(
                row["name"], int(row["section_offset"], 0),
                int(row["rva"], 0) - layout_rva, int(row["size"], 0))
                for row in rows)
            destination_coverage = bytearray(layout_size)
            for move in moves:
                if move.new_offset < 0 or move.new_offset + move.size > layout_size:
                    raise ValueError("reviewed allocation exceeds retail owner")
                if any(destination_coverage[move.new_offset:move.new_offset + move.size]):
                    raise ValueError("reviewed retail allocations overlap")
                destination_coverage[move.new_offset:move.new_offset + move.size] = (
                    b"\1" * move.size)
            if storage == "data":
                start = layout_rva - data_rva
                if not 0 <= start <= len(retail_data) - layout_size:
                    raise ValueError("initialized owner is outside retail raw .data")
                retail_owner = retail_data[start:start + layout_size]
                unexplained = [
                    index for index, (byte, covered) in enumerate(
                        zip(retail_owner, destination_coverage))
                    if byte != 0 and covered == 0
                ]
                if unexplained:
                    raise ValueError(
                        "unreviewed nonzero retail bytes begin at %s+0x%x"
                        % (unit, unexplained[0]))
            elif not data_rva <= layout_rva < data_rva + data_virtual_size:
                raise ValueError("loader-zero owner is outside retail virtual .data")
            retail_alignment = layout_rva & -layout_rva
            alignment = min(int(reviewed_section["alignment"], 0), retail_alignment)
            layouts.setdefault(unit, []).append(SectionLayout(
                ordinal, reviewed_section["name"], storage,
                layout_rva, layout_size, alignment,
                (storage == "data" or
                 layout_rva + layout_size <= last_reviewed_initialized_end),
                moves))
        return {unit: tuple(rows) for unit, rows in layouts.items()}

    def layouts_for(self, unit: str) -> tuple[SectionLayout, ...]:
        return self._layouts.get(unit, ())

    @property
    def section_count(self) -> int:
        return sum(len(rows) for rows in self._layouts.values())
