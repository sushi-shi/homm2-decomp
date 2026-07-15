"""Whole-tree strict enum syntax audit using the generated Clang database."""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import tempfile
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from . import census


ROOT = census.ROOT
OUTPUT = census.OUTPUT
DATABASE = ROOT / "build" / "clangd" / "compile_commands.json"
WARNING_FLAGS = [
    "-Wall",
    "-Wextra",
    "-Wenum-compare",
    "-Wenum-conversion",
    "-Wdeprecated-enum-enum-conversion",
    "-Wswitch-enum",
    "-Wassign-enum",
    "-Werror=enum-compare",
    "-Werror=enum-conversion",
    "-Werror=deprecated-enum-enum-conversion",
]
ENUM_WARNING_FLAGS = {
    "-Wassign-enum",
    "-Wdeprecated-enum-enum-conversion",
    "-Wenum-compare",
    "-Wenum-conversion",
    "-Wswitch",
    "-Wswitch-enum",
}
COMPATIBILITY_FLAGS = [
    "-ferror-limit=0",
    "-Wno-writable-strings",
    "-Wno-microsoft",
    "-Wno-ignored-pragmas",
    "-Wno-pragma-pack",
    "-Wno-nonportable-include-path",
]
DIAGNOSTIC_FLAGS = [
    "-fno-caret-diagnostics",
    "-fno-color-diagnostics",
    "-fdiagnostics-absolute-paths",
]
GNU_DIAGNOSTIC_RE = re.compile(
    r"^(?P<file>.*?):(?P<line>\d+):(?P<column>\d+): "
    r"(?P<severity>fatal error|error|warning|note): (?P<message>.*?)(?: \[(?P<flags>[^]]+)\])?$"
)
MSVC_DIAGNOSTIC_RE = re.compile(
    r"^(?P<file>.*?)\((?P<line>\d+),(?P<column>\d+)\) : "
    r"(?P<severity>fatal error|error|warning|note): (?P<message>.*?)(?: \[(?P<flags>[^]]+)\])?$"
)


def _compiler() -> str:
    clang = os.environ.get("HOMM2_CLANG")
    if clang:
        sibling = Path(clang).with_name("clang-cl")
        if sibling.exists():
            return str(sibling)
    compiler = shutil.which("clang-cl")
    if not compiler:
        raise RuntimeError("clang-cl not found; run inside `nix develop .#build`")
    return compiler


def _ensure_database() -> None:
    # The units manifest and worktree root can change while a generated database
    # still exists. Regeneration is cheap and makes every strict run self-contained.
    from homm2.init.clangd import main as generate_database
    generate_database()


def _entries() -> list[dict]:
    entries = json.loads(DATABASE.read_text())
    if not isinstance(entries, list):
        raise RuntimeError(f"{DATABASE.relative_to(ROOT)} must contain a JSON array")
    normalized = []
    seen = set()
    for entry in entries:
        if not isinstance(entry, dict) or "directory" not in entry or "file" not in entry:
            raise RuntimeError(f"malformed compilation database entry: {entry!r}")
        directory = Path(entry["directory"]).resolve()
        source = (directory / entry["file"]).resolve()
        try:
            relative = str(source.relative_to(ROOT))
        except ValueError as error:
            raise RuntimeError(f"compilation database source is outside this worktree: {source}") from error
        if not source.is_file():
            raise RuntimeError(f"compilation database source does not exist: {relative}")
        if relative in seen:
            raise RuntimeError(f"duplicate compilation database source: {relative}")
        seen.add(relative)
        normalized.append(entry)
    return sorted(normalized, key=lambda entry: str(
        (Path(entry["directory"]) / entry["file"]).resolve().relative_to(ROOT)
    ))


def _clang_flags(flags: list[str]) -> list[str]:
    # clang-cl treats `-Wall` as MSVC `/Wall`, which enables nearly every Clang
    # warning. `/clang:` forces GCC-style Clang option semantics.
    return [f"/clang:{flag}" for flag in flags]


