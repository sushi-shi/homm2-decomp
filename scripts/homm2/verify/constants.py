"""homm2.verify.constants - numeric literals, magic numbers and the open floor.

clang-tidy's readability-magic-numbers (0 and 1 ignored) finds the numeric
literals of every unit; a lexical pass classifies each by context. A finding
in executable code, a local table or a declaration is open until it is spelled
as a name (an enumerator, a named constant, NULL) or a row in
config/constants.tsv keeps it numeric with a reason. Files the per-file
checklist config/reviews/constants.tsv marks `third-party` are outside the
count. The committed `#floor` in config/constants.tsv is the open count and
only goes down.

    homm2 verify constants                 # census, build/constants/
    homm2 verify constants --list EVENTS   # open findings whose file/owner contains EVENTS
    homm2 verify constants --gate          # fail on a 0 spelled for a null pointer,
                                           # open findings above the floor, or
                                           # stale/malformed kept rows
    homm2 verify constants --update-floor  # lower the floor after a batch
"""

from __future__ import annotations

import argparse
import csv
import io
import json
import re
import shutil
import subprocess
import sys
from collections import Counter
from dataclasses import dataclass
from fnmatch import fnmatchcase
from pathlib import Path

from homm2.verify.constants_syntax import lex, parse_enum_declarations


REPO = next(path for path in Path(__file__).resolve().parents if (path / "flake.nix").exists())
OUTPUT = REPO / "build" / "constants"
DATABASE = REPO / "build" / "clangd" / "compile_commands.json"
REVIEW_MANIFEST = REPO / "config" / "reviews" / "constants.tsv"
#: The work list: the committed floor and the constants kept numeric on purpose.
WORKLIST = REPO / "config" / "constants.tsv"
_WORKLIST_FIELDS = ("file", "owner", "spelling", "group", "detail", "reason")
#: Finding categories that enter the cleanup queue (the rest are evidence:
#: annotations, source lines, global payloads, enum values, directives).
ACTIONABLE = ("code", "local-table", "declaration")
SOURCE_PATTERN = r"src/(BASE|SOURCE|EDITOR)/.*\.cpp"
ANNOTATION_MACROS = {
    "DATA", "DATA_COMPGEN", "DATA_COMPGEN_GUARD", "SIZE", "VA", "VA_COMPGEN",
    "VTBL", "VTBL2",
}
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
    owner: str = ""


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
    owner = ""
    owner_depth = -1
    result = []
    for index, token in enumerate(tokens):
        if token.text == "(":
            parens.append([tokens[index - 1].text if index else "", 0])
        elif token.text == ")" and parens:
            parens.pop()
        elif token.text == "," and parens:
            parens[-1][1] += 1
        elif token.text == "{":
            kind = _brace_kind(tokens, index, braces, statement_start)
            if kind == "function" and "function" not in braces:
                owner = _function_name(tokens, statement_start, index)
                owner_depth = len(braces)
            braces.append(kind)
        elif token.text == "}" and braces:
            braces.pop()
            if len(braces) == owner_depth:
                owner, owner_depth = "", -1
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
            owner if "function" in braces or category == "local-table" else "",
        ))
    return result


def _function_name(tokens, start: int, brace: int) -> str:
    """The qualified name before a function body's parameter list
    (`Class::Method`, `Class::~Class`, `Free`): the last named parenthesized
    group before the body, past VA(...) annotations and enum blocks that end
    without a semicolon, and before a constructor's initializer list; ''
    when it is not plain."""
    depth = 0
    candidate = ""
    for opening in range(start, brace):
        text = tokens[opening].text
        if text == ")":
            depth -= 1
            continue
        if depth == 0 and text == ":" and opening > start and tokens[opening - 1].text == ")" \
                and tokens[opening + 1].text != ":":
            break
        if text != "(":
            continue
        depth += 1
        if depth != 1:
            continue
        parts: list[str] = []
        cursor = opening - 1
        while cursor >= start and tokens[cursor].text.isidentifier():
            parts.append(tokens[cursor].text)
            cursor -= 1
            if cursor >= start and tokens[cursor].text == "~":
                parts.append("~")
                cursor -= 1
            if (cursor - 1 >= start and tokens[cursor].text == ":"
                    and tokens[cursor - 1].text == ":"):
                parts.append("::")
                cursor -= 2
                continue
            break
        name = "".join(reversed(parts))
        if name and name not in ANNOTATION_MACROS:
            candidate = name
    return candidate


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
            "owner": literal.owner if literal else "",
        })
    return sorted(result, key=lambda item: (item["path"], item["line"], item["column"]))


DIAGNOSTIC_ERROR_RE = re.compile(r"^(?P<path>/[^:]+):\d+:\d+: error: ")


