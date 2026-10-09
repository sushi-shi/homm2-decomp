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
The few byte-proven retail truth-value writes and parser-dialect diagnostics
live in ``config/reviews/bool_exceptions.tsv``. Full checks fail when an entry is
no longer observed, so the manifest cannot silently become a stale allowlist.
When one declaration statement owns Boolean and non-Boolean declarators, the
candidate is marked ``requires_declaration_split``: its type token must not be
changed until the declaration is split.
Boolean parameter storage is inventoried separately: its incoming domain belongs
to the function contract and is not itself a bad write.

Function contracts are proven over the whole program as well. A function
returning ``i32``, ``i8`` or plain char is a result candidate when every
``return`` yields ``0``/``1``, a comparison or logical value, Boolean storage,
or a source this proof also establishes (another candidate result or
candidate storage), with an observed domain of exactly ``{0, 1}``. Virtual
methods, operators and results a caller uses numerically (arithmetic,
indexing, ``switch``, comparison with a non-Boolean) keep their integer type
and are listed with the reason. A parameter is a candidate when
every call site's argument (or the parameter's default) and every write in the
body are proven the same way; a function whose address is taken has callers
the scan cannot see, so its parameters stay integral, and a parameter its body
uses numerically (an index, an operand) keeps its integer type. Already Boolean results
must keep returning proven values: a numeric ``0``/``1`` return or default
argument, or an unproven return, fails ``--check`` like the corresponding
storage write. The scan also inventories arguments to recovered Boolean
parameters: numeric ``0``/``1`` arguments and arguments outside a proven
Boolean domain fail ``--check``, as does an ``==``/``!=`` comparison of a
Boolean value with a numeric ``0``/``1``. Aggregate byte writes and external
deserialization remain review boundaries rather than inferred Boolean
evidence. ``b32``/``b8``/``bchar`` are typedefs, so a retype never changes a
decorated name; the literal spellings are checked by the byte gates.

Run inside ``nix develop .#build``::

    homm2 audit bool-fields
    homm2 audit bool-fields --check
    homm2 audit bool-fields --format json --output build/bool-fields.json
    homm2 audit bool-fields --all --tu SOURCE/HERO