def _probe_warnings(compiler: str) -> None:
    with tempfile.NamedTemporaryFile(suffix=".cpp", mode="w", delete=False) as probe:
        probe.write("int enum_type_probe;\n")
        probe_path = probe.name
    try:
        command = [compiler, "/nologo", "/Zs", probe_path,
                   *_clang_flags(["-Werror=unknown-warning-option", *WARNING_FLAGS,
                                  *DIAGNOSTIC_FLAGS])]
        result = subprocess.run(command, text=True, capture_output=True)
    finally:
        Path(probe_path).unlink(missing_ok=True)
    if result.returncode:
        diagnostics = (result.stdout + result.stderr).strip()
        raise RuntimeError(f"strict warning probe failed:\n{diagnostics}")


def _command(entry: dict, compiler: str) -> list[str]:
    source = str(Path(entry["directory"]) / entry["file"])
    arguments = list(entry.get("arguments", []))[1:]
    filtered = [argument for argument in arguments
                if argument not in ("/c", entry["file"], source)]
    return [compiler, "/nologo", "/Zs", source, *filtered,
            "/DHOMM2_STRICT_ENUM_TYPES",
            *_clang_flags([*COMPATIBILITY_FLAGS, *WARNING_FLAGS, *DIAGNOSTIC_FLAGS])]


def _run_entry(entry: dict, compiler: str) -> dict:
    source = str(Path(entry["directory"]) / entry["file"])
    result = subprocess.run(
        _command(entry, compiler), cwd=entry["directory"], text=True, capture_output=True
    )
    return {
        "source": str(Path(source).resolve().relative_to(ROOT)),
        "returncode": result.returncode,
        "output": result.stdout + result.stderr,
    }


def _is_enum_warning(warning: str | None) -> bool:
    return bool(warning and (warning in ENUM_WARNING_FLAGS or warning.startswith("-Wenum-")))


def _diagnostic_kind(severity: str, warning: str | None, promoted: bool,
                     domain: str | None, message: str) -> str:
    if promoted and _is_enum_warning(warning):
        return "promoted-enum-policy"
    if severity in ("error", "fatal error"):
        if _is_enum_warning(warning) or (domain and re.search(
            r"\b(?:enum|enumeration|convert|conversion|initialize|assign)\b", message, re.I
        )):
            return "hard-enum-conversion"
        return "compatibility-error"
    if _is_enum_warning(warning):
        return "enum-warning"
    return severity


def _is_driver_noise(line: str) -> bool:
    return bool(
        re.match(r"^In file included from .*:$", line)
        or re.match(r"^\d+ warnings?(?: and \d+ errors?)? generated\.$", line)
        or re.match(r"^\d+ errors? generated\.$", line)
    )


def _diagnostics(results: list[dict], domain_names, location_domains: dict | None = None) -> list[dict]:
    name_to_domain = ({name: name for name in domain_names}
                      if isinstance(domain_names, set) else domain_names)
    known_names = set(name_to_domain)
    location_domains = location_domains or {}
    diagnostics = []
    for result in results:
        for raw in result["output"].splitlines():
            match = GNU_DIAGNOSTIC_RE.match(raw) or MSVC_DIAGNOSTIC_RE.match(raw)
            if not match:
                if raw.strip() and not _is_driver_noise(raw.strip()):
                    diagnostics.append({
                        "source": result["source"], "file": result["source"],
                        "line": 0, "column": 0, "severity": "driver",
                        "warning": None, "promoted": False, "domain": None,
                        "kind": "driver",
                        "message": raw.strip(),
                    })
                continue
            data = match.groupdict()
            path = Path(data["file"])
            try:
                filename = str(path.resolve().relative_to(ROOT))
            except ValueError:
                filename = str(path)
            warning_flags = [flag.strip() for flag in (data["flags"] or "").split(",")
                             if flag.strip().startswith("-W") and flag.strip() != "-Werror"]
            message_identifiers = set(re.findall(r"\b[A-Za-z_]\w*\b", data["message"]))
            mentioned = sorted({name_to_domain[name]
                                for name in message_identifiers & known_names})
            if not mentioned:
                location = re.search(r"unnamed enum at (.*?):(\d+):\d+", data["message"])
                if location:
                    enum_path = Path(location.group(1))
                    try:
                        enum_file = str(enum_path.resolve().relative_to(ROOT))
                    except ValueError:
                        enum_file = str(enum_path)
                    domain = location_domains.get((enum_file, int(location.group(2))))
                    mentioned = [domain] if domain else []
            promoted = data["severity"] == "error" and (
                "-Werror" in (data["flags"] or "")
                or _is_enum_warning(warning_flags[-1] if warning_flags else None)
            )
            domain = mentioned[0] if mentioned else None
            diagnostics.append({
                "source": result["source"], "file": filename,
                "line": int(data["line"]), "column": int(data["column"]),
                "severity": data["severity"], "warning": warning_flags[-1] if warning_flags else None,
                "promoted": promoted,
                "domain": domain,
                "kind": _diagnostic_kind(data["severity"],
                                         warning_flags[-1] if warning_flags else None,
                                         promoted, domain, data["message"]),
                "message": data["message"],
            })
    return sorted(diagnostics, key=lambda item: (
        item["file"], item["line"], item["column"], item["severity"], item["message"], item["source"]
    ))


