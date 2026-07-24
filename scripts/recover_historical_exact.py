#!/usr/bin/env python3
"""Recover current-hash MAX evidence for every historically exact live residual.

The driver builds a persistent queue from ``historical_exact_losses.py`` and runs
``tu_state_noise.py`` independently for each function whose current hash does not
already retain MAX 100%.  Each target gets N deterministic parser-state
transformations.  The child search stops at its first audited exact closure,
restores the authored source, records only that target's MAX, and the driver moves
on.  A TSV under /tmp makes the campaign resumable.

Run inside ``nix develop .#build``::

    python3 scripts/recover_historical_exact.py --trials 128
"""

from __future__ import annotations

import argparse
import csv
import fcntl
import hashlib
import json
import os
import re
import signal
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path

from historical_exact_losses import parse_baseline
from tu_state_noise import (
    SourceMutationError,
    acquire_source_mutation_lock,
    resolve_target,
)


DEFAULT_QUEUE = Path("/tmp/homm2-historical-exact-recovery-queue.tsv")
DEFAULT_RESULTS = Path("/tmp/homm2-historical-exact-recovery-results.tsv")
DEFAULT_LOG_DIR = Path("/tmp/homm2-historical-exact-recovery-logs")
TERMINATION_GRACE_SECONDS = 3.0
ROLLBACK_TIMEOUT_SECONDS = 180.0
VA_MARKER_LINE = re.compile(r"^[ \t]*VA\(0x[0-9a-f]+,", re.M | re.I)
OD_STEER_NAME = "OD_STEER"
RESULT_FIELDS = (
    "timestamp",
    "profile",
    "rank",
    "unit",
    "rva",
    "symbol",
    "current_hash",
    "live_score",
    "status",
    "best_score",
    "trial_budget",
    "elapsed_seconds",
    "exit_code",
    "log",
    "artifact",
)


class RecoveryError(RuntimeError):
    pass


def target_source_span(text: str, marker_offset: int) -> tuple[int, int]:
    """Return the VA-delimited source block for one resolved function."""
    if marker_offset < 0 or marker_offset >= len(text):
        raise ValueError("target marker offset is outside the source")
    if re.match(r"VA\(0x[0-9a-f]+,", text[marker_offset:], re.I) is None:
        raise ValueError("target marker offset does not begin a VA marker")
    following = VA_MARKER_LINE.search(text, marker_offset + 1)
    return marker_offset, following.start() if following else len(text)


def _quoted_end(text: str, start: int, quote: str) -> int:
    cursor = start + 1
    while cursor < len(text):
        if text[cursor] == "\\":
            cursor += 2
        elif text[cursor] == quote:
            return cursor + 1
        else:
            cursor += 1
    raise ValueError("unterminated string or character literal")


def _comment_end(text: str, start: int) -> int:
    if text.startswith("//", start):
        newline = text.find("\n", start + 2)
        return len(text) if newline < 0 else newline
    end = text.find("*/", start + 2)
    if end < 0:
        raise ValueError("unterminated block comment")
    return end + 2


def _matching_paren(text: str, opening: int) -> int:
    depth = 1
    cursor = opening + 1
    while cursor < len(text):
        if text.startswith("//", cursor) or text.startswith("/*", cursor):
            cursor = _comment_end(text, cursor)
        elif text[cursor] in {'"', "'"}:
            cursor = _quoted_end(text, cursor, text[cursor])
        elif text[cursor] == "(":
            depth += 1
            cursor += 1
        elif text[cursor] == ")":
            depth -= 1
            if depth == 0:
                return cursor
            cursor += 1
        else:
            cursor += 1
    raise ValueError("unterminated OD_STEER invocation")