"""

from __future__ import annotations

import argparse
import csv
import ctypes
import functools
import gc
import importlib
import json
import os
import re
import shlex
import subprocess
import sys
from concurrent.futures import ProcessPoolExecutor
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Iterable

import clang.cindex as ci

from homm2.retail_labels.annotated_data import (
    _clang_args,
    configure_libclang,
)
from homm2.clang_options import ClangMode
from homm2.core.paths import REPO


SCHEMA_VERSION = 5
RETAIL_DATABASE = Path("build/clangd/compile_commands.json")
PORTABLE_DATABASE = Path("build/compile_commands.json")
RETAIL_EXCEPTION_MANIFEST = Path("config/reviews/bool_exceptions.tsv")
RETAIL_EXCEPTION_FIELDS = (
    "category", "file", "qualified_name", "write_kind", "detail", "reason",
)
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
REVIEW_BOUNDARY_WRITE_KINDS = {
    "address-escape",
    "mutable-reference-argument",
    "uninitialized-local",
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
        # An uninitialized automatic declaration is not itself a write. Keep
        # it in the report as a definite-assignment review boundary, but do
        # not let it hide a local whose every actual write is Boolean.
        blocking_unknown_writes = {
            write for write in self.unknown_writes
            if not (self.storage_kind == "variable"
                    and write.kind == "uninitialized-local")
        }
        return (self.declared_type in SOURCE_BOOLEAN_TARGETS
                and self.observed_domain == (0, 1)
                and not blocking_unknown_writes)

    @property
    def target_type(self) -> str | None:
        return SOURCE_BOOLEAN_TARGETS.get(self.declared_type)


@dataclass(frozen=True, order=True)
class ReviewedException:
    category: str
    file: str
    qualified_name: str
    write_kind: str
    detail: str
    reason: str

    @property
    def key(self) -> tuple[str, str, str, str, str]:
        return (
            self.category,
            self.file,
            self.qualified_name,
            self.write_kind,
            self.detail,
        )


def _reviewed_exceptions(
    repo: Path,
) -> dict[tuple[str, str, str, str, str], ReviewedException]:
    path = repo / RETAIL_EXCEPTION_MANIFEST
    if not path.is_file():
        return {}
    with path.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream, delimiter="\t")
        if tuple(reader.fieldnames or ()) != RETAIL_EXCEPTION_FIELDS:
            raise RuntimeError(
                f"{RETAIL_EXCEPTION_MANIFEST}: expected columns "
                + ", ".join(RETAIL_EXCEPTION_FIELDS))
        rows = [ReviewedException(**row) for row in reader]
    allowed = {"call", "parse-diagnostic", "return", "write"}
    invalid = [
        row for row in rows
        if (row.category not in allowed or not row.file or not row.detail or not row.reason
            or (row.category in {"call", "return", "write"}
                and (not row.qualified_name or not row.write_kind))
            or (row.category == "parse-diagnostic"
                and (row.qualified_name or row.write_kind)))
    ]
    if invalid:
        raise RuntimeError(f"{RETAIL_EXCEPTION_MANIFEST}: invalid exception row")
    by_key = {row.key: row for row in rows}
    if len(by_key) != len(rows):
        raise RuntimeError(f"{RETAIL_EXCEPTION_MANIFEST}: duplicate exception key")
    return by_key


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


def _evaluated_integer(cursor: ci.Cursor) -> int | None:
    """An integer literal's value from clang itself.

    A literal written inside a macro body has no usable tokens at its
    expansion site, so its spelling cannot be read back from the cursor.
    """
    lib = ci.conf.lib
    if not getattr(_evaluated_integer, "registered", False):
        lib.clang_Cursor_Evaluate.argtypes = [ci.Cursor]
        lib.clang_Cursor_Evaluate.restype = ctypes.c_void_p
        lib.clang_EvalResult_getKind.argtypes = [ctypes.c_void_p]
        lib.clang_EvalResult_getKind.restype = ctypes.c_int
        lib.clang_EvalResult_getAsLongLong.argtypes = [ctypes.c_void_p]
        lib.clang_EvalResult_getAsLongLong.restype = ctypes.c_longlong
        lib.clang_EvalResult_dispose.argtypes = [ctypes.c_void_p]
        lib.clang_EvalResult_dispose.restype = None
        _evaluated_integer.registered = True
    result = lib.clang_Cursor_Evaluate(cursor)
    if not result:
        return None
    try:
        # CXEval_Int == 1
        if lib.clang_EvalResult_getKind(result) != 1:
            return None
        return int(lib.clang_EvalResult_getAsLongLong(result))
    finally:
        lib.clang_EvalResult_dispose(result)


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
        value = _integer_literal(tokens[-1]) if len(tokens) == 1 else _evaluated_integer(cursor)
        return (value,) if value in (0, 1) else None
    if cursor.kind == ci.CursorKind.ENUM_CONSTANT_DECL:
        value = cursor.enum_value
        return (value,) if value in (0, 1) else None
    if cursor.kind == ci.CursorKind.DECL_REF_EXPR and cursor.referenced is not None:
        referenced = cursor.referenced
        if referenced.kind == ci.CursorKind.ENUM_CONSTANT_DECL:
            value = referenced.enum_value
            return (value,) if value in (0, 1) else None
        if referenced.type.spelling in BOOLEAN_TYPES:
            return (0, 1)
    # An unsigned one-bit field can hold nothing but 0 or 1.
    if cursor.kind == ci.CursorKind.MEMBER_REF_EXPR and cursor.referenced is not None \
            and cursor.referenced.kind == ci.CursorKind.FIELD_DECL \
            and cursor.referenced.is_bitfield() \
            and cursor.referenced.get_bitfield_width() == 1 \
            and cursor.referenced.type.get_canonical().kind in (
                ci.TypeKind.UCHAR, ci.TypeKind.USHORT, ci.TypeKind.UINT,
                ci.TypeKind.ULONG, ci.TypeKind.BOOL):
        return (0, 1)
    if cursor.kind in WRAPPER_KINDS and len(children) == 1:
        return _boolean_domain(children[0])
    if cursor.kind == ci.CursorKind.CONDITIONAL_OPERATOR and len(children) == 3:
        left = _boolean_domain(children[1])
        right = _boolean_domain(children[2])
        if left is not None and right is not None:
            return tuple(sorted(set(left) | set(right)))
    if cursor.kind == ci.CursorKind.BINARY_OPERATOR and cursor.spelling == "-" \
            and len(children) == 2:
        left = _boolean_domain(children[0])
        right = _boolean_domain(children[1])
        if left == (1,) and right is not None:
            return (0, 1)
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


def _initializer_leaves(cursor: ci.Cursor) -> list[ci.Cursor]:
    if cursor.kind != ci.CursorKind.INIT_LIST_EXPR:
        return [cursor]
    return [leaf
            for child in cursor.get_children()
            for leaf in _initializer_leaves(child)]


def _has_explicit_initializer(cursor: ci.Cursor) -> bool:
    tokens = _tokens(cursor)
    if "=" in tokens or any(child.kind == ci.CursorKind.INIT_LIST_EXPR
                            for child in cursor.get_children()):
        return True
    try:
        name_index = tokens.index(cursor.spelling)
    except ValueError:
        return False
    return (name_index + 1 < len(tokens)
            and tokens[name_index + 1] in ("(", "{"))


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


def _declaration_text(cursor: ci.Cursor, repo: Path) -> str:
    span = _span(cursor, repo)
    if span is None:
        return ""
    try:
        source = (repo / span.file).read_bytes()
        return source[span.start:span.end].decode("utf-8")
    except (OSError, UnicodeDecodeError):
        return ""


def _storage_type(type_: ci.Type) -> tuple[str, int]:
    """Return the scalar element spelling and array nesting depth."""
    depth = 0
    while type_.kind in (ci.TypeKind.CONSTANTARRAY,
                         ci.TypeKind.INCOMPLETEARRAY,
                         ci.TypeKind.VARIABLEARRAY,
                         ci.TypeKind.DEPENDENTSIZEDARRAY):
        depth += 1
        type_ = type_.get_array_element_type()
    return type_.spelling, depth


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


def _lvalue_references(cursor: ci.Cursor,
                       candidates: dict[str, FieldFacts]) -> list[tuple[str, ci.Cursor]]:
    """Return only the tracked scalar denoted by an lvalue expression.

    Walking the whole left-hand side is incorrect: in ``array[index] = value``
    the scalar ``index`` is read, not written. Scalar declarations and member
    references appear at the lvalue root, modulo transparent AST wrappers.
    """
    usr = _referenced_field(cursor, candidates)
    if usr is not None:
        return [(usr, cursor)]
    children = list(cursor.get_children())
    if cursor.kind == ci.CursorKind.ARRAY_SUBSCRIPT_EXPR and children:
        return _lvalue_references(children[0], candidates)
    if cursor.kind in WRAPPER_KINDS and len(children) == 1:
        return _lvalue_references(children[0], candidates)
    return []


def _record_declaration(type_: ci.Type) -> ci.Cursor | None:
    declaration = type_.get_declaration()
    return declaration if declaration is not None and declaration.kind in RECORD_KINDS else None


def _tracked_fields(cursor: ci.Cursor, repo: Path, unit: str) -> dict[str, FieldFacts]:
    out = {}
    for field_cursor in cursor.walk_preorder():
        storage_kind = STORAGE_DECL_KINDS.get(field_cursor.kind)
        declared_type, _ = _storage_type(field_cursor.type)
        if storage_kind is None or declared_type not in TRACKED_TYPES:
            continue
        declaration = _span(field_cursor, repo)
        type_span = _type_span(field_cursor, repo, declared_type)
        parent = field_cursor.semantic_parent
        if declaration is None or type_span is None or parent is None:
            continue
        # In retail-analysis mode H2_ENUM_STORAGE deliberately expands to its
        # ABI-width integer. The source nevertheless already carries a
        # stronger enum domain and must never be proposed as Boolean storage.
        if "H2_ENUM_" in _declaration_text(field_cursor, repo):
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
        if usr in candidates and _has_explicit_initializer(cursor):
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
                if initializers and _has_explicit_initializer(cursor):
                    initializer = initializers[-1]
                    _, array_depth = _storage_type(cursor.type)
                    values = (_initializer_leaves(initializer)
                              if array_depth else [initializer])
                    for value in values:
                        _mark(candidates, usr,
                              _write(value, "variable-initializer", repo,
                                     _boolean_domain(value)), handled)
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
        refs = _lvalue_references(lhs, candidates)
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
            operand = children[0] if children else cursor
            for usr, reference in _lvalue_references(operand, candidates):
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
                    for usr, reference in _lvalue_references(argument, candidates):
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


def analyze_boolean_call_arguments(
    translation: ci.TranslationUnit,
    repo: Path,
) -> list[dict]:
    """Inventory source-owned arguments passed to recovered Boolean parameters."""
    rows = []
    seen = set()
    for cursor in translation.cursor.walk_preorder():
        if cursor.kind != ci.CursorKind.CALL_EXPR or cursor.referenced is None:
            continue
        arguments = list(cursor.get_arguments())
        parameters = list(cursor.referenced.get_arguments())
        for index, (argument, parameter) in enumerate(zip(arguments, parameters)):
            parameter_type, depth = _storage_type(parameter.type)
            if depth or parameter_type not in BOOLEAN_TYPES:
                continue
            domain = _boolean_domain(argument)
            write = _write(argument, "call-argument", repo, domain)
            if write is None:
                continue
            callee = _qualified_record(cursor.referenced)
            parameter_name = parameter.spelling or f"argument-{index + 1}"
            key = (write.file, write.start, write.end, callee, index)
            if key in seen:
                continue
            seen.add(key)
            rows.append({
                "callee": callee,
                "parameter": parameter_name,
                "parameter_index": index,
                "parameter_type": parameter_type,
                "argument": asdict(write),
            })
    return rows


FUNCTION_DECL_KINDS = {
    ci.CursorKind.FUNCTION_DECL,
    ci.CursorKind.CXX_METHOD,
    ci.CursorKind.CONSTRUCTOR,
}
RESULT_DECL_KINDS = {
    ci.CursorKind.FUNCTION_DECL,
    ci.CursorKind.CXX_METHOD,
}
DISCARDED_CONTEXT_KINDS = {
    ci.CursorKind.COMPOUND_STMT,
    ci.CursorKind.CASE_STMT,
    ci.CursorKind.DEFAULT_STMT,
    ci.CursorKind.LABEL_STMT,
}
NUMERIC_RESULT_USE = "numeric"


def _strip_wrappers(cursor: ci.Cursor) -> ci.Cursor:
    while cursor.kind in WRAPPER_KINDS:
        children = list(cursor.get_children())
        if len(children) != 1:
            break
        cursor = children[0]
    return cursor


def _value_dependency(cursor: ci.Cursor) -> list[str] | None:
    """Name the integer source a whole-program fixpoint may still prove Boolean.

    A value read from a not-yet-Boolean function result or scalar is not a
    proof by itself; the scan resolves it once every write of that source has
    been proven. The dependency is ``[kind, usr]`` with kind ``function`` or
    ``storage``.
    """
    cursor = _strip_wrappers(cursor)
    referenced = cursor.referenced
    if referenced is None:
        return None
    if cursor.kind == ci.CursorKind.CALL_EXPR and referenced.kind in RESULT_DECL_KINDS:
        if referenced.result_type.spelling in SOURCE_BOOLEAN_TARGETS:
            return ["function", referenced.get_usr()]
        return None
    if cursor.kind in REFERENCE_KINDS and referenced.kind in STORAGE_DECL_KINDS:
        spelling, depth = _storage_type(referenced.type)
        if not depth and spelling in SOURCE_BOOLEAN_TARGETS:
            return ["storage", _field_usr(referenced)]
    return None


def _flow(cursor: ci.Cursor, kind: str, repo: Path) -> dict | None:
    """A value flowing into a Boolean contract: its write and open dependency."""
    domain = _boolean_domain(cursor)
    write = _write(cursor, kind, repo, domain)
    if write is None:
        return None
    stripped = _strip_wrappers(cursor)
    referenced = stripped.referenced if stripped.kind == ci.CursorKind.DECL_REF_EXPR else None
    return {
        "write": asdict(write),
        "dependency": None if domain is not None else _value_dependency(cursor),
        # A named enumerator is a stronger domain than a truth value, even
        # when its value happens to be 0 or 1.
        "enumerator": referenced is not None
                      and referenced.kind == ci.CursorKind.ENUM_CONSTANT_DECL,
    }


_COMMENT = re.compile(r"//[^\n]*|/\*.*?\*/", re.S)


def _result_type_span(cursor: ci.Cursor, repo: Path, declared_type: str) -> SourceSpan | None:
    """The one result-type token before a function declarator's name.

    libclang yields no tokens for a declaration whose extent starts in a macro
    (the ``VA(...)`` annotation), so the prefix is read from the source text.
    Parenthesized groups are skipped; a result spelled through an
    ``H2_ENUM_*`` macro already carries a stronger domain.
    """
    start, name = cursor.extent.start, cursor.location
    if start.file is None or name.file is None or str(start.file) != str(name.file):
        return None
    relative = _project_relative(str(name.file), repo)
    if relative is None:
        return None
    try:
        source = (repo / relative).read_bytes()
    except OSError:
        return None
    prefix = source[start.offset:name.offset].decode("utf-8", "replace")
    prefix = _COMMENT.sub(lambda match: " " * len(match.group()), prefix)
    depth = 0
    matches = []
    for token in re.finditer(r"[A-Za-z_][A-Za-z_0-9]*|[()]", prefix):
        spelling = token.group()
        if spelling == "(":
            depth += 1
        elif spelling == ")":
            depth -= 1
        elif depth == 0 and spelling.startswith("H2_ENUM_"):
            return None
        elif depth == 0 and spelling == declared_type:
            matches.append(token.start())
    if len(matches) != 1:
        return None
    offset = start.offset + len(prefix[:matches[0]].encode("utf-8"))
    line = source.count(b"\n", 0, offset) + 1
    column = offset - (source.rfind(b"\n", 0, offset) + 1) + 1
    return SourceSpan(file=relative, line=line, column=column, start=offset,
                      end=offset + len(declared_type))


def _result_use(node: ci.Cursor, parents: list[ci.Cursor]) -> str:
    """Classify how a call's integer result is consumed by its caller."""
    index = len(parents) - 1
    while index >= 0 and parents[index].kind in WRAPPER_KINDS:
        node = parents[index]
        index -= 1
    if index < 0:
        return "discarded"
    parent = parents[index]
    children = list(parent.get_children())
    position = next((i for i, child in enumerate(children) if child == node), -1)
    kind = parent.kind
    if kind in (ci.CursorKind.IF_STMT, ci.CursorKind.WHILE_STMT,
                ci.CursorKind.SWITCH_STMT):
        if position:
            return "discarded"
        return "condition" if kind != ci.CursorKind.SWITCH_STMT else NUMERIC_RESULT_USE
    if kind == ci.CursorKind.DO_STMT:
        return "condition" if position == len(children) - 1 else "discarded"
    if kind == ci.CursorKind.FOR_STMT:
        return "condition" if position != len(children) - 1 else "discarded"
    if kind == ci.CursorKind.UNARY_OPERATOR:
        tokens = _tokens(parent)
        return "condition" if tokens and tokens[0] == "!" else NUMERIC_RESULT_USE
    if kind == ci.CursorKind.BINARY_OPERATOR:
        operator = parent.spelling
        if operator in ("&&", "||"):
            return "condition"
        if operator in ("==", "!=") and len(children) == 2 and position in (0, 1):
            other = children[1 - position]
            return ("comparison" if _boolean_domain(other) is not None
                    or _value_dependency(other) is not None else NUMERIC_RESULT_USE)
        if operator == "=":
            return "assignment" if position == 1 else "store"
        if operator == ",":
            return "discarded" if position == 0 else "value"
        return NUMERIC_RESULT_USE
    if kind == ci.CursorKind.COMPOUND_ASSIGNMENT_OPERATOR and position == 0:
        return "store"
    if kind == ci.CursorKind.CONDITIONAL_OPERATOR:
        return "condition" if position == 0 else "value"
    if kind == ci.CursorKind.RETURN_STMT:
        return "return"
    if kind == ci.CursorKind.VAR_DECL:
        return "initializer"
    if kind == ci.CursorKind.CALL_EXPR:
        return "argument"
    if kind in DISCARDED_CONTEXT_KINDS:
        return "discarded"
    return NUMERIC_RESULT_USE


