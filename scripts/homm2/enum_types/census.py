"""Deterministic source census for integer-domain declarations and uses."""

from __future__ import annotations

import ast
import json
import re
import tomllib
from dataclasses import asdict, dataclass, field
from pathlib import Path


ROOT = next(p for p in Path(__file__).resolve().parents if (p / "flake.nix").exists())
OUTPUT = ROOT / "build" / "enum-types"
MANIFEST = ROOT / "config" / "integer_domains.toml"
SOURCE_SUFFIXES = {".h", ".hpp", ".cpp", ".c"}
DOMAIN_CATEGORIES = {
    "value",
    "flags",
    "serialized",
    "resource-id",
    "message-id",
    "sentinel",
    "scalar",
    "implementation",
}


@dataclass(frozen=True)
class Token:
    text: str
    line: int
    start: int
    end: int


@dataclass
class Enumerator:
    name: str
    expression: str | None
    value: int | None
    line: int
    uses: list[dict] = field(default_factory=list)
    operations: list[str] = field(default_factory=list)
    storage_locations: list[str] = field(default_factory=list)


@dataclass
class EnumDeclaration:
    name: str
    tag: str | None
    owner: str
    line: int
    end_line: int
    form: str
    enumerators: list[Enumerator]
    signedness: str = "unknown"
    minimum: int | None = None
    maximum: int | None = None
    type_uses: dict[str, list[str]] = field(default_factory=dict)
    dependent_tus: list[str] = field(default_factory=list)
    manifest_status: str = "unclassified"
    manifest_classified_enumerators: list[str] = field(default_factory=list)


def source_files() -> list[Path]:
    files = []
    for root in (ROOT / "include", ROOT / "src"):
        files.extend(p for p in root.rglob("*") if p.suffix in SOURCE_SUFFIXES)
    return sorted(files)


def _lex(text: str) -> list[Token]:
    tokens: list[Token] = []
    i = 0
    line = 1
    two_char = {"<<", ">>", "<=", ">=", "==", "!=", "&&", "||", "++", "--", "->"}
    while i < len(text):
        ch = text[i]
        if ch.isspace():
            line += ch == "\n"
            i += 1
            continue
        if text.startswith("//", i):
            end = text.find("\n", i)
            i = len(text) if end < 0 else end
            continue
        if text.startswith("/*", i):
            end = text.find("*/", i + 2)
            end = len(text) - 2 if end < 0 else end
            line += text.count("\n", i, end + 2)
            i = end + 2
            continue
        start, token_line = i, line
        if ch.isalpha() or ch == "_":
            i += 1
            while i < len(text) and (text[i].isalnum() or text[i] == "_"):
                i += 1
        elif ch.isdigit():
            i += 1
            while i < len(text) and (text[i].isalnum() or text[i] in "._"):
                i += 1
        elif ch in "\"'":
            quote = ch
            i += 1
            while i < len(text):
                if text[i] == "\\":
                    i += 2
                    continue
                if text[i] == quote:
                    i += 1
                    break
                line += text[i] == "\n"
                i += 1
        elif text[i:i + 2] in two_char:
            i += 2
        else:
            i += 1
        tokens.append(Token(text[start:i], token_line, start, i))
    return tokens


def _match(tokens: list[Token], start: int, opening: str, closing: str) -> int:
    depth = 0
    for i in range(start, len(tokens)):
        if tokens[i].text == opening:
            depth += 1
        elif tokens[i].text == closing:
            depth -= 1
            if depth == 0:
                return i
    raise ValueError(f"unclosed {opening} at line {tokens[start].line}")


def _split_enumerators(tokens: list[Token]) -> list[list[Token]]:
    result: list[list[Token]] = []
    current: list[Token] = []
    depth = 0
    for token in tokens:
        if token.text in ("(", "[", "{"):
            depth += 1
        elif token.text in (")", "]", "}"):
            depth -= 1
        if token.text == "," and depth == 0:
            if current:
                result.append(current)
            current = []
        else:
            current.append(token)
    if current:
        result.append(current)
    return result


def _expression(tokens: list[Token]) -> str:
    return " ".join(token.text for token in tokens)


