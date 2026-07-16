"""Canonicalize MSVC compiler-private data names in a disposable COFF copy.

The transform is deliberately local to one object.  It does not consult a
manifest, a paired object, source text, or retail addresses.  Symbol indices do
not change, so relocations and auxiliary symbol-index references keep pointing
at the same definitions while objdiff sees stable, content-derived names.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import io
import json
import os
import re
import struct
import tempfile
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


SYMBOL_SIZE = 18
VOLATILE_SG = re.compile(r"^\$SG[0-9]+$")
VOLATILE_T = re.compile(r"^\$T[0-9]+$")
NAMED_STATIC = re.compile(r"^(?P<prefix>.+\$S)[0-9]+$")

INITIALIZED_DATA = 0x00000040
UNINITIALIZED_DATA = 0x00000080
MEM_EXECUTE = 0x20000000
MEM_WRITE = 0x80000000
LNK_NRELOC_OVFL = 0x01000000

RELOCATION_WIDTHS = {
    0x0001: 2,  # IMAGE_REL_I386_DIR16
    0x0002: 2,  # IMAGE_REL_I386_REL16
    0x0006: 4,  # IMAGE_REL_I386_DIR32
    0x0007: 4,  # IMAGE_REL_I386_DIR32NB
    0x000A: 2,  # IMAGE_REL_I386_SECTION
    0x000B: 4,  # IMAGE_REL_I386_SECREL
    0x0014: 4,  # IMAGE_REL_I386_REL32
}


@dataclass(frozen=True)
class Section:
    index: int
    header_offset: int
    name: str
    raw_size: int
    raw_offset: int
    reloc_offset: int
    reloc_count: int
    characteristics: int


@dataclass(frozen=True)
class Symbol:
    index: int
    offset: int
    name: str
    value: int
    section: int
    typ: int
    storage_class: int
    aux_count: int


@dataclass(frozen=True)
class Relocation:
    section: int
    site: int
    symbol_index: int
    typ: int


@dataclass(frozen=True)
class Definition:
    symbol: Symbol
    section: Section
    storage: str
    start: int
    end: int


@dataclass(frozen=True)
class CanonicalRow:
    original_name: str
    canonical_name: str
    family: str
    storage: str
    section_ordinal: int
    section_offset: int
    physical_size: int
    meaningful_size: int
    occurrence: int
    digest: str
    proof: str
    preview: str


@dataclass(frozen=True)
class CanonicalizedObject:
    data: bytes
    rows: tuple[CanonicalRow, ...]


class CoffObject:
    def __init__(self, payload: bytes):
        self.data = bytes(payload)
        if len(self.data) < 20:
            raise ValueError("short COFF object")
        machine, section_count = struct.unpack_from("<HH", self.data, 0)
        if machine != 0x14C:
            raise ValueError(f"unsupported COFF machine 0x{machine:x}")
        self.section_count = section_count
        self.symbol_offset = struct.unpack_from("<I", self.data, 8)[0]
        self.symbol_count = struct.unpack_from("<I", self.data, 12)[0]
        optional_size = struct.unpack_from("<H", self.data, 16)[0]
        first_section = 20 + optional_size
        section_end = first_section + section_count * 40
        if section_end > len(self.data):
            raise ValueError("truncated COFF section table")
        self.string_offset = self.symbol_offset + self.symbol_count * SYMBOL_SIZE
        if self.string_offset + 4 > len(self.data):
            raise ValueError("missing COFF string table")
        self.string_size = struct.unpack_from("<I", self.data, self.string_offset)[0]
        if self.string_size < 4 or self.string_offset + self.string_size != len(self.data):
            raise ValueError("COFF string table is not final")
        self.sections = self._read_sections(first_section)
        self.symbols = self._read_symbols()
        self.relocations = self._read_relocations()

    def _string_name(self, offset: int) -> str:
        if not 4 <= offset < self.string_size:
            raise ValueError(f"invalid COFF string offset {offset}")
        start = self.string_offset + offset
        try:
            end = self.data.index(b"\0", start, self.string_offset + self.string_size)
        except ValueError as error:
            raise ValueError("unterminated COFF string") from error
        return self.data[start:end].decode("latin-1")

    def _symbol_name(self, offset: int) -> str:
        raw = self.data[offset:offset + 8]
        zero, string_offset = struct.unpack("<II", raw)
        if zero == 0:
            return self._string_name(string_offset)
        return raw.split(b"\0", 1)[0].decode("latin-1")

    def _section_name(self, offset: int) -> str:
        raw = self.data[offset:offset + 8].split(b"\0", 1)[0]
        if raw.startswith(b"/") and raw[1:].isdigit():
            return self._string_name(int(raw[1:]))
        return raw.decode("latin-1")

    def _read_sections(self, first: int) -> tuple[Section, ...]:
        rows = []
        for zero_index in range(self.section_count):
            offset = first + zero_index * 40
            raw_size, raw_offset, reloc_offset = struct.unpack_from(
                "<III", self.data, offset + 16)
            reloc_count = struct.unpack_from("<H", self.data, offset + 32)[0]
            characteristics = struct.unpack_from("<I", self.data, offset + 36)[0]
            if raw_offset and raw_offset + raw_size > len(self.data):
                raise ValueError("COFF section raw data is out of bounds")
            relocation_bytes = (10 if characteristics & LNK_NRELOC_OVFL
                                else reloc_count * 10)
            if reloc_count and reloc_offset + relocation_bytes > len(self.data):
                raise ValueError("COFF relocation table is out of bounds")
            rows.append(Section(
                zero_index + 1, offset, self._section_name(offset), raw_size,
                raw_offset, reloc_offset, reloc_count, characteristics,
            ))
        return tuple(rows)

    def _read_symbols(self) -> dict[int, Symbol]:
        if self.symbol_offset + self.symbol_count * SYMBOL_SIZE > len(self.data):
            raise ValueError("COFF symbol table is out of bounds")
        rows = {}
        index = 0
        while index < self.symbol_count:
            offset = self.symbol_offset + index * SYMBOL_SIZE
            value, section, typ, storage_class, aux_count = struct.unpack_from(
                "<IhHBB", self.data, offset + 8)
            if index + aux_count >= self.symbol_count:
                raise ValueError("COFF auxiliary symbols exceed the symbol table")
            rows[index] = Symbol(
                index, offset, self._symbol_name(offset), value, section, typ,
                storage_class, aux_count,
            )
            index += 1 + aux_count
        return rows

    def _read_relocations(self) -> tuple[Relocation, ...]:
        rows = []
        for section in self.sections:
            count = section.reloc_count
            first = 0
            if section.characteristics & LNK_NRELOC_OVFL:
                if count != 0xFFFF:
                    raise ValueError("COFF relocation overflow flag/count disagree")
                if section.reloc_offset + 10 > len(self.data):
                    raise ValueError("missing COFF relocation overflow record")
                count, symbol_index, typ = struct.unpack_from(
                    "<IIH", self.data, section.reloc_offset)
                if count < 1 or symbol_index or typ:
                    raise ValueError("invalid COFF relocation overflow record")
                first = 1
            for index in range(first, count):
                offset = section.reloc_offset + index * 10
                if offset + 10 > len(self.data):
                    raise ValueError("COFF relocation table is out of bounds")
                site, symbol_index, typ = struct.unpack_from("<IIH", self.data, offset)
                if site >= section.raw_size:
                    raise ValueError("COFF relocation site is outside its section")
                if symbol_index not in self.symbols:
                    raise ValueError("COFF relocation targets an auxiliary/missing symbol")
                rows.append(Relocation(section.index, site, symbol_index, typ))
        return tuple(rows)

    def section_bytes(self, section: Section) -> bytes:
        if section.raw_offset == 0:
            return bytes(section.raw_size)
        return self.data[section.raw_offset:section.raw_offset + section.raw_size]


def _storage(section: Section) -> str | None:
    flags = section.characteristics
    if flags & MEM_EXECUTE:
        return None
    if flags & UNINITIALIZED_DATA:
        return "bss"
    if not flags & INITIALIZED_DATA:
        return None
    return "data" if flags & MEM_WRITE else "rdata"


def _definitions(coff: CoffObject) -> tuple[Definition, ...]:
    by_section: dict[int, list[Symbol]] = defaultdict(list)
    for symbol in coff.symbols.values():
        if (symbol.section > 0 and symbol.typ == 0 and
                symbol.storage_class in (2, 3) and symbol.aux_count == 0 and
                _storage(coff.sections[symbol.section - 1]) is not None):
            by_section[symbol.section].append(symbol)
    rows = []
    for section_index, symbols in by_section.items():
        section = coff.sections[section_index - 1]
        offsets = sorted({symbol.value for symbol in symbols})
        next_offset = {
            value: offsets[index + 1] if index + 1 < len(offsets) else section.raw_size
            for index, value in enumerate(offsets)
        }
        symbols_at_offset = defaultdict(list)
        for symbol in symbols:
            symbols_at_offset[symbol.value].append(symbol.name)
        aliases = {
            offset: names for offset, names in symbols_at_offset.items()
            if len(names) > 1 and any(_family(name) is not None for name in names)
        }
        if aliases:
            raise ValueError(
                f"same-offset compiler-private data aliases in section "
                f"{section.index}: {aliases}")
        for symbol in symbols:
            end = next_offset[symbol.value]
            if not 0 <= symbol.value < end <= section.raw_size:
                raise ValueError(
                    f"invalid data extent for {symbol.name} at section "
                    f"{section.index}+0x{symbol.value:x}")
            rows.append(Definition(
                symbol, section, _storage(section) or "", symbol.value, end,
            ))
    return tuple(sorted(rows, key=lambda row: (
        row.section.index, row.start, row.symbol.index,
    )))


def _family(name: str) -> tuple[str, str | None] | None:
    if VOLATILE_SG.fullmatch(name):
        return "sg", None
    if VOLATILE_T.fullmatch(name):
        return "t", None
    match = NAMED_STATIC.fullmatch(name)
    if match:
        return "named", match.group("prefix")
    return None


def _relocation_width(typ: int) -> int:
    try:
        return RELOCATION_WIDTHS[typ]
    except KeyError as error:
        raise ValueError(f"unsupported i386 relocation type 0x{typ:x}") from error


def _float_width(payload: bytes):
    """Infer only widths proved by this allocation's bytes and physical span."""
    if len(payload) == 4:
        return 4, "extent-4"
    if len(payload) == 8 and any(payload[4:]):
        return 8, "extent-8-nonzero-upper-dword"
    return None, "ambiguous-content-width"


