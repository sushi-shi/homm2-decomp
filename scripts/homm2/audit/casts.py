#!/usr/bin/env python3
"""Inventory explicit C++ casts and classify the type boundary they cross.

The reconstruction uses explicit casts both for legitimate representation
boundaries and to paper over incomplete type recovery.  A textual count cannot
tell those apart.  This audit parses the compilation database with libclang and
records the source and destination domains, canonical types, storage widths,
enum identities, enclosing function, and exact source span for every explicit
C++ cast in project-owned source.

The report is deliberately diagnostic rather than a build gate.  Categories
such as ``cross-enum``, ``literal-to-enum``, ``same-type``, and
``integer-narrowing`` are high-priority review queues.  ``integer-to-enum`` and
``enum-to-integer`` often represent real packed-data or ABI boundaries, but the
report retains enough provenance to decide that from source rather than merely
silencing the compiler.

Run inside ``nix develop .#build``::

    homm2 audit casts
    homm2 audit casts --all
    homm2 audit casts --category cross-enum --category literal-to-enum
    homm2 audit casts --format json --output build/casts.json
    homm2 audit casts --tu SOURCE/Castle
"""

from __future__ import annotations

import argparse
import gc
import importlib
import json
import os
import re
import sys
from collections import Counter
from concurrent.futures import ProcessPoolExecutor
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable

import clang.cindex as ci

from homm2.audit.bool_fields import (
    _entries,
    _portable_clang_args,
    _project_relative,
)
from homm2.build.annotated_data import (
    _clang_args,
    _mask_lexical_noise,
    configure_libclang,
)
from homm2.clang_options import ClangMode
from homm2.core.paths import REPO


SCHEMA_VERSION = 1
CAST_KINDS = {
    ci.CursorKind.CXX_STATIC_CAST_EXPR: "static",
    ci.CursorKind.CXX_CONST_CAST_EXPR: "const",
    ci.CursorKind.CXX_REINTERPRET_CAST_EXPR: "reinterpret",
    ci.CursorKind.CXX_DYNAMIC_CAST_EXPR: "dynamic",
}
OWNER_KINDS = {
    ci.CursorKind.CONSTRUCTOR,
    ci.CursorKind.CONVERSION_FUNCTION,
    ci.CursorKind.CXX_METHOD,
    ci.CursorKind.DESTRUCTOR,
    ci.CursorKind.FUNCTION_DECL,
    ci.CursorKind.FUNCTION_TEMPLATE,
}
REFERENCE_CHILD_KINDS = {
    ci.CursorKind.NAMESPACE_REF,
    ci.CursorKind.OVERLOADED_DECL_REF,
    ci.CursorKind.TEMPLATE_REF,
    ci.CursorKind.TYPE_REF,
}
INTEGER_KINDS = {
    ci.TypeKind.BOOL,
    ci.TypeKind.CHAR_S,
    ci.TypeKind.CHAR_U,
    ci.TypeKind.SCHAR,
    ci.TypeKind.UCHAR,
    ci.TypeKind.SHORT,
    ci.TypeKind.USHORT,
    ci.TypeKind.INT,
    ci.TypeKind.UINT,
    ci.TypeKind.LONG,
    ci.TypeKind.ULONG,
    ci.TypeKind.LONGLONG,
    ci.TypeKind.ULONGLONG,
    ci.TypeKind.INT128,
    ci.TypeKind.UINT128,
    ci.TypeKind.WCHAR,
}
FLOAT_KINDS = {
    ci.TypeKind.FLOAT,
    ci.TypeKind.DOUBLE,
    ci.TypeKind.LONGDOUBLE,
    ci.TypeKind.FLOAT128,
}
POINTER_KINDS = {
    ci.TypeKind.POINTER,
    ci.TypeKind.MEMBERPOINTER,
    ci.TypeKind.BLOCKPOINTER,
    ci.TypeKind.OBJCOBJECTPOINTER,
}
REFERENCE_KINDS = {
    ci.TypeKind.LVALUEREFERENCE,
    ci.TypeKind.RVALUEREFERENCE,
}
HIGH_PRIORITY = frozenset({
    "cross-enum",
    "literal-to-enum",
    "same-type",
    "same-enum",
})
CAST_TOKEN = re.compile(rb"\b(static|const|reinterpret|dynamic)_cast\s*<")