def _parse_body(tokens: list[Token]) -> list[Enumerator]:
    result = []
    for part in _split_enumerators(tokens):
        if not part or not re.match(r"^[A-Za-z_]\w*$", part[0].text):
            continue
        equals = next((i for i, token in enumerate(part) if token.text == "="), None)
        expression = _expression(part[equals + 1:]) if equals is not None else None
        result.append(Enumerator(part[0].text, expression, None, part[0].line))
    return result


def parse_declarations(path: Path, text: str | None = None) -> list[EnumDeclaration]:
    if path.resolve() == (ROOT / "include" / "Ints.h").resolve():
        return []
    text = path.read_text(errors="replace") if text is None else text
    tokens = _lex(text)
    result: list[EnumDeclaration] = []
    i = 0
    owner = str(path.relative_to(ROOT))
    while i < len(tokens):
        token = tokens[i].text
        if token == "typedef" and i + 2 < len(tokens) and tokens[i + 1].text == "enum":
            cursor = i + 2
            tag = None
            if tokens[cursor].text != "{":
                tag = tokens[cursor].text
                cursor += 1
            if cursor >= len(tokens) or tokens[cursor].text != "{":
                i += 1
                continue
            close = _match(tokens, cursor, "{", "}")
            if close + 1 >= len(tokens):
                break
            name = tokens[close + 1].text
            result.append(EnumDeclaration(
                name=name,
                tag=tag,
                owner=owner,
                line=tokens[i].line,
                end_line=tokens[close + 1].line,
                form="typedef-enum",
                enumerators=_parse_body(tokens[cursor + 1:close]),
            ))
            i = close + 2
            continue
        if token == "H2_ENUM_CLASS_BEGIN" and i + 3 < len(tokens) and tokens[i + 1].text == "(":
            end_name = _match(tokens, i + 1, "(", ")")
            name = tokens[i + 2].text
            end = next((j for j in range(end_name + 1, len(tokens))
                        if tokens[j].text == "H2_ENUM_CLASS_END"), None)
            if end is None:
                raise ValueError(f"{owner}:{tokens[i].line}: missing H2_ENUM_CLASS_END")
            result.append(EnumDeclaration(
                name=name,
                tag=name,
                owner=owner,
                line=tokens[i].line,
                end_line=tokens[end].line,
                form="dual-mode",
                enumerators=_parse_body(tokens[end_name + 1:end]),
            ))
            i = end + 1
            continue
        if token == "enum" and (i == 0 or tokens[i - 1].text != "typedef"):
            cursor = i + 1
            tag = None
            if cursor < len(tokens) and tokens[cursor].text != "{":
                tag = tokens[cursor].text
                cursor += 1
            if cursor >= len(tokens) or tokens[cursor].text != "{":
                i += 1
                continue
            close = _match(tokens, cursor, "{", "}")
            enumerators = _parse_body(tokens[cursor + 1:close])
            first_name = enumerators[0].name if enumerators else f"line-{tokens[i].line}"
            result.append(EnumDeclaration(
                name=tag or f"AnonymousEnum@{first_name}",
                tag=tag,
                owner=owner,
                line=tokens[i].line,
                end_line=tokens[close].line,
                form="named-enum" if tag else "anonymous-enum",
                enumerators=enumerators,
            ))
            i = close + 1
            continue
        i += 1
    return result


class _Evaluator(ast.NodeVisitor):
    BINARY = {
        ast.Add: lambda a, b: a + b,
        ast.Sub: lambda a, b: a - b,
        ast.Mult: lambda a, b: a * b,
        ast.FloorDiv: lambda a, b: int(a / b),
        ast.Div: lambda a, b: int(a / b),
        ast.Mod: lambda a, b: a % b,
        ast.LShift: lambda a, b: a << b,
        ast.RShift: lambda a, b: a >> b,
        ast.BitOr: lambda a, b: a | b,
        ast.BitAnd: lambda a, b: a & b,
        ast.BitXor: lambda a, b: a ^ b,
    }
    UNARY = {ast.UAdd: lambda a: a, ast.USub: lambda a: -a, ast.Invert: lambda a: ~a}

    def __init__(self, values: dict[str, int]):
        self.values = values

    def visit_Expression(self, node):
        return self.visit(node.body)

    def visit_Constant(self, node):
        if isinstance(node.value, (int, str)):
            return ord(node.value) if isinstance(node.value, str) and len(node.value) == 1 else node.value
        raise ValueError("unsupported constant")

    def visit_Name(self, node):
        return self.values[node.id]

    def visit_BinOp(self, node):
        return self.BINARY[type(node.op)](self.visit(node.left), self.visit(node.right))

    def visit_UnaryOp(self, node):
        return self.UNARY[type(node.op)](self.visit(node.operand))

    def generic_visit(self, node):
        raise ValueError(f"unsupported expression node {type(node).__name__}")


