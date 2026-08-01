#!/usr/bin/env python3
"""Exhaust the live non-100% function queue with persistent island searches.

The queue is regenerated from the current objdiff report in descending fuzzy
percentage/RVA order. Completion is keyed by the search profile and effective
source hash, so a source or dependency change automatically makes a function
eligible again. Best candidate/retail objects, disassemblies, and diffs remain
under ``build/``; generated probe source never does.

Run inside ``nix develop .#build``::

    python3 scripts/recover_residual_functions.py --trials 128
"""

from __future__ import annotations

import argparse
import csv
import fcntl
import hashlib
import json
import os
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

from homm2.match.source_hashes import source_hashes
from homm2.permute.recover_historical_exact import (
    RESULT_FIELDS,
    append_result,
    artifact_name,
    completed_keys,
    current_baseline,
    positive_int,
    profile_id,
    read_tsv,
    repo_path,
    run_target,
)


DEFAULT_QUEUE = Path("/tmp/homm2-residual-campaign-queue.tsv")
DEFAULT_RESULTS = Path("/tmp/homm2-residual-campaign-results.tsv")
DEFAULT_LOG_DIR = Path("/tmp/homm2-residual-campaign-logs")
HISTORICAL_RESULTS = Path("/tmp/homm2-historical-exact-recovery-results.tsv")
QUEUE_FIELDS = (
    "rank",
    "priority",
    "kind",
    "unit",
    "rva",
    "symbol",
    "current_hash",
    "current_max",
    "live_score",
    "size",
    "demangled",
)


def run_checked(root: Path, command: list[str]) -> None:
    subprocess.run(command, cwd=root, check=True)


def refresh_live_report_and_queue(root: Path) -> Path:
    run_checked(root, ["homm2", "status", "--force-refresh"])
    run_checked(root, [sys.executable, str(root / "scripts/homm2/match/residual_queue.py")])
    return root / "build/gen/residual_function_queue.tsv"


def normalized_queue_rows(
    raw_rows: list[dict[str, str]],
    hashes: dict[tuple[str, str], str],
    baseline: dict[tuple[str, str], tuple[float, str]],
    object_epochs: dict[str, str],
) -> tuple[list[dict[str, str]], list[tuple[str, str]]]:
    rows = []
    stale = []
    for raw in raw_rows:
        key = (raw["unit"], raw["name"])
        current_hash = hashes.get(key)
        maximum = baseline.get(key)
        if current_hash is None:
            object_epoch = object_epochs.get(raw["unit"])
            if object_epoch is None:
                stale.append(key)
                continue
            rows.append(
                {
                    "rank": raw["rank"],
                    "priority": "",
                    "kind": "generated",
                    "unit": raw["unit"],
                    "rva": raw["rva"],
                    "symbol": raw["name"],
                    "current_hash": f"object-{object_epoch}",
                    "current_max": raw["fuzzy"],
                    "live_score": raw["fuzzy"],
                    "size": raw["size"],
                    "demangled": raw["demangled"],
                }
            )
            continue
        if maximum is None or maximum[1] != current_hash:
            stale.append(key)
            continue
        rows.append(
            {
                "rank": raw["rank"],
                "priority": "",
                "kind": "source",
                "unit": raw["unit"],
                "rva": raw["rva"],
                "symbol": raw["name"],
                "current_hash": current_hash,
                "current_max": f"{maximum[0]:.4f}",
                "live_score": raw["fuzzy"],
                "size": raw["size"],
                "demangled": raw["demangled"],
            }
        )
    return rows, stale


def normalized_object_epochs(root: Path, units: set[str]) -> dict[str, str]:
    epochs = {}
    for unit in units:
        path = root / "build/objdiff/normalized/base" / f"{unit}.obj"
        if path.is_file():
            epochs[unit] = hashlib.sha256(path.read_bytes()).hexdigest()[:16]
    return epochs


def historical_priority_keys(root: Path) -> set[tuple[str, str]]:
    output = Path("/tmp/homm2-residual-historical-priority.tsv")
    run_checked(
        root,
        [
            sys.executable,
            str(root / "scripts/historical_exact_losses.py"),
            "--output",
            str(output),
        ],
    )
    return {
        (row["unit"], row["symbol"])
        for row in read_tsv(output)
        if float(row["current_max"]) < 100.0
    }


def order_queue(
    rows: list[dict[str, str]],
    historical: set[tuple[str, str]],
) -> list[dict[str, str]]:
    def priority(row: dict[str, str]) -> int:
        key = (row["unit"], row["symbol"])
        if key in historical:
            return 1
        if float(row["live_score"]) >= 93.0:
            return 2
        return 3

    rows.sort(
        key=lambda row: (
            priority(row),
            -float(row["live_score"]),
            int(row["rva"], 0),
            row["unit"],
            row["symbol"],
        )
    )
    for rank, row in enumerate(rows, 1):
        row["rank"] = str(rank)
        row["priority"] = str(priority(row))
    return rows


