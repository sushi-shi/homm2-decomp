#!/usr/bin/env python3
"""Inventory goto/label control flow and rank likely reconstruction debt.

A textual ``rg goto`` count cannot distinguish a deliberate retry loop or a
shared cleanup tail from a one-off forward jump introduced while reconstructing
a CFG.  This audit parses every selected translation unit, resolves each goto
to its label, records the enclosing function and source/target spans, and
classifies the resulting edge without claiming that the classification proves
original authorship.

Run inside ``nix develop .#build``::

    homm2 audit gotos
    homm2 audit gotos --all
    homm2 audit gotos --category forward-single
    homm2 audit gotos --tu BASE/INPUTMGR --format json
"""

from __future__ import annotations

import argparse
import gc
import importlib
import json
import os
import re
import sys
from collections import Counter, defaultdict
from concurrent.futures import ProcessPoolExecutor
from dataclasses import asdict, dataclass, replace
from pathlib import Path
from typing import Iterable

import clang.cindex as ci

from homm2.audit.bool_fields import _entries, _portable_clang_args, _project_relative
from homm2.build.annotated_data import _clang_args, _mask_lexical_noise, configure_libclang
from homm2.clang_options import ClangMode
from homm2.core.paths import REPO


SCHEMA_VERSION = 1
OWNER_KINDS = {
    ci.CursorKind.CONSTRUCTOR,
    ci.CursorKind.CONVERSION_FUNCTION,
    ci.CursorKind.CXX_METHOD,
    ci.CursorKind.DESTRUCTOR,
    ci.CursorKind.FUNCTION_DECL,
    ci.CursorKind.FUNCTION_TEMPLATE,
}
GOTO_TOKEN = re.compile(rb"\bgoto\s+[A-Za-z_]\w*\s*;")
GOTO_EXTENT_TOKEN = re.compile(rb"\bgoto\s+[A-Za-z_]\w*\b")
CLEANUP_LABEL = re.compile(
    r"(?:cleanup|close|done|exit|fail|finish|free|restore|return|unlock)", re.IGNORECASE
)
DEFAULT_REVIEW = frozenset({"forward-single"})


@dataclass(frozen=True, order=True)
class Goto:
    file: str
    line: int
    column: int
    start: int
    end: int
    function: str
    label: str
    label_file: str
    label_line: int
    label_start: int
    direction: str
    distance_lines: int
    source_count: int
    target_statement: str
    category: str
    expression: str


def _source_slice(cursor: ci.Cursor, repo: Path) -> tuple[str, int, int, str] | None:
    start = cursor.extent.start
    end = cursor.extent.end
    if start.file is None or end.file is None or str(start.file) != str(end.file):
        return None
    relative = _project_relative(str(start.file), repo)
    if relative is None:
        return None
    path = repo / relative
    try:
        raw = path.read_bytes()[start.offset:end.offset]
    except OSError:
        return None
    # Clang's GOTO_STMT extent stops before the semicolon.
    if not GOTO_EXTENT_TOKEN.search(raw):
        return None
    return relative, start.offset, end.offset, raw.decode("utf-8", errors="replace").strip()


def _qualified_owner(cursor: ci.Cursor | None) -> str:
    if cursor is None:
        return ""
    names = [cursor.spelling or cursor.displayname]
    parent = cursor.semantic_parent
    while parent is not None and parent.kind in {
        ci.CursorKind.CLASS_DECL,
        ci.CursorKind.CLASS_TEMPLATE,
        ci.CursorKind.NAMESPACE,
        ci.CursorKind.STRUCT_DECL,
    }:
        if parent.spelling:
            names.append(parent.spelling)
        parent = parent.semantic_parent
    return "::".join(reversed([name for name in names if name]))


def _target_statement(label: ci.Cursor) -> str:
    children = list(label.get_children())
    if not children:
        return ""
    return children[0].kind.name.lower()


def analyze_translation_unit(translation: ci.TranslationUnit, repo: Path) -> list[Goto]:
    rows: list[Goto] = []

    def visit(cursor: ci.Cursor, owner: ci.Cursor | None = None) -> None:
        if cursor.kind in OWNER_KINDS:
            owner = cursor
        if cursor.kind == ci.CursorKind.GOTO_STMT:
            source = _source_slice(cursor, repo)
            label_ref = next(
                (child for child in cursor.get_children()
                 if child.kind == ci.CursorKind.LABEL_REF),
                None,
            )
            label = label_ref.referenced if label_ref is not None else None
            if source is not None and label is not None and label.location.file is not None:
                label_file = _project_relative(str(label.location.file), repo)
                if label_file is not None:
                    relative, start, end, expression = source
                    delta = label.location.line - cursor.location.line
                    rows.append(Goto(
                        file=relative,
                        line=cursor.location.line,
                        column=cursor.location.column,
                        start=start,
                        end=end,
                        function=_qualified_owner(owner),
                        label=label.spelling,
                        label_file=label_file,
                        label_line=label.location.line,
                        label_start=label.extent.start.offset,
                        direction="backward" if delta < 0 else "forward",
                        distance_lines=abs(delta),
                        source_count=0,
                        target_statement=_target_statement(label),
                        category="",
                        expression=expression,
                    ))
        for child in cursor.get_children():
            visit(child, owner)

    visit(translation.cursor)
    return rows


