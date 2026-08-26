#!/usr/bin/env python3
"""Compare the portable Ironfist scripting surface with pinned upstream.

This audit deliberately has a narrow name: it proves the Lua registrations,
callback names, and statically registered integer constants.  It also records
the size of upstream's replacement-symbol inventory.  It does not claim that
the implementations behind those interfaces are behaviorally equivalent.
"""

import argparse
import ast
import json
import operator
import pathlib
import re
import subprocess
import sys


PINNED_REVISION = "314932011ed5308efb9f35cecc62e8ca638a7375"
UPSTREAM_FUNCS = "src/cpp/shared/scripting/funcs.cpp"
UPSTREAM_CONSTS = "src/cpp/shared/scripting/consts.cpp"
UPSTREAM_IMPORTS = "src/asm/heroes2_imports.inc"
CURRENT_FUNCS = "src/IRONFIST/funcs.cpp"
CURRENT_CONSTS = "src/IRONFIST/consts.cpp"


class AuditError(RuntimeError):
    pass


def strip_cpp_comments(text):
    """Remove C/C++ comments without treating comment markers in strings as code."""
    output = []
    index = 0
    state = "code"
    while index < len(text):
        char = text[index]
        following = text[index + 1] if index + 1 < len(text) else ""
        if state == "code":
            if char == '"':
                state = "string"
                output.append(char)
            elif char == "'":
                state = "character"
                output.append(char)
            elif char == "/" and following == "/":
                state = "line-comment"
                output.extend("  ")
                index += 1
            elif char == "/" and following == "*":
                state = "block-comment"
                output.extend("  ")
                index += 1
            else:
                output.append(char)
        elif state == "string":
            output.append(char)
            if char == "\\" and following:
                output.append(following)
                index += 1
            elif char == '"':
                state = "code"
        elif state == "character":
            output.append(char)
            if char == "\\" and following:
                output.append(following)
                index += 1
            elif char == "'":
                state = "code"
        elif state == "line-comment":
            if char == "\n":
                output.append(char)
                state = "code"
            else:
                output.append(" ")
        else:
            if char == "*" and following == "/":
                output.extend("  ")
                index += 1
                state = "code"
            elif char == "\n":
                output.append(char)
            else:
                output.append(" ")
        index += 1
    return "".join(output)


def run_git(repository, *arguments):
    command = ["git", "-C", str(repository), *arguments]
    result = subprocess.run(command, check=False, text=True, capture_output=True)
    if result.returncode:
        detail = result.stderr.strip() or result.stdout.strip()
        raise AuditError(f"{' '.join(command)}: {detail}")
    return result.stdout


class PinnedTree:
    def __init__(self, repository, revision=PINNED_REVISION):
        self.repository = pathlib.Path(repository)
        self.revision = revision
        resolved = run_git(self.repository, "rev-parse", f"{revision}^{{commit}}").strip()
        if resolved != revision:
            raise AuditError(
                f"{self.repository}: {revision} resolved unexpectedly to {resolved}"
            )

    def read(self, path):
        return run_git(self.repository, "show", f"{self.revision}:{path}")

    def paths(self, *roots):
        listing = run_git(
            self.repository,
            "ls-tree",
            "-r",
            "--name-only",
            self.revision,
            *roots,
        )
        return [line for line in listing.splitlines() if line]


def extract_lua_registration_map(text):
    code = strip_cpp_comments(text)
    return dict(
        re.findall(
            r'\blua_register\s*\(\s*[^,]+,\s*"([^"]+)"\s*,\s*([A-Za-z_]\w*)\s*\)',
            code,
            re.S,
        )
    )


def extract_lua_handlers(text):
    code = strip_cpp_comments(text)
    return set(
        re.findall(
            r"\b(?:static\s+)?(?:int|i32)\s+(l_[A-Za-z_]\w*)\s*\(\s*lua_State\s*\*",
            code,
        )
    )