def write_queue(path: Path, rows: list[dict[str, str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + ".tmp")
    with temporary.open("w", encoding="utf-8", newline="") as stream:
        writer = csv.DictWriter(
            stream, fieldnames=QUEUE_FIELDS, delimiter="\t", lineterminator="\n"
        )
        writer.writeheader()
        writer.writerows(rows)
        stream.flush()
        os.fsync(stream.fileno())
    temporary.replace(path)


def refresh_persistent_queue(root: Path, queue: Path) -> list[dict[str, str]]:
    raw_queue = refresh_live_report_and_queue(root)
    raw_rows = read_tsv(raw_queue)
    object_epochs = normalized_object_epochs(
        root, {row["unit"] for row in raw_rows}
    )
    rows, stale = normalized_queue_rows(
        raw_rows, source_hashes(), current_baseline(root), object_epochs
    )
    if stale:
        # Establish an honest ledger epoch from the freshly refreshed live
        # object before running disposable probes against it.
        run_checked(root, ["homm2", "status", "update", "--force-refresh"])
        rows, stale = normalized_queue_rows(
            raw_rows, source_hashes(), current_baseline(root), object_epochs
        )
    if stale:
        sample = ", ".join(f"{unit}::{symbol}" for unit, symbol in stale[:5])
        raise RuntimeError(
            f"{len(stale)} residuals lack a current source-hash epoch: {sample}"
        )
    rows = order_queue(rows, historical_priority_keys(root))
    write_queue(queue, rows)
    return rows


def data_totals(root: Path) -> tuple[int, int]:
    report = json.loads((root / "build/objdiff/report.json").read_text())
    matched = sum(
        int(unit.get("measures", {}).get("matched_data", 0) or 0)
        for unit in report.get("units", [])
    )
    total = sum(
        int(unit.get("measures", {}).get("total_data", 0) or 0)
        for unit in report.get("units", [])
    )
    return matched, total


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
        help="do not refresh status once after the batch",
    )
    args = parser.parse_args(argv)

    root = Path(os.environ.get("HOMM2_DIR", Path.cwd())).resolve()
    queue = repo_path(root, args.queue)
    results = repo_path(root, args.results)
    log_dir = repo_path(root, args.log_dir)
    args.artifact_root = root / "build/tu-state-noise/residual-campaign"

    lock_path = results.with_suffix(results.suffix + ".lock")
    lock_path.parent.mkdir(parents=True, exist_ok=True)
    lock = lock_path.open("w")
    try:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        print(f"another residual driver owns {lock_path}", file=sys.stderr)
        return 2

    queue_rows = refresh_persistent_queue(root, queue)
    profile = profile_id(root, args)
    args.profile = profile
    done = completed_keys(results, profile)
    if HISTORICAL_RESULTS.exists():
        done |= completed_keys(HISTORICAL_RESULTS, profile)
    pending = [
        row
        for row in queue_rows
        if (row["unit"], row["symbol"], row["current_hash"]) not in done
    ]
    if args.limit is not None:
        pending = pending[: args.limit]

    print(
        f"residual recovery: profile={profile} live={len(queue_rows)} "
        f"already-tested={len(done)} pending={len(pending)}",
        flush=True,
    )
    exact_count = 0
    no_exact_count = 0
    wall_count = 0
    error_count = 0
    source_changed = False
    for index, row in enumerate(pending, 1):
        log_path = log_dir / f"{artifact_name(row)}.log"
        if row["kind"] == "generated":
            log_path.parent.mkdir(parents=True, exist_ok=True)
            log_path.write_text(
                "compiler-generated residual has no source VA block or effective "
                "function hash; retain it as a declaration/modelling wall\n"
            )
            status, exit_code, elapsed, best_score, artifact = (
                "wall",
                0,
                0.0,
                float(row["live_score"]),
                Path(),
            )
            print(
                f"[{index}/{len(pending)}] generated-code wall "
                f"{row['unit']} {row['rva']} {row['symbol']}",
                flush=True,
            )
        else:
            latest = current_baseline(root).get((row["unit"], row["symbol"]))
            if latest != (float(row["current_max"]), row["current_hash"]):
                if latest is not None and latest[1] != row["current_hash"]:
                    print(
                        f"[{index}/{len(pending)}] stale source epoch; defer "
                        f"{row['unit']} {row['rva']} {row['symbol']}",
                        flush=True,
                    )
                    continue
                status, exit_code, elapsed, best_score, artifact = (
                    "error",
                    3,
                    0.0,
                    None,
                    Path(),
                )
            else:
                print(
                    f"[{index}/{len(pending)}] {row['unit']} {row['rva']} "
                    f"{row['symbol']} ({row['live_score']}%)",
                    flush=True,
                )
                before_hash = row["current_hash"]
                try:
                    status, exit_code, elapsed, best_score, artifact = run_target(
                        root, row, args, log_path
                    )
                except KeyboardInterrupt:
                    print(
                        "interrupted; current target restored by child; queue is resumable"
                    )
                    return 130
                after = current_baseline(root).get((row["unit"], row["symbol"]))
                source_changed |= after is not None and after[1] != before_hash

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
        elif status == "wall":
            wall_count += 1
        else:
            error_count += 1
        print(
            f"  {status} in {elapsed:.1f}s; "
            f"exact={exact_count} no-exact={no_exact_count} "
            f"walls={wall_count} errors={error_count}",
            flush=True,
        )

    if (exact_count or source_changed) and not args.no_refresh_status:
        run_checked(root, ["homm2", "status", "--force-refresh"])
    matched_data, total_data = data_totals(root)
    if matched_data != total_data:
        print(
            f"FATAL: data topology regressed to {matched_data}/{total_data}",
            file=sys.stderr,
        )
        return 3
    print(
        f"batch complete: tested="
        f"{exact_count + no_exact_count + wall_count + error_count} "
        f"exact={exact_count} no-exact={no_exact_count} "
        f"walls={wall_count} errors={error_count}; "
        f"data={matched_data}/{total_data}; results={results}",
        flush=True,
    )
    return 1 if error_count else 0


if __name__ == "__main__":
    raise SystemExit(main())
