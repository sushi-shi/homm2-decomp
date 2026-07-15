#!/usr/bin/env python3
"""Generate reviewable libclang source variants for batch_source_variants.py.

The generator never compiles or modifies the source file.  It locates one reconstructed
function by its ``VA(...)`` marker, derives conservative AST mutations, combines only
non-overlapping edits, and writes their exact byte ranges and before/after text to JSON.
The batch runner validates those ranges against the unchanged source before compiling.

Generated families are operand order for commutative/relational expressions, reorder of
independent local assignments, split/merge of simple local declarations, extraction of a
pure expression into an inline helper, and extraction of the common read-then-advance cursor
idiom.  It can merge several helper definitions at the same insertion point and generate a
two-level helper chain for nested pure expressions.  Other sequencing changes are intentionally
absent: author those as explicit literal axes after a semantic audit.

Generated helpers pass scalar/pointer/enum values directly, preserve existing defensible lvalue
references, and bind read-only record objects by const reference. Record returns, volatile access,
implicit ``this``, member lvalue contexts, hidden calls/copies, and indirect assignment targets are
rejected rather than left for the compiler to filter.

Run inside ``nix develop .#build``::

    python3 scripts/generate_ast_variants.py src/BASE/icon2by.cpp 0xda270 \
        --max-depth 3 --limit 4096 -o /tmp/icon2by-ast.json
    python3 scripts/batch_source_variants.py /tmp/icon2by-ast.json
"""

from __future__ import annotations

import argparse
import glob
import hashlib
import itertools
import json
import os
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci

from tu_state_noise import (
    DEFAULT_FAMILIES as DEFAULT_STATE_FAMILIES,
    include_macro_guard,
    make_variants as make_state_variants,
    resolve_target as resolve_state_target,
    target_identifiers,
)


COMMUTATIVE = {"+", "*", "==", "!=", "&", "|", "^"}
RELATIONAL_FLIP = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
ASSIGNMENTS = {"=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="}
FUNCTION_KINDS = (
    ci.CursorKind.FUNCTION_DECL,
    ci.CursorKind.CXX_METHOD,
    ci.CursorKind.CONSTRUCTOR,
    ci.CursorKind.DESTRUCTOR,
)
CALL_KINDS = {
    kind for kind in (
        getattr(ci.CursorKind, "CALL_EXPR", None),
        getattr(ci.CursorKind, "CXX_MEMBER_CALL_EXPR", None),
        getattr(ci.CursorKind, "CXX_OPERATOR_CALL_EXPR", None),
        getattr(ci.CursorKind, "CXX_CONSTRUCT_EXPR", None),
        getattr(ci.CursorKind, "CXX_TEMPORARY_OBJECT_EXPR", None),
    ) if kind is not None
}
INTEGRAL_TYPE_KINDS = {
    getattr(ci.TypeKind, name) for name in (
        "BOOL", "CHAR_U", "UCHAR", "CHAR16", "CHAR32", "USHORT", "UINT", "ULONG",
        "ULONGLONG", "UINT128", "CHAR_S", "SCHAR", "WCHAR", "SHORT", "INT", "LONG",
        "LONGLONG", "INT128",
    ) if hasattr(ci.TypeKind, name)
}
FLOAT_TYPE_KINDS = {
    getattr(ci.TypeKind, name) for name in ("HALF", "FLOAT", "DOUBLE", "LONGDOUBLE")
    if hasattr(ci.TypeKind, name)
}
REFERENCE_TYPE_KINDS = {
    kind for kind in (
        getattr(ci.TypeKind, "LVALUEREFERENCE", None),
        getattr(ci.TypeKind, "RVALUEREFERENCE", None),
    ) if kind is not None
}
CONTEXT_SENSITIVE_TOKENS = (b"__COUNTER__", b"__FUNCTION__", b"__FUNCSIG__", b"__LINE__")


@dataclass(frozen=True)
class AstEdit:
    start: int
    end: int
    replacement: bytes


@dataclass(frozen=True)
class AstMutation:
    family: str
    label: str
    edits: tuple[AstEdit, ...]


def configure_libclang() -> None:
    libraries = glob.glob("/nix/store/*clang*-lib/lib/libclang.so")
    if libraries:
        try:
            ci.Config.set_library_file(libraries[0])
        except Exception:
            pass


def clang_args(root: Path, source: Path) -> list[str]:
    database_path = root / "build/clangd/compile_commands.json"
    database = json.loads(database_path.read_text()) if database_path.is_file() else []
    source_resolved = source.resolve()
    raw = []
    for entry in database:
        file_value = entry.get("file")
        if not isinstance(file_value, str):
            continue
        entry_file = Path(file_value)
        if not entry_file.is_absolute():
            entry_file = Path(entry.get("directory", root)) / entry_file
        if entry_file.resolve() == source_resolved:
            raw = entry.get("arguments", [])
            break
    out = [
        "-x", "c++", "-std=c++14", "-fms-compatibility", "-ferror-limit=0",
        "-Wno-writable-strings", "-fwritable-strings",
        "-I", str(root / "include"),
    ]
    vendor = root / "vendor"
    if vendor.is_dir():
        for include in sorted(path for path in vendor.iterdir() if path.is_dir()):
            out.extend(("-I", str(include)))
    index = 0
    while index < len(raw):
        arg = raw[index]
        if arg in ("/imsvc", "/I") and index + 1 < len(raw):
            out.extend(("-isystem" if arg == "/imsvc" else "-I", raw[index + 1]))
            index += 2
            continue
        if arg.startswith("/D"):
            out.append("-D" + arg[2:])
        elif arg.startswith(("--target=", "-fms", "-fdelayed")):
            out.append(arg)
        index += 1
    return out


