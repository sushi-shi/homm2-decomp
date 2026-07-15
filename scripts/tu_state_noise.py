#!/usr/bin/env python3
"""Probe temporary parser-visible TU-state inputs before one MSVC 4.2 function.

This remains the standalone diagnostic/record-max compatibility engine. New combined
searches should use ``scripts/match_variants.py``, which imports these state families and
crosses them with libclang mutations and hand-authored axes. It does not rewrite the target. Each
trial temporarily inserts deterministic parser-visible declarations, definitions, or
curated includes before the target's ``VA`` metadata block, compiles the real translation
unit with VC 4.2, scores the requested symbol with objdiff, and restores the source
immediately.  Probe-emitted symbols/storage exist only in the disposable candidate object.

The source is unchanged on normal exit, compiler failure, timeout, or Ctrl-C. Each
baseline/trial compile has a bounded timeout; on expiry the complete compiler process
group is terminated so Wine/MSVC descendants cannot survive. Results, exact snippets,
and COFF metrics are written to ``build/tu-state-noise``. Generated noise is diagnostic
input only and is never written back to reconstructed source.

Run inside ``nix develop .#build`` after entering the worktree first::

    python3 scripts/tu_state_noise.py \
      --source src/BASE/WINMGR.cpp --rva 0xca6d0 --trials 40

This is appropriate only after semantics, frame/slots, CFG, and external relocations
have already been audited.  It is not a substitute for reconstruction, od_slots.py,
or scripts/permute_ast.py.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import math
import os
import random
import re
import signal
import subprocess
import sys
import tempfile
import time
import tomllib
from contextlib import contextmanager
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable

from tu_state_metrics import read_coff


IMAGE_BASE = 0x400000
DEFAULT_FAMILIES = (
    "typedef", "enum", "struct", "class", "packed", "member", "extern",
    "static-data", "prototype", "function", "include", "mixed",
)
ALL_FAMILIES = DEFAULT_FAMILIES
SAFE_ENUM_VALUES = (-32768, -1, 0, 1, 2, 7, 31, 255, 256, 1024, 32767, 65535)
SAFE_SCALAR_TYPES = ("char", "unsigned char", "short", "unsigned short", "int", "unsigned long")
CURATED_INCLUDES = (
    "<stddef.h>", "<limits.h>", "<string.h>", "<stdlib.h>",
    "<va.h>", "<Ints.h>", "<windows.h>", "<BASE/bitmap.h>",
    "<BASE/IconEntry.h>", "<BASE/WINMGR.h>",
)
DEFAULT_COMPILE_TIMEOUT_SECONDS = 120.0
PROCESS_GROUP_TERMINATION_GRACE_SECONDS = 1.0


class BaselineUpdateError(ValueError):
    pass


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def parse_int(value: str) -> int:
    try:
        return int(value, 0)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid integer: {value}") from exc


def positive_seconds(value: str) -> float:
    try:
        seconds = float(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid timeout: {value}") from exc
    if not math.isfinite(seconds) or seconds <= 0:
        raise argparse.ArgumentTypeError("timeout must be a positive finite number")
    return seconds


@dataclass(frozen=True)
class Target:
    source: Path
    unit: str
    rva: int
    va: int
    symbol: str
    retail_size: int
    marker_offset: int
    insertion_offset: int
    logical_line: int


@dataclass(frozen=True)
class Variant:
    trial: int
    family: str
    tag: str
    body: str

    def block(self, logical_line: int) -> str:
        # Restore the pre-existing logical line before any authored source token.
        # No filename is supplied, so an earlier #line filename remains unchanged.
        return f"{self.body}#line {logical_line}\n"


def load_units(root: Path) -> dict[str, dict]:
    raw = tomllib.loads((root / "config/units.toml").read_text())
    profiles = raw.get("flags", {})
    out = {}
    for unit in raw.get("unit", []):
        out[unit["unit"]] = {
            "source": Path(unit["source"]),
            "flags": list(profiles[unit.get("flags", "base")]),
            "profile": unit.get("flags", "base"),
        }
    return out


def _normalize_rva(value: int) -> int:
    return value - IMAGE_BASE if value >= IMAGE_BASE else value


def _leading_metadata_offset(text: str, marker_offset: int) -> int:
    """Include contiguous blank/comment metadata immediately before a VA marker."""
    lines = text[:marker_offset].splitlines(keepends=True)
    offset = marker_offset
    for line in reversed(lines):
        stripped = line.strip()
        if not stripped or stripped.startswith("//"):
            offset -= len(line)
            continue
        break
    return offset


_LINE_DIRECTIVE = re.compile(r'^\s*#\s*line\s+(\d+)(?:\s+"[^"]*")?')
_INCLUDE_DIRECTIVE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.M)
_DEFINE_DIRECTIVE = re.compile(r'^\s*#\s*define\s+([A-Za-z_]\w*)', re.M)
_IDENTIFIER = re.compile(r'\b[A-Za-z_]\w*\b')
_VA_MARKER_LINE = re.compile(r'^[ \t]*VA\(0x[0-9a-f]+,', re.M | re.I)


def logical_line_at(text: str, offset: int) -> int:
    """Return the logical line number of the source line beginning at *offset*."""
    logical = 1
    for line in text[:offset].splitlines(keepends=True):
        match = _LINE_DIRECTIVE.match(line)
        logical = int(match.group(1)) if match else logical + 1
    return logical


def target_identifiers(text: str, target: Target) -> set[str]:
    """Conservative identifier census for the canonical target's VA-delimited block."""
    next_marker = _VA_MARKER_LINE.search(text, target.marker_offset + 1)
    block = text[target.marker_offset : next_marker.start() if next_marker else len(text)]
    return set(_IDENTIFIER.findall(block))


