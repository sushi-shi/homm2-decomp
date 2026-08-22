#!/usr/bin/env python3
"""Canonicalize equivalent delinked relocation identities using paired sites.

The synthetic PDB has to name every absolute relocation target so the delinker
can represent it in COFF.  Unnamed fields therefore become ``const_<RVA>``
symbols with an implicit addend of zero, while MSVC naturally emits a public
owner symbol plus a field offset.  Objdiff's ``data_value`` relocation mode
correctly treats those spellings as different.

This pass uses the current candidate only as site-specific proof of an
equivalent spelling. A relocation is rewritten when:

* both sides name the same function and have a relocation at the same
  function-relative site;
* a candidate DIR32 names either a source-claimed data symbol or an imported
  address-table slot recovered independently from the retail import directory;
* ``public owner RVA + candidate addend == retail PE target RVA`` exactly; or
* a candidate REL32 names a COMDAT whose candidate body is byte-identical to
  the retail-named COMDAT at that same site.

A wrong candidate offset cannot authorize a rewrite and remains visible to
strict objdiff. Equivalent REL32 aliases are likewise rewritten only at paired
sites when both names resolve to the same unique public RVA and carry the same
COFF addend. The disposable target receives a separate undefined COFF symbol
for each proven identity, so a synthetic symbol shared by unrelated relocations
is never renamed globally. Input and output paths must differ.

The pass also appends a local ``$fnpad@<offset>`` boundary symbol after each
function whose delinked ``.text`` span runs past its reviewed retail size and
whose remainder is one to fifteen bytes of pure ``0x90``/``0xCC`` alignment
fill. The delinked blob attributes that linker fill to the preceding function
span; padding is not part of any function, and objdiff only strips it by
itself when the tail is code. A function that ends in embedded switch-table
data otherwise keeps the fill inside its compared extent and can never reach
an exact score. The boundary symbol restores the reviewed size as the
comparison extent without deleting, masking, or reordering any byte. A span
whose remainder contains any other byte value, exceeds fifteen bytes, or has
no reviewed size claim is left untouched.

The delinker can also omit a recursive ``REL32`` relocation because the linked
displacement already targets the containing function. When the candidate has a
zero-addend relocation to that same public function at the same operand site,
and the raw retail displacement resolves exactly to the reviewed entry RVA, the
disposable target receives the missing record. This restores lost COFF metadata
without changing the linked instruction bytes.
"""

import argparse
import csv
import re
import shutil
import struct
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import NamedTuple

from homm2.build.assert_relocs import (
    IMAGE_BASE, _pe_read, folded_comdat_symbols, load_symbols, parse_obj,
    resolve,
)
from homm2.build.normalized_freshness import write_stamp


DIR32 = 0x0006
REL32 = 0x0014
SYMBOL_SIZE = 18
OUTPUT_MARKER = ".homm2-reloc-canonical"
SYM_CLASS_STATIC = 3
ALIGNMENT_FILL = {0x90, 0xCC}
ALIGNMENT_FILL_LIMIT = 16

# Compiler-local EH machinery spellings: FuncInfo records and handler
# funclets ($L..., __ehhandler$..., __unwindfunclet$...) live in .text$x,
# which the linker groups at the end of .text - their linked addresses are
# unknowable from the compiland, so identity is site-paired, never resolved.
EH_LOCAL = re.compile(r"^\$L\d+$|^__ehhandler\$|^__unwindfunclet\$|^__catch\$")
SYNTHETIC_TARGET = re.compile(r"^const_[0-9a-f]{8}$|^Unwind@[0-9a-f]{8}$")


class Section(NamedTuple):
    index: int
    name: str
    raw_size: int
    raw_offset: int
    reloc_offset: int
    reloc_count: int


class Symbol(NamedTuple):
    index: int
    offset: int
    name: str
    value: int
    section: int
    aux_count: int


class Relocation(NamedTuple):
    offset: int
    symbol_index: int
    typ: int


