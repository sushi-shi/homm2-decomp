#!/usr/bin/env python3
"""Source-shape experiment runner.

It performs only byte-exact substitutions declared in a JSON manifest, so every option is
authored or AST-generated and semantically reviewable before the run. No regular expression
rewrites are used. Axes form a Cartesian product with generated candidates. An axis option may
carry exact ``extra_edits`` so a helper definition and its call-site rewrite remain one choice.

Example manifest::

    {
      "schema": 1,
      "source": "src/BASE/Icon2b.cpp",
      "rva": "0xd0570",
      "axes": [
        {
          "name": "command_read",
          "find": "        gIcSrc++;\n        int cmd = gIcSrc[-1];\n",
          "options": [
            {"name": "baseline"},
            {"name": "post_increment", "replace": "        int cmd = *gIcSrc++;\n"}
          ]
        }
      ]
    }

Run inside ``nix develop .#build``::

    python3 scripts/batch_source_variants.py /tmp/icon2b-variants.json

The real source is restored after every compile and on interruption.  Results contain
scores, sizes, and ordered relocation metrics.  Candidate source is retained only for
an audited exact closure; sub-100 variants remain reproducible from the input manifest.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import itertools
import json
import os
import shutil
import signal
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass
from pathlib import Path

from homm2.analysis.disasm import _branch_kind, _cfg, _objdump
from tu_state_noise import (
    SourceMutationError,
    acquire_source_mutation_lock,
    byte_differences,
    compile_object,
    exact_closure_rejections,
    normalized_relocation_stream,
    object_metrics,
    objdiff_scores,
    resolve_target,
    target_state_identity,
    temporary_source,
)


@dataclass(frozen=True)
class Edit:
    start: int
    end: int
    original: bytes
    replacement: bytes


@dataclass(frozen=True)
class AxisOption:
    name: str
    replacement: bytes
    extra_edits: tuple[Edit, ...]


@dataclass(frozen=True)
class Axis:
    name: str
    start: int
    end: int
    original: bytes
    options: tuple[AxisOption, ...]


@dataclass(frozen=True)
class Candidate:
    name: str
    edits: tuple[Edit, ...]


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def result_rank(row: dict, retail_size: int, retail_relocs: int):
    return (
        -row["score"],
        abs(row["candidate_size"] - retail_size),
        abs(row["candidate_relocs"] - retail_relocs),
        row["trial"],
    )


def cfg_metrics(candidate_cfg, retail_cfg) -> dict:
    """Return the index-aligned structural signals shown by ``disasm --blocks``."""
    counts = {
        "exact": 0,
        "size_only": 0,
        "target_shift": 0,
        "flow_kind": 0,
        "missing": 0,
    }
    for index in range(max(len(candidate_cfg), len(retail_cfg))):
        if index >= len(candidate_cfg) or index >= len(retail_cfg):
            counts["missing"] += 1
            continue
        candidate_body, candidate_term = candidate_cfg[index][1:]
        retail_body, retail_term = retail_cfg[index][1:]
        same_size = len(candidate_body) == len(retail_body)
        if candidate_term == retail_term:
            counts["exact" if same_size else "size_only"] += 1
        elif _branch_kind(candidate_term, index) == _branch_kind(retail_term, index):
            counts["target_shift"] += 1
        else:
            counts["flow_kind"] += 1
    return {
        "candidate_blocks": len(candidate_cfg),
        "retail_blocks": len(retail_cfg),
        **counts,
    }


def parse_axis_extra_edit(raw_edit: dict, original: bytes, axis_name: str, option_name: str) -> Edit:
    if not isinstance(raw_edit, dict):
        raise ValueError(f"axis {axis_name}/{option_name}: every extra edit must be an object")
    find = raw_edit.get("find")
    replace = raw_edit.get("replace")
    insert_before = raw_edit.get("insert_before")
    insert_after = raw_edit.get("insert_after")
    text = raw_edit.get("text")
    modes = sum(value is not None for value in (find, insert_before, insert_after))
    if modes != 1:
        raise ValueError(
            f"axis {axis_name}/{option_name}: extra edit requires exactly one of "
            "find, insert_before, or insert_after"
        )
    if find is not None:
        if not isinstance(find, str) or not find or not isinstance(replace, str):
            raise ValueError(
                f"axis {axis_name}/{option_name}: find/replace must be non-empty string/string"
            )
        needle = find.encode("utf-8")
        replacement = replace.encode("utf-8")
        offset = original.find(needle)
        end = offset + len(needle)
    else:
        anchor = insert_before if insert_before is not None else insert_after
        if not isinstance(anchor, str) or not anchor or not isinstance(text, str):
            raise ValueError(
                f"axis {axis_name}/{option_name}: insertion anchor/text must be strings"
            )
        needle = anchor.encode("utf-8")
        anchor_offset = original.find(needle)
        offset = anchor_offset + (len(needle) if insert_after is not None else 0)
        end = offset
        replacement = text.encode("utf-8")
    if original.count(needle) != 1:
        raise ValueError(
            f"axis {axis_name}/{option_name}: extra-edit anchor occurs "
            f"{original.count(needle)} times, expected 1"
        )
    return Edit(offset, end, original[offset:end], replacement)


def load_manifest(path: Path, root: Path):
    payload = json.loads(path.read_text())
    if payload.get("schema") != 1:
        raise ValueError("manifest schema must be 1")
    source_value = payload.get("source")
    if not isinstance(source_value, str):
        raise ValueError("manifest source must be a string")
    rva_value = payload.get("rva")
    if not isinstance(rva_value, (str, int)):
        raise ValueError("manifest rva must be an integer or integer string")
    rva = int(rva_value, 0) if isinstance(rva_value, str) else rva_value
    source = (root / source_value).resolve()
    try:
        source.relative_to(root)
    except ValueError as exc:
        raise ValueError(f"source must be inside the worktree: {source}") from exc
    original = source.read_bytes()

    raw_axes = payload.get("axes")
    raw_candidates = payload.get("candidates")
    if raw_axes is None and raw_candidates is None:
        raise ValueError("manifest must contain axes, candidates, or both")
    axes = []
    candidates = []
    if raw_axes is not None:
        if not isinstance(raw_axes, list) or not raw_axes:
            raise ValueError("manifest axes must be a non-empty list")
        names = set()
        for raw_axis in raw_axes:
            if not isinstance(raw_axis, dict):
                raise ValueError("every axis must be an object")
            name = raw_axis.get("name")
            find = raw_axis.get("find")
            options = raw_axis.get("options")
            if not isinstance(name, str) or not name or name in names:
                raise ValueError(f"axis names must be unique non-empty strings: {name!r}")
            names.add(name)
            if not isinstance(find, str) or not find:
                raise ValueError(f"axis {name}: find must be a non-empty string")
            needle = find.encode("utf-8")
            if original.count(needle) != 1:
                raise ValueError(
                    f"axis {name}: exact find span occurs {original.count(needle)} times, expected 1"
                )
            start = original.index(needle)
            if not isinstance(options, list) or not options:
                raise ValueError(f"axis {name}: options must be a non-empty list")
            parsed_options = []
            option_names = set()
            for raw_option in options:
                if not isinstance(raw_option, dict):
                    raise ValueError(f"axis {name}: every option must be an object")
                option_name = raw_option.get("name")
                if not isinstance(option_name, str) or not option_name or option_name in option_names:
                    raise ValueError(f"axis {name}: option names must be unique non-empty strings")
                option_names.add(option_name)
                replacement = raw_option.get("replace", find)
                if not isinstance(replacement, str):
                    raise ValueError(f"axis {name}/{option_name}: replace must be a string")
                raw_extra_edits = raw_option.get("extra_edits", [])
                if not isinstance(raw_extra_edits, list):
                    raise ValueError(f"axis {name}/{option_name}: extra_edits must be a list")
                extra_edits = tuple(
                    parse_axis_extra_edit(raw_edit, original, name, option_name)
                    for raw_edit in raw_extra_edits
                )
                option_edits = (
                    Edit(start, start + len(needle), needle, replacement.encode("utf-8")),
                    *extra_edits,
                )
                ordered_option_edits = sorted(
                    option_edits, key=lambda edit: (edit.start, edit.end)
                )
                for left, right in zip(ordered_option_edits, ordered_option_edits[1:]):
                    if ranges_overlap(left.start, left.end, right.start, right.end):
                        raise ValueError(f"axis {name}/{option_name}: edits overlap")
                parsed_options.append(AxisOption(
                    option_name, replacement.encode("utf-8"), extra_edits,
                ))
            axes.append(Axis(name, start, start + len(needle), needle, tuple(parsed_options)))

        for axis_index, left_axis in enumerate(axes):
            for right_axis in axes[axis_index + 1:]:
                for left_option in left_axis.options:
                    left_edits = (
                        Edit(
                            left_axis.start, left_axis.end, left_axis.original,
                            left_option.replacement,
                        ),
                        *left_option.extra_edits,
                    )
                    for right_option in right_axis.options:
                        right_edits = (
                            Edit(
                                right_axis.start, right_axis.end, right_axis.original,
                                right_option.replacement,
                            ),
                            *right_option.extra_edits,
                        )
                        if any(
                            ranges_overlap(left.start, left.end, right.start, right.end)
                            for left in left_edits for right in right_edits
                        ):
                            raise ValueError(
                                f"axes overlap: {left_axis.name}/{left_option.name} and "
                                f"{right_axis.name}/{right_option.name}"
                            )
    if raw_candidates is not None:
        if not isinstance(raw_candidates, list) or not raw_candidates:
            raise ValueError("manifest candidates must be a non-empty list")
        candidate_names = set()
        for raw_candidate in raw_candidates:
            if not isinstance(raw_candidate, dict):
                raise ValueError("every candidate must be an object")
            name = raw_candidate.get("name")
            raw_edits = raw_candidate.get("edits")
            if not isinstance(name, str) or not name or name in candidate_names:
                raise ValueError(f"candidate names must be unique non-empty strings: {name!r}")
            candidate_names.add(name)
            if not isinstance(raw_edits, list):
                raise ValueError(f"candidate {name}: edits must be a list")
            if not raw_edits and name != "baseline":
                raise ValueError(
                    f"candidate {name}: only the named baseline candidate may have no edits"
                )
            edits = []
            for raw_edit in raw_edits:
                if not isinstance(raw_edit, dict):
                    raise ValueError(f"candidate {name}: every edit must be an object")
                start = raw_edit.get("start")
                end = raw_edit.get("end")
                find = raw_edit.get("find")
                replace = raw_edit.get("replace")
                if not isinstance(start, int) or not isinstance(end, int) or not 0 <= start <= end:
                    raise ValueError(f"candidate {name}: edit offsets must satisfy 0 <= start <= end")
                if not isinstance(find, str) or not isinstance(replace, str):
                    raise ValueError(f"candidate {name}: edit find/replace must be strings")
                needle = find.encode("utf-8")
                if end > len(original) or original[start:end] != needle:
                    raise ValueError(
                        f"candidate {name}: source does not equal find at byte range [{start}, {end})"
                    )
                edits.append(Edit(start, end, needle, replace.encode("utf-8")))
            edits.sort(key=lambda edit: edit.start)
            for left, right in zip(edits, edits[1:]):
                if left.end > right.start or (
                    left.start == left.end == right.start == right.end
                ):
                    raise ValueError(f"candidate {name}: edits overlap")
            candidates.append(Candidate(name, tuple(edits)))
    for axis in axes:
        for candidate in candidates:
            for option in axis.options:
                axis_edits = (
                    Edit(axis.start, axis.end, axis.original, option.replacement),
                    *option.extra_edits,
                )
                for axis_edit in axis_edits:
                    for edit in candidate.edits:
                        if ranges_overlap(
                            axis_edit.start, axis_edit.end, edit.start, edit.end
                        ):
                            raise ValueError(
                                f"axis {axis.name}/{option.name} overlaps an edit in "
                                f"candidate {candidate.name}"
                            )
    return payload, source, original, tuple(axes), tuple(candidates), rva


def ranges_overlap(first_start: int, first_end: int, second_start: int, second_end: int) -> bool:
    if first_start == first_end and second_start == second_end:
        return False
    if first_start == first_end:
        return second_start < first_start < second_end
    if second_start == second_end:
        return first_start < second_start < first_end
    return first_start < second_end and second_start < first_end


def render_edits(original: bytes, edits) -> bytes:
    insertions = {}
    replacements = []
    for start, end, replacement in edits:
        if start == end:
            insertions.setdefault(start, []).append(replacement)
        else:
            replacements.append((start, end, replacement))
    replacements.extend(
        (offset, offset, b"".join(parts)) for offset, parts in insertions.items()
    )
    rendered = original
    for start, end, replacement in sorted(
        replacements, key=lambda item: (item[0], item[1]), reverse=True
    ):
        rendered = rendered[:start] + replacement + rendered[end:]
    return rendered


def render_variant(original: bytes, axes: tuple[Axis, ...], choices) -> bytes:
    replacements = []
    for axis, option in zip(axes, choices):
        replacements.append((axis.start, axis.end, option.replacement))
        replacements.extend(
            (edit.start, edit.end, edit.replacement) for edit in option.extra_edits
        )
    return render_edits(original, replacements)


def render_candidate(original: bytes, candidate: Candidate) -> bytes:
    rendered = original
    for edit in reversed(candidate.edits):
        rendered = rendered[:edit.start] + edit.replacement + rendered[edit.end:]
    return rendered


def render_combined(original: bytes, axes: tuple[Axis, ...], choices, candidate: Candidate | None):
    replacements = []
    if candidate is not None:
        replacements.extend(
            (edit.start, edit.end, edit.replacement) for edit in candidate.edits
        )
    for axis, option in zip(axes, choices):
        replacements.append((axis.start, axis.end, option.replacement))
        replacements.extend(
            (edit.start, edit.end, edit.replacement) for edit in option.extra_edits
        )
    return render_edits(original, replacements)


def iter_variants(original: bytes, axes: tuple[Axis, ...], candidates: tuple[Candidate, ...]):
    candidate_values = candidates or (None,)
    choice_values = itertools.product(*(axis.options for axis in axes)) if axes else ((),)
    choices = tuple(choice_values)
    for candidate in candidate_values:
        for axis_choices in choices:
            labels = {
                axis.name: choice.name for axis, choice in zip(axes, axis_choices)
            }
            if candidate is not None:
                labels["candidate"] = candidate.name
            yield render_combined(original, axes, axis_choices, candidate), labels


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--limit", type=int, default=4096)
    parser.add_argument("--top", type=int, default=12)
    parser.add_argument("--compile-timeout", type=float, default=120.0)
    parser.add_argument(
        "--wall-time-seconds", type=float, default=1200.0,
        help="stop cleanly after this per-function search budget (default: 20 minutes)",
    )
    parser.add_argument("--output", type=Path)
    parser.add_argument(
        "--show-best-disasm", action="store_true",
        help="print the best candidate object disassembly before deleting disposable artifacts",
    )
    args = parser.parse_args(argv)
    if (
        args.limit < 1 or args.top < 1
        or args.compile_timeout <= 0 or args.wall_time_seconds <= 0
    ):
        parser.error("--limit, --top, --compile-timeout, and --wall-time-seconds must be positive")

    root = Path(os.environ.get("HOMM2_DIR", Path.cwd())).resolve()
    try:
        payload, source, original, axes, candidates, rva = load_manifest(args.manifest, root)
    except (OSError, ValueError, KeyError, json.JSONDecodeError) as exc:
        parser.error(str(exc))
    try:
        source_lock = acquire_source_mutation_lock(root, source)
        if source.read_bytes() != original:
            source_lock.close()
            raise SourceMutationError(f"source changed while acquiring mutation lock: {source}")
        target, flags = resolve_target(root, source, rva)
    except (OSError, SourceMutationError, ValueError, KeyError) as exc:
        parser.error(str(exc))

    combinations = len(candidates) if candidates else 1
    for axis in axes:
        combinations *= len(axis.options)
    if combinations > args.limit:
        parser.error(f"manifest expands to {combinations} variants, above --limit {args.limit}")

    target_obj = root / "build/delink" / f"{target.unit}.c.obj"
    if not target_obj.is_file():
        parser.error(f"retail object is missing: {target_obj}")
    stamp = time.strftime("%Y%m%d-%H%M%S")
    output = (
        (root / args.output).resolve()
        if args.output
        else root / "build/source-variant-batch" / f"{stamp}-{target.unit.replace('/', '-')}-0x{target.rva:x}"
    )
    output.mkdir(parents=True, exist_ok=False)
    (output / "input.json").write_text(json.dumps(payload, indent=2) + "\n")

    retail_metrics = object_metrics(target_obj)
    retail_target = retail_metrics.get(target.symbol)
    if retail_target is None:
        parser.error(f"target symbol absent from retail object: {target.symbol}")
    retail_cfg = _cfg(_objdump(target_obj, target.symbol, target.retail_size, 0))

    results = []
    states = {}
    seen = {}
    started = time.perf_counter()
    best_score = -1.0
    exact_source = None
    baseline_summary = None
    best_object_rank = None
    best_disasm = None
    original_handler = signal.getsignal(signal.SIGTERM)
    restoration_conflict = False
    wall_time_reached = False

    def interrupt(_signum, _frame):
        raise KeyboardInterrupt

    signal.signal(signal.SIGTERM, interrupt)
    try:
        with tempfile.TemporaryDirectory(prefix="source-variants-", dir=output) as scratch_name:
            scratch = Path(scratch_name)
            baseline_obj = scratch / "baseline.obj"
            with temporary_source(source, original, original):
                baseline_ok, baseline_log, baseline_timed_out = compile_object(
                    root, source, baseline_obj, flags, args.compile_timeout
                )
            if not baseline_ok:
                (output / "baseline.compile.log").write_text(baseline_log)
                reason = "timed out" if baseline_timed_out else "failed"
                print(f"baseline compile {reason}; source restored", file=sys.stderr)
                source_lock.close()
                return 2
            baseline_scores, baseline_sizes, baseline_counts, baseline_diff_log = objdiff_scores(
                target_obj, baseline_obj, target.symbol
            )
            baseline_metrics = object_metrics(baseline_obj)
            baseline_target = baseline_metrics.get(target.symbol)
            baseline_score = baseline_scores.get(target.symbol)
            if (
                baseline_target is None
                or baseline_score is None
                or baseline_counts.get(target.symbol) != 1
            ):
                (output / "baseline.objdiff.log").write_text(baseline_diff_log)
                print("target symbol missing or non-unique in baseline object", file=sys.stderr)
                source_lock.close()
                return 2
            baseline_summary = {
                "score": baseline_score,
                "candidate_size": baseline_sizes.get(target.symbol),
                "candidate_relocs": baseline_target["relocs"],
                "text_sha": baseline_target["text_sha"],
                "reloc_sha": baseline_target["reloc_sha"],
                "blocks": cfg_metrics(
                    _cfg(_objdump(baseline_obj, target.symbol, target.retail_size, 0)),
                    retail_cfg,
                ),
            }
            print(
                f"baseline {baseline_score:.6f}% size {baseline_sizes.get(target.symbol)} "
                f"relocs {baseline_target['relocs']}/{retail_target['relocs']}; "
                f"running {combinations} variants",
                flush=True,
            )
            for index, (candidate, labels) in enumerate(iter_variants(original, axes, candidates)):
                remaining_wall_time = args.wall_time_seconds - (time.perf_counter() - started)
                if remaining_wall_time <= 0:
                    wall_time_reached = True
                    break
                digest = sha256(candidate)
                if digest in seen:
                    results.append({
                        "trial": index,
                        "choices": labels,
                        "source_sha256": digest,
                        "duplicate_of": seen[digest],
                    })
                    continue
                seen[digest] = index
                candidate_obj = scratch / f"trial-{index:04d}.obj"
                with temporary_source(source, original, candidate):
                    ok, compile_log, timed_out = compile_object(
                        root, source, candidate_obj, flags,
                        min(args.compile_timeout, max(0.1, remaining_wall_time)),
                    )
                if time.perf_counter() - started >= args.wall_time_seconds:
                    wall_time_reached = True
                row = {
                    "trial": index,
                    "choices": labels,
                    "source_sha256": digest,
                    "compiled": ok,
                    "compile_timed_out": timed_out,
                }
                if not ok:
                    row["compile_log"] = compile_log
                    results.append(row)
                    continue
                scores, sizes, counts, diff_log = objdiff_scores(
                    target_obj, candidate_obj, target.symbol
                )
                metrics = object_metrics(candidate_obj)
                score = scores.get(target.symbol)
                candidate_target = metrics.get(target.symbol)
                if score is None or candidate_target is None or counts.get(target.symbol) != 1:
                    row["objdiff_error"] = diff_log or "target symbol missing or non-unique"
                    results.append(row)
                    continue
                candidate_size = sizes.get(target.symbol)
                identity_metrics = dict(candidate_target)
                identity_metrics["objdiff_size"] = candidate_size
                state_id = target_state_identity(identity_metrics)
                normalized_relocations = normalized_relocation_stream(candidate_target)
                normalized_reloc_sha = sha256(
                    "\n".join(normalized_relocations).encode("utf-8")
                )[:16]
                rejections = exact_closure_rejections(
                    score, candidate_size, target.retail_size, candidate_target, retail_target
                )
                sibling_regressions = []
                for symbol, baseline_symbol_score in baseline_scores.items():
                    if symbol == target.symbol:
                        continue
                    candidate_symbol_score = scores.get(symbol)
                    if candidate_symbol_score is None:
                        sibling_regressions.append(f"missing sibling {symbol}")
                    elif candidate_symbol_score < baseline_symbol_score - 1e-6:
                        sibling_regressions.append(
                            f"{symbol}: {baseline_symbol_score:.6f} -> {candidate_symbol_score:.6f}"
                        )
                for symbol, baseline_symbol in baseline_metrics.items():
                    if (
                        symbol != target.symbol
                        and baseline_scores.get(symbol) == 100.0
                        and metrics.get(symbol) != baseline_symbol
                    ):
                        sibling_regressions.append(
                            f"exact sibling raw/relocation metrics changed: {symbol}"
                        )
                row.update({
                    "score": score,
                    "score_delta": score - baseline_score,
                    "candidate_size": candidate_size,
                    "retail_size": target.retail_size,
                    "candidate_relocs": candidate_target["relocs"],
                    "retail_relocs": retail_target["relocs"],
                    "text_sha": candidate_target["text_sha"],
                    "reloc_sha": candidate_target["reloc_sha"],
                    "normalized_reloc_sha": normalized_reloc_sha,
                    "state_id": state_id,
                    "sibling_regressions": sibling_regressions,
                    "exact": not rejections,
                    "exact_rejections": rejections,
                    "blocks": cfg_metrics(
                        _cfg(_objdump(candidate_obj, target.symbol, target.retail_size, 0)),
                        retail_cfg,
                    ),
                })
                results.append(row)
                state = states.setdefault(state_id, {
                    "state_id": state_id,
                    "representative_trial": index,
                    "representative_choices": labels,
                    "score": score,
                    "scores": [],
                    "size": candidate_size,
                    "relocs": candidate_target["relocs"],
                    "text_sha": candidate_target["text_sha"],
                    "text_hex": candidate_target["text_hex"],
                    "normalized_reloc_sha": normalized_reloc_sha,
                    "normalized_reloc_stream": normalized_relocations,
                    "retail_byte_differences": byte_differences(
                        candidate_target["text_hex"], retail_target["text_hex"]
                    ),
                    "raw_reloc_detail_shas": [],
                    "observation_count": 0,
                })
                state["observation_count"] += 1
                if score not in state["scores"]:
                    state["scores"].append(score)
                raw_detail_sha = candidate_target["reloc_detail_sha"]
                if raw_detail_sha not in state["raw_reloc_detail_shas"]:
                    state["raw_reloc_detail_shas"].append(raw_detail_sha)
                rank = result_rank(row, target.retail_size, retail_target["relocs"])
                if args.show_best_disasm and (
                    best_object_rank is None or rank < best_object_rank
                ):
                    shutil.copyfile(candidate_obj, scratch / "best.obj")
                    best_object_rank = rank
                if score > best_score:
                    best_score = score
                    print(
                        f"new best {score:.6f}% trial {index}/{combinations - 1} "
                        f"size {candidate_size} relocs {candidate_target['relocs']}/{retail_target['relocs']} "
                        + " ".join(f"{name}={value}" for name, value in labels.items()),
                        flush=True,
                    )
                if not rejections and exact_source is None:
                    exact_source = candidate
                candidate_obj.unlink(missing_ok=True)
                Path(str(candidate_obj) + ".d").unlink(missing_ok=True)
            if args.show_best_disasm and best_object_rank is not None:
                command = [
                    "llvm-objdump", "-dr", f"--disassemble-symbols={target.symbol}",
                    str(scratch / "best.obj"),
                ]
                disassembly = subprocess.run(command, capture_output=True, text=True)
                best_disasm = disassembly.stdout + disassembly.stderr
    except KeyboardInterrupt:
        print("interrupted; source restored", file=sys.stderr)
        source_lock.close()
        return 130
    finally:
        signal.signal(signal.SIGTERM, original_handler)
        if source.read_bytes() != original:
            restoration_conflict = True

    if restoration_conflict:
        print(
            "FATAL: source changed outside the guarded candidate interval; "
            "refusing stale restoration",
            file=sys.stderr,
        )
        source_lock.close()
        return 3

    ranked = sorted(
        (row for row in results if row.get("score") is not None),
        key=lambda row: result_rank(row, target.retail_size, retail_target["relocs"]),
    )
    summary = {
        "schema": 1,
        "source": str(source.relative_to(root)),
        "source_sha256": sha256(original),
        "unit": target.unit,
        "rva": f"0x{target.rva:x}",
        "symbol": target.symbol,
        "variant_count": combinations,
        "completed_variant_count": len(results),
        "wall_time_seconds": args.wall_time_seconds,
        "truncated_by_wall_time": wall_time_reached,
        "unique_source_count": len(seen),
        "elapsed_seconds": time.perf_counter() - started,
        "source_restored": source.read_bytes() == original,
        "baseline": baseline_summary,
        "best": ranked[0] if ranked else None,
        "state_count": len(states),
        "states": sorted(
            states.values(),
            key=lambda state: (-state["score"], state["size"], state["state_id"]),
        ),
        "results": results,
    }
    (output / "results.json").write_text(json.dumps(summary, indent=2) + "\n")
    with (output / "results.csv").open("w", newline="") as handle:
        writer = csv.writer(handle)
        label_names = [axis.name for axis in axes]
        if candidates:
            label_names.append("candidate")
        writer.writerow(["trial", "score", "size", "relocs", "exact", *label_names])
        for row in ranked:
            writer.writerow([
                row["trial"], row["score"], row["candidate_size"], row["candidate_relocs"],
                row["exact"], *[row["choices"][name] for name in label_names],
            ])
    if exact_source is not None:
        (output / "exact.cpp").write_bytes(exact_source)

    disposition = (
        f"stopped after {len(results)}/{combinations} variants at the wall-time limit"
        if wall_time_reached else f"completed {combinations} variants"
    )
    print(f"{disposition} in {summary['elapsed_seconds']:.2f}s; source restored")
    for row in ranked[: args.top]:
        labels = " ".join(f"{name}={value}" for name, value in row["choices"].items())
        print(
            f"{row['score']:.6f}% size {row['candidate_size']} "
            f"relocs {row['candidate_relocs']}/{row['retail_relocs']} trial {row['trial']} {labels}"
        )
    if best_disasm is not None:
        print("--- best disposable candidate disassembly (object deleted after inspection) ---")
        print(best_disasm.rstrip())
    print(output)
    source_lock.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