def target_suffix_digest(text: str, va: int) -> str | None:
    """Hash the exact authored suffix beginning at one real target VA marker.

    Noise is inserted before that marker, so suffix identity is a stronger and much
    cheaper per-trial invariant than recomputing normalized hashes for the whole tree.
    """
    marker_pattern = re.compile(rf"^[ \t]*(VA\(0x{va:08x},)", re.M | re.I)
    matches = list(marker_pattern.finditer(text))
    if len(matches) != 1:
        return None
    return sha256_bytes(text[matches[0].start(1) :].encode("utf-8"))


def _case_insensitive_child(parent: Path, relative: str) -> Path | None:
    current = parent
    for part in Path(relative).parts:
        direct = current / part
        if direct.exists():
            current = direct
            continue
        if not current.is_dir():
            return None
        match = next((entry for entry in current.iterdir() if entry.name.lower() == part.lower()), None)
        if match is None:
            return None
        current = match
    return current if current.is_file() else None


def include_macro_guard(root: Path, probe_body: str, target_tokens: set[str]) -> dict:
    """Fail closed if curated includes can macro-rewrite an identifier in the target block."""
    requested = _INCLUDE_DIRECTIVE.findall(probe_body)
    if not requested:
        return {"checked": False, "headers": [], "macro_conflicts": []}
    allowed = {header[1:-1] for header in CURATED_INCLUDES}
    include_roots = [root / "include", root / "build/toolchain/msvc/include"]
    vendor = root / "vendor"
    if vendor.is_dir():
        include_roots.extend(sorted(path for path in vendor.iterdir() if path.is_dir()))
    pending = [(None, header) for header in requested]
    visited: set[Path] = set()
    macros: set[str] = set()
    unresolved = []
    while pending:
        owner, header = pending.pop()
        if owner is None and header not in allowed:
            unresolved.append(f"not allowlisted: {header}")
            continue
        search_roots = ([owner.parent] if owner is not None else []) + include_roots
        resolved = next(
            (candidate for base in search_roots if (candidate := _case_insensitive_child(base, header))),
            None,
        )
        if resolved is None:
            unresolved.append(header)
            continue
        resolved = resolved.resolve()
        if resolved in visited:
            continue
        visited.add(resolved)
        text = resolved.read_text(errors="replace")
        macros.update(_DEFINE_DIRECTIVE.findall(text))
        pending.extend((resolved, child) for child in _INCLUDE_DIRECTIVE.findall(text))
    conflicts = sorted(macros & target_tokens)
    return {
        "checked": True,
        "headers": requested,
        "transitive_header_count": len(visited),
        "defined_macro_count": len(macros),
        "macro_conflicts": conflicts,
        "unresolved_headers": sorted(set(unresolved)),
        "passed": not conflicts and not unresolved,
    }


def resolve_target(root: Path, source_arg: Path, requested_rva: int) -> tuple[Target, list[str]]:
    source = (root / source_arg).resolve() if not source_arg.is_absolute() else source_arg.resolve()
    try:
        source_rel = source.relative_to(root)
    except ValueError as exc:
        raise ValueError(f"source must be inside the worktree: {source}") from exc
    units = load_units(root)
    matches = [(name, cfg) for name, cfg in units.items() if cfg["source"] == source_rel]
    if len(matches) != 1:
        raise ValueError(f"source is not the unique source of a configured unit: {source_rel}")
    unit, cfg = matches[0]
    rva = _normalize_rva(requested_rva)
    with (root / "build/gen/symbol_names.csv").open(newline="") as handle:
        rows = [
            row for row in csv.DictReader(handle)
            if row["kind"] == "func" and row["unit"] == unit and int(row["rva"], 0) == rva
        ]
    if len(rows) != 1:
        raise ValueError(f"RVA 0x{rva:x} is not a unique CodeView function in {unit}")
    row = rows[0]
    text = source.read_text()
    va = rva + IMAGE_BASE
    marker = f"VA(0x{va:08x},"
    marker_pattern = re.compile(
        rf"^[ \t]*(VA\(0x{va:08x},)",
        re.M | re.I,
    )
    positions = [match.start(1) for match in marker_pattern.finditer(text)]
    if len(positions) != 1:
        raise ValueError(f"expected one source marker {marker}, found {len(positions)}")
    marker_offset = positions[0]
    insertion_offset = _leading_metadata_offset(text, marker_offset)
    return Target(
        source=source,
        unit=unit,
        rva=rva,
        va=va,
        symbol=row["name"],
        retail_size=int(row["size"], 0),
        marker_offset=marker_offset,
        insertion_offset=insertion_offset,
        logical_line=logical_line_at(text, insertion_offset),
    ), cfg["flags"]


