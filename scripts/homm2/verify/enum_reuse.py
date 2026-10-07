"""homm2.verify.enum_reuse - evaluated enum and constant-reuse census.

Equal integer values are review leads, never proof that two domains are the
same.  The census covers every named integer constant of both programs (the
game and the scenario editor) in src/{BASE,SOURCE,EDITOR} and include/:

* enum members: every H2_ENUM_* block (BEGIN, CLASS_BEGIN, CLASS_BEGIN_T,
  CLASS_BEGIN_SPLIT) and raw ``enum`` block, inventoried lexically, including
  editor-only and unreferenced source;
* object-like ``#define`` constants: every macro whose body evaluates to an
  integer constant expression; and
* ``const``/``static const`` integer variables, at namespace, class and
  function scope.

A libclang pass evaluates every translation unit of every image with that
image's defines (``homm2.manifest.clang_image_defines``: a shared unit is read
once as the game and once as the editor, so ``#ifdef HOMM2_EDITOR`` code is
evaluated too).  It evaluates aliases, shifts, negative values, character
constants and implicit increments with the target ABI.  The lexical inventory
and the evaluated view must cover one another: an enum member or a numeric
macro that no unit evaluates is a fatal coverage hole rather than a silently
incomplete report.

    homm2 verify enum-reuse                  # write the derived reports, check the ledger
    homm2 verify enum-reuse --by-value       # every value and every key that has it
    homm2 verify enum-reuse --value 10       # one value (repeatable)
    homm2 verify enum-reuse --duplicates     # values declared in two or more domains
    homm2 verify enum-reuse --json           # the selected values as JSON
    homm2 verify enum-reuse --init-ledger    # snapshot the review worklist
    homm2 verify enum-reuse --extend-ledger  # append new domains as pending

Each key also records its semantic use contexts (the declaration identity of
the field, parameter, comparison operand, switch subject, array or return that
receives each reference to it; homm2.verify.constant_context), so the
collision and pair reports rank domains that share producers/consumers above
numeric overlap.
"""

from __future__ import annotations

from homm2.core.usage import logged

import argparse
import csv
import ctypes
import json
import multiprocessing
import re
import sys
from collections import Counter, defaultdict
from concurrent.futures import ProcessPoolExecutor
from dataclasses import asdict, dataclass, field, replace
from itertools import combinations
from pathlib import Path

from homm2.core.paths import BUILD, REPO, job_cap


GEN = BUILD / "gen"
REPORT = GEN / "enum_reuse.tsv"
VALUE_REPORT = GEN / "constant_values.tsv"
VALUE_JSON = GEN / "constant_values.json"
COLLISION_REPORT = GEN / "enum_value_collisions.tsv"
PAIR_REPORT = GEN / "enum_domain_pairs.tsv"
ROLE_PAIR_REPORT = GEN / "enum_role_pairs.tsv"
LEDGER = REPO / "config/reviews/enum-reuse.tsv"
IMAGES = ("game", "editor")

LEDGER_FIELDS = (
    "source_enum", "members", "decision", "current_enums", "member_reuse",
    "reason",
)
LEDGER_DECISIONS = frozenset(("pending", "retain", "canonical", "reuse"))
#: member_reuse target of a member removed because no code names it: the
#: check requires the identifier to be absent from every project file.
RETIRED = "-"
_IDENTIFIER = re.compile(r"[A-Za-z_]\w*")
_SOURCE_SUFFIXES = (".h", ".hpp", ".inl", ".c", ".cpp")

#: Ints.h holds the H2_ENUM_* machinery; its macro parameters are not domains.
_ENUM_MACHINERY = "include/Ints.h"
_MACRO_BLOCK = re.compile(
    r"\bH2_ENUM_(BEGIN|CLASS_BEGIN_SPLIT|CLASS_BEGIN_T|CLASS_BEGIN)"
    r"\(\s*(\w+)\s*(?:,\s*(\w+)\s*)?\)(?P<body>.*?)"
    r"\bH2_ENUM_(?:END|CLASS_END_SPLIT|CLASS_END_T|CLASS_END)\(",
    re.S,
)
_RAW_ENUM = re.compile(
    r"\b(?:typedef\s+)?enum\s+(?:(?:class|struct)\s+)?"
    r"(?P<name>[A-Za-z_]\w*)?\s*(?::\s*[A-Za-z_][\w \t]*?)?\s*\{"
)
_MEMBER_NAME = re.compile(r"[A-Za-z_]\w*")
_DEFINE = re.compile(
    r"^[ \t]*#[ \t]*define[ \t]+(?P<name>[A-Za-z_]\w*)(?P<function>\()?"
    r"(?P<body>(?:[^\n\\]|\\.)*)", re.M | re.S)
_INTEGER_TOKEN = re.compile(
    r"(?<![\w.])(?:0[xX][0-9A-Fa-f]+|[0-9]+)[uUlL]*(?![\w.])")
_FLOAT_TOKEN = re.compile(r"(?<![\w.])(?:[0-9]+\.[0-9]*|\.[0-9]+|[0-9]+[eE][-+]?[0-9]+)")
_NUMBER = re.compile(rb"(?:0[xX][0-9A-Fa-f]+|[0-9]+)(?:[uUlL]*)(?![A-Za-z0-9_.])")
_SUFFIX = re.compile(r"[uUlL]+$")
#: Probe identifiers appended after a unit's last line to evaluate macros.
_PROBE = "__h2_enum_reuse_probe_"


@dataclass(frozen=True)
class Member:
    name: str
    line: int
    column: int
    offset: int
    expression: str


@dataclass(frozen=True)
class Block:
    source_enum: str
    domain: str
    kind: str
    storage: str
    file: str
    line: int
    offset: int
    end_offset: int
    members: tuple[Member, ...]
    #: Declared only for the strict-enum view (see _strict_only_lines).
    strict_only: bool = False


@dataclass(frozen=True)
class Macro:
    """An object-like #define; ``numeric`` when its body spells an integer."""
    name: str
    file: str
    line: int
    column: int
    offset: int
    body: str
    numeric: bool


@dataclass(frozen=True)
class RawConstant:
    category: str
    value: int
    name: str
    file: str
    line: int
    column: int
    offset: int
    parent_name: str
    scope: str
    context: str
    image: str
    use_contexts: tuple[str, ...] = ()


@dataclass(frozen=True)
class Constant:
    value: int
    name: str
    qualified_name: str
    category: str
    file: str
    line: int
    column: int
    offset: int
    source_enum: str
    domain: str
    kind: str
    storage: str
    expression: str
    contexts: tuple[str, ...]
    images: tuple[str, ...]
    use_contexts: tuple[str, ...] = field(default=())


@dataclass(frozen=True)
class Literal:
    """A bare integer literal in a function body and its destination."""
    file: str
    offset: int
    value: int
    context_key: str


# --- lexical inventory -----------------------------------------------------


def _project_files(repo: Path):
    roots = [repo / "include"] + [repo / "src" / tier for tier in ("BASE", "SOURCE", "EDITOR")]
    for root in roots:
        if not root.is_dir():
            continue
        for path in sorted(root.rglob("*")):
            if path.is_file() and path.suffix in _SOURCE_SUFFIXES:
                yield path


