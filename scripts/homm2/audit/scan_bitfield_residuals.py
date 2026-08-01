#!/usr/bin/env python3
"""Rank object diffs that look like byte-vs-bitfield load mismatches.

The scanner is read-only.  By default it recursively discovers retained
``best.candidate.obj`` / ``best.retail.obj`` pairs, reuses a sibling
``best.objdiff.json`` when present, and otherwise asks objdiff for JSON on
stdout.  The one-shot objdiff invocation uses the repository's relocation-aware
``functionRelocDiffs=data_value`` policy.

Pass ``--live`` to additionally compare every current normalized unit pair
under ``build/objdiff/paired/target`` and ``build/objdiff/base``.  Whole-unit
objdiff output is generated in memory, so this mode covers live residuals
without writing a second generated diff corpus.

Run inside ``nix develop .#build`` so ``objdiff-cli`` is available::

    python3 scripts/scan_bitfield_residuals.py
    python3 scripts/scan_bitfield_residuals.py build/tu-state-noise/run-a --format json

The signature currently recognized is the MSVC 4.2 /G5 lowering seen in
``advManager::DrawCell``:

    mov al, [source]        xor ecx, ecx
    and ax, 0x1f            mov cl, [source]
    and eax, 0xffff

The register family and low-bit mask may vary, and either retail or candidate
may contain the masked form.  Instruction rows come from objdiff, so relocation
operands have already been resolved under the configured comparison policy.
"""

from __future__ import annotations

import argparse
import csv
import json
import re
import shutil
import subprocess
import sys
from collections import Counter
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable, Sequence


DEFAULT_ROOT = Path("build/tu-state-noise")
DEFAULT_JSON_NAME = "best.objdiff.json"
DEFAULT_LIVE_PAIRED_ROOT = Path("build/objdiff/paired/target")
DEFAULT_LIVE_BASE_ROOT = Path("build/objdiff/base")
DEFAULT_SYMBOLS = Path("build/gen/symbol_names.csv")
RELOCATION_CONFIG = "functionRelocDiffs=data_value"
LOW_MASKS = frozenset((1 << width) - 1 for width in range(1, 8))
REGISTER_FAMILIES = {
    "al": ("al", "ax", "eax"),
    "bl": ("bl", "bx", "ebx"),
    "cl": ("cl", "cx", "ecx"),
    "dl": ("dl", "dx", "edx"),
}
FULL_REGISTER_FAMILIES = {
    full: (low, word, full)
    for low, word, full in REGISTER_FAMILIES.values()
}


@dataclass(frozen=True)
class LoadPattern:
    start_row: int
    end_row: int
    address: str | None
    memory: str
    register: str
    sequence: tuple[str, ...]
    mask: int | None = None


@dataclass(frozen=True)
class Occurrence:
    direction: str
    mask: str
    retail_row: int
    candidate_row: int
    retail_address: str | None
    candidate_address: str | None
    memory: str
    retail_sequence: tuple[str, ...]
    candidate_sequence: tuple[str, ...]


@dataclass(frozen=True)
class Artifact:
    directory: Path
    candidate: Path
    retail: Path
    manifest: Path | None
    cached_json: Path | None


@dataclass(frozen=True)
class LiveArtifact:
    unit: str
    candidate: Path
    retail: Path


def _parse_int(value) -> int | None:
    if value is None:
        return None
    try:
        return int(str(value), 0)
    except (TypeError, ValueError):
        return None


def _normalize_register(operand: str) -> str:
    return operand.strip().lower().removeprefix("%")


def _split_operands(text: str) -> list[str]:
    operands = []
    start = 0
    depth = 0
    for index, char in enumerate(text):
        if char in "[(":
            depth += 1
        elif char in "])":
            depth = max(0, depth - 1)
        elif char == "," and depth == 0:
            operands.append(text[start:index].strip())
            start = index + 1
    operands.append(text[start:].strip())
    return operands


def _parse_instruction(text: str) -> tuple[str, list[str]]:
    fields = text.strip().lower().split(None, 1)
    if not fields:
        return "", []
    mnemonic = fields[0]
    # Accept either objdiff's Intel spelling or an llvm-objdump-style size
    # suffix in explicit/synthetic instruction rows.
    if mnemonic in {"movb", "movw", "movl", "andw", "andl", "xorl"}:
        mnemonic = mnemonic[:-1]
    return mnemonic, _split_operands(fields[1]) if len(fields) == 2 else []


