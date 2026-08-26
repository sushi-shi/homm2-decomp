#!/usr/bin/env python3
"""Find integer members, globals, and locals whose observed write domain is Boolean.

This is a conservative, source-identity-aware census.  It uses libclang and the
repository's generated compilation database, so two same-spelled variables
never become one textual bucket. Storage is proposed only when:

* its declaration is spelled ``i32``, ``i8``, or plain ``char`` in
  project-owned source;
* the accumulated observed write domain is exactly ``{0, 1}``;
* every visible direct write is provably confined to that domain; and
* the object is not exposed directly through a mutable pointer/reference or
  another write shape the scanner cannot prove.

The scanner is deliberately an audit, not a rewrite command.  Its JSON report
contains declaration and literal-expression byte spans so a reviewed change
can replace ``i32`` with ``b32``, ``i8`` with ``b8``, or plain ``char`` with
``bchar`` while replacing associated ``0``/``1`` literals with
``false``/``true``. ``bchar`` deliberately preserves plain-char C++ identity
for globals whose decorated retail symbol depends on it. The audit also reports
numeric literal writes and unproven writes to already recovered Boolean
storage; ``--check`` fails on every actionable kind of remaining cleanup.
Boolean parameters are inventoried separately: their incoming domain belongs
to the function contract and is not itself a bad write.

Parameters are inventoried but normally remain rejected until a separate
call-site proof establishes their incoming domain. Aggregate byte writes and
external deserialization are outside this direct-write census and remain review
boundaries rather than inferred Boolean evidence.

Run inside ``nix develop .#build``::

    homm2 audit bool-fields
    homm2 audit bool-fields --check
    homm2 audit bool-fields --format json --output build/bool-fields.json
    homm2 audit bool-fields --all --tu SOURCE/HERO
"""

from __future__ import annotations

import argparse
import gc
import importlib
import json
import os
import re
import sys
from concurrent.futures import ProcessPoolExecutor
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Iterable

import clang.cindex as ci

from homm2.build.annotated_data import (
    _clang_args,
    configure_libclang,
)
from homm2.clang_options import ClangMode
from homm2.core.paths import REPO


SCHEMA_VERSION = 2
DATABASE = Path("build/clangd/compile_commands.json")
PROJECT_ROOTS = ("include", "src")
RECORD_KINDS = {
    ci.CursorKind.CLASS_DECL,
    ci.CursorKind.STRUCT_DECL,
}
REFERENCE_KINDS = {
    ci.CursorKind.DECL_REF_EXPR,
    ci.CursorKind.MEMBER_REF,
    ci.CursorKind.MEMBER_REF_EXPR,
}
WRAPPER_KINDS = {
    ci.CursorKind.UNEXPOSED_EXPR,
    ci.CursorKind.PAREN_EXPR,
    ci.CursorKind.CSTYLE_CAST_EXPR,
    ci.CursorKind.CXX_STATIC_CAST_EXPR,
    ci.CursorKind.CXX_FUNCTIONAL_CAST_EXPR,
    ci.CursorKind.CXX_CONST_CAST_EXPR,
    ci.CursorKind.CXX_REINTERPRET_CAST_EXPR,
}
MUTABLE_REFERENCE_KINDS = {
    ci.TypeKind.LVALUEREFERENCE,
    ci.TypeKind.RVALUEREFERENCE,
}
SOURCE_BOOLEAN_TARGETS = {
    "i32": "b32",
    "i8": "b8",
    "char": "bchar",
}
BOOLEAN_TYPES = frozenset(SOURCE_BOOLEAN_TARGETS.values())
TRACKED_TYPES = frozenset(SOURCE_BOOLEAN_TARGETS) | BOOLEAN_TYPES
STORAGE_DECL_KINDS = {
    ci.CursorKind.FIELD_DECL: "field",
    ci.CursorKind.VAR_DECL: "variable",
    ci.CursorKind.PARM_DECL: "parameter",
}


@dataclass(frozen=True, order=True)
class SourceSpan:
    file: str
    line: int
    column: int
    start: int
    end: int


