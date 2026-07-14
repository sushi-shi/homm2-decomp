"""Audit enum-bearing MSVC mangled symbols against the retail inventory."""

from __future__ import annotations

import csv
import json
import shutil
import subprocess

from .census import OUTPUT, ROOT


def _tool() -> str:
    tool = shutil.which("llvm-nm")
    if not tool:
        raise RuntimeError("llvm-nm not found; run inside `nix develop .#build`")
    return tool


def _retail_symbols() -> list[dict]:
    path = ROOT / "build" / "gen" / "symbol_names.csv"
    with path.open(newline="") as handle:
        rows = [row for row in csv.DictReader(handle) if "W4" in row["name"]]
    return sorted(rows, key=lambda row: (row["name"], row["unit"], row["rva"]))


def _object_symbols(tool: str) -> tuple[list[dict], list[str]]:
    roots = [ROOT / "build" / "objdiff" / "base", ROOT / "build" / "base"]
    objects = sorted({path for root in roots if root.exists() for path in root.rglob("*.obj")})
    if not objects:
        raise RuntimeError("no production .obj files found; run `homm2 build` first")
    records = []
    for path in objects:
        result = subprocess.run([tool, "--format=posix", str(path)], text=True, capture_output=True)
        if result.returncode:
            raise RuntimeError(f"llvm-nm failed for {path}: {result.stderr.strip()}")
        for line in result.stdout.splitlines():
            fields = line.split()
            if fields and "W4" in fields[0]:
                records.append({"name": fields[0], "object": str(path.relative_to(ROOT))})
    records.sort(key=lambda row: (row["name"], row["object"]))
    return records, [str(path.relative_to(ROOT)) for path in objects]


def run() -> int:
    tool = _tool()
    retail = _retail_symbols()
    production, objects = _object_symbols(tool)
    retail_names = {row["name"] for row in retail}
    unexpected = [row for row in production if row["name"] not in retail_names]
    report = {
        "schema_version": 1,
        "tool": tool,
        "objects_scanned": objects,
        "retail_w4_symbols": retail,
        "production_w4_symbols": production,
        "unexpected_production_w4_symbols": unexpected,
    }
    OUTPUT.mkdir(parents=True, exist_ok=True)
    (OUTPUT / "symbol-audit.json").write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    lines = [
        "# Enum Symbol Audit", "",
        f"- Objects scanned: {len(objects)}",
        f"- Retail `W4` symbols: {len(retail)}",
        f"- Production `W4` symbols: {len(production)}",
        f"- Unexpected production `W4` symbols: {len(unexpected)}", "",
    ]
    lines.extend(f"- `{row['name']}` in `{row['object']}`" for row in unexpected)
    (OUTPUT / "symbol-audit.md").write_text("\n".join(lines) + "\n")
    print(f"[enum-types] symbols: {len(objects)} objects, {len(production)} production W4, "
          f"{len(unexpected)} unexpected")
    return 1 if unexpected else 0