def _evaluate(expression: str, values: dict[str, int]) -> int:
    cleaned = re.sub(r"\b(0[xX][0-9A-Fa-f]+|\d+)[uUlL]+\b", r"\1", expression)
    cleaned = re.sub(r"(?<!/)/(?!/)", "//", cleaned)
    return int(_Evaluator(values).visit(ast.parse(cleaned, mode="eval")))


def evaluate_declarations(declarations: list[EnumDeclaration]) -> None:
    values: dict[str, int] = {}
    pending = list(declarations)
    for _ in range(len(declarations) + 1):
        progress = False
        for declaration in pending:
            previous = -1
            complete = True
            for enumerator in declaration.enumerators:
                try:
                    value = previous + 1 if enumerator.expression is None else _evaluate(
                        enumerator.expression, values
                    )
                except (KeyError, ValueError, SyntaxError, TypeError, ZeroDivisionError):
                    complete = False
                    continue
                if enumerator.value != value:
                    enumerator.value = value
                    values[enumerator.name] = value
                    progress = True
                previous = value
            if complete:
                numeric = [item.value for item in declaration.enumerators if item.value is not None]
                if numeric:
                    declaration.minimum = min(numeric)
                    declaration.maximum = max(numeric)
                    declaration.signedness = "unsigned" if declaration.maximum > 0x7fffffff else "signed"
        if not progress:
            break


def _include_graph(files: list[Path], texts: dict[Path, str]) -> dict[str, set[str]]:
    graph: dict[str, set[str]] = {}
    include_re = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.MULTILINE)
    by_suffix = {str(path.relative_to(ROOT / "include")): str(path.relative_to(ROOT))
                 for path in files if ROOT / "include" in path.parents}
    for path in files:
        rel = str(path.relative_to(ROOT))
        graph[rel] = {by_suffix[name] for name in include_re.findall(texts[path]) if name in by_suffix}
    return graph


def _dependents(owner: str, graph: dict[str, set[str]]) -> list[str]:
    result = []
    for source in sorted(name for name in graph if name.startswith("src/") and name.endswith(".cpp")):
        seen, pending = set(), [source]
        while pending:
            current = pending.pop()
            if current in seen:
                continue
            seen.add(current)
            pending.extend(graph.get(current, ()))
        if owner in seen:
            result.append(source)
    return result


def _line_context(line: str, name: str) -> str:
    escaped = re.escape(name)
    if re.search(rf"(?:static|reinterpret|const|dynamic)_cast\s*<\s*{escaped}\s*>", line):
        return "cast"
    if re.search(rf"\b{escaped}\b\s+[A-Za-z_]\w*\s*\(", line):
        return "return"
    if "(" in line and ")" in line and re.search(rf"\b{escaped}\b\s+[A-Za-z_]\w*", line):
        return "parameter"
    if re.search(rf"\b{escaped}\b\s+[A-Za-z_]\w*", line):
        return "member-or-local"
    return "other"


def _usage_operations(line: str, name: str) -> list[str]:
    result = []
    if re.search(rf"\b{re.escape(name)}\b\s*(?:<<|>>|[|&^])|(?:<<|>>|[|&^])\s*\b{re.escape(name)}\b", line):
        result.append("bitwise")
    if re.search(rf"\b{re.escape(name)}\b\s*[+*/%-]|[+*/%-]\s*\b{re.escape(name)}\b", line):
        result.append("arithmetic")
    if "[" in line and "]" in line:
        result.append("indexing")
    if re.search(r"\b(?:_read|_write|Read|Write|Serialize|Deserialize)\b", line, re.I):
        result.append("serialization")
    if re.search(r"\b(?:switch|case|message|command|widget|event)\b", line, re.I):
        result.append("message-dispatch")
    if re.search(rf"(?:==|!=|<=|>=|<|>)\s*\b{re.escape(name)}\b|\b{re.escape(name)}\b\s*(?:==|!=|<=|>=|<|>)", line):
        result.append("comparison")
    return sorted(set(result))