@dataclass(frozen=True, order=True)
class Write:
    file: str
    line: int
    column: int
    start: int
    end: int
    kind: str
    expression: str
    domain: tuple[int, ...] | None
    replacement: str | None = None


@dataclass
class FieldFacts:
    usr: str
    record_usr: str
    record: str
    name: str
    declared_type: str
    storage_kind: str = "field"
    declarations: set[SourceSpan] = field(default_factory=set)
    type_spans: set[SourceSpan] = field(default_factory=set)
    writes: set[Write] = field(default_factory=set)
    unknown_writes: set[Write] = field(default_factory=set)
    read_locations: set[tuple[str, int, int]] = field(default_factory=set)
    translation_units: set[str] = field(default_factory=set)

    @property
    def observed_domain(self) -> tuple[int, ...]:
        return tuple(sorted({value for write in self.writes
                             for value in (write.domain or ())}))

    @property
    def eligible(self) -> bool:
        # A member seen only as zero is not positive Boolean evidence: unused
        # payload words and reset-only queue indexes have exactly that shape.
        return (self.declared_type in SOURCE_BOOLEAN_TARGETS
                and self.observed_domain == (0, 1)
                and not self.unknown_writes)

    @property
    def target_type(self) -> str | None:
        return SOURCE_BOOLEAN_TARGETS.get(self.declared_type)


def _project_relative(path: str | Path | None, repo: Path) -> str | None:
    if path is None:
        return None
    try:
        relative = Path(path).resolve().relative_to(repo.resolve()).as_posix()
    except (OSError, ValueError):
        return None
    if relative.split("/", 1)[0] not in PROJECT_ROOTS:
        return None
    return relative


def _span(cursor: ci.Cursor, repo: Path) -> SourceSpan | None:
    location = cursor.location
    relative = _project_relative(str(location.file) if location.file else None, repo)
    if relative is None:
        return None
    return SourceSpan(
        file=relative,
        line=location.line,
        column=location.column,
        start=cursor.extent.start.offset,
        end=cursor.extent.end.offset,
    )


def _tokens(cursor: ci.Cursor) -> tuple[str, ...]:
    return tuple(token.spelling for token in cursor.get_tokens())


def _expression(cursor: ci.Cursor) -> str:
    text = " ".join(_tokens(cursor))
    return text if len(text) <= 160 else text[:157] + "..."


def _integer_literal(text: str) -> int | None:
    value = text.replace("'", "")
    match = re.fullmatch(r"(0[xX][0-9a-fA-F]+|0[bB][01]+|0[0-7]*|[1-9][0-9]*)(?:[uUlL]*)", value)
    if match is None:
        return None
    literal = match.group(1)
    if literal.lower().startswith("0x"):
        base = 16
    elif literal.lower().startswith("0b"):
        base = 2
    elif len(literal) > 1 and literal.startswith("0"):
        base = 8
    else:
        base = 10
    return int(literal, base)


def _boolean_domain(cursor: ci.Cursor) -> tuple[int, ...] | None:
    """Return the expression's proven subset of ``{0, 1}``, or ``None``."""
    children = list(cursor.get_children())
    if cursor.kind == ci.CursorKind.CXX_BOOL_LITERAL_EXPR:
        tokens = _tokens(cursor)
        if tokens and tokens[-1] in ("false", "true"):
            return (int(tokens[-1] == "true"),)
        return (0, 1)
    if cursor.kind == ci.CursorKind.INTEGER_LITERAL:
        tokens = _tokens(cursor)
        value = _integer_literal(tokens[-1]) if tokens else None
        return (value,) if value in (0, 1) else None
    if cursor.kind == ci.CursorKind.ENUM_CONSTANT_DECL:
        value = cursor.enum_value
        return (value,) if value in (0, 1) else None
    if cursor.kind == ci.CursorKind.DECL_REF_EXPR and cursor.referenced is not None:
        referenced = cursor.referenced
        if referenced.kind == ci.CursorKind.ENUM_CONSTANT_DECL:
            value = referenced.enum_value
            return (value,) if value in (0, 1) else None
    if cursor.kind in WRAPPER_KINDS and len(children) == 1:
        return _boolean_domain(children[0])
    if cursor.kind == ci.CursorKind.CONDITIONAL_OPERATOR and len(children) == 3:
        left = _boolean_domain(children[1])
        right = _boolean_domain(children[2])
        if left is not None and right is not None:
            return tuple(sorted(set(left) | set(right)))
    if cursor.kind == ci.CursorKind.BINARY_OPERATOR and cursor.spelling == "," and children:
        return _boolean_domain(children[-1])
    if cursor.kind == ci.CursorKind.BINARY_OPERATOR and cursor.spelling == "=" and children:
        return _boolean_domain(children[-1])
    # The aliases are semantic Boolean storage contracts maintained by this
    # audit. A value read through one of them is valid Boolean flow even though
    # its ABI-preserving underlying C++ type remains integral.
    if cursor.type.spelling in BOOLEAN_TYPES:
        return (0, 1)
    # Comparisons, logical operators, calls returning bool, and unary ! all have
    # a real Boolean result even in the retail C++98 analysis mode.
    if cursor.type.kind == ci.TypeKind.BOOL:
        return (0, 1)
    return None