@dataclass(frozen=True, order=True)
class Cast:
    file: str
    line: int
    column: int
    start: int
    end: int
    cast_kind: str
    category: str
    source_type: str
    destination_type: str
    source_canonical: str
    destination_canonical: str
    source_domain: str
    destination_domain: str
    source_size: int
    destination_size: int
    source_enum: str
    destination_enum: str
    function: str
    expression: str


def _canonical(type_: ci.Type) -> ci.Type:
    try:
        return type_.get_canonical()
    except Exception:
        return type_


def _domain(type_: ci.Type) -> str:
    kind = _canonical(type_).kind
    if kind == ci.TypeKind.ENUM:
        return "enum"
    if kind == ci.TypeKind.BOOL:
        return "bool"
    if kind in INTEGER_KINDS:
        return "integer"
    if kind in FLOAT_KINDS:
        return "floating"
    if kind in POINTER_KINDS:
        return "pointer"
    if kind in REFERENCE_KINDS:
        return "reference"
    if kind in (ci.TypeKind.RECORD, ci.TypeKind.ELABORATED):
        return "record"
    if kind == ci.TypeKind.VOID:
        return "void"
    return "other"


def _size(type_: ci.Type) -> int:
    try:
        return max(-1, int(_canonical(type_).get_size()))
    except Exception:
        return -1


def _enum_identity(type_: ci.Type) -> str:
    canonical = _canonical(type_)
    if canonical.kind != ci.TypeKind.ENUM:
        return ""
    declaration = canonical.get_declaration()
    if declaration is None:
        return canonical.spelling
    return declaration.get_usr() or declaration.displayname or canonical.spelling


def _integer_literal(cursor: ci.Cursor) -> bool:
    current = cursor
    wrappers = {
        ci.CursorKind.PAREN_EXPR,
        ci.CursorKind.UNEXPOSED_EXPR,
    }
    while current.kind in wrappers:
        children = list(current.get_children())
        if len(children) != 1:
            break
        current = children[0]
    return current.kind in {
        ci.CursorKind.INTEGER_LITERAL,
        ci.CursorKind.CHARACTER_LITERAL,
    }


def _signedness(type_: ci.Type) -> str:
    kind = _canonical(type_).kind
    if kind in {
        ci.TypeKind.UCHAR,
        ci.TypeKind.USHORT,
        ci.TypeKind.UINT,
        ci.TypeKind.ULONG,
        ci.TypeKind.ULONGLONG,
        ci.TypeKind.UINT128,
        ci.TypeKind.CHAR_U,
    }:
        return "unsigned"
    if kind in INTEGER_KINDS:
        return "signed"
    return ""


def classify(cast_kind: str, source: ci.Type, destination: ci.Type,
             operand: ci.Cursor) -> str:
    source_domain = _domain(source)
    destination_domain = _domain(destination)
    source_canonical = _canonical(source).spelling
    destination_canonical = _canonical(destination).spelling

    if cast_kind == "const":
        return "const-qualification"
    if cast_kind == "reinterpret":
        return "representation"
    if cast_kind == "dynamic":
        return "dynamic-type"
    if source.spelling == destination.spelling:
        return "same-enum" if source_domain == "enum" else "same-type"
    if source_canonical == destination_canonical:
        # Typedef aliases may carry distinct semantic roles even though Clang
        # lowers them to one representation (HGDIOBJ/HBITMAP is the clearest
        # SDK example). Do not call these redundant without source review.
        return "alias-conversion"
    if source_domain == "enum" and destination_domain == "enum":
        return "cross-enum"
    if destination_domain == "enum" and source_domain in {"integer", "bool"}:
        return "literal-to-enum" if _integer_literal(operand) else "integer-to-enum"
    if source_domain == "enum" and destination_domain in {"integer", "bool"}:
        return "enum-to-integer"
    if source_domain == "bool" and destination_domain == "integer":
        return "bool-to-integer"
    if source_domain == "integer" and destination_domain == "bool":
        return "integer-to-bool"
    if source_domain in {"integer", "bool"} and destination_domain in {"integer", "bool"}:
        source_size = _size(source)
        destination_size = _size(destination)
        if source_size >= 0 and destination_size >= 0 and destination_size < source_size:
            return "integer-narrowing"
        if _signedness(source) != _signedness(destination):
            return "integer-signedness"
        if source_size >= 0 and destination_size > source_size:
            return "integer-widening"
        return "integer-alias"
    if source_domain == "floating" and destination_domain in {"integer", "bool"}:
        return "floating-to-integer"
    if source_domain in {"integer", "bool"} and destination_domain == "floating":
        return "integer-to-floating"
    if source_domain == "floating" and destination_domain == "floating":
        return "floating-conversion"
    if source_domain in {"pointer", "reference"} or destination_domain in {"pointer", "reference"}:
        return "pointer-conversion"
    if source_domain == "record" and destination_domain == "record":
        return "record-conversion"
    if source_domain == "record" and destination_domain in {"integer", "bool"}:
        if source.spelling.startswith(("H2EnumStorage<", "H2SteppedEnumStorage<")):
            return "enum-storage-to-integer"
        return "explicit-scalar-conversion"
    return "other-static"