def _immediate(operand: str) -> int | None:
    value = operand.strip().lower().removeprefix("$")
    try:
        return int(value, 0)
    except ValueError:
        return None


def _is_memory(operand: str) -> bool:
    value = operand.strip().lower()
    return (
        "[" in value
        or "(" in value
        or value.startswith("byte ptr ")
        or value.startswith("word ptr ")
        or value.startswith("dword ptr ")
    )


def _normalize_memory(operand: str) -> str:
    value = operand.strip().lower()
    value = re.sub(r"^(?:byte|word|dword)\s+ptr\s+", "", value)
    return re.sub(r"\s+", "", value)


def _byte_load(text: str) -> tuple[tuple[str, str, str], str] | None:
    mnemonic, operands = _parse_instruction(text)
    if mnemonic != "mov" or len(operands) != 2:
        return None
    left = _normalize_register(operands[0])
    right = _normalize_register(operands[1])
    if left in REGISTER_FAMILIES and _is_memory(operands[1]):
        return REGISTER_FAMILIES[left], _normalize_memory(operands[1])
    if right in REGISTER_FAMILIES and _is_memory(operands[0]):
        return REGISTER_FAMILIES[right], _normalize_memory(operands[0])
    return None


def _and_immediate(text: str) -> tuple[str, int] | None:
    mnemonic, operands = _parse_instruction(text)
    if mnemonic != "and" or len(operands) != 2:
        return None
    for register_operand, immediate_operand in (
        (operands[0], operands[1]),
        (operands[1], operands[0]),
    ):
        register = _normalize_register(register_operand)
        immediate = _immediate(immediate_operand)
        if immediate is not None:
            return register, immediate
    return None


def _zeroing_xor(text: str) -> tuple[str, str, str] | None:
    mnemonic, operands = _parse_instruction(text)
    if mnemonic != "xor" or len(operands) != 2:
        return None
    left = _normalize_register(operands[0])
    right = _normalize_register(operands[1])
    if left == right:
        return FULL_REGISTER_FAMILIES.get(left)
    return None


def _instruction(row: dict) -> dict | None:
    instruction = row.get("instruction")
    return instruction if isinstance(instruction, dict) else None


def _formatted(row: dict) -> str | None:
    instruction = _instruction(row)
    value = instruction.get("formatted") if instruction else None
    return value if isinstance(value, str) else None


def _address(row: dict) -> str | None:
    instruction = _instruction(row)
    value = instruction.get("address") if instruction else None
    if value is None:
        return None
    parsed = _parse_int(value)
    return f"0x{parsed:x}" if parsed is not None else str(value)


def find_masked_loads(rows: Sequence[dict]) -> list[LoadPattern]:
    """Find ``mov r8,[mem]; and r16,mask; and r32,0xffff`` row triples."""
    out = []
    for index in range(len(rows) - 2):
        sequence = tuple(_formatted(rows[index + offset]) for offset in range(3))
        if any(text is None for text in sequence):
            continue
        load = _byte_load(sequence[0])
        first_and = _and_immediate(sequence[1])
        second_and = _and_immediate(sequence[2])
        if load is None or first_and is None or second_and is None:
            continue
        family, memory = load
        if (
            first_and[0] == family[1]
            and first_and[1] in LOW_MASKS
            and second_and == (family[2], 0xFFFF)
        ):
            out.append(LoadPattern(
                start_row=index,
                end_row=index + 2,
                address=_address(rows[index]),
                memory=memory,
                register=family[2],
                sequence=sequence,
                mask=first_and[1],
            ))
    return out


def find_plain_byte_loads(rows: Sequence[dict]) -> list[LoadPattern]:
    """Find ``xor r32,r32; mov r8,[mem]`` row pairs."""
    out = []
    for index in range(len(rows) - 1):
        sequence = tuple(_formatted(rows[index + offset]) for offset in range(2))
        if any(text is None for text in sequence):
            continue
        family = _zeroing_xor(sequence[0])
        load = _byte_load(sequence[1])
        if family is None or load is None or family != load[0]:
            continue
        out.append(LoadPattern(
            start_row=index,
            end_row=index + 1,
            address=_address(rows[index]),
            memory=load[1],
            register=family[2],
            sequence=sequence,
        ))
    return out


def _row_distance(left: LoadPattern, right: LoadPattern) -> int:
    if left.end_row < right.start_row:
        return right.start_row - left.end_row
    if right.end_row < left.start_row:
        return left.start_row - right.end_row
    return 0