def _domain_summary(diagnostics: list[dict]) -> dict:
    grouped = {}
    for domain in sorted({item["domain"] or "<unclassified>" for item in diagnostics}):
        items = [item for item in diagnostics if (item["domain"] or "<unclassified>") == domain]
        grouped[domain] = {
            "diagnostics": len(items),
            "errors": sum(item["severity"] in ("error", "fatal error") for item in items),
            "warnings": sum(item["severity"] == "warning" for item in items),
            "kinds": dict(sorted(Counter(item["kind"] for item in items).items())),
            "warning_flags": dict(sorted(Counter(
                item["warning"] for item in items if item["warning"]
            ).items())),
        }
    return grouped


def _summary_markdown(report: dict) -> str:
    lines = [
        "# Strict Enum Type Check",
        "",
        f"- Translation units: {report['translation_units']}",
        f"- Failed translation units: {len(report['failed_translation_units'])}",
        f"- Diagnostics: {len(report['diagnostics'])}",
        "",
        "| Domain | Diagnostics | Errors | Warnings | Kinds |",
        "|---|---:|---:|---:|---|",
    ]
    for domain, summary in report["domain_summary"].items():
        kinds = ", ".join(f"{name}={count}" for name, count in summary["kinds"].items())
        lines.append(
            f"| `{domain}` | {summary['diagnostics']} | {summary['errors']} | "
            f"{summary['warnings']} | {kinds} |"
        )
    lines.append("")
    return "\n".join(lines)


def run(jobs: int = 0, filters: list[str] | None = None) -> int:
    _ensure_database()
    compiler = _compiler()
    _probe_warnings(compiler)
    entries = _entries()
    filters = filters or []
    if filters:
        entries = [entry for entry in entries
                   if any(value.lower() in entry["file"].lower() for value in filters)]
    if not entries:
        print("[enum-types] strict: no translation units selected")
        return 2

    worker_count = jobs if jobs > 0 else min(8, os.cpu_count() or 1)
    with ThreadPoolExecutor(max_workers=worker_count) as pool:
        results = list(pool.map(lambda entry: _run_entry(entry, compiler), entries))
    results.sort(key=lambda item: item["source"])

    declarations, _ = census.build_census()
    names = {item.name: item.name for item in declarations}
    names.update({enumerator.name: item.name
                  for item in declarations for enumerator in item.enumerators})
    locations = {(item.owner, item.line): item.name for item in declarations}
    diagnostics = _diagnostics(results, names, locations)
    report = {
        "schema_version": 1,
        "compiler": compiler,
        "warning_flags": WARNING_FLAGS,
        "translation_units": len(results),
        "failed_translation_units": [item["source"] for item in results if item["returncode"]],
        "diagnostics": diagnostics,
        "domain_summary": _domain_summary(diagnostics),
    }
    OUTPUT.mkdir(parents=True, exist_ok=True)
    (OUTPUT / "strict-diagnostics.json").write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    text = [f"{item['file']}:{item['line']}:{item['column']}: {item['severity']}: "
            f"{item['message']}" + (f" [{item['warning']}]" if item["warning"] else "")
            for item in diagnostics]
    (OUTPUT / "strict-diagnostics.txt").write_text("\n".join(text) + ("\n" if text else ""))
    (OUTPUT / "strict-summary.md").write_text(_summary_markdown(report))
    failed = len(report["failed_translation_units"])
    print(f"[enum-types] strict: {len(results)} units, {failed} failed, "
          f"{len(diagnostics)} diagnostics")
    return 1 if failed else 0