def _is_string(payload: bytes, relocations: list[Relocation]) -> int | None:
    if relocations:
        return None
    terminator = payload.find(b"\0")
    if terminator < 0 or any(payload[terminator + 1:]):
        return None
    return terminator + 1


def _escaped_preview(payload: bytes, limit: int = 48) -> str:
    shown = payload[:limit]
    text = "".join(
        chr(byte) if 0x20 <= byte < 0x7F and chr(byte) not in "\\\""
        else f"\\x{byte:02x}"
        for byte in shown
    )
    return text + ("..." if len(payload) > limit else "")


def _record_bytes(record: dict) -> bytes:
    return json.dumps(record, sort_keys=True, separators=(",", ":")).encode("ascii")


def _digest(record: bytes, seen: dict[str, bytes]) -> str:
    value = hashlib.sha256(record).hexdigest()
    previous = seen.get(value)
    if previous is not None and previous != record:
        raise ValueError(f"SHA-256 collision for canonical data record {value}")
    seen[value] = record
    return value


def _rewrite_names(coff: CoffObject, renames: dict[int, str]) -> bytes:
    data = bytearray(coff.data[:coff.string_offset])
    strings = bytearray(struct.pack("<I", 4))
    offsets: dict[bytes, int] = {}

    def encoded(name: str, section=False) -> bytes:
        raw = name.encode("latin-1")
        if len(raw) <= 8:
            return raw.ljust(8, b"\0")
        offset = offsets.get(raw)
        if offset is None:
            offset = len(strings)
            offsets[raw] = offset
            strings.extend(raw + b"\0")
        if section:
            value = f"/{offset}".encode("ascii")
            if len(value) > 8:
                raise ValueError("COFF long section-name offset does not fit")
            return value.ljust(8, b"\0")
        return struct.pack("<II", 0, offset)

    for section in coff.sections:
        data[section.header_offset:section.header_offset + 8] = encoded(
            section.name, section=True)
    for symbol in coff.symbols.values():
        data[symbol.offset:symbol.offset + 8] = encoded(
            renames.get(symbol.index, symbol.name))
    struct.pack_into("<I", strings, 0, len(strings))
    data.extend(strings)
    return bytes(data)