def _match_patterns(
    masked: Sequence[LoadPattern],
    plain: Sequence[LoadPattern],
    *,
    direction: str,
    masked_is_retail: bool,
    max_row_distance: int = 2,
) -> list[Occurrence]:
    out = []
    used_plain: set[int] = set()
    for masked_load in masked:
        choices = [
            (index, plain_load)
            for index, plain_load in enumerate(plain)
            if (
                index not in used_plain
                and plain_load.memory == masked_load.memory
                and _row_distance(masked_load, plain_load) <= max_row_distance
            )
        ]
        if not choices:
            continue
        plain_index, plain_load = min(
            choices,
            key=lambda choice: (
                _row_distance(masked_load, choice[1]),
                abs(masked_load.start_row - choice[1].start_row),
            ),
        )
        used_plain.add(plain_index)
        retail = masked_load if masked_is_retail else plain_load
        candidate = plain_load if masked_is_retail else masked_load
        out.append(Occurrence(
            direction=direction,
            mask=f"0x{masked_load.mask:x}",
            retail_row=retail.start_row,
            candidate_row=candidate.start_row,
            retail_address=retail.address,
            candidate_address=candidate.address,
            memory=masked_load.memory,
            retail_sequence=retail.sequence,
            candidate_sequence=candidate.sequence,
        ))
    return out


def detect_occurrences(retail_rows: Sequence[dict], candidate_rows: Sequence[dict]):
    return (
        _match_patterns(
            find_masked_loads(retail_rows),
            find_plain_byte_loads(candidate_rows),
            direction="retail_masked_candidate_plain",
            masked_is_retail=True,
        )
        + _match_patterns(
            find_masked_loads(candidate_rows),
            find_plain_byte_loads(retail_rows),
            direction="retail_plain_candidate_masked",
            masked_is_retail=False,
        )
    )


def discover_artifacts(roots: Iterable[Path]) -> list[Artifact]:
    directories = set()
    for requested in roots:
        root = requested.resolve()
        if (
            root.is_file()
            and root.name == "best.candidate.obj"
            and not root.parent.name.startswith(".")
        ):
            directories.add(root.parent)
        elif (root / "best.candidate.obj").is_file():
            if not root.name.startswith("."):
                directories.add(root)
        elif root.is_dir():
            directories.update(
                path.parent
                for path in root.rglob("best.candidate.obj")
                if not path.parent.name.startswith(".")
            )
    artifacts = []
    for directory in sorted(directories):
        candidate = directory / "best.candidate.obj"
        retail = directory / "best.retail.obj"
        if not retail.is_file():
            continue
        manifest = directory / "manifest.json"
        cached_json = directory / DEFAULT_JSON_NAME
        artifacts.append(Artifact(
            directory=directory,
            candidate=candidate,
            retail=retail,
            manifest=manifest if manifest.is_file() else None,
            cached_json=cached_json if cached_json.is_file() else None,
        ))
    return artifacts


def _load_manifest(artifact: Artifact) -> dict:
    if artifact.manifest is None:
        return {}
    payload = json.loads(artifact.manifest.read_text())
    return payload if isinstance(payload, dict) else {}


def _manifest_symbol(manifest: dict) -> str | None:
    value = manifest.get("target", {}).get("symbol")
    return value if isinstance(value, str) and value else None


def run_objdiff(artifact: Artifact, symbol: str, executable: str) -> dict:
    command = [
        executable,
        "diff",
        "-1",
        str(artifact.retail),
        "-2",
        str(artifact.candidate),
        symbol,
        "-o",
        "-",
        "--format",
        "json",
        "-c",
        RELOCATION_CONFIG,
    ]
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode:
        detail = (result.stderr or result.stdout).strip()
        raise RuntimeError(f"objdiff failed ({result.returncode}): {detail}")
    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"objdiff returned invalid JSON: {exc}") from exc
    if not isinstance(payload, dict):
        raise RuntimeError("objdiff JSON root is not an object")
    return payload


def run_live_objdiff(artifact: LiveArtifact, executable: str) -> dict:
    command = [
        executable,
        "diff",
        "-1",
        str(artifact.retail),
        "-2",
        str(artifact.candidate),
        "-o",
        "-",
        "--format",
        "json",
        "-c",
        RELOCATION_CONFIG,
    ]
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode:
        detail = (result.stderr or result.stdout).strip()
        raise RuntimeError(f"objdiff failed ({result.returncode}): {detail}")
    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"objdiff returned invalid JSON: {exc}") from exc
    if not isinstance(payload, dict):
        raise RuntimeError("objdiff JSON root is not an object")
    return payload