def make_variants(count: int, families: Iterable[str], seed: int) -> list[Variant]:
    selected = tuple(families)
    unknown = sorted(set(selected) - set(ALL_FAMILIES))
    if unknown:
        raise ValueError(f"unknown noise families: {', '.join(unknown)}")
    if not selected:
        raise ValueError("at least one noise family is required")
    rng = random.Random(seed)
    variants = []
    for trial in range(1, count + 1):
        family = selected[(trial - 1) % len(selected)]
        tag = f"{seed:08x}-{trial:04d}-{rng.getrandbits(32):08x}"
        ident = f"HOMM2_TU_STATE_PROBE_{tag.replace('-', '_').upper()}"
        repeat = 1 + rng.randrange(4)
        aliases = "".join(
            f"typedef {rng.choice(SAFE_SCALAR_TYPES)} {ident}_ALIAS_{i};\n"
            for i in range(repeat)
        )

        enum_count = 1 + rng.randrange(8)
        enum_values = [rng.choice(SAFE_ENUM_VALUES) for _ in range(enum_count)]
        rng.shuffle(enum_values)
        enumerators = ",\n".join(
            f"    {ident}_ENUM_VALUE_{i} = {value}" for i, value in enumerate(enum_values)
        )
        enum_decl = (
            f"typedef enum {ident}_ENUM_TAG {{\n"
            f"{enumerators}\n"
            f"}} {ident}_ENUM;\n"
        )

        member_count = 1 + rng.randrange(6)
        record_members = "".join(
            f"    {rng.choice(SAFE_SCALAR_TYPES)} m_probe_{i}"
            f"{'[' + str(1 + rng.randrange(4)) + ']' if rng.randrange(3) == 0 else ''};\n"
            for i in range(member_count)
        )
        struct_decl = (
            f"struct {ident}_STRUCT {{\n"
            f"{record_members}"
            f"}};\n"
        )
        class_decl = (
            f"class {ident}_CLASS {{\n"
            f"public:\n{record_members}"
            f"}};\n"
        )

        pack_value = rng.choice((1, 2, 4, 8))
        packed_members = "".join(
            f"    {rng.choice(SAFE_SCALAR_TYPES)} m_packed_{i};\n"
            for i in range(2 + rng.randrange(5))
        )
        packed_decl = (
            f"#pragma pack(push, {pack_value})\n"
            f"struct {ident}_PACKED {{\n{packed_members}}};\n"
            f"#pragma pack(pop)\n"
        )

        method_count = 1 + rng.randrange(3)
        method_decls = "".join(
            f"    int ProbeDecl_{i}(int value);\n" for i in range(method_count)
        )
        body_constant = rng.choice((0, 1, 3, 7, 15, 31))
        member_decl = (
            f"class {ident}_MEMBERS {{\n"
            f"public:\n{method_decls}"
            f"    int ProbeIdentity(int value) {{ return value; }}\n"
            f"    unsigned long ProbeMix(unsigned long value) {{ return value ^ {body_constant}UL; }}\n"
            f"}};\n"
        )

        extern_decl = "".join(
            f"extern {rng.choice(SAFE_SCALAR_TYPES)} {ident}_EXTERN_{i};\n"
            for i in range(repeat)
        )
        static_values = [rng.choice(SAFE_ENUM_VALUES) for _ in range(repeat)]
        static_data = "".join(
            f"static {rng.choice(SAFE_SCALAR_TYPES)} {ident}_STATIC_{i} = {value};\n"
            for i, value in enumerate(static_values)
        )
        prototype_decl = "".join(
            f"int __fastcall {ident}_PROTOTYPE_{i}(int left, int right);\n"
            for i in range(repeat)
        )
        function_constant = rng.choice((0, 1, 3, 7, 15, 31, 63, 127))
        function_defs = (
            f"static int {ident}_FUNCTION_A(int value) {{ return value ^ {function_constant}; }}\n"
            f"static unsigned long {ident}_FUNCTION_B(unsigned long left, unsigned long right) "
            f"{{ return (left + right) ^ {function_constant}UL; }}\n"
        )
        include_count = 1 + rng.randrange(min(4, len(CURATED_INCLUDES)))
        include_choices = list(CURATED_INCLUDES)
        rng.shuffle(include_choices)
        includes = (
            "".join(f"#include {header}\n" for header in include_choices[:include_count])
            + f"typedef int {ident}_INCLUDE_MARKER;\n"
        )
        bodies = {
            "typedef": aliases,
            "enum": enum_decl,
            "struct": struct_decl,
            "class": class_decl,
            "packed": packed_decl,
            "member": member_decl,
            "extern": extern_decl,
            "static-data": static_data,
            "prototype": prototype_decl,
            "function": function_defs,
            "include": includes,
            "mixed": (
                includes + aliases + enum_decl + struct_decl + class_decl + packed_decl
                + member_decl + extern_decl + static_data + prototype_decl + function_defs
            ),
        }
        variants.append(Variant(trial, family, tag, bodies[family]))
    return variants


def insert_variant(original: str, target: Target, variant: Variant) -> str:
    block = variant.block(target.logical_line)
    return original[: target.insertion_offset] + block + original[target.insertion_offset :]


@contextmanager
def temporary_source(path: Path, original: bytes, candidate: bytes):
    """Expose one candidate to cl and unconditionally restore the exact original bytes."""
    path.write_bytes(candidate)
    try:
        yield
    finally:
        path.write_bytes(original)


@contextmanager
def measure_stage(timings: dict[str, dict[str, float | int]], stage: str):
    """Accumulate monotonic wall time for one diagnostic pipeline stage."""
    started = time.perf_counter()
    try:
        yield
    finally:
        entry = timings.setdefault(stage, {"seconds": 0.0, "calls": 0})
        entry["seconds"] += time.perf_counter() - started
        entry["calls"] += 1


def format_timings(timings: dict[str, dict[str, float | int]]) -> str:
    fields = []
    for stage, entry in timings.items():
        seconds = float(entry["seconds"])
        calls = int(entry["calls"])
        fields.append(f"{stage}={seconds:.3f}s/{calls} ({seconds / calls:.3f}s avg)")
    return "timings: " + "; ".join(fields)


