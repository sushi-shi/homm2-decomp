#!/usr/bin/env python3
"""Build build/gen/symbol_names.csv from the VA/DATA annotations in the source.

The PoL 2.0 line reads this inventory out of the CodeView NB09 stream: 3,541
publics, present whether or not anything has been reconstructed. This image is
stripped, so there is no such stream and no such gift. The only thing that knows
an address here is the source itself, and only where somebody has written the
address down.

That makes the rule strict and deliberate: a symbol exists for the delinker if and
only if a `VA(...)` or `DATA(...)` marker names it. Nothing is inferred from
Ghidra's function inventory, from signature matching, or from a neighbouring
symbol's extent. config/ghidra_functions.csv is a work list of 2,472 candidate
boundaries; this is the far smaller set that has been claimed and can be delinked
against. The two are not the same and must not be conflated - one is analysis
opinion, the other is a reviewed claim.

The set is empty until the campaign starts marking, and an empty inventory is the
correct answer to "what has been proven so far", not a failure.

Unlike homm2.build.annotated_functions - which recovers only STATIC functions,
because CodeView already supplied the public names - this names every annotated
definition, free function and method alike, since nothing else will.

    python3 -m homm2.build.source_symbols            # -> build/gen/symbol_names.csv
    python3 -m homm2.build.source_symbols --check    # report, write nothing
"""
from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci

from homm2.build.annotated_functions import (
    IMAGE_BASE, VA_TOKEN, _annotation, configure_libclang,
)
from homm2.build.annotated_data import ClangMode, _clang_args
from homm2.build.annotated_vtables import source_vtables
from homm2.core.paths import REPO

OUTPUT = REPO / "build/gen/symbol_names.csv"
HEADER = "rva,name,unit,size,kind,provenance\n"

# Every cursor kind that can carry a VA marker and produce a linker symbol.
DEFINITION_KINDS = (
    ci.CursorKind.FUNCTION_DECL,
    ci.CursorKind.CXX_METHOD,
    ci.CursorKind.CONSTRUCTOR,
    ci.CursorKind.DESTRUCTOR,
    ci.CursorKind.CONVERSION_FUNCTION,
)


@dataclass(frozen=True, order=True)
class SourceSymbol:
    rva: int
    name: str
    unit: str
    size: int
    kind: str
    provenance: str


def symbols_for_file(path: Path, source_root: Path, repo: Path) -> list[SourceSymbol]:
    """Every VA-annotated definition in one translation unit."""
    if not VA_TOKEN.search(path.read_bytes()):
        return []
    configure_libclang()
    translation = ci.Index.create().parse(
        str(path), args=_clang_args(repo, path, mode=ClangMode.RETAIL_ANALYSIS))
    errors = [d for d in translation.diagnostics if d.severity >= ci.Diagnostic.Error]
    if errors:
        detail = "; ".join(str(d) for d in errors[:5])
        raise ValueError(f"{path}: Clang could not read the annotations: {detail}")

    unit = path.relative_to(source_root).with_suffix("").as_posix()
    rows = []
    for cursor in translation.cursor.walk_preorder():
        if cursor.kind not in DEFINITION_KINDS or not cursor.is_definition():
            continue
        if cursor.location.file is None:
            continue
        if Path(str(cursor.location.file)).resolve() != path:
            continue
        annotated = _annotation(cursor)
        if annotated is None:
            continue
        va, size = annotated
        # A marker that cannot produce a symbol is a source defect, not a row to
        # drop quietly: the delinker would carve a span nothing can be matched to.
        if va < IMAGE_BASE or size <= 0 or not cursor.mangled_name:
            raise ValueError(
                f"{path}:{cursor.location.line}: unusable VA marker on "
                f"{cursor.spelling!r}")
        rows.append(SourceSymbol(
            rva=va - IMAGE_BASE, name=cursor.mangled_name, unit=unit,
            size=size, kind="func", provenance="source-annotation"))
    return rows


def collect(source_root: Path, repo: Path) -> list[SourceSymbol]:
    rows: list[SourceSymbol] = []
    for path in sorted(source_root.rglob("*.cpp")):
        rows.extend(symbols_for_file(path.resolve(), source_root, repo))
    for vtable in source_vtables(source_root, repo):
        rows.append(SourceSymbol(
            rva=vtable.rva, name=vtable.mangled_name, unit=vtable.unit,
            size=0, kind="data", provenance="source-vtable"))

    seen: dict[int, SourceSymbol] = {}
    for row in sorted(rows):
        clash = seen.get(row.rva)
        # Two markers on one address means one of them is wrong, and delinking
        # would silently keep whichever sorted first.
        if clash is not None and clash.name != row.name:
            raise ValueError(
                f"0x{row.rva:x} is claimed by both {clash.name} ({clash.unit}) "
                f"and {row.name} ({row.unit})")
        seen[row.rva] = row
    return sorted(seen.values())


def render(rows) -> str:
    lines = [HEADER]
    for row in rows:
        lines.append(f"0x{row.rva:x},{row.name},{row.unit},"
                     f"0x{row.size:x},{row.kind},{row.provenance}\n")
    return "".join(lines)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--source", type=Path, default=REPO / "src")
    parser.add_argument("--output", type=Path, default=OUTPUT)
    parser.add_argument("--check", action="store_true",
                        help="report what would be written and write nothing")
    args = parser.parse_args(argv)

    rows = collect(args.source.resolve(), REPO)
    functions = sum(1 for row in rows if row.kind == "func")
    print(f"[source-symbols] {len(rows)} annotated symbols "
          f"({functions} functions, {len(rows) - functions} data)")
    if not rows:
        print("[source-symbols] nothing is marked yet; the delinker will carve nothing")
    if args.check:
        return 0
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(render(rows))
    try:
        shown = args.output.relative_to(REPO)
    except ValueError:                      # --output may point outside the tree
        shown = args.output
    print(f"[source-symbols] -> {shown}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