def extract_lua_handler_bodies(text):
    code = strip_cpp_comments(text)
    bodies = {}
    pattern = re.compile(
        r"\b(?:static\s+)?(?:int|i32)\s+(l_[A-Za-z_]\w*)\s*"
        r"\(\s*lua_State\s*\*[^)]*\)\s*\{"
    )
    for match in pattern.finditer(code):
        opening = code.find("{", match.start())
        closing = matching_brace(code, opening)
        bodies[match.group(1)] = code[opening + 1 : closing]
    return bodies


def extract_callback_names(texts):
    pattern = re.compile(r'"((?:On[A-Z][A-Za-z0-9_]*|GetTooltipText))"')
    names = set()
    for text in texts:
        names.update(pattern.findall(strip_cpp_comments(text)))
    return names


def extract_constant_calls(text):
    code = strip_cpp_comments(text)
    calls = {}
    duplicates = {}
    # Upstream uses its C-style helpers; the integrated branch uses the same
    # operations through names normalized to this repository's C++ style.
    pattern = re.compile(
        r'\b(?:lua_setconst|SetConstant)\s*\(\s*[^,]+,\s*"([^"]+)"\s*,\s*([^;]+?)\s*\)\s*;',
        re.S,
    )
    for name, expression in pattern.findall(code):
        expression = " ".join(expression.split())
        if name in calls:
            duplicates.setdefault(name, [calls[name]]).append(expression)
        else:
            calls[name] = expression
    nil_names = set(
        re.findall(
            r'\b(?:lua_setconst_nil|SetNilConstant)\s*\(\s*[^,]+,\s*"([^"]+)"\s*\)',
            code,
        )
    )
    return calls, nil_names, duplicates


def split_top_level(text, delimiter=","):
    pieces = []
    start = 0
    # Angle brackets are deliberately absent: enum expressions commonly use
    # shifts, and commas inside template arguments do not occur in the integer
    # definitions this parser accepts.
    depths = {"(": 0, "[": 0, "{": 0}
    closing = {")": "(", "]": "[", "}": "{"}
    quote = None
    escaped = False
    for index, char in enumerate(text):
        if quote:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            continue
        if char in ('"', "'"):
            quote = char
            continue
        if char in depths:
            depths[char] += 1
        elif char in closing:
            opener = closing[char]
            if depths[opener]:
                depths[opener] -= 1
        elif char == delimiter and not any(depths.values()):
            pieces.append(text[start:index])
            start = index + 1
    pieces.append(text[start:])
    return pieces


def matching_brace(text, opening):
    depth = 0
    quote = None
    escaped = False
    for index in range(opening, len(text)):
        char = text[index]
        if quote:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == quote:
                quote = None
            continue
        if char in ('"', "'"):
            quote = char
        elif char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return index
    raise AuditError("unterminated enum body")


def collect_integer_definitions(texts):
    """Collect simple preprocessor, const, and enum integer definitions."""
    definitions = {}

    def add(name, expression):
        definitions.setdefault(name, []).append(expression.strip())

    enum_pattern = re.compile(
        r"\benum(?:\s+class)?(?:\s+[A-Za-z_]\w*)?(?:\s*:\s*[^\{]+)?\s*\{"
    )
    for original in texts:
        text = strip_cpp_comments(original)
        for name, expression in re.findall(
            r"(?m)^\s*#\s*define\s+([A-Za-z_]\w*)\s+([^\n]+)$", text
        ):
            add(name, expression)
        for name, expression in re.findall(
            r"\b(?:constexpr\s+|const\s+)(?:signed\s+|unsigned\s+)?"
            r"(?:char|short|int|long|i8|u8|i16|u16|i32|u32)\s+"
            r"([A-Za-z_]\w*)\s*=\s*([^;]+);",
            text,
        ):
            add(name, expression)
        for match in enum_pattern.finditer(text):
            opening = text.find("{", match.start())
            closing = matching_brace(text, opening)
            previous = None
            for raw_entry in split_top_level(text[opening + 1 : closing]):
                entry = raw_entry.strip()
                if not entry:
                    continue
                if "=" in entry:
                    left, expression = entry.split("=", 1)
                    names = re.findall(r"[A-Za-z_]\w*", left)
                    if not names:
                        continue
                    name = names[-1]
                else:
                    names = re.findall(r"[A-Za-z_]\w*", entry)
                    if len(names) != 1:
                        continue
                    name = names[0]
                    expression = "0" if previous is None else f"({previous}) + 1"
                add(name, expression)
                previous = name
    return definitions