def _read(path: Path) -> str:
    """Source text with one character per byte, so offsets are Clang's."""
    return path.read_bytes().decode("latin-1")


def blank_comments(text: str) -> str:
    """`text` with // and /* */ bodies blanked to spaces (newlines kept), so a
    declaration quoted in prose is never read as real. String/char literals
    are honoured (a `//` inside a string is not a comment)."""
    out, n, i, state = list(text), len(text), 0, "code"
    while i < n:
        c = text[i]
        if state == "code":
            if c == "/" and i + 1 < n and text[i + 1] == "/":
                while i < n and text[i] != "\n":
                    out[i] = " "
                    i += 1
                continue
            if c == "/" and i + 1 < n and text[i + 1] == "*":
                while i < n and not (text[i] == "*" and i + 1 < n and text[i + 1] == "/"):
                    if text[i] != "\n":
                        out[i] = " "
                    i += 1
                if i < n:
                    out[i] = out[i + 1] = " "
                    i += 2
                continue
            if c in "\"'":
                state = c
        elif c == "\\":
            i += 2
            continue
        elif c == state:
            state = "code"
        i += 1
    return "".join(out)


def _line_column(text: str, offset: int) -> tuple[int, int]:
    line = text.count("\n", 0, offset) + 1
    column = offset - text.rfind("\n", 0, offset)
    return line, column


def _matching_brace(text: str, opening: int) -> int:
    depth = 0
    for offset in range(opening, len(text)):
        if text[offset] == "{":
            depth += 1
        elif text[offset] == "}":
            depth -= 1
            if depth == 0:
                return offset
    raise ValueError(f"unterminated enum opening at byte {opening}")


def _members(text: str, body_start: int, body_end: int) -> tuple[Member, ...]:
    """Split an enum body on top-level commas and retain source offsets."""
    boundaries = []
    start = body_start
    depth = 0
    quote = ""
    escaped = False
    for offset in range(body_start, body_end):
        char = text[offset]
        if quote:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = ""
            continue
        if char in "\"'":
            quote = char
        elif char in "([{":
            depth += 1
        elif char in ")]}" and depth:
            depth -= 1
        elif char == "," and depth == 0:
            boundaries.append((start, offset))
            start = offset + 1
    boundaries.append((start, body_end))

    result = []
    for start, end in boundaries:
        segment = text[start:end]
        match = _MEMBER_NAME.search(segment)
        if match is None:
            continue
        offset = start + match.start()
        line, column = _line_column(text, offset)
        source = " ".join(text[offset:end].split())
        expression = source.split("=", 1)[1].strip() if "=" in source else ""
        result.append(Member(match.group(), line, column, offset, expression))
    return tuple(result)


def _source_enum(rel: str, domain: str, line: int, members, seen: Counter) -> str:
    anonymous = members[0].name if members else f"line-{line}"
    base = f"{rel}:{domain}" if domain else f"{rel}:<anonymous:{anonymous}>"
    seen[base] += 1
    return base if seen[base] == 1 else f"{base}#{seen[base]}"


_DIRECTIVE = re.compile(r"^[ \t]*#[ \t]*(\w*)[ \t]*(.*)$")


def _blank_directives(text: str) -> str:
    """`text` with preprocessor lines (and their continuations) blanked, so a
    conditional inside an enum body never reads as a member."""
    out = list(text)
    offset = 0
    continuing = False
    for line in text.split("\n"):
        if continuing or line.lstrip().startswith("#"):
            for index in range(offset, offset + len(line)):
                out[index] = " "
            continuing = line.endswith("\\")
        offset += len(line) + 1
    return "".join(out)


def _strict_only_lines(text: str) -> set[int]:
    """Lines only the strict-enum view compiles (`#if H2_STRICT_ENUMS`, or the
    `#else` of `#if !H2_STRICT_ENUMS`): the retail compiler never sees them."""
    lines = set()
    stack: list[tuple[bool, bool]] = []  # (strict conditional, strict branch)
    for number, line in enumerate(text.split("\n"), 1):
        match = _DIRECTIVE.match(line)
        if match:
            directive, argument = match.group(1), " ".join(match.group(2).split())
            if directive == "if" and argument in ("H2_STRICT_ENUMS", "!H2_STRICT_ENUMS"):
                stack.append((True, argument == "H2_STRICT_ENUMS"))
            elif directive in ("if", "ifdef", "ifndef"):
                stack.append((False, False))
            elif directive in ("else", "elif") and stack and stack[-1][0]:
                stack[-1] = (True, not stack[-1][1])
            elif directive == "endif" and stack:
                stack.pop()
            continue
        if any(conditional and branch for conditional, branch in stack):
            lines.add(number)
    return lines


def scan_blocks(*, repo: Path = REPO, paths=None) -> list[Block]:
    """Inventory source enum blocks without preprocessing."""
    blocks = []
    seen: Counter = Counter()
    for path in list(paths) if paths is not None else _project_files(repo):
        original = blank_comments(_read(path))
        text = _blank_directives(original)
        strict_only = _strict_only_lines(original)
        rel = path.resolve().relative_to(repo.resolve()).as_posix()
        occupied = []
        if rel != _ENUM_MACHINERY:
            for match in _MACRO_BLOCK.finditer(text):
                kind, domain, storage = match.group(1), match.group(2), match.group(3) or ""
                line, _ = _line_column(text, match.start())
                members = _members(text, match.start("body"), match.end("body"))
                blocks.append(Block(
                    _source_enum(rel, domain, line, members, seen), domain,
                    kind.lower(), storage, rel, line, match.start(), match.end(), members,
                    line in strict_only,
                ))
                occupied.append((match.start(), match.end()))

        for match in _RAW_ENUM.finditer(text):
            if any(start <= match.start() < end for start, end in occupied):
                continue
            opening = match.end() - 1
            closing = _matching_brace(text, opening)
            domain = match.group("name") or ""
            line, _ = _line_column(text, match.start())
            members = _members(text, opening + 1, closing)
            blocks.append(Block(
                _source_enum(rel, domain, line, members, seen), domain, "raw", "", rel,
                line, match.start(), closing + 1, members, line in strict_only,
            ))
    return sorted(blocks, key=lambda block: (block.file, block.offset))


def _probe_body(body: str) -> bool:
    """Whether a macro body can stand alone as a parenthesized expression."""
    if not body or any(token in body for token in ("{", "}", ";", "#", '"')):
        return False
    depth = 0
    for char in body:
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth < 0:
                return False
        elif char == "," and depth == 0:
            return False
    return depth == 0