def marker_span(blob: bytes, rva: int) -> tuple[int, int]:
    marker = f"VA(0x{rva + 0x400000:08x},".encode()
    marker_start = blob.find(marker)
    if marker_start < 0:
        raise ValueError(f"VA marker not found for RVA 0x{rva:x}")
    next_marker = blob.find(b"\nVA(0x", marker_start + 1)
    return marker_start, next_marker if next_marker >= 0 else len(blob)


def target_function(tu: ci.TranslationUnit, source: Path, blob: bytes, rva: int):
    marker_start, span_end = marker_span(blob, rva)
    matches = []
    for cursor in tu.cursor.walk_preorder():
        if cursor.kind not in FUNCTION_KINDS or not cursor.is_definition():
            continue
        cursor_file = cursor.extent.start.file
        if cursor_file is None or Path(str(cursor_file)).resolve() != source.resolve():
            continue
        if marker_start <= cursor.extent.start.offset < span_end:
            matches.append(cursor)
    if len(matches) != 1:
        raise ValueError(f"expected one function in RVA marker span, found {len(matches)}")
    return matches[0]


def classify_parse_errors(tu: ci.TranslationUnit, source: Path, fn):
    """Only a nonfatal diagnostic strictly after the target cannot affect its parsed AST."""
    blocking = []
    trailing = []
    source = source.resolve()
    for diagnostic in tu.diagnostics:
        if diagnostic.severity < ci.Diagnostic.Error:
            continue
        location_file = diagnostic.location.file
        same_source = (
            location_file is not None and Path(str(location_file)).resolve() == source
        )
        if (
            diagnostic.severity >= ci.Diagnostic.Fatal or not same_source
            or diagnostic.location.offset <= fn.extent.end.offset
        ):
            blocking.append(str(diagnostic))
        else:
            trailing.append(str(diagnostic))
    return blocking, trailing


def cursor_range(cursor) -> tuple[int, int]:
    return cursor.extent.start.offset, cursor.extent.end.offset


def operator_token(node, left_end: int, right_start: int, accepted: set[str]):
    tokens = [
        token for token in node.get_tokens()
        if left_end <= token.extent.start.offset and token.extent.end.offset <= right_start
        and token.spelling in accepted
    ]
    return tokens[0] if len(tokens) == 1 else None


def has_side_effect(cursor) -> bool:
    for node in cursor.walk_preorder():
        if node.kind in CALL_KINDS or node.kind in (
            ci.CursorKind.CXX_NEW_EXPR, ci.CursorKind.CXX_DELETE_EXPR,
        ):
            return True
        if node.kind == ci.CursorKind.BINARY_OPERATOR:
            children = list(node.get_children())
            if len(children) == 2:
                token = operator_token(
                    node, children[0].extent.end.offset, children[1].extent.start.offset,
                    ASSIGNMENTS,
                )
                if token is not None:
                    return True
        if node.kind == ci.CursorKind.UNARY_OPERATOR:
            spellings = {token.spelling for token in node.get_tokens()}
            if spellings & {"++", "--"}:
                return True
        if node.kind in (ci.CursorKind.DECL_REF_EXPR, ci.CursorKind.MEMBER_REF_EXPR) \
                and type_is_volatile(node.type):
            return True
    return False


def type_is_volatile(type_) -> bool:
    spellings = (type_.spelling, type_.get_canonical().spelling)
    return any(
        "volatile" in spelling.replace("*", " ").replace("&", " ").split()
        for spelling in spellings
    )


def canonical_kind(type_):
    return type_.get_canonical().kind


def is_integral_or_enum(type_) -> bool:
    return canonical_kind(type_) in INTEGRAL_TYPE_KINDS | {ci.TypeKind.ENUM}


def is_value_type(type_) -> bool:
    return (
        not type_is_volatile(type_)
        and canonical_kind(type_) in INTEGRAL_TYPE_KINDS | FLOAT_TYPE_KINDS
        | {ci.TypeKind.ENUM, ci.TypeKind.POINTER}
    )


def operator_is_value_neutral(operator: str, left, right) -> bool:
    if not is_value_type(left.type) or not is_value_type(right.type):
        return False
    if operator in ("==", "!=") or operator in RELATIONAL_FLIP:
        return True
    if operator in ("&", "|", "^", "*"):
        return is_integral_or_enum(left.type) and is_integral_or_enum(right.type)
    if operator == "+":
        left_pointer = canonical_kind(left.type) == ci.TypeKind.POINTER
        right_pointer = canonical_kind(right.type) == ci.TypeKind.POINTER
        return (
            (is_integral_or_enum(left.type) and is_integral_or_enum(right.type))
            or (left_pointer and is_integral_or_enum(right.type))
            or (right_pointer and is_integral_or_enum(left.type))
        )
    return False


def contains_context_sensitive_tokens(text: bytes) -> bool:
    return any(token in text for token in CONTEXT_SENSITIVE_TOKENS)


def line_number(blob: bytes, offset: int) -> int:
    return blob.count(b"\n", 0, offset) + 1


def utf8_byte_offset(text: str, character_offset: int) -> int:
    return len(text[:character_offset].encode("utf-8"))


def expression_edits(fn, blob: bytes) -> list[AstMutation]:
    edits = []
    for node in fn.walk_preorder():
        if node.kind != ci.CursorKind.BINARY_OPERATOR:
            continue
        children = list(node.get_children())
        if len(children) != 2 or has_side_effect(children[0]) or has_side_effect(children[1]):
            continue
        left, right = children
        left_start, left_end = cursor_range(left)
        right_start, right_end = cursor_range(right)
        if not left_start < left_end <= right_start < right_end:
            continue
        accepted = COMMUTATIVE | set(RELATIONAL_FLIP)
        token = operator_token(node, left_end, right_start, accepted)
        if token is None:
            continue
        operator = token.spelling
        left_text = blob[left_start:left_end]
        right_text = blob[right_start:right_end]
        if (
            not operator_is_value_neutral(operator, left, right)
            or contains_context_sensitive_tokens(left_text + right_text)
        ):
            continue
        if operator in COMMUTATIVE and left_text == right_text:
            continue
        middle = bytearray(blob[left_end:right_start])
        if operator in RELATIONAL_FLIP:
            token_start = token.extent.start.offset - left_end
            token_end = token.extent.end.offset - left_end
            middle[token_start:token_end] = RELATIONAL_FLIP[operator].encode()
            family = "relational_order"
        elif operator in COMMUTATIVE:
            family = "commutative_order"
        else:
            continue
        replacement = right_text + bytes(middle) + left_text
        edits.append(AstMutation(
            family,
            f"line-{line_number(blob, left_start)}-{operator}",
            (AstEdit(left_start, right_end, replacement),),
        ))
    return edits


