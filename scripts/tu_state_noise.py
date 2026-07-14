#!/usr/bin/env python3
"""Probe semantically inert preprocessor noise before one MSVC 4.2 function.

This is a deliberately narrow last-mile tool.  It does not rewrite a function and it
does not create declarations, symbols, loads, or relocations.  Each trial inserts a
small, balanced preprocessor-only block before the target's ``VA`` metadata block,
compiles the real translation unit with VC 4.2, scores the requested symbol with
objdiff, and restores the source immediately.

The source is unchanged on normal exit, compiler failure, or Ctrl-C.  Results, exact
snippets, and COFF metrics are written to ``build/tu-state-noise``.  Generated noise
is evidence only and is never written back to reconstructed source.

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
DEFAULT_FAMILIES = ("macro", "conditional", "warning", "comment", "mixed")
SAFE_MACRO_VALUES = ("0", "1", "(-1)", "(0x13579bdfUL)", "probe_token", "(1 + 2)")
SAFE_WARNING_CODES = (4100, 4189, 4505)


class BaselineUpdateError(ValueError):
    pass


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def parse_int(value: str) -> int:
    try:
        return int(value, 0)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid integer: {value}") from exc


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
        return (
            f"// h2-tu-state-probe {self.tag} ({self.family}); generated evidence only\n"
            f"{self.body}"
            f"#line {logical_line}\n"
        )


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


def logical_line_at(text: str, offset: int) -> int:
    """Return the logical line number of the source line beginning at *offset*."""
    logical = 1
    for line in text[:offset].splitlines(keepends=True):
        match = _LINE_DIRECTIVE.match(line)
        logical = int(match.group(1)) if match else logical + 1
    return logical


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
    positions = [match.start() for match in re.finditer(re.escape(marker), text)]
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
    unknown = sorted(set(selected) - set(DEFAULT_FAMILIES))
    if unknown:
        raise ValueError(f"unknown noise families: {', '.join(unknown)}")
    if not selected:
        raise ValueError("at least one noise family is required")
    rng = random.Random(seed)
    variants = []
    for trial in range(1, count + 1):
        family = selected[(trial - 1) % len(selected)]
        tag = f"{seed:08x}-{trial:04d}-{rng.getrandbits(32):08x}"
        macro_name = f"HOMM2_TU_STATE_PROBE_{tag.replace('-', '_').upper()}"
        repeat = 1 + rng.randrange(4)
        macro = "".join(
            f"#define {macro_name}_{i} {rng.choice(SAFE_MACRO_VALUES)}\n"
            f"#undef {macro_name}_{i}\n"
            for i in range(repeat)
        )
        conditional = "".join("#if 1\n#endif\n" for _ in range(repeat))
        warning = "".join(
            "#ifdef _MSC_VER\n"
            "#pragma warning(push)\n"
            f"#pragma warning(disable: {rng.choice(SAFE_WARNING_CODES)})\n"
            "#pragma warning(pop)\n"
            "#endif\n"
            for _ in range(repeat)
        )
        comment = "".join(
            f"/* tu-state lexical padding {tag} {i} {'x' * (1 + rng.randrange(31))} */\n"
            for i in range(repeat)
        )
        bodies = {
            "macro": macro,
            "conditional": conditional,
            "warning": warning,
            "comment": comment,
            "mixed": comment + macro + conditional + warning,
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


def compile_object(root: Path, source: Path, output: Path, flags: list[str]) -> tuple[bool, str]:
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
    result = subprocess.run(command, cwd=root, capture_output=True, text=True)
    log = result.stdout + result.stderr
    return result.returncode == 0 and output.exists(), log


def objdiff_scores(
    target_obj: Path, candidate_obj: Path, symbol: str
) -> tuple[dict[str, float], dict[str, int], str]:
    command = [
        "objdiff-cli", "diff", "-1", str(target_obj), "-2", str(candidate_obj), symbol,
        "-o", "-", "--format", "json",
    ]
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode:
        return {}, {}, result.stdout + result.stderr
    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError:
        return {}, {}, result.stdout + result.stderr
    scores = {
        entry["name"]: float(entry["match_percent"])
        for entry in payload.get("right", {}).get("symbols", [])
        if isinstance(entry.get("name"), str) and entry.get("match_percent") is not None
    }
    sizes = {
        entry["name"]: int(str(entry["size"]), 0)
        for entry in payload.get("right", {}).get("symbols", [])
        if isinstance(entry.get("name"), str) and entry.get("size") is not None
    }
    return scores, sizes, result.stderr


def object_metrics(path: Path) -> dict[str, dict]:
    _object_sha, rows = read_coff(path)
    return {
        row["function"]: {
            "size": row["size"],
            "text_sha": row["text_sha"],
            "relocs": row["relocs"],
            "reloc_sha": row["reloc_sha"],
        }
        for row in rows
    }


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


def _predecessor_regressions(
    target: Target, baseline_metrics: dict[str, dict], candidate_metrics: dict[str, dict], root: Path
) -> list[str]:
    with (root / "build/gen/symbol_names.csv").open(newline="") as handle:
        predecessors = {
            row["name"] for row in csv.DictReader(handle)
            if row["kind"] == "func" and row["unit"] == target.unit and int(row["rva"], 0) < target.rva
        }
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
    """Validate one retained-max row and, if higher, replace only its max field.

    All non-target bytes and all other target-row fields are preserved exactly.  Validation
    happens even when *new_score* is None or not higher, so ``--record-max`` never silently
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
        result["reason"] = "no_eligible_improvement"
        return result
    if not math.isfinite(new_score) or not 0.0 <= new_score <= 100.0:
        raise BaselineUpdateError(f"invalid eligible best for {unit}::{symbol}: {new_score}")
    result["eligible_best"] = new_score
    if new_score <= old_max:
        result["reason"] = "not_above_stored_max"
        return result

    formatted_score = f"{new_score:.4f}"
    written_max = float(formatted_score)
    if written_max <= old_max:
        result["reason"] = "not_above_stored_precision"
        return result
    ending = line[len(line.rstrip(b"\r\n")) :]
    replacement_fields = list(fields)
    replacement_fields[2] = formatted_score.encode("ascii")
    lines[index] = b"\t".join(replacement_fields) + ending
    updated = b"".join(lines)
    if updated == original:
        result["reason"] = "not_above_stored_precision"
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
    result.update({"updated": True, "new_max": written_max, "reason": "raised"})
    return result


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("--source", required=True, type=Path, help="configured TU source path")
    parser.add_argument("--rva", required=True, type=parse_int, help="exact CodeView RVA or image VA")
    parser.add_argument("--trials", type=int, default=30, help="number of deterministic trials")
    parser.add_argument("--seed", type=parse_int, default=0x484F4D32)
    parser.add_argument(
        "--families", default=",".join(DEFAULT_FAMILIES),
        help=f"comma-separated subset of {','.join(DEFAULT_FAMILIES)}",
    )
    parser.add_argument("--output", type=Path, help="artifact directory (default: build/tu-state-noise/...)")
    parser.add_argument("--dry-run", action="store_true", help="resolve target and emit snippets without compiling")
    parser.add_argument(
        "--record-max", action="store_true",
        help="after source restoration, raise only this target's retained max when an eligible best exceeds it",
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
    output = (root / args.output).resolve() if args.output else root / "build/tu-state-noise" / default_name
    output.mkdir(parents=True, exist_ok=False)
    source_rel = target.source.relative_to(root)
    original_bytes = target.source.read_bytes()
    original = original_bytes.decode("utf-8")
    (output / "original.cpp").write_bytes(original_bytes)
    manifest = {
        "schema": 1,
        "mode": "dry-run" if args.dry_run else "compile",
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
        },
        "compiler_flags": flags,
        "seed": args.seed,
        "policy": {
            "parser_visible_declarations": False,
            "runtime_or_linkage_effects": False,
            "source_restored_after_every_trial": True,
            "generated_noise_retained_in_source": False,
            "default_repository_mutation": False,
            "sibling_score_regressions_allowed": False,
            "exact_sibling_raw_or_reloc_changes_allowed": False,
            "target_size_or_reloc_count_distance_may_not_worsen": True,
        },
        "baseline": None,
        "trials": [],
        "best": None,
        "record_max": {"requested": args.record_max, "updated": False},
    }

    if args.dry_run:
        for variant in variants:
            snippet_path = output / f"trial-{variant.trial:04d}-{variant.family}.snippet"
            snippet_path.write_text(variant.block(target.logical_line))
            manifest["trials"].append({**asdict(variant), "snippet": snippet_path.name})
        manifest["source_restored"] = target.source.read_bytes() == original_bytes
        (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
        print(f"dry-run: {len(variants)} auditable variants in {output}")
        return 0

    target_obj = root / "build/delink" / f"{target.unit}.c.obj"
    if not target_obj.exists():
        parser.error(f"retail object is missing: {target_obj}")

    baseline_obj = output / "baseline.obj"
    ok, log = compile_object(root, target.source, baseline_obj, flags)
    (output / "baseline.compile.log").write_text(log)
    if not ok:
        print(f"baseline compile failed; see {output / 'baseline.compile.log'}", file=sys.stderr)
        return 2
    baseline_scores, baseline_sizes, diff_log = objdiff_scores(target_obj, baseline_obj, target.symbol)
    (output / "baseline.objdiff.log").write_text(diff_log)
    baseline_metrics = object_metrics(baseline_obj)
    retail_metrics = object_metrics(target_obj)
    if target.symbol not in baseline_scores or target.symbol not in baseline_metrics:
        print(f"target symbol absent from baseline object: {target.symbol}", file=sys.stderr)
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

    best = None
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
            with temporary_source(target.source, original_bytes, candidate_bytes):
                ok, compile_log = compile_object(root, target.source, trial_obj, flags)
            trial = {
                **asdict(variant),
                "source_sha256": sha256_bytes(candidate_bytes),
                "compiled": ok,
                "score": None,
                "score_delta": None,
                "candidate": None,
                "eligible": False,
                "rejections": [],
            }
            if not ok:
                (output / f"trial-{variant.trial:04d}.compile.log").write_text(compile_log)
                trial["rejections"].append("compile failed")
            else:
                scores, sizes, trial_diff_log = objdiff_scores(target_obj, trial_obj, target.symbol)
                if trial_diff_log:
                    (output / f"trial-{variant.trial:04d}.objdiff.log").write_text(trial_diff_log)
                metrics = object_metrics(trial_obj)
                score = scores.get(target.symbol)
                target_metrics = metrics.get(target.symbol)
                if score is None or target_metrics is None:
                    trial["rejections"].append("target absent from candidate object/diff")
                else:
                    target_metrics["objdiff_size"] = sizes.get(target.symbol)
                    trial["score"] = score
                    trial["score_delta"] = score - baseline_score
                    trial["candidate"] = target_metrics
                    trial["rejections"].extend(_regressions(baseline_scores, scores, target.symbol))
                    trial["rejections"].extend(
                        _exact_sibling_metric_regressions(
                            baseline_scores, baseline_metrics, metrics, target.symbol
                        )
                    )
                    trial["rejections"].extend(
                        _predecessor_regressions(target, baseline_metrics, metrics, root)
                    )
                    candidate_size = target_metrics.get("objdiff_size")
                    baseline_size = baseline_target.get("objdiff_size")
                    if candidate_size is None or baseline_size is None:
                        trial["rejections"].append("objdiff function size unavailable")
                    elif abs(candidate_size - target.retail_size) > abs(baseline_size - target.retail_size):
                        trial["rejections"].append("target size distance from retail worsened")
                    retail_relocs = retail_target.get("relocs")
                    if retail_relocs is not None and abs(target_metrics["relocs"] - retail_relocs) > abs(
                        baseline_target["relocs"] - retail_relocs
                    ):
                        trial["rejections"].append("target relocation-count distance from retail worsened")
                    trial["eligible"] = not trial["rejections"]
                    if trial["eligible"] and score > baseline_score + 1e-6:
                        if best is None or score > best["score"] + 1e-6:
                            best = trial
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
        print("probe interrupted; restoring source and writing partial evidence", file=sys.stderr)
    finally:
        # This is deliberately redundant with temporary_source: it also covers exceptions
        # between trials and makes the postcondition explicit.
        target.source.write_bytes(original_bytes)
        signal.signal(signal.SIGTERM, old_term)

    (output / "trials.tsv").write_text(
        "trial\tfamily\tscore\tdelta\teligible\trejections\n" + "".join(rows)
    )
    if best is not None:
        manifest["best"] = {
            "trial": best["trial"],
            "family": best["family"],
            "score": best["score"],
            "score_delta": best["score_delta"],
            "candidate": best["candidate"],
            "source_hash_unchanged": True,
            "generated_noise_retained": False,
        }
    manifest["source_restored"] = target.source.read_bytes() == original_bytes
    if not manifest["source_restored"]:
        (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
        print("FATAL: source restoration check failed", file=sys.stderr)
        return 3
    record_error = False
    if args.record_max and interrupted:
        manifest["record_max"] = {
            "requested": True,
            "updated": False,
            "reason": "search_interrupted",
        }
    elif args.record_max:
        from homm2.match.status import source_hashes as project_source_hashes

        current_hash = project_source_hashes().get((target.unit, target.symbol))
        try:
            manifest["record_max"] = record_target_max(
                root / "config/match_baseline.tsv",
                target.unit,
                target.symbol,
                current_hash,
                best["score"] if best is not None else None,
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
    if record_error:
        print(f"record-max refused: {manifest['record_max']['error']}", file=sys.stderr)
        return 4
    if interrupted:
        return 130
    if best is None:
        print(f"no eligible improvement; source restored; evidence: {output}")
    else:
        print(
            f"best {baseline_score:.6f}% -> {best['score']:.6f}% (trial {best['trial']}); "
            f"source restored; generated noise retained only in {output / 'manifest.json'}",
        )
        if best["score"] >= 100.0 - 1e-9:
            print("100% is relocation-masked; the evidence still requires raw-byte and relocation review.")
    if args.record_max:
        state = manifest["record_max"]
        if state["updated"]:
            print(
                f"retained max raised {state['old_max']:.4f}% -> {state['new_max']:.4f}% "
                f"for unchanged source hash {state['source_hash']}"
            )
        else:
            print(f"retained max unchanged: {state['reason']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