def scan_macros(*, repo: Path = REPO, paths=None) -> list[Macro]:
    """Inventory object-like #define directives without preprocessing."""
    macros = []
    for path in list(paths) if paths is not None else _project_files(repo):
        text = blank_comments(_read(path))
        rel = path.resolve().relative_to(repo.resolve()).as_posix()
        for match in _DEFINE.finditer(text):
            if match.group("function"):
                continue
            body = " ".join(match.group("body").replace("\\\n", " ").split())
            if not body:
                continue
            numeric = (_probe_body(body) and bool(_INTEGER_TOKEN.search(body))
                       and not _FLOAT_TOKEN.search(body) and "'" not in body)
            offset = match.start("name")
            line, column = _line_column(text, offset)
            macros.append(Macro(match.group("name"), rel, line, column, offset, body, numeric))
    return macros


# --- libclang evaluation ---------------------------------------------------


def _evaluator(cidx):
    """clang_Cursor_Evaluate, which the Python bindings do not wrap."""
    lib = cidx.conf.lib
    if not getattr(lib, "_h2_evaluate", False):
        lib.clang_Cursor_Evaluate.argtypes = [cidx.Cursor]
        lib.clang_Cursor_Evaluate.restype = ctypes.c_void_p
        lib.clang_EvalResult_getKind.argtypes = [ctypes.c_void_p]
        lib.clang_EvalResult_getKind.restype = ctypes.c_int
        lib.clang_EvalResult_isUnsignedInt.argtypes = [ctypes.c_void_p]
        lib.clang_EvalResult_isUnsignedInt.restype = ctypes.c_uint
        lib.clang_EvalResult_getAsUnsigned.argtypes = [ctypes.c_void_p]
        lib.clang_EvalResult_getAsUnsigned.restype = ctypes.c_ulonglong
        lib.clang_EvalResult_getAsLongLong.argtypes = [ctypes.c_void_p]
        lib.clang_EvalResult_getAsLongLong.restype = ctypes.c_longlong
        lib.clang_EvalResult_dispose.argtypes = [ctypes.c_void_p]
        lib._h2_evaluate = True

    def evaluate(cursor):
        result = lib.clang_Cursor_Evaluate(cursor)
        if not result:
            return None
        try:
            if lib.clang_EvalResult_getKind(result) != 1:  # CXEval_Int
                return None
            if lib.clang_EvalResult_isUnsignedInt(result):
                return int(lib.clang_EvalResult_getAsUnsigned(result))
            return int(lib.clang_EvalResult_getAsLongLong(result))
        finally:
            lib.clang_EvalResult_dispose(result)
    return evaluate


_FUNCTION_KINDS = ("FUNCTION_DECL", "CXX_METHOD", "CONSTRUCTOR", "DESTRUCTOR",
                   "CONVERSION_FUNCTION", "FUNCTION_TEMPLATE")
_INTEGER_TYPES = ("CHAR_U", "UCHAR", "CHAR16", "CHAR32", "USHORT", "UINT", "ULONG",
                  "ULONGLONG", "UINT128", "CHAR_S", "SCHAR", "WCHAR", "SHORT", "INT",
                  "LONG", "LONGLONG", "INT128", "ENUM")


def _scope_name(cursor) -> str:
    parts = []
    while cursor is not None and cursor.kind.name != "TRANSLATION_UNIT":
        if cursor.spelling:
            parts.append(cursor.spelling)
        cursor = cursor.semantic_parent
    return "::".join(reversed(parts))


def _const_scope(cidx, node) -> str:
    """The ledger group of a const: the file, a class, or a function."""
    parent = node.semantic_parent
    if parent is None or parent.kind == cidx.CursorKind.TRANSLATION_UNIT:
        return "<const>"
    if parent.kind.name in _FUNCTION_KINDS:
        return f"{_scope_name(parent)}()::<const>"
    if parent.kind == cidx.CursorKind.NAMESPACE:
        return f"{_scope_name(parent)}::<const>"
    return f"{_scope_name(parent)}::<const>"


def _function_like(definition) -> bool:
    tokens = definition.get_tokens()
    name = next(tokens, None)
    parameters = next(tokens, None)
    return (name is not None and parameters is not None and parameters.spelling == "("
            and parameters.extent.start.offset == name.extent.end.offset)


def _accepted_diagnostics(repo: Path) -> frozenset[tuple[str, str]]:
    from homm2.audit.bool_fields import _reviewed_exceptions
    return frozenset((row.file, row.detail) for row in _reviewed_exceptions(repo).values()
                     if row.category == "parse-diagnostic")