def _literal_replacement(cursor: ci.Cursor, domain: tuple[int, ...] | None) -> str | None:
    if domain not in ((0,), (1,)):
        return None
    children = list(cursor.get_children())
    if cursor.kind in WRAPPER_KINDS and len(children) == 1:
        return _literal_replacement(children[0], domain)
    current = _tokens(cursor)
    if len(current) == 1:
        value = _integer_literal(current[0])
        if value in (0, 1):
            return "true" if value else "false"
    return None


def _write(cursor: ci.Cursor, kind: str, repo: Path,
           domain: tuple[int, ...] | None) -> Write | None:
    span = _span(cursor, repo)
    if span is None:
        return None
    return Write(
        file=span.file,
        line=span.line,
        column=span.column,
        start=span.start,
        end=span.end,
        kind=kind,
        expression=_expression(cursor),
        domain=domain,
        replacement=_literal_replacement(cursor, domain),
    )


def _qualified_record(cursor: ci.Cursor) -> str:
    names = []
    current = cursor
    while current is not None and current.kind != ci.CursorKind.TRANSLATION_UNIT:
        if current.spelling:
            names.append(current.spelling)
        current = current.semantic_parent
    return "::".join(reversed(names)) or "<anonymous>"


def _type_span(cursor: ci.Cursor, repo: Path, declared_type: str) -> SourceSpan | None:
    for child in cursor.get_children():
        if child.kind == ci.CursorKind.TYPE_REF and child.spelling == declared_type:
            return _span(child, repo)
    # Built-in types and some macro-expanded declarations do not carry a
    # TypeRef. Use the exact declaration token only when its spelling is
    # unambiguous and it occurs before the declared name.
    matches = []
    for token in cursor.get_tokens():
        if token.spelling != declared_type:
            continue
        location = token.location
        relative = _project_relative(str(location.file) if location.file else None, repo)
        if relative is None:
            continue
        matches.append(SourceSpan(
            file=relative,
            line=location.line,
            column=location.column,
            start=token.extent.start.offset,
            end=token.extent.end.offset,
        ))
    if len(matches) == 1:
        return matches[0]
    return None


def _field_usr(cursor: ci.Cursor) -> str:
    usr = cursor.get_usr()
    if usr:
        return usr
    location = cursor.location
    return f"{location.file}:{location.line}:{location.column}:{cursor.spelling}"


def _referenced_field(cursor: ci.Cursor, candidates: dict[str, FieldFacts]) -> str | None:
    if cursor.kind not in REFERENCE_KINDS or cursor.referenced is None:
        return None
    usr = _field_usr(cursor.referenced)
    return usr if usr in candidates else None


def _field_references(cursor: ci.Cursor, candidates: dict[str, FieldFacts]) -> list[tuple[str, ci.Cursor]]:
    rows = []
    for child in cursor.walk_preorder():
        usr = _referenced_field(child, candidates)
        if usr is not None:
            rows.append((usr, child))
    return rows