def _is_callee(node: ci.Cursor, parents: list[ci.Cursor]) -> bool:
    index = len(parents) - 1
    while index >= 0 and parents[index].kind in WRAPPER_KINDS:
        node = parents[index]
        index -= 1
    if index < 0 or parents[index].kind != ci.CursorKind.CALL_EXPR:
        return False
    children = list(parents[index].get_children())
    return bool(children) and children[0] == node


def _function_entry(cursor: ci.Cursor, repo: Path) -> dict | None:
    declaration = _span(cursor, repo)
    if declaration is None:
        return None
    result_type = (cursor.result_type.spelling
                   if cursor.kind in RESULT_DECL_KINDS else "")
    result_span = (_result_type_span(cursor, repo, result_type)
                   if result_type in TRACKED_TYPES else None)
    parameters = []
    for index, parameter in enumerate(cursor.get_arguments()):
        declared_type, depth = _storage_type(parameter.type)
        if depth or declared_type not in TRACKED_TYPES:
            continue
        type_span = _type_span(parameter, repo, declared_type)
        defaults = [child for child in parameter.get_children()
                    if child.kind not in (ci.CursorKind.TYPE_REF,
                                          ci.CursorKind.ANNOTATE_ATTR,
                                          ci.CursorKind.NAMESPACE_REF)]
        default = _flow(defaults[-1], "default-argument", repo) if defaults else None
        parameters.append({
            "index": index,
            "name": parameter.spelling,
            "declared_type": declared_type,
            "enum_domain": "H2_ENUM_" in _declaration_text(parameter, repo),
            "type_spans": [asdict(type_span)] if type_span is not None else [],
            "unspanned": type_span is None,
            "default": default,
            "storage_usr": _field_usr(parameter) if cursor.is_definition() else None,
        })
    return {
        "usr": cursor.get_usr(),
        "qualified_name": _qualified_record(cursor),
        "name": cursor.spelling,
        "result_type": result_type,
        "result_type_spans": [asdict(result_span)] if result_span is not None else [],
        "result_unspanned": result_type in SOURCE_BOOLEAN_TARGETS and result_span is None,
        "declarations": [asdict(declaration)],
        "defined": cursor.is_definition(),
        "virtual": cursor.kind == ci.CursorKind.CXX_METHOD and cursor.is_virtual_method(),
        "variadic": cursor.type.kind == ci.TypeKind.FUNCTIONPROTO
                    and cursor.type.is_function_variadic(),
        "operator": cursor.spelling.startswith("operator"),
        "parameters": parameters,
    }


