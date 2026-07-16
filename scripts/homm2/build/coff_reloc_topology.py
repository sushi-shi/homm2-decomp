"""Compare whole-object data relocation topology in two COFF objects.

This is deliberately independent of objdiff's allocation JSON.  It parses raw
i386 COFF section, symbol, and relocation tables and emits a lossless census plus
per-site residuals suitable for correcting object-level data topology.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import io
import json
import re
import struct
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path


COFF_HEADER = struct.Struct("<HHIIIHH")
SECTION_HEADER = struct.Struct("<8sIIIIIIHHI")
SYMBOL = struct.Struct("<8sIhHBB")
RELOCATION = struct.Struct("<IIH")

IMAGE_FILE_MACHINE_I386 = 0x014C
IMAGE_SCN_CNT_CODE = 0x00000020
IMAGE_SCN_CNT_INITIALIZED_DATA = 0x00000040
IMAGE_SCN_CNT_UNINITIALIZED_DATA = 0x00000080
IMAGE_SCN_LNK_INFO = 0x00000200
IMAGE_SCN_LNK_NRELOC_OVFL = 0x01000000

RELOCATION_NAMES = {
    0x0000: "ABSOLUTE",
    0x0001: "DIR16",
    0x0002: "REL16",
    0x0006: "DIR32",
    0x0007: "DIR32NB",
    0x0009: "SEG12",
    0x000A: "SECTION",
    0x000B: "SECREL",
    0x000C: "TOKEN",
    0x000D: "SECREL7",
    0x0014: "REL32",
}
RELOCATION_WIDTHS = {
    0x0000: 0,
    0x0001: 2,
    0x0002: 2,
    0x0006: 4,
    0x0007: 4,
    0x0009: 2,
    0x000A: 2,
    0x000B: 4,
    0x000C: 4,
    0x000D: 1,
    0x0014: 4,
}
STORAGE_CLASS_NAMES = {
    2: "external",
    3: "static",
    6: "label",
    101: "function",
    103: "file",
    105: "weak-external",
}
DATA_SECTION_CLASSES = {"initialized-data", "uninitialized-data"}
SYNTHETIC_PREFIXES = (
    "const_", "string_", "data_", "bss_", "unresolved_", "fallback_",
    "__homm2_data_alias$",
)
SYNTHETIC_NAMES = {"empty_stub"}
DATA_MARKER_RE = re.compile(r"^\s*DATA\(0x([0-9a-fA-F]+)\)\s+(.*)$")
csv.field_size_limit(sys.maxsize)


@dataclass(frozen=True)
class Section:
    index: int
    name: str
    size: int
    raw_offset: int
    relocation_offset: int
    relocation_count: int
    characteristics: int

    @property
    def section_class(self) -> str:
        if self.characteristics & IMAGE_SCN_CNT_CODE:
            return "code"
        if self.characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA:
            return "uninitialized-data"
        if self.characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA:
            return "initialized-data"
        if self.characteristics & IMAGE_SCN_LNK_INFO:
            return "link-info"
        return "other"


@dataclass(frozen=True)
class Symbol:
    index: int
    name: str
    value: int
    section_number: int
    type: int
    storage_class: int
    auxiliary_count: int

    @property
    def status(self) -> str:
        if self.section_number > 0:
            return "defined"
        if self.section_number == -1:
            return "absolute"
        if self.section_number == -2:
            return "debug"
        if self.value and self.storage_class in (2, 105):
            return "common"
        return "undefined"


class CoffError(ValueError):
    pass


def _slice(data: bytes, offset: int, size: int, label: str) -> bytes:
    if offset < 0 or size < 0 or offset + size > len(data):
        raise CoffError(f"{label} lies outside the COFF object")
    return data[offset:offset + size]


def _cstring(data: bytes, offset: int, label: str) -> str:
    if offset < 4 or offset >= len(data):
        raise CoffError(f"{label} has invalid string-table offset {offset:#x}")
    end = data.find(b"\0", offset)
    if end < 0:
        raise CoffError(f"{label} is not NUL terminated")
    return data[offset:end].decode("latin1")


def _name(raw: bytes, strings: bytes, label: str) -> str:
    zeroes, offset = struct.unpack("<II", raw)
    if zeroes == 0:
        return _cstring(strings, offset, label)
    return raw.split(b"\0", 1)[0].decode("latin1")


def _section_name(raw: bytes, strings: bytes, label: str) -> str:
    short = raw.split(b"\0", 1)[0].decode("latin1")
    if not short.startswith("/"):
        return short
    try:
        offset = int(short[1:], 10)
    except ValueError as error:
        raise CoffError(f"{label} has invalid long-name reference {short!r}") from error
    return _cstring(strings, offset, label)


def _signed_value(data: bytes) -> int:
    return int.from_bytes(data, "little", signed=True)


class CoffObject:
    def __init__(self, path: Path):
        self.path = path
        self.data = path.read_bytes()
        self.machine = 0
        self.sections: list[Section] = []
        self.symbols: list[Symbol] = []
        self.symbol_slots: list[Symbol | None] = []
        self.relocations: list[dict] = []
        self._parse()

    def _parse(self) -> None:
        if len(self.data) < COFF_HEADER.size:
            raise CoffError(f"{self.path}: truncated COFF header")
        (self.machine, section_count, _, symbol_offset, symbol_count,
         optional_size, _) = COFF_HEADER.unpack_from(self.data)
        if self.machine != IMAGE_FILE_MACHINE_I386:
            raise CoffError(
                f"{self.path}: machine {self.machine:#x} is not i386 COFF"
            )
        section_table = COFF_HEADER.size + optional_size
        _slice(
            self.data, section_table, section_count * SECTION_HEADER.size,
            f"{self.path}: section table",
        )
        if symbol_count == 0 and symbol_offset == 0:
            symbol_bytes = b""
            strings = b"\x04\0\0\0"
        else:
            symbol_bytes = _slice(
                self.data, symbol_offset, symbol_count * SYMBOL.size,
                f"{self.path}: symbol table",
            )
            string_offset = symbol_offset + len(symbol_bytes)
            string_length = struct.unpack(
                "<I", _slice(self.data, string_offset, 4, f"{self.path}: string table")
            )[0]
            if string_length < 4:
                raise CoffError(f"{self.path}: invalid string-table length {string_length}")
            strings = _slice(
                self.data, string_offset, string_length, f"{self.path}: string table"
            )

        section_rows = []
        for ordinal in range(section_count):
            values = SECTION_HEADER.unpack_from(
                self.data, section_table + ordinal * SECTION_HEADER.size
            )
            section_rows.append(values)
        for ordinal, values in enumerate(section_rows, 1):
            (raw_name, _, _, size, raw_offset, reloc_offset, _, reloc_count,
             _, characteristics) = values
            self.sections.append(Section(
                index=ordinal,
                name=_section_name(raw_name, strings, f"section {ordinal}"),
                size=size,
                raw_offset=raw_offset,
                relocation_offset=reloc_offset,
                relocation_count=reloc_count,
                characteristics=characteristics,
            ))

        self.symbol_slots = [None] * symbol_count
        index = 0
        while index < symbol_count:
            raw_name, value, section_number, type_, storage_class, aux_count = (
                SYMBOL.unpack_from(symbol_bytes, index * SYMBOL.size)
            )
            if index + aux_count >= symbol_count:
                raise CoffError(f"{self.path}: symbol {index} auxiliary records overflow table")
            symbol = Symbol(
                index=index,
                name=_name(raw_name, strings, f"symbol {index}"),
                value=value,
                section_number=section_number,
                type=type_,
                storage_class=storage_class,
                auxiliary_count=aux_count,
            )
            self.symbols.append(symbol)
            self.symbol_slots[index] = symbol
            index += 1 + aux_count

        for section in self.sections:
            self._parse_relocations(section)

    def _parse_relocations(self, section: Section) -> None:
        count = section.relocation_count
        offset = section.relocation_offset
        if count == 0:
            return
        if (count == 0xFFFF
                and section.characteristics & IMAGE_SCN_LNK_NRELOC_OVFL):
            overflow = RELOCATION.unpack(
                _slice(self.data, offset, RELOCATION.size,
                       f"{self.path}: relocation overflow record")
            )
            count = overflow[0] - 1
            offset += RELOCATION.size
        payload = _slice(
            self.data, offset, count * RELOCATION.size,
            f"{self.path}: {section.name} relocations",
        )
        for ordinal in range(count):
            address, symbol_index, relocation_type = RELOCATION.unpack_from(
                payload, ordinal * RELOCATION.size
            )
            if symbol_index >= len(self.symbol_slots):
                raise CoffError(
                    f"{self.path}: relocation target {symbol_index} is outside symbol table"
                )
            symbol = self.symbol_slots[symbol_index]
            if symbol is None:
                raise CoffError(
                    f"{self.path}: relocation targets auxiliary symbol slot {symbol_index}"
                )
            width = RELOCATION_WIDTHS.get(relocation_type)
            field = b""
            addend = None
            if width:
                field = (b"\0" * width if section.section_class == "uninitialized-data"
                         and section.raw_offset == 0 else _slice(
                    self.data, section.raw_offset + address, width,
                    f"{self.path}: relocation field {section.name}+{address:#x}",
                ))
                addend = _signed_value(field)
            self.relocations.append({
                "ordinal": ordinal,
                "section_index": section.index,
                "section_name": section.name,
                "section_class": section.section_class,
                "offset": address,
                "type": relocation_type,
                "type_name": RELOCATION_NAMES.get(
                    relocation_type, f"UNKNOWN_{relocation_type:#06x}"
                ),
                "width": width,
                "field_bytes": field.hex() if width is not None else None,
                "addend": addend,
                "symbol_index": symbol.index,
                "symbol_name": symbol.name,
                "symbol_status": symbol.status,
                "symbol_section": symbol.section_number,
                "symbol_value": symbol.value,
                "symbol_storage_class": symbol.storage_class,
                "symbol_storage_name": STORAGE_CLASS_NAMES.get(
                    symbol.storage_class, str(symbol.storage_class)
                ),
            })

    def section_for_symbol(self, symbol: Symbol) -> Section | None:
        if 1 <= symbol.section_number <= len(self.sections):
            return self.sections[symbol.section_number - 1]
        return None

    def owner(self, symbol: Symbol, addend: int | None) -> dict:
        effective_addend = addend or 0
        section = self.section_for_symbol(symbol)
        if section is None:
            return {
                "name": symbol.name,
                "offset": effective_addend,
                "section_class": symbol.status,
            }
        destination = symbol.value + effective_addend
        external = [candidate for candidate in self.symbols
                    if candidate.section_number == symbol.section_number
                    and candidate.storage_class == 2
                    and candidate.value <= destination]
        candidates = external or [candidate for candidate in self.symbols
                                  if candidate.section_number == symbol.section_number
                                  and candidate.storage_class not in (3, 6, 103)
                                  and candidate.value <= destination]
        if candidates:
            owner = max(candidates, key=lambda candidate: (candidate.value, -candidate.index))
            return {
                "name": owner.name,
                "offset": destination - owner.value,
                "section_class": section.section_class,
            }
        return {
            "name": f"<section:{section.name}>",
            "offset": destination,
            "section_class": section.section_class,
        }

    def census(self, include_code: bool = False) -> dict:
        included = []
        for relocation in self.relocations:
            if not include_code and relocation["section_class"] not in DATA_SECTION_CLASSES:
                continue
            symbol = self.symbol_slots[relocation["symbol_index"]]
            assert symbol is not None
            row = dict(relocation)
            row["target_section_class"] = (
                self.section_for_symbol(symbol).section_class
                if self.section_for_symbol(symbol) else symbol.status
            )
            row["owner"] = self.owner(symbol, relocation["addend"])
            included.append(row)
        return {
            "path": str(self.path),
            "sha256": hashlib.sha256(self.data).hexdigest(),
            "machine": self.machine,
            "sections": [{
                "index": section.index,
                "name": section.name,
                "class": section.section_class,
                "size": section.size,
                "characteristics": section.characteristics,
                "relocation_count": section.relocation_count,
            } for section in self.sections],
            "symbols": [{
                "index": symbol.index,
                "name": symbol.name,
                "value": symbol.value,
                "section_number": symbol.section_number,
                "section_class": (
                    self.section_for_symbol(symbol).section_class
                    if self.section_for_symbol(symbol) else symbol.status
                ),
                "type": symbol.type,
                "storage_class": symbol.storage_class,
                "storage_name": STORAGE_CLASS_NAMES.get(
                    symbol.storage_class, str(symbol.storage_class)
                ),
                "status": symbol.status,
                "auxiliary_count": symbol.auxiliary_count,
            } for symbol in self.symbols],
            "relocations": included,
        }


def _load_mappings(path: Path | None) -> dict:
    if path is None:
        return {"symbol_mappings": {}, "section_mappings": {}}
    document = json.loads(path.read_text())
    if document.get("schema") != 1:
        raise CoffError("mapping document must have schema 1")
    for key in ("symbol_mappings", "section_mappings"):
        if not isinstance(document.get(key, {}), dict):
            raise CoffError(f"mapping document {key} must be an object")
    symbols = {}
    for target_name, value in document.get("symbol_mappings", {}).items():
        entry = {"base_name": value, "status": "proven", "addend_adjustment": 0}
        if isinstance(value, dict):
            entry = dict(value)
            entry.setdefault("status", "proven")
            entry.setdefault("addend_adjustment", 0)
        if not isinstance(entry.get("base_name"), str) or not entry["base_name"]:
            raise CoffError(f"symbol mapping {target_name!r} has no base_name")
        if _synthetic_identity(target_name) or _synthetic_identity(entry["base_name"]):
            raise CoffError(f"symbol mapping {target_name!r} uses a forbidden synthetic identity")
        if entry["status"] not in ("proven", "provisional"):
            raise CoffError(f"symbol mapping {target_name!r} has invalid status")
        if not isinstance(entry["addend_adjustment"], int):
            raise CoffError(f"symbol mapping {target_name!r} has non-integer addend adjustment")
        provenance = str(entry.get("provenance", ""))
        if "unresolved" in provenance.lower() or "fallback" in provenance.lower():
            raise CoffError(
                f"symbol mapping {target_name!r} uses forbidden {provenance!r} provenance"
            )
        symbols[target_name] = entry
    return {
        "symbol_mappings": symbols,
        "section_mappings": document.get("section_mappings", {}),
    }


def _mapping(mappings: dict, name: str, target: bool) -> dict | None:
    return mappings["symbol_mappings"].get(name) if target else None


def _mapped_name(mappings: dict, name: str, target: bool) -> str:
    entry = _mapping(mappings, name, target)
    if entry and entry["status"] == "proven":
        return entry["base_name"]
    return name


def _normalized(row: dict, mappings: dict, target: bool) -> dict:
    sections = mappings["section_mappings"] if target else {}
    symbol_mapping = _mapping(mappings, row["symbol_name"], target)
    owner_mapping = _mapping(mappings, row["owner"]["name"], target)
    addend_adjustment = (
        symbol_mapping["addend_adjustment"]
        if symbol_mapping and symbol_mapping["status"] == "proven" else 0
    )
    owner_adjustment = (
        owner_mapping["addend_adjustment"]
        if owner_mapping and owner_mapping["status"] == "proven" else 0
    )
    return {
        "section": sections.get(row["section_name"], row["section_name"]),
        "section_class": row["section_class"],
        "offset": row["offset"],
        "type": row["type"],
        "symbol": _mapped_name(mappings, row["symbol_name"], target),
        "symbol_status": row["symbol_status"],
        "owner": _mapped_name(mappings, row["owner"]["name"], target),
        "owner_offset": row["owner"]["offset"] + owner_adjustment,
        "target_section_class": row["target_section_class"],
        "addend": (row["addend"] + addend_adjustment
                    if row["addend"] is not None else None),
        "width": row["width"],
    }


def _fingerprint(row: dict) -> tuple:
    return tuple(row[key] for key in (
        "section", "section_class", "offset", "type", "symbol", "symbol_status",
        "owner", "owner_offset", "target_section_class", "addend", "width",
    ))


def _site(row: dict) -> tuple:
    return row["section"], row["offset"]


def _differences(target: dict, base: dict) -> list[str]:
    return [key for key in target if target[key] != base.get(key)]


def _provisional_mapping(row: dict, mappings: dict) -> dict | None:
    entry = _mapping(mappings, row["symbol_name"], True)
    if not entry or entry["status"] != "provisional":
        return None
    return {"target_name": row["symbol_name"], **entry}


def compare_censuses(target: dict, base: dict, mappings: dict) -> tuple[list[dict], int]:
    target_rows = [(row, _normalized(row, mappings, True))
                   for row in target["relocations"]]
    base_rows = [(row, _normalized(row, mappings, False))
                 for row in base["relocations"]]
    target_counter = Counter(_fingerprint(normalized) for _, normalized in target_rows)
    base_counter = Counter(_fingerprint(normalized) for _, normalized in base_rows)
    matched = sum((target_counter & base_counter).values())

    target_sites: dict[tuple, list[tuple[dict, dict]]] = {}
    base_sites: dict[tuple, list[tuple[dict, dict]]] = {}
    for original, normalized in target_rows:
        target_sites.setdefault(_site(normalized), []).append((original, normalized))
    for original, normalized in base_rows:
        base_sites.setdefault(_site(normalized), []).append((original, normalized))

    residuals = []
    for site in sorted(set(target_sites) | set(base_sites)):
        target_site = target_sites.get(site, [])
        base_site = base_sites.get(site, [])
        target_values = Counter(_fingerprint(row) for _, row in target_site)
        base_values = Counter(_fingerprint(row) for _, row in base_site)
        if target_values == base_values:
            continue
        if not target_site:
            kind = "extra-in-base"
        elif not base_site:
            kind = "missing-in-base"
        elif sum(target_values.values()) != sum(base_values.values()):
            kind = "multiplicity-mismatch"
        else:
            kind = "property-mismatch"
        differing = set()
        if len(target_site) == 1 and len(base_site) == 1:
            differing.update(_differences(target_site[0][1], base_site[0][1]))
        else:
            differing.add("multiplicity" if kind == "multiplicity-mismatch" else "topology")
        residuals.append({
            "kind": kind,
            "section": site[0],
            "offset": site[1],
            "differences": sorted(differing),
            "provisional_mapping": next((
                note for original, _ in target_site
                if (note := _provisional_mapping(original, mappings)) is not None
            ), None),
            "target": [{"relocation": original, "normalized": normalized}
                       for original, normalized in target_site],
            "base": [{"relocation": original, "normalized": normalized}
                     for original, normalized in base_site],
        })
    return residuals, matched


def _definition_name(code: str) -> str | None:
    code = code.split("//", 1)[0]
    if not (code[:1].isalpha() or code[:1] == "_"):
        return None
    declaration = code.split("=", 1)[0].rstrip().rstrip(";{").rstrip()
    match = re.search(
        r"([A-Za-z_]\w*)\s*(?:\[[^\]]*\]\s*)*$",
        declaration,
    )
    return match.group(1) if match else None


def _source_data_anchors(root: Path) -> tuple[dict[str, dict], list[dict]]:
    inventory = root / "build" / "gen" / "symbol_names.csv"
    identifier_symbols: dict[str, list[dict]] = {}
    if inventory.is_file():
        with inventory.open(encoding="latin-1", newline="") as handle:
            for row in csv.DictReader(handle):
                if row.get("kind") != "data":
                    continue
                match = (re.match(r"\?([A-Za-z_]\w*)@@", row["name"])
                         or re.match(r"[_@]?([A-Za-z_]\w*)", row["name"]))
                if match:
                    identifier_symbols.setdefault(match.group(1), []).append(row)
    anchors = {}
    diagnostics = []
    for source in sorted((root / "src").glob("**/*.cpp")):
        lines = source.read_text(encoding="latin-1").splitlines()
        for line_number, line in enumerate(lines, 1):
            marker = DATA_MARKER_RE.match(line)
            if not marker:
                continue
            declaration = marker.group(2)
            cursor = line_number
            while (not any(token in declaration for token in ("=", ";", "{"))
                   and cursor < len(lines) and cursor < line_number + 8):
                declaration += " " + lines[cursor].strip()
                cursor += 1
            name = _definition_name(declaration)
            if not name:
                continue
            va = int(marker.group(1), 16)
            rva = va - 0x400000 if va >= 0x400000 else va
            rows = identifier_symbols.get(name, [])
            symbol_name = rows[0]["name"] if len(rows) == 1 else name
            unit = str(source.relative_to(root / "src").with_suffix(""))
            anchor_key = symbol_name if len(rows) == 1 else f"{unit}::{name}"
            entry = {
                "symbol": symbol_name,
                "identifier": name,
                "unit": unit,
                "rva": rva,
                "source": f"{source.relative_to(root)}:{line_number}",
                "provenance": "source-DATA",
            }
            if anchor_key in anchors:
                diagnostics.append({
                    "kind": "duplicate-source-DATA",
                    "symbol": anchor_key,
                    "first": anchors[anchor_key],
                    "duplicate": entry,
                })
            else:
                anchors[anchor_key] = entry
    return anchors, diagnostics


def _load_tsv(path: Path) -> list[dict]:
    if not path.is_file():
        return []
    lines = [line for line in path.read_text(encoding="utf-8").splitlines()
             if line and not line.startswith("#")]
    return list(csv.DictReader(io.StringIO("\n".join(lines)), delimiter="\t"))


def load_homm2_provenance(root: Path) -> dict:
    root = root.resolve()
    anchors, diagnostics = _source_data_anchors(root)
    supplemental = _load_tsv(root / "config" / "delink_data_topology.tsv")
    supplemental_anchors = {}
    for row in supplemental:
        name = row.get("name", "")
        if not isinstance(name, str) or not name:
            diagnostics.append({"kind": "malformed-supplemental-row", "row": row})
            continue
        try:
            rva = int(row["rva"], 0)
        except (KeyError, ValueError):
            diagnostics.append({"kind": "malformed-supplemental-row", "row": row})
            continue
        if name in anchors:
            diagnostics.append({
                "kind": ("supplemental-data-duplication"
                         if anchors[name]["rva"] == rva
                         else "supplemental-data-disagreement"),
                "symbol": name,
                "source_DATA": anchors[name],
                "supplemental_rva": rva,
                "supplemental_object": row.get("object"),
            })
            continue
        supplemental_anchors[name] = {
            "symbol": name,
            "rva": rva,
            "source": "config/delink_data_topology.tsv",
            "provenance": row.get("provenance", "supplemental"),
        }
    unresolved = []
    for row in _load_tsv(root / "config" / "delink_unresolved_data.tsv"):
        try:
            start = int(row["rva"], 0)
            size = int(row["size"], 0)
        except (KeyError, ValueError):
            diagnostics.append({"kind": "malformed-unresolved-row", "row": row})
            continue
        unresolved.append({
            "start": start,
            "end": start + size,
            "object": row.get("object"),
            "provenance": row.get("provenance"),
        })
    return {
        "root": str(root),
        "anchors": {**supplemental_anchors, **anchors},
        "source_DATA_count": len(anchors),
        "supplemental_count": len(supplemental_anchors),
        "unresolved_ranges": unresolved,
        "diagnostics": diagnostics,
    }


def _synthetic_identity(name: str) -> str | None:
    lower = name.lower()
    if lower in SYNTHETIC_NAMES:
        return "synthetic-name"
    if lower.startswith(SYNTHETIC_PREFIXES):
        return "synthetic-prefix"
    return None


def _target_policy_errors(target: dict, provenance: dict | None) -> list[dict]:
    errors = []
    anchors = provenance["anchors"] if provenance else {}
    unresolved = provenance["unresolved_ranges"] if provenance else []
    for symbol in target["symbols"]:
        reason = _synthetic_identity(symbol["name"])
        if reason:
            errors.append({
                "kind": "forbidden-synthetic-symbol",
                "reason": reason,
                "symbol": symbol["name"],
                "symbol_index": symbol["index"],
                "symbol_status": symbol["status"],
            })
    for relocation in target["relocations"]:
        for field in ("symbol_name",):
            name = relocation[field]
            reason = _synthetic_identity(name)
            if reason:
                errors.append({
                    "kind": "forbidden-synthetic-identity",
                    "reason": reason,
                    "section": relocation["section_name"],
                    "offset": relocation["offset"],
                    "symbol": name,
                })
        owner_name = relocation["owner"]["name"]
        owner_reason = _synthetic_identity(owner_name)
        if owner_reason and owner_name != relocation["symbol_name"]:
            errors.append({
                "kind": "forbidden-synthetic-owner",
                "reason": owner_reason,
                "section": relocation["section_name"],
                "offset": relocation["offset"],
                "symbol": owner_name,
            })
        anchor = anchors.get(relocation["symbol_name"])
        if not anchor:
            continue
        destination = anchor["rva"] + (relocation["addend"] or 0)
        unresolved_owner = next(
            (row for row in unresolved if row["start"] <= destination < row["end"]), None
        )
        if unresolved_owner:
            errors.append({
                "kind": "unresolved-range-generated-identity",
                "section": relocation["section_name"],
                "offset": relocation["offset"],
                "symbol": relocation["symbol_name"],
                "destination_rva": destination,
                "unresolved_range": unresolved_owner,
            })
    return errors


def compare_pair(unit: str, base_path: Path, target_path: Path, mappings: dict,
                 include_code: bool = False, provenance: dict | None = None) -> dict:
    base_object = CoffObject(base_path)
    target_object = CoffObject(target_path)
    base = base_object.census(include_code)
    target = target_object.census(include_code)
    residuals, matched = compare_censuses(target, base, mappings)
    policy_errors = _target_policy_errors(target_object.census(True), provenance)
    return {
        "unit": unit,
        "target": target,
        "base": base,
        "summary": {
            "target_relocations": len(target["relocations"]),
            "base_relocations": len(base["relocations"]),
            "matched_relocations": matched,
            "residual_sites": len(residuals),
            "policy_errors": len(policy_errors),
        },
        "residuals": residuals,
        "policy_errors": policy_errors,
    }


def _pairs(args) -> list[dict]:
    if args.manifest:
        document = json.loads(args.manifest.read_text())
        if document.get("schema") != 1 or not isinstance(document.get("pairs"), list):
            raise CoffError("pair manifest must have schema 1 and a pairs array")
        return document["pairs"]
    if not args.base or not args.target:
        raise CoffError("provide BASE TARGET or --manifest PAIRS.json")
    return [{"unit": args.unit or args.base.stem, "base": str(args.base),
             "target": str(args.target)}]


def run(args) -> tuple[dict, int]:
    mappings = _load_mappings(args.mappings)
    provenance = load_homm2_provenance(args.homm2_root) if args.homm2_root else None
    units = []
    errors = []
    for pair in _pairs(args):
        try:
            units.append(compare_pair(
                str(pair["unit"]), Path(pair["base"]), Path(pair["target"]),
                mappings, args.include_code, provenance,
            ))
        except (KeyError, OSError, CoffError, ValueError) as error:
            errors.append({"unit": pair.get("unit", "<unknown>"), "error": str(error)})
    report = {
        "schema": 1,
        "scope": "all-relocation-sections" if args.include_code else "data-sections",
        "summary": {
            "units": len(units),
            "units_with_residuals": sum(
                bool(unit["residuals"] or unit["policy_errors"]) for unit in units
            ),
            "residual_sites": sum(len(unit["residuals"]) for unit in units),
            "policy_errors": sum(len(unit["policy_errors"]) for unit in units),
            "provenance_diagnostics": (
                len(provenance["diagnostics"]) if provenance else 0
            ),
            "errors": len(errors),
        },
        "units": units,
        "errors": errors,
        "provenance": provenance,
    }
    failed = bool(
        errors
        or (provenance and provenance["diagnostics"])
        or any(unit["residuals"] or unit["policy_errors"] for unit in units)
    )
    return report, int(failed)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("base", nargs="?", type=Path, help="candidate base COFF object")
    parser.add_argument("target", nargs="?", type=Path, help="delink target COFF object")
    parser.add_argument("--unit", help="translation-unit label for a single pair")
    parser.add_argument("--manifest", type=Path, help="schema-1 JSON pair manifest")
    parser.add_argument("--mappings", type=Path,
                        help="schema-1 target-to-base symbol/section mappings")
    parser.add_argument("--homm2-root", type=Path,
                        help="enforce source-DATA and canonical topology provenance")
    parser.add_argument("--include-code", action="store_true",
                        help="include code/linker sections instead of data sections only")
    parser.add_argument("-o", "--output", type=Path, required=True)
    args = parser.parse_args(argv)
    try:
        report, result = run(args)
    except (OSError, CoffError, ValueError) as error:
        parser.error(str(error))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    summary = report["summary"]
    print(
        f"COFF data topology: {summary['units']} units, "
        f"{summary['units_with_residuals']} with residuals, "
        f"{summary['residual_sites']} residual sites, "
        f"{summary['policy_errors']} policy errors, "
        f"{summary['provenance_diagnostics']} provenance diagnostics, "
        f"{summary['errors']} parse errors"
    )
    if result:
        print(f"topology mismatch; review {args.output}", file=sys.stderr)
    return result


if __name__ == "__main__":
    raise SystemExit(main())
