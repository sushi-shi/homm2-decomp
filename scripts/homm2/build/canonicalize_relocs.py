#!/usr/bin/env python3
"""Canonicalize delinked interior-data relocations using exact paired sites.

The synthetic PDB has to name every absolute relocation target so the delinker
can represent it in COFF.  Unnamed fields therefore become ``const_<RVA>``
symbols with an implicit addend of zero, while MSVC naturally emits a public
owner symbol plus a field offset.  Objdiff's ``data_value`` relocation mode
correctly treats those spellings as different.

This pass uses the current candidate only as site-specific proof of an
equivalent spelling.  A relocation is rewritten when:

* the function is byte-exact under objdiff's relocation-masked report;
* both sides have a DIR32 relocation at the same function-relative site;
* the candidate names a CodeView public data symbol; and
* ``public owner RVA + candidate addend == retail PE target RVA`` exactly.

A wrong candidate offset cannot authorize a rewrite and remains visible to
strict objdiff.  Input and output trees must differ: callers regenerate the
output from an immutable raw delink tree on every pass.
"""

import argparse
import csv
import json
import shutil
import struct
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import NamedTuple

from homm2.build.assert_relocs import (
    IMAGE_BASE, _pe_read, load_symbols, parse_obj, resolve,
)


DIR32 = 0x0006
SYMBOL_SIZE = 18
OUTPUT_MARKER = ".homm2-reloc-canonical"


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
    exact_functions: int = 0
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
        self._renames = {}

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

    def patch_dir32(self, function, site, expected_symbol, new_symbol, addend):
        absolute_site = function.value + site
        reloc = self.relocations.get((function.section, absolute_site))
        if reloc is None or reloc.typ != DIR32:
            return False
        symbol = self.symbols.get(reloc.symbol_index)
        if symbol is None or symbol.name != expected_symbol:
            return False
        previous = self._renames.get(symbol.index)
        if previous is not None and previous != new_symbol:
            return False
        section = self.sections[function.section - 1]
        if absolute_site + 4 > section.raw_size:
            return False
        struct.pack_into("<I", self.data,
                         section.raw_offset + absolute_site, addend & 0xFFFFFFFF)
        self._renames[symbol.index] = new_symbol
        return True

    def finish(self):
        for symbol_index, name in sorted(self._renames.items()):
            symbol = self.symbols[symbol_index]
            encoded = name.encode("latin-1")
            if len(encoded) <= 8:
                self.data[symbol.offset:symbol.offset + 8] = encoded.ljust(8, b"\0")
                continue
            string_offset = self._string_offsets.get(encoded)
            if string_offset is None:
                string_offset = self.string_size
                self._string_offsets[encoded] = string_offset
                self.data.extend(encoded + b"\0")
                self.string_size += len(encoded) + 1
            struct.pack_into("<II", self.data, symbol.offset, 0, string_offset)
        struct.pack_into("<I", self.data, self.string_offset, self.string_size)
        self.path.write_bytes(self.data)


def load_retail_symbols(path):
    public_data = {}
    function_rvas = {}
    with open(path, encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "func":
                function_rvas[(row["unit"], row["name"])] = int(row["rva"], 0)
            elif (row.get("kind") == "data" and
                  row.get("provenance") == "cv-public-data"):
                public_data[row["name"]] = int(row["rva"], 0)
    return public_data, function_rvas


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


def exact_functions(report_path):
    report = json.loads(Path(report_path).read_text())
    return {
        unit["name"]: {
            function["name"] for function in unit.get("functions", [])
            if function.get("fuzzy_match_percent") == 100.0
        }
        for unit in report["units"]
    }


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


def canonicalize_unit(unit, names, public_data, function_rvas, symbols, data,
                      duplicates, base_path, target_path):
    base_sites = parse_obj(str(base_path), with_sites=True)
    target_sites = parse_obj(str(target_path), with_sites=True)
    target = CoffFile(target_path)
    target_functions = target.unique_text_functions()
    patched_functions = set()
    patched_aliases = set()
    patched_sites = 0
    coverage = Coverage(exact_functions=len(names))
    for name in sorted(names):
        if (name not in base_sites or name not in target_sites or
                name not in target_functions or
                (unit, name) not in function_rvas):
            continue
        coverage.paired_functions += 1
        function_rva = function_rvas[(unit, name)]
        target_by_site = {reloc[0]: reloc[1:] for reloc in target_sites[name]}
        coverage.base_sites += len(base_sites[name])
        for site, base_type, base_symbol, base_addend in base_sites[name]:
            target_reloc = target_by_site.get(site)
            base = (base_type, base_symbol, base_addend)
            if not record_site_coverage(
                    coverage, base, target_reloc, symbols, data, duplicates):
                continue
            target_type, target_symbol, target_addend = target_reloc
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
            if owner is None or (target_symbol, target_addend) == base:
                continue
            if not target.patch_dir32(
                    target_functions[name], site, target_symbol,
                    owner, base_addend):
                continue
            patched_functions.add(name)
            patched_aliases.add((target_symbol, base_symbol, base_addend))
            patched_sites += 1
    target.finish()
    return len(patched_functions), len(patched_aliases), patched_sites, coverage


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--base-dir", default="build/objdiff/base")
    parser.add_argument("--target-dir", default="build/delink")
    parser.add_argument("--output-dir", default="build/delink-paired")
    parser.add_argument("--report", default="build/objdiff/report.json")
    parser.add_argument("--symbols", default="build/gen/symbol_names.csv")
    args = parser.parse_args(argv)
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

    public_data, function_rvas = load_retail_symbols(args.symbols)
    symbols, data, duplicates = load_symbols()
    exact = exact_functions(args.report)
    function_count = alias_count = site_count = 0
    unit_count = 0
    coverage = Coverage()
    for unit, names in sorted(exact.items()):
        base_path = base_dir / (unit + ".obj")
        target_path = output_dir / (unit + ".c.obj")
        if not names or not base_path.exists() or not target_path.exists():
            continue
        functions, aliases, sites, unit_coverage = canonicalize_unit(
            unit, names, public_data, function_rvas, symbols, data,
            duplicates, base_path, target_path)
        coverage.merge(unit_coverage)
        if sites:
            unit_count += 1
            function_count += functions
            alias_count += aliases
            site_count += sites
    print("canonicalized %d relocation sites (%d aliases, %d exact functions, %d units)" %
          (site_count, alias_count, function_count, unit_count))
    print("coverage: exact_functions=%d paired_functions=%d base_sites=%d "
          "same_site_same_type=%d missing_target_site=%d type_mismatch=%d "
          "unresolved_base=%d unresolved_target=%d duplicate_string_ambiguity=%d "
          "unknown_compiler_local=%d" % (
              coverage.exact_functions, coverage.paired_functions,
              coverage.base_sites, coverage.same_site_same_type,
              coverage.missing_target_site, coverage.type_mismatch,
              coverage.unresolved_base, coverage.unresolved_target,
              coverage.duplicate_string_ambiguity,
              coverage.unknown_compiler_local))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