def _qualified_owner(cursor: ci.Cursor | None) -> str:
    if cursor is None:
        return "<global>"
    names = []
    current = cursor
    while current is not None and current.kind != ci.CursorKind.TRANSLATION_UNIT:
        if current.spelling:
            names.append(current.spelling)
        current = current.semantic_parent
    return "::".join(reversed(names)) or "<global>"


def _operand(cursor: ci.Cursor) -> ci.Cursor | None:
    children = [child for child in cursor.get_children()
                if child.kind not in REFERENCE_CHILD_KINDS]
    if not children:
        return None
    operand = children[-1]
    # Clang inserts an UNEXPOSED_EXPR carrying the destination type around
    # many explicit conversions. The child below it is the actual pre-cast
    # expression and therefore the only useful source-type evidence.
    while operand.kind in {
        ci.CursorKind.PAREN_EXPR,
        ci.CursorKind.UNEXPOSED_EXPR,
    }:
        nested = [child for child in operand.get_children()
                  if child.kind not in REFERENCE_CHILD_KINDS]
        if len(nested) != 1:
            break
        operand = nested[0]
    # An explicit conversion operator appears as a destination-typed CallExpr.
    # Recover the object on which that operator is invoked; otherwise a
    # `static_cast<char>(H2EnumStorage<...>)` is falsely reported as char->char.
    if (operand.kind == ci.CursorKind.CALL_EXPR
            and operand.referenced is not None
            and operand.referenced.kind == ci.CursorKind.CONVERSION_FUNCTION):
        call_children = list(operand.get_children())
        if call_children:
            member_children = list(call_children[0].get_children())
            if member_children:
                converted = member_children[0]
                while converted.kind in {
                    ci.CursorKind.PAREN_EXPR,
                    ci.CursorKind.UNEXPOSED_EXPR,
                }:
                    nested = list(converted.get_children())
                    if len(nested) != 1:
                        break
                    converted = nested[0]
                operand = converted
    return operand


def _spelled_expression(cursor: ci.Cursor, repo: Path, relative: str,
                        cast_kind: str) -> str | None:
    """Return source text only when this cast is spelled at its cursor site.

    Clang materializes casts from macro bodies once per expansion and locates
    them at the invocation. Counting those AST nodes inflated the inventory
    (notably ``H2_ALLOC`` and the enum helper macros) and attributed one source
    cast to scores of unrelated call sites. The exact cursor span must contain
    the cast token; macro-definition coverage is accounted for separately by
    the lexical census.
    """
    try:
        blob = (repo / relative).read_bytes()
        raw = blob[cursor.extent.start.offset:cursor.extent.end.offset]
        token = f"{cast_kind}_cast".encode()
        if token not in raw:
            return None
        text = raw.decode("utf-8")
    except (OSError, UnicodeDecodeError):
        return None
    compact = " ".join(text.split())
    return compact if len(compact) <= 160 else compact[:157] + "..."


