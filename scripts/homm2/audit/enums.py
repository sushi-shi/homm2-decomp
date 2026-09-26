"""Group evaluated project enumerators by value for semantic reuse review.

Integer equality is a search lead, never an automatic replacement rule. The
retail-analysis Clang mode preserves the VC6 integer representation; source
blocks retain the H2_ENUM domain names erased by that compatibility mode.
"""

from __future__ import annotations

import argparse
import csv
from dataclasses import asdict, dataclass
from functools import lru_cache
import json
from pathlib import Path
import sys

import clang.cindex as ci

from homm2.audit.bool_fields import _entries, _project_relative, _reviewed_exceptions
from homm2.build.annotated_data import _clang_args, configure_libclang
from homm2.clang_options import ClangMode
from homm2.constants_syntax import lex, parse_enum_declarations
from homm2.core.paths import REPO


@dataclass(frozen=True, order=True)
class Constant:
    value: int
    name: str
    file: str
    line: int
    column: int
    offset: int
    enum: str
    enum_line: int
    enum_offset: int


@lru_cache(None)
def source_blocks(path: Path) -> list[dict]:
    text = path.read_text(encoding="utf-8")
    result = []
    for block in parse_enum_declarations(path, text):
        opening = text[block.start:block.end]
        tokens = lex(opening)
        if tokens[0].text.startswith("H2_ENUM"):
            name = tokens[2].text
        else:
            name_index = 2 if tokens[0].text == "typedef" else 1
            if tokens[name_index].text in {"class", "struct"}:
                name_index += 1
            candidate = tokens[name_index].text
            name = candidate if candidate.isidentifier() else ""
        # The lexer indexes Unicode characters; Clang locations index UTF-8 bytes.
        offset = len(text[:block.start].encode("utf-8"))
        end_offset = offset + len(opening.encode("utf-8"))
        result.append({"line": block.line, "end_line": block.end_line,
                       "offset": offset, "end_offset": end_offset,
                       "enum": name or f"anonymous@{block.line}:{offset}"})
    return result


def scan_file(path: Path, arguments: list[str], *, root: Path,
              accepted_diagnostics: frozenset[tuple[str, str]] = frozenset(),
              diagnostic_notes: list[dict] | None = None) -> tuple[Constant, ...]:
    """Evaluate expressions/implicit values; fail on unreviewed parse errors."""
    root = root.resolve()
    configure_libclang()
    translation = ci.Index.create().parse(str(path.resolve()), args=arguments)
    errors = []
    for diagnostic in translation.diagnostics:
        if diagnostic.severity < ci.Diagnostic.Error:
            continue
        relative = (_project_relative(diagnostic.location.file.name, root)
                    if diagnostic.location.file else None)
        # The pinned VC6 SDK contains pre-standard templates Clang rejects.
        # Follow the existing audits' project-source boundary, but expose every
        # tolerated diagnostic and never suppress fatal/missing-header errors.
        if (diagnostic.severity == ci.Diagnostic.Error and diagnostic.location.file
                and diagnostic.location.is_in_system_header and relative is None):
            if diagnostic_notes is not None:
                diagnostic_notes.append({"translation_unit": str(path.relative_to(root)),
                                         "diagnostic": str(diagnostic), "kind": "sdk-dialect"})
            continue
        if (relative, diagnostic.spelling) in accepted_diagnostics and diagnostic_notes is not None:
            diagnostic_notes.append({"translation_unit": str(path.relative_to(root)),
                                     "diagnostic": str(diagnostic), "kind": "reviewed-project"})
        if (relative, diagnostic.spelling) not in accepted_diagnostics:
            errors.append(str(diagnostic))
    if errors:
        raise ValueError(f"{path}: parsing failed:\n" + "\n".join(errors))

    @lru_cache(None)
    def blocks(filename: str) -> list[dict]:
        return source_blocks(root / filename)

    constants = set()
    @lru_cache(None)
    def project_file(filename):
        return _project_relative(filename, root)

    def walk(cursor):
        location = cursor.location
        relative = None
        if location.file:
            relative = project_file(location.file.name)
            if location.is_in_system_header or relative is None:
                return
            if not blocks(relative):
                return
            start, end = cursor.extent.start, cursor.extent.end
            if (start.file == location.file == end.file and
                    not any(start.line <= block["end_line"] and end.line >= block["line"]
                            for block in blocks(relative))):
                return
        if cursor.kind == ci.CursorKind.ENUM_CONSTANT_DECL and location.file:
            owner = next((block for block in blocks(relative)
                          if block["offset"] <= location.offset < block["end_offset"]), None)
            # Exclude compatibility machinery (Ints.h) and non-enum macro output.
            if owner:
                constants.add(Constant(cursor.enum_value, cursor.spelling, relative,
                                       location.line, location.column, location.offset,
                                       owner["enum"], owner["line"], owner["offset"]))
        for child in cursor.get_children():
            walk(child)
    walk(translation.cursor)
    return tuple(sorted(constants))