def _assignment_target(line: str, name: str) -> str | None:
    match = re.search(rf"([A-Za-z_][\w.>\[\]-]*)\s*=\s*[^;]*\b{re.escape(name)}\b", line)
    return match.group(1) if match else None


def collect_uses(declarations: list[EnumDeclaration], files: list[Path], texts: dict[Path, str]) -> None:
    declaration_ranges = {(item.owner, item.name): range(item.line, item.end_line + 1)
                          for item in declarations}
    enum_by_name = {enumerator.name: (item, enumerator)
                    for item in declarations for enumerator in item.enumerators}
    type_by_name = {item.name: item for item in declarations}
    enum_names = set(enum_by_name)
    type_names = set(type_by_name)
    for path in files:
        rel = str(path.relative_to(ROOT))
        for line_number, line in enumerate(texts[path].splitlines(), 1):
            code_line = line.split("//", 1)[0]
            identifiers = set(re.findall(r"\b[A-Za-z_]\w*\b", code_line))
            for name in identifiers & type_names:
                declaration = type_by_name[name]
                if line_number in declaration_ranges.get((rel, name), ()):
                    continue
                context = _line_context(code_line, name)
                if context != "other":
                    declaration.type_uses.setdefault(context, []).append(f"{rel}:{line_number}")
            for name in identifiers & enum_names:
                declaration, enumerator = enum_by_name[name]
                if declaration.owner == rel and enumerator.line == line_number:
                    continue
                operations = _usage_operations(code_line, name)
                enumerator.uses.append({"location": f"{rel}:{line_number}", "operations": operations})
                enumerator.operations = sorted(set(enumerator.operations) | set(operations))
                target = _assignment_target(code_line, name)
                if target:
                    enumerator.storage_locations.append(f"{rel}:{line_number}:{target}")


def _mixed_constant_group(declaration: EnumDeclaration) -> bool:
    if not declaration.name.endswith("Constant"):
        return False
    classes = set()
    for enumerator in declaration.enumerators:
        name = enumerator.name.upper()
        if any(word in name for word in ("COUNT", "SIZE", "WIDTH", "HEIGHT", "OFFSET", "STRIDE", "DELAY")):
            classes.add("scalar")
        if any(word in name for word in ("MASK", "FLAG", "BIT")):
            classes.add("flags")
        if any(word in name for word in ("COMMAND", "MESSAGE", "EVENT", "WIDGET", "CONTROL")):
            classes.add("message")
        if any(word in name for word in ("FRAME", "ICON", "TILESET", "RESOURCE", "ARTIFACT", "CREATURE")):
            classes.add("resource")
    return len(classes) > 1


def _load_manifest() -> dict:
    return tomllib.loads(MANIFEST.read_text()) if MANIFEST.exists() else {"version": 1, "domain": []}


def _apply_manifest(declarations: list[EnumDeclaration], manifest: dict) -> None:
    entries_by_declaration = {}
    for section in ("domain", "constant_group"):
        for entry in manifest.get(section, []):
            key = (entry.get("declaration"), entry.get("owner"))
            if all(isinstance(value, str) for value in key):
                entries_by_declaration.setdefault(key, []).append(entry)
    for declaration in declarations:
        entries = entries_by_declaration.get((declaration.name, declaration.owner), [])
        source_names = {item.name for item in declaration.enumerators}
        classified_names = {
            name for entry in entries
            for name in (entry.get("enumerators")
                         if isinstance(entry.get("enumerators"), list) else [])
            if name in source_names
        }
        declaration.manifest_classified_enumerators = sorted(classified_names)
        if not entries:
            declaration.manifest_status = "unclassified"
        elif classified_names != source_names:
            declaration.manifest_status = "partial"
        else:
            statuses = {entry.get("status", "reviewed") for entry in entries}
            declaration.manifest_status = statuses.pop() if len(statuses) == 1 else "mixed"


def build_census() -> tuple[list[EnumDeclaration], dict]:
    files = source_files()
    texts = {path: path.read_text(errors="replace") for path in files}
    declarations = [item for path in files for item in parse_declarations(path, texts[path])]
    declarations.sort(key=lambda item: (item.owner, item.line, item.name))
    evaluate_declarations(declarations)
    collect_uses(declarations, files, texts)
    graph = _include_graph(files, texts)
    for declaration in declarations:
        declaration.dependent_tus = _dependents(declaration.owner, graph)
        declaration.type_uses = {key: sorted(set(value)) for key, value in sorted(declaration.type_uses.items())}
        for enumerator in declaration.enumerators:
            enumerator.storage_locations = sorted(set(enumerator.storage_locations))
    manifest = _load_manifest()
    _apply_manifest(declarations, manifest)
    return declarations, manifest


