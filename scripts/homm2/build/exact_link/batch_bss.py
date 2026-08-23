#!/usr/bin/env python3
"""Adapt every fully annotated SOURCE .bss contribution to retail topology."""

from __future__ import annotations

import argparse
import csv
import struct
from collections import defaultdict
from pathlib import Path

from .adapt_bss import (
    COFF_HEADER_SIZE,
    SECTION_HEADER_SIZE,
    CoffError,
    adapt,
)


ROOT = next(path for path in Path(__file__).resolve().parents
            if (path / "flake.nix").exists())
MANIFEST = ROOT / "build/gen/delink_data_from_source.tsv"
INPUT_ROOT = ROOT / "build/objdiff/base"
OUTPUT_ROOT = ROOT / "build/link/bss-layout-all"


def align_up(value: int, alignment: int) -> int:
    return (value + alignment - 1) & -alignment


def bss_size_and_alignment(blob: bytes) -> tuple[int, int]:
    section_count = struct.unpack_from("<H", blob, 2)[0]
    optional_size = struct.unpack_from("<H", blob, 16)[0]
    section_offset = COFF_HEADER_SIZE + optional_size
    found = []
    for index in range(section_count):
        offset = section_offset + index * SECTION_HEADER_SIZE
        name = blob[offset:offset + 8].split(b"\0", 1)[0]
        if name != b".bss":
            continue
        size = struct.unpack_from("<I", blob, offset + 16)[0]
        characteristics = struct.unpack_from("<I", blob, offset + 36)[0]
        encoded = (characteristics >> 20) & 0xF
        alignment = 1 << (encoded - 1) if encoded else 0
        found.append((size, alignment))
    if len(found) != 1:
        raise CoffError(f"expected exactly one .bss section, found {len(found)}")
    return found[0]


def load_claims(owner: str):
    groups = defaultdict(list)
    with MANIFEST.open(newline="") as stream:
        rows = (line for line in stream if not line.startswith("#"))
        for row in csv.DictReader(rows, delimiter="\t"):
            if row["storage"] != "bss" or not row["object"].startswith(owner + "\\"):
                continue
            groups[row["object"]].append(row)
    return groups


def object_path(object_name: str) -> Path:
    relative = object_name.removesuffix(".c").replace("\\", "/") + ".obj"
    return INPUT_ROOT / relative


def raw_symbol_name_map(source: Path) -> dict[str, str]:
    relative = source.relative_to(INPUT_ROOT).with_suffix(".symbols.tsv")
    sidecar = ROOT / "build/objdiff/normalized/base" / relative
    result = {}
    with sidecar.open(newline="") as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            canonical = row["canonical_name"]
            original = row["original_name"]
            previous = result.setdefault(canonical, original)
            if previous != original:
                raise CoffError(f"{sidecar}: ambiguous canonical symbol {canonical}")
    return result


def choose_alignment(previous_end: int, start: int, original: int) -> int:
    choices = [
        alignment for alignment in (1, 2, 4, 8, 16, 32, 64, 128, 256,
                                    512, 1024, 2048, 4096, 8192)
        if align_up(previous_end, alignment) == start
    ]
    if not choices:
        raise CoffError(
            f"retail gap {previous_end:#x}..{start:#x} is not section alignment"
        )
    return original if original in choices else min(choices)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--report-only", action="store_true")
    parser.add_argument("--owner", choices=("SOURCE", "BASE"), default="SOURCE")
    parser.add_argument(
        "--unit",
        help=r"adapt only this manifest object, for example SOURCE\REQUEST.c",
    )
    parser.add_argument("--previous-end", type=lambda value: int(value, 0), default=0x123DD4)
    options = parser.parse_args()
    claims = load_claims(options.owner)
    planned = []
    for object_name, rows in claims.items():
        if options.unit is not None and object_name != options.unit:
            continue
        source = object_path(object_name)
        blob = source.read_bytes()
        size, original_alignment = bss_size_and_alignment(blob)
        start = min(int(row["rva"], 16) for row in rows)
        end = max(int(row["rva"], 16) + int(row["size"], 16) for row in rows)
        if end - start != size:
            print(
                f"SPAN_MISMATCH {object_name}: markers={end-start:#x} candidate={size:#x}"
            )
        canonical_to_raw = raw_symbol_name_map(source)
        offsets = {
            canonical_to_raw.get(row["name"], row["name"]):
                int(row["rva"], 16) - start
            for row in rows
        }
        if len(offsets) != len(rows):
            raise CoffError(f"{object_name}: duplicate .bss claim")
        planned.append((start, end, object_name, source, blob, offsets, original_alignment))

    if options.unit is not None and not planned:
        raise CoffError(f"no .bss claims found for {options.unit}")

    planned.sort()
    if options.report_only:
        claimed_paths = {object_path(name) for name in claims}
        for source in sorted((INPUT_ROOT / options.owner).glob("*.obj")):
            if source in claimed_paths:
                continue
            try:
                size, alignment = bss_size_and_alignment(source.read_bytes())
            except CoffError:
                continue
            print(
                f"UNCLAIMED {source.relative_to(INPUT_ROOT)}: size={size:#x} align={alignment}"
            )
        for start, end, object_name, _, _, offsets, original_alignment in planned:
            print(
                f"{object_name}: {start:#x}..{end:#x} span={end-start:#x} "
                f"align={original_alignment} symbols={len(offsets)}"
            )
        return
    previous_end = options.previous_end
    for start, end, object_name, source, blob, offsets, original_alignment in planned:
        alignment = choose_alignment(previous_end, start, original_alignment)
        relative = source.relative_to(INPUT_ROOT)
        output = OUTPUT_ROOT / relative
        adapted = adapt(blob, object_name, offsets, alignment, end - start)
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_bytes(adapted)
        print(
            f"{object_name}: {start:#x}..{end:#x} size={end-start:#x} "
            f"align={original_alignment}->{alignment} symbols={len(offsets)}"
        )
        previous_end = end


if __name__ == "__main__":
    main()