def _record_declaration(type_: ci.Type) -> ci.Cursor | None:
    declaration = type_.get_declaration()
    return declaration if declaration is not None and declaration.kind in RECORD_KINDS else None


def _tracked_fields(cursor: ci.Cursor, repo: Path, unit: str) -> dict[str, FieldFacts]:
    out = {}
    for field_cursor in cursor.walk_preorder():
        storage_kind = STORAGE_DECL_KINDS.get(field_cursor.kind)
        declared_type = field_cursor.type.spelling
        if storage_kind is None or declared_type not in TRACKED_TYPES:
            continue
        declaration = _span(field_cursor, repo)
        type_span = _type_span(field_cursor, repo, declared_type)
        parent = field_cursor.semantic_parent
        if declaration is None or type_span is None or parent is None:
            continue
        # Writing one member of a union overwrites all of them; direct member
        # assignments cannot establish a field-local value domain there.
        if field_cursor.kind == ci.CursorKind.FIELD_DECL and parent.kind == ci.CursorKind.UNION_DECL:
            continue
        usr = _field_usr(field_cursor)
        current = out.get(usr)
        if current is None:
            current = FieldFacts(
                usr=usr,
                record_usr=parent.get_usr(),
                record=_qualified_record(parent),
                name=field_cursor.spelling,
                declared_type=declared_type,
                storage_kind=storage_kind,
            )
            out[usr] = current
        current.declarations.add(declaration)
        current.type_spans.add(type_span)
        current.translation_units.add(unit)
    return out


def _mark(candidates: dict[str, FieldFacts], usr: str, value: Write | None,
          handled: set[tuple[str, int, int, int]]) -> None:
    if value is None:
        return
    key = (usr, value.start, value.end, value.line)
    handled.add(key)
    if value.domain is None:
        candidates[usr].unknown_writes.add(value)
    else:
        candidates[usr].writes.add(value)