def load_objdiff(
    artifact: Artifact,
    symbol: str,
    executable: str,
    refresh: bool = False,
) -> tuple[dict, str]:
    cache_is_current = (
        artifact.cached_json is not None
        and artifact.cached_json.stat().st_mtime_ns
        >= max(
            artifact.candidate.stat().st_mtime_ns,
            artifact.retail.stat().st_mtime_ns,
        )
    )
    if cache_is_current and not refresh:
        payload = json.loads(artifact.cached_json.read_text())
        if _symbol_entry(payload, "right", symbol) is not None:
            return payload, str(artifact.cached_json)
    return run_objdiff(artifact, symbol, executable), "generated-on-stdout"


def _symbol_entry(payload: dict, side: str, symbol: str) -> dict | None:
    symbols = payload.get(side, {}).get("symbols", [])
    matches = [
        entry for entry in symbols
        if (
            isinstance(entry, dict)
            and entry.get("name") == symbol
            and isinstance(entry.get("instructions"), list)
        )
    ]
    return matches[0] if len(matches) == 1 else None


def _display_path(path: Path, cwd: Path) -> str:
    try:
        return str(path.resolve().relative_to(cwd.resolve()))
    except ValueError:
        return str(path.resolve())


def analyze_artifact(
    artifact: Artifact,
    *,
    executable: str,
    refresh: bool,
    cwd: Path,
) -> dict | None:
    manifest = _load_manifest(artifact)
    symbol = _manifest_symbol(manifest)
    if symbol is None:
        raise ValueError("manifest has no target symbol")
    payload, json_source = load_objdiff(artifact, symbol, executable, refresh)
    retail = _symbol_entry(payload, "left", symbol)
    candidate = _symbol_entry(payload, "right", symbol)
    if retail is None or candidate is None:
        raise ValueError(f"objdiff does not contain one instruction-bearing {symbol}")
    occurrences = detect_occurrences(retail["instructions"], candidate["instructions"])
    if not occurrences:
        return None
    target = manifest.get("target", {})
    baseline = manifest.get("baseline", {})
    masks = Counter(occurrence.mask for occurrence in occurrences)
    directions = Counter(occurrence.direction for occurrence in occurrences)
    score = candidate.get("match_percent", retail.get("match_percent"))
    return {
        "artifact": _display_path(artifact.directory, cwd),
        "objdiff_json": (
            _display_path(Path(json_source), cwd)
            if json_source != "generated-on-stdout"
            else json_source
        ),
        "unit": target.get("unit"),
        "function": symbol,
        "rva": target.get("rva"),
        "occurrence_count": len(occurrences),
        "masks": dict(sorted(masks.items())),
        "directions": dict(sorted(directions.items())),
        "score": float(score) if score is not None else None,
        "baseline_score": baseline.get("score"),
        "retail_size": _parse_int(retail.get("size")),
        "candidate_size": _parse_int(candidate.get("size")),
        "occurrences": [asdict(occurrence) for occurrence in occurrences],
    }


def scan(
    roots: Iterable[Path],
    *,
    executable: str = "objdiff-cli",
    refresh: bool = False,
    cwd: Path | None = None,
) -> dict:
    cwd = cwd or Path.cwd()
    artifacts = discover_artifacts(roots)
    findings = []
    errors = []
    for artifact in artifacts:
        try:
            finding = analyze_artifact(
                artifact,
                executable=executable,
                refresh=refresh,
                cwd=cwd,
            )
            if finding is not None:
                findings.append(finding)
        except (OSError, ValueError, RuntimeError, json.JSONDecodeError) as exc:
            errors.append({
                "artifact": _display_path(artifact.directory, cwd),
                "error": str(exc),
            })
    findings.sort(key=_finding_sort_key)
    for rank, finding in enumerate(findings, 1):
        finding["rank"] = rank
    return {
        "schema": 1,
        "relocation_config": RELOCATION_CONFIG,
        "scanned_pairs": len(artifacts),
        "finding_count": len(findings),
        "findings": findings,
        "errors": errors,
    }


def _finding_sort_key(finding: dict):
    return (
        -finding["occurrence_count"],
        finding["score"] if finding["score"] is not None else 101.0,
        finding["rva"] or "",
        finding["artifact"],
    )


