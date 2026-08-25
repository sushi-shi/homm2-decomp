"""Recover source-annotated function spans and file-local identities for delinking.

A ``static`` function has no external linkage name anywhere but its candidate
COFF's decorated local symbol. ``VA`` already records the reviewed retail
address and extent. Clang supplies the exact Microsoft-decorated identity from
the definition's semantic signature.
"""

from __future__ import annotations

import argparse
import csv
import io
import re
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci

from homm2.clang_options import ClangMode
from homm2.build.fixed_asm import claims as fixed_asm_claims
from homm2.build.annotated_data import (
    _clang_args,
    _mask_lexical_noise,
    configure_libclang,
)
from homm2.build.canonicalize_data_symbols import CoffObject


IMAGE_BASE = 0x400000
REPO = next((parent for parent in Path(__file__).resolve().parents
             if (parent / "flake.nix").exists()), Path.cwd())
DEFAULT_OUTPUT = REPO / "build/gen/source_private_functions.csv"
DEFAULT_SPANS_OUTPUT = REPO / "build/gen/source_function_spans.csv"
DEFAULT_SYMBOLS = REPO / "build/gen/symbol_names.csv"
VA_TOKEN = re.compile(rb"\bVA\s*\(")
VA_MARKER = re.compile(
    rb"^[ \t]*VA\(\s*(0x[0-9a-fA-F]+)\s*,\s*(0x[0-9a-fA-F]+|[0-9]+)\s*\)",
    re.M)
STATIC_VA_TOKEN = re.compile(
    rb"\bVA\s*\([^)]*\)\s*(?:(?!\{).){0,1024}\bstatic\b", re.S)
VA_ANNOTATION = re.compile(r"^va:(0x[0-9a-fA-F]+) size:(0x[0-9a-fA-F]+|[0-9]+)$")


@dataclass(frozen=True, order=True)
class AnnotatedPrivateFunction:
    unit: str
    name: str
    mangled_name: str
    rva: int
    size: int
    location: str


@dataclass(frozen=True, order=True)
class AnnotatedFunctionSpan:
    unit: str
    rva: int
    size: int
    location: str


def _annotation(cursor) -> tuple[int, int] | None:
    values = [child.spelling for child in cursor.get_children()
              if child.kind == ci.CursorKind.ANNOTATE_ATTR]
    matches = [VA_ANNOTATION.match(value) for value in values]
    matches = [match for match in matches if match is not None]
    if not matches:
        return None
    if len(matches) != 1:
        raise ValueError(
            f"{cursor.location.file}:{cursor.location.line}: multiple VA annotations")
    return int(matches[0].group(1), 16), int(matches[0].group(2), 0)


def definitions_for_file(path: Path, source_root: Path,
                         repo: Path) -> list[AnnotatedPrivateFunction]:
    repo = repo.resolve()
    source_root = source_root.resolve()
    path = path.resolve()
    if not VA_TOKEN.search(path.read_bytes()):
        return []
    configure_libclang()
    translation = ci.Index.create().parse(
        str(path),
        args=_clang_args(repo, path, mode=ClangMode.RETAIL_ANALYSIS),
    )
    errors = [diagnostic for diagnostic in translation.diagnostics
              if diagnostic.severity >= ci.Diagnostic.Error]
    if errors:
        detail = "; ".join(str(diagnostic) for diagnostic in errors[:5])
        raise ValueError(f"{path}: Clang could not recover private functions: {detail}")
    rows = []
    unit = path.relative_to(source_root.resolve()).with_suffix("").as_posix()
    for cursor in translation.cursor.walk_preorder():
        if cursor.kind != ci.CursorKind.FUNCTION_DECL or not cursor.is_definition():
            continue
        if cursor.location.file is None or Path(str(cursor.location.file)).resolve() != path:
            continue
        annotated = _annotation(cursor)
        if annotated is None or cursor.storage_class != ci.StorageClass.STATIC:
            continue
        va, size = annotated
        if va < IMAGE_BASE or size <= 0 or not cursor.mangled_name:
            raise ValueError(f"{path}:{cursor.location.line}: invalid static VA function")
        rows.append(AnnotatedPrivateFunction(
            unit=unit,
            name=cursor.spelling,
            mangled_name=cursor.mangled_name,
            rva=va - IMAGE_BASE,
            size=size,
            location=f"{path.relative_to(repo).as_posix()}:{cursor.location.line}",
        ))
    return rows


def source_private_functions(source_root: Path,
                             repo: Path) -> list[AnnotatedPrivateFunction]:
    source_root = Path(source_root).resolve()
    repo = Path(repo).resolve()
    rows = []
    for path in sorted(source_root.rglob("*.cpp")):
        blob = path.read_bytes()
        if STATIC_VA_TOKEN.search(_mask_lexical_noise(blob)):
            definitions = definitions_for_file(path, source_root, repo)
            if not definitions:
                raise ValueError(f"{path}: static VA marker has no function definition")
            rows.extend(definitions)
    identities = {(row.unit, row.rva) for row in rows}
    if len(identities) != len(rows):
        raise ValueError("duplicate source-private function RVA within a translation unit")
    return sorted(rows, key=lambda row: (row.unit, row.rva))