def assignment_info(stmt, fn):
    if stmt.kind != ci.CursorKind.BINARY_OPERATOR:
        return None
    children = list(stmt.get_children())
    if len(children) != 2:
        return None
    left, right = children
    token = operator_token(stmt, left.extent.end.offset, right.extent.start.offset, {"="})
    if token is None or has_side_effect(right):
        return None
    direct_left = unwrap_single_expression(left)
    if direct_left.kind != ci.CursorKind.DECL_REF_EXPR:
        return None
    declaration = direct_left.referenced
    if declaration is None or declaration.kind != ci.CursorKind.VAR_DECL:
        return None
    if declaration.semantic_parent != fn or not is_value_type(declaration.type):
        return None
    reads = set()
    for node in right.walk_preorder():
        if node.kind == ci.CursorKind.DECL_REF_EXPR and node.referenced is not None:
            reads.add(node.referenced.hash)
    return declaration.hash, reads


def statement_order_edits(fn, blob: bytes) -> list[AstMutation]:
    edits = []
    for compound in fn.walk_preorder():
        if compound.kind != ci.CursorKind.COMPOUND_STMT:
            continue
        statements = list(compound.get_children())
        for first, second in zip(statements, statements[1:]):
            first_info = assignment_info(first, fn)
            second_info = assignment_info(second, fn)
            if first_info is None or second_info is None:
                continue
            first_write, first_reads = first_info
            second_write, second_reads = second_info
            if first_write == second_write or first_write in second_reads or second_write in first_reads:
                continue
            first_start, first_end = cursor_range(first)
            second_start, second_end = cursor_range(second)
            if not first_start < first_end <= second_start < second_end:
                continue
            gap = blob[first_end:second_start]
            if contains_context_sensitive_tokens(blob[first_start:second_end]):
                continue
            replacement = blob[second_start:second_end] + gap + blob[first_start:first_end]
            edits.append(AstMutation(
                "independent_statement_order",
                f"lines-{line_number(blob, first_start)}-{line_number(blob, second_start)}",
                (AstEdit(first_start, second_end, replacement),),
            ))
    return edits


def simple_declaration(cursor, blob: bytes):
    if cursor.kind != ci.CursorKind.DECL_STMT:
        return None
    variables = [child for child in cursor.get_children() if child.kind == ci.CursorKind.VAR_DECL]
    if len(variables) != 1:
        return None
    variable = variables[0]
    start, end = cursor_range(cursor)
    name_start = variable.location.offset
    variable_end = variable.extent.end.offset
    if not start < name_start < variable_end <= end:
        return None
    prefix = blob[start:name_start]
    if b"*" in prefix or b"&" in prefix or b"," in prefix:
        return None
    trailer = blob[variable_end:end]
    if trailer.strip() != b";":
        return None
    return prefix, blob[name_start:variable_end], start, end


def declaration_edits(fn, blob: bytes) -> list[AstMutation]:
    edits = []
    for compound in fn.walk_preorder():
        if compound.kind != ci.CursorKind.COMPOUND_STMT:
            continue
        statements = list(compound.get_children())
        for statement in statements:
            if statement.kind != ci.CursorKind.DECL_STMT:
                continue
            variables = [child for child in statement.get_children() if child.kind == ci.CursorKind.VAR_DECL]
            if len(variables) < 2:
                continue
            start, end = cursor_range(statement)
            if b"__LINE__" in blob[start:end]:
                continue
            prefix = blob[start:variables[0].location.offset]
            if b"*" in prefix or b"&" in prefix or b"," in prefix:
                continue
            declarators = []
            valid = True
            for variable in variables:
                name_start = variable.location.offset
                variable_end = variable.extent.end.offset
                if not start < name_start < variable_end <= end or variable.type.spelling != variables[0].type.spelling:
                    valid = False
                    break
                declarators.append(blob[name_start:variable_end])
            if not valid:
                continue
            tokens = list(statement.get_tokens())
            comma_count = sum(token.spelling == "," for token in tokens)
            if comma_count != len(variables) - 1:
                continue
            line_start = blob.rfind(b"\n", 0, start) + 1
            indent = blob[line_start:start]
            replacement = (b";\n" + indent).join(prefix + declarator for declarator in declarators) + b";"
            edits.append(AstMutation(
                "declaration_split",
                f"line-{line_number(blob, start)}-{len(variables)}",
                (AstEdit(start, end, replacement),),
            ))
        for first, second in zip(statements, statements[1:]):
            first_decl = simple_declaration(first, blob)
            second_decl = simple_declaration(second, blob)
            if first_decl is None or second_decl is None:
                continue
            first_prefix, first_value, first_start, first_end = first_decl
            second_prefix, second_value, second_start, second_end = second_decl
            if first_prefix != second_prefix or blob[first_end:second_start].strip():
                continue
            if b"__LINE__" in blob[first_start:second_end]:
                continue
            replacement = first_prefix + first_value + b", " + second_value + b";"
            edits.append(AstMutation(
                "declaration_merge",
                f"lines-{line_number(blob, first_start)}-{line_number(blob, second_start)}",
                (AstEdit(first_start, second_end, replacement),),
            ))
    return edits


def usable_type_spelling(spelling: str) -> bool:
    return bool(spelling) and not any(piece in spelling for piece in ("(anonymous", "{", "[", ")("))