@dataclass
class Coverage:
    functions: int = 0
    paired_functions: int = 0
    base_sites: int = 0
    same_site_same_type: int = 0
    missing_target_site: int = 0
    type_mismatch: int = 0
    unresolved_base: int = 0
    unresolved_target: int = 0
    duplicate_string_ambiguity: int = 0
    unknown_compiler_local: int = 0

    def merge(self, other):
        for name in self.__dataclass_fields__:
            setattr(self, name, getattr(self, name) + getattr(other, name))


class CoffFile:
    def __init__(self, path):
        self.path = Path(path)
        self.data = bytearray(self.path.read_bytes())
        if len(self.data) < 20:
            raise ValueError("short COFF file: %s" % self.path)
        self.section_count = struct.unpack_from("<H", self.data, 2)[0]
        self.symbol_offset = struct.unpack_from("<I", self.data, 8)[0]
        self.symbol_count = struct.unpack_from("<I", self.data, 12)[0]
        optional_size = struct.unpack_from("<H", self.data, 16)[0]
        self.sections = self._read_sections(20 + optional_size)
        self.string_offset = self.symbol_offset + self.symbol_count * SYMBOL_SIZE
        if self.string_offset + 4 > len(self.data):
            raise ValueError("missing COFF string table: %s" % self.path)
        self.string_size = struct.unpack_from("<I", self.data, self.string_offset)[0]
        if self.string_offset + self.string_size != len(self.data):
            raise ValueError("COFF string table is not final: %s" % self.path)
        self.symbols = self._read_symbols()
        self.relocations = self._read_relocations()
        self._string_offsets = self._read_string_offsets()
        self._new_symbols = {}
        self._new_boundaries = []
        self._new_relocs = []

    def _read_sections(self, first):
        sections = []
        for zero_index in range(self.section_count):
            off = first + zero_index * 40
            name = bytes(self.data[off:off + 8]).rstrip(b"\0").decode("latin-1")
            raw_size, raw_offset, reloc_offset = struct.unpack_from(
                "<III", self.data, off + 16)
            reloc_count = struct.unpack_from("<H", self.data, off + 32)[0]
            sections.append(Section(
                zero_index + 1, name, raw_size, raw_offset,
                reloc_offset, reloc_count))
        return sections

    def _name_at(self, offset):
        raw = bytes(self.data[offset:offset + 8])
        zero, string_offset = struct.unpack("<II", raw)
        if zero == 0:
            if not 4 <= string_offset < self.string_size:
                raise ValueError("invalid COFF string offset in %s" % self.path)
            start = self.string_offset + string_offset
            end = self.data.index(0, start, self.string_offset + self.string_size)
            return bytes(self.data[start:end]).decode("latin-1")
        return raw.split(b"\0", 1)[0].decode("latin-1")

    def _read_symbols(self):
        symbols = {}
        index = 0
        while index < self.symbol_count:
            off = self.symbol_offset + index * SYMBOL_SIZE
            name = self._name_at(off)
            value, section, _typ, _storage, aux_count = struct.unpack_from(
                "<IhHBB", self.data, off + 8)
            symbols[index] = Symbol(index, off, name, value, section, aux_count)
            index += 1 + aux_count
        return symbols

    def _read_relocations(self):
        relocations = {}
        for section in self.sections:
            for index in range(section.reloc_count):
                off = section.reloc_offset + index * 10
                site, symbol_index, typ = struct.unpack_from("<IIH", self.data, off)
                relocations[(section.index, site)] = Relocation(
                    off, symbol_index, typ)
        return relocations

    def _read_string_offsets(self):
        offsets = {}
        pos = 4
        while pos < self.string_size:
            start = self.string_offset + pos
            end = self.data.index(0, start, self.string_offset + self.string_size)
            offsets.setdefault(bytes(self.data[start:end]), pos)
            pos = end - self.string_offset + 1
        return offsets

    def unique_text_functions(self):
        text_sections = {section.index for section in self.sections
                         if section.name == ".text"}
        found = defaultdict(list)
        for symbol in self.symbols.values():
            if symbol.section in text_sections:
                found[symbol.name].append(symbol)
        return {name: symbols[0] for name, symbols in found.items()
                if len(symbols) == 1}

    def add_alignment_boundary(self, function, claimed_size):
        """Bound one function span at its reviewed size across proven fill.

        Returns True only when the span between the claimed end and the next
        symbol (or section end) is one to fifteen bytes of pure 0x90/0xCC
        linker alignment fill. Anything else leaves the object unchanged.
        """
        if claimed_size <= 0:
            return False
        section = self.sections[function.section - 1]
        if section.raw_offset == 0:
            return False
        claim_end = function.value + claimed_size
        span_end = min(
            (symbol.value for symbol in self.symbols.values()
             if symbol.section == function.section
             and symbol.value > function.value),
            default=section.raw_size)
        if not claim_end < span_end <= section.raw_size:
            return False
        if span_end - claim_end >= ALIGNMENT_FILL_LIMIT:
            return False
        fill = self.data[section.raw_offset + claim_end:
                         section.raw_offset + span_end]
        if any(byte not in ALIGNMENT_FILL for byte in fill):
            return False
        self._new_boundaries.append(
            ("$fnpad@%x" % claim_end, claim_end, function.section))
        return True

    def patch_dir32(self, function, site, expected_symbol, new_symbol, addend):
        absolute_site = function.value + site
        reloc = self.relocations.get((function.section, absolute_site))
        if reloc is None or reloc.typ != DIR32:
            return False
        symbol = self.symbols.get(reloc.symbol_index)
        if symbol is None or symbol.name != expected_symbol:
            return False
        section = self.sections[function.section - 1]
        if absolute_site + 4 > section.raw_size:
            return False
        new_index = self._new_symbols.setdefault(
            new_symbol, self.symbol_count + len(self._new_symbols))
        struct.pack_into("<I", self.data, reloc.offset + 4, new_index)
        struct.pack_into("<I", self.data,
                         section.raw_offset + absolute_site, addend & 0xFFFFFFFF)
        return True

    def _insert_relocation(self, function, site, symbol_name, typ,
                           symbol_index=None):
        """Queue a relocation at a site that has none (field bytes kept)."""
        absolute_site = function.value + site
        if (function.section, absolute_site) in self.relocations:
            return False
        section = self.sections[function.section - 1]
        if absolute_site + 4 > section.raw_size:
            return False
        if symbol_index is None:
            symbol_index = self._new_symbols.setdefault(
                symbol_name, self.symbol_count + len(self._new_symbols))
        self._new_relocs.append(
            (function.section, absolute_site, symbol_index, typ))
        return True

    def insert_dir32(self, function, site, symbol_name):
        """Queue a DIR32 record at a site that has none (field bytes kept).

        Used only for fs:[0] chain links (__except_list): the compiled
        object carries a relocation against the absolute zero of the TIB
        slot while the field itself stays zero, so adding the record to the
        disposable comparison copy changes no byte of the section.
        """
        return self._insert_relocation(function, site, symbol_name, DIR32)

    def insert_rel32(self, function, site, symbol_name):
        """Queue a REL32 record at a proven recursive-call operand."""
        if function.name != symbol_name:
            return False
        if not self._insert_relocation(
                function, site, symbol_name, REL32, function.index):
            return False
        section = self.sections[function.section - 1]
        struct.pack_into(
            "<I", self.data, section.raw_offset + function.value + site, 0)
        return True

    def patch_rel32(self, function, site, expected_symbol, new_symbol):
        absolute_site = function.value + site
        reloc = self.relocations.get((function.section, absolute_site))
        if reloc is None or reloc.typ != REL32:
            return False
        symbol = self.symbols.get(reloc.symbol_index)
        if symbol is None or symbol.name != expected_symbol:
            return False
        new_index = self._new_symbols.setdefault(
            new_symbol, self.symbol_count + len(self._new_symbols))
        struct.pack_into("<I", self.data, reloc.offset + 4, new_index)
        return True

    def finish(self):
        if (not self._new_symbols and not self._new_boundaries
                and not self._new_relocs):
            self.path.write_bytes(self.data)
            return
        string_table = bytearray(
            self.data[self.string_offset:self.string_offset + self.string_size])
        records = bytearray()

        def name_field(name):
            encoded = name.encode("latin-1")
            if len(encoded) <= 8:
                return encoded.ljust(8, b"\0")
            string_offset = self._string_offsets.get(encoded)
            if string_offset is None:
                string_offset = len(string_table)
                self._string_offsets[encoded] = string_offset
                string_table.extend(encoded + b"\0")
            return struct.pack("<II", 0, string_offset)

        for name, _symbol_index in sorted(
                self._new_symbols.items(), key=lambda item: item[1]):
            records.extend(name_field(name))
            records.extend(struct.pack("<IhHBB", 0, 0, 0, 2, 0))
        for name, value, section in sorted(
                self._new_boundaries, key=lambda item: (item[2], item[1])):
            records.extend(name_field(name))
            records.extend(struct.pack(
                "<IhHBB", value, section, 0, SYM_CLASS_STATIC, 0))
        struct.pack_into("<I", string_table, 0, len(string_table))
        prefix = bytearray(self.data[:self.symbol_offset])
        tables = bytearray()
        if self._new_relocs:
            # Rebuild each touched section's relocation table between the
            # raw data and the symbol table (COFF reads it through the
            # header pointer) with the queued records merged in site order.
            first_header = 20 + struct.unpack_from("<H", self.data, 16)[0]
            by_section = defaultdict(list)
            for section_index, site, symbol_index, typ in self._new_relocs:
                by_section[section_index].append((site, symbol_index, typ))
            for section_index, additions in sorted(by_section.items()):
                section = self.sections[section_index - 1]
                table = [bytes(self.data[section.reloc_offset + k * 10:
                                         section.reloc_offset + k * 10 + 10])
                         for k in range(section.reloc_count)]
                table.extend(struct.pack("<IIH", site, symbol_index, typ)
                             for site, symbol_index, typ in additions)
                table.sort(key=lambda record: struct.unpack_from("<I", record)[0])
                header = first_header + (section_index - 1) * 40
                struct.pack_into("<I", prefix, header + 24,
                                 self.symbol_offset + len(tables))
                struct.pack_into("<H", prefix, header + 32, len(table))
                tables.extend(b"".join(table))
            struct.pack_into("<I", prefix, 8, self.symbol_offset + len(tables))
        self.data = (prefix + tables +
                     self.data[self.symbol_offset:self.string_offset] +
                     records + string_table)
        struct.pack_into("<I", self.data, 12,
                         self.symbol_count + len(self._new_symbols) +
                         len(self._new_boundaries))
        self.path.write_bytes(self.data)