def analyze_function_contracts(translation: ci.TranslationUnit, repo: Path) -> dict:
    """Collect result and parameter contracts of project functions in one TU.

    The rows are merged across the program by :func:`_function_contracts`:
    every ``return`` of a function returning ``i32``, ``i8`` or plain char,
    every argument passed to such a parameter (or its default), how each call
    result is consumed, and every reference that takes a function's address.
    """
    functions: dict[str, dict] = {}
    returns = []
    arguments = []
    result_uses = []
    address_taken = []
    parameter_uses = []
    parents: list[ci.Cursor] = []
    owners: list[str | None] = []

    def visit(cursor: ci.Cursor) -> None:
        kind = cursor.kind
        owner_pushed = False
        if kind in FUNCTION_DECL_KINDS:
            entry = _function_entry(cursor, repo)
            if entry is not None:
                current = functions.setdefault(entry["usr"], entry)
                if current is not entry:
                    _merge_function_entry(current, entry)
            if cursor.is_definition():
                owners.append(cursor.get_usr() if entry is not None else None)
                owner_pushed = True
        elif kind == ci.CursorKind.RETURN_STMT and owners and owners[-1] is not None:
            children = list(cursor.get_children())
            if children:
                flow = _flow(children[0], "return", repo)
                if flow is not None:
                    returns.append({"function": owners[-1], **flow})
        elif kind == ci.CursorKind.CALL_EXPR and cursor.referenced is not None \
                and cursor.referenced.kind in FUNCTION_DECL_KINDS:
            callee = cursor.referenced
            usr = callee.get_usr()
            supplied = list(cursor.get_arguments())
            location = _span(cursor, repo)
            for index, parameter in enumerate(callee.get_arguments()):
                declared_type, depth = _storage_type(parameter.type)
                if depth or declared_type not in TRACKED_TYPES:
                    continue
                # libclang lists an omitted defaulted argument as an expression
                # without a source extent; the parameter's default carries its proof.
                if index < len(supplied) and supplied[index].extent.start.file is not None:
                    flow = _flow(supplied[index], "call-argument", repo)
                    if flow is None:
                        continue
                elif location is not None:
                    flow = {"write": None, "dependency": None,
                            "call": asdict(location)}
                else:
                    continue
                arguments.append({"function": usr, "index": index, **flow})
            if callee.kind in RESULT_DECL_KINDS \
                    and callee.result_type.spelling in TRACKED_TYPES:
                use = _result_use(cursor, parents)
                write = _write(cursor, "result-use", repo, None)
                if write is not None:
                    result_uses.append({"function": usr, "use": use,
                                        "write": asdict(write)})
        elif kind == ci.CursorKind.DECL_REF_EXPR and cursor.referenced is not None \
                and cursor.referenced.kind == ci.CursorKind.PARM_DECL \
                and _storage_type(cursor.referenced.type) in (
                    (name, 0) for name in SOURCE_BOOLEAN_TARGETS):
            if _result_use(cursor, parents) == NUMERIC_RESULT_USE:
                write = _write(cursor, "numeric-use", repo, None)
                if write is not None:
                    parameter_uses.append({"function": _field_usr(cursor.referenced),
                                           "write": asdict(write)})
        elif kind in (ci.CursorKind.DECL_REF_EXPR, ci.CursorKind.MEMBER_REF_EXPR) \
                and cursor.referenced is not None \
                and cursor.referenced.kind in FUNCTION_DECL_KINDS \
                and not _is_callee(cursor, parents):
            write = _write(cursor, "address-taken", repo, None)
            if write is not None:
                address_taken.append({"function": cursor.referenced.get_usr(),
                                      "write": asdict(write)})
        parents.append(cursor)
        for child in cursor.get_children():
            visit(child)
        parents.pop()
        if owner_pushed:
            owners.pop()

    visit(translation.cursor)
    return {
        "functions": list(functions.values()),
        "returns": returns,
        "arguments": arguments,
        "result_uses": result_uses,
        "address_taken": address_taken,
        "parameter_uses": parameter_uses,
    }


def _merge_function_entry(current: dict, entry: dict) -> None:
    for key in ("declarations", "result_type_spans"):
        current[key] = sorted({tuple(sorted(item.items())): item
                               for item in current[key] + entry[key]}.values(),
                              key=lambda item: (item["file"], item["start"]))
    for key in ("defined", "virtual", "variadic", "operator", "result_unspanned"):
        current[key] = current[key] or entry[key]
    by_index = {item["index"]: item for item in current["parameters"]}
    for parameter in entry["parameters"]:
        known = by_index.get(parameter["index"])
        if known is None:
            current["parameters"].append(parameter)
            by_index[parameter["index"]] = parameter
            continue
        known["type_spans"] = sorted(
            {tuple(sorted(item.items())): item
             for item in known["type_spans"] + parameter["type_spans"]}.values(),
            key=lambda item: (item["file"], item["start"]))
        known["enum_domain"] = known["enum_domain"] or parameter["enum_domain"]
        known["unspanned"] = known["unspanned"] or parameter["unspanned"]
        known["name"] = known["name"] or parameter["name"]
        known["default"] = known["default"] or parameter["default"]
        known["storage_usr"] = known["storage_usr"] or parameter["storage_usr"]
        if known["declared_type"] != parameter["declared_type"]:
            known["declared_type"] = "<inconsistent>"
    current["parameters"].sort(key=lambda item: item["index"])