def _unexpected_failures(log: str) -> list[str]:
    """Files clang-tidy could not process because of an error in OUR code.

    VC6's pre-standard STL headers do not parse as modern C++ (template
    default arguments, iostream manipulators); clang recovers and the game
    cursors survive, as in the source-claim scanner. Errors confined to
    system or vendor headers are tolerated; one in src/ or include/ fails."""
    own = sorted({match.group("path") for match in map(DIAGNOSTIC_ERROR_RE.match,
                                                       log.splitlines())
                  if match and (Path(match.group("path")).resolve().is_relative_to(REPO / "src")
                                or Path(match.group("path")).resolve()
                                .is_relative_to(REPO / "include"))})
    if own:
        return own
    return []


def _is_zero_null_spelling(spelling: str) -> bool:
    return spelling == "false" or INTEGER_ZERO_RE.fullmatch(spelling) is not None


#: The editor's view of the shared units that have editor-only code: the
#: clangd database reads a shared unit as the game (homm2.lsp.compdb).
EDITOR_DATABASE = REPO / "build" / "clangd" / "editor-view" / "compile_commands.json"


def _editor_view_database() -> str | None:
    """Write the editor-view database; return a run-clang-tidy file pattern
    for its units, or None when no shared unit has editor-only code."""
    from homm2.manifest import all_units, clang_image_defines, unit_images
    database = json.loads(DATABASE.read_text())
    shared = {unit["source"] for unit in all_units()
              if len(unit_images(unit)) > 1 and unit["source"].endswith(".cpp")
              and "HOMM2_EDITOR" in (REPO / unit["source"]).read_text(errors="replace")}
    entries = []
    for entry in database:
        if entry["file"] in shared:
            defines = clang_image_defines(REPO / entry["file"], "editor")
            entries.append({**entry, "arguments": [*entry["arguments"], *defines]})
    if not entries:
        return None
    EDITOR_DATABASE.parent.mkdir(parents=True, exist_ok=True)
    EDITOR_DATABASE.write_text(json.dumps(entries, indent=2) + "\n")
    return "(" + "|".join(re.escape(entry["file"]) for entry in entries) + ")$"


def _run_tidy(check: str, *, config: dict | None = None, jobs: int = 8,
              database: Path = DATABASE, pattern: str = SOURCE_PATTERN) -> str:
    runner = shutil.which("run-clang-tidy")
    if not runner:
        raise RuntimeError("run-clang-tidy not found; enter `nix develop .#build`")
    command = [runner, "-j", str(jobs), "-quiet", "-p", str(database.parent),
               f"-checks=-*,{check}"]
    if config:
        command.extend(["-config", json.dumps(config, separators=(",", ":"))])
    if check == "modernize-use-nullptr":
        command.append(r"-header-filter=.*/include/(BASE|SOURCE|EDITOR)/.*")
    command.append(pattern)
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


@dataclass(frozen=True)
class Keep:
    """A config/constants.tsv row: fnmatch globs over a finding's file, owner
    (the enclosing function), spelling, group (category) and detail (the
    source line), then the reason it stays numeric."""
    line: int
    file: str
    owner: str
    spelling: str
    group: str
    detail: str
    reason: str

    def matches(self, item: dict) -> bool:
        return (fnmatchcase(item["path"], self.file)
                and fnmatchcase(item.get("owner", ""), self.owner)
                and fnmatchcase(item["literal"], self.spelling)
                and fnmatchcase(item["category"], self.group)
                and fnmatchcase(item["context"], self.detail))


def load_worklist(path: Path = WORKLIST) -> tuple[list[Keep], int | None, list[str]]:
    """Kept rows, the committed floor of open findings, and format errors."""
    keeps: list[Keep] = []
    floor = None
    errors: list[str] = []
    if not path.is_file():
        return keeps, floor, errors
    for number, text in enumerate(path.read_text().splitlines(), 1):
        if not text.strip():
            continue
        if text.startswith("#"):
            parts = text[1:].split("\t")
            if parts[0].strip() == "floor" and len(parts) == 2:
                floor = int(parts[1])
            continue
        parts = text.split("\t")
        if len(parts) != len(_WORKLIST_FIELDS):
            errors.append(f"{path.name}:{number}: expected "
                          f"{len(_WORKLIST_FIELDS)} tab-separated fields")
            continue
        keep = Keep(number, *parts)
        if not keep.reason.strip() or keep.reason.strip() == "*":
            errors.append(f"{path.name}:{number}: a kept constant needs a reason")
            continue
        keeps.append(keep)
    return keeps, floor, errors


