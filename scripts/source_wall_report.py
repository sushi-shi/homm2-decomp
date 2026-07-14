#!/usr/bin/env python3
"""Generate the durable SOURCE matching wall ledger from the current build."""

import csv
import json
import re
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
REPORT = ROOT / "build/objdiff/report.json"
BASELINE = ROOT / "config/match_baseline.tsv"
SYMBOLS = ROOT / "build/gen/symbol_names.csv"
OUTPUT = ROOT / "docs/source-wall-ledger.md"
RVA_BASE = 0x400000


def load_baseline():
    retained = defaultdict(float)
    with BASELINE.open(newline="") as stream:
        for row in csv.reader((line for line in stream if not line.startswith("#")),
                              delimiter="\t"):
            if len(row) >= 3:
                retained[(row[0], row[1])] = max(retained[(row[0], row[1])],
                                                  float(row[2]))
    return retained


def load_functions(retained):
    data = json.loads(REPORT.read_text())
    functions = {}
    for unit in data["units"]:
        unit_name = unit["name"]
        if not unit_name.startswith("SOURCE/"):
            continue
        for function in unit.get("functions") or []:
            key = (unit_name, function["name"])
            live = float(function.get("fuzzy_match_percent") or 0.0)
            functions[key] = {
                "unit": unit_name,
                "symbol": function["name"],
                "name": function.get("metadata", {}).get("demangled_name",
                                                            function["name"]),
                "size": int(function["size"]),
                "live": live,
                "retained": max(live, retained[key]),
            }
    return functions


def load_rva_keys():
    keys = {}
    with SYMBOLS.open(newline="") as stream:
        for row in csv.DictReader(stream):
            if row["kind"] == "func" and row["unit"].startswith("SOURCE/"):
                keys[int(row["rva"], 16)] = (row["unit"], row["name"])
    return keys


def clean_comment(line):
    line = re.sub(r"^\s*//\s?", "", line).strip()
    return line.replace("|", "\\|").replace("`", "'")


def load_source_markers(rva_keys):
    source = {}
    va_pattern = re.compile(r"^\s*VA\(0x([0-9a-fA-F]+)\s*,", re.MULTILINE)
    marker_pattern = re.compile(r"@(?:early-stop|match-note)")

    for path in sorted((ROOT / "src/SOURCE").glob("*.cpp")):
        text = path.read_text(errors="replace")
        lines = text.splitlines()

        previous_line = 0
        for match in va_pattern.finditer(text):
            va = int(match.group(1), 16)
            key = rva_keys.get(va - RVA_BASE)
            if not key:
                continue
            line_number = text.count("\n", 0, match.start()) + 1
            prefix = lines[previous_line:line_number - 1]
            marker_index = None
            for index, line in enumerate(prefix):
                if marker_pattern.search(line):
                    marker_index = index
            marker = None
            evidence = ""
            if marker_index is not None:
                marker_line = prefix[marker_index]
                marker = "early-stop" if "@early-stop" in marker_line else "match-note"
                evidence_lines = []
                for line in prefix[marker_index:]:
                    if line.strip().startswith("//"):
                        evidence_lines.append(clean_comment(line))
                    elif line.strip():
                        evidence_lines = []
                evidence = " ".join(evidence_lines)
                evidence = evidence.replace("@early-stop", "").replace("@match-note", "")
                evidence = re.sub(r"\s+", " ", evidence).strip(" :")
            source[key] = {
                "path": path.relative_to(ROOT).as_posix(),
                "line": line_number,
                "marker": marker,
                "evidence": evidence,
            }
            previous_line = line_number
    return source


def category(record):
    marker = record.get("marker")
    if marker == "early-stop":
        return "accepted"
    if marker == "match-note":
        return "provisional"
    return "unmarked"


