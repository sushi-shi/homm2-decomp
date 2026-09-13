"""Check the pinned follow-up queue; review coverage is not defect closure."""
import argparse
import collections
import csv
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CATEGORIES = {"casts": 86, "address": 9, "outside": 66}
STATES = {"-", "pending", "fixed", "replaced", "covered-pr", "contract", "required", "partial", "open"}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--require-reviewed", action="store_true")
    args = parser.parse_args()
    rows = list(csv.DictReader((ROOT / "docs/casts-pointers-master.tsv").open(), delimiter="\t"))
    errors = []
    if len(rows) != 148 or len({r["id"] for r in rows}) != 148:
        errors.append("Expected exactly 148 distinct inherited function IDs")
    for category, expected in CATEGORIES.items():
        counts = collections.Counter(r[category] for r in rows if r[category] != "-")
        print(f"{category}: {dict(sorted(counts.items()))}")
        if sum(counts.values()) != expected:
            errors.append(f"{category}: expected {expected} pinned entries")
    for row in rows:
        if any(row[c] not in STATES for c in CATEGORIES):
            errors.append(f"{row['id']}: unknown state")
        if args.require_reviewed and any(row[c] == "pending" for c in CATEGORIES):
            errors.append(f"{row['id']}: review pending")
        if all(row[c] in {"pending", "-"} for c in CATEGORIES):
            continue
        path = ROOT / row["master_path"]
        digest = hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else "absent"
        if digest != row["reviewed_file_sha256"]:
            errors.append(f"{row['id']}: source changed since review; reassess, do not blindly refresh")
        if not row["evidence"] or row["evidence"].startswith("Not yet"):
            errors.append(f"{row['id']}: missing evidence")
    print(f"{len(rows)} functions; {sum(CATEGORIES.values())} category entries")
    for error in errors:
        print(error)
    return bool(errors)


if __name__ == "__main__":
    raise SystemExit(main())