def finalize_compiled_artifacts(
    scratch: tempfile.TemporaryDirectory,
    scratch_path: Path,
    final_path: Path,
    preserve_exact_closure: bool,
) -> Path | None:
    """Delete a compiled run by default, or atomically retain an audited exact closure."""
    if preserve_exact_closure:
        scratch_path.rename(final_path)
        scratch.cleanup()
        return final_path
    scratch.cleanup()
    return None


def _terminate_process_group(process: subprocess.Popen) -> tuple[str, str]:
    """Terminate and reap a process group created with ``start_new_session=True``."""
    try:
        os.killpg(process.pid, signal.SIGTERM)
    except ProcessLookupError:
        pass
    try:
        stdout, stderr = process.communicate(timeout=PROCESS_GROUP_TERMINATION_GRACE_SECONDS)
    except subprocess.TimeoutExpired:
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        stdout, stderr = process.communicate()
    else:
        # The session leader may have exited while a descendant closed its inherited
        # pipes and ignored SIGTERM. Fail closed by killing any remaining group member.
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
    return stdout or "", stderr or ""


def _run_command_with_timeout(
    command: list[str], cwd: Path, timeout_seconds: float
) -> tuple[int | None, str, str, bool]:
    process = subprocess.Popen(
        command,
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        start_new_session=True,
    )
    try:
        stdout, stderr = process.communicate(timeout=timeout_seconds)
        return process.returncode, stdout or "", stderr or "", False
    except subprocess.TimeoutExpired:
        stdout, stderr = _terminate_process_group(process)
        return process.returncode, stdout, stderr, True
    except BaseException:
        _terminate_process_group(process)
        raise


def compile_object(
    root: Path,
    source: Path,
    output: Path,
    flags: list[str],
    timeout_seconds: float,
) -> tuple[bool, str, bool]:
    command = [
        sys.executable,
        "-m",
        "homm2.build.cc_wrap",
        "--out",
        str(output),
        "--src",
        str(source),
        "--",
        *flags,
    ]
    returncode, stdout, stderr, timed_out = _run_command_with_timeout(
        command, root, timeout_seconds
    )
    log = stdout + stderr
    if timed_out:
        output.unlink(missing_ok=True)
        Path(str(output) + ".d").unlink(missing_ok=True)
        log += (
            f"\ncompile timed out after {timeout_seconds:g} seconds; "
            "terminated compiler process group\n"
        )
        return False, log, True
    return returncode == 0 and output.exists(), log, False


def objdiff_scores(
    target_obj: Path, candidate_obj: Path, symbol: str
) -> tuple[dict[str, float], dict[str, int], dict[str, int], str]:
    command = [
        "objdiff-cli", "diff", "-1", str(target_obj), "-2", str(candidate_obj), symbol,
        "-o", "-", "--format", "json",
    ]
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode:
        return {}, {}, {}, result.stdout + result.stderr
    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError:
        return {}, {}, {}, result.stdout + result.stderr
    entries = payload.get("right", {}).get("symbols", [])
    scores = {
        entry["name"]: float(entry["match_percent"])
        for entry in entries
        if isinstance(entry.get("name"), str) and entry.get("match_percent") is not None
    }
    sizes = {
        entry["name"]: int(str(entry["size"]), 0)
        for entry in entries
        if isinstance(entry.get("name"), str) and entry.get("size") is not None
    }
    counts = {}
    for entry in entries:
        name = entry.get("name")
        if isinstance(name, str):
            counts[name] = counts.get(name, 0) + 1
    return scores, sizes, counts, result.stderr


def object_metrics(path: Path) -> dict[str, dict]:
    _object_sha, rows = read_coff(path)
    return {
        row["function"]: {
            "size": row["size"],
            "text_sha": row["text_sha"],
            "relocs": row["relocs"],
            "reloc_sha": row["reloc_sha"],
            "reloc_stream": row["reloc_stream"],
            "reloc_detail_sha": row["reloc_detail_sha"],
            "reloc_stream_complete": row["reloc_stream_complete"],
        }
        for row in rows
    }


def exact_closure_rejections(
    score: float,
    candidate_size: int | None,
    retail_size: int,
    candidate_metrics: dict,
    retail_metrics: dict,
) -> list[str]:
    """Return fail-closed reasons why a 100% objdiff score cannot close the target."""
    out = []
    if score != 100.0:
        out.append("unrounded objdiff score is not exactly 100.0")
    if candidate_size != retail_size:
        out.append(f"target size is not exact: candidate {candidate_size}, retail {retail_size}")
    if not candidate_metrics.get("reloc_stream_complete"):
        out.append("candidate ordered relocation addends are not fully decoded")
    if not retail_metrics.get("reloc_stream_complete"):
        out.append("retail ordered relocation addends are not fully decoded")
    if candidate_metrics.get("reloc_stream") != retail_metrics.get("reloc_stream"):
        out.append("ordered relocation offsets/types/identities/addends differ from retail")
    return out


def _regressions(baseline: dict[str, float], candidate: dict[str, float], target: str) -> list[str]:
    out = []
    for symbol, score in baseline.items():
        if symbol == target:
            continue
        current = candidate.get(symbol)
        if current is None:
            out.append(f"missing sibling {symbol}")
        elif current < score - 1e-6:
            out.append(f"sibling {symbol}: {score:.6f} -> {current:.6f}")
    return out


def _exact_sibling_metric_regressions(
    baseline_scores: dict[str, float], baseline_metrics: dict[str, dict], candidate_metrics: dict[str, dict], target: str
) -> list[str]:
    out = []
    for symbol, score in baseline_scores.items():
        if symbol == target or score < 100.0 - 1e-9 or symbol not in baseline_metrics:
            continue
        if candidate_metrics.get(symbol) != baseline_metrics[symbol]:
            out.append(f"exact sibling raw/reloc metrics changed: {symbol}")
    return out