def _scan_entry(payload):
    source_text, args, image, repo_text, probe_names, accepted = payload
    repo = Path(repo_text)
    import clang.cindex as cidx
    from homm2.retail_labels.annotated_data import configure_libclang
    from homm2.verify.constant_context import semantic_context

    configure_libclang()
    evaluate = _evaluator(cidx)
    path = Path(source_text)
    context = path.relative_to(repo).as_posix()
    original = path.read_bytes()
    original_lines = original.count(b"\n") + 1
    # The localization overlay maps an authored file with Tr("id") text to an
    # offset-preserving rendered copy; the probes extend that copy.
    compiled = original
    if "-ivfsoverlay" in args:
        overlay = json.loads(Path(args[args.index("-ivfsoverlay") + 1]).read_text())
        for root in overlay.get("roots", ()):
            if Path(root["name"]).resolve() == path:
                compiled = Path(root["external-contents"]).read_bytes()
    # A name probe evaluates the macro visible at the end of the unit; a body
    # probe evaluates the definition text of a macro #undef'd before it.
    probes = []
    for index, (kind, text, _definition) in enumerate(probe_names):
        guard = f"#ifdef {text}\n" if kind == "name" else ""
        probes.append(f"{guard}enum {_PROBE}{index} : __int64 "
                      f"{{ {_PROBE}value_{index} = ({text}) }};\n{'#endif' if guard else ''}\n")
    probe_start = original_lines + 1
    contents = compiled.decode("latin-1") + "\n" + "".join(probes)
    try:
        tu = cidx.Index.create().parse(
            str(path), args=args, unsaved_files=[(str(path), contents)],
            options=cidx.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
    except cidx.TranslationUnitLoadError as exc:
        return None, f"{context} [{image}]: libclang could not load TU: {exc}"

    relative_paths: dict[str, str | None] = {}

    def project(name):
        if name not in relative_paths:
            try:
                rel = Path(name).resolve().relative_to(repo).as_posix()
            except ValueError:
                rel = None
            relative_paths[name] = rel if rel and rel.split("/", 1)[0] in ("src", "include") else None
        return relative_paths[name]

    failed_probe_lines = set()
    errors = []
    for diagnostic in tu.diagnostics:
        if diagnostic.severity < cidx.Diagnostic.Error:
            continue
        location = diagnostic.location
        rel = project(location.file.name) if location.file else None
        if rel == context and location.line >= probe_start:
            failed_probe_lines.add(location.line)
            continue
        # The pinned VC6 SDK contains pre-standard templates Clang rejects.
        if location.file and location.is_in_system_header and rel is None:
            continue
        if (rel, diagnostic.spelling) in accepted:
            continue
        errors.append(str(diagnostic))
    if errors:
        return None, f"{context} [{image}]: parse error: {errors[0]}"

    def in_source(node):
        """(project path, offset) of a node in the unit's own text."""
        location = node.location
        if location.file is None:
            return None
        rel = project(location.file.name)
        if rel is None or (rel == context and location.line >= probe_start):
            return None
        return rel, location.offset

    # Macro definitions and expansions sit at the top level of the record.
    definitions_by_name: dict[str, tuple] = {}
    expansions: dict[tuple[str, int], tuple[tuple[str, int], int]] = {}
    macro_cursors = {}
    for node in tu.cursor.get_children():
        if node.kind == cidx.CursorKind.MACRO_DEFINITION:
            where = in_source(node)
            if where is not None and not _function_like(node):
                definitions_by_name[node.spelling] = where
                macro_cursors[where] = node
        elif node.kind == cidx.CursorKind.MACRO_INSTANTIATION:
            where = in_source(node)
            ref = node.referenced
            if where is None or ref is None:
                continue
            definition = in_source(ref)
            if definition is not None and definition in macro_cursors:
                expansions[where] = (definition, node.extent.end.offset)

    constants: dict[tuple, RawConstant] = {}
    uses: dict[tuple, set[str]] = defaultdict(set)
    literals: list[Literal] = []
    raw_cache: dict[str, bytes] = {}
    macro_values: dict[tuple[str, int], int] = {}
    consumed: set[tuple[str, int]] = set()

    def add(category, value, node, where, parent_name="", scope=""):
        key = (category, where[0], where[1], value)
        if key not in constants:
            constants[key] = RawConstant(
                category, value, node.spelling, where[0], node.location.line,
                node.location.column, where[1], parent_name, scope, context, image)

    # Probe enumerators: each evaluated macro visible at the end of the unit,
    # then the bodies of the unit's macros that are no longer defined there.
    body_values = {}
    for node in tu.cursor.get_children():
        if node.kind != cidx.CursorKind.ENUM_DECL or not node.spelling.startswith(_PROBE):
            continue
        if node.location.line in failed_probe_lines:
            continue
        kind, text, body_definition = probe_names[int(node.spelling[len(_PROBE):])]
        definition = definitions_by_name.get(text) if kind == "name" else body_definition
        for member in node.get_children():
            if member.kind == cidx.CursorKind.ENUM_CONSTANT_DECL and definition:
                (macro_values if kind == "name" else body_values)[definition] = \
                    member.enum_value
    for definition, value in body_values.items():
        if definition in macro_cursors and definition not in macro_values:
            macro_values[definition] = value

    def literal_value(node, stack, rel):
        raw = raw_cache.get(rel)
        if raw is None:
            raw = raw_cache[rel] = original if rel == context else (repo / rel).read_bytes()
        offset = node.location.offset
        if offset >= len(raw) or (offset and (chr(raw[offset - 1]).isalnum()
                                              or raw[offset - 1:offset] in (b"_", b"."))):
            return None
        match = _NUMBER.match(raw, offset)
        if match is None:
            return None
        try:
            value = int(_SUFFIX.sub("", match.group().decode("ascii")), 0)
        except ValueError:
            return None
        parent = stack[-1] if stack else None
        if parent is not None and parent.kind == cidx.CursorKind.UNARY_OPERATOR:
            tokens = list(parent.get_tokens())
            if tokens and tokens[0].spelling == "-":
                value = -value
        return value

    def walk(node, stack):
        kind = node.kind
        where = in_source(node)
        if kind == cidx.CursorKind.ENUM_CONSTANT_DECL and where is not None:
            parent = node.semantic_parent
            if not (parent is not None and parent.spelling.startswith(_PROBE)):
                add("enum", node.enum_value, node, where,
                    parent.spelling if parent is not None else "")
        elif kind == cidx.CursorKind.VAR_DECL and where is not None:
            ty = node.type
            canonical = ty.get_canonical()
            if (ty.is_const_qualified() or canonical.is_const_qualified()) \
                    and canonical.kind.name in _INTEGER_TYPES \
                    and any(child.kind.is_expression() for child in node.get_children()):
                value = evaluate(node)
                if value is not None:
                    add("const", value, node, where, scope=_const_scope(cidx, node))
        elif kind == cidx.CursorKind.DECL_REF_EXPR and where is not None:
            ref = node.referenced
            if ref is not None and ref.kind in (cidx.CursorKind.ENUM_CONSTANT_DECL,
                                                cidx.CursorKind.VAR_DECL):
                definition = in_source(ref)
                if definition is not None:
                    key, _label = semantic_context(cidx, node, stack)
                    if key:
                        uses[definition].add(key)
        elif kind == cidx.CursorKind.INTEGER_LITERAL and where is not None:
            if any(parent.kind.name in _FUNCTION_KINDS for parent in stack) and not any(
                    parent.kind == cidx.CursorKind.ENUM_DECL for parent in stack):
                value = literal_value(node, stack, where[0])
                if value is not None:
                    key, _label = semantic_context(cidx, node, stack)
                    literals.append(Literal(where[0], where[1], value, key))
        if where is not None and kind.is_expression() and where in expansions \
                and where not in consumed:
            definition, end = expansions[where]
            if node.extent.end.offset == end:
                consumed.add(where)
                key, _label = semantic_context(cidx, node, stack)
                if key:
                    uses[definition].add(key)
        if kind in (cidx.CursorKind.MACRO_DEFINITION, cidx.CursorKind.MACRO_INSTANTIATION,
                    cidx.CursorKind.INCLUSION_DIRECTIVE):
            return
        child_stack = (*stack, node)
        for child in node.get_children():
            walk(child, child_stack)

    for node in tu.cursor.get_children():
        location = node.location
        # Declarations from system and vendor headers hold no project value.
        if location.file is not None and project(location.file.name) is None:
            continue
        walk(node, (tu.cursor,))

    for definition, value in macro_values.items():
        node = macro_cursors.get(definition)
        if node is not None:
            add("macro", value, node, definition)
    result = []
    for (category, file, offset, value), row in constants.items():
        result.append(replace(row, use_contexts=tuple(sorted(uses.get((file, offset), ())))))
    return (result, literals), None


def compile_entries(*, repo: Path = REPO) -> list[tuple[Path, list[str], str]]:
    """(source, clang arguments, image) for every C++ unit of every image.

    A shared unit is read once per image, with that image's defines."""
    from homm2.clang_options import ClangMode
    from homm2.graph.fixed_asm import UNITS as FIXED_ASM_UNITS
    from homm2.manifest import load, units
    from homm2.retail_labels.annotated_data import _clang_args
    manifest = load()
    entries = []
    for image in IMAGES:
        for unit in units(manifest, image=image):
            if unit["unit"] in FIXED_ASM_UNITS or not unit["source"].endswith(".cpp"):
                continue
            source = (repo / unit["source"]).resolve()
            entries.append((source, _clang_args(repo, source, mode=ClangMode.RETAIL_ANALYSIS,
                                                image=image), image))
    return entries


def scan_entries(entries, probe_names, *, repo: Path = REPO, jobs: int = 1):
    accepted = _accepted_diagnostics(repo)
    payloads = [(str(source), args, image, str(repo.resolve()), probe_names, accepted)
                for source, args, image in entries]
    rows: dict[tuple, RawConstant] = {}
    contexts: dict[tuple, set[str]] = defaultdict(set)
    images: dict[tuple, set[str]] = defaultdict(set)
    literals: dict[tuple[str, int], Literal] = {}
    errors = []
    # Workers receive the importable module's function: run as __main__ (or
    # through `homm2 audit enums`), this module's own names do not pickle.
    from homm2.verify.enum_reuse import _scan_entry as worker
    pool = ProcessPoolExecutor(max_workers=jobs) if jobs > 1 else None
    results = pool.map(worker, payloads) if pool else map(worker, payloads)
    try:
        for done, (result, error) in enumerate(results, 1):
            if error:
                errors.append(error)
                continue
            constants, unit_literals = result
            for constant in constants:
                key = (constant.category, constant.file, constant.offset, constant.value)
                old = rows.get(key)
                rows[key] = replace(old or constant, use_contexts=tuple(sorted(
                    set(constant.use_contexts) | set(old.use_contexts if old else ()))))
                contexts[key].add(constant.context)
                images[key].add(constant.image)
            for literal in unit_literals:
                literals.setdefault((literal.file, literal.offset), literal)
            if done % 20 == 0:
                print(f"[enum-reuse] {done}/{len(payloads)} unit views", file=sys.stderr)
    finally:
        if pool is not None:
            pool.shutdown()
    return rows, contexts, images, list(literals.values()), errors


def _qualified(category: str, domain: str, name: str) -> str:
    if category == "macro":
        return name
    if category == "const":
        scope = domain.removesuffix("<const>").removesuffix("::")
        return f"{scope}::{name}" if scope else name
    return f"{domain}::{name}" if domain else name


def _join(raw, contexts, images, blocks, macros):
    by_member = {}
    by_name = defaultdict(list)
    for block in blocks:
        for member in block.members:
            by_member[(block.file, member.offset)] = (block, member)
            by_name[(block.file, member.name)].append((block, member))
    macro_at = {(macro.file, macro.offset): macro for macro in macros}

    constants = []
    uncovered_ast = []
    covered_members = set()
    covered_macros = set()
    for key, constant in raw.items():
        tus = tuple(sorted(contexts[key]))
        image_set = tuple(image for image in IMAGES if image in images[key])
        if constant.category == "enum":
            match = by_member.get((constant.file, constant.offset))
            if match is None:
                candidates = by_name.get((constant.file, constant.name), [])
                if len(candidates) == 1:
                    match = candidates[0]
            if match is None:
                uncovered_ast.append(constant)
                continue
            block, member = match
            covered_members.add((block.file, member.offset))
            source_enum, domain, kind, storage = (block.source_enum, block.domain,
                                                  block.kind, block.storage)
            expression = member.expression
        elif constant.category == "macro":
            macro = macro_at.get((constant.file, constant.offset))
            if macro is None:
                uncovered_ast.append(constant)
                continue
            covered_macros.add((macro.file, macro.offset))
            domain = "<macros>"
            source_enum, kind, storage = f"{constant.file}:{domain}", "define", ""
            expression = macro.body
        else:
            domain = constant.scope
            source_enum, kind, storage, expression = f"{constant.file}:{domain}", "const", "", ""
        constants.append(Constant(
            constant.value, constant.name, _qualified(constant.category, domain, constant.name),
            constant.category, constant.file, constant.line, constant.column, constant.offset,
            source_enum, domain, kind, storage, expression, tus, image_set,
            constant.use_contexts,
        ))

    uncovered_source = [
        (block.file, member.line, f"{block.domain}::{member.name}")
        for block in blocks
        if not block.strict_only
        for member in block.members
        if (block.file, member.offset) not in covered_members
    ]
    # One macro name of one file is one key: a definition in a branch this
    # compiler never takes (H2_STRICT_ENUMS 1, H2_RETAIL_COMPILER 1) is
    # covered by its evaluated alternative.
    evaluated_names = {(file, macro_at[(file, offset)].name) for file, offset in covered_macros}
    uncovered_source += [
        (macro.file, macro.line, f"#define {macro.name} {macro.body}")
        for macro in macros
        if macro.numeric and (macro.file, macro.name) not in evaluated_names
    ]
    constants.sort(key=lambda row: (row.value, row.file, row.offset))
    return constants, uncovered_source, uncovered_ast


def probe_list(macros: list[Macro]) -> list[tuple[str, str, tuple[str, int] | None]]:
    """Probes appended to every unit: each macro name once, and the body of
    each numeric definition whose file #undefs that name."""
    probes = [("name", name, None)
              for name in sorted({macro.name for macro in macros if _probe_body(macro.body)})]
    undefined = defaultdict(set)
    for macro in macros:
        if macro.numeric and macro.file not in undefined:
            text = blank_comments(_read(REPO / macro.file))
            undefined[macro.file] = set(re.findall(r"^[ \t]*#[ \t]*undef[ \t]+(\w+)", text, re.M))
    probes += [("body", macro.body, (macro.file, macro.offset)) for macro in macros
               if macro.numeric and macro.name in undefined[macro.file]]
    return probes


def collect(*, repo: Path = REPO, jobs: int = 1):
    blocks = scan_blocks(repo=repo)
    macros = scan_macros(repo=repo)
    probe_names = probe_list(macros)
    entries = compile_entries(repo=repo)
    if not entries:
        raise RuntimeError("config/units.toml: no C++ translation units")
    raw, contexts, images, literals, errors = scan_entries(
        entries, probe_names, repo=repo, jobs=jobs)
    constants, uncovered_source, uncovered_ast = _join(raw, contexts, images, blocks, macros)
    return constants, blocks, literals, uncovered_source, uncovered_ast, errors, len(entries)


# --- reports ---------------------------------------------------------------


def _write_tsv(path: Path, fieldnames: tuple[str, ...], rows) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=fieldnames, dialect="excel-tab",
                                lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def write_report(path: Path, constants: list[Constant]) -> None:
    fields = (
        "value", "hex", "category", "name", "qualified_name", "file", "line", "column",
        "offset", "source_enum", "domain", "kind", "storage", "expression", "images",
        "contexts", "use_contexts",
    )
    rows = []
    for constant in constants:
        row = asdict(constant)
        row["hex"] = hex(constant.value)
        row["images"] = ";".join(constant.images)
        row["contexts"] = ";".join(constant.contexts)
        row["use_contexts"] = json.dumps(list(constant.use_contexts))
        rows.append({name: row[name] for name in fields})
    _write_tsv(path, fields, rows)


def by_value(constants: list[Constant]) -> list[tuple[int, list[Constant]]]:
    """Map<value, keys>: values held by two or more keys first, ordered by the
    number of distinct domains that share them, then by key count."""
    groups: dict[int, list[Constant]] = defaultdict(list)
    for constant in constants:
        groups[constant.value].append(constant)
    ordered = []
    for value, keys in groups.items():
        keys.sort(key=lambda row: (row.file, row.line, row.name))
        ordered.append((value, keys))
    ordered.sort(key=lambda item: (
        len(item[1]) < 2,
        -len({row.source_enum for row in item[1]}),
        -len(item[1]),
        item[0],
    ))
    return ordered


_VALUE_FIELDS = (
    "value", "hex", "value_keys", "value_domains", "category", "qualified_name",
    "domain", "file", "line", "images", "use_contexts",
)


def _value_rows(groups):
    for value, keys in groups:
        domains = len({row.source_enum for row in keys})
        for row in keys:
            yield {
                "value": value, "hex": hex(value), "value_keys": len(keys),
                "value_domains": domains, "category": row.category,
                "qualified_name": row.qualified_name, "domain": row.source_enum,
                "file": row.file, "line": row.line, "images": ";".join(row.images),
                "use_contexts": json.dumps(list(row.use_contexts)),
            }


def _value_json(groups) -> dict:
    return {str(value): [{
        "qualified_name": row.qualified_name, "category": row.category,
        "domain": row.source_enum, "location": f"{row.file}:{row.line}",
        "images": list(row.images), "use_contexts": list(row.use_contexts),
    } for row in keys] for value, keys in groups}


def write_value_report(path: Path, json_path: Path, constants: list[Constant]) -> None:
    groups = by_value(constants)
    _write_tsv(path, _VALUE_FIELDS, _value_rows(groups))
    json_path.write_text(json.dumps({"schema": 1, "values": _value_json(groups)},
                                    indent=1) + "\n")


def write_collision_report(path: Path, constants: list[Constant],
                           literals: list[Literal]) -> None:
    groups = defaultdict(list)
    for constant in constants:
        groups[constant.value].append(constant)
    literal_sites = Counter(literal.value for literal in literals)
    literal_contexts = defaultdict(set)
    for literal in literals:
        if literal.context_key:
            literal_contexts[literal.value].add(literal.context_key)
    fields = (
        "value", "hex", "declarations", "domains", "members",
        "function_literal_sites", "shared_named_contexts", "shared_literal_contexts",
    )
    rows = []
    for value, declarations in sorted(groups.items()):
        domains = sorted({row.source_enum for row in declarations})
        if len(domains) < 2 and not literal_sites[value]:
            continue
        context_domains = defaultdict(set)
        for declaration in declarations:
            for key in declaration.use_contexts:
                context_domains[key].add(declaration.source_enum)
        rows.append({
            "value": value,
            "hex": hex(value),
            "declarations": len(declarations),
            "domains": ";".join(domains),
            "members": ";".join(f"{row.qualified_name}@{row.file}:{row.line}"
                                for row in declarations),
            "function_literal_sites": literal_sites[value],
            "shared_named_contexts": json.dumps(sorted(
                key for key, owners in context_domains.items() if len(owners) > 1)),
            "shared_literal_contexts": json.dumps(sorted(
                set(context_domains) & literal_contexts[value])),
        })
    rows.sort(key=lambda row: (
        not json.loads(row["shared_named_contexts"]),
        not json.loads(row["shared_literal_contexts"]), row["value"]))
    _write_tsv(path, fields, rows)


def _enum_members(constants: list[Constant]) -> list[Constant]:
    return [row for row in constants if row.category == "enum"]


def write_pair_report(path: Path, constants: list[Constant]) -> None:
    """Rank enum pairs by shared evaluated values without asserting identity."""
    by_domain = defaultdict(list)
    for constant in _enum_members(constants):
        by_domain[constant.source_enum].append(constant)
    values = {name: {row.value for row in rows} for name, rows in by_domain.items()}
    contexts = {name: {(row.value, key) for row in rows for key in row.use_contexts}
                for name, rows in by_domain.items()}
    fields = (
        "left", "right", "left_members", "right_members", "shared_values",
        "shared_count", "min_coverage_pct", "exact_value_set",
        "exact_value_sequence", "shared_contexts", "direct_shared_contexts",
    )
    rows = []
    for left_name, right_name in combinations(sorted(by_domain), 2):
        left_values, right_values = values[left_name], values[right_name]
        shared = left_values & right_values
        exact_set = left_values == right_values
        if not shared and not exact_set:
            continue
        shared_contexts = {key for value, key in contexts[left_name] & contexts[right_name]}
        if len(shared) < 2 and not exact_set and not shared_contexts:
            continue
        left, right = by_domain[left_name], by_domain[right_name]
        direct = sorted(key for key in shared_contexts if "/via:" not in key)
        coverage = 100.0 * len(shared) / min(len(left_values), len(right_values))
        rows.append({
            "left": left_name,
            "right": right_name,
            "left_members": ";".join(f"{row.name}={row.value}" for row in left),
            "right_members": ";".join(f"{row.name}={row.value}" for row in right),
            "shared_values": ";".join(str(value) for value in sorted(shared)),
            "shared_count": len(shared),
            "min_coverage_pct": f"{coverage:.2f}",
            "exact_value_set": "yes" if exact_set else "no",
            "exact_value_sequence": (
                "yes" if [row.value for row in left] == [row.value for row in right]
                else "no"
            ),
            "shared_contexts": json.dumps(sorted(shared_contexts)),
            "direct_shared_contexts": json.dumps(direct),
        })
    rows.sort(key=lambda row: (
        -len(json.loads(row["direct_shared_contexts"])),
        -len(json.loads(row["shared_contexts"])),
        row["exact_value_sequence"] != "yes",
        row["exact_value_set"] != "yes",
        -float(row["min_coverage_pct"]),
        -row["shared_count"],
        row["left"],
        row["right"],
    ))
    _write_tsv(path, fields, rows)


def _member_roles(members: list[Constant]) -> dict[tuple[int, tuple[str, ...]], str]:
    """Remove only a prefix common to every member of one source enum."""
    names = [member.name.split("_") for member in members]
    prefix_length = 0
    for parts in zip(*names):
        if len(set(parts)) != 1:
            break
        prefix_length += 1
    if prefix_length == min(map(len, names)):
        prefix_length -= 1
    return {
        (member.value, tuple(parts[prefix_length:])): member.name
        for member, parts in zip(members, names)
    }


def write_role_pair_report(path: Path, constants: list[Constant]) -> None:
    """Shortlist equal values with equal member roles; still only review leads."""
    by_domain = _members_by_enum(_enum_members(constants))
    roles = {name: _member_roles(members) for name, members in by_domain.items()}
    fields = ("left", "right", "matching_roles", "left_count", "right_count", "role_coverage_pct")
    rows = []
    for left, right in combinations(sorted(roles), 2):
        shared = sorted(roles[left].keys() & roles[right].keys())
        if len(shared) < 2:
            continue
        rows.append({
            "left": left,
            "right": right,
            "matching_roles": ";".join(
                f"{value}:{roles[left][value, role]}={roles[right][value, role]}"
                for value, role in shared
            ),
            "left_count": len(by_domain[left]),
            "right_count": len(by_domain[right]),
            "role_coverage_pct": f"{100.0 * len(shared) / min(len(by_domain[left]), len(by_domain[right])):.2f}",
        })
    rows.sort(key=lambda row: (
        -float(row["role_coverage_pct"]),
        -len(row["matching_roles"].split(";")),
        row["left"], row["right"],
    ))
    _write_tsv(path, fields, rows)


def _members_by_enum(constants: list[Constant]) -> dict[str, list[Constant]]:
    result = defaultdict(list)
    for constant in constants:
        result[constant.source_enum].append(constant)
    for members in result.values():
        members.sort(key=lambda row: (row.offset, row.value))
    return result


# --- review ledger ---------------------------------------------------------


def _ledger_domains(constants: list[Constant], blocks: list[Block]) -> list[str]:
    """Every enum block in source order, then each file's macro and const groups."""
    names = [block.source_enum for block in blocks if not block.strict_only]
    seen = set(names)
    for constant in sorted(constants, key=lambda row: (row.file, row.source_enum)):
        if constant.source_enum not in seen:
            seen.add(constant.source_enum)
            names.append(constant.source_enum)
    return names


def _snapshot(members: list[Constant]) -> str:
    return ";".join(f"{row.name}={row.value}" for row in members)


def init_ledger(path: Path, constants: list[Constant], blocks: list[Block]) -> int:
    """Snapshot the complete starting worklist; never overwrite review work."""
    if path.exists():
        raise FileExistsError(f"{path}: review ledger already exists")
    by_enum = _members_by_enum(constants)
    rows = [{
        "source_enum": name,
        "members": _snapshot(by_enum.get(name, [])),
        "decision": "pending",
        "current_enums": name,
        "member_reuse": "",
        "reason": "",
    } for name in _ledger_domains(constants, blocks)]
    _write_tsv(path, LEDGER_FIELDS, rows)
    return len(rows)


def extend_ledger(path: Path, constants: list[Constant]) -> int:
    """Add unclaimed current declarations as pending, preserving old decisions."""
    with path.open(newline="") as stream:
        reader = csv.DictReader(stream, dialect="excel-tab")
        if tuple(reader.fieldnames or ()) != LEDGER_FIELDS:
            raise ValueError(f"{path}: unexpected ledger schema")
        rows = list(reader)
    claimed = set()
    existing = {row["source_enum"] for row in rows}
    for row in rows:
        members, findings = _parse_members(row["members"], source_enum=row["source_enum"])
        reuse, more = _parse_reuse(row["member_reuse"], source_enum=row["source_enum"])
        if findings or more:
            raise ValueError("; ".join(findings + more))
        claimed.update(reuse.get(name, f"{row['source_enum']}::{name}") for name in members)
    added = 0
    for domain, members in sorted(_members_by_enum(constants).items()):
        unclaimed = [row for row in members if f"{domain}::{row.name}" not in claimed]
        if not unclaimed or domain in existing:
            # An existing row keeps its starting snapshot and decision;
            # check_ledger still reports the unclaimed additions.
            continue
        rows.append({"source_enum": domain, "members": _snapshot(unclaimed),
                     "decision": "pending", "current_enums": domain,
                     "member_reuse": "", "reason": ""})
        added += 1
    _write_tsv(path, LEDGER_FIELDS, rows)
    return added


def _parse_members(text: str, *, source_enum: str) -> tuple[dict[str, list[int]], list[str]]:
    """``NAME=value`` items; one name may carry two values when an image or
    conditional evaluates its declaration differently."""
    members: dict[str, list[int]] = {}
    findings = []
    for item in filter(None, text.split(";")):
        if "=" not in item:
            findings.append(f"{source_enum}: malformed member snapshot {item!r}")
            continue
        name, value_text = item.rsplit("=", 1)
        try:
            value = int(value_text, 0)
        except ValueError:
            findings.append(f"{source_enum}: invalid value in snapshot {item!r}")
            continue
        values = members.setdefault(name, [])
        if value in values:
            findings.append(f"{source_enum}: duplicate snapshot member {item}")
        values.append(value)
    return members, findings


def _parse_reuse(text: str, *, source_enum: str) -> tuple[dict[str, str], list[str]]:
    """Parse ``OLD=target-source-enum::TARGET`` member provenance entries."""
    reuse = {}
    findings = []
    for item in filter(None, text.split(";")):
        if "=" not in item:
            findings.append(f"{source_enum}: malformed member_reuse {item!r}")
            continue
        old, target = item.split("=", 1)
        if target != RETIRED and "::" not in target:
            findings.append(
                f"{source_enum}: member_reuse target needs source-enum::member: {item!r}")
            continue
        if old in reuse:
            findings.append(f"{source_enum}: duplicate member_reuse for {old}")
        reuse[old] = target
    return reuse, findings


def _project_identifiers(repo: Path) -> set[str]:
    names: set[str] = set()
    for path in _project_files(repo):
        names.update(_IDENTIFIER.findall(blank_comments(_read(path))))
    return names


def check_ledger(path: Path, constants: list[Constant], *, repo: Path = REPO) -> list[str]:
    """Prove that every starting member has a reviewed, value-preserving home."""
    identifiers = None
    if not path.is_file():
        return [f"{path.relative_to(repo)}: missing review ledger (run --init-ledger once)"]
    current: dict[str, set[int]] = defaultdict(set)
    for row in constants:
        current[f"{row.source_enum}::{row.name}"].add(row.value)
    claimed = set()
    findings = []
    seen = set()
    with path.open(newline="") as stream:
        reader = csv.DictReader(stream, dialect="excel-tab")
        if tuple(reader.fieldnames or ()) != LEDGER_FIELDS:
            return [f"{path}: expected columns {','.join(LEDGER_FIELDS)}"]
        for line, row in enumerate(reader, 2):
            source_enum = row["source_enum"]
            if not source_enum:
                findings.append(f"{path.name}:{line}: empty source_enum")
                continue
            if source_enum in seen:
                findings.append(f"{path.name}:{line}: duplicate source_enum {source_enum}")
                continue
            seen.add(source_enum)
            decision = row["decision"]
            if decision not in LEDGER_DECISIONS:
                findings.append(f"{source_enum}: invalid decision {decision!r}")
            if decision == "pending":
                findings.append(f"{source_enum}: review is pending")
            if decision != "pending" and not row["reason"].strip():
                findings.append(f"{source_enum}: reviewed row needs an evidence reason")
            members, row_findings = _parse_members(row["members"], source_enum=source_enum)
            findings.extend(row_findings)
            reuse, row_findings = _parse_reuse(row["member_reuse"], source_enum=source_enum)
            findings.extend(row_findings)
            unknown = sorted(set(reuse) - set(members))
            if unknown:
                findings.append(
                    f"{source_enum}: member_reuse names absent from snapshot: "
                    + ", ".join(unknown))
            if decision == "reuse" and not reuse:
                findings.append(f"{source_enum}: reuse decision has no member mapping")
            if decision in ("retain", "canonical") and reuse:
                findings.append(f"{source_enum}: {decision} decision cannot redirect members")

            target_enums = set()
            for name, values in members.items():
                target = reuse.get(name, f"{source_enum}::{name}")
                if target == RETIRED:
                    if identifiers is None:
                        identifiers = _project_identifiers(repo)
                    if name in identifiers:
                        findings.append(
                            f"{source_enum}: retired member {name} is still named in the project")
                    continue
                target_enum, separator, target_name = target.rpartition("::")
                if not separator or not target_enum or not target_name:
                    findings.append(f"{source_enum}: invalid target for {name}: {target!r}")
                    continue
                target_enums.add(target_enum)
                if target not in current:
                    findings.append(f"{source_enum}: {name} has no current target {target}")
                else:
                    for value in values:
                        if value not in current[target]:
                            findings.append(
                                f"{source_enum}: {name}={value} maps to {target}="
                                f"{'/'.join(map(str, sorted(current[target])))}")
                claimed.add(target)
            declared_enums = set(filter(None, row["current_enums"].split(";")))
            if target_enums != declared_enums and (target_enums or members):
                findings.append(
                    f"{source_enum}: current_enums is {sorted(declared_enums)}, "
                    f"member targets use {sorted(target_enums)}")

    unclaimed = sorted(set(current) - claimed)
    if unclaimed:
        preview = ", ".join(unclaimed[:10])
        suffix = " ..." if len(unclaimed) > 10 else ""
        findings.append(
            f"{len(unclaimed)} current member(s) have no starting-ledger provenance: "
            f"{preview}{suffix}")
    return findings


def _print_values(groups, stream=sys.stdout) -> None:
    writer = csv.DictWriter(stream, fieldnames=_VALUE_FIELDS, dialect="excel-tab",
                            lineterminator="\n")
    writer.writeheader()
    writer.writerows(_value_rows(groups))


@logged
def main(argv=None) -> int:
    parser = argparse.ArgumentParser(
        prog="homm2 verify enum-reuse", description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--by-value", action="store_true",
                        help="print every value with every key that has it")
    parser.add_argument("--value", action="append", type=lambda text: int(text, 0),
                        default=[], help="print one evaluated integer (repeatable)")
    parser.add_argument("--duplicates", action="store_true",
                        help="print only values declared in two or more domains")
    parser.add_argument("--json", action="store_true",
                        help="print the selected values as JSON")
    parser.add_argument("--no-report", action="store_true",
                        help="do not write the build/gen derived reports")
    parser.add_argument("--init-ledger", action="store_true",
                        help="create the complete pending review ledger; refuse overwrite")
    parser.add_argument("--extend-ledger", action="store_true",
                        help="append unclaimed current domains as pending rows")
    parser.add_argument("--jobs", type=int,
                        default=min(4, multiprocessing.cpu_count()),
                        help="parallel libclang translation-unit workers "
                             "(at most $HOMM2_JOBS)")
    args = parser.parse_args(argv)
    try:
        (constants, blocks, literals, uncovered_source, uncovered_ast, errors,
         views) = collect(jobs=job_cap(args.jobs))
    except (FileNotFoundError, RuntimeError, ValueError) as exc:
        print(f"[enum-reuse] FATAL: {exc}")
        return 2
    if errors or uncovered_source or uncovered_ast:
        for error in errors[:10]:
            print(f"   {error}")
        for file, line, text in uncovered_source[:20]:
            print(f"   {file}:{line}: source declaration not evaluated: {text}")
        for row in uncovered_ast[:20]:
            print(f"   {row.file}:{row.line}: evaluated {row.category} not inventoried: "
                  f"{row.parent_name}::{row.name}")
        total = len(errors) + len(uncovered_source) + len(uncovered_ast)
        print(f"[enum-reuse] FATAL: {total} coverage/parsing finding(s)")
        return 2

    groups = by_value(constants)
    selected = [
        (value, keys) for value, keys in groups
        if (not args.value or value in args.value)
        and (not args.duplicates or len({row.source_enum for row in keys}) > 1)
    ]
    if args.value:
        order = {value: index for index, value in enumerate(args.value)}
        selected.sort(key=lambda item: order[item[0]])
    if args.json:
        print(json.dumps({"schema": 1, "values": _value_json(selected)}, indent=2))
    elif args.by_value or args.value or args.duplicates:
        _print_values(selected)
    if not args.no_report:
        write_report(REPORT, constants)
        write_value_report(VALUE_REPORT, VALUE_JSON, constants)
        write_collision_report(COLLISION_REPORT, constants, literals)
        write_pair_report(PAIR_REPORT, constants)
        write_role_pair_report(ROLE_PAIR_REPORT, constants)
    if args.init_ledger:
        try:
            rows = init_ledger(LEDGER, constants, blocks)
        except FileExistsError as exc:
            print(f"[enum-reuse] FATAL: {exc}")
            return 2
        print(f"[enum-reuse] initialized {LEDGER.relative_to(REPO)} with "
              f"{rows} pending row(s)", file=sys.stderr)
    if args.extend_ledger:
        try:
            added = extend_ledger(LEDGER, constants)
        except (OSError, ValueError) as exc:
            print(f"[enum-reuse] FATAL: {exc}")
            return 2
        print(f"[enum-reuse] appended {added} pending row(s) to "
              f"{LEDGER.relative_to(REPO)}", file=sys.stderr)
    findings = check_ledger(LEDGER, constants)
    pending = sum(finding.endswith(": review is pending") for finding in findings)
    other = [finding for finding in findings if not finding.endswith(": review is pending")]
    for finding in other[:20]:
        print(f"   {finding}", file=sys.stderr)
    if len(other) > 20:
        print(f"   ... {len(other) - 20} more", file=sys.stderr)
    categories = Counter(row.category for row in constants)
    domains_by_value = {value: {row.source_enum for row in keys} for value, keys in groups}
    shared = sum(len(keys) > 1 for _value, keys in groups)
    collisions = sum(len(domains) > 1 for domains in domains_by_value.values())
    strict_only = sum(block.strict_only for block in blocks)
    print(f"[enum-reuse] {views} unit view(s); {len(blocks) - strict_only} enum block(s) "
          f"(+{strict_only} strict-view only); "
          f"{len(constants)} key(s) (enum {categories['enum']}, macro "
          f"{categories['macro']}, const {categories['const']}); {len(groups)} value(s), "
          f"{shared} held by two or more keys, {collisions} by two or more domains",
          file=sys.stderr)
    if not args.no_report:
        print(f"[enum-reuse] reports: {VALUE_REPORT.relative_to(REPO)} (+ .json), "
              f"{REPORT.relative_to(REPO)}, {COLLISION_REPORT.relative_to(REPO)}, "
              f"{PAIR_REPORT.relative_to(REPO)}, {ROLE_PAIR_REPORT.relative_to(REPO)}",
              file=sys.stderr)
    if findings:
        print(f"[enum-reuse] FAIL: {pending} pending row(s), {len(other)} other ledger "
              f"finding(s)", file=sys.stderr)
        return 1
    with LEDGER.open(newline="") as stream:
        reviewed = sum(1 for _row in csv.DictReader(stream, dialect="excel-tab"))
    print(f"[enum-reuse] OK: all {reviewed} starting domain(s) reviewed and "
          "all current keys accounted for", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