def _analyze_cursor(cursor: ci.Cursor, candidates: dict[str, FieldFacts], repo: Path,
                    handled: set[tuple[str, int, int, int]]) -> None:
    children = list(cursor.get_children())

    if cursor.kind == ci.CursorKind.CONSTRUCTOR:
        # libclang exposes each member initializer as adjacent direct children:
        # MEMBER_REF(field), initializer-expression.  The function body starts
        # at COMPOUND_STMT and is traversed normally below.
        index = 0
        while index + 1 < len(children) and children[index].kind != ci.CursorKind.COMPOUND_STMT:
            usr = _referenced_field(children[index], candidates)
            if usr is not None:
                initializer = children[index + 1]
                _mark(candidates, usr,
                      _write(initializer, "constructor-initializer", repo,
                             _boolean_domain(initializer)), handled)
                ref_span = _span(children[index], repo)
                if ref_span is not None:
                    handled.add((usr, ref_span.start, ref_span.end, ref_span.line))
                index += 2
            else:
                index += 1

    if cursor.kind == ci.CursorKind.FIELD_DECL:
        usr = _field_usr(cursor)
        if usr in candidates:
            initializers = [child for child in children if child.kind != ci.CursorKind.TYPE_REF]
            if initializers:
                initializer = initializers[-1]
                _mark(candidates, usr,
                      _write(initializer, "field-initializer", repo,
                             _boolean_domain(initializer)), handled)

    if cursor.kind in (ci.CursorKind.VAR_DECL, ci.CursorKind.PARM_DECL):
        usr = _field_usr(cursor)
        if usr in candidates:
            if cursor.kind == ci.CursorKind.PARM_DECL:
                _mark(candidates, usr,
                      _write(cursor, "incoming-parameter", repo, None), handled)
            else:
                ignored = {
                    ci.CursorKind.ANNOTATE_ATTR,
                    ci.CursorKind.NAMESPACE_REF,
                    ci.CursorKind.TEMPLATE_REF,
                    ci.CursorKind.TYPE_REF,
                }
                initializers = [child for child in children if child.kind not in ignored]
                if initializers:
                    initializer = initializers[-1]
                    _mark(candidates, usr,
                          _write(initializer, "variable-initializer", repo,
                                 _boolean_domain(initializer)), handled)
                else:
                    parent = cursor.semantic_parent
                    has_static_storage = (
                        parent is not None
                        and parent.kind in (ci.CursorKind.TRANSLATION_UNIT,
                                            ci.CursorKind.NAMESPACE)
                    ) or cursor.storage_class == ci.StorageClass.STATIC
                    if cursor.is_definition() and has_static_storage:
                        _mark(candidates, usr,
                              _write(cursor, "static-implicit-zero", repo, (0,)), handled)
                    elif cursor.is_definition():
                        _mark(candidates, usr,
                              _write(cursor, "uninitialized-local", repo, None), handled)

    if cursor.kind == ci.CursorKind.INIT_LIST_EXPR:
        declaration = _record_declaration(cursor.type)
        if declaration is not None:
            fields = [child for child in declaration.get_children()
                      if child.kind == ci.CursorKind.FIELD_DECL]
            values = list(cursor.get_children())
            for index, field_cursor in enumerate(fields):
                usr = _field_usr(field_cursor)
                if usr not in candidates:
                    continue
                if index < len(values):
                    value = values[index]
                    write = _write(value, "aggregate-initializer", repo,
                                   _boolean_domain(value))
                else:
                    write = _write(cursor, "aggregate-implicit-zero", repo, (0,))
                _mark(candidates, usr, write, handled)

    if cursor.kind in (ci.CursorKind.BINARY_OPERATOR,
                       ci.CursorKind.COMPOUND_ASSIGNMENT_OPERATOR) and len(children) >= 2:
        operator = cursor.spelling
        lhs, rhs = children[0], children[-1]
        refs = _field_references(lhs, candidates)
        if refs and (operator == "=" or cursor.kind == ci.CursorKind.COMPOUND_ASSIGNMENT_OPERATOR):
            domain = _boolean_domain(rhs) if operator == "=" else None
            kind = "assignment" if operator == "=" else "compound-assignment"
            write = _write(rhs if operator == "=" else cursor, kind, repo, domain)
            for usr, reference in refs:
                _mark(candidates, usr, write, handled)
                ref_span = _span(reference, repo)
                if ref_span is not None:
                    handled.add((usr, ref_span.start, ref_span.end, ref_span.line))

    if cursor.kind == ci.CursorKind.UNARY_OPERATOR:
        tokens = _tokens(cursor)
        operator = tokens[0] if tokens and tokens[0] in ("&", "++", "--") else (
            tokens[-1] if tokens and tokens[-1] in ("++", "--") else None)
        if operator is not None:
            kind = "address-escape" if operator == "&" else "unary-update"
            for usr, reference in _field_references(cursor, candidates):
                _mark(candidates, usr, _write(cursor, kind, repo, None), handled)
                ref_span = _span(reference, repo)
                if ref_span is not None:
                    handled.add((usr, ref_span.start, ref_span.end, ref_span.line))

    if cursor.kind == ci.CursorKind.CALL_EXPR and cursor.referenced is not None:
        arguments = list(cursor.get_arguments())
        parameters = list(cursor.referenced.get_arguments())
        for argument, parameter in zip(arguments, parameters):
            # Direct mutable binding of a scalar field is a possible write even
            # without an explicit '&'.  Pointer arguments are already caught by
            # the unary address-escape case, but recording both is harmless.
            if parameter.type.kind in MUTABLE_REFERENCE_KINDS:
                pointee = parameter.type.get_pointee()
                if not pointee.is_const_qualified():
                    for usr, reference in _field_references(argument, candidates):
                        _mark(candidates, usr,
                              _write(argument, "mutable-reference-argument", repo, None), handled)
                        ref_span = _span(reference, repo)
                        if ref_span is not None:
                            handled.add((usr, ref_span.start, ref_span.end, ref_span.line))

    for child in children:
        _analyze_cursor(child, candidates, repo, handled)