BIN_OPS = {
    ast.Add: operator.add,
    ast.Sub: operator.sub,
    ast.Mult: operator.mul,
    ast.Div: operator.floordiv,
    ast.FloorDiv: operator.floordiv,
    ast.Mod: operator.mod,
    ast.LShift: operator.lshift,
    ast.RShift: operator.rshift,
    ast.BitOr: operator.or_,
    ast.BitAnd: operator.and_,
    ast.BitXor: operator.xor,
}
UNARY_OPS = {ast.UAdd: operator.pos, ast.USub: operator.neg, ast.Invert: operator.invert}


def normalize_integer_expression(expression):
    expression = expression.strip()
    expression = re.sub(r"\b(?:true|TRUE)\b", "1", expression)
    expression = re.sub(r"\b(?:false|FALSE)\b", "0", expression)
    expression = re.sub(r"\b(?:[A-Za-z_]\w*::)+([A-Za-z_]\w*)", r"\1", expression)
    previous = None
    while expression != previous:
        previous = expression
        expression = re.sub(r"\bH2EnumIndex\s*\(([^()]+)\)", r"(\1)", expression)
        expression = re.sub(
            r"\bstatic_cast\s*<[^>]+>\s*\(([^()]+)\)", r"(\1)", expression
        )
    expression = re.sub(r"(?i)(0x[0-9a-f]+|\b\d+)(?:u|l)+\b", r"\1", expression)
    return expression


class IntegerResolver:
    def __init__(self, definitions):
        self.definitions = definitions
        self.cache = {}
        self.active = set()

    def resolve_name(self, name):
        if name in self.cache:
            return self.cache[name]
        if name in self.active:
            raise AuditError(f"cyclic integer definition involving {name}")
        choices = self.definitions.get(name)
        if not choices:
            raise AuditError(f"unresolved integer symbol {name}")
        self.active.add(name)
        values = []
        errors = []
        for expression in choices:
            try:
                values.append(self.evaluate(expression))
            except AuditError as error:
                errors.append(str(error))
        self.active.remove(name)
        unique = set(values)
        if len(unique) != 1:
            if not unique:
                detail = "; ".join(errors)
                raise AuditError(f"could not resolve {name}: {detail}")
            raise AuditError(f"ambiguous integer symbol {name}: {sorted(unique)}")
        value = values[0]
        self.cache[name] = value
        return value

    def evaluate(self, expression):
        normalized = normalize_integer_expression(expression)
        try:
            node = ast.parse(normalized, mode="eval").body
        except SyntaxError as error:
            raise AuditError(f"unsupported integer expression {expression!r}") from error
        return self._evaluate_node(node)

    def _evaluate_node(self, node):
        if isinstance(node, ast.Constant) and isinstance(node.value, (int, bool)):
            return int(node.value)
        if isinstance(node, ast.Name):
            return self.resolve_name(node.id)
        if isinstance(node, ast.BinOp) and type(node.op) in BIN_OPS:
            return BIN_OPS[type(node.op)](
                self._evaluate_node(node.left), self._evaluate_node(node.right)
            )
        if isinstance(node, ast.UnaryOp) and type(node.op) in UNARY_OPS:
            return UNARY_OPS[type(node.op)](self._evaluate_node(node.operand))
        raise AuditError(f"unsupported integer syntax: {ast.dump(node)}")


def resolve_constant_values(calls, source_texts):
    resolver = IntegerResolver(collect_integer_definitions(source_texts))
    values = {}
    unresolved = {}
    for name, expression in calls.items():
        try:
            values[name] = resolver.evaluate(expression)
        except AuditError as error:
            unresolved[name] = str(error)
    return values, unresolved


