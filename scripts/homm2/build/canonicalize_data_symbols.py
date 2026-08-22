"""Canonicalize MSVC compiler-private names in a disposable COFF copy.

The data transform is deliberately local to one object and content-derived.
Reviewed ``DATA_COMPGEN`` bindings and source-free real-literal placements
additionally replace the physical candidate symbol with its reviewed identity
after proving section, offset, extent, storage, and scope against the generated
data manifest.
Compiler-generated functions may additionally consume source ``VA_COMPGEN``
claims, then prove their semantic role from the object's relocation graph before
renaming volatile ``$E`` symbols. Symbol indices do not change. In embedded
.text jump tables, same-function DIR32 references through volatile local labels
or another external function owner are rewritten to the containing external
function plus an equivalent owner-relative addend; all resolved section offsets
are proved unchanged.
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
import warnings
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

from homm2.build.normalized_freshness import write_stamp


SYMBOL_SIZE = 18
VOLATILE_SG = re.compile(r"^\$SG[0-9]+$")
ANON_STR = re.compile(r"^(\$anon_str_[0-9a-f]{64})_[0-9]+$")
VOLATILE_T = re.compile(r"^\$T[0-9]+$")
NAMED_STATIC = re.compile(r"^(?P<prefix>.+\$S)[0-9]+$")
VOLATILE_E_FUNCTION = re.compile(r"^_?\$E[0-9]+$")
COMPGEN_PREFIX = "__h2cg$"
REAL_LITERAL = re.compile(r"^__real@(4|8)@[0-9a-f]{20}$")

INITIALIZED_DATA = 0x00000040
UNINITIALIZED_DATA = 0x00000080
MEM_EXECUTE = 0x20000000
MEM_WRITE = 0x80000000
LNK_NRELOC_OVFL = 0x01000000
LNK_INFO = 0x00000200

RELOCATION_WIDTHS = {
    0x0001: 2,  # IMAGE_REL_I386_DIR16
    0x0002: 2,  # IMAGE_REL_I386_REL16
    0x0006: 4,  # IMAGE_REL_I386_DIR32
    0x0007: 4,  # IMAGE_REL_I386_DIR32NB
    0x000A: 2,  # IMAGE_REL_I386_SECTION
    0x000B: 4,  # IMAGE_REL_I386_SECREL
    0x0014: 4,  # IMAGE_REL_I386_REL32
}
DIR32 = 0x0006
FUNCTION_TYPE = 0x0020
EXTERNAL_STORAGE = 2
STATIC_STORAGE = 3
WEAK_EXTERNAL_STORAGE = 105


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
    offset: int = 0


@dataclass(frozen=True)
class JumpTableRewrite:
    relocation_offset: int
    section: int
    site: int
    original_symbol_index: int
    owner_symbol_index: int
    original_addend: int
    owner_addend: int
    resolved_offset: int


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
class CompgenClaim:
    name: str
    kind: str
    owner: str
    size: int


@dataclass(frozen=True)
class CompgenDataClaim:
    name: str
    section: int
    offset: int
    size: int
    storage: str
    scope: str
    proof: str = "source-DATA_COMPGEN"


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
                rows.append(Relocation(
                    section.index, site, symbol_index, typ, offset))
        return tuple(rows)

    def section_bytes(self, section: Section) -> bytes:
        if section.raw_offset == 0:
            return bytes(section.raw_size)
        return self.data[section.raw_offset:section.raw_offset + section.raw_size]


def weak_and_strong_names(payload: bytes) -> tuple[set[str], set[str]]:
    """Return weakly referenced and strongly defined external names."""
    coff = CoffObject(payload)
    weak = {
        symbol.name for symbol in coff.symbols.values()
        if symbol.storage_class == WEAK_EXTERNAL_STORAGE
    }
    strong = {
        symbol.name for symbol in coff.symbols.values()
        if symbol.storage_class == EXTERNAL_STORAGE and symbol.section > 0
    }
    return weak, strong


def assert_weak_external_link_set(paths: list[Path]) -> int:
    """Prove that resolving weak externals to their defaults is link-faithful."""
    weak: set[str] = set()
    strong: set[str] = set()
    for path in paths:
        one_weak, one_strong = weak_and_strong_names(path.read_bytes())
        weak.update(one_weak)
        strong.update(one_strong)
    clash = sorted(weak & strong)
    if clash:
        raise ValueError(
            f"{len(clash)} weak external(s) also have a strong definition, "
            "so default retargeting is not link-equivalent: " +
            ", ".join(clash[:8]))
    return len(weak)


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
    if name.startswith("??_C@"):
        # /Gf pooled string literal: an external COMDAT whose mangled name
        # encodes the content - same content-identified family as $SG cells.
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


def _function_ranges(coff: CoffObject) -> dict[int, tuple[tuple[int, int, Symbol], ...]]:
    """Return unambiguous function ownership ranges per text section."""
    by_section: dict[int, dict[int, list[Symbol]]] = defaultdict(
        lambda: defaultdict(list))
    for symbol in coff.symbols.values():
        if (symbol.section > 0 and symbol.typ == FUNCTION_TYPE and
                symbol.storage_class in (EXTERNAL_STORAGE, STATIC_STORAGE) and
                coff.sections[symbol.section - 1].characteristics & MEM_EXECUTE):
            by_section[symbol.section][symbol.value].append(symbol)
    result = {}
    for section_index, by_start in by_section.items():
        starts = sorted(by_start)
        ranges = []
        for index, start in enumerate(starts):
            if len(by_start[start]) != 1:
                continue
            end = starts[index + 1] if index + 1 < len(starts) else (
                coff.sections[section_index - 1].raw_size)
            ranges.append((start, end, by_start[start][0]))
        result[section_index] = tuple(ranges)
    return result


def _function_owner(ranges, section: int, offset: int) -> Symbol | None:
    for start, end, symbol in ranges.get(section, ()):
        if start <= offset < end:
            return symbol
    return None


def _compgen_renames(coff: CoffObject, claims: tuple[CompgenClaim, ...]):
    if not claims:
        return {}, ()
    claim_names = [claim.name for claim in claims]
    claim_name_set = set(claim_names)
    if len(claim_name_set) != len(claim_names):
        raise ValueError("duplicate semantic compiler-function claim")
    defined_functions = {
        symbol.index: symbol for symbol in coff.symbols.values()
        if (symbol.section > 0 and symbol.typ == FUNCTION_TYPE and
            coff.sections[symbol.section - 1].characteristics & MEM_EXECUTE)
    }
    by_name = defaultdict(list)
    for symbol in defined_functions.values():
        by_name[symbol.name].append(symbol)
    unexpected = sorted(
        name for name in by_name
        if name.startswith(COMPGEN_PREFIX) and name not in claim_name_set)
    if unexpected:
        raise ValueError("unclaimed semantic compiler functions: %s" %
                         ", ".join(unexpected))
    by_section = defaultdict(list)
    for symbol in defined_functions.values():
        by_section[symbol.section].append(symbol)

    def extent(symbol):
        later = sorted(other.value for other in by_section[symbol.section]
                       if other.value > symbol.value)
        end = later[0] if later else coff.sections[symbol.section - 1].raw_size
        return symbol.value, end

    def validate_extent(symbol, claim):
        start, end = extent(symbol)
        section = coff.sections[symbol.section - 1]
        body = coff.section_bytes(section)
        padding = body[start + claim.size:end]
        if end - start < claim.size or any(byte not in (0x90, 0xCC)
                                           for byte in padding):
            raise ValueError("%s physical span 0x%x does not contain the "
                             "expected 0x%x-byte body plus code padding" %
                             (symbol.name, end - start, claim.size))
        return start, start + claim.size

    satisfied = set()
    for claim in claims:
        semantic = by_name.get(claim.name, ())
        if semantic:
            if len(semantic) != 1:
                raise ValueError("duplicate semantic compiler function: %s" % claim.name)
            validate_extent(semantic[0], claim)
            satisfied.add(claim.name)
    pending = [claim for claim in claims if claim.name not in satisfied]
    if not pending:
        return {}, ()

    volatile = {index: symbol for index, symbol in defined_functions.items()
                if VOLATILE_E_FUNCTION.fullmatch(symbol.name)}
    extents = {}
    for index, symbol in volatile.items():
        extents[index] = extent(symbol)
    outgoing = defaultdict(set)
    for relocation in coff.relocations:
        for index, symbol in volatile.items():
            start, end = extents[index]
            if relocation.section == symbol.section and start <= relocation.site < end:
                outgoing[index].add(relocation.symbol_index)

    def target_names(index):
        return {coff.symbols[target].name for target in outgoing[index]}

    def owner_present(names, owner):
        parts = owner.split("::")
        mangled_prefix = "?" + "@".join(reversed(parts)) + "@@"
        return any(
            (len(parts) == 1 and name == "_" + owner)
            or name.startswith(mangled_prefix)
            for name in names)

    def is_special_member(index, owner, prefix):
        names = target_names(index)
        return (owner_present(names, owner) and
                any(name.startswith(prefix) for name in names))

    def is_inline_static_member_helper(index, owner):
        return "::" in owner and owner_present(target_names(index), owner)

    def is_atexit(index, owner):
        return ("_atexit" in target_names(index) and
                any(target in volatile and
                    (is_special_member(target, owner, "??1") or
                     is_inline_static_member_helper(target, owner))
                    for target in outgoing[index]))

    def is_locale_facet_id_atexit(index):
        names = target_names(index)
        return ("_atexit" in names and
                any(name.startswith("?id@?$ctype@") for name in names))

    def is_static_init_dispatch(index, owner):
        targets = outgoing[index]
        return (any(target in volatile and
                    (is_special_member(target, owner, "??0") or
                     is_inline_static_member_helper(target, owner))
                    for target in targets) and
                any(target in volatile and is_atexit(target, owner)
                    for target in targets))

    def is_inline_static_ctor(index, owner):
        return (is_inline_static_member_helper(index, owner) and
                any(index in outgoing[caller] and
                    is_static_init_dispatch(caller, owner)
                    for caller in volatile))

    def is_inline_static_dtor(index, owner):
        return (is_inline_static_member_helper(index, owner) and
                any(index in outgoing[caller] and is_atexit(caller, owner)
                    for caller in volatile))

    def has_role(index, claim):
        if claim.kind == "STATIC_CTOR":
            return (is_special_member(index, claim.owner, "??0") or
                    is_inline_static_ctor(index, claim.owner))
        if claim.kind == "STATIC_DTOR":
            return (is_special_member(index, claim.owner, "??1") or
                    is_inline_static_dtor(index, claim.owner))
        if claim.kind == "STATIC_ATEXIT":
            return is_atexit(index, claim.owner)
        if claim.kind == "STATIC_INIT_DISPATCH":
            return is_static_init_dispatch(index, claim.owner)
        if claim.kind == "LOCALE_FACET_ID_INIT":
            names = target_names(index)
            return (any(name.startswith("??_B?1???id@?$ctype@")
                        for name in names) and
                    any(target in volatile and is_locale_facet_id_atexit(target)
                        for target in outgoing[index]))
        if claim.kind == "LOCALE_FACET_ID_ATEXIT":
            return is_locale_facet_id_atexit(index)
        return False

    assigned = {}
    for claim in pending:
        candidates = [index for index in volatile
                      if index not in assigned and has_role(index, claim)]
        if len(candidates) != 1:
            warnings.warn(
                "%s has %d semantic compiler-function candidates; leaving "
                "claim unbound" % (claim.name, len(candidates)),
                RuntimeWarning, stacklevel=2)
            continue
        assigned[candidates[0]] = claim

    rows = []
    renames = {}
    for index, claim in assigned.items():
        symbol = volatile[index]
        start, end = validate_extent(symbol, claim)
        renames[index] = claim.name
        rows.append(CanonicalRow(
            symbol.name, claim.name, "compgen", "text", symbol.section,
            start, end - start, end - start, 0, "-",
            "semantic-relocation-role", "%s:%s" % (claim.kind, claim.owner),
        ))
    return renames, tuple(rows)


def _compgen_data_renames(
        coff: CoffObject,
        definitions: tuple[Definition, ...],
        claims: tuple[CompgenDataClaim, ...]):
    if not claims:
        return {}, ()
    by_location = defaultdict(list)
    by_name = defaultdict(list)
    for definition in definitions:
        by_location[(
            definition.section.index,
            definition.start,
            definition.storage,
            "external" if definition.symbol.storage_class == EXTERNAL_STORAGE
            else "local",
        )].append(definition)
        by_name[definition.symbol.name].append(definition)
    renames = {}
    rows = []
    claimed_names = set()
    for claim in claims:
        if claim.name in claimed_names:
            raise ValueError(
                f"duplicate semantic compiler-data claim: {claim.name}")
        claimed_names.add(claim.name)
        key = (
            claim.section,
            claim.offset,
            claim.storage,
            claim.scope,
        )
        named = by_name.get(claim.name, ())
        if len(named) > 1:
            raise ValueError(
                f"{claim.name} has {len(named)} existing semantic definitions")
        matches = named or by_location.get(key, ())
        if len(matches) != 1:
            raise ValueError(
                f"{claim.name} has {len(matches)} candidate definitions at "
                f"section {claim.section}+0x{claim.offset:x}")
        definition = matches[0]
        definition_scope = (
            "external" if definition.symbol.storage_class == EXTERNAL_STORAGE
            else "local")
        if (definition.storage, definition_scope) != (
                claim.storage, claim.scope):
            raise ValueError(
                f"{claim.name} existing definition has "
                f"{definition.storage}/{definition_scope}, expected "
                f"{claim.storage}/{claim.scope}")
        physical_size = definition.end - definition.start
        if not 0 < claim.size <= physical_size:
            raise ValueError(
                f"{claim.name} logical size 0x{claim.size:x} exceeds candidate "
                f"extent 0x{physical_size:x}")
        collision = next((
            symbol for symbol in coff.symbols.values()
            if symbol.name == claim.name
            and symbol.index != definition.symbol.index
        ), None)
        reviewed_real_reference = (
            claim.proof == "candidate-COFF-real"
            and REAL_LITERAL.fullmatch(claim.name) is not None
            and collision is not None
            and collision.section == 0
            and collision.value == 0
            and collision.typ == 0
            and collision.storage_class == EXTERNAL_STORAGE)
        if collision is not None and not reviewed_real_reference:
            raise ValueError(
                "semantic compiler-data name collides with existing symbol: "
                f"{claim.name}")
        renames[definition.symbol.index] = claim.name
        meaningful = coff.section_bytes(definition.section)[
            definition.start:definition.start + claim.size]
        rows.append(CanonicalRow(
            definition.symbol.name,
            claim.name,
            "semantic-data",
            definition.storage,
            definition.section.index,
            definition.start,
            physical_size,
            claim.size,
            0,
            hashlib.sha256(meaningful).hexdigest(),
            (f"{claim.proof}-existing-name" if named else claim.proof),
            _escaped_preview(meaningful),
        ))
    return renames, tuple(rows)


def _real_literal_reference_renames(
        coff: CoffObject,
        definitions: tuple[Definition, ...],
        semantic_by_symbol: dict[str, str]):
    """Apply one reviewed identity to every copy of an external VC6 real COMDAT."""
    if not semantic_by_symbol:
        return {}, ()
    from homm2.build.canonicalize_relocs import real_literal_bytes

    definition_by_index = {
        definition.symbol.index: definition for definition in definitions
    }
    renames = {}
    rows = []
    for symbol in sorted(coff.symbols.values(), key=lambda row: row.index):
        semantic = semantic_by_symbol.get(symbol.name)
        if semantic is None:
            continue
        if (REAL_LITERAL.fullmatch(symbol.name) is None or
                symbol.storage_class != EXTERNAL_STORAGE):
            raise ValueError(
                f"invalid reviewed real-literal reference identity: {symbol.name}")
        definition = definition_by_index.get(symbol.index)
        if definition is not None:
            expected = real_literal_bytes(symbol.name)
            assert expected is not None
            actual = coff.section_bytes(definition.section)[
                definition.start:definition.start + len(expected)]
            if actual != expected:
                raise ValueError(
                    f"{symbol.name} COMDAT payload contradicts its VC6 spelling")
            storage = definition.storage
            section = definition.section.index
            offset = definition.start
            physical_size = definition.end - definition.start
            meaningful_size = len(expected)
            preview = _escaped_preview(expected)
        else:
            storage = "undefined"
            section = 0
            offset = 0
            physical_size = 0
            meaningful_size = 0
            preview = ""
        collision = next((
            other for other in coff.symbols.values()
            if other.name == semantic and other.index != symbol.index
        ), None)
        if collision is not None:
            raise ValueError(
                f"reviewed real-literal identity collides with {semantic}")
        renames[symbol.index] = semantic
        rows.append(CanonicalRow(
            symbol.name, semantic, "semantic-real-comdat", storage, section,
            offset, physical_size, meaningful_size, 0, "-",
            "source-DATA_COMPGEN-external-COMDAT", preview,
        ))
    return renames, tuple(rows)


def _rewrite_jump_table_relocations(
        original: CoffObject, payload: bytes) -> tuple[bytes, tuple[JumpTableRewrite, ...]]:
    """Rewrite same-function .text DIR32 sites to owner+relative addend."""
    data = bytearray(payload)
    ranges = _function_ranges(original)
    rewrites = []
    for relocation in original.relocations:
        if relocation.typ != DIR32:
            continue
        section = original.sections[relocation.section - 1]
        if not section.characteristics & MEM_EXECUTE:
            continue
        target = original.symbols[relocation.symbol_index]
        if target.section != relocation.section:
            continue
        is_local_label = target.typ == 0 and target.storage_class == 6
        is_external_function = (
            target.typ == FUNCTION_TYPE and target.storage_class == EXTERNAL_STORAGE)
        if not is_local_label and not is_external_function:
            continue
        if relocation.site + 4 > section.raw_size:
            raise ValueError("DIR32 jump-table relocation crosses .text payload")
        operand_offset = section.raw_offset + relocation.site
        original_addend = struct.unpack_from("<I", original.data, operand_offset)[0]
        resolved_offset = (target.value + original_addend) & 0xFFFFFFFF
        site_owner = _function_owner(ranges, relocation.section, relocation.site)
        target_owner = _function_owner(ranges, target.section, resolved_offset)
        if site_owner is None or target_owner is None or site_owner != target_owner:
            continue
        owner_addend = (resolved_offset - target_owner.value) & 0xFFFFFFFF
        if ((target_owner.value + owner_addend) & 0xFFFFFFFF != resolved_offset):
            raise RuntimeError("jump-table owner/addend resolution changed")
        struct.pack_into("<I", data, operand_offset, owner_addend)
        struct.pack_into("<I", data, relocation.offset + 4, target_owner.index)
        rewrites.append(JumpTableRewrite(
            relocation.offset, relocation.section, relocation.site,
            target.index, target_owner.index, original_addend, owner_addend,
            resolved_offset,
        ))
    return bytes(data), tuple(rewrites)


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


def _assert_only_canonical_changes(
        original: CoffObject, payload: bytes, renames: dict[int, str],
        jump_table_rewrites: tuple[JumpTableRewrite, ...],
        equivalent_retargets: dict[int, int] | None = None) -> None:
    equivalent_retargets = equivalent_retargets or {}
    normalized = CoffObject(payload)
    rewrites_by_offset = {
        rewrite.relocation_offset: rewrite for rewrite in jump_table_rewrites
    }
    if len(rewrites_by_offset) != len(jump_table_rewrites):
        raise RuntimeError("duplicate jump-table rewrite record")
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
        before_payload = bytearray(original.section_bytes(before))
        after_payload = bytearray(normalized.section_bytes(after))
        for rewrite in jump_table_rewrites:
            if rewrite.section != before.index:
                continue
            expected_before = struct.pack("<I", rewrite.original_addend)
            expected_after = struct.pack("<I", rewrite.owner_addend)
            site = rewrite.site
            if (before_payload[site:site + 4] != expected_before or
                    after_payload[site:site + 4] != expected_after):
                raise RuntimeError("canonical COFF emitted an unexpected jump-table addend")
            before_payload[site:site + 4] = bytes(4)
            after_payload[site:site + 4] = bytes(4)
        if before_payload != after_payload:
            raise RuntimeError("canonical COFF changed unexpected section payload bytes")
    if len(original.relocations) != len(normalized.relocations):
        raise RuntimeError("canonical COFF changed relocation count")
    for before, after in zip(original.relocations, normalized.relocations):
        rewrite = rewrites_by_offset.get(before.offset)
        expected_symbol = (rewrite.owner_symbol_index if rewrite is not None
                           else equivalent_retargets.get(
                               before.symbol_index, before.symbol_index))
        if (before.offset, before.section, before.site, before.typ) != (
                after.offset, after.section, after.site, after.typ):
            raise RuntimeError("canonical COFF changed relocation site/type/order")
        if after.symbol_index != expected_symbol:
            raise RuntimeError("canonical COFF changed an unexpected relocation target")
        if rewrite is not None and before.symbol_index != rewrite.original_symbol_index:
            raise RuntimeError("jump-table rewrite source-symbol postcondition failed")
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
    for rewrite in jump_table_rewrites:
        section = original.sections[rewrite.section - 1]
        operand = section.raw_offset + rewrite.site
        before_prefix[operand:operand + 4] = bytes(4)
        after_prefix[operand:operand + 4] = bytes(4)
        before_prefix[rewrite.relocation_offset + 4:
                      rewrite.relocation_offset + 8] = bytes(4)
        after_prefix[rewrite.relocation_offset + 4:
                     rewrite.relocation_offset + 8] = bytes(4)
    for relocation in original.relocations:
        if relocation.symbol_index in equivalent_retargets:
            before_prefix[relocation.offset + 4:relocation.offset + 8] = bytes(4)
            after_prefix[relocation.offset + 4:relocation.offset + 8] = bytes(4)
    if before_prefix != after_prefix:
        raise RuntimeError(
            "canonical COFF changed bytes outside symbol names/jump-table relocations")

    normalized_relocations = {row.offset: row for row in normalized.relocations}
    for rewrite in jump_table_rewrites:
        before_relocation = next(
            row for row in original.relocations
            if row.offset == rewrite.relocation_offset)
        after_relocation = normalized_relocations[rewrite.relocation_offset]
        before_target = original.symbols[before_relocation.symbol_index]
        after_target = normalized.symbols[after_relocation.symbol_index]
        before_section = original.sections[before_relocation.section - 1]
        after_section = normalized.sections[after_relocation.section - 1]
        before_addend = struct.unpack_from(
            "<I", original.data,
            before_section.raw_offset + before_relocation.site)[0]
        after_addend = struct.unpack_from(
            "<I", payload,
            after_section.raw_offset + after_relocation.site)[0]
        before_resolved = (
            before_target.section,
            (before_target.value + before_addend) & 0xFFFFFFFF,
        )
        after_resolved = (
            after_target.section,
            (after_target.value + after_addend) & 0xFFFFFFFF,
        )
        expected = (rewrite.section, rewrite.resolved_offset)
        if before_resolved != expected or after_resolved != expected:
            raise RuntimeError(
                "jump-table relocation resolved-target postcondition failed")


def defer_data_comparison(payload: bytes) -> bytes:
    """Remove data from a comparison copy until the data campaign models it.

    Without reviewed DATA/DATA_COMPGEN identities, comparing data sections can
    only mislead: anonymous payloads pair by content while placement stays
    unproven. This pass empties every non-executable data section header and
    turns its symbols into plain undefined externals, so objdiff sees the unit
    as code only. Comparison-copy surgery exclusively - raw objects are
    untouched.
    """
    coff = CoffObject(payload)
    data = bytearray(payload)
    deferred = set()
    for section in coff.sections:
        if not section.characteristics & (INITIALIZED_DATA | UNINITIALIZED_DATA):
            continue
        if section.characteristics & (MEM_EXECUTE | LNK_INFO):
            continue
        deferred.add(section.index)
        header = section.header_offset
        struct.pack_into("<I", data, header + 8, 0)    # VirtualSize
        struct.pack_into("<I", data, header + 16, 0)   # SizeOfRawData
        struct.pack_into("<I", data, header + 20, 0)   # PointerToRawData
        struct.pack_into("<I", data, header + 24, 0)   # PointerToRelocations
        struct.pack_into("<H", data, header + 32, 0)   # NumberOfRelocations
        struct.pack_into("<I", data, header + 36,
                         section.characteristics & ~LNK_NRELOC_OVFL)
    if not deferred:
        return payload
    for symbol in coff.symbols.values():
        if symbol.section not in deferred:
            continue
        struct.pack_into("<IhHBB", data, symbol.offset + 8,
                         0, 0, symbol.typ, EXTERNAL_STORAGE, symbol.aux_count)
        for aux in range(symbol.aux_count):
            start = symbol.offset + (aux + 1) * SYMBOL_SIZE
            data[start:start + SYMBOL_SIZE] = bytes(SYMBOL_SIZE)
    return bytes(data)


def canonicalize_coff(payload: bytes,
                      compgen: tuple[CompgenClaim, ...] = (),
                      compgen_data: tuple[CompgenDataClaim, ...] = (),
                      real_literal_references: dict[str, str] | None = None,
                      ) -> CanonicalizedObject:
    """Return a normalized comparison copy and its readable rename records."""
    coff = CoffObject(payload)
    definitions = _definitions(coff)
    definition_by_symbol = {row.symbol.index: row for row in definitions}
    section_relocations: dict[int, list[Relocation]] = defaultdict(list)
    for relocation in coff.relocations:
        section_relocations[relocation.section].append(relocation)

    candidates = {
        row.symbol.index: row for row in definitions
        if _family(row.symbol.name) is not None and
        (row.symbol.storage_class == 3 or row.symbol.name.startswith("??_C@"))
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

        # Duplicate literals take their occurrence index from the first
        # executable-section site that references them (code order is what
        # both sides of a byte-proven pair share); definition address order
        # is only the tiebreak and the fallback for unreferenced cells.
        first_reference: dict[int, tuple[int, int]] = {}
        for section in coff.sections:
            if not section.characteristics & MEM_EXECUTE:
                continue
            for relocation in section_relocations[section.index]:
                key = (section.index, relocation.site)
                index = relocation.symbol_index
                if index not in first_reference or key < first_reference[index]:
                    first_reference[index] = key
        for (definition, family, kind, meaningful, proof, preview,
             digest, base_name, record) in sorted(
                 prepared, key=lambda row: (
                     first_reference.get(row[0].symbol.index,
                                         (0x7FFFFFFF, 0x7FFFFFFF)),
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

    compgen_data_rename, compgen_data_rows = _compgen_data_renames(
        coff, definitions, compgen_data)
    if compgen_data_rename:
        claimed_locations = {
            (definition.section.index, definition.start)
            for definition in definitions
            if definition.symbol.index in compgen_data_rename
        }
        rows = [
            row for row in rows
            if (row.section_ordinal, row.section_offset) not in claimed_locations
        ]
        renames.update(compgen_data_rename)
        rows.extend(compgen_data_rows)

    real_reference_rename, real_reference_rows = _real_literal_reference_renames(
        coff, definitions, real_literal_references or {})
    for index, name in real_reference_rename.items():
        existing = renames.get(index)
        if existing is not None and existing != name:
            raise ValueError(
                f"conflicting semantic identities for {coff.symbols[index].name}")
        renames[index] = name
    rows.extend(
        row for row in real_reference_rows
        if not any(existing.original_name == row.original_name
                   and existing.canonical_name == row.canonical_name
                   for existing in rows)
    )

    compgen_rename, compgen_rows = _compgen_renames(coff, compgen)
    overlap = set(renames) & set(compgen_rename)
    if overlap:
        raise RuntimeError("data and compiler-function canonicalization overlap")
    renames.update(compgen_rename)
    rows.extend(compgen_rows)

    # The compiler emits a virtual vector-deleting destructor as a COFF weak
    # external (`??_E...`) whose auxiliary record names the scalar-deleting
    # destructor (`??_G...`) as its default. The linked retail image therefore
    # contains the default address, and the delinked object names that resolved
    # symbol directly. Retarget the disposable comparison copy exactly as the
    # linker did. `assert_weak_external_link_set` supplies the required global
    # precondition: no object in the candidate link defines the weak name.
    defined_external_index = {}
    for symbol in coff.symbols.values():
        if symbol.section > 0 and symbol.storage_class == EXTERNAL_STORAGE:
            defined_external_index.setdefault(symbol.name, symbol.index)
    equivalent_retargets = {}
    for symbol in coff.symbols.values():
        if (symbol.section == 0 and symbol.value == 0 and
                symbol.storage_class == EXTERNAL_STORAGE and
                symbol.index not in renames and
                symbol.name in defined_external_index):
            renames[symbol.index] = "$dup$" + symbol.name
            equivalent_retargets[symbol.index] = defined_external_index[symbol.name]
            rows.append(CanonicalRow(
                symbol.name, renames[symbol.index], "dup", "undefined",
                0, 0, 0, 0, 0, "-", "undef-dup-of-definition", "",
            ))
    for symbol in coff.symbols.values():
        if symbol.storage_class != WEAK_EXTERNAL_STORAGE:
            continue
        if (symbol.section != 0 or symbol.value != 0 or symbol.aux_count != 1):
            raise ValueError(
                f"invalid COFF weak external metadata for {symbol.name}")
        default_index, search = struct.unpack_from(
            "<II", coff.data, symbol.offset + SYMBOL_SIZE)
        default = coff.symbols.get(default_index)
        if default is None or default.index == symbol.index or search not in (1, 2, 3):
            raise ValueError(
                f"invalid COFF weak external default for {symbol.name}")
        equivalent_retargets[symbol.index] = default.index
        rows.append(CanonicalRow(
            symbol.name, symbol.name, "weak", "undefined", 0, 0, 0, 0, 0, "-",
            "weak-external-resolved-to-" + default.name, "",
        ))
    for index in list(equivalent_retargets):
        seen = {index}
        target = equivalent_retargets[index]
        while target in equivalent_retargets and target not in seen:
            seen.add(target)
            target = equivalent_retargets[target]
        if target in seen:
            raise ValueError("cyclic equivalent COFF relocation targets")
        equivalent_retargets[index] = target

    # /Gf turns pooled literals into COMDATs the linker folds image-wide, so
    # a delinked relocation can carry another unit's global occurrence index
    # in its $anon_str name. Both sides therefore renumber every $anon_str
    # symbol they see - defined candidate or external reference - by order
    # of first executable-section reference: a per-object, code-ordered
    # identity that byte-proven pairs share.
    anon_stems = {}
    for index, symbol in coff.symbols.items():
        match = ANON_STR.fullmatch(renames.get(index, symbol.name))
        if match:
            anon_stems[index] = match.group(1)
    if anon_stems:
        anon_first_site = {}
        for relocation in coff.relocations:
            section = coff.sections[relocation.section - 1]
            if not section.characteristics & MEM_EXECUTE:
                continue
            index = relocation.symbol_index
            if index in anon_stems:
                key = (relocation.section, relocation.site)
                if index not in anon_first_site or key < anon_first_site[index]:
                    anon_first_site[index] = key
        counters = {}
        for index in sorted(anon_stems, key=lambda i: (
                anon_first_site.get(i, (0x7FFFFFFF, 0x7FFFFFFF)), i)):
            stem = anon_stems[index]
            occurrence = counters.get(stem, 0)
            counters[stem] = occurrence + 1
            renumbered = f"{stem}_{occurrence}"
            if renumbered != renames.get(index, coff.symbols[index].name):
                renames[index] = renumbered

    normalized = _rewrite_names(coff, renames)
    normalized, jump_table_rewrites = _rewrite_jump_table_relocations(
        coff, normalized)
    if equivalent_retargets:
        data = bytearray(normalized)
        jump_table_offsets = {
            rewrite.relocation_offset for rewrite in jump_table_rewrites
        }
        for relocation in coff.relocations:
            if (relocation.symbol_index in equivalent_retargets and
                    relocation.offset not in jump_table_offsets):
                struct.pack_into(
                    "<I", data, relocation.offset + 4,
                    equivalent_retargets[relocation.symbol_index])
        normalized = bytes(data)
    _assert_only_canonical_changes(
        coff, normalized, renames, jump_table_rewrites,
        equivalent_retargets)
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


def load_compgen_claims(path: Path | None, unit: str | None):
    if path is None or unit is None:
        return ()
    if not path.exists():
        raise FileNotFoundError("compiler-function manifest does not exist: %s" % path)
    with path.open(newline="") as stream:
        rows = csv.DictReader(stream)
        return tuple(CompgenClaim(
            row["name"], row["kind"], row["owner"], int(row["size"], 0))
            for row in rows if row["unit"] == unit)


def load_compgen_data_claims(path: Path | None, unit: str | None):
    if path is None or unit is None:
        return ()
    if not path.exists():
        raise FileNotFoundError(
            f"compiler-data manifest does not exist: {path}")
    with path.open(newline="") as stream:
        rows = csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter="\t")
        claims = []
        for row in rows:
            provenance = row["provenance"]
            if not provenance.startswith((
                    "source-DATA_COMPGEN:", "candidate-COFF-real:")):
                continue
            object_unit = row["object"].replace("\\", "/")
            if object_unit.lower().endswith(".c"):
                object_unit = object_unit[:-2]
            if object_unit != unit:
                continue
            claims.append(CompgenDataClaim(
                row["name"],
                int(row["section_ordinal"], 0),
                int(row["section_offset"], 0),
                int(row["size"], 0),
                row["storage"],
                row["scope"],
                ("source-DATA_COMPGEN" if provenance.startswith(
                    "source-DATA_COMPGEN:") else "candidate-COFF-real"),
            ))
        return tuple(claims)


def load_real_literal_reference_renames(
        path: Path | None, base_root: Path | None) -> dict[str, str]:
    """Derive global VC6 real-COMDAT aliases from reviewed owner coordinates."""
    if path is None or base_root is None:
        return {}
    coff_by_unit = {}
    aliases = {}
    with path.open(newline="") as stream:
        rows = csv.DictReader(
            (line for line in stream if not line.lstrip().startswith("#")),
            delimiter="\t")
        for row in rows:
            if (not row["provenance"].startswith("source-DATA_COMPGEN:") or
                    row["scope"] != "external"):
                continue
            object_unit = row["object"].replace("\\", "/")
            if object_unit.lower().endswith(".c"):
                object_unit = object_unit[:-2]
            coff = coff_by_unit.get(object_unit)
            if coff is None:
                coff = CoffObject(
                    (Path(base_root) / f"{object_unit}.obj").read_bytes())
                coff_by_unit[object_unit] = coff
            section = int(row["section_ordinal"], 0)
            offset = int(row["section_offset"], 0)
            matches = [
                symbol for symbol in coff.symbols.values()
                if (symbol.section == section and symbol.value == offset and
                    symbol.typ == 0 and
                    symbol.storage_class == EXTERNAL_STORAGE)
            ]
            if len(matches) != 1:
                raise ValueError(
                    f"{row['name']} has {len(matches)} external owner symbols at "
                    f"{object_unit}:{section}+0x{offset:x}")
            original = matches[0].name
            if REAL_LITERAL.fullmatch(original) is None:
                continue
            previous = aliases.get(original)
            if previous is not None and previous != row["name"]:
                raise ValueError(
                    f"{original} has conflicting semantic identities: "
                    f"{previous}, {row['name']}")
            aliases[original] = row["name"]
    return aliases


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--sidecar", type=Path)
    parser.add_argument("--unit")
    parser.add_argument("--compgen-manifest", type=Path)
    parser.add_argument("--data-manifest", type=Path)
    parser.add_argument("--data-base-root", type=Path)
    parser.add_argument("--defer-data", action="store_true",
                        help="drop data sections/symbols from the comparison "
                             "copy until the data campaign models storage")
    parser.add_argument("--summary-root", type=Path, action="append")
    parser.add_argument("--summary-output", type=Path)
    parser.add_argument("--assert-weak-link-set", type=Path, nargs="+")
    parser.add_argument("--weak-stamp", type=Path)
    args = parser.parse_args(argv)
    if args.assert_weak_link_set:
        if (args.input or args.output or args.sidecar or args.summary_root or
                args.summary_output or not args.weak_stamp):
            parser.error("weak-link-set mode requires only input objects and --weak-stamp")
        count = assert_weak_external_link_set(args.assert_weak_link_set)
        _atomic_write(
            args.weak_stamp,
            ("# candidate whole-link weak-external proof\n"
             f"objects\t{len(args.assert_weak_link_set)}\n"
             f"weak_names\t{count}\n").encode("utf-8"))
        print(
            f"[normalize-data] whole-link weak-external proof: "
            f"{count} names across {len(args.assert_weak_link_set)} objects")
        return 0
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
    claims = load_compgen_claims(args.compgen_manifest, args.unit)
    data_claims = load_compgen_data_claims(args.data_manifest, args.unit)
    real_literal_references = load_real_literal_reference_renames(
        args.data_manifest, args.data_base_root)
    result = canonicalize_coff(
        args.input.read_bytes(), claims, data_claims,
        real_literal_references)
    payload = result.data
    if args.defer_data:
        payload = defer_data_comparison(payload)
    _atomic_write(args.output, payload)
    _atomic_write(args.sidecar, sidecar_bytes(result.rows))
    stamp_inputs = {"input": args.input}
    if args.compgen_manifest:
        stamp_inputs["compgen_manifest"] = args.compgen_manifest
    if args.data_manifest:
        stamp_inputs["data_manifest"] = args.data_manifest
    write_stamp(args.output, stamp_inputs)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