def msvc42_type_spelling(spelling: str) -> str:
    # Retail's compiler predates built-in bool support; comparisons are represented as int.
    return "int" if spelling == "bool" else spelling


def helper_parameter_spelling(type_):
    spelling = msvc42_type_spelling(type_.spelling)
    if not usable_type_spelling(spelling) or type_is_volatile(type_):
        return None
    kind = canonical_kind(type_)
    if kind in INTEGRAL_TYPE_KINDS | FLOAT_TYPE_KINDS | {ci.TypeKind.ENUM, ci.TypeKind.POINTER}:
        return spelling
    if kind == ci.TypeKind.LVALUEREFERENCE:
        pointee = type_.get_pointee()
        pointee_kind = canonical_kind(pointee)
        if (
            not type_is_volatile(pointee)
            and pointee_kind in INTEGRAL_TYPE_KINDS | FLOAT_TYPE_KINDS
            | {ci.TypeKind.ENUM, ci.TypeKind.POINTER, ci.TypeKind.RECORD}
        ):
            return spelling
        return None
    if kind == ci.TypeKind.RECORD:
        return (spelling if spelling.startswith("const ") else "const " + spelling) + " &"
    return None


def helper_return_spelling(type_):
    if not is_value_type(type_):
        return None
    spelling = msvc42_type_spelling(type_.spelling)
    return spelling if usable_type_spelling(spelling) else None


def helper_parameters(node, fn):
    declarations = {}
    spellings = {}
    for ref in node.walk_preorder():
        if ref.kind == ci.CursorKind.CXX_THIS_EXPR or (
            ref.kind == ci.CursorKind.MEMBER_REF_EXPR and not list(ref.get_children())
        ):
            return None
        if ref.kind != ci.CursorKind.DECL_REF_EXPR or ref.referenced is None:
            continue
        declaration = ref.referenced
        if type_is_volatile(declaration.type):
            return None
        is_parameter = declaration.kind == ci.CursorKind.PARM_DECL
        is_local = (
            declaration.kind == ci.CursorKind.VAR_DECL
            and declaration.semantic_parent is not None
            and declaration.semantic_parent.hash == fn.hash
        )
        if not (is_parameter or is_local):
            continue
        name = declaration.spelling
        type_spelling = helper_parameter_spelling(declaration.type)
        if not name or type_spelling is None:
            return None
        if name in spellings and spellings[name] != declaration.hash:
            return None
        spellings[name] = declaration.hash
        declarations[declaration.hash] = (declaration.location.offset, type_spelling, name)
    return [(type_spelling, name) for _offset, type_spelling, name in sorted(declarations.values())]


def member_requires_lvalue(node, fn, parents) -> bool:
    """Reject contexts where replacing a member lvalue with a returned value changes identity."""
    current = node
    while current.hash in parents:
        parent = parents[current.hash]
        if parent.kind in CALL_KINDS:
            return True
        if parent.kind == ci.CursorKind.UNARY_OPERATOR:
            spellings = {token.spelling for token in parent.get_tokens()}
            if spellings & {"&", "++", "--"}:
                return True
        if parent.kind == ci.CursorKind.BINARY_OPERATOR:
            children = list(parent.get_children())
            if len(children) == 2:
                token = operator_token(
                    parent, children[0].extent.end.offset, children[1].extent.start.offset,
                    ASSIGNMENTS,
                )
                if (
                    token is not None
                    and children[0].extent.start.offset <= current.extent.start.offset
                    and current.extent.end.offset <= children[0].extent.end.offset
                ):
                    return True
        if parent.kind == ci.CursorKind.VAR_DECL \
                and canonical_kind(parent.type) in REFERENCE_TYPE_KINDS:
            return True
        if parent.kind == ci.CursorKind.RETURN_STMT \
                and canonical_kind(fn.result_type) in REFERENCE_TYPE_KINDS:
            return True
        if parent.kind in (
            ci.CursorKind.CXX_STATIC_CAST_EXPR,
            ci.CursorKind.CXX_REINTERPRET_CAST_EXPR,
            ci.CursorKind.CXX_CONST_CAST_EXPR,
            ci.CursorKind.CXX_FUNCTIONAL_CAST_EXPR,
        ) and canonical_kind(parent.type) in REFERENCE_TYPE_KINDS:
            return True
        current = parent
    return False


def inline_expression_edits(fn, blob: bytes, insertion: int, helper_name_count: int) -> list[AstMutation]:
    mutations = []
    accepted = COMMUTATIVE | set(RELATIONAL_FLIP)
    expression_index = 0
    for node in fn.walk_preorder():
        if node.kind != ci.CursorKind.BINARY_OPERATOR or has_side_effect(node):
            continue
        children = list(node.get_children())
        if len(children) != 2:
            continue
        left_start, left_end = cursor_range(children[0])
        right_start, right_end = cursor_range(children[1])
        if not left_start < left_end <= right_start < right_end:
            continue
        token = operator_token(node, left_end, right_start, accepted)
        if token is None or not operator_is_value_neutral(token.spelling, children[0], children[1]):
            continue
        start, end = cursor_range(node)
        expression = blob[start:end]
        if len(expression) > 160 or contains_context_sensitive_tokens(expression):
            continue
        return_type = helper_return_spelling(node.type)
        parameters = helper_parameters(node, fn)
        if parameters is None or len(parameters) > 4 or return_type is None:
            continue
        parameter_text = ", ".join(f"{type_spelling} {name}" for type_spelling, name in parameters)
        argument_text = ", ".join(name for _type_spelling, name in parameters)
        for name_index in range(helper_name_count):
            helper_name = f"H2AstInline{expression_index:03d}_{name_index}"
            helper = (
                f"static inline {return_type} {helper_name}({parameter_text})\n"
                "{\n"
                f"    return {expression.decode('utf-8')};\n"
                "}\n\n"
            ).encode()
            replacement = f"{helper_name}({argument_text})".encode()
            mutations.append(AstMutation(
                "inline_expression",
                f"line-{line_number(blob, start)}-{token.spelling}-{name_index}",
                (AstEdit(insertion, insertion, helper), AstEdit(start, end, replacement)),
            ))
        expression_index += 1
    return mutations