def unwrap_od_steer(text: str) -> tuple[str, int]:
    """Remove real OD_STEER calls while leaving comments and literals untouched."""
    pieces: list[str] = []
    cursor = 0
    count = 0
    while cursor < len(text):
        if text.startswith("//", cursor) or text.startswith("/*", cursor):
            end = _comment_end(text, cursor)
            pieces.append(text[cursor:end])
            cursor = end
            continue
        if text[cursor] in {'"', "'"}:
            end = _quoted_end(text, cursor, text[cursor])
            pieces.append(text[cursor:end])
            cursor = end
            continue
        if text.startswith(OD_STEER_NAME, cursor):
            before = text[cursor - 1] if cursor else ""
            after_index = cursor + len(OD_STEER_NAME)
            after = text[after_index] if after_index < len(text) else ""
            if (before.isalnum() or before == "_") or (after.isalnum() or after == "_"):
                pieces.append(text[cursor])
                cursor += 1
                continue
            opening = after_index
            while opening < len(text) and text[opening].isspace():
                opening += 1
            if opening >= len(text) or text[opening] != "(":
                raise ValueError("OD_STEER token is not a function-like invocation")
            closing = _matching_paren(text, opening)
            inner, nested = unwrap_od_steer(text[opening + 1 : closing])
            pieces.append(inner)
            count += nested + 1
            cursor = closing + 1
            continue
        pieces.append(text[cursor])
        cursor += 1
    return "".join(pieces), count


def clean_target_od_steer(text: str, marker_offset: int) -> tuple[str, int]:
    """Unwrap OD_STEER only within the resolved target's VA-delimited block."""
    start, end = target_source_span(text, marker_offset)
    cleaned_block, count = unwrap_od_steer(text[start:end])
    return text[:start] + cleaned_block + text[end:], count


def positive_int(value: str) -> int:
    parsed = int(value, 0)
    if parsed <= 0:
        raise argparse.ArgumentTypeError("value must be positive")
    return parsed


def repo_path(root: Path, value: Path) -> Path:
    return value if value.is_absolute() else root / value


def read_tsv(path: Path) -> list[dict[str, str]]:
    with path.open(encoding="utf-8", newline="") as stream:
        return list(csv.DictReader(stream, delimiter="\t"))


def current_baseline(root: Path) -> dict[tuple[str, str], tuple[float, str]]:
    return parse_baseline((root / "config/match_baseline.tsv").read_text())


def pending_losses(
    rows: list[dict[str, str]],
    baseline: dict[tuple[str, str], tuple[float, str]],
) -> list[dict[str, str]]:
    pending = []
    for row in rows:
        key = (row["unit"], row["symbol"])
        current = baseline.get(key)
        if current is None:
            continue
        maximum, source_hash = current
        if maximum >= 100.0:
            continue
        if source_hash != row["current_hash"]:
            continue
        if not row.get("rva"):
            continue
        pending.append(row)
    return pending


def profile_id(root: Path, args: argparse.Namespace) -> str:
    payload = {
        "tool_sha256": hashlib.sha256(
            (root / "scripts/tu_state_noise.py").read_bytes()
        ).hexdigest(),
        "trials": args.trials,
        "families": args.families,
        "insertion": args.insertion,
        "max_declarations": args.max_declarations,
        "seed": args.seed,
        "compile_timeout_seconds": args.compile_timeout_seconds,
    }
    encoded = json.dumps(payload, sort_keys=True, separators=(",", ":")).encode()
    return hashlib.sha256(encoded).hexdigest()[:16]


def completed_keys(path: Path, profile: str) -> set[tuple[str, str, str]]:
    if not path.exists():
        return set()
    completed = set()
    for row in read_tsv(path):
        if row.get("profile") != profile:
            continue
        if row.get("status") not in {"exact", "no-exact", "wall"}:
            continue
        completed.add((row["unit"], row["symbol"], row["current_hash"]))
    return completed