def analyze_translation_unit(translation: ci.TranslationUnit, repo: Path) -> list[Cast]:
    rows = []

    def visit(cursor: ci.Cursor, owner: ci.Cursor | None) -> None:
        if cursor.kind in OWNER_KINDS:
            owner = cursor
        cast_kind = CAST_KINDS.get(cursor.kind)
        if cast_kind is not None:
            location = cursor.location
            relative = _project_relative(
                str(location.file) if location.file else None, repo)
            operand = _operand(cursor)
            expression = (_spelled_expression(cursor, repo, relative, cast_kind)
                          if relative is not None else None)
            if relative is not None and operand is not None and expression is not None:
                source = operand.type
                destination = cursor.type
                rows.append(Cast(
                    file=relative,
                    line=location.line,
                    column=location.column,
                    start=cursor.extent.start.offset,
                    end=cursor.extent.end.offset,
                    cast_kind=cast_kind,
                    category=classify(cast_kind, source, destination, operand),
                    source_type=source.spelling,
                    destination_type=destination.spelling,
                    source_canonical=_canonical(source).spelling,
                    destination_canonical=_canonical(destination).spelling,
                    source_domain=_domain(source),
                    destination_domain=_domain(destination),
                    source_size=_size(source),
                    destination_size=_size(destination),
                    source_enum=_enum_identity(source),
                    destination_enum=_enum_identity(destination),
                    function=_qualified_owner(owner),
                    expression=expression,
                ))
        for child in cursor.get_children():
            visit(child, owner)

    visit(translation.cursor, None)
    return rows


def _lexical_counts(repo: Path) -> dict[str, int]:
    counts: Counter[str] = Counter()
    for root_name in ("include", "src"):
        root = repo / root_name
        if not root.is_dir():
            continue
        for path in root.rglob("*"):
            if path.suffix.lower() not in {".h", ".hh", ".hpp", ".cpp", ".cxx"}:
                continue
            try:
                masked = _mask_lexical_noise(path.read_bytes())
            except (OSError, ValueError):
                continue
            counts.update(match.group(1).decode() for match in CAST_TOKEN.finditer(masked))
    return dict(sorted(counts.items()))


def _parse_batch(payload: tuple[Path, list[dict], bool]) -> dict:
    repo, entries, portable = payload
    configure_libclang()
    index = ci.Index.create()
    rows = []
    diagnostics = []
    for entry in entries:
        source = (Path(entry["directory"]) / entry["file"]).resolve()
        # Cast provenance is meaningful in the repository's strict enum and
        # const model. The VC6-compatible C++98 view intentionally erases those
        # distinctions (and cannot type a few conditional string expressions),
        # which would make this audit both incomplete and misleading.
        args = (_portable_clang_args(repo, entry) if portable else
                _clang_args(repo, source, mode=ClangMode.STRICT))
        translation = index.parse(str(source), args=args)
        for diagnostic in translation.diagnostics:
            if diagnostic.severity < ci.Diagnostic.Error or diagnostic.location.file is None:
                continue
            relative = _project_relative(str(diagnostic.location.file), repo)
            if relative is not None:
                diagnostics.append({
                    "file": relative,
                    "line": diagnostic.location.line,
                    "column": diagnostic.location.column,
                    "translation_unit": source.relative_to(repo).as_posix(),
                    "message": diagnostic.spelling,
                })
        rows.extend(asdict(item) for item in analyze_translation_unit(translation, repo))
        del translation
        gc.collect()
    return {"casts": rows, "diagnostics": diagnostics}