def inline_member_access_edits(
    fn, blob: bytes, insertion: int, helper_name_count: int
) -> list[AstMutation]:
    mutations = []
    member_index = 0
    parents = {
        child.hash: parent
        for parent in fn.walk_preorder()
        for child in parent.get_children()
    }
    for node in fn.walk_preorder():
        if (
            node.kind != ci.CursorKind.MEMBER_REF_EXPR or has_side_effect(node)
            or member_requires_lvalue(node, fn, parents)
        ):
            continue
        start, end = cursor_range(node)
        expression = blob[start:end]
        return_type = helper_return_spelling(node.type)
        parameters = helper_parameters(node, fn)
        if (
            not expression or len(expression) > 160 or parameters is None or len(parameters) > 4
            or return_type is None or contains_context_sensitive_tokens(expression)
        ):
            continue
        parameter_text = ", ".join(
            f"{type_spelling} {name}" for type_spelling, name in parameters
        )
        argument_text = ", ".join(name for _type_spelling, name in parameters)
        for name_index in range(helper_name_count):
            helper_name = f"H2AstMember{member_index:03d}_{name_index}"
            helper = (
                f"static inline {return_type} {helper_name}({parameter_text})\n"
                "{\n"
                f"    return {expression.decode('utf-8')};\n"
                "}\n\n"
            ).encode()
            replacement = f"{helper_name}({argument_text})".encode()
            mutations.append(AstMutation(
                "inline_member_access",
                f"line-{line_number(blob, start)}-{node.spelling}-{name_index}",
                (AstEdit(insertion, insertion, helper), AstEdit(start, end, replacement)),
            ))
        member_index += 1
    return mutations


def inline_nested_expression_edits(
    fn, blob: bytes, insertion: int, helper_name_count: int
) -> list[AstMutation]:
    mutations = []
    accepted = COMMUTATIVE | set(RELATIONAL_FLIP)
    chain_index = 0
    for outer in fn.walk_preorder():
        if outer.kind != ci.CursorKind.BINARY_OPERATOR or has_side_effect(outer):
            continue
        outer_children = list(outer.get_children())
        if len(outer_children) != 2:
            continue
        outer_left_start, outer_left_end = cursor_range(outer_children[0])
        outer_right_start, outer_right_end = cursor_range(outer_children[1])
        outer_token = operator_token(outer, outer_left_end, outer_right_start, accepted)
        if (
            outer_token is None
            or not operator_is_value_neutral(outer_token.spelling, outer_children[0], outer_children[1])
        ):
            continue
        outer_start, outer_end = cursor_range(outer)
        outer_parameters = helper_parameters(outer, fn)
        outer_return_type = helper_return_spelling(outer.type)
        if (
            outer_parameters is None or len(outer_parameters) > 4
            or outer_return_type is None or outer_end - outer_start > 220
            or contains_context_sensitive_tokens(blob[outer_start:outer_end])
        ):
            continue
        for raw_inner in outer_children:
            inner = unwrap_single_expression(raw_inner)
            if inner.kind != ci.CursorKind.BINARY_OPERATOR or has_side_effect(inner):
                continue
            inner_children = list(inner.get_children())
            if len(inner_children) != 2:
                continue
            inner_left_start, inner_left_end = cursor_range(inner_children[0])
            inner_right_start, inner_right_end = cursor_range(inner_children[1])
            inner_token = operator_token(inner, inner_left_end, inner_right_start, accepted)
            if (
                inner_token is None
                or not operator_is_value_neutral(
                    inner_token.spelling, inner_children[0], inner_children[1]
                )
            ):
                continue
            inner_start, inner_end = cursor_range(inner)
            if not outer_start <= inner_start < inner_end <= outer_end:
                continue
            inner_parameters = helper_parameters(inner, fn)
            inner_return_type = helper_return_spelling(inner.type)
            if (
                inner_parameters is None or len(inner_parameters) > 4
                or inner_return_type is None
            ):
                continue
            outer_parameter_text = ", ".join(
                f"{type_spelling} {name}" for type_spelling, name in outer_parameters
            )
            outer_argument_text = ", ".join(name for _type_spelling, name in outer_parameters)
            inner_parameter_text = ", ".join(
                f"{type_spelling} {name}" for type_spelling, name in inner_parameters
            )
            inner_argument_text = ", ".join(name for _type_spelling, name in inner_parameters)
            outer_expression = blob[outer_start:outer_end]
            inner_expression = blob[inner_start:inner_end]
            for name_index in range(helper_name_count):
                inner_name = f"H2AstNestedInner{chain_index:03d}_{name_index}"
                outer_name = f"H2AstNestedOuter{chain_index:03d}_{name_index}"
                inner_call = f"{inner_name}({inner_argument_text})".encode()
                relative_start = inner_start - outer_start
                relative_end = inner_end - outer_start
                rewritten_outer = (
                    outer_expression[:relative_start] + inner_call + outer_expression[relative_end:]
                )
                helpers = (
                    f"static inline {inner_return_type} {inner_name}({inner_parameter_text})\n"
                    "{\n"
                    f"    return {inner_expression.decode('utf-8')};\n"
                    "}\n\n"
                    f"static inline {outer_return_type} {outer_name}({outer_parameter_text})\n"
                    "{\n"
                    f"    return {rewritten_outer.decode('utf-8')};\n"
                    "}\n\n"
                ).encode()
                replacement = f"{outer_name}({outer_argument_text})".encode()
                mutations.append(AstMutation(
                    "inline_nested_expression",
                    f"line-{line_number(blob, outer_start)}-{name_index}",
                    (AstEdit(insertion, insertion, helpers), AstEdit(outer_start, outer_end, replacement)),
                ))
            chain_index += 1
    return mutations