def discover_live_artifacts(
    paired_root: Path,
    base_root: Path,
) -> list[LiveArtifact]:
    paired_root = paired_root.resolve()
    base_root = base_root.resolve()
    artifacts = []
    for candidate in sorted(paired_root.rglob("*.c.obj")):
        relative = candidate.relative_to(paired_root)
        retail_name = relative.name.removesuffix(".c.obj") + ".obj"
        retail = base_root / relative.parent / retail_name
        if not retail.is_file():
            continue
        unit = str(relative.parent / relative.name.removesuffix(".c.obj"))
        artifacts.append(LiveArtifact(unit, candidate, retail))
    return artifacts


def symbol_rvas(path: Path) -> dict[tuple[str, str], str]:
    grouped: dict[tuple[str, str], list[str]] = {}
    with path.open(encoding="latin-1", newline="") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") != "func":
                continue
            grouped.setdefault((row["unit"], row["name"]), []).append(row["rva"])
    return {
        key: values[0]
        for key, values in grouped.items()
        if len(values) == 1
    }


def analyze_live_artifact(
    artifact: LiveArtifact,
    *,
    executable: str,
    rvas: dict[tuple[str, str], str],
    cwd: Path,
) -> list[dict]:
    payload = run_live_objdiff(artifact, executable)
    left_symbols = payload.get("left", {}).get("symbols", [])
    right_symbols = payload.get("right", {}).get("symbols", [])
    findings = []
    for retail in left_symbols:
        if not isinstance(retail, dict):
            continue
        target_index = _parse_int(retail.get("target_symbol"))
        retail_rows = retail.get("instructions")
        if (
            target_index is None
            or not isinstance(retail_rows, list)
            or target_index < 0
            or target_index >= len(right_symbols)
        ):
            continue
        candidate = right_symbols[target_index]
        if not isinstance(candidate, dict):
            continue
        candidate_rows = candidate.get("instructions")
        symbol = retail.get("name")
        if (
            not isinstance(candidate_rows, list)
            or not isinstance(symbol, str)
            or candidate.get("name") != symbol
        ):
            continue
        occurrences = detect_occurrences(retail_rows, candidate_rows)
        if not occurrences:
            continue
        masks = Counter(occurrence.mask for occurrence in occurrences)
        directions = Counter(occurrence.direction for occurrence in occurrences)
        score = candidate.get("match_percent", retail.get("match_percent"))
        findings.append({
            "artifact": _display_path(artifact.candidate, cwd),
            "objdiff_json": "generated-on-stdout",
            "unit": artifact.unit,
            "function": symbol,
            "rva": rvas.get((artifact.unit, symbol)),
            "occurrence_count": len(occurrences),
            "masks": dict(sorted(masks.items())),
            "directions": dict(sorted(directions.items())),
            "score": float(score) if score is not None else None,
            "baseline_score": None,
            "retail_size": _parse_int(retail.get("size")),
            "candidate_size": _parse_int(candidate.get("size")),
            "occurrences": [asdict(occurrence) for occurrence in occurrences],
        })
    return findings


def scan_live(
    paired_root: Path,
    base_root: Path,
    symbols: Path,
    *,
    executable: str = "objdiff-cli",
    cwd: Path | None = None,
) -> dict:
    cwd = cwd or Path.cwd()
    artifacts = discover_live_artifacts(paired_root, base_root)
    rvas = symbol_rvas(symbols)
    findings = []
    errors = []
    for artifact in artifacts:
        try:
            findings.extend(analyze_live_artifact(
                artifact,
                executable=executable,
                rvas=rvas,
                cwd=cwd,
            ))
        except (OSError, RuntimeError, json.JSONDecodeError) as exc:
            errors.append({
                "artifact": _display_path(artifact.candidate, cwd),
                "error": str(exc),
            })
    findings.sort(key=_finding_sort_key)
    return {
        "schema": 1,
        "relocation_config": RELOCATION_CONFIG,
        "scanned_pairs": len(artifacts),
        "finding_count": len(findings),
        "findings": findings,
        "errors": errors,
    }


def merge_reports(retained: dict, live: dict | None) -> dict:
    if live is None:
        return retained
    findings = retained["findings"] + live["findings"]
    findings.sort(key=_finding_sort_key)
    for rank, finding in enumerate(findings, 1):
        finding["rank"] = rank
    return {
        "schema": 1,
        "relocation_config": RELOCATION_CONFIG,
        "scanned_pairs": retained["scanned_pairs"] + live["scanned_pairs"],
        "scanned_retained_pairs": retained["scanned_pairs"],
        "scanned_live_pairs": live["scanned_pairs"],
        "finding_count": len(findings),
        "findings": findings,
        "errors": retained["errors"] + live["errors"],
    }


