"""Audit reviewed HoMM2 data allocations with objdiff's strict symbol schema."""

import argparse
import base64
import csv
import hashlib
import json
import os
import shutil
import subprocess
import sys
import time
from collections import defaultdict
from pathlib import Path

from homm2.build.link_exe import (
    load_required_initialized_storage,
    read_pe_payload_evidence,
)
from homm2.build.reviewed_data import ensure_reviewed_targets


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
LEDGER = REPO / "config/required_initialized_storage.tsv"
SYMBOLS = REPO / "build/gen/symbol_names.csv"
RETAIL_EXE = REPO / "build/orig/HEROES2W.EXE"
OBJDIFF_PROJECT = REPO / "build/objdiff"
OUTPUT = REPO / "build/gen/strict-allocations"
GENERIC_CHECKER = REPO / "scripts/strict_allocation_diff.py"


def _number(value):
    if value is None:
        return 0
    if isinstance(value, int):
        return value
    return int(str(value), 0)


def _payload(symbol):
    return b"".join(
        base64.b64decode(segment.get("data", ""))
        for segment in symbol.get("data_diff", [])
    )


def _cstring(symbol):
    payload = _payload(symbol)
    terminator = payload.find(b"\0")
    if terminator < 0:
        raise ValueError("candidate literal has no serialized NUL terminator")
    return payload[:terminator]


def _unique_object(side, name):
    matches = [
        symbol for symbol in side.get("symbols", [])
        if symbol.get("kind") == "SYMBOL_OBJECT" and symbol.get("name") == name
    ]
    if len(matches) != 1:
        raise ValueError("allocation %r has %d object definitions" % (name, len(matches)))
    return matches[0]


def _allocation_relocations(side, symbol, extent):
    start = _number(symbol.get("address"))
    end = start + extent
    symbols = side.get("symbols", [])
    rows = []
    for item in symbol.get("data_relocations", []):
        item_start = _number(item.get("start"))
        item_end = _number(item.get("end"))
        if item_start < start or item_start >= end:
            continue
        if item_end > end:
            raise ValueError("relocation at %#x crosses reviewed extent" % (item_start - start))
        relocation = item.get("relocation") or {}
        target_index = _number(relocation.get("target_symbol"))
        if target_index < 0 or target_index >= len(symbols):
            raise ValueError("invalid relocation target index %d" % target_index)
        rows.append({
            "offset": item_start - start,
            "type": _number(relocation.get("type")),
            "addend": _number(relocation.get("addend")),
            "target": symbols[target_index],
        })
    return sorted(rows, key=lambda row: row["offset"])


def load_literal_inventory(path=SYMBOLS):
    by_name = defaultdict(list)
    with Path(path).open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") != "data":
                continue
            if row.get("provenance") != "pe-reloc-constant":
                continue
            by_name[row["name"]].append({**row, "rva": int(row["rva"], 0)})
    return dict(by_name)


def _identity_pattern(values):
    identities = {}
    return [identities.setdefault(value, len(identities)) for value in values]


def _prove_base_literal(side, symbol):
    name = symbol.get("name", "")
    definitions = [
        item for item in side.get("symbols", [])
        if item.get("kind") == "SYMBOL_OBJECT" and item.get("name") == name and
        "section" in item
    ]
    if len(definitions) != 1:
        raise ValueError("base literal %r has %d defined object identities" %
                         (name, len(definitions)))
    section_index = _number(definitions[0]["section"])
    sections = side.get("sections", [])
    if section_index < 0 or section_index >= len(sections):
        raise ValueError("base literal %r has invalid section %d" % (name, section_index))
    if sections[section_index].get("kind") != "SECTION_DATA":
        raise ValueError("base literal %r is not in initialized data" % name)
    return _cstring(definitions[0])


def _exclude(excluded, row, reason):
    excluded[row["name"]] = {
        "name": row["name"],
        "unit": row["unit"],
        "audit": row["audit"],
        "reason": reason,
    }