def direct_assignment(stmt, accepted: set[str]):
    if stmt.kind != ci.CursorKind.BINARY_OPERATOR:
        return None
    children = list(stmt.get_children())
    if len(children) != 2:
        return None
    left, right = children
    token = operator_token(stmt, left.extent.end.offset, right.extent.start.offset, accepted)
    return (left, right, token) if token is not None else None


def referenced_variable(node):
    refs = [cursor for cursor in node.walk_preorder() if cursor.kind == ci.CursorKind.DECL_REF_EXPR]
    return refs[0].referenced if len(refs) == 1 else None


def unwrap_single_expression(node):
    while node.kind in (ci.CursorKind.UNEXPOSED_EXPR, ci.CursorKind.PAREN_EXPR):
        children = list(node.get_children())
        if len(children) != 1:
            break
        node = children[0]
    return node


def inline_read_advance_edits(fn, blob: bytes, insertion: int, helper_name_count: int) -> list[AstMutation]:
    mutations = []
    idiom_index = 0
    for compound in fn.walk_preorder():
        if compound.kind != ci.CursorKind.COMPOUND_STMT:
            continue
        statements = list(compound.get_children())
        for first, second in zip(statements, statements[1:]):
            first_assignment = direct_assignment(first, {"="})
            second_assignment = direct_assignment(second, {"="})
            if first_assignment is None or second_assignment is None:
                continue
            destination, read_expr, _first_token = first_assignment
            pointer_lhs, advance_expr, _second_token = second_assignment
            direct_pointer_lhs = unwrap_single_expression(pointer_lhs)
            if direct_pointer_lhs.kind != ci.CursorKind.DECL_REF_EXPR:
                continue
            pointer_decl = direct_pointer_lhs.referenced
            read_refs = [
                cursor for cursor in read_expr.walk_preorder()
                if cursor.kind == ci.CursorKind.DECL_REF_EXPR and cursor.referenced is not None
            ]
            if pointer_decl is None or len(read_refs) != 1 or read_refs[0].referenced.hash != pointer_decl.hash:
                continue
            read_expr = unwrap_single_expression(read_expr)
            if read_expr.kind != ci.CursorKind.UNARY_OPERATOR:
                continue
            if "*" not in {token.spelling for token in read_expr.get_tokens()}:
                continue
            advance_children = list(advance_expr.get_children())
            if advance_expr.kind != ci.CursorKind.BINARY_OPERATOR or len(advance_children) != 2:
                continue
            add_token = operator_token(
                advance_expr, advance_children[0].extent.end.offset,
                advance_children[1].extent.start.offset, {"+"},
            )
            if add_token is None:
                continue
            advance_refs = [
                cursor for cursor in advance_expr.walk_preorder()
                if cursor.kind == ci.CursorKind.DECL_REF_EXPR and cursor.referenced is not None
            ]
            integer_tokens = [token.spelling for token in advance_expr.get_tokens() if token.spelling.isdigit()]
            if (
                len(advance_refs) != 1 or advance_refs[0].referenced.hash != pointer_decl.hash
                or integer_tokens != ["1"]
            ):
                continue
            direct_destination = unwrap_single_expression(destination)
            if direct_destination.kind != ci.CursorKind.DECL_REF_EXPR:
                continue
            destination_decl = direct_destination.referenced
            if (
                destination_decl is None or destination_decl.kind != ci.CursorKind.VAR_DECL
                or destination_decl.semantic_parent != fn or destination_decl.hash == pointer_decl.hash
                or not is_value_type(destination_decl.type)
            ):
                continue
            if (
                pointer_decl.kind not in (ci.CursorKind.VAR_DECL, ci.CursorKind.PARM_DECL)
                or pointer_decl.semantic_parent != fn
                or type_is_volatile(pointer_decl.type)
            ):
                continue
            pointer_type = pointer_decl.type
            if pointer_type.kind != ci.TypeKind.POINTER:
                continue
            pointer_spelling = pointer_type.spelling
            pointee_type = pointer_type.get_pointee()
            value_spelling = helper_return_spelling(pointee_type)
            if not usable_type_spelling(pointer_spelling) or value_spelling is None:
                continue
            destination_start, destination_end = cursor_range(destination)
            first_start, _first_end = cursor_range(first)
            _second_start, second_end = cursor_range(second)
            destination_text = blob[destination_start:destination_end].decode()
            pointer_name = pointer_decl.spelling
            if not pointer_name:
                continue
            for name_index in range(helper_name_count):
                helper_name = f"H2AstReadAdvance{idiom_index:03d}_{name_index}"
                helper = (
                    f"static inline {value_spelling} {helper_name}({pointer_spelling} &cursor)\n"
                    "{\n"
                    "    return *cursor++;\n"
                    "}\n\n"
                ).encode()
                replacement = f"{destination_text} = {helper_name}({pointer_name})".encode()
                mutations.append(AstMutation(
                    "inline_read_advance",
                    f"lines-{line_number(blob, first_start)}-{line_number(blob, second_end)}-{name_index}",
                    (AstEdit(insertion, insertion, helper), AstEdit(first_start, second_end, replacement)),
                ))
            idiom_index += 1
    return mutations


def atomic_mutations(
    fn, blob: bytes, insertion: int, families: set[str], helper_name_count: int
) -> list[AstMutation]:
    mutations = expression_edits(fn, blob) + statement_order_edits(fn, blob) + declaration_edits(fn, blob)
    if "inline_expression" in families:
        mutations += inline_expression_edits(fn, blob, insertion, helper_name_count)
    if "inline_member_access" in families:
        mutations += inline_member_access_edits(fn, blob, insertion, helper_name_count)
    if "inline_nested_expression" in families:
        mutations += inline_nested_expression_edits(fn, blob, insertion, helper_name_count)
    if "inline_read_advance" in families:
        mutations += inline_read_advance_edits(fn, blob, insertion, helper_name_count)
    unique = {}
    for mutation in mutations:
        if mutation.family not in families:
            continue
        if all(blob[edit.start:edit.end] == edit.replacement for edit in mutation.edits):
            continue
        key = tuple((edit.start, edit.end, edit.replacement) for edit in mutation.edits)
        unique[key] = mutation
    return balance_mutations(unique.values())