def _function_contracts(
    parsed: list[dict],
    storage: list[FieldFacts],
    exceptions: dict[tuple[str, str, str, str, str], ReviewedException],
    used_exceptions: set[tuple[str, str, str, str, str]],
) -> dict:
    """Prove Boolean function results and parameters across the whole program.

    A result is Boolean when every ``return`` yields a value in ``{0, 1}``, a
    Boolean-typed value, or a value whose source this fixpoint also proves; a
    parameter when every argument (or default) and every write in its body is.
    Both need the observed domain to be exactly ``{0, 1}``. The fixpoint is
    greatest: mutually dependent contracts stand or fall together.
    """
    functions: dict[str, dict] = {}
    for batch in parsed:
        for entry in batch["functions"]:
            current = functions.get(entry["usr"])
            if current is None:
                functions[entry["usr"]] = json.loads(json.dumps(entry))
            else:
                _merge_function_entry(current, entry)

    def unique(rows: Iterable[dict], key) -> list[dict]:
        return list({key(item): item for item in rows}.values())

    def write_key(item: dict) -> tuple:
        write = item.get("write") or item.get("call") or {}
        return (item["function"], item.get("index"), write.get("file"),
                write.get("start"), write.get("end"))

    returns = unique((row for batch in parsed for row in batch["returns"]), write_key)
    arguments = unique((row for batch in parsed for row in batch["arguments"]), write_key)
    result_uses = unique((row for batch in parsed for row in batch["result_uses"]),
                         write_key)
    address_taken = unique((row for batch in parsed for row in batch["address_taken"]),
                           write_key)
    # Keyed by the defining PARM_DECL's identity (stored in "function").
    parameter_uses: dict[str, list[dict]] = {}
    for row in unique((row for batch in parsed for row in batch["parameter_uses"]),
                      write_key):
        parameter_uses.setdefault(row["function"], []).append(row)

    returns_by_function: dict[str, list[dict]] = {}
    for row in returns:
        returns_by_function.setdefault(row["function"], []).append(row)
    arguments_by_parameter: dict[tuple[str, int], list[dict]] = {}
    for row in arguments:
        arguments_by_parameter.setdefault((row["function"], row["index"]), []).append(row)
    uses_by_function: dict[str, list[dict]] = {}
    for row in result_uses:
        uses_by_function.setdefault(row["function"], []).append(row)
    taken: dict[str, list[dict]] = {}
    for row in address_taken:
        taken.setdefault(row["function"], []).append(row)
    storage_by_usr = {item.usr: item for item in storage}
    eligible_storage = {item.usr for item in storage if item.eligible}

    # Parameters own their incoming domain: their storage rows carry only the
    # writes made inside the body.
    def body_writes(parameter: dict) -> tuple[list[Write], list[Write]]:
        facts = storage_by_usr.get(parameter.get("storage_usr") or "")
        if facts is None:
            return [], []
        unknown = [write for write in facts.unknown_writes
                   if write.kind != "incoming-parameter"]
        return sorted(facts.writes), sorted(unknown)

    result_reasons: dict[str, list[str]] = {}
    for usr, function in functions.items():
        if function["result_type"] not in SOURCE_BOOLEAN_TARGETS:
            continue
        reasons = []
        if not function["defined"]:
            reasons.append("no-definition")
        if function["virtual"]:
            reasons.append("virtual")
        if function["operator"]:
            reasons.append("operator")
        if function["result_unspanned"] or not function["result_type_spans"]:
            reasons.append("result-type-not-spelled")
        rows = returns_by_function.get(usr, [])
        if not rows:
            reasons.append("no-return")
        if any(row["write"]["domain"] is None and row["dependency"] is None
               for row in rows):
            reasons.append("non-boolean-return")
        if any(row.get("enumerator") for row in rows):
            reasons.append("enumerator-return")
        if any(row["use"] == NUMERIC_RESULT_USE for row in uses_by_function.get(usr, [])):
            reasons.append("numeric-caller-use")
        result_reasons[usr] = reasons

    parameter_reasons: dict[tuple[str, int], list[str]] = {}
    for usr, function in functions.items():
        for parameter in function["parameters"]:
            if parameter["declared_type"] not in SOURCE_BOOLEAN_TARGETS:
                continue
            key = (usr, parameter["index"])
            reasons = []
            if parameter["enum_domain"]:
                reasons.append("enum-domain")
            if not function["defined"]:
                reasons.append("no-definition")
            if function["virtual"]:
                reasons.append("virtual")
            if function["operator"] or function["variadic"]:
                reasons.append("operator-or-variadic")
            if usr in taken:
                reasons.append("address-taken")
            if parameter["unspanned"] or not parameter["type_spans"]:
                reasons.append("parameter-type-not-spelled")
            rows = arguments_by_parameter.get(key, [])
            if not rows:
                reasons.append("no-call")
            for row in rows:
                flow = row if row["write"] is not None else parameter["default"]
                if flow is None:
                    reasons.append("missing-default")
                elif flow["write"]["domain"] is None and flow["dependency"] is None:
                    reasons.append("non-boolean-argument")
                elif flow.get("enumerator"):
                    reasons.append("enumerator-argument")
            _, unknown = body_writes(parameter)
            if unknown:
                reasons.append("non-boolean-body-write")
            if parameter_uses.get(parameter.get("storage_usr") or ""):
                reasons.append("numeric-body-use")
            parameter_reasons[key] = sorted(set(reasons))

    storage_parameter = {
        parameter["storage_usr"]: (usr, parameter["index"])
        for usr, function in functions.items()
        for parameter in function["parameters"]
        if parameter.get("storage_usr")
    }

    def dependency_holds(dependency: list[str] | None, results: set[str],
                         parameters: set[tuple[str, int]]) -> bool:
        if dependency is None:
            return True
        kind, usr = dependency
        if kind == "function":
            return usr in results
        if usr in storage_parameter:
            return storage_parameter[usr] in parameters
        return usr in eligible_storage

    def domain(flows: Iterable[dict | None]) -> set[int]:
        values: set[int] = set()
        for flow in flows:
            if flow is None:
                continue
            if flow["dependency"] is not None:
                values.update((0, 1))
            else:
                values.update(flow["write"]["domain"] or ())
        return values

    results = {usr for usr, reasons in result_reasons.items() if not reasons}
    parameters = {key for key, reasons in parameter_reasons.items() if not reasons}
    changed = True
    while changed:
        changed = False
        for usr in sorted(results):
            rows = returns_by_function.get(usr, [])
            if not all(dependency_holds(row["dependency"], results, parameters)
                       for row in rows) or domain(rows) != {0, 1}:
                results.discard(usr)
                result_reasons[usr].append(
                    "insufficient-observed-domain" if domain(rows) != {0, 1}
                    else "unproven-dependency")
                changed = True
        for key in sorted(parameters):
            function = functions[key[0]]
            parameter = next(item for item in function["parameters"]
                             if item["index"] == key[1])
            flows = [row if row["write"] is not None else parameter["default"]
                     for row in arguments_by_parameter.get(key, [])]
            writes, _ = body_writes(parameter)
            values = domain(flows) | {value for write in writes
                                      for value in (write.domain or ())}
            if not all(dependency_holds(flow["dependency"], results, parameters)
                       for flow in flows) or values != {0, 1}:
                parameters.discard(key)
                parameter_reasons[key].append(
                    "insufficient-observed-domain" if values != {0, 1}
                    else "unproven-dependency")
                changed = True

    def return_row(row: dict) -> dict:
        return {key: value for key, value in row.items() if key != "function"}

    def emit_result(usr: str) -> dict:
        function = functions[usr]
        uses = uses_by_function.get(usr, [])
        return {
            "usr": usr,
            "qualified_name": function["qualified_name"],
            "declared_type": function["result_type"],
            "target_type": SOURCE_BOOLEAN_TARGETS.get(function["result_type"]),
            "declarations": function["declarations"],
            "result_type_spans": function["result_type_spans"],
            "returns": [return_row(row) for row in sorted(
                returns_by_function.get(usr, []),
                key=lambda row: (row["write"]["file"], row["write"]["start"]))],
            "result_uses": {use: sum(row["use"] == use for row in uses)
                            for use in sorted({row["use"] for row in uses})},
            "numeric_result_uses": [row["write"] for row in uses
                                    if row["use"] == NUMERIC_RESULT_USE],
            "reasons": sorted(set(result_reasons.get(usr, []))),
        }

    def emit_parameter(key: tuple[str, int]) -> dict:
        function = functions[key[0]]
        parameter = next(item for item in function["parameters"]
                         if item["index"] == key[1])
        writes, unknown = body_writes(parameter)
        return {
            "usr": key[0],
            "function": function["qualified_name"],
            "qualified_name": f"{function['qualified_name']}::"
                              f"{parameter['name'] or 'argument-' + str(key[1] + 1)}",
            "index": key[1],
            "name": parameter["name"],
            "declared_type": parameter["declared_type"],
            "target_type": SOURCE_BOOLEAN_TARGETS.get(parameter["declared_type"]),
            "declarations": function["declarations"],
            "type_spans": parameter["type_spans"],
            "arguments": [return_row(row) for row in arguments_by_parameter.get(key, [])],
            "default": parameter["default"],
            "body_writes": [asdict(write) for write in writes],
            "unknown_body_writes": [asdict(write) for write in unknown],
            "numeric_body_uses": [row["write"] for row in
                                  parameter_uses.get(parameter.get("storage_usr") or "", [])],
            "reasons": sorted(set(parameter_reasons.get(key, []))),
        }

    # Already Boolean results must keep returning provably Boolean values, and
    # their 0/1 literals read as false/true.
    numeric_returns = []
    unproven_returns = []
    accepted_unproven_returns = []
    numeric_result_uses = []
    for usr, function in sorted(functions.items(),
                                key=lambda item: item[1]["qualified_name"]):
        if function["result_type"] not in BOOLEAN_TYPES:
            continue
        for row in sorted(returns_by_function.get(usr, []),
                          key=lambda row: (row["write"]["file"], row["write"]["start"])):
            write = row["write"]
            item = {"qualified_name": function["qualified_name"],
                    "declared_type": function["result_type"], "write": write}
            if write["replacement"] is not None:
                numeric_returns.append(item)
            elif write["domain"] is None:
                key = ("return", write["file"], function["qualified_name"],
                       write["kind"], write["expression"])
                exception = exceptions.get(key)
                if exception is None:
                    unproven_returns.append(item)
                else:
                    used_exceptions.add(key)
                    accepted_unproven_returns.append({**item, "reason": exception.reason})
        numeric_result_uses.extend(
            {"qualified_name": function["qualified_name"], "write": row["write"]}
            for row in uses_by_function.get(usr, [])
            if row["use"] == NUMERIC_RESULT_USE)

    numeric_defaults = [
        {"qualified_name": f"{function['qualified_name']}::{parameter['name']}",
         "declared_type": parameter["declared_type"],
         "write": parameter["default"]["write"]}
        for function in sorted(functions.values(), key=lambda item: item["qualified_name"])
        for parameter in function["parameters"]
        if parameter["declared_type"] in BOOLEAN_TYPES and parameter["default"]
        and parameter["default"]["write"]["replacement"] is not None
    ]

    return {
        "boolean_numeric_default_arguments": numeric_defaults,
        "result_candidates": [emit_result(usr) for usr in sorted(
            results, key=lambda usr: functions[usr]["qualified_name"])],
        "rejected_results": [emit_result(usr) for usr in sorted(
            set(result_reasons) - results,
            key=lambda usr: functions[usr]["qualified_name"])],
        "parameter_candidates": [emit_parameter(key) for key in sorted(
            parameters, key=lambda key: (functions[key[0]]["qualified_name"], key[1]))],
        "rejected_parameters": [emit_parameter(key) for key in sorted(
            set(parameter_reasons) - parameters,
            key=lambda key: (functions[key[0]]["qualified_name"], key[1]))],
        "boolean_numeric_returns": numeric_returns,
        "boolean_unproven_returns": unproven_returns,
        "accepted_boolean_unproven_returns": accepted_unproven_returns,
        "boolean_numeric_result_uses": numeric_result_uses,
    }


