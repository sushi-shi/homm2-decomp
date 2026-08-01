#!/usr/bin/env python3
"""Audit source-backed functions that were exact historically but are not exact now.

Historical evidence comes from every committed version of
``config/match_baseline.tsv``. Current truth comes from
``build/objdiff/report.json``. The output deliberately distinguishes an unchanged
effective-source hash from a changed hash: the former is strong TU/compiler-state
evidence, while the latter usually requires inspecting a source or helper change.

Run after ``homm2 build``::

    homm2 audit historical-losses \
        --output /tmp/homm2-historical-exact-losses.tsv
"""

from __future__ import annotations

import argparse
import csv
import json
import os
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


from homm2.core.paths import REPO

DEFAULT_BASELINE = Path("config/match_baseline.tsv")
DEFAULT_REPORT = Path("build/objdiff/report.json")
DEFAULT_SYMBOLS = Path("build/gen/symbol_names.csv")
EXACT = 100.0


@dataclass(frozen=True)
class Commit:
    sha: str
    date: str
    subject: str


@dataclass
class ExactEpoch:
    source_hash: str
    first: Commit
    last: Commit


def _repo_path(root: Path, value: Path) -> Path:
    return value if value.is_absolute() else root / value


def _git(root: Path, *args: str, check: bool = True) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        ("git", *args),
        cwd=root,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=check,
    )


def parse_baseline(text: str) -> dict[tuple[str, str], tuple[float, str]]:
    """Return the greatest recorded maximum for every ``(unit, symbol)`` row."""
    rows: dict[tuple[str, str], tuple[float, str]] = {}
    for line_number, line in enumerate(text.splitlines(), 1):
        if not line or line.startswith("#"):
            continue
        fields = line.split("\t")
        if len(fields) < 3:
            raise ValueError(f"invalid baseline row {line_number}")
        key = (fields[0], fields[1])
        # The first ledger epoch predates source hashes. Keep its exactness
        # evidence, but classify it separately instead of inventing identity.
        source_hash = fields[3] if len(fields) >= 4 else ""
        value = (float(fields[2]), source_hash)
        previous = rows.get(key)
        if previous is None or value[0] > previous[0]:
            rows[key] = value
    return rows


def live_functions(report: dict) -> dict[tuple[str, str], dict]:
    """Return unique source-facing report rows keyed by unit and symbol."""
    grouped: dict[tuple[str, str], list[dict]] = defaultdict(list)
    for unit in report.get("units", []):
        unit_name = unit.get("name", "?")
        for function in unit.get("functions", []) or []:
            grouped[(unit_name, function.get("name", "?"))].append(function)

    out = {}
    for key, rows in grouped.items():
        scored = [row for row in rows if row.get("fuzzy_match_percent") is not None]
        if len(scored) == 1:
            out[key] = scored[0]
        elif len(rows) == 1:
            out[key] = rows[0]
    return out


def symbol_inventory(path: Path) -> dict[tuple[str, str], dict]:
    if not path.exists():
        return {}
    with path.open(encoding="latin-1", newline="") as stream:
        return {
            (row["unit"], row["name"]): row
            for row in csv.DictReader(stream)
            if row.get("kind") == "func"
        }


def commit_history(root: Path, ref: str, baseline: Path) -> list[Commit]:
    relative = baseline.relative_to(root)
    result = _git(
        root,
        "log",
        "--reverse",
        "--format=%H%x09%aI%x09%s",
        ref,
        "--",
        str(relative),
    )
    commits = []
    for line in result.stdout.splitlines():
        sha, date, subject = line.split("\t", 2)
        commits.append(Commit(sha, date, subject.replace("\t", " ")))
    return commits


def historical_exact_epochs(
    root: Path, baseline: Path, commits: list[Commit]
) -> dict[tuple[str, str], list[ExactEpoch]]:
    """Recover exact epochs and their first/last observing ledger commits."""
    relative = baseline.relative_to(root)
    blob_cache: dict[str, dict[tuple[str, str], tuple[float, str]]] = {}
    epochs: dict[tuple[str, str], list[ExactEpoch]] = defaultdict(list)
    active: dict[tuple[str, str, str], ExactEpoch] = {}

    for commit in commits:
        spec = f"{commit.sha}:{relative}"
        blob = _git(root, "rev-parse", spec, check=False)
        if blob.returncode != 0:
            current_exact: set[tuple[str, str, str]] = set()
        else:
            blob_id = blob.stdout.strip()
            if blob_id not in blob_cache:
                contents = _git(root, "show", spec).stdout
                blob_cache[blob_id] = parse_baseline(contents)
            rows = blob_cache[blob_id]
            current_exact = {
                (unit, symbol, source_hash)
                for (unit, symbol), (maximum, source_hash) in rows.items()
                if maximum >= EXACT
            }

        for identity in current_exact:
            epoch = active.get(identity)
            if epoch is None:
                unit, symbol, source_hash = identity
                epoch = ExactEpoch(source_hash, commit, commit)
                active[identity] = epoch
                epochs[(unit, symbol)].append(epoch)
            else:
                epoch.last = commit
        for identity in set(active) - current_exact:
            del active[identity]

    return epochs


