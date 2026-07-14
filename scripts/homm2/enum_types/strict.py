"""Whole-tree strict enum syntax audit using the generated Clang database."""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import tempfile
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
COMPATIBILITY_FLAGS = [
    "-ferror-limit=0",
    "-Wno-writable-strings",
    "-Wno-microsoft",
    "-Wno-ignored-pragmas",
    "-Wno-pragma-pack",
    "-Wno-nonportable-include-path",
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
    if DATABASE.exists():
        return
    from homm2.init.clangd import main as generate_database
    generate_database()


def _probe_warnings(compiler: str) -> None:
    with tempfile.NamedTemporaryFile(suffix=".cpp", mode="w", delete=False) as probe:
        probe.write("int enum_type_probe;\n")
        probe_path = probe.name
    try:
        command = [compiler, "/nologo", "/Zs", probe_path,
                   "-Werror=unknown-warning-option", *WARNING_FLAGS]
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
            "/DHOMM2_STRICT_ENUM_TYPES", *COMPATIBILITY_FLAGS, *WARNING_FLAGS]


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


def _diagnostics(results: list[dict], domain_names: set[str]) -> list[dict]:
    diagnostics = []
    for result in results:
        for raw in result["output"].splitlines():
            match = GNU_DIAGNOSTIC_RE.match(raw) or MSVC_DIAGNOSTIC_RE.match(raw)
            if not match:
                if raw.strip():
                    diagnostics.append({
                        "source": result["source"], "file": result["source"],
                        "line": 0, "column": 0, "severity": "driver",
                        "warning": None, "promoted": False, "domain": None,
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
            mentioned = sorted(message_identifiers & domain_names)
            diagnostics.append({
                "source": result["source"], "file": filename,
                "line": int(data["line"]), "column": int(data["column"]),
                "severity": data["severity"], "warning": warning_flags[-1] if warning_flags else None,
                "promoted": data["severity"] == "error" and "-Werror" in (data["flags"] or ""),
                "domain": mentioned[0] if mentioned else None,
                "message": data["message"],
            })
    return sorted(diagnostics, key=lambda item: (
        item["file"], item["line"], item["column"], item["severity"], item["message"], item["source"]
    ))


def run(jobs: int = 0, filters: list[str] | None = None) -> int:
    _ensure_database()
    compiler = _compiler()
    _probe_warnings(compiler)
    entries = json.loads(DATABASE.read_text())
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
    names = {item.name for item in declarations}
    names.update(enumerator.name for item in declarations for enumerator in item.enumerators)
    diagnostics = _diagnostics(results, names)
    report = {
        "schema_version": 1,
        "compiler": compiler,
        "translation_units": len(results),
        "failed_translation_units": [item["source"] for item in results if item["returncode"]],
        "diagnostics": diagnostics,
    }
    OUTPUT.mkdir(parents=True, exist_ok=True)
    (OUTPUT / "strict-diagnostics.json").write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    text = [f"{item['file']}:{item['line']}:{item['column']}: {item['severity']}: "
            f"{item['message']}" + (f" [{item['warning']}]" if item["warning"] else "")
            for item in diagnostics]
    (OUTPUT / "strict-diagnostics.txt").write_text("\n".join(text) + ("\n" if text else ""))
    failed = len(report["failed_translation_units"])
    print(f"[enum-types] strict: {len(results)} units, {failed} failed, "
          f"{len(diagnostics)} diagnostics")
    return 1 if failed else 0