def resolve_lua_return_arities(function_text, source_texts):
    """Resolve the Lua result counts returned by every registered handler."""
    resolver = IntegerResolver(collect_integer_definitions(source_texts))
    arities = {}
    unresolved = {}
    for name, body in extract_lua_handler_bodies(function_text).items():
        values = []
        for expression in re.findall(r"\breturn\s+([^;]+);", body):
            try:
                values.append(resolver.evaluate(expression))
            except AuditError as error:
                unresolved[f"{name}: {expression.strip()}"] = str(error)
        arities[name] = sorted(set(values))
    return arities, unresolved


def compare_sets(expected, actual):
    return {
        "expected_count": len(expected),
        "actual_count": len(actual),
        "missing": sorted(expected - actual),
        "extra": sorted(actual - expected),
    }


def compare_maps(expected, actual):
    shared = expected.keys() & actual.keys()
    return {
        "compared_count": len(shared),
        "mismatches": {
            name: {"upstream": expected[name], "current": actual[name]}
            for name in sorted(shared)
            if expected[name] != actual[name]
        },
    }


def compare_binary_strings(expected, binary):
    # Match complete printable C strings.  A raw ``name + NUL`` substring test
    # would incorrectly accept ``One`` from ``prefixOne\0``.
    strings = {
        match.group(0)[:-1].decode("ascii")
        for match in re.finditer(rb"[\x20-\x7e]+\0", binary)
    }
    present = expected & strings
    return {
        "expected_count": len(expected),
        "present_count": len(present),
        "present": sorted(present),
        "missing": sorted(expected - present),
    }


def audit(
    repository,
    upstream_repository,
    revision=PINNED_REVISION,
    binary_path=None,
):
    repository = pathlib.Path(repository)
    upstream = PinnedTree(upstream_repository, revision)

    upstream_paths = upstream.paths("src/cpp/shared", "src/cpp/editor")
    first_party = [
        path
        for path in upstream_paths
        if "/lib/" not in path and pathlib.PurePosixPath(path).suffix in (".cpp", ".h")
    ]
    upstream_shared = [path for path in first_party if "/shared/" in path]
    upstream_editor = [path for path in first_party if "/editor/" in path]
    upstream_texts = {path: upstream.read(path) for path in upstream_shared}

    current_paths = [
        path
        for root in (repository / "src", repository / "include")
        for path in root.rglob("*")
        if path.suffix in (".cpp", ".h")
    ]
    current_texts = {str(path.relative_to(repository)): path.read_text() for path in current_paths}

    upstream_funcs_text = upstream.read(UPSTREAM_FUNCS)
    current_funcs_text = (repository / CURRENT_FUNCS).read_text()
    upstream_registrations = extract_lua_registration_map(upstream_funcs_text)
    current_registrations = extract_lua_registration_map(current_funcs_text)
    upstream_functions = set(upstream_registrations)
    current_functions = set(current_registrations)
    upstream_handlers = extract_lua_handlers(upstream_funcs_text)
    current_handlers = extract_lua_handlers(current_funcs_text)
    upstream_callbacks = extract_callback_names(upstream_texts.values())
    current_callbacks = extract_callback_names(current_texts.values())

    upstream_calls, upstream_nil, upstream_duplicates = extract_constant_calls(
        upstream.read(UPSTREAM_CONSTS)
    )
    current_calls, current_nil, current_duplicates = extract_constant_calls(
        (repository / CURRENT_CONSTS).read_text()
    )
    upstream_values, upstream_unresolved = resolve_constant_values(
        upstream_calls, upstream_texts.values()
    )
    # The constants TU sees public headers and its own IRONFIST tier.  Private
    # enums in unrelated SOURCE .cpp files can legally reuse names and must not
    # be treated as competing definitions here.
    current_value_sources = [
        text
        for path, text in current_texts.items()
        if path.startswith("include/") or path.startswith("src/IRONFIST/")
    ]
    current_values, current_unresolved = resolve_constant_values(
        current_calls, current_value_sources
    )
    upstream_return_arities, upstream_return_unresolved = resolve_lua_return_arities(
        upstream_funcs_text, upstream_texts.values()
    )
    current_return_arities, current_return_unresolved = resolve_lua_return_arities(
        current_funcs_text, current_texts.values()
    )

    imports_text = upstream.read(UPSTREAM_IMPORTS)
    imports = re.findall(r"(?m)^IMPORT_(\S+)\s*=\s*1\s*$", imports_text)
    clones = re.findall(r"(?m)^(\S+)_clone\s+EQU\s+(\S+)\s*$", imports_text)

    binary_surface = None
    if binary_path is not None:
        binary = pathlib.Path(binary_path).read_bytes()
        binary_surface = {
            "path": str(binary_path),
            "lua_functions": compare_binary_strings(upstream_functions, binary),
            "callbacks": compare_binary_strings(upstream_callbacks, binary),
            "constant_names": compare_binary_strings(
                set(upstream_calls) | upstream_nil, binary
            ),
            "current_only_lua_functions": compare_binary_strings(
                current_functions - upstream_functions, binary
            ),
            "current_only_callbacks": compare_binary_strings(
                current_callbacks - upstream_callbacks, binary
            ),
            "current_only_constant_names": compare_binary_strings(
                (set(current_calls) | current_nil) - (set(upstream_calls) | upstream_nil),
                binary,
            ),
        }

    result = {
        "format": "homm2-ironfist-interface-audit-v1",
        "upstream_revision": revision,
        "upstream_inventory": {
            "first_party_files": len(first_party),
            "shared_files": len(upstream_shared),
            "editor_files": len(upstream_editor),
            "replacement_symbols": len(imports),
            "original_body_clones": len(clones),
        },
        "lua_functions": compare_sets(upstream_functions, current_functions),
        "lua_handlers": compare_sets(upstream_handlers, current_handlers),
        "lua_registration_targets": compare_maps(
            upstream_registrations, current_registrations
        ),
        "lua_return_arities": compare_maps(
            upstream_return_arities, current_return_arities
        ),
        "lua_return_arity_resolution": {
            "upstream_unresolved": upstream_return_unresolved,
            "current_unresolved": current_return_unresolved,
        },
        "callbacks": compare_sets(upstream_callbacks, current_callbacks),
        "constant_names": compare_sets(
            set(upstream_calls) | upstream_nil, set(current_calls) | current_nil
        ),
        "constant_values": compare_maps(upstream_values, current_values),
        "constant_resolution": {
            "upstream_resolved": len(upstream_values),
            "current_resolved": len(current_values),
            "upstream_unresolved": upstream_unresolved,
            "current_unresolved": current_unresolved,
        },
        "duplicate_constant_registrations": {
            "upstream": upstream_duplicates,
            "current": current_duplicates,
        },
        "binary_surface": binary_surface,
        "scope_limits": [
            "dynamic ARTIFACT_* constants depend on loaded artifacts.xml and are not covered",
            "matching names and integer values do not prove function or callback behavior",
            "the replacement-symbol inventory is counted but not mapped by this audit",
        ],
    }
    return result