def _format_tsv(report: dict) -> str:
    columns = (
        "rank", "occurrences", "masks", "score", "baseline", "sizes",
        "unit", "rva", "function", "artifact",
    )
    lines = ["\t".join(columns)]
    for finding in report["findings"]:
        masks = ",".join(
            f"{mask}:{count}" for mask, count in finding["masks"].items()
        )
        score = "" if finding["score"] is None else f"{finding['score']:.6f}"
        baseline = (
            "" if finding["baseline_score"] is None
            else f"{float(finding['baseline_score']):.6f}"
        )
        sizes = "/".join(
            "" if size is None else str(size)
            for size in (finding["retail_size"], finding["candidate_size"])
        )
        lines.append("\t".join(str(value) if value is not None else "" for value in (
            finding["rank"],
            finding["occurrence_count"],
            masks,
            score,
            baseline,
            sizes,
            finding["unit"],
            finding["rva"],
            finding["function"],
            finding["artifact"],
        )))
    return "\n".join(lines) + "\n"


def parse_args(argv: Sequence[str] | None = None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "roots",
        nargs="*",
        type=Path,
        default=[DEFAULT_ROOT],
        help="artifact directories or recursive roots (default: build/tu-state-noise)",
    )
    parser.add_argument(
        "--format",
        choices=("tsv", "json"),
        default="tsv",
        help="report format (default: tsv)",
    )
    parser.add_argument(
        "--objdiff-cli",
        default="objdiff-cli",
        help="objdiff executable used for artifacts without cached JSON",
    )
    parser.add_argument(
        "--refresh",
        action="store_true",
        help="ignore cached best.objdiff.json files (generated JSON remains in memory)",
    )
    parser.add_argument(
        "--live",
        action="store_true",
        help="also scan every current normalized unit pair",
    )
    parser.add_argument(
        "--live-paired-root",
        type=Path,
        default=DEFAULT_LIVE_PAIRED_ROOT,
        help=f"current candidate unit root (default: {DEFAULT_LIVE_PAIRED_ROOT})",
    )
    parser.add_argument(
        "--live-base-root",
        type=Path,
        default=DEFAULT_LIVE_BASE_ROOT,
        help=f"current retail unit root (default: {DEFAULT_LIVE_BASE_ROOT})",
    )
    parser.add_argument(
        "--symbols",
        type=Path,
        default=DEFAULT_SYMBOLS,
        help=f"function RVA inventory (default: {DEFAULT_SYMBOLS})",
    )
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    if shutil.which(args.objdiff_cli) is None:
        cached_pairs = [
            artifact for artifact in discover_artifacts(args.roots)
            if (
                artifact.cached_json is not None
                and artifact.cached_json.stat().st_mtime_ns
                >= max(
                    artifact.candidate.stat().st_mtime_ns,
                    artifact.retail.stat().st_mtime_ns,
                )
                and not args.refresh
            )
        ]
        all_pairs = discover_artifacts(args.roots)
        if len(cached_pairs) != len(all_pairs):
            print(
                f"{args.objdiff_cli!r} is unavailable; enter nix develop .#build "
                "or scan only artifacts with best.objdiff.json",
                file=sys.stderr,
            )
            return 2
    retained_report = scan(
        args.roots,
        executable=args.objdiff_cli,
        refresh=args.refresh,
    )
    live_report = None
    if args.live:
        live_report = scan_live(
            args.live_paired_root,
            args.live_base_root,
            args.symbols,
            executable=args.objdiff_cli,
        )
    report = merge_reports(retained_report, live_report)
    if args.format == "json":
        print(json.dumps(report, indent=2, sort_keys=True))
    else:
        sys.stdout.write(_format_tsv(report))
        for error in report["errors"]:
            print(
                f"warning: {error['artifact']}: {error['error']}",
                file=sys.stderr,
            )
        if args.live:
            summary = (
                f"scanned {report['scanned_retained_pairs']} retained and "
                f"{report['scanned_live_pairs']} live pairs"
            )
        else:
            summary = f"scanned {report['scanned_pairs']} retained pairs"
        print(
            f"{summary}; found {report['finding_count']} likely bitfield residuals",
            file=sys.stderr,
        )
    return 1 if report["errors"] else 0


if __name__ == "__main__":
    raise SystemExit(main())