def _boolean_operand(cursor: ci.Cursor) -> bool:
    cursor = _strip_wrappers(cursor)
    if cursor.kind in (ci.CursorKind.INTEGER_LITERAL, ci.CursorKind.CXX_BOOL_LITERAL_EXPR):
        return False
    if cursor.type.spelling in BOOLEAN_TYPES or cursor.type.kind == ci.TypeKind.BOOL:
        return True
    referenced = cursor.referenced
    return (cursor.kind in REFERENCE_KINDS and referenced is not None
            and referenced.kind in STORAGE_DECL_KINDS
            and _storage_type(referenced.type)[0] in BOOLEAN_TYPES)


def analyze_boolean_comparisons(translation: ci.TranslationUnit, repo: Path) -> list[dict]:
    """Inventory ``==``/``!=`` comparisons of a Boolean value with a 0/1 literal."""
    rows = []
    for cursor in translation.cursor.walk_preorder():
        if cursor.kind != ci.CursorKind.BINARY_OPERATOR or cursor.spelling not in ("==", "!="):
            continue
        children = list(cursor.get_children())
        if len(children) != 2:
            continue
        for operand, other in ((children[0], children[1]), (children[1], children[0])):
            if not _boolean_operand(operand):
                continue
            literal = _strip_wrappers(other)
            if literal.kind != ci.CursorKind.INTEGER_LITERAL:
                continue
            write = _write(literal, "boolean-comparison", repo, _boolean_domain(literal))
            if write is None or write.replacement is None:
                continue
            rows.append({"expression": _expression(cursor), "write": asdict(write)})
    return rows


def _command_arguments(entry: dict) -> list[str]:
    arguments = entry.get("arguments")
    if arguments:
        return list(arguments)
    command = entry.get("command")
    if command:
        return shlex.split(command)
    raise RuntimeError(f"compilation database entry has no command: {entry.get('file', '<unknown>')}")


def _parse_driver_includes(stderr: str) -> tuple[str, ...]:
    collecting = False
    paths = []
    for raw in stderr.splitlines():
        line = raw.strip()
        if line == "#include <...> search starts here:":
            collecting = True
            continue
        if collecting and line == "End of search list.":
            break
        if collecting and line:
            suffix = " (framework directory)"
            if line.endswith(suffix):
                line = line[:-len(suffix)]
            paths.append(line)
    return tuple(paths)


@functools.lru_cache(maxsize=None)
def _compiler_system_includes(compiler: str,
                              target_options: tuple[str, ...]) -> tuple[str, ...]:
    result = subprocess.run(
        [compiler, *target_options, "-E", "-x", "c++", "-", "-v"],
        input="",
        text=True,
        capture_output=True,
        check=False,
    )
    includes = _parse_driver_includes(result.stderr)
    if result.returncode or not includes:
        detail = result.stderr.strip().splitlines()
        tail = detail[-1] if detail else f"exit status {result.returncode}"
        raise RuntimeError(f"could not query native compiler include paths: {tail}")
    return includes


def _portable_clang_args(repo: Path, entry: dict) -> list[str]:
    command = _command_arguments(entry)
    compiler = command[0]
    directory = Path(entry.get("directory", repo))
    args = ["-x", "c++", "-std=c++20", "-ferror-limit=0"]
    target_options = tuple(
        option for option in command[1:]
        if option in ("-m32", "-m64") or option.startswith("--target=")
    )

    path_options = {"-I", "-isystem", "-iquote", "-idirafter", "-include", "-imacros"}
    index = 1
    while index < len(command):
        option = command[index]
        if option in path_options:
            if index + 1 >= len(command):
                raise RuntimeError(f"missing argument after {option} in compilation database")
            value = Path(command[index + 1])
            args.extend((option, str(value if value.is_absolute() else directory / value)))
            index += 2
            continue
        if option.startswith("-I") and option != "-I":
            value = Path(option[2:])
            args.append("-I" + str(value if value.is_absolute() else directory / value))
        elif option.startswith(("-D", "-U", "--sysroot=", "--target=")):
            args.append(option)
        elif option in ("-m32", "-m64", "-pthread", "-fms-extensions"):
            args.append(option)
        index += 1

    for path in _compiler_system_includes(compiler, target_options):
        args.extend(("-isystem", path))
    args.extend(("-I", str(repo / "include"), "-I", str(repo)))
    return args


def parse_translation_unit(index: ci.Index, source: Path, clang_args: list[str],
                           repo: Path) -> dict:
    """Parse one source and return its storage, argument and contract rows."""
    source = source.resolve()
    translation = index.parse(str(source), args=clang_args)
    diagnostics = []
    for diagnostic in translation.diagnostics:
        if diagnostic.severity < ci.Diagnostic.Error or diagnostic.location.file is None:
            continue
        relative = _project_relative(str(diagnostic.location.file), repo)
        if relative is not None:
            diagnostics.append({
                "file": relative,
                "line": diagnostic.location.line,
                "column": diagnostic.location.column,
                "translation_unit": source.relative_to(repo.resolve()).as_posix(),
                "message": diagnostic.spelling,
            })
    rows = []
    for facts in analyze_translation_unit(translation, source, repo):
        row = asdict(facts)
        row["declarations"] = [asdict(item) for item in sorted(facts.declarations)]
        row["type_spans"] = [asdict(item) for item in sorted(facts.type_spans)]
        row["writes"] = [asdict(item) for item in sorted(facts.writes)]
        row["unknown_writes"] = [asdict(item) for item in sorted(facts.unknown_writes)]
        row["read_locations"] = sorted(facts.read_locations)
        row["translation_units"] = sorted(facts.translation_units)
        rows.append(row)
    parsed = {
        "rows": rows,
        "call_arguments": analyze_boolean_call_arguments(translation, repo),
        "comparisons": analyze_boolean_comparisons(translation, repo),
        "diagnostics": diagnostics,
        **analyze_function_contracts(translation, repo),
    }
    del translation
    gc.collect()
    return parsed


def _parse_batch(arguments: tuple[Path, list[dict], bool]) -> dict:
    repo, entries, portable = arguments
    configure_libclang()
    index = ci.Index.create()
    out: dict[str, list] = {}
    for entry in entries:
        source = (Path(entry["directory"]) / entry["file"]).resolve()
        clang_args = (_portable_clang_args(repo, entry) if portable else
                      _clang_args(repo, source, mode=ClangMode.RETAIL_ANALYSIS))
        for key, values in parse_translation_unit(index, source, clang_args, repo).items():
            out.setdefault(key, []).extend(values)
    return out


def _entries(
    repo: Path, filters: Iterable[str] = (), *, portable: bool = False
) -> list[dict]:
    relative_database = PORTABLE_DATABASE if portable else RETAIL_DATABASE
    database = repo / relative_database
    if not database.is_file():
        instruction = (
            "configure CMake with `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`"
            if portable
            else "run `homm2 clangd` inside `nix develop .#build`"
        )
        raise RuntimeError(f"{relative_database} not found; {instruction}")
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
        try:
            source.relative_to(repo.resolve() / "src")
        except ValueError:
            # Portable branches may compile bundled libraries and test tools
            # through the same CMake database. They are not reconstructed game
            # storage and do not belong to this source-tree audit.
            continue
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


