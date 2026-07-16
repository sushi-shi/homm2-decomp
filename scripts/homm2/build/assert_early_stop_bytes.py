#!/usr/bin/env python3
"""Hard gate for ``@early-stop`` raw-byte identity claims.

Some early-stop markers state that the complete instruction bytes agree and that
only COFF relocation identity remains. Those claims use the explicit
``@early-stop-reloc-only`` tag. Objdiff masks relocation fields, but a
high fuzzy score does not prove the rest of the function: an ordinary immediate
or member displacement can still differ.  This gate turns those strong source
comments into checked assertions.

For every qualifying marker, the gate reads the function's declared retail span
from ``report.json``, extracts that span from both i386 COFF objects, masks the
actual relocation fields present on either side, and requires every other byte
to be identical.  It intentionally does not normalize branch destinations,
stack slots, immediates, padding, or operands.
"""

from __future__ import annotations

import csv
import json
import re
import struct
import sys
from dataclasses import dataclass
from pathlib import Path

from homm2.build.canonicalize_relocs import CoffFile


REPO = Path(__file__).resolve().parents[3]
VA_RE = re.compile(r"^\s*VA\(0x([0-9a-fA-F]+),\s*0x[0-9a-fA-F]+\)\s*$")
RELOCATION_WIDTHS = {
    0x0000: 0,  # ABSOLUTE
    0x0001: 2,  # DIR16
    0x0002: 2,  # REL16
    0x0006: 4,  # DIR32
    0x0007: 4,  # DIR32NB
    0x0009: 2,  # SEG12
    0x000A: 2,  # SECTION
    0x000B: 4,  # SECREL
    0x000C: 4,  # TOKEN
    0x000D: 1,  # SECREL7
    0x0014: 4,  # REL32
}


@dataclass(frozen=True)
class IdentityClaim:
    source: Path
    line: int
    unit: str
    rva: int
    marker: str


@dataclass(frozen=True)
class FunctionSpan:
    payload: bytes
    relocations: tuple[tuple[int, int], ...]


def is_raw_identity_claim(marker: str) -> bool:
    """Recognize the explicit machine-checked raw-identity claim."""
    text = " ".join(marker.lower().split())
    return "@early-stop-reloc-only" in text


def scan_claims(source_root: Path, symbols_path: Path) -> list[IdentityClaim]:
    by_rva = {}
    with symbols_path.open(encoding="latin-1", newline="") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "func":
                by_rva[int(row["rva"], 0)] = row

    claims = []
    for source in sorted(source_root.rglob("*.cpp")):
        lines = source.read_text(encoding="latin-1").splitlines()
        for index, line in enumerate(lines):
            match = VA_RE.match(line)
            if not match:
                continue
            start = index - 1
            while start >= 0 and (not lines[start].strip()
                                  or lines[start].lstrip().startswith("//")):
                start -= 1
            marker = "\n".join(lines[start + 1:index])
            if not is_raw_identity_claim(marker):
                continue
            rva = int(match.group(1), 16) - 0x400000
            row = by_rva.get(rva)
            if row is None:
                raise ValueError(
                    f"{source}:{index + 1}: identity claim has no function at RVA {rva:#x}"
                )
            claims.append(IdentityClaim(
                source=source, line=index + 1, unit=row["unit"], rva=rva,
                marker=marker,
            ))
    return claims


def _is_external_function(coff: CoffFile, symbol) -> bool:
    type_, storage = struct.unpack_from("<HB", coff.data, symbol.offset + 14)
    return type_ == 0x20 and storage == 2


def _symbol(coff: CoffFile, name: str):
    found = [symbol for symbol in coff.symbols.values()
             if symbol.name == name and symbol.section > 0
             and coff.sections[symbol.section - 1].name == ".text"
             and _is_external_function(coff, symbol)]
    if len(found) != 1:
        raise ValueError(
            f"{coff.path}: expected one .text definition of {name}, found {len(found)}"
        )
    return found[0]