def analyze_translation_unit(translation: ci.TranslationUnit, source: Path,
                             repo: Path) -> list[FieldFacts]:
    unit = source.resolve().relative_to((repo / "src").resolve()).with_suffix("").as_posix()
    candidates = _tracked_fields(translation.cursor, repo, unit)
    handled: set[tuple[str, int, int, int]] = set()
    _analyze_cursor(translation.cursor, candidates, repo, handled)

    # Every remaining member/global/local reference is a read. Keeping the locations is useful in
    # review and, more importantly, prevents an unclassified cursor from being
    # silently treated as a safe write.
    for cursor in translation.cursor.walk_preorder():
        usr = _referenced_field(cursor, candidates)
        if usr is None:
            continue
        span = _span(cursor, repo)
        if span is None:
            continue
        key = (usr, span.start, span.end, span.line)
        if key not in handled:
            candidates[usr].read_locations.add((span.file, span.line, span.column))
    return list(candidates.values())


def _parse_batch(arguments: tuple[Path, list[dict]]) -> list[dict]:
    repo, entries = arguments
    configure_libclang()
    index = ci.Index.create()
    rows = []
    for entry in entries:
        source = (Path(entry["directory"]) / entry["file"]).resolve()
        translation = index.parse(
            str(source), args=_clang_args(repo, source, mode=ClangMode.RETAIL_ANALYSIS))
        own_errors = []
        for diagnostic in translation.diagnostics:
            if diagnostic.severity < ci.Diagnostic.Error or diagnostic.location.file is None:
                continue
            if _project_relative(str(diagnostic.location.file), repo) is not None:
                own_errors.append(str(diagnostic))
        if own_errors:
            raise RuntimeError(
                f"{source.relative_to(repo)}: libclang project diagnostics: "
                + "; ".join(own_errors[:8]))
        for facts in analyze_translation_unit(translation, source, repo):
            row = asdict(facts)
            row["declarations"] = [asdict(item) for item in sorted(facts.declarations)]
            row["type_spans"] = [asdict(item) for item in sorted(facts.type_spans)]
            row["writes"] = [asdict(item) for item in sorted(facts.writes)]
            row["unknown_writes"] = [asdict(item) for item in sorted(facts.unknown_writes)]
            row["read_locations"] = sorted(facts.read_locations)
            row["translation_units"] = sorted(facts.translation_units)
            rows.append(row)
        del translation
        gc.collect()
    return rows


def _entries(repo: Path, filters: Iterable[str] = ()) -> list[dict]:
    database = repo / DATABASE
    if not database.is_file():
        raise RuntimeError(
            f"{DATABASE} not found; run `homm2 clangd` inside `nix develop .#build`")
    raw = json.loads(database.read_text())
    filters = tuple(value.lower() for value in filters)
    out = []
    for entry in raw:
        directory = Path(entry.get("directory", repo))
        source = (directory / entry.get("file", "")).resolve()
        try:
            relative = source.relative_to(repo.resolve()).as_posix()
        except ValueError as error:
            raise RuntimeError(f"compilation database entry is outside this worktree: {source}") from error
        if not source.is_file():
            raise RuntimeError(f"compilation database source is missing: {relative}")
        if filters and not any(value in relative.lower() for value in filters):
            continue
        out.append({**entry, "directory": str(directory)})
    return sorted(out, key=lambda row: row["file"])


def _merge(rows: list[dict]) -> list[FieldFacts]:
    merged: dict[str, FieldFacts] = {}
    for row in rows:
        usr = row["usr"]
        current = merged.get(usr)
        declarations = {SourceSpan(**item) for item in row["declarations"]}
        type_spans = {SourceSpan(**item) for item in row["type_spans"]}
        if current is None:
            current = FieldFacts(
                usr=usr,
                record_usr=row["record_usr"],
                record=row["record"],
                name=row["name"],
                declared_type=row["declared_type"],
                storage_kind=row.get("storage_kind", "field"),
            )
            merged[usr] = current
        elif (current.record, current.name, current.declared_type,
              current.storage_kind) != (
                row["record"], row["name"], row["declared_type"],
                row.get("storage_kind", "field")):
            raise RuntimeError(f"inconsistent repeated field identity: {usr}")
        current.declarations.update(declarations)
        current.type_spans.update(type_spans)
        current.writes.update(Write(**item) for item in row["writes"])
        current.unknown_writes.update(Write(**item) for item in row["unknown_writes"])
        current.read_locations.update(tuple(item) for item in row["read_locations"])
        current.translation_units.update(row["translation_units"])
    return sorted(merged.values(), key=lambda item: (
        min(item.declarations).file,
        min(item.declarations).line,
        min(item.declarations).column))


