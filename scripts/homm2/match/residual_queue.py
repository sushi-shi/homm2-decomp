#!/usr/bin/env python3
"""Generate the exhaustive live non-exact function queue."""

import csv
import json
import os
import re
from pathlib import Path


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
IMAGE_BASE = 0x400000
REPORT = REPO / "build/objdiff/report.json"
SYMBOLS = REPO / "build/gen/symbol_names.csv"
OUTPUT = REPO / "build/gen/residual_function_queue.tsv"
VA_RE = re.compile(r"^\s*VA\(0x([0-9a-fA-F]+),")


def source_markers(source_root):
    markers = {}
    for path in sorted(Path(source_root).rglob("*.cpp")):
        lines = path.read_text(encoding="latin-1").splitlines()
        for index, line in enumerate(lines):
            match = VA_RE.match(line)
            if not match:
                continue
            marker = ""
            cursor = index - 1
            while cursor >= 0 and (not lines[cursor].strip() or
                                   lines[cursor].lstrip().startswith("//")):
                text = lines[cursor]
                if "@early-stop" in text:
                    marker = "early-stop"
                elif "@semantic" in text and not marker:
                    marker = "semantic"
                cursor -= 1
            va = int(match.group(1), 16)
            markers[va - IMAGE_BASE if va >= IMAGE_BASE else va] = marker
    return markers


def symbol_inventory(path):
    rows = {}
    with Path(path).open(encoding="latin-1", newline="") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") != "func":
                continue
            rows[(row["unit"], row["name"])] = row
    return rows


def residual_rows(report, symbols, markers):
    rows = []
    missing = []
    for unit in report["units"]:
        unit_name = unit["name"]
        for function in unit.get("functions", []):
            fuzzy = float(function.get("fuzzy_match_percent", 0.0) or 0.0)
            if fuzzy >= 100.0:
                continue
            key = (unit_name, function["name"])
            symbol = symbols.get(key)
            if symbol is None:
                missing.append(key)
                continue
            rva = int(symbol["rva"], 0)
            rows.append({
                "unit": unit_name,
                "rva": rva,
                "fuzzy": fuzzy,
                "size": int(function.get("size", 0) or 0),
                "name": function["name"],
                "demangled": function.get("metadata", {}).get(
                    "demangled_name", function["name"]),
                "marker": markers.get(rva, ""),
            })
    if missing:
        sample = ", ".join("%s:%s" % row for row in missing[:5])
        raise ValueError("%d report functions lack an RVA inventory row: %s" %
                         (len(missing), sample))
    rows.sort(key=lambda row: (-row["fuzzy"], row["rva"], row["unit"], row["name"]))
    for rank, row in enumerate(rows, 1):
        row["rank"] = rank
    return rows


def write_queue(path, rows):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    fields = ("rank", "fuzzy", "unit", "rva", "va", "size", "marker",
              "name", "demangled")
    with path.open("w", encoding="utf-8", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields, delimiter="\t",
                                lineterminator="\n")
        writer.writeheader()
        for row in rows:
            output = dict(row)
            output["fuzzy"] = "%.6f" % row["fuzzy"]
            output["rva"] = "0x%08x" % row["rva"]
            output["va"] = "0x%08x" % (row["rva"] + IMAGE_BASE)
            writer.writerow({field: output.get(field, "") for field in fields})


def main():
    report = json.loads(REPORT.read_text(encoding="utf-8"))
    rows = residual_rows(report, symbol_inventory(SYMBOLS),
                         source_markers(REPO / "src"))
    write_queue(OUTPUT, rows)
    marked = sum(bool(row["marker"]) for row in rows)
    print("residual queue: %d live non-exact functions, %d with prior markers -> %s" %
          (len(rows), marked, OUTPUT.relative_to(REPO)))


if __name__ == "__main__":
    main()