def source_function_spans(source_root: Path,
                          repo: Path) -> list[AnnotatedFunctionSpan]:
    source_root = Path(source_root).resolve()
    repo = Path(repo).resolve()
    rows = []
    for path in sorted(source_root.rglob("*.cpp")):
        blob = path.read_bytes()
        masked = _mask_lexical_noise(blob)
        unit = path.relative_to(source_root).with_suffix("").as_posix()
        for match in VA_MARKER.finditer(masked):
            va = int(match.group(1), 16)
            size = int(match.group(2), 0)
            line = blob.count(b"\n", 0, match.start()) + 1
            if va < IMAGE_BASE or size <= 0:
                raise ValueError(f"{path}:{line}: invalid VA function span")
            rows.append(AnnotatedFunctionSpan(
                unit=unit,
                rva=va - IMAGE_BASE,
                size=size,
                location=f"{path.relative_to(repo).as_posix()}:{line}",
            ))
    for unit, source, claim in fixed_asm_claims("func"):
        if (repo / source).is_file():
            rows.append(AnnotatedFunctionSpan(
                unit=unit, rva=claim.rva, size=claim.size, location=source,
            ))
    identities = {row.rva for row in rows}
    if len(identities) != len(rows):
        raise ValueError("duplicate source function RVA")
    return sorted(rows, key=lambda row: (row.unit, row.rva))


def csv_bytes(rows: list[AnnotatedPrivateFunction]) -> bytes:
    stream = io.StringIO(newline="")
    writer = csv.writer(stream, lineterminator="\n")
    writer.writerow(("rva", "name", "unit", "size", "source_name", "source"))
    for row in rows:
        writer.writerow((f"0x{row.rva:x}", row.mangled_name, row.unit,
                         f"0x{row.size:x}", row.name, row.location))
    return stream.getvalue().encode("utf-8")


def span_csv_bytes(rows: list[AnnotatedFunctionSpan]) -> bytes:
    stream = io.StringIO(newline="")
    writer = csv.writer(stream, lineterminator="\n")
    writer.writerow(("rva", "unit", "size", "source"))
    for row in rows:
        writer.writerow((f"0x{row.rva:x}", row.unit,
                         f"0x{row.size:x}", row.location))
    return stream.getvalue().encode("utf-8")


def validate_candidate_symbols(rows: list[AnnotatedPrivateFunction],
                               object_root: Path) -> None:
    by_unit = {}
    for row in rows:
        by_unit.setdefault(row.unit, []).append(row)
    failures = []
    for unit, claims in sorted(by_unit.items()):
        path = object_root / f"{unit}.obj"
        if not path.is_file():
            failures.append(f"{unit}: missing candidate object {path}")
            continue
        coff = CoffObject(path.read_bytes())
        definitions = [symbol for symbol in coff.symbols.values()
                       if symbol.section > 0 and symbol.typ == 0x20 and
                       symbol.storage_class == 3]
        for claim in claims:
            matches = [symbol for symbol in definitions
                       if symbol.name == claim.mangled_name]
            if len(matches) != 1:
                failures.append(
                    f"{unit}: {claim.location} expects one static COFF definition "
                    f"named {claim.mangled_name}, found {len(matches)}")
    if failures:
        raise ValueError("source-private COFF validation failed:\n" + "\n".join(failures))


def validate_symbol_manifest(spans: list[AnnotatedFunctionSpan], path: Path) -> None:
    if not path.is_file():
        raise ValueError(f"missing generated symbol manifest: {path}")
    with path.open(newline="") as stream:
        rows = {int(row["rva"], 16): row for row in csv.DictReader(stream)
                if row.get("kind") == "func"}
    failures = []
    for span in spans:
        row = rows.get(span.rva)
        if row is None:
            failures.append(f"{span.location}: no generated function at 0x{span.rva:x}")
        elif int(row["size"], 16) != span.size or row["unit"] != span.unit:
            failures.append(
                f"{span.location}: source {span.unit} size 0x{span.size:x}, generated "
                f"{row['unit']} size {row['size']}")
    if failures:
        raise ValueError("source function span validation failed:\n" + "\n".join(failures))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-root", type=Path, default=REPO / "src")
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--spans-output", type=Path, default=DEFAULT_SPANS_OUTPUT)
    parser.add_argument("--symbols", type=Path, default=DEFAULT_SYMBOLS)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--objects", type=Path)
    args = parser.parse_args(argv)
    rows = source_private_functions(args.source_root, REPO)
    spans = source_function_spans(args.source_root, REPO)
    payload = csv_bytes(rows)
    spans_payload = span_csv_bytes(spans)
    if args.check:
        if not args.output.is_file() or args.output.read_bytes() != payload:
            raise SystemExit(f"source-private function manifest is stale: {args.output}")
        if (not args.spans_output.is_file() or
                args.spans_output.read_bytes() != spans_payload):
            raise SystemExit(f"source function span manifest is stale: {args.spans_output}")
        validate_symbol_manifest(spans, args.symbols)
        if args.objects is not None:
            validate_candidate_symbols(rows, args.objects)
        print("source function manifests are current")
        return 0
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(payload)
    args.spans_output.parent.mkdir(parents=True, exist_ok=True)
    args.spans_output.write_bytes(spans_payload)
    print(f"source functions: wrote {len(rows)} private identities and {len(spans)} spans")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
