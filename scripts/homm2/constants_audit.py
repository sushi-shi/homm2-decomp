"""Inventory numeric literals and semantic null/magic-number findings."""

from __future__ import annotations

import csv
import io
import json
import re
import shutil
import subprocess
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

from homm2.constants_syntax import lex, parse_enum_declarations


REPO = next(path for path in Path(__file__).resolve().parents if (path / "flake.nix").exists())
OUTPUT = REPO / "build" / "constants"
DATABASE = REPO / "build" / "clangd" / "compile_commands.json"
REVIEW_MANIFEST = REPO / "config" / "constants_review.tsv"
SOURCE_PATTERN = r"src/(BASE|SOURCE|EDITOR)/.*\.cpp"
ANNOTATION_MACROS = {"DATA", "SIZE", "SYMBOL", "VA", "VAU", "VTBL"}
SOURCE_LINE_ARGUMENTS = {
    "DDSD": 2,
    "DPSD": 2,
    "H2_ALLOC": 1,
    "H2_FREE": 1,
    "H2_ALLOC_AT": 2,
    "H2_FREE_AT": 2,
    "H2_ASSERT": 2,
    "ProcessAssert": 2,
}
SOURCE_LINE_DECLARATION_RE = re.compile(r"\b\w*source_?line\w*\s*=", re.IGNORECASE)
MAGIC_RE = re.compile(
    r"^(?P<path>.*?):(?P<line>\d+):(?P<column>\d+): warning: "
    r"(?P<value>.+?) is a magic number;.*\[readability-magic-numbers\]$"
)
NULL_RE = re.compile(
    r"^(?P<path>.*?):(?P<line>\d+):(?P<column>\d+): warning: use nullptr "
    r"\[modernize-use-nullptr\]$"
)
PROCESS_ERROR_RE = re.compile(r"^Error while processing (?P<path>.*)\.$")
INTEGER_ZERO_RE = re.compile(
    r"(?:0[xX]0+|0[bB]0+|0+)(?:[uU](?:ll|LL|l|L)?|(?:ll|LL|l|L)[uU]?)?"
)


@dataclass(frozen=True)
class Literal:
    path: str
    line: int
    column: int
    token: str
    category: str
    context: str


def source_files() -> list[Path]:
    files = list((REPO / "src").rglob("*.cpp"))
    for tier in ("BASE", "SOURCE", "EDITOR"):
        files.extend((REPO / "include" / tier).rglob("*.h"))
    return sorted(files)


def _column(text: str, offset: int) -> int:
    return offset - text.rfind("\n", 0, offset)


def _brace_kind(tokens, index: int, stack: list[str], statement_start: int) -> str:
    previous = tokens[index - 1].text if index else ""
    if previous == "=" or (stack and stack[-1].endswith("initializer")):
        return "local-initializer" if "function" in stack else "global-initializer"
    if "function" in stack:
        return "function"
    recent = [token.text for token in tokens[statement_start:index]]
    if ")" in recent:
        return "function"
    return "declaration"


def _context_category(stack: list[str]) -> str:
    if "local-initializer" in stack:
        return "local-table"
    if "function" in stack:
        return "code"
    if "global-initializer" in stack:
        return "data-payload"
    return "declaration"


def lexical_inventory(path: Path) -> list[Literal]:
    text = path.read_text(errors="replace")
    lines = text.splitlines()
    tokens = lex(text)
    enum_lines = set()
    for declaration in parse_enum_declarations(path, text):
        enum_lines.update(range(declaration.line, declaration.end_line + 1))

    relative = str(path.relative_to(REPO))
    braces: list[str] = []
    parens: list[list[str | int]] = []
    statement_start = 0
    result = []
    for index, token in enumerate(tokens):
        if token.text == "(":
            parens.append([tokens[index - 1].text if index else "", 0])
        elif token.text == ")" and parens:
            parens.pop()
        elif token.text == "," and parens:
            parens[-1][1] += 1
        elif token.text == "{":
            braces.append(_brace_kind(tokens, index, braces, statement_start))
        elif token.text == "}" and braces:
            braces.pop()
            statement_start = index + 1
        elif token.text == ";" and not any(kind == "function" for kind in braces):
            statement_start = index + 1

        if not token.text[:1].isdigit():
            continue
        line_text = lines[token.line - 1] if token.line <= len(lines) else ""
        if line_text.lstrip().startswith("#"):
            category = "preprocessor"
        elif any(frame[0] in ANNOTATION_MACROS for frame in parens):
            category = "annotation"
        elif any(SOURCE_LINE_ARGUMENTS.get(str(frame[0])) == frame[1] for frame in parens):
            category = "source-line"
        elif SOURCE_LINE_DECLARATION_RE.search(line_text):
            category = "source-line"
        elif token.line in enum_lines:
            category = "enum"
        else:
            category = _context_category(braces)
        result.append(Literal(
            relative,
            token.line,
            _column(text, token.start),
            token.text,
            category,
            line_text.strip().replace("\t", " "),
        ))
    return result