def failures(result):
    found = []
    for section in ("lua_functions", "lua_handlers", "callbacks", "constant_names"):
        if result[section]["missing"] or result[section]["extra"]:
            found.append(section)
    if result["lua_registration_targets"]["mismatches"]:
        found.append("lua_registration_targets")
    if result["lua_return_arities"]["mismatches"]:
        found.append("lua_return_arities")
    return_resolution = result["lua_return_arity_resolution"]
    if (
        return_resolution["upstream_unresolved"]
        or return_resolution["current_unresolved"]
    ):
        found.append("lua_return_arity_resolution")
    if result["constant_values"]["mismatches"]:
        found.append("constant_values")
    resolution = result["constant_resolution"]
    if resolution["upstream_unresolved"] or resolution["current_unresolved"]:
        found.append("constant_resolution")
    if result["binary_surface"] is not None:
        for section in ("lua_functions", "callbacks", "constant_names"):
            if result["binary_surface"][section]["missing"]:
                found.append(f"binary_{section}")
    return found


def print_human(result):
    inventory = result["upstream_inventory"]
    print(f"upstream: {result['upstream_revision']}")
    print(
        "inventory: "
        f"{inventory['shared_files']} shared + {inventory['editor_files']} editor files, "
        f"{inventory['replacement_symbols']} replacement symbols, "
        f"{inventory['original_body_clones']} original-body clones"
    )
    for section in ("lua_functions", "lua_handlers", "callbacks", "constant_names"):
        comparison = result[section]
        print(
            f"{section.replace('_', ' ')}: "
            f"upstream {comparison['expected_count']}, current {comparison['actual_count']}"
        )
        if comparison["missing"]:
            print(f"  missing: {', '.join(comparison['missing'])}")
        if comparison["extra"]:
            print(f"  extra: {', '.join(comparison['extra'])}")
    registration_targets = result["lua_registration_targets"]
    print(
        "lua registration targets: "
        f"{registration_targets['compared_count']} compared, "
        f"{len(registration_targets['mismatches'])} mismatches"
    )
    for name, mismatch in registration_targets["mismatches"].items():
        print(
            f"  {name}: upstream {mismatch['upstream']}, "
            f"current {mismatch['current']}"
        )
    return_arities = result["lua_return_arities"]
    return_resolution = result["lua_return_arity_resolution"]
    print(
        "lua return arities: "
        f"{return_arities['compared_count']} compared, "
        f"{len(return_arities['mismatches'])} mismatches, "
        f"{len(return_resolution['upstream_unresolved'])} upstream unresolved, "
        f"{len(return_resolution['current_unresolved'])} current unresolved"
    )
    for name, mismatch in return_arities["mismatches"].items():
        print(
            f"  {name}: upstream {mismatch['upstream']}, "
            f"current {mismatch['current']}"
        )
    values = result["constant_values"]
    resolution = result["constant_resolution"]
    print(
        f"constant values: {values['compared_count']} compared, "
        f"{len(values['mismatches'])} mismatches, "
        f"{len(resolution['upstream_unresolved'])} upstream unresolved, "
        f"{len(resolution['current_unresolved'])} current unresolved"
    )
    for name, mismatch in values["mismatches"].items():
        print(f"  {name}: upstream {mismatch['upstream']}, current {mismatch['current']}")
    for side in ("upstream", "current"):
        unresolved = resolution[f"{side}_unresolved"]
        for name, reason in sorted(unresolved.items()):
            print(f"  unresolved {side} {name}: {reason}")
    if result["binary_surface"] is not None:
        binary = result["binary_surface"]
        print(f"binary string surface: {binary['path']}")
        for section in ("lua_functions", "callbacks", "constant_names"):
            comparison = binary[section]
            print(
                f"  {section.replace('_', ' ')}: "
                f"{comparison['present_count']}/{comparison['expected_count']} present"
            )
            if comparison["missing"]:
                print(f"    missing: {', '.join(comparison['missing'])}")
        for section in (
            "current_only_lua_functions",
            "current_only_callbacks",
            "current_only_constant_names",
        ):
            comparison = binary[section]
            print(
                f"  {section.replace('_', ' ')} found: "
                f"{comparison['present_count']}/{comparison['expected_count']}"
            )
            if comparison["present"]:
                print(f"    present: {', '.join(comparison['present'])}")


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--upstream", required=True, help="Project Ironfist Git checkout")
    parser.add_argument(
        "--revision",
        default=PINNED_REVISION,
        help=f"exact upstream commit to audit (default: {PINNED_REVISION})",
    )
    parser.add_argument(
        "--repository",
        default=pathlib.Path(__file__).resolve().parents[1],
        help="portable homm2 checkout (defaults to this script's repository)",
    )
    parser.add_argument(
        "--binary",
        help="optional extracted Ironfist executable whose registered-name strings are audited",
    )
    parser.add_argument("--json", action="store_true", help="write the full JSON report")
    args = parser.parse_args(argv)
    try:
        result = audit(args.repository, args.upstream, args.revision, args.binary)
    except (AuditError, OSError) as error:
        print(f"ironfist-interface-audit: {error}", file=sys.stderr)
        return 2
    if args.json:
        print(json.dumps(result, indent=2, sort_keys=True))
    else:
        print_human(result)
    failed = failures(result)
    if failed:
        print(f"ironfist-interface-audit: failed: {', '.join(failed)}", file=sys.stderr)
        return 1
    if not args.json:
        print("ironfist-interface-audit: passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
