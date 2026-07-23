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
import signal
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path

from historical_exact_losses import parse_baseline


DEFAULT_QUEUE = Path("/tmp/homm2-historical-exact-recovery-queue.tsv")
DEFAULT_RESULTS = Path("/tmp/homm2-historical-exact-recovery-results.tsv")
DEFAULT_LOG_DIR = Path("/tmp/homm2-historical-exact-recovery-logs")
TERMINATION_GRACE_SECONDS = 3.0
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
    "trial_budget",
    "elapsed_seconds",
    "exit_code",
    "log",
)


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
        if row.get("status") not in {"exact", "no-exact"}:
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


def run_target(
    root: Path,
    row: dict[str, str],
    args: argparse.Namespace,
    log_path: Path,
) -> tuple[str, int, float]:
    source = source_for_unit(root, row["unit"])
    if not source.is_file():
        return "error", 2, 0.0
    output = (
        root / "build/tu-state-noise/historical-exact-recovery"
        / artifact_name(row)
    )
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
        "--record-max",
    ]
    log_path.parent.mkdir(parents=True, exist_ok=True)
    started = time.monotonic()
    with log_path.open("w", encoding="utf-8") as log:
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
            exit_code = process.wait(timeout=args.target_timeout_seconds)
        except subprocess.TimeoutExpired:
            terminate_process(process)
            return "timeout", 124, time.monotonic() - started
        except KeyboardInterrupt:
            terminate_process(process)
            raise
    elapsed = time.monotonic() - started
    if exit_code != 0:
        return "error", exit_code, elapsed
    maximum, source_hash = current_baseline(root).get(
        (row["unit"], row["symbol"]), (0.0, "")
    )
    if source_hash != row["current_hash"]:
        return "error", 3, elapsed
    return ("exact" if maximum >= 100.0 else "no-exact"), exit_code, elapsed


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
            status, exit_code, elapsed = "error", 3, 0.0
        else:
            log_path = log_dir / f"{artifact_name(row)}.log"
            print(
                f"[{index}/{len(pending)}] {row['unit']} {row['rva']} "
                f"{row['symbol']} ({row['live_score']}%)",
                flush=True,
            )
            try:
                status, exit_code, elapsed = run_target(
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
                "trial_budget": args.trials,
                "elapsed_seconds": f"{elapsed:.3f}",
                "exit_code": exit_code,
                "log": log_path,
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
