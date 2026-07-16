#!/usr/bin/env python3
"""Track the one-pass audit of source-backed non-exact functions.

The state is deliberately independent of live candidate object hashes.  A row
remains checked while unrelated source edits perturb its TU, but becomes pending
when its own normalized source hash changes or the comparison epoch changes.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import math
import os
import shutil
import sys
import tempfile
from pathlib import Path

from homm2.match import status


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
STATE = REPO / "config/breadth_audit.json"
STATE_VERSION = 1
EPOCH_VERSION = 1
OUTCOMES = frozenset(("exact", "improved", "parked"))


class AuditError(RuntimeError):
    pass


def _sha256(path):
    digest = hashlib.sha256()
    with Path(path).open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _json_bytes(value):
    return json.dumps(value, sort_keys=True, separators=(",", ":")).encode("utf-8")


def comparison_epoch(repo=REPO, objdiff_cli=None):
    """Return a candidate-independent fingerprint of comparison semantics."""
    repo = Path(repo)
    objdiff = repo / "build/objdiff"
    config_path = objdiff / "objdiff.json"
    canonicalizer = repo / "scripts/homm2/build/canonicalize_data_symbols.py"
    try:
        config = json.loads(config_path.read_text(encoding="utf-8"))
    except (FileNotFoundError, OSError, json.JSONDecodeError) as error:
        raise AuditError("cannot read objdiff comparison config: %s" % error) from error
    if not isinstance(config, dict) or not isinstance(config.get("units"), list):
        raise AuditError("objdiff comparison config has no units list")

    projected = dict(config)
    projected_units = []
    targets = []
    seen_units = set()
    digest_cache = {}
    for raw in config["units"]:
        if not isinstance(raw, dict):
            raise AuditError("objdiff comparison config contains a non-object unit")
        unit = raw.get("name")
        reference = raw.get("target_path")
        if not isinstance(unit, str) or not unit or unit in seen_units:
            raise AuditError("objdiff comparison config has an invalid/duplicate unit")
        if not isinstance(reference, str) or not reference:
            raise AuditError("objdiff unit %s has no target_path" % unit)
        seen_units.add(unit)
        entry = dict(raw)
        entry.pop("base_path", None)
        projected_units.append(entry)
        target = (objdiff / reference).resolve()
        if not target.is_file():
            raise AuditError("comparison target is missing: %s" % target)
        key = str(target)
        digest_cache.setdefault(key, _sha256(target))
        targets.append({
            "unit": unit,
            "reference": reference,
            "sha256": digest_cache[key],
        })
    projected["units"] = projected_units

    executable_name = objdiff_cli or shutil.which("objdiff-cli")
    if not executable_name:
        raise AuditError("objdiff-cli is required to compute the comparison epoch")
    executable = Path(executable_name).resolve()
    if not executable.is_file():
        raise AuditError("objdiff-cli is not a file: %s" % executable)
    if not canonicalizer.is_file():
        raise AuditError("comparison canonicalizer is missing: %s" % canonicalizer)

    identity = {
        "version": EPOCH_VERSION,
        "comparison_config": projected,
        "target_objects": targets,
        "objdiff_cli_sha256": _sha256(executable),
        "canonicalizer_sha256": _sha256(canonicalizer),
    }
    return hashlib.sha256(_json_bytes(identity)).hexdigest()


def _record_key(record):
    return record["unit"], record["function"]


def _validate_percent(value, field):
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise AuditError("%s must be a number" % field)
    value = float(value)
    if not math.isfinite(value) or not 0.0 <= value <= 100.0:
        raise AuditError("%s must be between 0 and 100" % field)
    return value


def _validate_record(record):
    required = {"unit", "function", "source_hash", "epoch", "outcome",
                "iterations", "before", "after", "note"}
    if not isinstance(record, dict) or set(record) != required:
        raise AuditError("audit record fields must be exactly: %s" %
                         ", ".join(sorted(required)))
    for field in ("unit", "function", "note"):
        if not isinstance(record[field], str) or not record[field].strip():
            raise AuditError("audit record %s must be a non-empty string" % field)
    if (not isinstance(record["source_hash"], str) or
            len(record["source_hash"]) != 12 or
            any(character not in "0123456789abcdef" for character in record["source_hash"])):
        raise AuditError("audit record source_hash must be 12 lowercase hex characters")
    if (not isinstance(record["epoch"], str) or len(record["epoch"]) != 64 or
            any(character not in "0123456789abcdef" for character in record["epoch"])):
        raise AuditError("audit record epoch must be 64 lowercase hex characters")
    if record["outcome"] not in OUTCOMES:
        raise AuditError("audit outcome must be exact, improved, or parked")
    iterations = record["iterations"]
    if isinstance(iterations, bool) or not isinstance(iterations, int) or not 0 <= iterations <= 10:
        raise AuditError("audit iterations must be an integer from 0 through 10")
    _validate_percent(record["before"], "before")
    _validate_percent(record["after"], "after")
    return record


def load_state(path=STATE):
    path = Path(path)
    if not path.exists():
        return {"version": STATE_VERSION, "records": []}
    try:
        state = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise AuditError("cannot read breadth audit state: %s" % error) from error
    if (not isinstance(state, dict) or set(state) != {"version", "records"} or
            state.get("version") != STATE_VERSION or
            not isinstance(state.get("records"), list)):
        raise AuditError("breadth audit state has an unsupported or malformed schema")
    seen = set()
    for record in state["records"]:
        _validate_record(record)
        key = _record_key(record)
        if key in seen:
            raise AuditError("breadth audit state has duplicate function: %s::%s" % key)
        seen.add(key)
    return state


def write_state(state, path=STATE):
    path = Path(path)
    records = sorted(state["records"], key=lambda row: (row["unit"], row["function"]))
    payload = json.dumps({"version": STATE_VERSION, "records": records},
                         indent=2, ensure_ascii=True) + "\n"
    path.parent.mkdir(parents=True, exist_ok=True)
    handle, temporary = tempfile.mkstemp(prefix=".%s." % path.name, dir=path.parent)
    try:
        with os.fdopen(handle, "w", encoding="utf-8") as stream:
            stream.write(payload)
        os.replace(temporary, path)
    except BaseException:
        try:
            os.unlink(temporary)
        except FileNotFoundError:
            pass
        raise


def report_functions(report):
    rows = {}
    duplicates = set()
    if not isinstance(report, dict) or not isinstance(report.get("units"), list):
        raise AuditError("objdiff report has no units list")
    for unit in report["units"]:
        unit_name = unit.get("name")
        if not isinstance(unit_name, str) or not unit_name:
            raise AuditError("objdiff report has an invalid unit name")
        for function in unit.get("functions", []) or []:
            name = function.get("name")
            if not isinstance(name, str) or not name:
                raise AuditError("objdiff report has an invalid function name")
            key = (unit_name, name)
            if key in rows:
                duplicates.add(key)
            rows[key] = function
    return rows, duplicates


def audit_universe(report, source_hashes):
    rows, duplicates = report_functions(report)
    universe = {}
    for key, source_hash in source_hashes.items():
        if key in duplicates:
            raise AuditError("source-backed report function is duplicated: %s::%s" % key)
        function = rows.get(key)
        if function is None:
            raise AuditError("source-backed function is missing from report: %s::%s" % key)
        if not source_hash:
            raise AuditError("source-backed function has an empty source hash: %s::%s" % key)
        fuzzy = _validate_percent(
            float(function.get("fuzzy_match_percent") or 0.0), "report fuzzy")
        universe[key] = {
            "unit": key[0], "function": key[1], "source_hash": source_hash,
            "fuzzy": fuzzy, "size": int(function.get("size", 0) or 0),
            "demangled": (function.get("metadata", {}) or {}).get(
                "demangled_name", key[1]),
        }
    return universe


def classify(report, source_hashes, state, epoch, module=None):
    universe = audit_universe(report, source_hashes)
    records = {_record_key(record): record for record in state["records"]}
    unknown = sorted(set(records) - set(universe))
    if unknown:
        sample = ", ".join("%s::%s" % key for key in unknown[:5])
        raise AuditError("audit state contains function(s) absent from source/report: %s" % sample)
    module = module.rstrip("/") if module else None
    exact = []
    checked = []
    pending = []
    stale = []
    for key, row in universe.items():
        if (module and row["unit"] != module and
                not row["unit"].startswith(module + "/")):
            continue
        if row["fuzzy"] >= 100.0:
            exact.append(row)
            continue
        record = records.get(key)
        if (record is not None and record["source_hash"] == row["source_hash"] and
                record["epoch"] == epoch):
            checked.append((row, record))
        else:
            pending.append(row)
            if record is not None:
                stale.append((row, record))
    pending.sort(key=lambda row: (-row["fuzzy"], row["unit"], row["function"]))
    return {
        "total": len(exact) + len(checked) + len(pending),
        "exact": exact, "checked": checked, "pending": pending, "stale": stale,
    }


def _load_batch(path):
    stream = sys.stdin if str(path) == "-" else Path(path).open(
        encoding="utf-8", newline="")
    try:
        reader = csv.DictReader(stream, delimiter="\t")
        required = {"unit", "function", "outcome", "iterations", "before", "after", "note"}
        optional = {"source_hash", "epoch"}
        fields = set(reader.fieldnames or ())
        if not required <= fields or fields - required - optional:
            raise AuditError("batch TSV requires %s (optional: source_hash, epoch)" %
                             ", ".join(sorted(required)))
        return [dict(row) for row in reader]
    finally:
        if stream is not sys.stdin:
            stream.close()


def record_audits(requests, report, source_hashes, state, epoch):
    universe = audit_universe(report, source_hashes)
    existing = {_record_key(record): record for record in state["records"]}
    seen = set()
    for request in requests:
        key = (request.get("unit", ""), request.get("function", ""))
        if key in seen:
            raise AuditError("batch contains duplicate function: %s::%s" % key)
        seen.add(key)
        row = universe.get(key)
        if row is None:
            raise AuditError("function/source hash is missing: %s::%s" % key)
        asserted_hash = request.get("source_hash")
        if asserted_hash and asserted_hash != row["source_hash"]:
            raise AuditError("source hash changed for %s::%s" % key)
        asserted_epoch = request.get("epoch")
        if asserted_epoch and asserted_epoch != epoch:
            raise AuditError("comparison epoch changed for %s::%s" % key)
        try:
            iterations = int(request["iterations"])
            before = float(request["before"])
            after = float(request["after"])
        except (KeyError, TypeError, ValueError) as error:
            raise AuditError("invalid numeric audit fields for %s::%s" % key) from error
        record = _validate_record({
            "unit": key[0], "function": key[1],
            "source_hash": row["source_hash"], "epoch": epoch,
            "outcome": request.get("outcome"), "iterations": iterations,
            "before": before, "after": after, "note": request.get("note", ""),
        })
        if record["outcome"] == "exact" and record["after"] != 100.0:
            raise AuditError("exact outcome requires an after score of 100%%: %s::%s" % key)
        if record["outcome"] == "improved" and record["after"] <= record["before"]:
            raise AuditError("improved outcome requires after > before: %s::%s" % key)
        existing[key] = record
    return {"version": STATE_VERSION, "records": list(existing.values())}


def _current():
    report = status.load_report()
    if report is None:
        raise AuditError("objdiff report is unavailable; run homm2 build")
    hashes = status.source_hashes()
    if not hashes:
        raise AuditError("function source hashes are unavailable; run homm2 build")
    return report, hashes, comparison_epoch()


def _print_summary(classified, epoch):
    nonexact = len(classified["checked"]) + len(classified["pending"])
    print("breadth audit epoch: %s" % epoch)
    print("source-backed: %d  exact/no-record: %d  nonexact: %d  "
          "checked: %d  remaining: %d  stale: %d" % (
              classified["total"], len(classified["exact"]), nonexact,
              len(classified["checked"]), len(classified["pending"]),
              len(classified["stale"])))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    subparsers.add_parser("epoch", help="print the current comparison epoch")

    record = subparsers.add_parser("record", help="record one or a TSV batch of audits")
    record.add_argument("--input", help="batch TSV path, or - for stdin")
    record.add_argument("--unit")
    record.add_argument("--function")
    record.add_argument("--outcome", choices=sorted(OUTCOMES))
    record.add_argument("--iterations", type=int)
    record.add_argument("--before", type=float)
    record.add_argument("--after", type=float)
    record.add_argument("--note")
    record.add_argument("--source-hash", help="optional stale-handoff assertion")
    record.add_argument("--epoch", help="optional stale-handoff assertion")

    queue = subparsers.add_parser("queue", help="print closest-to-100 pending functions")
    queue.add_argument("--module", help="restrict to a module such as SOURCE or BASE")
    queue.add_argument("--limit", type=int, default=0, help="maximum rows; zero means all")
    summary = subparsers.add_parser("summary", help="print exact checked/remaining counts")
    summary.add_argument("--module", help="restrict to a module such as SOURCE or BASE")
    args = parser.parse_args(argv)

    try:
        if args.command == "epoch":
            print(comparison_epoch())
            return 0
        report, hashes, epoch = _current()
        state = load_state()
        # Validate every retained identity before using or updating the state.
        classify(report, hashes, state, epoch)
        if args.command == "record":
            if args.input:
                if any(value is not None for value in (
                        args.unit, args.function, args.outcome, args.iterations,
                        args.before, args.after, args.note, args.source_hash, args.epoch)):
                    raise AuditError("--input cannot be combined with one-record fields")
                requests = _load_batch(args.input)
            else:
                required = (args.unit, args.function, args.outcome, args.iterations,
                            args.before, args.after, args.note)
                if any(value is None for value in required):
                    raise AuditError("one-record mode requires unit, function, outcome, "
                                     "iterations, before, after, and note")
                requests = [{
                    "unit": args.unit, "function": args.function,
                    "outcome": args.outcome, "iterations": args.iterations,
                    "before": args.before, "after": args.after, "note": args.note,
                    "source_hash": args.source_hash, "epoch": args.epoch,
                }]
            if not requests:
                raise AuditError("audit batch is empty")
            updated = record_audits(requests, report, hashes, state, epoch)
            write_state(updated)
            print("breadth audit: recorded %d function(s); %d retained row(s)" %
                  (len(requests), len(updated["records"])))
            return 0
        classified = classify(report, hashes, state, epoch, args.module)
        if args.command == "summary":
            _print_summary(classified, epoch)
            return 0
        rows = classified["pending"]
        if args.limit < 0:
            raise AuditError("--limit must be zero or positive")
        if args.limit:
            rows = rows[:args.limit]
        writer = csv.writer(sys.stdout, delimiter="\t", lineterminator="\n")
        writer.writerow(("rank", "fuzzy", "unit", "size", "source_hash",
                         "function", "demangled"))
        for rank, row in enumerate(rows, 1):
            writer.writerow((rank, "%.6f" % row["fuzzy"], row["unit"], row["size"],
                             row["source_hash"], row["function"], row["demangled"]))
        print("breadth audit: %d checked, %d remaining%s" % (
            len(classified["checked"]), len(classified["pending"]),
            " (showing %d)" % len(rows) if len(rows) != len(classified["pending"]) else ""),
              file=sys.stderr)
        return 0
    except (AuditError, OSError, ValueError) as error:
        print("breadth audit: %s" % error, file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