def write_floor(path: Path, floor: int) -> None:
    text = path.read_text() if path.is_file() else ""
    lines = [line for line in text.splitlines() if not line.startswith("#floor")]
    head = [line for line in lines if line.startswith("#")]
    body = [line for line in lines if not line.startswith("#")]
    path.write_text("\n".join(head + [f"#floor\t{floor}"] + body) + "\n")


def open_findings(magic: list[dict], review: list[dict],
                  keeps: list[Keep]) -> tuple[list[dict], list[Keep]]:
    """Actionable findings no row keeps, and the rows that keep nothing."""
    third_party = {row["path"] for row in review if row["status"] == "third-party"}
    used: set[int] = set()
    result = []
    for item in magic:
        if item["category"] not in ACTIONABLE or item["path"] in third_party:
            continue
        keep = next((row for row in keeps if row.matches(item)), None)
        if keep is None:
            result.append(item)
        else:
            used.add(keep.line)
    return result, [row for row in keeps if row.line not in used]


def _image_split(rows: list[dict]) -> Counter:
    """Open findings by program: a unit by its config/units.toml images
    (game, editor, or shared by both), a header by its include/ tier."""
    from homm2.manifest import all_units, unit_images
    unit_images_by_source = {unit["source"]: unit_images(unit) for unit in all_units()}
    counts: Counter = Counter()
    for item in rows:
        images = unit_images_by_source.get(item["path"])
        if images is None:
            key = "editor" if item["path"].startswith("include/EDITOR/") else "game"
        elif len(images) > 1:
            key = "shared"
        else:
            key = images[0]
        counts[key] += 1
    return counts


def _review_rows() -> list[dict]:
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
    return rows


def _premature(review: list[dict], open_rows: list[dict]) -> list[str]:
    """Files checked off as reviewed while they still have open findings."""
    remaining = Counter(item["path"] for item in open_rows)
    return sorted(row["path"] for row in review
                  if row["status"] == "reviewed" and remaining[row["path"]])