def append_result(path: Path, row: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    existed = path.exists() and path.stat().st_size > 0
    with path.open("a", encoding="utf-8", newline="") as stream:
        writer = csv.DictWriter(
            stream, fieldnames=RESULT_FIELDS, delimiter="\t", lineterminator="\n"
        )
        if not existed:
            writer.writeheader()
        writer.writerow({field: row.get(field, "") for field in RESULT_FIELDS})
        stream.flush()
        os.fsync(stream.fileno())


def target_seed(base_seed: int, row: dict[str, str]) -> int:
    payload = (
        f"{base_seed}:{row['unit']}:{row['rva']}:{row['symbol']}:"
        f"{row['current_hash']}"
    ).encode()
    return int.from_bytes(hashlib.sha256(payload).digest()[:4], "little")


def source_for_unit(root: Path, unit: str) -> Path:
    return root / "src" / f"{unit}.cpp"


def artifact_name(row: dict[str, str]) -> str:
    unit = row["unit"].replace("/", "-")
    return f"{unit}-{int(row['rva'], 0):08x}-{row['current_hash']}"


def terminate_process(process: subprocess.Popen) -> None:
    if process.poll() is not None:
        return
    os.killpg(process.pid, signal.SIGTERM)
    try:
        process.wait(timeout=TERMINATION_GRACE_SECONDS)
    except subprocess.TimeoutExpired:
        os.killpg(process.pid, signal.SIGKILL)
        process.wait()


def run_logged_process(
    root: Path,
    command: list[str],
    log_path: Path,
    deadline: float,
    mode: str = "a",
) -> tuple[int, float]:
    remaining = deadline - time.monotonic()
    if remaining <= 0:
        return 124, 0.0
    started = time.monotonic()
    log_path.parent.mkdir(parents=True, exist_ok=True)
    with log_path.open(mode, encoding="utf-8") as log:
        log.write("command: " + " ".join(command) + "\n")
        log.flush()
        process = subprocess.Popen(
            command,
            cwd=root,
            stdout=log,
            stderr=subprocess.STDOUT,
            start_new_session=True,
            text=True,
        )
        try:
            exit_code = process.wait(timeout=remaining)
        except subprocess.TimeoutExpired:
            terminate_process(process)
            return 124, time.monotonic() - started
        except KeyboardInterrupt:
            terminate_process(process)
            raise
    return exit_code, time.monotonic() - started


def noise_command(
    root: Path,
    row: dict[str, str],
    args: argparse.Namespace,
    source: Path,
    output: Path,
    *,
    record_max: bool,
    only_trial: int | None = None,
) -> list[str]:
    command = [
        sys.executable,
        str(root / "scripts/tu_state_noise.py"),
        "--source",
        str(source),
        "--rva",
        row["rva"],
        "--trials",
        str(args.trials),
        "--seed",
        hex(target_seed(args.seed, row)),
        "--insertion",
        args.insertion,
        "--families",
        args.families,
        "--max-declarations",
        str(args.max_declarations),
        "--compile-timeout-seconds",
        str(args.compile_timeout_seconds),
        "--output",
        str(output),
        "--retain-best",
    ]
    if record_max:
        command.append("--record-max")
    if only_trial is not None:
        command.extend(("--only-trial", str(only_trial)))
    return command


def load_manifest(output: Path) -> dict:
    path = output / "manifest.json"
    return json.loads(path.read_text()) if path.exists() else {}


def write_source_guarded(
    root: Path,
    source: Path,
    expected: bytes,
    replacement: bytes,
) -> None:
    lock = acquire_source_mutation_lock(root, source)
    try:
        current = source.read_bytes()
        if current != expected:
            raise SourceMutationError(f"source changed before guarded write: {source}")
        source.write_bytes(replacement)
    finally:
        lock.close()


def rebuild_normalized_unit(
    root: Path,
    unit: str,
    log_path: Path,
    deadline: float,
) -> None:
    target = f"build/objdiff/normalized/base/{unit}.obj"
    exit_code, _elapsed = run_logged_process(
        root, ["ninja", target], log_path, deadline
    )
    if exit_code != 0:
        raise RecoveryError(f"canonical rebuild failed with exit code {exit_code}")


def rollback_cleanup(
    root: Path,
    source: Path,
    clean_bytes: bytes,
    original_bytes: bytes,
    baseline_path: Path,
    baseline_bytes: bytes,
    expected_baseline_bytes: bytes,
    unit: str,
    log_path: Path,
    deadline: float,
) -> None:
    write_source_guarded(root, source, clean_bytes, original_bytes)
    current_baseline_bytes = baseline_path.read_bytes()
    if current_baseline_bytes != baseline_bytes:
        if current_baseline_bytes != expected_baseline_bytes:
            raise RecoveryError(
                "baseline changed outside the cleanup transaction; refusing rollback overwrite"
            )
        baseline_path.write_bytes(baseline_bytes)
    rebuild_normalized_unit(root, unit, log_path, deadline)
    exit_code, _elapsed = run_logged_process(
        root, ["homm2", "status", "--force-refresh"], log_path, deadline
    )
    if exit_code != 0:
        raise RecoveryError(
            f"rollback status refresh failed with exit code {exit_code}"
        )


def retire_target_od_steer(
    root: Path,
    row: dict[str, str],
    args: argparse.Namespace,
    log_path: Path,
    output: Path,
    original_bytes: bytes,
    clean_bytes: bytes,
    count: int,
    deadline: float,
) -> Path:
    """Keep a target-local OD_STEER removal only after clean exact revalidation."""
    source = source_for_unit(root, row["unit"])
    baseline_path = root / "config/match_baseline.tsv"
    baseline_bytes = baseline_path.read_bytes()
    suffix = f"-no-od-steer-{time.time_ns()}"
    clean_output = output.with_name(output.name + suffix)
    replay_output = output.with_name(output.name + suffix + "-record")
    cleanup_manifest = output / "od-steer-cleanup.json"
    state = {
        "target_local_invocations": count,
        "source_retained": False,
        "clean_search_artifact": str(clean_output),
        "record_artifact": str(replay_output),
    }

    expected_baseline_bytes = baseline_bytes
    source_cleaned = False
    try:
        write_source_guarded(root, source, original_bytes, clean_bytes)
        source_cleaned = True
        command = noise_command(
            root, row, args, source, clean_output, record_max=False
        )
        exit_code, _elapsed = run_logged_process(
            root, command, log_path, deadline
        )
        clean_manifest = load_manifest(clean_output)
        exact = clean_manifest.get("exact_closure")
        if exit_code != 0 or exact is None:
            raise RecoveryError(
                "clean OD_STEER-free target did not reproduce an audited exact state"
            )

        rebuild_normalized_unit(root, row["unit"], log_path, deadline)
        exit_code, _elapsed = run_logged_process(
            root,
            ["homm2", "status", "update", "--force-refresh"],
            log_path,
            deadline,
        )
        expected_baseline_bytes = baseline_path.read_bytes()
        if exit_code != 0:
            raise RecoveryError(
                f"clean-source status transition failed with exit code {exit_code}"
            )

        trial = int(exact["trial"])
        command = noise_command(
            root,
            row,
            args,
            source,
            replay_output,
            record_max=True,
            only_trial=trial,
        )
        exit_code, _elapsed = run_logged_process(
            root, command, log_path, deadline
        )
        expected_baseline_bytes = baseline_path.read_bytes()
        replay_manifest = load_manifest(replay_output)
        recorded = replay_manifest.get("record_max", {})
        if (
            exit_code != 0
            or replay_manifest.get("exact_closure") is None
            or float(recorded.get("new_max", 0.0)) < 100.0
        ):
            raise RecoveryError(
                "clean exact replay did not retain MAX 100 for the new source hash"
            )
        if source.read_bytes() != clean_bytes:
            raise RecoveryError("clean exact replay did not restore the clean source")
        state.update(
            {
                "source_retained": True,
                "exact_trial": trial,
                "new_source_hash": recorded.get("source_hash"),
            }
        )
        cleanup_manifest.write_text(json.dumps(state, indent=2) + "\n")
        return replay_output
    except (OSError, RecoveryError, SourceMutationError, ValueError) as exc:
        state["error"] = str(exc)
        try:
            if source_cleaned or source.read_bytes() == clean_bytes:
                rollback_cleanup(
                    root,
                    source,
                    clean_bytes,
                    original_bytes,
                    baseline_path,
                    baseline_bytes,
                    expected_baseline_bytes,
                    row["unit"],
                    log_path,
                    max(deadline, time.monotonic() + ROLLBACK_TIMEOUT_SECONDS),
                )
            elif (
                source.read_bytes() != original_bytes
                or baseline_path.read_bytes() != baseline_bytes
            ):
                raise RecoveryError(
                    "cleanup failed before ownership was established and state changed"
                )
            state["rollback_complete"] = True
        except (OSError, RecoveryError, SourceMutationError) as rollback_exc:
            state["rollback_complete"] = False
            state["rollback_error"] = str(rollback_exc)
        cleanup_manifest.write_text(json.dumps(state, indent=2) + "\n")
        raise RecoveryError(state["error"]) from exc


def run_target(
    root: Path,
    row: dict[str, str],
    args: argparse.Namespace,
    log_path: Path,
) -> tuple[str, int, float, float | None, Path]:
    source = source_for_unit(root, row["unit"])
    artifact_root = Path(
        getattr(
            args,
            "artifact_root",
            root / "build/tu-state-noise/historical-exact-recovery",
        )
    )
    output = (
        artifact_root
        / getattr(args, "profile", "manual")
        / artifact_name(row)
    )
    if output.exists():
        output = output.with_name(output.name + f"-retry-{time.time_ns()}")
    if not source.is_file():
        return "error", 2, 0.0, None, output
    started = time.monotonic()
    deadline = started + args.target_timeout_seconds
    original_bytes = source.read_bytes()
    try:
        target, _flags = resolve_target(root, source, int(row["rva"], 0))
        clean_text, steer_count = clean_target_od_steer(
            original_bytes.decode("utf-8"), target.marker_offset
        )
    except (OSError, UnicodeError, ValueError) as exc:
        log_path.parent.mkdir(parents=True, exist_ok=True)
        log_path.write_text(f"target OD_STEER inspection failed: {exc}\n")
        return "error", 2, time.monotonic() - started, None, output
    clean_bytes = clean_text.encode("utf-8")
    command = noise_command(
        root,
        row,
        args,
        source,
        output,
        record_max=steer_count == 0,
    )
    exit_code, _elapsed = run_logged_process(
        root, command, log_path, deadline, mode="w"
    )
    elapsed = time.monotonic() - started
    if exit_code == 124:
        return "timeout", exit_code, elapsed, None, output
    if exit_code != 0:
        return "error", exit_code, elapsed, None, output
    manifest = load_manifest(output)
    best_score = manifest.get("best_retained", {}).get("score")
    exact = manifest.get("exact_closure") is not None
    if steer_count and exact:
        try:
            output = retire_target_od_steer(
                root,
                row,
                args,
                log_path,
                output,
                original_bytes,
                clean_bytes,
                steer_count,
                deadline,
            )
        except RecoveryError as exc:
            with log_path.open("a", encoding="utf-8") as log:
                log.write(f"target-local OD_STEER retirement failed: {exc}\n")
            return "error", 5, time.monotonic() - started, best_score, output
        return "exact", 0, time.monotonic() - started, 100.0, output
    _maximum, source_hash = current_baseline(root).get(
        (row["unit"], row["symbol"]), (0.0, "")
    )
    if source_hash != row["current_hash"]:
        return "error", 3, elapsed, best_score, output
    return (
        "exact" if exact else "no-exact",
        exit_code,
        elapsed,
        best_score,
        output,
    )


def refresh_queue(root: Path, queue: Path) -> None:
    queue.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(
        [
            sys.executable,
            str(root / "scripts/historical_exact_losses.py"),
            "--output",
            str(queue),
        ],
        cwd=root,
        check=True,
    )


def refresh_status(root: Path) -> None:
    subprocess.run(
        ["homm2", "status", "--force-refresh"],
        cwd=root,
        check=True,
        stdout=subprocess.DEVNULL,
    )


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trials", type=positive_int, default=128)
    parser.add_argument("--families", default="forest")
    parser.add_argument("--insertion", choices=("target", "top"), default="target")
    parser.add_argument("--max-declarations", type=positive_int, default=64)
    parser.add_argument("--seed", type=lambda value: int(value, 0), default=0x48495354)
    parser.add_argument("--compile-timeout-seconds", type=positive_int, default=120)
    parser.add_argument("--target-timeout-seconds", type=positive_int, default=1200)
    parser.add_argument("--queue", type=Path, default=DEFAULT_QUEUE)
    parser.add_argument("--results", type=Path, default=DEFAULT_RESULTS)
    parser.add_argument("--log-dir", type=Path, default=DEFAULT_LOG_DIR)
    parser.add_argument("--limit", type=positive_int)
    parser.add_argument(
        "--no-refresh-status",
        action="store_true",
        help="do not refresh README/status once after the batch",
    )
    args = parser.parse_args(argv)

    root = Path(os.environ.get("HOMM2_DIR", Path.cwd())).resolve()
    queue = repo_path(root, args.queue)
    results = repo_path(root, args.results)
    log_dir = repo_path(root, args.log_dir)
    lock_path = results.with_suffix(results.suffix + ".lock")
    lock_path.parent.mkdir(parents=True, exist_ok=True)
    lock = lock_path.open("w")
    try:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        print(f"another recovery driver owns {lock_path}", file=sys.stderr)
        return 2

    refresh_queue(root, queue)
    baseline = current_baseline(root)
    queue_rows = pending_losses(read_tsv(queue), baseline)
    profile = profile_id(root, args)
    args.profile = profile
    done = completed_keys(results, profile)
    pending = [
        row for row in queue_rows
        if (row["unit"], row["symbol"], row["current_hash"]) not in done
    ]
    if args.limit is not None:
        pending = pending[:args.limit]

    print(
        f"historical exact recovery: profile={profile} "
        f"eligible={len(queue_rows)} already-tested={len(done)} pending={len(pending)}",
        flush=True,
    )
    exact_count = 0
    no_exact_count = 0
    error_count = 0
    for index, row in enumerate(pending, 1):
        latest = current_baseline(root).get((row["unit"], row["symbol"]))
        if latest != (float(row["current_max"]), row["current_hash"]):
            if latest is not None and latest[0] >= 100.0:
                continue
            status, exit_code, elapsed, best_score, artifact = (
                "error", 3, 0.0, None, Path()
            )
        else:
            log_path = log_dir / f"{artifact_name(row)}.log"
            print(
                f"[{index}/{len(pending)}] {row['unit']} {row['rva']} "
                f"{row['symbol']} ({row['live_score']}%)",
                flush=True,
            )
            try:
                status, exit_code, elapsed, best_score, artifact = run_target(
                    root, row, args, log_path
                )
            except KeyboardInterrupt:
                print("interrupted; current target restored by child; queue is resumable")
                return 130
        log_path = log_dir / f"{artifact_name(row)}.log"
        append_result(
            results,
            {
                "timestamp": datetime.now(timezone.utc).isoformat(),
                "profile": profile,
                "rank": row["rank"],
                "unit": row["unit"],
                "rva": row["rva"],
                "symbol": row["symbol"],
                "current_hash": row["current_hash"],
                "live_score": row["live_score"],
                "status": status,
                "best_score": "" if best_score is None else f"{best_score:.6f}",
                "trial_budget": args.trials,
                "elapsed_seconds": f"{elapsed:.3f}",
                "exit_code": exit_code,
                "log": log_path,
                "artifact": artifact,
            },
        )
        if status == "exact":
            exact_count += 1
        elif status == "no-exact":
            no_exact_count += 1
        else:
            error_count += 1
        print(
            f"  {status} in {elapsed:.1f}s; "
            f"exact={exact_count} no-exact={no_exact_count} errors={error_count}",
            flush=True,
        )

    if exact_count and not args.no_refresh_status:
        refresh_status(root)
    print(
        f"batch complete: tested={exact_count + no_exact_count + error_count} "
        f"exact={exact_count} no-exact={no_exact_count} errors={error_count}; "
        f"results={results}",
        flush=True,
    )
    return 1 if error_count else 0


if __name__ == "__main__":
    raise SystemExit(main())