def _type_spans_requiring_split(fields: list[FieldFacts]) -> set[SourceSpan]:
    owners: dict[SourceSpan, list[FieldFacts]] = {}
    for item in fields:
        for span in item.type_spans:
            owners.setdefault(span, []).append(item)
    return {
        span for span, items in owners.items()
        if len({item.usr for item in items}) > 1
        and not all(item.eligible and item.target_type == items[0].target_type
                    for item in items)
    }


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
        # `homm2 audit` dispatches tools through runpy with run_name="__main__".
        # ProcessPool cannot pickle that transient module identity, so always
        # hand it the function from the canonically importable package module.
        worker = importlib.import_module("homm2.audit.bool_fields")._parse_batch
        with ProcessPoolExecutor(max_workers=min(worker_count, len(batches))) as pool:
            parsed = list(pool.map(worker, batches))
    return build_report(repo, parsed, translation_units=len(entries),
                        filters=filters, portable=portable)


def build_report(repo: Path, parsed: list[dict], *, translation_units: int,
                 filters: Iterable[str] = (), portable: bool = False) -> dict:
    """Merge parsed translation-unit rows into the whole-program report."""
    filters = tuple(filters)
    parsed = [{key: batch.get(key, []) for key in (
        "rows", "call_arguments", "comparisons", "diagnostics", "functions", "returns",
        "arguments", "result_uses", "address_taken", "parameter_uses")}
        for batch in parsed]
    raw = [row for batch in parsed for row in batch["rows"]]
    call_arguments = sorted({
        (
            item["argument"]["file"], item["argument"]["start"],
            item["argument"]["end"], item["callee"], item["parameter_index"],
        ): item
        for batch in parsed for item in batch["call_arguments"]
    }.values(), key=lambda item: (
        item["argument"]["file"], item["argument"]["line"],
        item["argument"]["column"], item["callee"], item["parameter_index"]))
    diagnostics = sorted({
        (item["file"], item["line"], item["column"],
         item["translation_unit"], item["message"]): item
        for batch in parsed for item in batch["diagnostics"]
    }.values(), key=lambda item: (
        item["file"], item["line"], item["column"],
        item["translation_unit"], item["message"]))
    exceptions = {} if portable else _reviewed_exceptions(repo)
    used_exceptions: set[tuple[str, str, str, str, str]] = set()
    accepted_parse_diagnostics = []
    unexpected_diagnostics = []
    for item in diagnostics:
        key = ("parse-diagnostic", item["file"], "", "", item["message"])
        exception = exceptions.get(key)
        if exception is None:
            unexpected_diagnostics.append(item)
            continue
        used_exceptions.add(key)
        accepted_parse_diagnostics.append({**item, "reason": exception.reason})
    fields = _merge(raw)
    split_type_spans = _type_spans_requiring_split(fields)
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
            "requires_declaration_split": any(
                span in split_type_spans for span in item.type_spans),
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
    def emit_unknown(item: FieldFacts, write: Write) -> dict:
        return {
            "qualified_name": f"{item.record}::{item.name}",
            "storage_kind": item.storage_kind,
            "declarations": [asdict(value) for value in sorted(item.declarations)],
            "write": asdict(write),
        }

    review_boundaries = [
        emit_unknown(item, write)
        for item in boolean_storage
        for write in sorted(item.unknown_writes)
        if write.kind in REVIEW_BOUNDARY_WRITE_KINDS
    ]
    all_unproven_boolean_writes = [
        emit_unknown(item, write)
        for item in boolean_storage
        for write in sorted(item.unknown_writes)
        if (write.kind != "incoming-parameter"
            and write.kind not in REVIEW_BOUNDARY_WRITE_KINDS)
    ]
    accepted_unproven_boolean_writes = []
    unproven_boolean_writes = []
    for item in all_unproven_boolean_writes:
        write = item["write"]
        key = (
            "write", write["file"], item["qualified_name"],
            write["kind"], write["expression"],
        )
        exception = exceptions.get(key)
        if exception is None:
            unproven_boolean_writes.append(item)
            continue
        used_exceptions.add(key)
        accepted_unproven_boolean_writes.append({**item, "reason": exception.reason})
    numeric_call_arguments = [
        item for item in call_arguments if item["argument"]["replacement"] is not None
    ]
    accepted_unproven_call_arguments = []
    unproven_call_arguments = []
    for item in call_arguments:
        argument = item["argument"]
        if argument["domain"] is not None:
            continue
        qualified_name = f"{item['callee']}::{item['parameter']}"
        key = (
            "call", argument["file"], qualified_name,
            argument["kind"], argument["expression"],
        )
        exception = exceptions.get(key)
        if exception is None:
            unproven_call_arguments.append(item)
            continue
        used_exceptions.add(key)
        accepted_unproven_call_arguments.append({**item, "reason": exception.reason})
    contracts = _function_contracts(parsed, fields, exceptions, used_exceptions)
    numeric_comparisons = sorted({
        (item["write"]["file"], item["write"]["start"]): item
        for batch in parsed for item in batch["comparisons"]
    }.values(), key=lambda item: (item["write"]["file"], item["write"]["start"]))
    unused_exceptions = []
    if not filters and not portable:
        unused_exceptions = [
            asdict(exceptions[key]) for key in sorted(set(exceptions) - used_exceptions)
        ]
    boolean_parameters = [emit(item) for item in boolean_storage
                          if item.storage_kind == "parameter"]
    return {
        "schema_version": SCHEMA_VERSION,
        "whole_program": not filters,
        "filters": list(filters),
        "translation_units": translation_units,
        "parse_diagnostics": unexpected_diagnostics,
        "accepted_parse_diagnostics": accepted_parse_diagnostics,
        "parse_clean": not unexpected_diagnostics,
        "unused_retail_exceptions": unused_exceptions,
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
        "accepted_boolean_unproven_writes": accepted_unproven_boolean_writes,
        "boolean_call_arguments": call_arguments,
        "boolean_numeric_call_arguments": numeric_call_arguments,
        "boolean_unproven_call_arguments": unproven_call_arguments,
        "accepted_boolean_unproven_call_arguments": accepted_unproven_call_arguments,
        "boolean_review_boundaries": review_boundaries,
        "boolean_parameters": boolean_parameters,
        "b32_numeric_literal_writes": numeric_literal_writes,
        "b32_unproven_writes": unproven_boolean_writes,
        "boolean_numeric_comparisons": numeric_comparisons,
        **contracts,
    }


def check_failures(report: dict) -> list[str]:
    """Name every actionable kind of remaining Boolean cleanup in a report."""
    kinds = {
        "parse_diagnostics": "parse diagnostics",
        "unused_retail_exceptions": "stale reviewed exceptions",
        "candidates": "integer storage with a proven Boolean domain",
        "b32_numeric_literal_writes": "numeric 0/1 writes to Boolean storage",
        "b32_unproven_writes": "unproven writes to Boolean storage",
        "boolean_numeric_call_arguments": "numeric 0/1 Boolean arguments",
        "boolean_unproven_call_arguments": "unproven Boolean arguments",
        "boolean_numeric_default_arguments": "numeric 0/1 Boolean default arguments",
        "boolean_numeric_comparisons": "Boolean values compared with numeric 0/1",
        "result_candidates": "integer results whose every return is Boolean",
        "parameter_candidates": "integer parameters whose every argument is Boolean",
        "boolean_numeric_returns": "numeric 0/1 returns from Boolean functions",
        "boolean_unproven_returns": "unproven returns from Boolean functions",
    }
    return [f"{len(report[key])} {label}" for key, label in kinds.items()
            if report.get(key)]