def derive_manifest(unit, rows, diff, literal_inventory, retail_exe=RETAIL_EXE):
    """Return a strict manifest and exact cstring exclusions for one unit."""
    wrong_units = sorted({row["unit"] for row in rows if row["unit"] != unit})
    if wrong_units:
        raise ValueError(
            "manifest for %s contains rows from %s" % (unit, ", ".join(wrong_units)))
    left = diff.get("left") or {}
    right = diff.get("right") or {}
    allocations = []
    excluded = {}
    observations = []

    for row in rows:
        allocation = {
            "target_name": row["name"],
            "extent": row["size"],
            "section_kind": "SECTION_DATA",
        }
        if row["audit"] == "bytes":
            allocations.append(allocation)
            continue
        if row["audit"] != "cstring-pointer-table":
            _exclude(excluded, row, "unsupported ledger audit kind %r" % row["audit"])
            continue

        try:
            target_allocation = _unique_object(left, row["name"])
            base_allocation = _unique_object(right, row["name"])
            target_relocs = _allocation_relocations(left, target_allocation, row["size"])
            base_relocs = _allocation_relocations(right, base_allocation, row["size"])
            retail = read_pe_payload_evidence(
                retail_exe,
                row["retail_rva"],
                row["size"],
                "cstring-pointer-table",
            )
            if retail["sha256"] != row["retail_sha256"]:
                raise ValueError("reviewed retail table hash changed")
            if retail["highlow_base_relocation_count"] != row["highlow_count"]:
                raise ValueError("reviewed retail relocation count changed")
            if len(target_relocs) != row["highlow_count"] or len(base_relocs) != row["highlow_count"]:
                raise ValueError(
                    "COFF relocation count differs: target=%d base=%d reviewed=%d" %
                    (len(target_relocs), len(base_relocs), row["highlow_count"]))
            offsets = retail["highlow_relative_offsets"]
            if [item["offset"] for item in target_relocs] != offsets:
                raise ValueError("target relocation offsets differ from retail PE")
            if [item["offset"] for item in base_relocs] != offsets:
                raise ValueError("base relocation offsets differ from retail PE")

            row_observations = []
            for index, (target_reloc, base_reloc) in enumerate(zip(target_relocs, base_relocs)):
                if (target_reloc["type"], target_reloc["addend"]) != (
                        base_reloc["type"], base_reloc["addend"]):
                    raise ValueError("relocation type/addend differs at %#x" % offsets[index])
                target_name = target_reloc["target"].get("name", "")
                base_name = base_reloc["target"].get("name", "")
                inventory = literal_inventory.get(target_name, [])
                retail_target = retail["cstring_targets"][index]
                inventory_matches = [
                    item for item in inventory if item["rva"] == retail_target["rva"]
                ]
                if len(inventory_matches) != 1:
                    raise ValueError(
                        "target literal %r has %d authoritative records at retail pointer RVA %#x" %
                        (target_name, len(inventory_matches), retail_target["rva"]))
                target_rva = inventory_matches[0]["rva"]
                if not base_name.startswith("$SG"):
                    raise ValueError("base literal %r is not an MSVC $SG symbol" % base_name)
                base_payload = _prove_base_literal(right, base_reloc["target"])
                if hashlib.sha256(base_payload).hexdigest() != retail_target["sha256"]:
                    raise ValueError(
                        "literal payload differs at %#x (%r -> %r)" %
                        (offsets[index], target_name, base_name))
                row_observations.append({
                    "row": row,
                    "offset": offsets[index],
                    "type": target_reloc["type"],
                    "addend": target_reloc["addend"],
                    "target_name": target_name,
                    "target_rva": target_rva,
                    "base_name": base_name,
                })

            target_pattern = _identity_pattern(
                [item["target_name"] for item in row_observations])
            rva_pattern = _identity_pattern(
                [item["target_rva"] for item in row_observations])
            base_pattern = _identity_pattern(
                [item["base_name"] for item in row_observations])
            if not target_pattern == rva_pattern == base_pattern:
                raise ValueError(
                    "literal equality partition differs between target names, retail RVAs, and base names")
            observations.extend(row_observations)
        except (KeyError, TypeError, ValueError) as error:
            _exclude(excluded, row, str(error))

    active = [item for item in observations if item["row"]["name"] not in excluded]
    target_to_base = defaultdict(set)
    base_to_target = defaultdict(set)
    for item in active:
        target_to_base[item["target_name"]].add(item["base_name"])
        base_to_target[item["base_name"]].add(item["target_name"])
    ambiguous_targets = {name for name, values in target_to_base.items() if len(values) != 1}
    ambiguous_bases = {name for name, values in base_to_target.items() if len(values) != 1}
    for item in active:
        if item["target_name"] in ambiguous_targets or item["base_name"] in ambiguous_bases:
            _exclude(
                excluded,
                item["row"],
                "literal mapping is not globally bijective (%r -> %r)" %
                (item["target_name"], item["base_name"]),
            )

    active = [item for item in observations if item["row"]["name"] not in excluded]
    mappings = {}
    target_partitions = defaultdict(set)
    base_partitions = defaultdict(set)
    for item in active:
        mappings[item["target_name"]] = item["base_name"]
        signature = (item["row"]["name"], item["offset"], item["type"], item["addend"])
        target_partitions[item["target_name"]].add(signature)
        base_partitions[item["base_name"]].add(signature)
    for target_name, base_name in mappings.items():
        if target_partitions[target_name] != base_partitions[base_name]:
            raise AssertionError("internal literal occurrence partition mismatch")

    accepted_names = {item["row"]["name"] for item in active}
    for row in rows:
        if row["audit"] == "cstring-pointer-table" and row["name"] in accepted_names:
            allocations.append({
                "target_name": row["name"],
                "extent": row["size"],
                "section_kind": "SECTION_DATA",
            })
    allocations.sort(key=lambda item: item["target_name"])
    return {
        "version": 1,
        "symbol_mappings": dict(sorted(mappings.items())),
        "section_mappings": {},
        "allocations": allocations,
    }, sorted(excluded.values(), key=lambda item: item["name"])


