#!/usr/bin/env python3
"""Strictly compare reviewed data allocations from objdiff one-shot JSON."""

from __future__ import annotations

import argparse
import base64
import json
import sys
from pathlib import Path


DATA_KINDS = {"SECTION_DATA", "SECTION_BSS", "SECTION_COMMON"}


def _number(value: object) -> int:
    if value is None:
        return 0
    if isinstance(value, int):
        return value
    return int(str(value), 0)


def _payload(symbol: dict) -> bytes:
    chunks: list[bytes] = []
    for segment in symbol.get("data_diff", []):
        if "data" not in segment:
            if segment.get("kind") not in {"DIFF_DELETE", "DIFF_INSERT"}:
                raise ValueError("data_diff segment has neither payload nor diff-side kind")
            continue
        chunk = base64.b64decode(segment["data"])
        size = _number(segment.get("size"))
        if len(chunk) != size:
            raise ValueError(
                f"data_diff segment payload has size {len(chunk):#x}, expected {size:#x}"
            )
        chunks.append(chunk)
    return b"".join(chunks)


def _symbol_section(side: dict, symbol: dict) -> dict:
    if "section" not in symbol:
        raise ValueError(f"{symbol.get('name', '<unnamed>')}: objdiff JSON has no section index")
    index = _number(symbol["section"])
    sections = side.get("sections", [])
    if index < 0 or index >= len(sections):
        raise ValueError(f"{symbol.get('name', '<unnamed>')}: section index {index} is invalid")
    return sections[index]


def _find_symbol(side: dict, name: str, label: str) -> dict:
    matches = [
        symbol
        for symbol in side.get("symbols", [])
        if symbol.get("kind") == "SYMBOL_OBJECT" and symbol.get("name") == name
    ]
    if not matches:
        raise ValueError(f"{label}: missing allocation {name!r}")
    if len(matches) != 1:
        raise ValueError(f"{label}: allocation {name!r} is defined {len(matches)} times")
    return matches[0]


def _effective_extent(symbol: dict, reviewed_extent: int | None, label: str) -> int:
    reported = _number(symbol.get("size"))
    inferred = bool(symbol.get("flags", {}).get("size_inferred"))
    if inferred and reviewed_extent is None:
        raise ValueError(
            f"{label}: extent {reported:#x} is inferred; provide an explicit reviewed extent"
        )
    if reviewed_extent is None:
        return reported
    if reviewed_extent <= 0:
        raise ValueError(f"{label}: reviewed extent must be positive")
    if not inferred and reported != reviewed_extent:
        raise ValueError(
            f"{label}: declared extent {reported:#x} != reviewed extent {reviewed_extent:#x}"
        )
    if inferred and reported < reviewed_extent:
        raise ValueError(
            f"{label}: inferred span {reported:#x} is shorter than reviewed extent {reviewed_extent:#x}"
        )
    return reviewed_extent


def _payload_boundaries(symbol: dict) -> set[int]:
    boundaries: set[int] = set()
    offset = 0
    for segment in symbol.get("data_diff", []):
        if "data" not in segment:
            continue
        offset += _number(segment.get("size"))
        boundaries.add(offset)
    return boundaries


def _relocations(
    side: dict,
    symbol: dict,
    extent: int,
    symbol_mappings: dict[str, str],
    map_names: bool,
) -> list[tuple[int, int, str, int]]:
    start = _number(symbol.get("address"))
    end = start + extent
    grouped: dict[tuple[int, int, int, int, int], int] = {}
    symbols = side.get("symbols", [])
    for item in symbol.get("data_relocations", []):
        reloc_start = _number(item.get("start"))
        reloc_end = _number(item.get("end"))
        if reloc_start < start or reloc_start >= end:
            continue
        if reloc_end > end:
            raise ValueError(
                f"{symbol.get('name')}: relocation {reloc_start:#x}..{reloc_end:#x} crosses extent"
            )
        relocation = item.get("relocation") or {}
        target_index = _number(relocation.get("target_symbol"))
        if target_index < 0 or target_index >= len(symbols):
            raise ValueError(f"{symbol.get('name')}: invalid relocation target {target_index}")
        key = (
            reloc_start,
            reloc_end,
            _number(relocation.get("type")),
            target_index,
            _number(relocation.get("addend")),
        )
        grouped[key] = grouped.get(key, 0) + 1

    boundaries = _payload_boundaries(symbol)
    rows: list[tuple[int, int, str, int]] = []
    for (reloc_start, reloc_end, reloc_type, target_index, addend), count in grouped.items():
        relative_start = reloc_start - start
        relative_end = reloc_end - start
        serialized_copies = 1 + sum(
            relative_start < boundary < relative_end for boundary in boundaries)
        if count != 1 and count != serialized_copies:
            raise ValueError(
                f"{symbol.get('name')}: relocation at {relative_start:#x} is repeated "
                f"{count} times but crosses {serialized_copies - 1} data_diff boundaries"
            )
        target_name = symbols[target_index].get("name", "")
        if map_names:
            target_name = symbol_mappings.get(target_name, target_name)
        rows.append(
            (
                relative_start,
                reloc_type,
                target_name,
                addend,
            )
        )
    rows.sort()
    return rows