def artifact_class(evidence):
    text = evidence.lower()
    if any(word in text for word in ("delink", "local-label", "local label",
                                     "reloc-naming", "jump-table", "folded")):
        return "delinker/relocation identity"
    if any(word in text for word in ("alignment", "padding", "int3", "boundary")):
        return "boundary/alignment"
    if any(word in text for word in ("inline", "continuation", "jmp $+0")):
        return "inline placement"
    if "constant-pool" in text or "constant pool" in text:
        return "constant-pool identity"
    if any(word in text for word in ("register", "compiler", "tu-state", "tu state")):
        return "compiler code shape"
    return "other byte-proven artifact"


def markdown_table(records, include_class=False):
    columns = ["Retained", "Live", "Bytes", "Function", "Source"]
    if include_class:
        columns.append("Class")
    columns.append("Evidence")
    output = ["| " + " | ".join(columns) + " |",
              "| " + " | ".join("---" for _ in columns) + " |"]
    for record in records:
        link = f"[{record['path']}:{record['line']}](../{record['path']}#L{record['line']})"
        row = [f"{record['retained']:.4f}%", f"{record['live']:.4f}%",
               str(record["size"]), record["name"].replace("|", "\\|"), link]
        if include_class:
            row.append(artifact_class(record["evidence"]))
        row.append(record["evidence"] or "No durable evidence recorded.")
        output.append("| " + " | ".join(row) + " |")
    return output


def main():
    retained = load_baseline()
    functions = load_functions(retained)
    source = load_source_markers(load_rva_keys())

    below = []
    resolved_markers = []
    for key, function in functions.items():
        record = dict(function)
        record.update(source.get(key, {"path": "", "line": 0, "marker": None,
                                       "evidence": ""}))
        if record["retained"] < 100.0:
            below.append(record)
        elif record["marker"]:
            resolved_markers.append(record)

    groups = {name: [] for name in ("accepted", "provisional", "unmarked")}
    for record in below:
        groups[category(record)].append(record)
    for records in groups.values():
        records.sort(key=lambda item: (item["unit"], item["line"], item["name"]))

    lines = [
        "# SOURCE matching wall ledger",
        "",
        "Generated by `scripts/source_wall_report.py` from the current objdiff report, retained",
        "source-hash maxima, CodeView symbol map, and durable markers immediately above each `VA()`.",
        "",
        "Only `@early-stop` entries below 100% are accepted walls. `@match-note` entries are",
        "structurally complete checkpoints that must be revisited in the final pass. Unmarked",
        "non-100 functions are ordinary queue work; this ledger does not invent wall evidence for them.",
        "",
        "## Summary",
        "",
        f"- SOURCE functions: {len(functions)}",
        f"- Retained max 100%: {sum(item['retained'] >= 100.0 for item in functions.values())}",
        f"- Accepted non-100 walls: {len(groups['accepted'])}",
        f"- Provisional non-100 checkpoints: {len(groups['provisional'])}",
        f"- Unmarked non-100 queue functions: {len(groups['unmarked'])}",
        f"- Resolved 100% functions with stale markers: {len(resolved_markers)}",
        "",
        "## Accepted Walls",
        "",
        *markdown_table(groups["accepted"], include_class=True),
        "",
        "## Provisional Checkpoints",
        "",
        *markdown_table(groups["provisional"]),
        "",
        "## Unmarked Non-100 Functions",
        "",
        *markdown_table(groups["unmarked"]),
        "",
        "## Resolved Markers",
        "",
        "These functions retain 100%; their marker is stale documentation, not a current wall.",
        "",
        *markdown_table(sorted(resolved_markers,
                               key=lambda item: (item["unit"], item["line"], item["name"])),
                         include_class=False),
        "",
    ]
    OUTPUT.write_text("\n".join(lines))
    print(f"wrote {OUTPUT.relative_to(ROOT)}")
    print(f"accepted={len(groups['accepted'])} provisional={len(groups['provisional'])} "
          f"unmarked={len(groups['unmarked'])} resolved_markers={len(resolved_markers)}")


if __name__ == "__main__":
    main()