def predecessor_symbols(target: Target, root: Path) -> set[str]:
    with (root / "build/gen/symbol_names.csv").open(newline="") as handle:
        return {
            row["name"] for row in csv.DictReader(handle)
            if row["kind"] == "func" and row["unit"] == target.unit and int(row["rva"], 0) < target.rva
        }


def _predecessor_regressions(
    predecessors: set[str], baseline_metrics: dict[str, dict], candidate_metrics: dict[str, dict]
) -> list[str]:
    return [
        f"predecessor raw/reloc metrics changed: {symbol}"
        for symbol in sorted(predecessors)
        if symbol in baseline_metrics and candidate_metrics.get(symbol) != baseline_metrics[symbol]
    ]


def record_target_max(
    baseline_path: Path,
    unit: str,
    symbol: str,
    current_hash: str | None,
    new_score: float | None,
) -> dict:
    """Validate one retained-max row and move only its max field to exact 100.

    All non-target bytes and all other target-row fields are preserved exactly.  Validation
    happens even when *new_score* is None or sub-100, so ``--record-max`` never silently
    accepts a missing, duplicate, or stale-hash ledger.
    """
    original = baseline_path.read_bytes()
    lines = original.splitlines(keepends=True)
    matches = []
    for index, line in enumerate(lines):
        body = line.rstrip(b"\r\n")
        if not body or body.startswith(b"#"):
            continue
        fields = body.split(b"\t")
        if len(fields) >= 2 and fields[0].decode("utf-8") == unit and fields[1].decode("utf-8") == symbol:
            matches.append((index, line, fields))
    if not matches:
        raise BaselineUpdateError(f"missing baseline row for {unit}::{symbol}")
    if len(matches) != 1:
        raise BaselineUpdateError(f"duplicate baseline rows for {unit}::{symbol}")
    index, line, fields = matches[0]
    if len(fields) < 4 or not fields[3]:
        raise BaselineUpdateError(f"baseline row has no source hash for {unit}::{symbol}")
    stored_hash = fields[3].decode("utf-8")
    if current_hash is None:
        raise BaselineUpdateError(f"current normalized source hash is missing for {unit}::{symbol}")
    if current_hash != stored_hash:
        raise BaselineUpdateError(
            f"source hash mismatch for {unit}::{symbol}: baseline {stored_hash}, current {current_hash}"
        )
    try:
        old_max = float(fields[2])
    except (IndexError, ValueError) as exc:
        raise BaselineUpdateError(f"invalid baseline max for {unit}::{symbol}") from exc
    if not math.isfinite(old_max) or not 0.0 <= old_max <= 100.0:
        raise BaselineUpdateError(f"invalid baseline max for {unit}::{symbol}: {old_max}")
    result = {
        "requested": True,
        "updated": False,
        "unit": unit,
        "symbol": symbol,
        "source_hash": current_hash,
        "old_max": old_max,
        "new_max": old_max,
    }
    if new_score is None:
        result["reason"] = "no_exact_closure"
        return result
    if not math.isfinite(new_score) or not 0.0 <= new_score <= 100.0:
        raise BaselineUpdateError(f"invalid exact-closure score for {unit}::{symbol}: {new_score}")
    result["observed_score"] = new_score
    if new_score != 100.0:
        result["reason"] = "sub_100_is_disposable"
        return result
    if old_max == 100.0:
        result["reason"] = "already_exact"
        return result
    formatted_score = "100.0000"
    written_max = 100.0
    ending = line[len(line.rstrip(b"\r\n")) :]
    replacement_fields = list(fields)
    replacement_fields[2] = formatted_score.encode("ascii")
    lines[index] = b"\t".join(replacement_fields) + ending
    updated = b"".join(lines)
    if updated == original:
        result["reason"] = "already_exact"
        return result

    mode = baseline_path.stat().st_mode & 0o777
    temporary_name = None
    try:
        with tempfile.NamedTemporaryFile(dir=baseline_path.parent, delete=False) as handle:
            temporary_name = Path(handle.name)
            handle.write(updated)
            handle.flush()
            os.fsync(handle.fileno())
        os.chmod(temporary_name, mode)
        os.replace(temporary_name, baseline_path)
    finally:
        if temporary_name is not None and temporary_name.exists():
            temporary_name.unlink()
    result.update({"updated": True, "new_max": written_max, "reason": "exact_closure"})
    return result


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("--source", required=True, type=Path, help="configured TU source path")
    parser.add_argument("--rva", required=True, type=parse_int, help="exact CodeView RVA or image VA")
    parser.add_argument("--trials", type=int, default=30, help="number of deterministic trials")
    parser.add_argument(
        "--compile-timeout-seconds",
        type=positive_seconds,
        default=DEFAULT_COMPILE_TIMEOUT_SECONDS,
        help=(
            "maximum seconds for each baseline/trial compile; a timeout terminates the "
            "compiler process group (default: 120)"
        ),
    )
    parser.add_argument("--seed", type=parse_int, default=0x484F4D32)
    parser.add_argument(
        "--families", default=",".join(DEFAULT_FAMILIES),
        help=f"comma-separated subset of {','.join(DEFAULT_FAMILIES)}",
    )
    parser.add_argument("--output", type=Path, help="artifact directory (default: build/tu-state-noise/...)")
    parser.add_argument("--dry-run", action="store_true", help="resolve target and emit snippets without compiling")
    parser.add_argument(
        "--record-max", action="store_true",
        help="after restoration, set only this target's max to 100 for an audited exact closure",
    )
    args = parser.parse_args(argv)
    if args.trials < 1:
        parser.error("--trials must be positive")
    if args.dry_run and args.record_max:
        parser.error("--record-max requires compiled trials, not --dry-run")

    root = Path(os.environ.get("HOMM2_DIR", Path.cwd())).resolve()
    try:
        target, flags = resolve_target(root, args.source, args.rva)
        families = tuple(item.strip() for item in args.families.split(",") if item.strip())
        variants = make_variants(args.trials, families, args.seed)
    except (OSError, KeyError, ValueError) as exc:
        parser.error(str(exc))

    stamp = time.strftime("%Y%m%d-%H%M%S")
    default_name = f"{stamp}-{target.unit.replace('/', '-')}-0x{target.rva:x}"
    final_output = (
        (root / args.output).resolve()
        if args.output
        else root / "build/tu-state-noise" / default_name
    )
    source_rel = target.source.relative_to(root)
    original_bytes = target.source.read_bytes()
    original = original_bytes.decode("utf-8")
    from homm2.match.status import source_hashes as project_source_hashes

    target_key = (target.unit, target.symbol)
    canonical_target_hash = project_source_hashes().get(target_key)
    if canonical_target_hash is None:
        parser.error(f"normalized source hash is missing for {target.unit}::{target.symbol}")
    canonical_target_tokens = target_identifiers(original, target)
    target_token_digest = sha256_bytes("\n".join(sorted(canonical_target_tokens)).encode("utf-8"))
    canonical_target_suffix_digest = target_suffix_digest(original, target.va)
    if canonical_target_suffix_digest is None:
        parser.error(f"target VA marker is not uniquely identifiable in {source_rel}")
    predecessors = predecessor_symbols(target, root)

    target_obj = root / "build/delink" / f"{target.unit}.c.obj"
    if not args.dry_run and not target_obj.exists():
        parser.error(f"retail object is missing: {target_obj}")

    scratch = None
    if args.dry_run:
        output = final_output
        output.mkdir(parents=True, exist_ok=False)
    else:
        final_output.parent.mkdir(parents=True, exist_ok=True)
        if final_output.exists():
            parser.error(f"output path already exists: {final_output}")
        scratch = tempfile.TemporaryDirectory(
            prefix=f".{final_output.name}.tmp-", dir=final_output.parent
        )
        output = Path(scratch.name)
    (output / "original.cpp").write_bytes(original_bytes)
    manifest = {
        "schema": 1,
        "mode": "dry-run-non-matching-diagnostic" if args.dry_run else "compile",
        "root": str(root),
        "git_head": subprocess.run(
            ["git", "rev-parse", "HEAD"], cwd=root, capture_output=True, text=True
        ).stdout.strip(),
        "source": str(source_rel),
        "source_sha256": sha256_bytes(original_bytes),
        "target": {
            "unit": target.unit,
            "rva": f"0x{target.rva:x}",
            "va": f"0x{target.va:08x}",
            "symbol": target.symbol,
            "codeview_size": target.retail_size,
            "logical_insertion_line": target.logical_line,
            "canonical_source_hash": canonical_target_hash,
            "canonical_identifier_digest": target_token_digest,
        },
        "compiler_flags": flags,
        "compile_timeout_seconds": args.compile_timeout_seconds,
        "seed": args.seed,
        "policy": {
            "parser_visible_temporary_probes": True,
            "probe_symbols_or_storage_may_exist_only_in_candidate_object": True,
            "source_restored_after_every_trial": True,
            "generated_noise_retained_in_source": False,
            "default_repository_mutation": False,
            "sub_100_results_are_disposable": True,
            "record_max_requires_unrounded_exact_100_size_and_ordered_relocations": True,
            "sibling_score_regressions_allowed": False,
            "exact_sibling_raw_or_reloc_changes_allowed": False,
            "target_size_or_reloc_count_distance_may_not_worsen": True,
            "compiler_process_group_terminated_on_timeout": True,
        },
        "baseline": None,
        "trials": [],
        "best_observed_disposable": None,
        "exact_closure": None,
        "record_max": {"requested": args.record_max, "updated": False},
    }
    timings = {}
    manifest["timings"] = timings

    if args.dry_run:
        for variant in variants:
            snippet_path = output / f"trial-{variant.trial:04d}-{variant.family}.snippet"
            snippet_path.write_text(variant.block(target.logical_line))
            manifest["trials"].append({**asdict(variant), "snippet": snippet_path.name})
        manifest["source_restored"] = target.source.read_bytes() == original_bytes
        restored_target_hash = project_source_hashes().get(target_key)
        manifest["target_source_hash_restored"] = restored_target_hash == canonical_target_hash
        manifest["restored_target_source_hash"] = restored_target_hash
        (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
        print(f"dry-run non-matching diagnostic: {len(variants)} auditable variants in {output}")
        return 0

    assert scratch is not None

    baseline_obj = output / "baseline.obj"
    with measure_stage(timings, "baseline_compile"):
        ok, log, baseline_timed_out = compile_object(
            root, target.source, baseline_obj, flags, args.compile_timeout_seconds
        )
    (output / "baseline.compile.log").write_text(log)
    if not ok:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        reason = "timed out" if baseline_timed_out else "failed"
        print(f"baseline compile {reason}; disposable artifacts removed", file=sys.stderr)
        return 2
    with measure_stage(timings, "baseline_objdiff"):
        baseline_scores, baseline_sizes, baseline_counts, diff_log = objdiff_scores(
            target_obj, baseline_obj, target.symbol
        )
    (output / "baseline.objdiff.log").write_text(diff_log)
    with measure_stage(timings, "baseline_coff_metrics"):
        baseline_metrics = object_metrics(baseline_obj)
        retail_metrics = object_metrics(target_obj)
    if baseline_counts.get(target.symbol) != 1:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print(f"target symbol is not unique in baseline objdiff: {target.symbol}", file=sys.stderr)
        return 2
    if target.symbol not in baseline_scores or target.symbol not in baseline_metrics:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print(f"target symbol absent from baseline object: {target.symbol}", file=sys.stderr)
        return 2
    if target.symbol not in retail_metrics:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print(f"target symbol absent from retail object: {target.symbol}", file=sys.stderr)
        return 2
    baseline_target = baseline_metrics[target.symbol]
    baseline_target["objdiff_size"] = baseline_sizes.get(target.symbol)
    retail_target = retail_metrics.get(target.symbol, {})
    retail_target["codeview_size"] = target.retail_size
    baseline_score = baseline_scores[target.symbol]
    manifest["baseline"] = {
        "score": baseline_score,
        "candidate": baseline_target,
        "retail": retail_target,
    }
    print(
        f"target {target.unit} {target.symbol} RVA 0x{target.rva:x}: "
        f"baseline {baseline_score:.6f}% size {baseline_target['size']} "
        f"relocs {baseline_target['relocs']}/{retail_target.get('relocs', '?')}",
        flush=True,
    )

    best_observed = None
    exact_closure = None
    rows = []
    interrupted = False
    old_term = signal.getsignal(signal.SIGTERM)

    def stop_for_signal(_signum, _frame):
        raise KeyboardInterrupt

    signal.signal(signal.SIGTERM, stop_for_signal)
    try:
        for variant in variants:
            candidate = insert_variant(original, target, variant)
            candidate_bytes = candidate.encode("utf-8")
            trial_obj = output / f"trial-{variant.trial:04d}.obj"
            include_guard = include_macro_guard(root, variant.body, canonical_target_tokens)
            compile_timed_out = False
            with temporary_source(target.source, original_bytes, candidate_bytes):
                with measure_stage(timings, "target_hash_check"):
                    candidate_target_hash = (
                        canonical_target_hash
                        if target_suffix_digest(candidate, target.va) == canonical_target_suffix_digest
                        else None
                    )
                if candidate_target_hash != canonical_target_hash:
                    ok = False
                    compile_log = (
                        "canonical target normalized source hash changed: "
                        f"{canonical_target_hash} -> {candidate_target_hash}\n"
                    )
                elif not include_guard.get("passed", True):
                    ok = False
                    compile_log = "include macro guard rejected candidate: " + json.dumps(include_guard) + "\n"
                else:
                    with measure_stage(timings, "trial_compile"):
                        ok, compile_log, compile_timed_out = compile_object(
                            root,
                            target.source,
                            trial_obj,
                            flags,
                            args.compile_timeout_seconds,
                        )
            trial = {
                **asdict(variant),
                "source_sha256": sha256_bytes(candidate_bytes),
                "compiled": ok,
                "compile_timed_out": compile_timed_out,
                "score": None,
                "score_delta": None,
                "candidate": None,
                "canonical_target_source_hash": candidate_target_hash,
                "include_macro_guard": include_guard,
                "eligible": False,
                "exact_closure_eligible": False,
                "exact_closure_rejections": [],
                "rejections": [],
            }
            if not ok:
                (output / f"trial-{variant.trial:04d}.compile.log").write_text(compile_log)
                if candidate_target_hash != canonical_target_hash:
                    trial["rejections"].append("canonical target normalized source hash changed")
                elif not include_guard.get("passed", True):
                    trial["rejections"].append("include macro guard failed")
                elif compile_timed_out:
                    trial["rejections"].append("compile timed out")
                else:
                    trial["rejections"].append("compile failed")
            else:
                with measure_stage(timings, "trial_objdiff"):
                    scores, sizes, symbol_counts, trial_diff_log = objdiff_scores(
                        target_obj, trial_obj, target.symbol
                    )
                if trial_diff_log:
                    (output / f"trial-{variant.trial:04d}.objdiff.log").write_text(trial_diff_log)
                with measure_stage(timings, "trial_coff_metrics"):
                    metrics = object_metrics(trial_obj)
                score = scores.get(target.symbol)
                target_metrics = metrics.get(target.symbol)
                if symbol_counts.get(target.symbol) != 1:
                    trial["rejections"].append("target symbol is not uniquely identifiable")
                if score is None or target_metrics is None:
                    trial["rejections"].append("target absent from candidate object/diff")
                else:
                    target_metrics["objdiff_size"] = sizes.get(target.symbol)
                    trial["score"] = score
                    trial["score_delta"] = score - baseline_score
                    trial["candidate"] = target_metrics
                    with measure_stage(timings, "regression_gates"):
                        trial["rejections"].extend(
                            _regressions(baseline_scores, scores, target.symbol)
                        )
                        trial["rejections"].extend(
                            _exact_sibling_metric_regressions(
                                baseline_scores, baseline_metrics, metrics, target.symbol
                            )
                        )
                        trial["rejections"].extend(
                            _predecessor_regressions(predecessors, baseline_metrics, metrics)
                        )
                        candidate_size = target_metrics.get("objdiff_size")
                        baseline_size = baseline_target.get("objdiff_size")
                        if candidate_size is None or baseline_size is None:
                            trial["rejections"].append("objdiff function size unavailable")
                        elif abs(candidate_size - target.retail_size) > abs(
                            baseline_size - target.retail_size
                        ):
                            trial["rejections"].append("target size distance from retail worsened")
                        retail_relocs = retail_target.get("relocs")
                        if retail_relocs is not None and abs(
                            target_metrics["relocs"] - retail_relocs
                        ) > abs(baseline_target["relocs"] - retail_relocs):
                            trial["rejections"].append(
                                "target relocation-count distance from retail worsened"
                            )
                        trial["eligible"] = not trial["rejections"]
                        trial["exact_closure_rejections"] = exact_closure_rejections(
                            score,
                            candidate_size,
                            target.retail_size,
                            target_metrics,
                            retail_target,
                        )
                        trial["exact_closure_eligible"] = (
                            trial["eligible"] and not trial["exact_closure_rejections"]
                        )
                    if trial["exact_closure_eligible"] and exact_closure is None:
                        exact_closure = trial
                    if trial["eligible"] and score > baseline_score + 1e-6:
                        if best_observed is None or score > best_observed["score"] + 1e-6:
                            best_observed = trial
                trial_obj.unlink(missing_ok=True)
                Path(str(trial_obj) + ".d").unlink(missing_ok=True)
            manifest["trials"].append(trial)
            rows.append(
                f"{variant.trial}\t{variant.family}\t"
                f"{trial['score'] if trial['score'] is not None else 'NA'}\t"
                f"{trial['score_delta'] if trial['score_delta'] is not None else 'NA'}\t"
                f"{int(trial['eligible'])}\t{' | '.join(trial['rejections'])}\n"
            )
            state = "eligible" if trial["eligible"] else "rejected"
            score_text = "compile-failed" if trial["score"] is None else f"{trial['score']:.6f}%"
            print(f"[{variant.trial:04d}/{len(variants):04d}] {variant.family}: {score_text} {state}", flush=True)
    except KeyboardInterrupt:
        interrupted = True
        manifest["interrupted"] = True
        print("probe interrupted; restoring source and removing disposable artifacts", file=sys.stderr)
    finally:
        # This is deliberately redundant with temporary_source: it also covers exceptions
        # between trials and makes the postcondition explicit.
        target.source.write_bytes(original_bytes)
        signal.signal(signal.SIGTERM, old_term)

    (output / "trials.tsv").write_text(
        "trial\tfamily\tscore\tdelta\teligible\trejections\n" + "".join(rows)
    )
    if best_observed is not None:
        manifest["best_observed_disposable"] = {
            "trial": best_observed["trial"],
            "family": best_observed["family"],
            "score": best_observed["score"],
            "score_delta": best_observed["score_delta"],
            "candidate": best_observed["candidate"],
            "source_hash_unchanged": True,
            "generated_noise_retained": False,
        }
    if exact_closure is not None:
        manifest["exact_closure"] = {
            "trial": exact_closure["trial"],
            "family": exact_closure["family"],
            "tag": exact_closure["tag"],
            "score": exact_closure["score"],
            "body": exact_closure["body"],
            "target_size": exact_closure["candidate"]["objdiff_size"],
            "reloc_detail_sha": exact_closure["candidate"]["reloc_detail_sha"],
            "source_hash_unchanged": True,
            "generated_noise_retained": False,
        }
    manifest["source_restored"] = target.source.read_bytes() == original_bytes
    restored_target_hash = project_source_hashes().get(target_key)
    manifest["target_source_hash_restored"] = restored_target_hash == canonical_target_hash
    manifest["restored_target_source_hash"] = restored_target_hash
    if not manifest["source_restored"] or not manifest["target_source_hash_restored"]:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print("FATAL: source or normalized target-hash restoration check failed", file=sys.stderr)
        return 3
    record_error = False
    if args.record_max and interrupted:
        manifest["record_max"] = {
            "requested": True,
            "updated": False,
            "reason": "search_interrupted",
        }
    elif args.record_max:
        try:
            manifest["record_max"] = record_target_max(
                root / "config/match_baseline.tsv",
                target.unit,
                target.symbol,
                restored_target_hash,
                exact_closure["score"] if exact_closure is not None else None,
            )
        except (OSError, BaselineUpdateError) as exc:
            record_error = True
            manifest["record_max"] = {
                "requested": True,
                "updated": False,
                "refused": True,
                "error": str(exc),
            }
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(format_timings(timings), flush=True)
    if record_error:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        print(f"record-max refused: {manifest['record_max']['error']}", file=sys.stderr)
        return 4
    if interrupted:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        return 130

    if exact_closure is None:
        finalize_compiled_artifacts(scratch, output, final_output, False)
        if best_observed is None:
            print("no audited exact closure; source restored; disposable artifacts removed")
        else:
            print(
                f"best observed (disposable) {baseline_score:.6f}% -> "
                f"{best_observed['score']:.6f}% (trial {best_observed['trial']}); "
                "no audited exact closure; source restored; artifacts removed",
            )
        if args.record_max:
            state = manifest["record_max"]
            print(f"no exact closure retained; baseline unchanged: {state['reason']}")
        return 0

    retained_output = finalize_compiled_artifacts(scratch, output, final_output, True)
    assert retained_output is not None
    print(
        f"audited exact closure 100.0000% (trial {exact_closure['trial']}); "
        f"source restored; reproducible artifact preserved: {retained_output}"
    )
    if args.record_max:
        state = manifest["record_max"]
        if state["updated"]:
            print(
                f"exact closure retained {state['old_max']:.4f}% -> 100.0000% "
                f"for unchanged source hash {state['source_hash']}; generated probe not retained"
            )
        else:
            print(f"exact closure already retained; baseline unchanged: {state['reason']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