def load_rows(ledger=LEDGER, symbols=SYMBOLS):
    retail_rvas = {}
    with Path(symbols).open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "data" and row.get("provenance") == "cv-public-data":
                retail_rvas.setdefault(row["name"], []).append(int(row["rva"], 0))
    rows = load_required_initialized_storage(ledger)
    for row in rows:
        rvas = retail_rvas.get(row["name"], [])
        if len(rvas) != 1:
            raise RuntimeError("reviewed allocation %r has %d public RVAs" %
                               (row["name"], len(rvas)))
        row["retail_rva"] = rvas[0]
    return rows


def _run(command, cwd=REPO):
    return subprocess.run(command, cwd=cwd, text=True, capture_output=True)


def audit_units(units=None, output=OUTPUT, runner=_run, keep_diffs=False):
    ensure_reviewed_targets()
    dry_run = runner(["ninja", "-n"])
    if dry_run.returncode:
        raise RuntimeError("cannot check candidate freshness: %s" %
                           (dry_run.stderr.strip() or dry_run.stdout.strip()))
    if "no work to do" not in dry_run.stdout:
        raise RuntimeError("candidate objects are stale; run `homm2 build` before auditing")
    rows = load_rows()
    selected = set(units or {row["unit"] for row in rows})
    known = {row["unit"] for row in rows}
    unknown = sorted(selected - known)
    if unknown:
        raise RuntimeError("units are absent from the reviewed ledger: %s" % ", ".join(unknown))
    rows = [row for row in rows if row["unit"] in selected]
    by_unit = defaultdict(list)
    for row in rows:
        by_unit[row["unit"]].append(row)

    objdiff = shutil.which("objdiff-cli")
    if not objdiff:
        raise RuntimeError("objdiff-cli is required; run inside the repository Nix shell")
    output = Path(output)
    output.mkdir(parents=True, exist_ok=True)
    literal_inventory = load_literal_inventory()
    report = {
        "version": 1,
        "ledger": str(LEDGER.relative_to(REPO)),
        "units": [],
        "excluded": [],
    }
    started = time.monotonic()
    for unit in sorted(by_unit):
        stem = unit.replace("/", "__")
        diff_path = output / (stem + ".diff.json")
        manifest_path = output / (stem + ".manifest.json")
        run = runner([
            objdiff, "diff", "-p", str(OBJDIFF_PROJECT), "-u", unit,
            "-o", str(diff_path), "--format", "json",
        ])
        if run.returncode:
            raise RuntimeError("objdiff failed for %s: %s" % (unit, run.stderr.strip()))
        diff = json.loads(diff_path.read_text(encoding="utf-8"))
        manifest, excluded = derive_manifest(
            unit, by_unit[unit], diff, literal_inventory)
        manifest_path.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n")
        check = runner([sys.executable, str(GENERIC_CHECKER),
                        str(diff_path), str(manifest_path)])
        report["units"].append({
            "unit": unit,
            "ledger_rows": len(by_unit[unit]),
            "audited_rows": len(manifest["allocations"]),
            "excluded_rows": len(excluded),
            "mapping_count": len(manifest["symbol_mappings"]),
            "status": "passed" if check.returncode == 0 else "failed",
            "errors": [line for line in check.stderr.splitlines() if line],
        })
        report["excluded"].extend(excluded)
        if check.returncode == 0 and not keep_diffs:
            diff_path.unlink()
    report["runtime_seconds"] = round(time.monotonic() - started, 3)
    report["ledger_rows"] = len(rows)
    report["audited_rows"] = sum(row["audited_rows"] for row in report["units"])
    report["excluded_rows"] = len(report["excluded"])
    report["passed"] = all(row["status"] == "passed" for row in report["units"])
    (output / "report.json").write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--unit", action="append", help="audit one ledger unit (repeatable)")
    parser.add_argument("--output", type=Path, default=OUTPUT,
                        help="generated manifest, report, and optional diff directory")
    parser.add_argument("--keep-diffs", action="store_true",
                        help="retain large intermediate objdiff JSON files")
    args = parser.parse_args(argv)
    try:
        report = audit_units(args.unit, args.output, keep_diffs=args.keep_diffs)
    except (OSError, RuntimeError, ValueError) as error:
        print("strict allocations: %s" % error, file=sys.stderr)
        return 1
    for unit in report["units"]:
        print("strict allocations: %s %d/%d audited, %d mappings, %s" %
              (unit["unit"], unit["audited_rows"], unit["ledger_rows"],
               unit["mapping_count"], unit["status"]))
        for error in unit["errors"]:
            print("  %s" % error, file=sys.stderr)
    for row in report["excluded"]:
        print("strict allocations: EXCLUDED %s (%s): %s" %
              (row["name"], row["unit"], row["reason"]))
    print("strict allocations: %d/%d audited; %d excluded; %.3fs" %
          (report["audited_rows"], report["ledger_rows"],
           report["excluded_rows"], report["runtime_seconds"]))
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