def _assert_only_names_changed(original: CoffObject, payload: bytes,
                               renames: dict[int, str]) -> None:
    normalized = CoffObject(payload)
    if (original.section_count != normalized.section_count or
            original.symbol_count != normalized.symbol_count or
            original.symbol_offset != normalized.symbol_offset):
        raise RuntimeError("canonical COFF topology postcondition failed")
    if len(original.sections) != len(normalized.sections):
        raise RuntimeError("canonical COFF section-count postcondition failed")
    for before, after in zip(original.sections, normalized.sections):
        if before.name != after.name:
            raise RuntimeError("canonical COFF changed a decoded section name")
        if (before.raw_size, before.raw_offset, before.reloc_offset,
                before.reloc_count, before.characteristics) != (
                after.raw_size, after.raw_offset, after.reloc_offset,
                after.reloc_count, after.characteristics):
            raise RuntimeError("canonical COFF changed section metadata")
        if original.section_bytes(before) != normalized.section_bytes(after):
            raise RuntimeError("canonical COFF changed section payload bytes")
    if original.relocations != normalized.relocations:
        raise RuntimeError("canonical COFF changed relocation records")
    if set(original.symbols) != set(normalized.symbols):
        raise RuntimeError("canonical COFF changed symbol indices")
    for index, before in original.symbols.items():
        after = normalized.symbols[index]
        if after.name != renames.get(index, before.name):
            raise RuntimeError("canonical COFF emitted an unexpected symbol name")
        if (before.value, before.section, before.typ, before.storage_class,
                before.aux_count) != (
                after.value, after.section, after.typ, after.storage_class,
                after.aux_count):
            raise RuntimeError("canonical COFF changed symbol metadata")
        aux_start = before.offset + SYMBOL_SIZE
        aux_end = aux_start + before.aux_count * SYMBOL_SIZE
        if original.data[aux_start:aux_end] != payload[aux_start:aux_end]:
            raise RuntimeError("canonical COFF changed auxiliary symbol bytes")

    # Everything before the string table must be identical after masking only
    # the section-name and primary-symbol-name fields that are allowed to move
    # between inline and string-table encodings.
    before_prefix = bytearray(original.data[:original.string_offset])
    after_prefix = bytearray(payload[:normalized.string_offset])
    for section in original.sections:
        before_prefix[section.header_offset:section.header_offset + 8] = bytes(8)
        after_prefix[section.header_offset:section.header_offset + 8] = bytes(8)
    for symbol in original.symbols.values():
        before_prefix[symbol.offset:symbol.offset + 8] = bytes(8)
        after_prefix[symbol.offset:symbol.offset + 8] = bytes(8)
    if before_prefix != after_prefix:
        raise RuntimeError("canonical COFF changed bytes outside symbol names")