def balance_mutations(mutations) -> list[AstMutation]:
    groups = {}
    for mutation in sorted(mutations, key=lambda item: (item.family, item.label)):
        groups.setdefault(mutation.family, []).append(mutation)
    balanced = []
    for index in range(max(map(len, groups.values()), default=0)):
        for family in sorted(groups):
            if index < len(groups[family]):
                balanced.append(groups[family][index])
    return balanced


def tu_state_mutations(
    root: Path, source: Path, rva: int, blob: bytes, count: int,
    families: tuple[str, ...], seed: int,
) -> tuple[list[AstMutation], list[dict]]:
    if count == 0:
        return [], []
    target, _flags = resolve_state_target(root, source, rva)
    decoded = blob.decode("utf-8")
    tokens = target_identifiers(decoded, target)
    # tu_state_noise operates on Python string offsets; manifests and libclang ranges
    # are byte offsets.  Convert explicitly because reconstructed comments contain UTF-8.
    insertion_offset = utf8_byte_offset(decoded, target.insertion_offset)
    mutations = []
    rejected = []
    for variant in make_state_variants(count, families, seed):
        guard = include_macro_guard(root, variant.body, tokens)
        record = {
            "trial": variant.trial,
            "family": variant.family,
            "tag": variant.tag,
            "include_macro_guard": guard,
        }
        if not guard.get("passed", True):
            record["rejected"] = True
            rejected.append(record)
            continue
        body = variant.block(target.logical_line).encode()
        mutations.append(AstMutation(
            f"tu_state_{variant.family}",
            f"trial-{variant.trial}-{variant.tag}",
            (AstEdit(insertion_offset, insertion_offset, body),),
        ))
    return mutations, rejected


def non_overlapping(edits) -> bool:
    ordered = sorted(edits, key=lambda edit: edit.start)
    return all(
        left.end <= right.start
        and not left.start == left.end == right.start == right.end
        for left, right in zip(ordered, ordered[1:])
    )


def merge_insertions(edits) -> tuple[AstEdit, ...]:
    insertions = {}
    replacements = []
    for edit in edits:
        if edit.start == edit.end:
            insertions.setdefault(edit.start, []).append(edit.replacement)
        else:
            replacements.append(edit)
    replacements.extend(
        AstEdit(offset, offset, b"".join(parts)) for offset, parts in insertions.items()
    )
    return tuple(sorted(replacements, key=lambda edit: (edit.start, edit.end)))


def mutation_name(mutation: AstMutation) -> str:
    digest = hashlib.sha256()
    for edit in mutation.edits:
        digest.update(edit.start.to_bytes(8, "little"))
        digest.update(edit.end.to_bytes(8, "little"))
        digest.update(edit.replacement)
    anchor = max(edit.start for edit in mutation.edits)
    return f"{mutation.family}:{mutation.label}@{anchor}-{digest.hexdigest()[:8]}"