def scan(repo: Path = REPO, *, jobs: int = 0,
         filters: Iterable[str] = (), portable: bool = False) -> dict:
    filters = tuple(filters)
    entries = _entries(repo, filters, portable=portable)
    if not entries:
        raise RuntimeError("no translation units selected")
    worker_count = jobs if jobs > 0 else min(8, max(1, (os.cpu_count() or 4) // 2))
    batch_size = max(1, (len(entries) + worker_count - 1) // worker_count)
    batches = [(repo, entries[index:index + batch_size], portable)
               for index in range(0, len(entries), batch_size)]
    if len(batches) == 1:
        parsed = [_parse_batch(batches[0])]
    else:
        worker = importlib.import_module("homm2.audit.casts")._parse_batch
        with ProcessPoolExecutor(max_workers=min(worker_count, len(batches))) as pool:
            parsed = list(pool.map(worker, batches))
    raw = [row for batch in parsed for row in batch["casts"]]
    diagnostics = sorted({
        (item["file"], item["line"], item["column"], item["translation_unit"], item["message"]):
        item
        for batch in parsed for item in batch["diagnostics"]
    }.values(), key=lambda item: (
        item["file"], item["line"], item["column"], item["translation_unit"], item["message"]))

    # Header casts appear once per including TU. Source identity, not spelling,
    # is the durable deduplication key.
    unique = {
        (row["file"], row["start"], row["end"], row["cast_kind"]): Cast(**row)
        for row in raw
    }
    casts = sorted(unique.values())
    counts = Counter(item.category for item in casts)
    kind_counts = Counter(item.cast_kind for item in casts)
    lexical_counts = _lexical_counts(repo)
    return {
        "schema_version": SCHEMA_VERSION,
        "whole_program": not filters,
        "filters": list(filters),
        "translation_units": len(entries),
        "casts": [asdict(item) for item in casts],
        "counts": dict(sorted(counts.items())),
        "cast_kinds": dict(sorted(kind_counts.items())),
        "lexical_cast_kinds": lexical_counts,
        "unmapped_lexical_casts": {
            kind: lexical_counts.get(kind, 0) - kind_counts.get(kind, 0)
            for kind in sorted(set(lexical_counts) | set(kind_counts))
        },
        "high_priority": sum(item.category in HIGH_PRIORITY for item in casts),
        "strict_diagnostics": diagnostics,
        "strict_parse_clean": not diagnostics,
    }


def _text(report: dict, *, include_all: bool,
          categories: frozenset[str]) -> str:
    scope = "" if report["whole_program"] else "partial "
    counts = ", ".join(f"{name}={count}" for name, count in report["counts"].items())
    lines = [
        f"[{scope}casts] {report['translation_units']} translation units, "
        f"{len(report['casts'])} explicit casts, "
        f"{report['high_priority']} high-priority review sites, "
        f"{len(report.get('strict_diagnostics', []))} strict diagnostics",
        counts,
    ]
    for item in report.get("strict_diagnostics", []):
        lines.append(
            f"STRICT {item['file']}:{item['line']}:{item['column']}: "
            f"{item['message']}")
    for item in report["casts"]:
        if categories:
            if item["category"] not in categories:
                continue
        elif not include_all and item["category"] not in HIGH_PRIORITY:
            continue
        lines.append(
            f"{item['category'].upper()} {item['file']}:{item['line']} "
            f"{item['function']}: {item['source_type']} -> "
            f"{item['destination_type']} | {item['expression']}")
    return "\n".join(lines) + "\n"


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--format", choices=("text", "json"), default="text")
    parser.add_argument("--output", type=Path, help="write report instead of stdout")
    parser.add_argument("--all", action="store_true", help="show every cast in text output")
    parser.add_argument("--category", action="append", default=[],
                        help="show only a category (repeatable)")
    parser.add_argument("--tu", action="append", default=[],
                        help="limit source paths (partial evidence)")
    parser.add_argument("--portable", action="store_true",
                        help="parse a portable CMake compilation database")
    parser.add_argument("-j", "--jobs", type=int, default=0)
    args = parser.parse_args(argv)
    try:
        report = scan(jobs=args.jobs, filters=args.tu, portable=args.portable)
    except RuntimeError as error:
        print(f"homm2 audit casts: {error}", file=sys.stderr)
        return 2
    unknown = sorted(set(args.category) - set(report["counts"]))
    if unknown:
        print("homm2 audit casts: unknown/empty categories: " + ", ".join(unknown),
              file=sys.stderr)
        return 2
    output = (json.dumps(report, indent=2) + "\n" if args.format == "json" else
              _text(report, include_all=args.all,
                    categories=frozenset(args.category)))
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(output)
    else:
        print(output, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