def current_hash_origins(
    root: Path,
    baseline: Path,
    commits: list[Commit],
    current: dict[tuple[str, str], tuple[float, str]],
) -> dict[tuple[str, str], tuple[Commit, tuple[float, str] | None]]:
    """Return the commit that began each current effective-source-hash epoch."""
    relative = baseline.relative_to(root)
    blob_cache: dict[str, dict[tuple[str, str], tuple[float, str]]] = {}
    previous: dict[tuple[str, str], tuple[float, str]] = {}
    origins = {}
    for commit in commits:
        spec = f"{commit.sha}:{relative}"
        blob = _git(root, "rev-parse", spec, check=False)
        if blob.returncode != 0:
            rows = {}
        else:
            blob_id = blob.stdout.strip()
            if blob_id not in blob_cache:
                blob_cache[blob_id] = parse_baseline(_git(root, "show", spec).stdout)
            rows = blob_cache[blob_id]
        for key, value in rows.items():
            old = previous.get(key)
            if (
                value[1]
                and current.get(key, (None, None))[1] == value[1]
                and (old is None or old[1] != value[1])
            ):
                origins[key] = (commit, old)
        previous = rows
    return origins


def hash_relation(current_hash: str, historical_hashes: set[str]) -> str:
    if current_hash in historical_hashes:
        return "same-effective-hash"
    if any(
        "." not in old_hash and current_hash.startswith(old_hash + ".")
        for old_hash in historical_hashes
        if old_hash
    ):
        return "body-hash-upgrade"
    if not any(historical_hashes):
        return "unknown-historical-hash"
    return "changed-effective-hash"


def loss_rows(
    live: dict[tuple[str, str], dict],
    current: dict[tuple[str, str], tuple[float, str]],
    epochs: dict[tuple[str, str], list[ExactEpoch]],
    symbols: dict[tuple[str, str], dict],
    origins: dict[
        tuple[str, str], tuple[Commit, tuple[float, str] | None]
    ] | None = None,
) -> list[dict]:
    origins = origins or {}
    rows = []
    relation_rank = {
        "same-effective-hash": 0,
        "body-hash-upgrade": 1,
        "changed-effective-hash": 2,
        "unknown-historical-hash": 3,
    }
    for key, function in live.items():
        score = float(function.get("fuzzy_match_percent", 0.0) or 0.0)
        if score >= EXACT or key not in current or key not in epochs:
            continue
        current_max, current_hash = current[key]
        exact_epochs = epochs[key]
        historical_hashes = {epoch.source_hash for epoch in exact_epochs}
        relation = hash_relation(current_hash, historical_hashes)
        relevant = [
            epoch for epoch in exact_epochs
            if (
                epoch.source_hash == current_hash
                if relation == "same-effective-hash"
                else (
                    "." not in epoch.source_hash
                    and current_hash.startswith(epoch.source_hash + ".")
                    if relation == "body-hash-upgrade"
                    else True
                )
            )
        ]
        first = min(relevant, key=lambda epoch: epoch.first.date)
        last = max(relevant, key=lambda epoch: epoch.last.date)
        symbol = symbols.get(key, {})
        origin, previous = origins.get(key, (None, None))
        transition_from_exact = (
            previous is not None
            and previous[0] >= EXACT
            and previous[1] != current_hash
        )
        rows.append({
            "relation": relation,
            "live_score": score,
            "current_max": current_max,
            "unit": key[0],
            "rva": int(symbol["rva"], 0) if symbol.get("rva") else None,
            "size": int(function.get("size", 0) or 0),
            "symbol": key[1],
            "current_hash": current_hash,
            "historical_hashes": ",".join(
                sorted(source_hash or "<pre-hash>" for source_hash in historical_hashes)
            ),
            "exact_epochs": len(exact_epochs),
            "first_exact_commit": first.first.sha[:12],
            "first_exact_date": first.first.date,
            "first_exact_subject": first.first.subject,
            "last_exact_commit": last.last.sha[:12],
            "last_exact_date": last.last.date,
            "last_exact_subject": last.last.subject,
            "current_hash_commit": origin.sha[:12] if origin else "",
            "current_hash_date": origin.date if origin else "",
            "current_hash_subject": origin.subject if origin else "",
            "previous_max": previous[0] if previous else None,
            "previous_hash": previous[1] if previous else "",
            "origin_kind": "hash-transition" if previous else "ledger-reentry",
            "transition_from_exact": transition_from_exact,
            "_relation_rank": relation_rank[relation],
        })
    rows.sort(key=lambda row: (
        row["_relation_rank"],
        -row["live_score"],
        row["rva"] if row["rva"] is not None else sys.maxsize,
        row["unit"],
        row["symbol"],
    ))
    return rows