def load_retail_symbols(path):
    public_data = {}
    function_rvas = {}
    function_sizes = {}
    with open(path, encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "func":
                function_rvas[(row["unit"], row["name"])] = int(row["rva"], 0)
                function_sizes[(row["unit"], row["name"])] = int(row["size"], 0)
            elif row.get("kind") == "data":
                public_data[row["name"]] = int(row["rva"], 0)
    return public_data, function_rvas, function_sizes


REAL_LITERAL = re.compile(r"^__real@(4|8)@([0-9a-f]{20})$")


def real_literal_bytes(symbol):
    """The IEEE bytes a VC6 __real@ symbol's cell must hold, or None.

    The mangled digits are the constant in x87 80-bit extended form
    (1 sign, 15 exponent biased 16383, 64 mantissa with explicit
    integer bit); the cell holds it narrowed to float or double.
    """
    match = REAL_LITERAL.match(symbol)
    if match is None:
        return None
    width = int(match.group(1))
    bits = int(match.group(2), 16)
    sign = -1.0 if bits >> 79 else 1.0
    exponent = (bits >> 64) & 0x7FFF
    mantissa = bits & 0xFFFFFFFFFFFFFFFF
    if exponent == 0 and mantissa == 0:
        value = 0.0 * sign
    else:
        value = sign * (mantissa / float(1 << 63)) * 2.0 ** (exponent - 16383)
    return struct.pack("<f" if width == 4 else "<d", value)


def authorize_owner_alias(public_data, base_type, base_symbol, base_addend,
                          retail_target_rva):
    """Return the public owner name only for an exact resolved-address proof."""
    if base_type != "DIR32":
        return None
    owner_rva = public_data.get(base_symbol)
    if owner_rva is None:
        return None
    if (owner_rva + base_addend) & 0xFFFFFFFF != retail_target_rva:
        return None
    return base_symbol


def authorize_import_alias(import_iat, base_type, base_symbol, base_addend,
                           retail_target_rva):
    """Return a candidate import-pointer spelling for its exact retail IAT slot."""
    if base_type != "DIR32" or base_addend != 0:
        return None
    slot = import_iat.get(base_symbol)
    if slot is None:
        stdcall = re.fullmatch(
            r"__imp__([A-Za-z][A-Za-z0-9_]*)@[0-9]+", base_symbol or "")
        if stdcall is not None:
            slot = import_iat.get(("stdcall", stdcall.group(1)))
    if slot is None:
        cdecl = re.fullmatch(
            r"__imp__([A-Za-z][A-Za-z0-9_]*)", base_symbol or "")
        if cdecl is not None:
            slot = import_iat.get(("cdecl", cdecl.group(1)))
    if slot != retail_target_rva:
        return None
    return base_symbol


def load_import_iat_symbols(path):
    """Map unambiguous COFF ``__imp_`` names to retail IAT slot RVAs."""
    payload = Path(path).read_bytes()
    if len(payload) < 0x40 or payload[:2] != b"MZ":
        raise ValueError("not a PE image: %s" % path)
    pe = struct.unpack_from("<I", payload, 0x3C)[0]
    if pe + 24 > len(payload) or payload[pe:pe + 4] != b"PE\0\0":
        raise ValueError("invalid PE header: %s" % path)
    section_count = struct.unpack_from("<H", payload, pe + 6)[0]
    optional_size = struct.unpack_from("<H", payload, pe + 20)[0]
    optional = pe + 24
    if (optional + optional_size > len(payload) or optional_size < 112 or
            struct.unpack_from("<H", payload, optional)[0] != 0x10B):
        raise ValueError("expected a PE32 optional header: %s" % path)
    sections = []
    first_section = optional + optional_size
    for index in range(section_count):
        offset = first_section + index * 40
        if offset + 40 > len(payload):
            raise ValueError("truncated PE section table: %s" % path)
        virtual_size, rva, raw_size, raw = struct.unpack_from(
            "<IIII", payload, offset + 8)
        sections.append((rva, max(virtual_size, raw_size), raw_size, raw))

    def raw_offset(rva):
        for start, span, raw_size, raw in sections:
            delta = rva - start
            if 0 <= delta < span and delta < raw_size:
                return raw + delta
        return None

    def read_string(rva):
        offset = raw_offset(rva)
        if offset is None:
            raise ValueError("unmapped import string RVA 0x%x in %s" % (rva, path))
        end = payload.find(b"\0", offset)
        if end < 0:
            raise ValueError("unterminated import string in %s" % path)
        return payload[offset:end].decode("latin-1")

    import_rva = struct.unpack_from("<I", payload, optional + 104)[0]
    descriptor = raw_offset(import_rva) if import_rva else None
    candidates = defaultdict(set)
    while descriptor is not None:
        if descriptor + 20 > len(payload):
            raise ValueError("truncated import descriptor in %s" % path)
        original, _stamp, _chain, name_rva, first = struct.unpack_from(
            "<IIIII", payload, descriptor)
        if not (original or first or name_rva):
            break
        lookup = raw_offset(original or first)
        if lookup is None:
            raise ValueError("unmapped import lookup table in %s" % path)
        slot = first
        while True:
            if lookup + 4 > len(payload):
                raise ValueError("truncated import lookup table in %s" % path)
            entry = struct.unpack_from("<I", payload, lookup)[0]
            if entry == 0:
                break
            if not entry & 0x80000000:
                import_name = read_string(entry + 2)
                candidates["__imp_" + import_name].add(slot)
                if re.fullmatch(r"[A-Za-z][A-Za-z0-9_]*", import_name):
                    candidates[("stdcall", import_name)].add(slot)
                    candidates[("cdecl", import_name)].add(slot)
            lookup += 4
            slot += 4
        descriptor += 20
    return {
        name: next(iter(slots))
        for name, slots in candidates.items()
        if len(slots) == 1
    }


def authorize_rel32_alias(symbols, data, duplicates, base, target):
    """Return the candidate spelling for one proven same-address call target."""
    base_type, base_symbol, base_addend = base
    target_type, target_symbol, target_addend = target
    if base_type != "REL32" or target_type != "REL32":
        return None
    if base_symbol == target_symbol or base_addend != target_addend:
        return None
    if (_has_duplicate_name(duplicates, base) or
            _has_duplicate_name(duplicates, target)):
        return None
    base_rva = resolve(symbols, data, *base)
    target_rva = resolve(symbols, data, *target)
    if base_rva is None or base_rva != target_rva:
        return None
    return base_symbol


def authorize_missing_self_rel32(function_rva, function_size, function_name,
                                  site, base, retail_opcode, retail_operand):
    """Return the self symbol when linked bytes prove a lost REL32 record."""
    base_type, base_symbol, base_addend = base
    if (base_type != "REL32" or base_symbol != function_name
            or base_addend != 0 or retail_operand is None
            or len(retail_operand) != 4 or site < 0
            or site + 4 > function_size or retail_opcode != b"\xe8"):
        return None
    displacement = struct.unpack("<i", retail_operand)[0]
    retail_target = (function_rva + site + 4 + displacement) & 0xFFFFFFFF
    if retail_target != function_rva:
        return None
    return base_symbol


def function_inventory(function_rvas):
    result = defaultdict(set)
    for unit, name in function_rvas:
        result[unit].add(name)
    return result


def _has_duplicate_name(duplicates, relocation):
    _typ, symbol, _addend = relocation
    return len(duplicates.get(symbol, set())) > 1


def record_site_coverage(coverage, base, target, symbols, data, duplicates):
    if target is None:
        coverage.missing_target_site += 1
        return False
    if base[0] != target[0]:
        coverage.type_mismatch += 1
        return False
    coverage.same_site_same_type += 1
    duplicate = (_has_duplicate_name(duplicates, base) or
                 _has_duplicate_name(duplicates, target))
    if duplicate:
        coverage.duplicate_string_ambiguity += 1
    base_rva = resolve(symbols, data, *base)
    target_rva = resolve(symbols, data, *target)
    if base_rva is None:
        coverage.unresolved_base += 1
    if target_rva is None:
        coverage.unresolved_target += 1
    if (base_rva is None or target_rva is None) and not duplicate:
        coverage.unknown_compiler_local += 1
    return True


def canonicalize_unit(unit, names, public_data, function_rvas, function_sizes,
                      symbols, data, duplicates, import_iat, base_path,
                      target_path):
    base_sites = parse_obj(str(base_path), with_sites=True, include_imports=True)
    target_sites = parse_obj(
        str(target_path), with_sites=True, include_imports=True)
    target = CoffFile(target_path)
    target_functions = target.unique_text_functions()
    patched_functions = set()
    patched_aliases = set()
    patched_sites = 0
    coverage = Coverage(functions=len(names))
    for name in sorted(names):
        if (name not in base_sites or name not in target_sites or
                name not in target_functions or
                (unit, name) not in function_rvas):
            continue
        coverage.paired_functions += 1
        function_rva = function_rvas[(unit, name)]
        target_by_site = {reloc[0]: reloc[1:] for reloc in target_sites[name]}
        folded_symbols = folded_comdat_symbols(
            str(base_path), symbols, data, base_sites[name], target_sites[name])
        coverage.base_sites += len(base_sites[name])
        for site, base_type, base_symbol, base_addend in base_sites[name]:
            target_reloc = target_by_site.get(site)
            base = (base_type, base_symbol, base_addend)
            if (target_reloc is None and base_type == "DIR32"
                    and base_symbol == "__except_list" and base_addend == 0):
                # fs:[0] chain link: our object relocates the TIB-slot-zero
                # operand while the linked field is 0. The retail operand
                # must read 0; the record is then added to the disposable
                # copy without touching any section byte.
                retail_operand = _pe_read(function_rva + site, 4)
                if retail_operand == b"\x00\x00\x00\x00" and target.insert_dir32(
                        target_functions[name], site, "__except_list"):
                    patched_functions.add(name)
                    patched_sites += 1
                    continue
            if target_reloc is None and base_type == "REL32":
                self_symbol = authorize_missing_self_rel32(
                    function_rva, function_sizes[(unit, name)], name, site, base,
                    _pe_read(function_rva + site - 1, 1),
                    _pe_read(function_rva + site, 4))
                if (self_symbol is not None and target.insert_rel32(
                        target_functions[name], site, self_symbol)):
                    patched_functions.add(name)
                    patched_sites += 1
                    continue
            if not record_site_coverage(
                    coverage, base, target_reloc, symbols, data, duplicates):
                continue
            target_type, target_symbol, target_addend = target_reloc
            if target_type == "REL32" and base_type == "REL32":
                alias = None
                if (base_addend == target_addend and
                        folded_symbols.get(base_symbol) == target_symbol):
                    alias = base_symbol
                if alias is None:
                    alias = authorize_rel32_alias(
                        symbols, data, duplicates, base, target_reloc)
                if alias is None:
                    continue
                if not target.patch_rel32(
                        target_functions[name], site, target_symbol, alias):
                    continue
                patched_functions.add(name)
                patched_aliases.add((target_symbol, base_symbol, base_addend))
                patched_sites += 1
                continue
            if target_type != "DIR32" or base_type != "DIR32":
                continue
            retail_operand = _pe_read(function_rva + site, 4)
            if retail_operand is None:
                continue
            retail_target_rva = (
                struct.unpack("<I", retail_operand)[0] - IMAGE_BASE) & 0xFFFFFFFF
            owner = authorize_owner_alias(
                public_data, base_type, base_symbol, base_addend,
                retail_target_rva)
            if owner is None:
                owner = authorize_import_alias(
                    import_iat, base_type, base_symbol, base_addend,
                    retail_target_rva)
            if owner is None and EH_LOCAL.match(base_symbol or ""):
                # Compiler-local EH machinery (.text$x FuncInfo records and
                # handler funclets): the linked address is unknowable from
                # the compiland, so identity is site-paired. Only synthetic
                # delinker aliases are renamed; the delinker constructed
                # owner + addend to equal the retail operand, so rewriting
                # both to our spelling preserves the resolved address.
                if SYNTHETIC_TARGET.match(target_symbol or ""):
                    owner = base_symbol
            if (owner is None and base_addend == 0
                    and SYNTHETIC_TARGET.match(target_symbol or "")):
                # Float literal pool: the retail cell must hold exactly the
                # IEEE bytes the __real@ spelling encodes - content identity,
                # the same proof the string channel uses.
                expected = real_literal_bytes(base_symbol or "")
                if expected is not None and expected == _pe_read(
                        retail_target_rva, len(expected)):
                    owner = base_symbol
            if owner is None or (target_symbol, target_addend) == base:
                continue
            if not target.patch_dir32(
                    target_functions[name], site, target_symbol,
                    owner, base_addend):
                continue
            patched_functions.add(name)
            patched_aliases.add((target_symbol, base_symbol, base_addend))
            patched_sites += 1
    boundaries = 0
    for name in sorted(names):
        function = target_functions.get(name)
        claimed_size = function_sizes.get((unit, name))
        if function is None or claimed_size is None:
            continue
        if target.add_alignment_boundary(function, claimed_size):
            boundaries += 1
    target.finish()
    return (len(patched_functions), len(patched_aliases), patched_sites,
            boundaries, coverage)


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--base-dir", default="build/objdiff/base")
    parser.add_argument("--target-dir", default="build/delink")
    parser.add_argument("--output-dir", default="build/delink-paired")
    parser.add_argument("--symbols", default="build/gen/symbol_names.csv")
    parser.add_argument("--unit")
    parser.add_argument("--base")
    parser.add_argument("--target")
    parser.add_argument("--output")
    parser.add_argument("--retail-exe", default="build/orig/HMM2PL.exe")
    args = parser.parse_args(argv)
    import_iat = load_import_iat_symbols(args.retail_exe)
    single_paths = (args.base, args.target, args.output)
    if args.unit or any(single_paths):
        if not args.unit or not all(single_paths):
            parser.error("--unit, --base, --target, and --output must be used together")
        output = Path(args.output)
        if Path(args.target).resolve() == output.resolve():
            parser.error("--target and --output must differ")
        output.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(args.target, output)
        public_data, function_rvas, function_sizes = load_retail_symbols(
            args.symbols)
        symbols, data, duplicates = load_symbols()
        names = function_inventory(function_rvas).get(args.unit, set())
        functions, aliases, sites, boundaries, coverage = canonicalize_unit(
            args.unit, names, public_data, function_rvas, function_sizes,
            symbols, data, duplicates, import_iat, Path(args.base), output)
        write_stamp(output, {
            "target": Path(args.target),
            "base": Path(args.base),
            "symbols": Path(args.symbols),
        })
        print("canonicalized %d relocation sites (%d aliases, %d functions, "
              "%d alignment boundaries)" % (sites, aliases, functions,
                                            boundaries))
        print("coverage: functions=%d paired_functions=%d base_sites=%d "
              "same_site_same_type=%d missing_target_site=%d type_mismatch=%d "
              "unresolved_base=%d unresolved_target=%d "
              "duplicate_string_ambiguity=%d unknown_compiler_local=%d" % (
                  coverage.functions, coverage.paired_functions,
                  coverage.base_sites, coverage.same_site_same_type,
                  coverage.missing_target_site, coverage.type_mismatch,
                  coverage.unresolved_base, coverage.unresolved_target,
                  coverage.duplicate_string_ambiguity,
                  coverage.unknown_compiler_local))
        return 0
    base_dir = Path(args.base_dir)
    target_dir = Path(args.target_dir)
    output_dir = Path(args.output_dir)
    if target_dir.resolve() == output_dir.resolve():
        raise SystemExit("target and output directories must differ")
    if output_dir.exists():
        if not (output_dir / OUTPUT_MARKER).is_file():
            raise SystemExit("refusing to replace unmarked output directory: %s" %
                             output_dir)
        shutil.rmtree(output_dir)
    shutil.copytree(target_dir, output_dir)
    (output_dir / OUTPUT_MARKER).write_text(
        "generated by homm2.build.canonicalize_relocs\n")

    public_data, function_rvas, function_sizes = load_retail_symbols(
        args.symbols)
    symbols, data, duplicates = load_symbols()
    inventory = function_inventory(function_rvas)
    function_count = alias_count = site_count = boundary_count = 0
    unit_count = 0
    coverage = Coverage()
    for unit, names in sorted(inventory.items()):
        base_path = base_dir / (unit + ".obj")
        target_path = output_dir / (unit + ".c.obj")
        if not names or not base_path.exists() or not target_path.exists():
            continue
        functions, aliases, sites, boundaries, unit_coverage = canonicalize_unit(
            unit, names, public_data, function_rvas, function_sizes, symbols,
            data, duplicates, import_iat, base_path, target_path)
        coverage.merge(unit_coverage)
        boundary_count += boundaries
        if sites:
            unit_count += 1
            function_count += functions
            alias_count += aliases
            site_count += sites
    print("canonicalized %d relocation sites (%d aliases, %d functions, "
          "%d units, %d alignment boundaries)" %
          (site_count, alias_count, function_count, unit_count,
           boundary_count))
    print("coverage: functions=%d paired_functions=%d base_sites=%d "
          "same_site_same_type=%d missing_target_site=%d type_mismatch=%d "
          "unresolved_base=%d unresolved_target=%d duplicate_string_ambiguity=%d "
          "unknown_compiler_local=%d" % (
              coverage.functions, coverage.paired_functions,
              coverage.base_sites, coverage.same_site_same_type,
              coverage.missing_target_site, coverage.type_mismatch,
              coverage.unresolved_base, coverage.unresolved_target,
              coverage.duplicate_string_ambiguity,
              coverage.unknown_compiler_local))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