def _reviewed_relocations(rows: object) -> list[dict]:
    if not isinstance(rows, list):
        raise ValueError("relocation_mappings must be an array")
    reviewed: list[dict] = []
    offsets: set[int] = set()
    for row in rows:
        if not isinstance(row, dict):
            raise ValueError("relocation_mappings entries must be objects")
        offset = _number(row["offset"])
        if offset in offsets:
            raise ValueError(f"relocation_mappings repeats offset {offset:#x}")
        offsets.add(offset)
        reviewed.append(
            {
                "offset": offset,
                "type": _number(row["type"]),
                "addend": _number(row.get("addend")),
                "target_name": row["target_name"],
                "base_name": row["base_name"],
            }
        )
    reviewed.sort(key=lambda row: row["offset"])
    return reviewed


def audit(diff: dict, manifest: dict) -> list[str]:
    errors: list[str] = []
    target = diff.get("left") or {}
    base = diff.get("right") or {}
    symbol_mappings = manifest.get("symbol_mappings", {})
    section_mappings = manifest.get("section_mappings", {})
    allocations = manifest.get("allocations")
    if manifest.get("version") != 1 or not isinstance(allocations, list):
        return ["manifest must have version 1 and an allocations array"]

    seen_target: set[str] = set()
    seen_base: set[str] = set()
    for row_number, allocation in enumerate(allocations, 1):
        try:
            target_name = allocation["target_name"]
            base_name = allocation.get(
                "base_name", symbol_mappings.get(target_name, target_name)
            )
            if target_name in seen_target or base_name in seen_base:
                raise ValueError("manifest allocation names must be unique on each side")
            seen_target.add(target_name)
            seen_base.add(base_name)

            target_symbol = _find_symbol(target, target_name, "target")
            base_symbol = _find_symbol(base, base_name, "base")
            target_section = _symbol_section(target, target_symbol)
            base_section = _symbol_section(base, base_symbol)

            target_kind = target_section.get("kind")
            base_kind = base_section.get("kind")
            if target_kind not in DATA_KINDS or base_kind not in DATA_KINDS:
                raise ValueError(f"non-data storage: {target_kind!r} vs {base_kind!r}")
            if target_kind != base_kind:
                raise ValueError(f"section kind differs: {target_kind} vs {base_kind}")
            expected_kind = allocation.get("section_kind")
            if expected_kind and target_kind != expected_kind:
                raise ValueError(f"section kind {target_kind} != reviewed {expected_kind}")

            target_section_name = target_section.get("name", "")
            base_section_name = base_section.get("name", "")
            mapped_section_name = section_mappings.get(target_section_name, target_section_name)
            if mapped_section_name != base_section_name:
                raise ValueError(
                    f"section name differs: {target_section_name!r} vs {base_section_name!r}"
                )

            reviewed_extent = allocation.get("extent")
            reviewed_extent = (
                _number(reviewed_extent) if reviewed_extent is not None else None
            )
            target_extent = _effective_extent(
                target_symbol, reviewed_extent, f"target {target_name}"
            )
            base_extent = _effective_extent(base_symbol, reviewed_extent, f"base {base_name}")
            if target_extent != base_extent:
                raise ValueError(
                    f"extent differs: {target_extent:#x} vs {base_extent:#x}"
                )

            if target_kind == "SECTION_DATA":
                target_payload = _payload(target_symbol)
                base_payload = _payload(base_symbol)
                if len(target_payload) < target_extent or len(base_payload) < base_extent:
                    raise ValueError("serialized payload is shorter than the reviewed extent")
                if target_payload[:target_extent] != base_payload[:base_extent]:
                    raise ValueError("payload differs")
            elif _payload(target_symbol) or _payload(base_symbol):
                raise ValueError("BSS/common allocation unexpectedly contains payload bytes")

            reviewed_relocs = allocation.get("relocation_mappings")
            if reviewed_relocs is None:
                target_relocs = _relocations(
                    target,
                    target_symbol,
                    target_extent,
                    symbol_mappings,
                    True,
                )
                base_relocs = _relocations(
                    base,
                    base_symbol,
                    base_extent,
                    symbol_mappings,
                    False,
                )
                if target_relocs != base_relocs:
                    raise ValueError(
                        f"relocations differ: target={target_relocs!r}, base={base_relocs!r}"
                    )
            else:
                reviewed = _reviewed_relocations(reviewed_relocs)
                expected_target = [
                    (row["offset"], row["type"], row["target_name"], row["addend"])
                    for row in reviewed
                ]
                expected_base = [
                    (row["offset"], row["type"], row["base_name"], row["addend"])
                    for row in reviewed
                ]
                target_relocs = _relocations(
                    target, target_symbol, target_extent, {}, False)
                base_relocs = _relocations(
                    base, base_symbol, base_extent, {}, False)
                if target_relocs != expected_target:
                    raise ValueError(
                        f"target relocations differ from reviewed: "
                        f"actual={target_relocs!r}, reviewed={expected_target!r}"
                    )
                if base_relocs != expected_base:
                    raise ValueError(
                        f"base relocations differ from reviewed: "
                        f"actual={base_relocs!r}, reviewed={expected_base!r}"
                    )
        except (KeyError, TypeError, ValueError) as error:
            errors.append(f"allocation {row_number}: {error}")
    return errors


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("diff", type=Path, help="objdiff-cli diff JSON")
    parser.add_argument("manifest", type=Path, help="reviewed allocation manifest JSON")
    args = parser.parse_args(argv)
    diff = json.loads(args.diff.read_text(encoding="utf-8"))
    manifest = json.loads(args.manifest.read_text(encoding="utf-8"))
    errors = audit(diff, manifest)
    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1
    print(f"strict allocation audit passed: {len(manifest['allocations'])} allocations")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