def _literal_lookup(rows: list[Literal]) -> dict[tuple[str, int], list[Literal]]:
    lookup: dict[tuple[str, int], list[Literal]] = {}
    for row in rows:
        lookup.setdefault((row.path, row.line), []).append(row)
    return lookup


def _relative(path: str) -> str:
    candidate = Path(path)
    if candidate.is_absolute():
        return str(candidate.resolve().relative_to(REPO))
    return str(candidate)


def _source_spelling(path: str, line: int, column: int) -> tuple[str, str]:
    source_lines = (REPO / path).read_text(errors="replace").splitlines()
    context = source_lines[line - 1] if 0 < line <= len(source_lines) else ""
    match = re.match(r"(?:[A-Za-z_]\w*|\d[\w.]*)", context[column - 1:])
    return (match.group(0) if match else "", context.strip().replace("\t", " "))


def _diagnostic_rows(log: str, pattern: re.Pattern, lexical: list[Literal]) -> list[dict]:
    lookup = _literal_lookup(lexical)
    result = []
    seen = set()
    for raw in log.splitlines():
        match = pattern.match(raw)
        if not match:
            continue
        data = match.groupdict()
        path = _relative(data["path"])
        line = int(data["line"])
        column = int(data["column"])
        candidates = lookup.get((path, line), [])
        literal = next((item for item in candidates if item.column == column), None)
        if literal is None and candidates:
            literal = min(candidates, key=lambda item: abs(item.column - column))
        spelling, source_context = _source_spelling(path, line, column)
        key = (path, line, column, data.get("value", ""))
        if key in seen:
            continue
        seen.add(key)
        result.append({
            "path": path,
            "line": line,
            "column": column,
            "literal": spelling or (literal.token if literal else data.get("value", "")),
            "category": literal.category if literal else "unknown",
            "context": source_context or (literal.context if literal else ""),
        })
    return sorted(result, key=lambda item: (item["path"], item["line"], item["column"]))


def _unexpected_failures(log: str) -> list[str]:
    failures = []
    for raw in log.splitlines():
        match = PROCESS_ERROR_RE.match(raw)
        if match and not match.group("path").endswith("src/BASE/TILE.cpp"):
            failures.append(match.group("path"))
    return failures


def _is_zero_null_spelling(spelling: str) -> bool:
    return spelling == "false" or INTEGER_ZERO_RE.fullmatch(spelling) is not None


def _run_tidy(check: str, *, config: dict | None = None, jobs: int = 8) -> str:
    runner = shutil.which("run-clang-tidy")
    if not runner:
        raise RuntimeError("run-clang-tidy not found; enter `nix develop .#build`")
    command = [runner, "-j", str(jobs), "-quiet", "-p", str(DATABASE.parent),
               f"-checks=-*,{check}"]
    if config:
        command.extend(["-config", json.dumps(config, separators=(",", ":"))])
    if check == "modernize-use-nullptr":
        command.append(r"-header-filter=.*/include/(BASE|SOURCE|EDITOR)/.*")
    command.append(SOURCE_PATTERN)
    completed = subprocess.run(command, cwd=REPO, text=True, capture_output=True)
    log = completed.stdout + completed.stderr
    failures = _unexpected_failures(log)
    if failures:
        raise RuntimeError("clang-tidy failed to process: " + ", ".join(sorted(failures)))
    return log


def _write_tsv(path: Path, fieldnames: list[str], rows: list[dict]) -> None:
    stream = io.StringIO()
    writer = csv.DictWriter(stream, fieldnames=fieldnames, dialect="excel-tab", lineterminator="\n")
    writer.writeheader()
    writer.writerows(rows)
    path.write_text(stream.getvalue())


def _review_rows(magic: list[dict]) -> list[dict]:
    with REVIEW_MANIFEST.open(newline="") as stream:
        rows = list(csv.DictReader(stream, dialect="excel-tab"))
    expected = {str(path.relative_to(REPO)) for path in source_files()}
    actual = [row.get("path", "") for row in rows]
    duplicates = sorted(path for path, count in Counter(actual).items() if count > 1)
    missing = sorted(expected - set(actual))
    extra = sorted(set(actual) - expected)
    invalid = sorted((row.get("path", ""), row.get("status", "")) for row in rows
                     if row.get("status") not in ("pending", "reviewed", "third-party"))
    if duplicates or missing or extra or invalid:
        raise RuntimeError(
            "invalid constants review manifest: "
            f"duplicates={duplicates} missing={missing} extra={extra} invalid={invalid}"
        )
    actionable = Counter(item["path"] for item in magic
                         if item["category"] in ("code", "local-table", "declaration"))
    premature = sorted(row["path"] for row in rows
                       if row["status"] == "reviewed" and actionable[row["path"]])
    if premature:
        raise RuntimeError("reviewed files still have actionable constants: " + ", ".join(premature))
    return rows


