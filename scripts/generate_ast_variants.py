#!/usr/bin/env python3
"""Generate reviewable libclang source variants for batch_source_variants.py.

The generator never compiles or modifies the source file.  It locates one reconstructed
function by its ``VA(...)`` marker, derives conservative AST mutations, combines only
non-overlapping edits, and writes their exact byte ranges and before/after text to JSON.
The batch runner validates those ranges against the unchanged source before compiling.

Generated families are operand order for commutative/relational expressions, reorder of
independent local assignments, split/merge of simple local declarations, extraction of a
pure expression into an inline helper, and extraction of the common read-then-advance cursor
idiom.  Other sequencing changes are intentionally absent: author those as explicit literal
axes after a semantic audit.

Run inside ``nix develop .#build``::

    python3 scripts/generate_ast_variants.py src/BASE/icon2by.cpp 0xda270 \
        --max-depth 3 --limit 4096 -o /tmp/icon2by-ast.json
    python3 scripts/batch_source_variants.py /tmp/icon2by-ast.json
"""

from __future__ import annotations

import argparse
import glob
import itertools
import json
import os
from dataclasses import dataclass
from pathlib import Path

import clang.cindex as ci


COMMUTATIVE = {"+", "*", "==", "!=", "&", "|", "^"}
RELATIONAL_FLIP = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
ASSIGNMENTS = {"=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="}
FUNCTION_KINDS = (
    ci.CursorKind.FUNCTION_DECL,
    ci.CursorKind.CXX_METHOD,
    ci.CursorKind.CONSTRUCTOR,
    ci.CursorKind.DESTRUCTOR,
)


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
    out = ["-x", "c++", "-std=c++14", "-fms-compatibility", "-ferror-limit=0"]
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
        if node.kind in (
            ci.CursorKind.CALL_EXPR,
            ci.CursorKind.CXX_NEW_EXPR,
            ci.CursorKind.CXX_DELETE_EXPR,
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
        if node.kind == ci.CursorKind.DECL_REF_EXPR and "volatile" in node.type.spelling.split():
            return True
    return False


def line_number(blob: bytes, offset: int) -> int:
    return blob.count(b"\n", 0, offset) + 1


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
    refs = [node for node in left.walk_preorder() if node.kind == ci.CursorKind.DECL_REF_EXPR]
    if len(refs) != 1 or any(node.kind == ci.CursorKind.MEMBER_REF_EXPR for node in left.walk_preorder()):
        return None
    declaration = refs[0].referenced
    if declaration is None or declaration.kind != ci.CursorKind.VAR_DECL:
        return None
    if declaration.semantic_parent != fn or "volatile" in declaration.type.spelling.split():
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
            replacement = first_prefix + first_value + b", " + second_value + b";"
            edits.append(AstMutation(
                "declaration_merge",
                f"lines-{line_number(blob, first_start)}-{line_number(blob, second_start)}",
                (AstEdit(first_start, second_end, replacement),),
            ))
    return edits


def usable_type_spelling(spelling: str) -> bool:
    return bool(spelling) and not any(piece in spelling for piece in ("(anonymous", "{", "[", ")("))


def helper_parameters(node, fn):
    declarations = {}
    spellings = {}
    for ref in node.walk_preorder():
        if ref.kind != ci.CursorKind.DECL_REF_EXPR or ref.referenced is None:
            continue
        declaration = ref.referenced
        is_parameter = declaration.kind == ci.CursorKind.PARM_DECL
        is_local = (
            declaration.kind == ci.CursorKind.VAR_DECL
            and declaration.semantic_parent is not None
            and declaration.semantic_parent.hash == fn.hash
        )
        if not (is_parameter or is_local):
            continue
        name = declaration.spelling
        type_spelling = declaration.type.spelling
        if not name or not usable_type_spelling(type_spelling):
            return None
        if name in spellings and spellings[name] != declaration.hash:
            return None
        spellings[name] = declaration.hash
        declarations[declaration.hash] = (declaration.location.offset, type_spelling, name)
    return [(type_spelling, name) for _offset, type_spelling, name in sorted(declarations.values())]


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
        if token is None:
            continue
        start, end = cursor_range(node)
        expression = blob[start:end]
        if len(expression) > 160:
            continue
        return_type = node.type.spelling
        parameters = helper_parameters(node, fn)
        if parameters is None or len(parameters) > 4 or not usable_type_spelling(return_type):
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
            pointer_decl = referenced_variable(pointer_lhs)
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
            pointer_type = pointer_decl.type
            if pointer_type.kind != ci.TypeKind.POINTER:
                continue
            pointer_spelling = pointer_type.spelling
            value_spelling = pointer_type.get_pointee().spelling
            if not usable_type_spelling(pointer_spelling) or not usable_type_spelling(value_spelling):
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
    groups = {}
    for mutation in sorted(unique.values(), key=lambda item: (item.family, item.label)):
        groups.setdefault(mutation.family, []).append(mutation)
    balanced = []
    for index in range(max(map(len, groups.values()), default=0)):
        for family in sorted(groups):
            if index < len(groups[family]):
                balanced.append(groups[family][index])
    return balanced


def non_overlapping(edits) -> bool:
    ordered = sorted(edits, key=lambda edit: edit.start)
    return all(
        left.end <= right.start
        and not left.start == left.end == right.start == right.end
        for left, right in zip(ordered, ordered[1:])
    )


def candidate_payloads(blob: bytes, mutations: list[AstMutation], max_depth: int, limit: int):
    candidates = []
    for depth in range(1, max_depth + 1):
        for combination in itertools.combinations(mutations, depth):
            edits = tuple(edit for mutation in combination for edit in mutation.edits)
            if not non_overlapping(edits):
                continue
            ordered = sorted(edits, key=lambda edit: edit.start)
            candidates.append({
                "name": "+".join(
                    f"{mutation.family}:{mutation.label}@{max(edit.start for edit in mutation.edits)}"
                    for mutation in combination
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
    parser.add_argument("--limit", type=int, default=4096)
    parser.add_argument(
        "--families",
        default=(
            "commutative_order,relational_order,independent_statement_order,declaration_split,"
            "declaration_merge,inline_expression,inline_read_advance"
        ),
        help="comma-separated AST edit families",
    )
    parser.add_argument("-o", "--output", type=Path, required=True)
    parser.add_argument(
        "--helper-name-count", type=int, default=1,
        help="number of deterministic identifier spellings for each generated inline helper",
    )
    args = parser.parse_args(argv)
    if args.max_depth < 1 or args.limit < 1 or args.helper_name_count < 1:
        parser.error("--max-depth, --limit, and --helper-name-count must be positive")

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
    diagnostics = [str(diagnostic) for diagnostic in tu.diagnostics if diagnostic.severity >= 3]
    if diagnostics:
        parser.error("libclang parse errors:\n" + "\n".join(diagnostics[:20]))
    try:
        fn = target_function(tu, source, blob, args.rva)
    except ValueError as exc:
        parser.error(str(exc))
    insertion, _span_end = marker_span(blob, args.rva)
    mutations = atomic_mutations(fn, blob, insertion, families, args.helper_name_count)
    candidates, truncated = candidate_payloads(blob, mutations, args.max_depth, args.limit)
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
            "limit": args.limit,
            "truncated": truncated,
        },
        "candidates": candidates,
    }
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
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