def _summary(lexical: list[Literal], magic: list[dict], null_zero: list[dict],
             review: list[dict], open_rows: list[dict], keeps: list[Keep],
             floor: int | None) -> str:
    lexical_categories = Counter(item.category for item in lexical)
    magic_categories = Counter(item["category"] for item in magic)
    by_file = Counter(item["path"] for item in open_rows)
    retained_evidence = magic_categories["data-payload"] + magic_categories["source-line"]
    reviewed = sum(row["status"] == "reviewed" for row in review)
    third_party = sum(row["status"] == "third-party" for row in review)
    split = _image_split(open_rows)
    lines = [
        "# Constants audit",
        "",
        "Generated by `homm2 verify constants`. Scores are not used; this inventory",
        "records source locations and semantic context.",
        "",
        f"- Numeric tokens: {len(lexical)}",
        f"- Clang magic-number diagnostics: {len(magic)}",
        f"- Open findings: {len(open_rows)} (floor {floor if floor is not None else 'unset'}; "
        f"game {split['game']}, shared {split['shared']}, editor {split['editor']})",
        f"- Kept by config/constants.tsv: {len(keeps)} row(s)",
        f"- Retained payload/source evidence findings: {retained_evidence}",
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
    lines.extend(["", "## Review queue", "", "| file | open findings |", "|---|---:|"])
    lines.extend(f"| `{path}` | {count} |" for path, count in by_file.most_common())
    lines.append("")
    return "\n".join(lines)


def run(*, jobs: int = 8, magic_log: Path | None = None, null_log: Path | None = None,
        gate: bool = False, update_floor: bool = False, listing: str | None = None) -> int:
    lexical = [item for path in source_files() for item in lexical_inventory(path)]
    if magic_log is None or null_log is None:
        from homm2.lsp.compdb import main as generate_database
        generate_database()
    # One census covers both programs: editor-only units are in the database
    # with the editor's defines, and the shared units with editor-only code
    # are read a second time as the editor; diagnostics are joined by site.
    editor_view = _editor_view_database() if magic_log is None or null_log is None else None
    views = [(DATABASE, SOURCE_PATTERN)]
    if editor_view is not None:
        views.append((EDITOR_DATABASE, editor_view))
    magic_config = {
        "CheckOptions": {
            "readability-magic-numbers.IgnoredIntegerValues": "0;1;",
            "readability-magic-numbers.IgnoredFloatingPointValues": "0.0;1.0;",
        }
    }
    if magic_log is None:
        magic_log_text = "".join(
            _run_tidy("readability-magic-numbers", config=magic_config, jobs=jobs,
                      database=database, pattern=pattern)
            for database, pattern in views)
    else:
        magic_log_text = magic_log.read_text(errors="replace")
    if null_log is None:
        null_log_text = "".join(
            _run_tidy("modernize-use-nullptr", jobs=jobs, database=database, pattern=pattern)
            for database, pattern in views)
    else:
        null_log_text = null_log.read_text(errors="replace")

    magic = _diagnostic_rows(magic_log_text, MAGIC_RE, lexical)
    null_rows = _diagnostic_rows(null_log_text, NULL_RE, lexical)
    null_zero = [item for item in null_rows if _is_zero_null_spelling(item["literal"])]
    keeps, floor, worklist_errors = load_worklist()
    review = _review_rows()
    open_rows, stale = open_findings(magic, review, keeps)
    premature = _premature(review, open_rows)
    fields = ["path", "line", "column", "literal", "category", "owner", "context"]
    OUTPUT.mkdir(parents=True, exist_ok=True)
    (OUTPUT / "magic-numbers.log").write_text(magic_log_text)
    (OUTPUT / "null-pointers.log").write_text(null_log_text)
    _write_tsv(OUTPUT / "literals.tsv",
               ["path", "line", "column", "token", "category", "owner", "context"],
               [item.__dict__ for item in lexical])
    _write_tsv(OUTPUT / "magic-numbers.tsv", fields, magic)
    _write_tsv(OUTPUT / "null-zero.tsv", fields, null_zero)
    _write_tsv(OUTPUT / "open.tsv", fields, open_rows)
    (OUTPUT / "README.md").write_text(_summary(lexical, magic, null_zero, review, open_rows,
                                               keeps, floor))
    if listing is not None:
        for item in open_rows:
            if listing in item["path"] or listing in item["owner"]:
                print(f"{item['path']}:{item['line']}:{item['column']}\t{item['owner']}\t"
                      f"{item['literal']}\t{item['category']}\t{item['context']}")
    split = _image_split(open_rows)
    print(f"[constants] numeric={len(lexical)} magic={len(magic)} "
          f"null-zero={len(null_zero)}")
    print(f"[constants] {len(open_rows)} open finding(s) (game {split['game']}, shared "
          f"{split['shared']}, editor {split['editor']}); floor "
          f"{floor if floor is not None else 'unset'} "
          f"({WORKLIST.relative_to(REPO)}: {len(keeps)} kept row(s))")
    print("[constants] wrote build/constants/{README.md,literals.tsv,magic-numbers.tsv,"
          "null-zero.tsv,open.tsv}")
    for error in worklist_errors:
        print(f"   {error}")
    for keep in stale:
        print(f"   {WORKLIST.name}:{keep.line}: keeps no constant (stale row)")
    for path in premature:
        print(f"   {REVIEW_MANIFEST.relative_to(REPO)}: {path} is reviewed but has open findings")
    if update_floor:
        if floor is None or len(open_rows) < floor:
            write_floor(WORKLIST, len(open_rows))
            print(f"[constants] floor -> {len(open_rows)}")
        return 0
    failed = []
    if null_zero:
        failed.append(f"{len(null_zero)} null pointer(s) spelled 0")
    if premature:
        failed.append(f"{len(premature)} reviewed file(s) with open findings")
    if floor is not None and len(open_rows) > floor:
        failed.append(f"open findings rose {floor} -> {len(open_rows)}")
    if stale or worklist_errors:
        failed.append(f"{len(stale)} stale and {len(worklist_errors)} malformed "
                      f"work-list row(s)")
    if gate and failed:
        print(f"[constants] FAIL: {'; '.join(failed)}")
        return 1
    return 0


from homm2.core.usage import logged


@logged
def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(prog="homm2 verify constants", description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--jobs", type=int, default=8, help="clang-tidy workers")
    parser.add_argument("--magic-log", type=Path,
                        help="reuse a readability-magic-numbers log (with --null-log)")
    parser.add_argument("--null-log", type=Path,
                        help="reuse a modernize-use-nullptr log (with --magic-log)")
    parser.add_argument("--gate", action="store_true",
                        help="fail on a null pointer spelled 0, a reviewed file with open "
                             "findings, open findings above the floor, or stale rows")
    parser.add_argument("--list", metavar="FILTER", nargs="?", const="",
                        help="print the open findings whose file or owner contains FILTER")
    parser.add_argument("--update-floor", action="store_true",
                        help="lower the committed floor to the current open count")
    args = parser.parse_args(argv)
    if (args.magic_log is None) != (args.null_log is None):
        parser.error("--magic-log and --null-log must be supplied together")
    from homm2.core.paths import job_cap
    return run(jobs=job_cap(args.jobs), magic_log=args.magic_log, null_log=args.null_log,
               gate=args.gate, update_floor=args.update_floor, listing=args.list)


if __name__ == "__main__":
    sys.exit(main())