def _summary(lexical: list[Literal], magic: list[dict], null_zero: list[dict],
             review: list[dict]) -> str:
    lexical_categories = Counter(item.category for item in lexical)
    magic_categories = Counter(item["category"] for item in magic)
    pending = {row["path"] for row in review if row["status"] == "pending"}
    by_file = Counter(item["path"] for item in magic
                      if item["path"] in pending
                      and item["category"] in ("code", "local-table", "declaration"))
    reviewed = sum(row["status"] == "reviewed" for row in review)
    third_party = sum(row["status"] == "third-party" for row in review)
    lines = [
        "# Constants audit",
        "",
        "Generated by `homm2 constants`. Scores are not used; this inventory records source",
        "locations and semantic context. `BASE/TILE.cpp` is audited lexically because Clang",
        "cannot parse its MSVC 4.2 naked inline assembly.",
        "",
        f"- Numeric tokens: {len(lexical)}",
        f"- Semantic magic-number findings: {len(magic)}",
        f"- Remaining numeric null-pointer spellings: {len(null_zero)}",
        f"- Files resolved: {reviewed + third_party}/{len(review)}",
        f"- Reconstructed files reviewed: {reviewed}",
        f"- Third-party files retained: {third_party}",
        "",
        "## Numeric tokens by context",
        "",
        "| context | occurrences |",
        "|---|---:|",
    ]
    lines.extend(f"| {name} | {count} |" for name, count in sorted(lexical_categories.items()))
    lines.extend(["", "## Magic-number findings by context", "", "| context | occurrences |",
                  "|---|---:|"])
    lines.extend(f"| {name} | {count} |" for name, count in sorted(magic_categories.items()))
    lines.extend(["", "## Review queue", "", "| file | actionable findings |", "|---|---:|"])
    lines.extend(f"| `{path}` | {count} |" for path, count in by_file.most_common())
    lines.append("")
    return "\n".join(lines)


def run(*, jobs: int = 8, magic_log: Path | None = None,
        null_log: Path | None = None) -> int:
    lexical = [item for path in source_files() for item in lexical_inventory(path)]
    if magic_log is None or null_log is None:
        from homm2.init.clangd import main as generate_database
        generate_database()
    if magic_log is None:
        magic_log_text = _run_tidy("readability-magic-numbers", config={
            "CheckOptions": {
                "readability-magic-numbers.IgnoredIntegerValues": "0;1;",
                "readability-magic-numbers.IgnoredFloatingPointValues": "0.0;1.0;",
            }
        }, jobs=jobs)
    else:
        magic_log_text = magic_log.read_text(errors="replace")
    if null_log is None:
        null_log_text = _run_tidy("modernize-use-nullptr", jobs=jobs)
    else:
        null_log_text = null_log.read_text(errors="replace")

    magic = _diagnostic_rows(magic_log_text, MAGIC_RE, lexical)
    null_rows = _diagnostic_rows(null_log_text, NULL_RE, lexical)
    null_zero = [item for item in null_rows if _is_zero_null_spelling(item["literal"])]
    review = _review_rows(magic)
    OUTPUT.mkdir(parents=True, exist_ok=True)
    (OUTPUT / "magic-numbers.log").write_text(magic_log_text)
    (OUTPUT / "null-pointers.log").write_text(null_log_text)
    _write_tsv(OUTPUT / "literals.tsv",
               ["path", "line", "column", "token", "category", "context"],
               [item.__dict__ for item in lexical])
    _write_tsv(OUTPUT / "magic-numbers.tsv",
               ["path", "line", "column", "literal", "category", "context"], magic)
    _write_tsv(OUTPUT / "null-zero.tsv",
               ["path", "line", "column", "literal", "category", "context"], null_zero)
    (OUTPUT / "README.md").write_text(_summary(lexical, magic, null_zero, review))
    print(f"[constants] numeric={len(lexical)} magic={len(magic)} null-zero={len(null_zero)}")
    print("[constants] wrote build/constants/{README.md,literals.tsv,magic-numbers.tsv,null-zero.tsv}")
    return 1 if null_zero else 0


def main(argv: list[str]) -> int:
    jobs = 8
    magic_log = None
    null_log = None
    index = 0
    while index < len(argv):
        argument = argv[index]
        if argument in ("--jobs", "--magic-log", "--null-log") and index + 1 < len(argv):
            value = argv[index + 1]
            if argument == "--jobs":
                jobs = int(value)
            elif argument == "--magic-log":
                magic_log = Path(value)
            else:
                null_log = Path(value)
            index += 2
            continue
        print("usage: homm2 constants [--jobs N] [--magic-log FILE --null-log FILE]")
        return 1
    if (magic_log is None) != (null_log is None):
        print("--magic-log and --null-log must be supplied together")
        return 1
    return run(jobs=jobs, magic_log=magic_log, null_log=null_log)