def _text(report: dict, include_rejected: bool) -> str:
    scope = "" if report.get("whole_program", True) else "partial "
    lines = [
        f"[{scope}bool-fields] {report['translation_units']} translation units, "
        f"{report.get('source_integer_storage', report['i32_fields'])} source-integer storage objects, "
        f"{report.get('eligible_storage', report['eligible_fields'])} Boolean candidates, "
        f"{report.get('rejected_storage', report['rejected_fields'])} rejected; "
        f"{len(report.get('boolean_numeric_literal_writes', report['b32_numeric_literal_writes']))} "
        f"numeric and {len(report.get('boolean_unproven_writes', report['b32_unproven_writes']))} "
        f"unproven writes to Boolean storage; "
        f"{len(report.get('boolean_review_boundaries', []))} review boundaries; "
        f"{len(report.get('accepted_boolean_unproven_writes', []))} reviewed retail writes; "
        f"{len(report.get('boolean_numeric_call_arguments', []))} numeric and "
        f"{len(report.get('boolean_unproven_call_arguments', []))} unproven Boolean arguments",
        f"[bool-contracts] {len(report.get('result_candidates', []))} Boolean result "
        f"candidates, {len(report.get('rejected_results', []))} integer results kept; "
        f"{len(report.get('parameter_candidates', []))} Boolean parameter candidates, "
        f"{len(report.get('rejected_parameters', []))} integer parameters kept; "
        f"{len(report.get('boolean_numeric_returns', []))} numeric and "
        f"{len(report.get('boolean_unproven_returns', []))} unproven Boolean returns; "
        f"{len(report.get('accepted_boolean_unproven_returns', []))} reviewed retail returns; "
        f"{len(report.get('boolean_numeric_result_uses', []))} numeric uses of Boolean results; "
        f"{len(report.get('boolean_numeric_comparisons', []))} numeric Boolean comparisons",
    ]
    for item in report.get("parse_diagnostics", []):
        lines.append(
            f"PARSE {item['file']}:{item['line']}:{item['column']}: "
            f"{item['message']}"
        )
    for item in report.get("unused_retail_exceptions", []):
        lines.append(
            f"STALE-EXCEPTION {item['category']} {item['file']}: {item['detail']}"
        )
    for item in report["candidates"]:
        location = item["declarations"][0]
        lines.append(
            f"CANDIDATE {location['file']}:{location['line']} "
            f"{item.get('storage_kind', 'field')} {item['qualified_name']} "
            f"{item.get('declared_type', 'i32')}->{item.get('target_type', 'b32')} "
            f"({len(item['writes'])} writes, "
            f"{item['read_count']} reads)"
            + (" [split declaration]" if item.get("requires_declaration_split") else ""))
        for write in item["writes"]:
            lines.append(
                f"  {write['file']}:{write['line']} {write['kind']}: "
                f"{write['expression']} -> {write['domain']}")
    for item in report.get("result_candidates", []):
        location = item["result_type_spans"][0]
        lines.append(
            f"RESULT-CANDIDATE {location['file']}:{location['line']} "
            f"{item['qualified_name']} {item['declared_type']}->{item['target_type']} "
            f"({len(item['returns'])} returns, "
            f"{len(item['result_type_spans'])} declarations)")
        for row in item["returns"]:
            write = row["write"]
            proof = (f"via {row['dependency'][0]}" if row["dependency"]
                     else str(write["domain"]))
            lines.append(f"  {write['file']}:{write['line']} return "
                         f"{write['expression']} -> {proof}")
    for item in report.get("parameter_candidates", []):
        location = item["type_spans"][0]
        lines.append(
            f"PARAMETER-CANDIDATE {location['file']}:{location['line']} "
            f"{item['qualified_name']} {item['declared_type']}->{item['target_type']} "
            f"({len(item['arguments'])} call sites, "
            f"{len(item['type_spans'])} declarations)")
    if include_rejected:
        for item in report.get("rejected_results", []):
            location = item["declarations"][0]
            lines.append(
                f"RESULT-KEPT {location['file']}:{location['line']} "
                f"{item['qualified_name']}: {', '.join(item['reasons'])}")
        for item in report.get("rejected_parameters", []):
            location = item["declarations"][0]
            lines.append(
                f"PARAMETER-KEPT {location['file']}:{location['line']} "
                f"{item['qualified_name']}: {', '.join(item['reasons'])}")
        for key, label in (("boolean_numeric_default_arguments", "BOOL-DEFAULT-LITERAL"),
                           ("boolean_numeric_returns", "BOOL-RETURN-LITERAL"),
                           ("boolean_unproven_returns", "BOOL-RETURN-UNPROVEN")):
            for item in report.get(key, []):
                write = item["write"]
                lines.append(
                    f"{label} {write['file']}:{write['line']} "
                    f"{item['qualified_name']}: {write['expression']}"
                    + (f" -> {write['replacement']}" if write["replacement"] else ""))
        for item in report.get("boolean_numeric_comparisons", []):
            write = item["write"]
            lines.append(f"BOOL-COMPARE-LITERAL {write['file']}:{write['line']} "
                         f"{item['expression']}: {write['expression']} -> "
                         f"{write['replacement']}")
        for item in report.get("boolean_numeric_result_uses", []):
            write = item["write"]
            lines.append(f"BOOL-RESULT-NUMERIC {write['file']}:{write['line']} "
                         f"{item['qualified_name']}: {write['expression']}")
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
        for item in report.get("boolean_review_boundaries", []):
            write = item["write"]
            lines.append(
                f"BOOL-BOUNDARY {write['file']}:{write['line']} "
                f"{item['qualified_name']}: {write['kind']}: "
                f"{write['expression']}")
        for item in report.get("boolean_numeric_call_arguments", []):
            argument = item["argument"]
            lines.append(
                f"BOOL-ARG-LITERAL {argument['file']}:{argument['line']} "
                f"{item['callee']}::{item['parameter']}: "
                f"{argument['expression']} -> {argument['replacement']}")
        for item in report.get("boolean_unproven_call_arguments", []):
            argument = item["argument"]
            lines.append(
                f"BOOL-ARG-UNPROVEN {argument['file']}:{argument['line']} "
                f"{item['callee']}::{item['parameter']}: {argument['expression']}")
    return "\n".join(lines) + "\n"


from homm2.core.usage import logged


@logged
def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--format", choices=("text", "json"), default="text")
    parser.add_argument("--output", type=Path, help="write the report instead of stdout")
    parser.add_argument(
        "--all", action="store_true",
        help="include rejected storage and Boolean write/argument details in text")
    parser.add_argument(
        "--check", action="store_true",
        help=("fail when an integer Boolean storage, result or parameter candidate, "
              "a numeric 0/1 write, argument or return, an unproven Boolean value, "
              "a parse diagnostic, or a stale retail exception remains"))
    parser.add_argument(
        "--tu", action="append", default=[],
        help="limit evidence to matching source paths (partial report; not whole-program proof)")
    parser.add_argument(
        "--portable", action="store_true",
        help="parse a native C++20 compilation database instead of the retail VC6 model")
    parser.add_argument("-j", "--jobs", type=int, default=0)
    args = parser.parse_args(argv)
    if args.check and args.tu:
        print("homm2 audit bool-fields: --check requires the full compilation database; "
              "remove --tu", file=sys.stderr)
        return 1
    try:
        report = scan(jobs=args.jobs, filters=args.tu, portable=args.portable)
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
    if not args.check:
        return 0
    failures = check_failures(report)
    if failures:
        print("homm2 audit bool-fields --check: " + "; ".join(failures) + ".\n"
              "Retype each RESULT/PARAMETER/storage CANDIDATE (i32->b32, i8->b8, "
              "char->bchar) at every listed declaration and spell its 0/1 literals "
              "false/true; `--all` lists literal and unproven sites. A retail-proven "
              "truth value that is not 0/1 belongs in "
              f"{RETAIL_EXCEPTION_MANIFEST} with its reason.", file=sys.stderr)
    return int(bool(failures))


if __name__ == "__main__":
    raise SystemExit(main())