def _classify(row: Goto, source_count: int) -> str:
    if row.direction == "backward":
        return "backward-loop"
    if CLEANUP_LABEL.search(row.label):
        return "cleanup-tail"
    if source_count > 1:
        return "shared-tail"
    return "forward-single"


def _finalize(rows: Iterable[Goto]) -> list[Goto]:
    rows = list(rows)
    counts: dict[tuple[str, str, int, str], int] = defaultdict(int)
    for row in rows:
        counts[(row.label_file, row.function, row.label_start, row.label)] += 1
    return sorted(
        replace(
            row,
            source_count=counts[(row.label_file, row.function, row.label_start, row.label)],
            category=_classify(
                row,
                counts[(row.label_file, row.function, row.label_start, row.label)],
            ),
        )
        for row in rows
    )


def _lexical_count(repo: Path) -> int:
    total = 0
    for root in (repo / "src", repo / "include"):
        if not root.exists():
            continue
        for path in root.rglob("*"):
            if path.suffix not in {".cpp", ".h"}:
                continue
            total += len(GOTO_TOKEN.findall(_mask_lexical_noise(path.read_bytes())))
    return total


def _parse_batch(payload: tuple[Path, list[dict], bool]) -> dict:
    repo, entries, portable = payload
    configure_libclang()
    index = ci.Index.create()
    rows: list[dict] = []
    diagnostics: list[dict] = []
    for entry in entries:
        source = (Path(entry["directory"]) / entry["file"]).resolve()
        args = (_portable_clang_args(repo, entry) if portable else
                _clang_args(repo, source, mode=ClangMode.RETAIL_ANALYSIS))
        translation = index.parse(str(source), args=args)
        own_errors = [
            {
                "file": _project_relative(str(diagnostic.location.file), repo),
                "line": diagnostic.location.line,
                "column": diagnostic.location.column,
                "translation_unit": source.relative_to(repo).as_posix(),
                "message": diagnostic.spelling,
            }
            for diagnostic in translation.diagnostics
            if diagnostic.severity >= ci.Diagnostic.Error
            and diagnostic.location.file is not None
            and _project_relative(str(diagnostic.location.file), repo) is not None
        ]
        diagnostics.extend(own_errors)
        rows.extend(asdict(item) for item in analyze_translation_unit(translation, repo))
        del translation
        gc.collect()
    return {"gotos": rows, "diagnostics": diagnostics}


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
        worker = importlib.import_module("homm2.audit.gotos")._parse_batch
        with ProcessPoolExecutor(max_workers=min(worker_count, len(batches))) as pool:
            parsed = list(pool.map(worker, batches))

    unique = {
        (row["file"], row["start"], row["end"]): Goto(**row)
        for batch in parsed for row in batch["gotos"]
    }
    diagnostics = sorted({
        (item["file"], item["line"], item["column"], item["translation_unit"], item["message"]):
        item
        for batch in parsed for item in batch["diagnostics"]
    }.values(), key=lambda item: (
        item["file"], item["line"], item["column"], item["translation_unit"], item["message"]))
    gotos = _finalize(unique.values())
    counts = Counter(item.category for item in gotos)
    lexical = _lexical_count(repo)
    return {
        "schema_version": SCHEMA_VERSION,
        "whole_program": not filters,
        "filters": list(filters),
        "translation_units": len(entries),
        "gotos": [asdict(item) for item in gotos],
        "counts": dict(sorted(counts.items())),
        "lexical_gotos": lexical,
        "unmapped_lexical_gotos": lexical - len(gotos),
        "parse_diagnostics": diagnostics,
        "parse_clean": not diagnostics,
    }


def _text(report: dict, *, include_all: bool,
          categories: frozenset[str]) -> str:
    scope = "" if report["whole_program"] else "partial "
    counts = ", ".join(f"{name}={count}" for name, count in report["counts"].items())
    lines = [
        f"[{scope}gotos] {report['translation_units']} translation units, "
        f"{len(report['gotos'])} resolved gotos, "
        f"{report['unmapped_lexical_gotos']} unmapped lexical sites, "
        f"{len(report.get('parse_diagnostics', []))} parse diagnostics",
        counts,
    ]
    for item in report.get("parse_diagnostics", []):
        lines.append(
            f"PARSE {item['file']}:{item['line']}:{item['column']}: {item['message']}"
        )
    for item in report["gotos"]:
        if categories:
            if item["category"] not in categories:
                continue
        elif not include_all and item["category"] not in DEFAULT_REVIEW:
            continue
        lines.append(
            f"{item['category'].upper()} {item['file']}:{item['line']} "
            f"{item['function']}: goto {item['label']} -> "
            f"{item['label_file']}:{item['label_line']} "
            f"({item['distance_lines']} lines, {item['source_count']} source(s), "
            f"{item['target_statement']})"
        )
    return "\n".join(lines) + "\n"


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--format", choices=("text", "json"), default="text")
    parser.add_argument("--output", type=Path, help="write report instead of stdout")
    parser.add_argument("--all", action="store_true", help="show every goto")
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
        print(f"homm2 audit gotos: {error}", file=sys.stderr)
        return 2
    unknown = sorted(set(args.category) - set(report["counts"]))
    if unknown:
        print("homm2 audit gotos: unknown/empty categories: " + ", ".join(unknown),
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