def function_span(coff: CoffFile, name: str, size: int) -> FunctionSpan:
    symbol = _symbol(coff, name)
    section = coff.sections[symbol.section - 1]
    end = symbol.value + size
    if end > section.raw_size:
        raise ValueError(
            f"{coff.path}: {name} span {symbol.value:#x}..{end:#x} "
            f"exceeds {section.name} size {section.raw_size:#x}"
        )

    # Do not silently consume the next externally visible function when a
    # candidate is shorter than retail. Local labels are deliberately ignored.
    next_functions = [candidate.value for candidate in coff.symbols.values()
                      if candidate.section == symbol.section
                      and _is_external_function(coff, candidate)
                      and candidate.value > symbol.value]
    if next_functions and min(next_functions) < end:
        raise ValueError(
            f"{coff.path}: {name} declared span overlaps the next function at "
            f"{min(next_functions):#x}"
        )

    payload_start = section.raw_offset + symbol.value
    payload = bytes(coff.data[payload_start:payload_start + size])
    relocations = []
    for (section_index, site), relocation in coff.relocations.items():
        if section_index != symbol.section or not symbol.value <= site < end:
            continue
        width = RELOCATION_WIDTHS.get(relocation.typ)
        if width is None:
            raise ValueError(
                f"{coff.path}: {name} uses unsupported i386 relocation {relocation.typ:#x}"
            )
        relative = site - symbol.value
        if relative + width > size:
            raise ValueError(
                f"{coff.path}: relocation at {relative:#x} crosses {name}'s span"
            )
        relocations.append((relative, width))
    return FunctionSpan(payload, tuple(sorted(relocations)))


def compare_masked_spans(base: FunctionSpan, target: FunctionSpan) -> list[int]:
    if len(base.payload) != len(target.payload):
        raise ValueError("function spans have different lengths")
    if base.relocations != target.relocations:
        raise ValueError(
            "relocation-only claim has different relocation sites: "
            f"retail={target.relocations} base={base.relocations}"
        )
    masked = bytearray(len(base.payload))
    for offset, width in base.relocations:
        masked[offset:offset + width] = b"\1" * width
    return [offset for offset, (actual, expected) in enumerate(
        zip(base.payload, target.payload))
        if not masked[offset] and actual != expected]


def review(repo: Path = REPO) -> int:
    report_path = repo / "build/objdiff/report.json"
    symbols_path = repo / "build/gen/symbol_names.csv"
    claims = scan_claims(repo / "src", symbols_path)
    report = json.loads(report_path.read_text(encoding="utf-8"))
    functions = {
        (unit["name"], function["name"]): function
        for unit in report["units"] for function in unit.get("functions", [])
    }
    symbol_rows = {}
    with symbols_path.open(encoding="latin-1", newline="") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "func":
                symbol_rows[(row["unit"], int(row["rva"], 0))] = row

    coff_cache = {}
    failures = []
    checked_bytes = 0
    for claim in claims:
        row = symbol_rows[(claim.unit, claim.rva)]
        function = functions.get((claim.unit, row["name"]))
        if function is None:
            failures.append((claim, "function is absent from report.json"))
            continue
        size = int(function["size"], 0)
        base_path = repo / "build/objdiff/base" / f"{claim.unit}.obj"
        target_path = repo / "build/delink" / f"{claim.unit}.c.obj"
        try:
            if base_path not in coff_cache:
                coff_cache[base_path] = CoffFile(base_path)
            if target_path not in coff_cache:
                coff_cache[target_path] = CoffFile(target_path)
            base = coff_cache[base_path]
            target = coff_cache[target_path]
            base_span = function_span(base, row["name"], size)
            target_span = function_span(target, row["name"], size)
            differences = compare_masked_spans(base_span, target_span)
        except (OSError, ValueError) as error:
            failures.append((claim, str(error)))
            continue
        checked_bytes += size
        if differences:
            samples = ", ".join(
                f"+{offset:#x} retail={target_span.payload[offset]:02x} "
                f"base={base_span.payload[offset]:02x}"
                for offset in differences[:8]
            )
            failures.append((claim, f"{len(differences)} unmasked byte(s) differ: {samples}"))

    for claim, diagnostic in failures:
        print(f"  {claim.unit} RVA {claim.rva:#x} "
              f"({claim.source.relative_to(repo)}:{claim.line}): {diagnostic}")
    if failures:
        print(f"\nEARLY-STOP BYTES FAIL: {len(failures)}/{len(claims)} "
              "relocation-only identity claim(s) are false.")
        return 1
    print(f"early-stop bytes OK: {len(claims)} claims, "
          f"{checked_bytes} relocation-masked function bytes checked.")
    return 0


def main(argv=None) -> int:
    if argv:
        print("usage: python3 -m homm2.build.assert_early_stop_bytes", file=sys.stderr)
        return 2
    return review()


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