def canonicalize_coff(payload: bytes) -> CanonicalizedObject:
    """Return a normalized comparison copy and its readable rename records."""
    coff = CoffObject(payload)
    definitions = _definitions(coff)
    definition_by_symbol = {row.symbol.index: row for row in definitions}
    section_relocations: dict[int, list[Relocation]] = defaultdict(list)
    for relocation in coff.relocations:
        section_relocations[relocation.section].append(relocation)

    candidates = {
        row.symbol.index: row for row in definitions
        if row.symbol.storage_class == 3 and _family(row.symbol.name) is not None
    }
    kinds: dict[int, tuple[str, bytes, str, str]] = {}
    for definition in candidates.values():
        family = _family(definition.symbol.name)
        raw = coff.section_bytes(definition.section)[definition.start:definition.end]
        own_relocs = [row for row in section_relocations[definition.section.index]
                      if definition.start <= row.site < definition.end]
        kind, meaningful, proof = "data", raw, "physical-span"
        if family and family[0] == "sg":
            string_size = _is_string(raw, own_relocs)
            if string_size is not None:
                kind, meaningful, proof = "string", raw[:string_size], "nul-terminated"
        elif family and family[0] == "t":
            width, proof = _float_width(raw)
            if width == 4:
                kind, meaningful = "f32", raw[:4]
            elif width == 8:
                kind, meaningful = "f64", raw[:8]
        kinds[definition.symbol.index] = (kind, meaningful, proof, _escaped_preview(meaningful))

    digest_records: dict[str, bytes] = {}
    dependencies = {}
    for symbol_index, definition in candidates.items():
        dependencies[symbol_index] = {
            relocation.symbol_index
            for relocation in section_relocations[definition.section.index]
            if (definition.start <= relocation.site < definition.end and
                relocation.symbol_index in candidates)
        }

    levels = {}
    finding_level = set()

    def level(symbol_index):
        if symbol_index in levels:
            return levels[symbol_index]
        if symbol_index in finding_level:
            raise ValueError(
                f"cyclic compiler-private data initializer at "
                f"{coff.symbols[symbol_index].name}")
        finding_level.add(symbol_index)
        value = 0
        if dependencies[symbol_index]:
            value = 1 + max(level(target) for target in dependencies[symbol_index])
        finding_level.remove(symbol_index)
        levels[symbol_index] = value
        return value

    for symbol_index in candidates:
        level(symbol_index)

    occurrences = defaultdict(int)
    renames = {}
    existing_names = {
        symbol.name: symbol.index for symbol in coff.symbols.values()
        if _family(symbol.name) is None
    }
    canonical_owners = {}
    resolved = {}

    for current_level in range(max(levels.values(), default=-1) + 1):
        prepared = []
        for symbol_index, definition in candidates.items():
            if levels[symbol_index] != current_level:
                continue
            family = _family(definition.symbol.name)
            assert family is not None
            kind, meaningful, proof, preview = kinds[definition.symbol.index]
            masked = bytearray(meaningful)
            reloc_rows = []
            for relocation in section_relocations[definition.section.index]:
                if not definition.start <= relocation.site < definition.end:
                    continue
                relative = relocation.site - definition.start
                width = _relocation_width(relocation.typ)
                if relative + width > len(meaningful):
                    raise ValueError(
                        f"relocation crosses meaningful payload for "
                        f"{definition.symbol.name}")
                addend = int.from_bytes(
                    masked[relative:relative + width], "little", signed=True)
                masked[relative:relative + width] = bytes(width)
                target = coff.symbols[relocation.symbol_index]
                if relocation.symbol_index in candidates:
                    target_identity = (
                        "canonical", renames[relocation.symbol_index])
                else:
                    target_identity = ("symbol", target.name)
                reloc_rows.append((
                    relative, relocation.typ, width, addend, target_identity,
                ))
            reloc_rows.sort(key=lambda row: _record_bytes({"relocation": row}))
            record = _record_bytes({
                "schema": "homm2-anon-data-v1",
                "kind": kind,
                "storage": definition.storage,
                "span": definition.end - definition.start,
                "meaningful_size": len(meaningful),
                "payload": bytes(masked).hex(),
                "relocations": reloc_rows,
            })
            record_digest = _digest(record, digest_records)
            if kind == "f32":
                identity = f"{int.from_bytes(meaningful, 'little'):08x}"
                base_name = f"$anon_f32_{identity}"
            elif kind == "f64":
                identity = f"{int.from_bytes(meaningful, 'little'):016x}"
                base_name = f"$anon_f64_{identity}"
            elif kind == "string":
                identity = _digest(meaningful, digest_records)
                base_name = f"$anon_str_{identity}"
            else:
                identity = record_digest
                base_name = f"$anon_data_{identity}"
            prefix = family[1]
            if family[0] == "named":
                base_name = f"{prefix}{kind}_{definition.storage}_{identity}"
            display_digest = identity if kind in ("string", "data") else record_digest
            prepared.append((
                definition, family[0], kind, meaningful, proof, preview,
                display_digest, base_name, record,
            ))

        for (definition, family, kind, meaningful, proof, preview,
             digest, base_name, record) in sorted(
                 prepared, key=lambda row: (
                     row[0].section.index, row[0].start)):
            occurrence = occurrences[base_name]
            occurrences[base_name] += 1
            canonical = f"{base_name}_{occurrence}"
            collision = existing_names.get(canonical)
            if collision is not None and collision != definition.symbol.index:
                raise ValueError(
                    f"canonical symbol name collides with existing symbol: {canonical}")
            collision = canonical_owners.get(canonical)
            if collision is not None and collision != definition.symbol.index:
                raise ValueError(f"duplicate canonical symbol name: {canonical}")
            canonical_owners[canonical] = definition.symbol.index
            renames[definition.symbol.index] = canonical
            resolved[definition.symbol.index] = (
                definition, family, kind, meaningful, proof, preview,
                digest, canonical, occurrence, record,
            )

    rows = []
    for definition in sorted(candidates.values(), key=lambda row: (
            row.section.index, row.start)):
        (_definition, family, _kind, meaningful, proof, preview,
         digest, canonical, occurrence, _record) = resolved[definition.symbol.index]
        rows.append(CanonicalRow(
            definition.symbol.name, canonical, family, definition.storage,
            definition.section.index, definition.start, definition.end - definition.start,
            len(meaningful), occurrence, digest, proof, preview,
        ))

    for symbol in sorted(coff.symbols.values(), key=lambda row: row.index):
        family = _family(symbol.name)
        if family is None or symbol.index in candidates:
            continue
        definition = definition_by_symbol.get(symbol.index)
        if definition is not None:
            storage = definition.storage
            section_ordinal = definition.section.index
            section_offset = definition.start
            physical_size = definition.end - definition.start
            status = "defined-nonprivate"
        else:
            storage = "common" if symbol.section == 0 and symbol.value else "undefined"
            section_ordinal = max(symbol.section, 0)
            section_offset = symbol.value
            physical_size = 0
            status = storage
        rows.append(CanonicalRow(
            symbol.name, symbol.name, family[0], storage, section_ordinal,
            section_offset, physical_size, 0, 0, "-", f"skipped-{status}", "",
        ))

    normalized = _rewrite_names(coff, renames)
    _assert_only_names_changed(coff, normalized, renames)
    return CanonicalizedObject(normalized, tuple(rows))