def candidate_payloads(
    blob: bytes, mutations: list[AstMutation], max_depth: int, limit: int, min_depth: int = 1,
    required_names: set[str] | None = None,
):
    candidates = []
    required_names = required_names or set()
    for depth in range(min_depth, max_depth + 1):
        for combination in itertools.combinations(mutations, depth):
            combination_names = {mutation_name(mutation) for mutation in combination}
            if not required_names <= combination_names:
                continue
            edits = merge_insertions(
                tuple(edit for mutation in combination for edit in mutation.edits)
            )
            if not non_overlapping(edits):
                continue
            ordered = sorted(edits, key=lambda edit: edit.start)
            candidates.append({
                "name": "+".join(
                    mutation_name(mutation) for mutation in combination
                ),
                "families": sorted({mutation.family for mutation in combination}),
                "edits": [
                    {
                        "start": edit.start,
                        "end": edit.end,
                        "find": blob[edit.start:edit.end].decode("utf-8"),
                        "replace": edit.replacement.decode("utf-8"),
                    }
                    for edit in ordered
                ],
            })
            if len(candidates) >= limit:
                return candidates, True
    return candidates, False


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("rva", type=lambda value: int(value, 0))
    parser.add_argument("--max-depth", type=int, default=3)
    parser.add_argument(
        "--min-depth", type=int, default=1,
        help="minimum number of compatible AST mutations in each emitted candidate",
    )
    parser.add_argument("--limit", type=int, default=4096)
    parser.add_argument(
        "--families",
        default=(
            "commutative_order,relational_order,independent_statement_order,declaration_split,"
            "declaration_merge,inline_expression,inline_read_advance"
            ",inline_nested_expression,inline_member_access"
        ),
        help="comma-separated AST edit families",
    )
    parser.add_argument("-o", "--output", type=Path, required=True)
    parser.add_argument(
        "--helper-name-count", type=int, default=1,
        help="number of deterministic identifier spellings for each generated inline helper",
    )
    parser.add_argument(
        "--require-mutation", action="append", default=[],
        help="require an exact content-fingerprinted mutation name; may be repeated",
    )
    parser.add_argument(
        "--state-trials", type=int, default=0,
        help="number of deterministic parser-visible TU-state mutations to add",
    )
    parser.add_argument(
        "--state-families", default=",".join(DEFAULT_STATE_FAMILIES),
        help="comma-separated TU-state families from tu_state_noise.py",
    )
    parser.add_argument("--state-seed", type=lambda value: int(value, 0), default=0x484F4D32)
    parser.add_argument(
        "--axes-from", type=Path,
        help="copy hand-authored exact-span axes from another schema-1 manifest",
    )
    parser.add_argument("--run", action="store_true", help="compile and score the emitted manifest")
    parser.add_argument("--top", type=int, default=12, help="ranked results printed by --run")
    parser.add_argument("--compile-timeout", type=float, default=120.0)
    parser.add_argument("--batch-output", type=Path, help="artifact directory used by --run")
    args = parser.parse_args(argv)
    if (
        args.min_depth < 1 or args.max_depth < args.min_depth
        or args.limit < 1 or args.helper_name_count < 1 or args.state_trials < 0
        or args.top < 1 or args.compile_timeout <= 0
    ):
        parser.error("require 1 <= --min-depth <= --max-depth and positive limit/name count")

    root = Path(os.environ.get("HOMM2_DIR", Path.cwd())).resolve()
    source = (root / args.source).resolve()
    try:
        source.relative_to(root)
    except ValueError:
        parser.error("source must be inside the worktree")
    families = {family for family in args.families.split(",") if family}
    known_families = {
        "commutative_order", "relational_order", "independent_statement_order",
        "declaration_split", "declaration_merge", "inline_expression", "inline_read_advance",
        "inline_nested_expression", "inline_member_access",
    }
    unknown = families - known_families
    if unknown:
        parser.error("unknown families: " + ", ".join(sorted(unknown)))

    configure_libclang()
    blob = source.read_bytes()
    text = blob.decode("utf-8")
    index = ci.Index.create()
    tu = index.parse(
        str(source),
        args=clang_args(root, source),
        unsaved_files=[(str(source), text)],
        options=ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD,
    )
    try:
        fn = target_function(tu, source, blob, args.rva)
    except ValueError as exc:
        parser.error(str(exc))
    diagnostics, trailing_diagnostics = classify_parse_errors(tu, source, fn)
    if diagnostics:
        parser.error("libclang parse errors:\n" + "\n".join(diagnostics[:20]))
    insertion, _span_end = marker_span(blob, args.rva)
    mutations = atomic_mutations(fn, blob, insertion, families, args.helper_name_count)
    state_families = tuple(
        family.strip() for family in args.state_families.split(",") if family.strip()
    )
    try:
        state_mutations, rejected_state_mutations = tu_state_mutations(
            root, source, args.rva, blob, args.state_trials, state_families, args.state_seed
        )
    except (OSError, KeyError, ValueError) as exc:
        parser.error(str(exc))
    mutations += state_mutations
    mutations = balance_mutations(mutations)
    available_names = {mutation_name(mutation) for mutation in mutations}
    required_names = set(args.require_mutation)
    unknown_required = required_names - available_names
    if unknown_required:
        parser.error("required mutations not generated:\n" + "\n".join(sorted(unknown_required)))
    if len(required_names) > args.max_depth:
        parser.error("number of required mutations exceeds --max-depth")
    candidates, truncated = candidate_payloads(
        blob, mutations, args.max_depth, args.limit, min_depth=args.min_depth,
        required_names=required_names,
    )
    if not candidates:
        parser.error("no AST variants generated")
    payload = {
        "schema": 1,
        "source": str(source.relative_to(root)),
        "rva": f"0x{args.rva:x}",
        "generator": {
            "tool": "scripts/generate_ast_variants.py",
            "families": sorted(families),
            "atomic_mutation_count": len(mutations),
            "max_depth": args.max_depth,
            "min_depth": args.min_depth,
            "limit": args.limit,
            "truncated": truncated,
            "ignored_trailing_diagnostics": trailing_diagnostics,
            "required_mutations": sorted(required_names),
            "tu_state": {
                "trials_requested": args.state_trials,
                "mutations_emitted": len(state_mutations),
                "families": list(state_families),
                "seed": args.state_seed,
                "rejected": rejected_state_mutations,
            },
        },
        "candidates": candidates,
    }
    if args.axes_from is not None:
        try:
            axes_payload = json.loads(args.axes_from.read_text())
        except (OSError, json.JSONDecodeError) as exc:
            parser.error(f"cannot read --axes-from manifest: {exc}")
        if axes_payload.get("schema") != 1 or not isinstance(axes_payload.get("axes"), list):
            parser.error("--axes-from must be a schema-1 manifest containing axes")
        if axes_payload.get("source") != payload["source"]:
            parser.error("--axes-from source does not match generated manifest source")
        axes_rva = axes_payload.get("rva")
        try:
            parsed_axes_rva = int(axes_rva, 0) if isinstance(axes_rva, str) else axes_rva
        except ValueError:
            parsed_axes_rva = None
        if parsed_axes_rva != args.rva:
            parser.error("--axes-from RVA does not match generated manifest RVA")
        payload["axes"] = axes_payload["axes"]
    args.output.write_text(json.dumps(payload, indent=2) + "\n")
    counts = {
        family: sum(mutation.family == family for mutation in mutations)
        for family in sorted(families)
    }
    print(
        f"wrote {len(candidates)} candidates from {len(mutations)} atomic mutations "
        f"({', '.join(f'{name}={count}' for name, count in counts.items())})"
        + ("; limit reached" if truncated else "")
    )
    print(args.output)
    if trailing_diagnostics:
        print(
            f"ignored {len(trailing_diagnostics)} nonfatal parse diagnostic(s) strictly after "
            "the target; recorded in the manifest"
        )
    if args.run:
        from batch_source_variants import main as run_batch

        batch_args = [
            str(args.output), "--limit", str(len(candidates) * max(1, _axis_product(payload))),
            "--top", str(args.top), "--compile-timeout", str(args.compile_timeout),
        ]
        if args.batch_output is not None:
            batch_args.extend(("--output", str(args.batch_output)))
        return run_batch(batch_args)
    return 0


def _axis_product(payload: dict) -> int:
    product = 1
    for axis in payload.get("axes", []):
        product *= len(axis.get("options", []))
    return product


if __name__ == "__main__":
    raise SystemExit(main())