def _json_report(declarations: list[EnumDeclaration]) -> dict:
    constant_only = [item.name for item in declarations if not item.type_uses]
    constant_only_typedefs = [item.name for item in declarations
                              if item.form == "typedef-enum" and not item.type_uses]
    status_counts = {}
    for item in declarations:
        status_counts[item.manifest_status] = status_counts.get(item.manifest_status, 0) + 1
    incomplete = [item for item in declarations
                  if item.manifest_status in ("unclassified", "partial")]
    classified_enumerators = sum(len(item.manifest_classified_enumerators)
                                 for item in declarations)
    total_enumerators = sum(len(item.enumerators) for item in declarations)
    return {
        "schema_version": 1,
        "declaration_count": len(declarations),
        "enumerator_count": sum(len(item.enumerators) for item in declarations),
        "unclassified": [item.name for item in declarations if item.manifest_status == "unclassified"],
        "unclassified_declarations": [
            {"name": item.name, "owner": item.owner, "line": item.line}
            for item in declarations if item.manifest_status == "unclassified"
        ],
        "incomplete_declarations": [
            {"name": item.name, "owner": item.owner, "line": item.line,
             "status": item.manifest_status}
            for item in incomplete
        ],
        "manifest_coverage": {
            "classified": sum(item.manifest_status not in ("unclassified", "partial")
                              for item in declarations),
            "total": len(declarations),
            "classified_enumerators": classified_enumerators,
            "total_enumerators": total_enumerators,
            "status_counts": dict(sorted(status_counts.items())),
        },
        "audits": {
            "constant_only_declarations": constant_only,
            "constant_only_typedef_enums": constant_only_typedefs,
            "mixed_constant_groups": [item.name for item in declarations if _mixed_constant_group(item)],
        },
        "declarations": [asdict(item) for item in declarations],
    }


def _markdown_report(report: dict) -> str:
    lines = [
        "# Integer Domain Census",
        "",
        f"- Declarations: {report['declaration_count']}",
        f"- Enumerators: {report['enumerator_count']}",
        f"- Unclassified: {len(report['unclassified'])}",
        f"- Manifest coverage: {report['manifest_coverage']['classified']}/"
        f"{report['manifest_coverage']['total']}",
        f"- Enumerator coverage: {report['manifest_coverage']['classified_enumerators']}/"
        f"{report['manifest_coverage']['total_enumerators']}",
        f"- Constant-only declarations: {len(report['audits']['constant_only_declarations'])}",
        f"- Constant-only typedef enums: {len(report['audits']['constant_only_typedef_enums'])}",
        f"- Mixed `*Constant` candidates: {len(report['audits']['mixed_constant_groups'])}",
        "",
        "| Domain | Owner | Form | Range | Type uses | TUs | Status |",
        "|---|---|---|---:|---:|---:|---|",
    ]
    for item in report["declarations"]:
        range_text = "?" if item["minimum"] is None else f"{item['minimum']}..{item['maximum']}"
        use_count = sum(len(value) for value in item["type_uses"].values())
        lines.append(
            f"| `{item['name']}` | `{item['owner']}:{item['line']}` | {item['form']} | "
            f"{range_text} | {use_count} | {len(item['dependent_tus'])} | {item['manifest_status']} |"
        )
    lines.append("")
    return "\n".join(lines)