def scan(repo: Path = REPO, *, jobs: int = 0,
         filters: Iterable[str] = ()) -> dict:
    entries = _entries(repo, filters)
    if not entries:
        raise RuntimeError("no translation units selected")
    worker_count = jobs if jobs > 0 else min(8, max(1, (os.cpu_count() or 4) // 2))
    batch_size = max(1, (len(entries) + worker_count - 1) // worker_count)
    batches = [(repo, entries[index:index + batch_size])
               for index in range(0, len(entries), batch_size)]
    if len(batches) == 1:
        raw = _parse_batch(batches[0])
    else:
        # `homm2 audit` dispatches tools through runpy with run_name="__main__".
        # ProcessPool cannot pickle that transient module identity, so always
        # hand it the function from the canonically importable package module.
        worker = importlib.import_module("homm2.audit.bool_fields")._parse_batch
        with ProcessPoolExecutor(max_workers=min(worker_count, len(batches))) as pool:
            raw = [row for batch in pool.map(worker, batches) for row in batch]
    fields = _merge(raw)
    source_storage = [item for item in fields if item.declared_type in SOURCE_BOOLEAN_TARGETS]
    boolean_storage = [item for item in fields if item.declared_type in BOOLEAN_TYPES]
    candidates = [item for item in source_storage if item.eligible]
    rejected = [item for item in source_storage if not item.eligible]
    field_candidates = [item for item in candidates if item.storage_kind == "field"]
    field_rejected = [item for item in rejected if item.storage_kind == "field"]

    def emit(item: FieldFacts) -> dict:
        return {
            "usr": item.usr,
            "record": item.record,
            "name": item.name,
            "declared_type": item.declared_type,
            "target_type": item.target_type,
            "storage_kind": item.storage_kind,
            "qualified_name": f"{item.record}::{item.name}",
            "declarations": [asdict(value) for value in sorted(item.declarations)],
            "type_spans": [asdict(value) for value in sorted(item.type_spans)],
            "writes": [asdict(value) for value in sorted(item.writes)],
            "unknown_writes": [asdict(value) for value in sorted(item.unknown_writes)],
            "observed_domain": list(item.observed_domain),
            "read_count": len(item.read_locations),
            "translation_units": sorted(item.translation_units),
            "eligible": item.eligible,
        }

    numeric_literal_writes = [
        {
            "qualified_name": f"{item.record}::{item.name}",
            "storage_kind": item.storage_kind,
            "declarations": [asdict(value) for value in sorted(item.declarations)],
            "write": asdict(write),
        }
        for item in boolean_storage
        for write in sorted(item.writes)
        if write.replacement is not None
    ]
    unproven_boolean_writes = [
        {
            "qualified_name": f"{item.record}::{item.name}",
            "storage_kind": item.storage_kind,
            "declarations": [asdict(value) for value in sorted(item.declarations)],
            "write": asdict(write),
        }
        for item in boolean_storage
        for write in sorted(item.unknown_writes)
        if write.kind != "incoming-parameter"
    ]
    boolean_parameters = [emit(item) for item in boolean_storage
                          if item.storage_kind == "parameter"]
    return {
        "schema_version": SCHEMA_VERSION,
        "translation_units": len(entries),
        # Legacy count names remain machine-readable for report consumers.
        "i32_fields": sum(item.declared_type == "i32" and item.storage_kind == "field"
                          for item in fields),
        "b32_fields": sum(item.declared_type == "b32" and item.storage_kind == "field"
                          for item in fields),
        "source_integer_storage": len(source_storage),
        "boolean_storage": len(boolean_storage),
        "storage_by_kind": {
            kind: sum(item.storage_kind == kind for item in fields)
            for kind in STORAGE_DECL_KINDS.values()
        },
        "eligible_fields": len(field_candidates),
        "rejected_fields": len(field_rejected),
        "eligible_storage": len(candidates),
        "rejected_storage": len(rejected),
        "candidates": [emit(item) for item in candidates],
        "rejected": [emit(item) for item in rejected],
        "boolean_numeric_literal_writes": numeric_literal_writes,
        "boolean_unproven_writes": unproven_boolean_writes,
        "boolean_parameters": boolean_parameters,
        "b32_numeric_literal_writes": numeric_literal_writes,
        "b32_unproven_writes": unproven_boolean_writes,
    }


def _text(report: dict, include_rejected: bool) -> str:
    lines = [
        f"[bool-fields] {report['translation_units']} translation units, "
        f"{report.get('source_integer_storage', report['i32_fields'])} source-integer storage objects, "
        f"{report.get('eligible_storage', report['eligible_fields'])} Boolean candidates, "
        f"{report.get('rejected_storage', report['rejected_fields'])} rejected; "
        f"{len(report.get('boolean_numeric_literal_writes', report['b32_numeric_literal_writes']))} "
        f"numeric and {len(report.get('boolean_unproven_writes', report['b32_unproven_writes']))} "
        f"unproven writes to Boolean storage",
    ]
    for item in report["candidates"]:
        location = item["declarations"][0]
        lines.append(
            f"CANDIDATE {location['file']}:{location['line']} "
            f"{item.get('storage_kind', 'field')} {item['qualified_name']} "
            f"{item.get('declared_type', 'i32')}->{item.get('target_type', 'b32')} "
            f"({len(item['writes'])} writes, "
            f"{item['read_count']} reads)")
        for write in item["writes"]:
            lines.append(
                f"  {write['file']}:{write['line']} {write['kind']}: "
                f"{write['expression']} -> {write['domain']}")
    if include_rejected:
        for item in report["rejected"]:
            location = item["declarations"][0]
            reasons = item["unknown_writes"] or [{
                "kind": ("no-observed-write" if not item["writes"]
                         else "insufficient-observed-domain")
            }]
            kinds = ", ".join(sorted({value["kind"] for value in reasons}))
            lines.append(
                f"REJECTED  {location['file']}:{location['line']} "
                f"{item['qualified_name']}: {kinds}")
        for item in report.get("boolean_numeric_literal_writes",
                               report["b32_numeric_literal_writes"]):
            write = item["write"]
            lines.append(
                f"BOOL-LITERAL {write['file']}:{write['line']} "
                f"{item['qualified_name']}: {write['expression']} -> "
                f"{write['replacement']}")
        for item in report.get("boolean_unproven_writes",
                               report["b32_unproven_writes"]):
            write = item["write"]
            lines.append(
                f"BOOL-UNPROVEN {write['file']}:{write['line']} "
                f"{item['qualified_name']}: {write['kind']}: "
                f"{write['expression']}")
    return "\n".join(lines) + "\n"


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--format", choices=("text", "json"), default="text")
    parser.add_argument("--output", type=Path, help="write the report instead of stdout")
    parser.add_argument("--all", action="store_true", help="include rejected fields in text")
    parser.add_argument(
        "--check", action="store_true",
        help=("fail when an integer Boolean candidate, numeric 0/1 write, or "
              "unproven write to Boolean storage remains"))
    parser.add_argument("--tu", action="append", default=[], help="limit to matching source paths")
    parser.add_argument("-j", "--jobs", type=int, default=0)
    args = parser.parse_args(argv)
    try:
        report = scan(jobs=args.jobs, filters=args.tu)
    except (OSError, RuntimeError, ValueError) as error:
        print(f"homm2 audit bool-fields: {error}", file=sys.stderr)
        return 1
    output = (json.dumps(report, indent=2, sort_keys=True) + "\n"
              if args.format == "json" else _text(report, args.all))
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(output)
    else:
        print(output, end="")
    return int(bool(args.check and (
        report.get("eligible_storage", report["eligible_fields"])
        or report["b32_numeric_literal_writes"]
        or report["b32_unproven_writes"])))


if __name__ == "__main__":
    raise SystemExit(main())