def write_tsv(path: Path | None, rows: list[dict]) -> None:
    fields = (
        "rank",
        "relation",
        "live_score",
        "current_max",
        "unit",
        "rva",
        "size",
        "symbol",
        "current_hash",
        "historical_hashes",
        "exact_epochs",
        "first_exact_commit",
        "first_exact_date",
        "first_exact_subject",
        "last_exact_commit",
        "last_exact_date",
        "last_exact_subject",
        "current_hash_commit",
        "current_hash_date",
        "current_hash_subject",
        "previous_max",
        "previous_hash",
        "origin_kind",
        "transition_from_exact",
    )
    stream = sys.stdout if path is None else path.open("w", encoding="utf-8", newline="")
    try:
        writer = csv.DictWriter(
            stream, fieldnames=fields, delimiter="\t", lineterminator="\n"
        )
        writer.writeheader()
        for rank, row in enumerate(rows, 1):
            output = dict(row)
            output["rank"] = rank
            output["live_score"] = f"{row['live_score']:.6f}"
            output["current_max"] = f"{row['current_max']:.4f}"
            output["rva"] = "" if row["rva"] is None else f"0x{row['rva']:08x}"
            output["previous_max"] = (
                "" if row["previous_max"] is None else f"{row['previous_max']:.4f}"
            )
            output["transition_from_exact"] = (
                "yes" if row["transition_from_exact"] else "no"
            )
            writer.writerow({field: output.get(field, "") for field in fields})
    finally:
        if path is not None:
            stream.close()


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ref", default="HEAD", help="Git history tip (default: HEAD)")
    parser.add_argument("--baseline", type=Path, default=DEFAULT_BASELINE)
    parser.add_argument("--report", type=Path, default=DEFAULT_REPORT)
    parser.add_argument("--symbols", type=Path, default=DEFAULT_SYMBOLS)
    parser.add_argument("--output", type=Path, help="TSV path; default is stdout")
    args = parser.parse_args(argv)

    root = REPO.resolve()
    baseline = _repo_path(root, args.baseline).resolve()
    report_path = _repo_path(root, args.report).resolve()
    symbols_path = _repo_path(root, args.symbols).resolve()
    output = _repo_path(root, args.output).resolve() if args.output else None
    if output is not None:
        output.parent.mkdir(parents=True, exist_ok=True)

    current = parse_baseline(baseline.read_text())
    report = json.loads(report_path.read_text())
    commits = commit_history(root, args.ref, baseline)
    epochs = historical_exact_epochs(root, baseline, commits)
    origins = current_hash_origins(root, baseline, commits, current)
    rows = loss_rows(
        live_functions(report),
        current,
        epochs,
        symbol_inventory(symbols_path),
        origins,
    )
    write_tsv(output, rows)

    counts = defaultdict(int)
    for row in rows:
        counts[row["relation"]] += 1
        if row["transition_from_exact"]:
            counts["transition-from-exact"] += 1
    destination = str(output) if output is not None else "stdout"
    print(
        "historical exact losses: "
        f"{len(rows)} -> {destination} "
        f"(same-hash={counts['same-effective-hash']}, "
        f"body-upgrade={counts['body-hash-upgrade']}, "
        f"changed-hash={counts['changed-effective-hash']}, "
        f"unknown-hash={counts['unknown-historical-hash']}, "
        f"transition-from-exact={counts['transition-from-exact']})",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