def validate_manifest(declarations: list[EnumDeclaration] | None = None,
                      manifest: dict | None = None, require_complete: bool = False) -> int:
    declarations, loaded = build_census() if declarations is None else (declarations, manifest or _load_manifest())
    manifest = loaded
    errors = []
    if manifest.get("version") != 1:
        errors.append(f"unsupported manifest version {manifest.get('version')!r}")
    known = {(item.name, item.owner): item for item in declarations}
    seen_names = set()
    enumerator_claims = {}
    for section in ("domain", "constant_group"):
        for entry in manifest.get(section, []):
            name = entry.get("name")
            declaration = entry.get("declaration")
            owner = entry.get("owner")
            if name in seen_names:
                errors.append(f"duplicate manifest name {name}")
            seen_names.add(name)
            declaration_key = (declaration, owner)
            source_declaration = known.get(declaration_key)
            if source_declaration is None:
                errors.append(f"{section} {name}: declaration {declaration!r} not found in {owner!r}")
            category = entry.get("category")
            if category not in DOMAIN_CATEGORIES:
                errors.append(f"{section} {name}: invalid category {category!r}")
            if section == "domain" and not entry.get("production_carrier"):
                errors.append(f"domain {name}: missing production_carrier")
            if section == "domain" and not isinstance(entry.get("strict"), bool):
                errors.append(f"domain {name}: strict must be a boolean")
            if section == "domain":
                for field_name in ("interfaces", "members", "storage", "conversions"):
                    if not isinstance(entry.get(field_name), list):
                        errors.append(f"domain {name}: {field_name} must be a list")
            if entry.get("status") not in ("reviewed", "provisional", "deferred"):
                errors.append(f"{section} {name}: invalid status {entry.get('status')!r}")
            enumerators = entry.get("enumerators")
            if not isinstance(enumerators, list) or not enumerators:
                errors.append(f"{section} {name}: enumerators must be a non-empty list")
            elif source_declaration is not None:
                source_names = {item.name for item in source_declaration.enumerators}
                unknown = sorted(set(enumerators) - source_names)
                if unknown:
                    errors.append(f"{section} {name}: unknown enumerators {', '.join(unknown)}")
                if len(enumerators) != len(set(enumerators)):
                    errors.append(f"{section} {name}: duplicate enumerators")
                claims = enumerator_claims.setdefault(declaration_key, set())
                overlap = sorted(claims & set(enumerators))
                if overlap:
                    errors.append(f"{section} {name}: enumerators classified more than once: "
                                  f"{', '.join(overlap)}")
                claims.update(enumerators)
            sentinels = entry.get("sentinels")
            if section == "domain" and not isinstance(sentinels, list):
                errors.append(f"domain {name}: sentinels must be a list")
            elif isinstance(sentinels, list) and isinstance(enumerators, list):
                unknown_sentinels = sorted(set(sentinels) - set(enumerators))
                if unknown_sentinels:
                    errors.append(f"domain {name}: sentinels are not domain enumerators: "
                                  f"{', '.join(unknown_sentinels)}")
            if not entry.get("evidence"):
                errors.append(f"{section} {name}: missing evidence")
    if require_complete:
        missing = []
        for declaration_key, source_declaration in known.items():
            claimed = enumerator_claims.get(declaration_key, set())
            missing.extend((name, declaration_key[0], declaration_key[1])
                           for name in {item.name for item in source_declaration.enumerators} - claimed)
        missing.sort(key=lambda item: (item[2], item[1], item[0]))
        if missing:
            preview = ", ".join(
                f"{enumerator} in {declaration} ({owner})"
                for enumerator, declaration, owner in missing[:20]
            )
            suffix = f"; and {len(missing) - 20} more" if len(missing) > 20 else ""
            errors.append(f"{len(missing)} enumerators remain unclassified: {preview}{suffix}")
    if errors:
        for error in errors:
            print(f"[enum-types] manifest: {error}")
        return 1
    print(f"[enum-types] manifest OK: {len(manifest.get('domain', []))} domains, "
          f"{len(manifest.get('constant_group', []))} constant groups")
    return 0


def run(check_manifest: bool = False, require_complete: bool = False) -> int:
    declarations, manifest = build_census()
    report = _json_report(declarations)
    OUTPUT.mkdir(parents=True, exist_ok=True)
    (OUTPUT / "census.json").write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    (OUTPUT / "census.md").write_text(_markdown_report(report))
    print(f"[enum-types] wrote build/enum-types/census.json and census.md: "
          f"{report['declaration_count']} declarations, {report['enumerator_count']} enumerators")
    print(f"[enum-types] unclassified={len(report['unclassified'])} "
          f"enumerator-coverage={report['manifest_coverage']['classified_enumerators']}/"
          f"{report['manifest_coverage']['total_enumerators']} "
          f"constant-only={len(report['audits']['constant_only_typedef_enums'])} "
          f"mixed-constant={len(report['audits']['mixed_constant_groups'])}")
    return validate_manifest(declarations, manifest, require_complete) if check_manifest else 0