def group_values(rows: list[dict]) -> dict[int, list[dict]]:
    groups: dict[int, list[dict]] = {}
    for row in rows:
        groups.setdefault(row["value"], []).append(row)
    return dict(sorted(groups.items()))


def collect(repo: Path = REPO, *, filters: tuple[str, ...] = (), strict: bool = False) -> dict:
    source_blocks.cache_clear()
    repo = repo.resolve()
    entries = _entries(repo, filters)
    if not entries:
        raise ValueError("no translation units selected")
    accepted = frozenset((row.file, row.detail)
                         for row in _reviewed_exceptions(repo).values()
                         if row.category == "parse-diagnostic")
    rows = {}
    diagnostics = []
    failures = []
    for index, entry in enumerate(entries, 1):
        source = (Path(entry["directory"]) / entry["file"]).resolve()
        print(f"[enums] {index}/{len(entries)} {source.relative_to(repo)}", file=sys.stderr)
        arguments = _clang_args(repo, source, mode=ClangMode.STRICT if strict else ClangMode.RETAIL_ANALYSIS)
        try:
            constants = scan_file(source, arguments, root=repo, accepted_diagnostics=accepted,
                                  diagnostic_notes=diagnostics)
        except ValueError as error:
            failures.append(str(error))
            continue
        for constant in constants:
            # Keep conditional values at one declaration as separate rows.
            key = (constant.file, constant.offset, constant.name, constant.value)
            row = rows.setdefault(key, {**asdict(constant), "contexts": []})
            context = source.relative_to(repo).as_posix()
            if context not in row["contexts"]:
                row["contexts"].append(context)
    if failures:
        raise ValueError("\n".join(failures))
    constants = sorted(rows.values(), key=lambda row: (row["value"], row["file"], row["offset"]))
    observed = {(row["file"], row["enum_offset"]) for row in constants}
    lexical = []
    for directory in ("src", "include"):
        for path in sorted((repo / directory).rglob("*")):
            if path.suffix not in {".cpp", ".h"}:
                continue
            for block in source_blocks(path):
                lexical.append({"file": path.relative_to(repo).as_posix(), **block})
    missing = [block for block in lexical if (block["file"], block["offset"]) not in observed]
    return {"schema": 1, "mode": "strict" if strict else "retail",
            "translation_units": len(entries), "constants": constants,
            "accepted_diagnostics": diagnostics,
            "source_blocks": lexical, "unobserved_blocks": missing,
            "partial": bool(filters),
            "by_value": {str(value): members for value, members in group_values(constants).items()}}


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--tu", action="append", default=[], help="TU substring (repeatable)")
    parser.add_argument("--value", action="append", type=lambda text: int(text, 0), default=[])
    parser.add_argument("--duplicates", action="store_true")
    parser.add_argument("--strict", action="store_true", help="evaluate the modern typed-enum view")
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args(argv)
    try:
        report = collect(filters=tuple(args.tu), strict=args.strict)
    except (ValueError, RuntimeError, OSError, ci.TranslationUnitLoadError) as error:
        print(f"enums: {error}", file=sys.stderr)
        return 2
    groups = group_values(report["constants"])
    selected = {value: members for value, members in groups.items()
                if (not args.value or value in args.value)
                and (not args.duplicates or len(members) > 1)}
    report["constants"] = [row for members in selected.values() for row in members]
    report["by_value"] = {str(value): members for value, members in selected.items()}
    report["partial"] = report["partial"] or bool(args.value or args.duplicates)
    if args.json:
        print(json.dumps(report, indent=2))
    else:
        writer = csv.writer(sys.stdout, delimiter="\t", lineterminator="\n")
        writer.writerow(("value", "hex", "name", "file", "line", "enum"))
        for row in report["constants"]:
            writer.writerow((row["value"], hex(row["value"]), row["name"], row["file"],
                             row["line"], row["enum"]))
    print(f"{len(report['constants'])} declarations; {len(groups)} values; "
          f"{len(report['source_blocks'])} source enum blocks; "
          f"{len(report['unobserved_blocks'])} unobserved blocks", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