SIDECAR_HEADER = (
    "original_name", "canonical_name", "family", "storage", "section_ordinal",
    "section_offset", "physical_size", "meaningful_size", "occurrence", "sha256",
    "proof", "preview",
)


def sidecar_bytes(rows: tuple[CanonicalRow, ...]) -> bytes:
    stream = io.StringIO(newline="")
    writer = csv.writer(stream, delimiter="\t", lineterminator="\n")
    writer.writerow(SIDECAR_HEADER)
    for row in rows:
        writer.writerow((
            row.original_name, row.canonical_name, row.family, row.storage,
            row.section_ordinal, f"0x{row.section_offset:x}", f"0x{row.physical_size:x}",
            f"0x{row.meaningful_size:x}", row.occurrence, row.digest, row.proof, row.preview,
        ))
    return stream.getvalue().encode("utf-8")


def _atomic_write(path: Path, payload: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(
            dir=path.parent, prefix=f".{path.name}.", delete=False) as stream:
        temporary = Path(stream.name)
        stream.write(payload)
    os.replace(temporary, path)


def corpus_summary(roots: list[Path]) -> dict:
    summary = {"schema": 1, "roots": {}}
    for root in roots:
        counts = defaultdict(int)
        for path in sorted(root.rglob("*.obj")):
            result = canonicalize_coff(path.read_bytes())
            counts["objects"] += 1
            for row in result.rows:
                counts["rows"] += 1
                counts[f"family:{row.family}"] += 1
                counts[f"proof:{row.proof}"] += 1
                if row.canonical_name.startswith("$anon_f32_"):
                    counts["kind:f32"] += 1
                elif row.canonical_name.startswith("$anon_f64_"):
                    counts["kind:f64"] += 1
                elif row.canonical_name.startswith("$anon_str_"):
                    counts["kind:string"] += 1
                elif row.canonical_name.startswith("$anon_data_"):
                    counts["kind:data"] += 1
                elif row.canonical_name == row.original_name:
                    counts["kind:skipped"] += 1
                else:
                    counts["kind:named-static"] += 1
        summary["roots"][str(root)] = dict(sorted(counts.items()))
    return summary


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--sidecar", type=Path)
    parser.add_argument("--summary-root", type=Path, action="append")
    parser.add_argument("--summary-output", type=Path)
    args = parser.parse_args(argv)
    if args.summary_root:
        if args.input or args.output or args.sidecar:
            parser.error("summary mode cannot be combined with object mode")
        payload = (json.dumps(corpus_summary(args.summary_root), indent=2,
                              sort_keys=True) + "\n").encode("utf-8")
        if args.summary_output:
            _atomic_write(args.summary_output, payload)
        else:
            print(payload.decode("utf-8"), end="")
        return 0
    if not args.input or not args.output or not args.sidecar:
        parser.error("object mode requires --input, --output, and --sidecar")
    resolved_paths = [path.resolve() for path in (
        args.input, args.output, args.sidecar,
    )]
    if len(set(resolved_paths)) != len(resolved_paths):
        parser.error("input, output, and sidecar paths must be distinct")
    result = canonicalize_coff(args.input.read_bytes())
    _atomic_write(args.output, result.data)
    _atomic_write(args.sidecar, sidecar_bytes(result.rows))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
