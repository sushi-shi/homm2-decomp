#!/usr/bin/env python3
"""Track linked-image contribution drift in retail address order."""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import os
import struct
import sys
import tempfile
from collections import Counter
from pathlib import Path

from homm2.build.link_exe import parse_map_symbol_records


REPO = Path(os.environ.get("HOMM2_DIR", Path(__file__).resolve().parents[3]))
REPORT = REPO / "build/link/HEROES2W.link.json"
MAP = REPO / "build/link/HEROES2W.map"
SYMBOLS = REPO / "build/gen/symbol_names.csv"
OBJECTS = REPO / "build/objdiff/base"
RETAIL_EXE = REPO / "build/orig/HEROES2W.EXE"
STATE = REPO / "config/link_layout_audit.json"
STATE_VERSION = 1
OUTCOMES = frozenset(("open", "exact", "blocked"))
RECORD_FIELDS = frozenset((
    "section", "region_kind", "unit", "retail_start_rva", "retail_size",
    "alignment", "anchor", "candidate_object_sha256", "candidate_section_sha256",
    "retail_evidence_sha256", "linker_sha256", "link_config_sha256", "outcome",
    "note",
))


class LayoutAuditError(RuntimeError):
    pass


def _parse_int(value, field):
    if isinstance(value, bool):
        raise LayoutAuditError("%s must be an integer" % field)
    try:
        return int(value, 0) if isinstance(value, str) else int(value)
    except (TypeError, ValueError) as error:
        raise LayoutAuditError("invalid %s: %r" % (field, value)) from error


def _sha256_bytes(payload):
    return hashlib.sha256(payload).hexdigest()


def _sha256_file(path):
    return _sha256_bytes(Path(path).read_bytes())


def _json_sha256(value):
    return _sha256_bytes(json.dumps(
        value, sort_keys=True, separators=(",", ":")).encode("utf-8"))


def _coff_section_payload(path, section_name):
    data = Path(path).read_bytes()
    if len(data) < 20:
        raise LayoutAuditError("truncated COFF object: %s" % path)
    section_count = struct.unpack_from("<H", data, 2)[0]
    optional_size = struct.unpack_from("<H", data, 16)[0]
    section_table = 20 + optional_size
    if section_table + section_count * 40 > len(data):
        raise LayoutAuditError("truncated COFF section table: %s" % path)
    for index in range(section_count):
        offset = section_table + index * 40
        name = data[offset:offset + 8].split(b"\0", 1)[0].decode("ascii", "replace")
        if name != section_name:
            continue
        raw_size, raw_offset = struct.unpack_from("<II", data, offset + 16)
        if raw_offset + raw_size > len(data):
            raise LayoutAuditError("truncated %s payload in %s" % (section_name, path))
        return data[raw_offset:raw_offset + raw_size]
    raise LayoutAuditError("COFF object %s has no %s section" % (path, section_name))


def _pe_rva_payload(path, rva, size):
    data = Path(path).read_bytes()
    if len(data) < 0x40 or data[:2] != b"MZ":
        raise LayoutAuditError("not a PE file: %s" % path)
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise LayoutAuditError("missing PE signature: %s" % path)
    coff = pe_offset + 4
    section_count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    section_table = coff + 20 + optional_size
    for index in range(section_count):
        offset = section_table + index * 40
        virtual_size, section_rva, raw_size, raw_offset = struct.unpack_from(
            "<IIII", data, offset + 8)
        backed_size = min(raw_size, virtual_size or raw_size)
        if section_rva <= rva and rva + size <= section_rva + backed_size:
            start = raw_offset + rva - section_rva
            return data[start:start + size]
    raise LayoutAuditError(
        "RVA range 0x%x..0x%x is not raw-backed in %s" % (rva, rva + size, path))


def _load_report(path=REPORT):
    try:
        report = json.loads(Path(path).read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise LayoutAuditError("cannot read link report: %s" % error) from error
    if not isinstance(report, dict) or not isinstance(report.get("units"), list):
        raise LayoutAuditError("link report has no units list")
    if report.get("status") != "linked":
        raise LayoutAuditError("link report is not from a successful linked image")
    return report


def _load_symbols(path=SYMBOLS):
    try:
        with Path(path).open(newline="", encoding="latin-1") as stream:
            rows = list(csv.DictReader(stream))
    except OSError as error:
        raise LayoutAuditError("cannot read symbol inventory: %s" % error) from error
    required = {"rva", "name", "unit", "kind", "provenance"}
    if not rows or not required.issubset(rows[0]):
        raise LayoutAuditError("symbol inventory lacks required provenance columns")
    return rows


def _unit_object_path(unit, object_root=OBJECTS):
    return Path(object_root) / (unit + ".obj")


def _owner_stem(value):
    return Path(value.replace("\\", "/")).stem.lower()


def _anchor_status(row, symbols, map_records, image_base):
    anchor = row.get("rva_anchor")
    if not isinstance(anchor, str) or not anchor:
        return "unavailable", "unit has no candidate function anchor"

    public_rows = [
        symbol for symbol in symbols
        if symbol.get("name") == anchor and symbol.get("kind") == "func" and
        symbol.get("provenance", "").startswith("cv-public")
    ]
    if len(public_rows) != 1:
        return "invalid", "anchor is not one unique retained public function"
    public = public_rows[0]
    if public.get("unit") != row.get("unit"):
        return "invalid", "retail public anchor belongs to %s" % public.get("unit")
    if _parse_int(public.get("rva"), "symbol RVA") != _parse_int(
            row.get("retail_rva"), "retail_rva"):
        return "invalid", "retail public anchor RVA disagrees with link report"

    candidate_rows = [record for record in map_records if record["name"] == anchor]
    if len(candidate_rows) != 1:
        return "invalid", "candidate MAP contains %d spellings of anchor" % len(candidate_rows)
    candidate = candidate_rows[0]
    if _owner_stem(candidate.get("object") or "") != Path(row["unit"]).name.lower():
        return "invalid", "candidate MAP anchor belongs to %s" % candidate.get("object")
    candidate_rva = candidate["va"] - image_base
    reported_rva = row.get("candidate_rva")
    if reported_rva is None or candidate_rva != _parse_int(reported_rva, "candidate_rva"):
        return "invalid", "candidate MAP anchor RVA disagrees with link report"
    return "valid", "unique retained public and unique candidate MAP definition"


def link_config_sha256(report):
    """Hash stable linker/order inputs without embedding worktree paths."""
    link_order = report.get("link_input_order", {})
    identity = {
        "linker_sha256": report.get("linker", {}).get("sha256"),
        "flags": report.get("link_flags"),
        "order_source": report.get("order_source"),
        "unit_order": [row.get("unit") for row in report.get("units", [])],
        "system_before": link_order.get("system_libraries_before_vendor"),
        "system_after": link_order.get("system_libraries_after_vendor"),
        "vendor_inputs": [Path(value).name for value in
                          link_order.get("vendor_import_libraries", [])],
        "section_alignment": report.get("candidate", {}).get("section_alignment"),
        "file_alignment": report.get("candidate", {}).get("file_alignment"),
    }
    return _json_sha256(identity)


def build_queue(report, symbols, map_records):
    """Classify .text contribution rows and attribute cumulative drift."""
    image_base = _parse_int(report.get("candidate", {}).get("image_base"), "image_base")
    ordered = sorted(report["units"], key=lambda row: (
        _parse_int(row.get("contribution_rva"), "contribution_rva"), row.get("unit", "")))
    seen_units = set()
    rows = []
    active_blocker = None
    for raw in ordered:
        unit = raw.get("unit")
        if not isinstance(unit, str) or not unit or unit in seen_units:
            raise LayoutAuditError("link report contains an invalid/duplicate unit")
        seen_units.add(unit)
        anchor_status, anchor_note = _anchor_status(raw, symbols, map_records, image_base)
        retail_start = _parse_int(raw.get("contribution_rva"), "contribution_rva")
        retail_size = _parse_int(raw.get("contribution_size"), "contribution_size")
        raw_size = _parse_int(raw.get("candidate_text_raw_size"), "candidate_text_raw_size")
        aligned_size = _parse_int(
            raw.get("candidate_text_aligned_size"), "candidate_text_aligned_size")
        alignment = _parse_int(raw.get("candidate_text_alignment"), "alignment")
        local_delta = aligned_size - retail_size
        candidate_start = (None if anchor_status != "valid" else
                           _parse_int(raw.get("candidate_rva"), "candidate_rva"))
        upstream_delta = None if candidate_start is None else candidate_start - retail_start

        # A proven zero start delta closes any earlier cumulative chain before this unit.
        if anchor_status == "valid" and upstream_delta == 0:
            active_blocker = None
        if anchor_status != "valid":
            kind = "invalid-anchor"
            blocker = active_blocker or unit
        elif local_delta:
            kind = "root"
            blocker = active_blocker or unit
            active_blocker = blocker
        elif upstream_delta:
            kind = "downstream"
            blocker = active_blocker or "unattributed-upstream"
        else:
            kind = "exact"
            blocker = None
        rows.append({
            "section": ".text",
            "retail_start": retail_start,
            "candidate_start": candidate_start,
            "upstream_delta": upstream_delta,
            "unit": unit,
            "retail_size": retail_size,
            "candidate_raw_size": raw_size,
            "candidate_aligned_size": aligned_size,
            "alignment": alignment,
            "local_delta": local_delta,
            "kind": kind,
            "blocker": blocker,
            "actionable": kind == "root" and blocker == unit,
            "anchor": raw.get("rva_anchor"),
            "anchor_status": anchor_status,
            "anchor_note": anchor_note,
        })
    return rows


def current_queue(report_path=REPORT, symbols_path=SYMBOLS, map_path=MAP):
    report = _load_report(report_path)
    symbols = _load_symbols(symbols_path)
    try:
        map_records = parse_map_symbol_records(map_path)
    except OSError as error:
        raise LayoutAuditError("cannot read candidate MAP: %s" % error) from error
    if not map_records:
        raise LayoutAuditError("candidate MAP has no public symbol records")
    return report, build_queue(report, symbols, map_records)


def _current_evidence(row, report, object_root=OBJECTS, retail_exe=RETAIL_EXE):
    object_path = _unit_object_path(row["unit"], object_root)
    section_payload = _coff_section_payload(object_path, row["section"])
    retail_payload = _pe_rva_payload(
        retail_exe, row["retail_start"], row["retail_size"])
    return {
        "section": row["section"],
        "region_kind": "tu-contribution",
        "unit": row["unit"],
        "retail_start_rva": "0x%x" % row["retail_start"],
        "retail_size": "0x%x" % row["retail_size"],
        "alignment": "0x%x" % row["alignment"],
        "anchor": row["anchor"],
        "candidate_object_sha256": _sha256_file(object_path),
        "candidate_section_sha256": _sha256_bytes(section_payload),
        "retail_evidence_sha256": _sha256_bytes(retail_payload),
        "linker_sha256": report.get("linker", {}).get("sha256"),
        "link_config_sha256": link_config_sha256(report),
    }


def _validate_record(record):
    if not isinstance(record, dict) or set(record) != RECORD_FIELDS:
        raise LayoutAuditError("layout record fields must be exactly: %s" %
                               ", ".join(sorted(RECORD_FIELDS)))
    for field in RECORD_FIELDS - {"outcome"}:
        if not isinstance(record[field], str) or not record[field].strip():
            raise LayoutAuditError("layout record %s must be a non-empty string" % field)
    if record["section"] != ".text" or record["region_kind"] != "tu-contribution":
        raise LayoutAuditError("only .text TU contribution records are currently supported")
    for field in ("retail_start_rva", "retail_size", "alignment"):
        _parse_int(record[field], field)
    for field in ("candidate_object_sha256", "candidate_section_sha256",
                  "retail_evidence_sha256", "linker_sha256", "link_config_sha256"):
        if len(record[field]) != 64 or any(char not in "0123456789abcdef"
                                           for char in record[field]):
            raise LayoutAuditError("layout record %s must be lowercase SHA-256" % field)
    if record["outcome"] not in OUTCOMES:
        raise LayoutAuditError("layout outcome must be open, exact, or blocked")


def load_state(path=STATE):
    path = Path(path)
    if not path.exists():
        return {"version": STATE_VERSION, "records": []}
    try:
        state = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise LayoutAuditError("cannot read layout state: %s" % error) from error
    if not isinstance(state, dict) or set(state) != {"version", "records"}:
        raise LayoutAuditError("layout state must contain only version and records")
    if state["version"] != STATE_VERSION or not isinstance(state["records"], list):
        raise LayoutAuditError("unsupported layout state version")
    seen = set()
    for record in state["records"]:
        _validate_record(record)
        key = record["section"], record["unit"]
        if key in seen:
            raise LayoutAuditError("duplicate layout record for %s %s" % key)
        seen.add(key)
    return state


def write_state(state, path=STATE):
    path = Path(path)
    payload = json.dumps(state, indent=2, sort_keys=True) + "\n"
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(
            mode="w", encoding="utf-8", dir=path.parent, delete=False) as stream:
        stream.write(payload)
        temp_path = Path(stream.name)
    temp_path.replace(path)


def classify_records(state, rows, report, object_root=OBJECTS, retail_exe=RETAIL_EXE):
    by_unit = {row["unit"]: row for row in rows}
    classified = []
    for record in state["records"]:
        row = by_unit.get(record["unit"])
        if row is None:
            raise LayoutAuditError("layout record names unknown unit %s" % record["unit"])
        evidence = _current_evidence(row, report, object_root, retail_exe)
        stale_fields = sorted(field for field in evidence if record[field] != evidence[field])
        classified.append({"record": record, "row": row, "stale_fields": stale_fields})
    return classified


def record_outcome(unit, outcome, note, state, rows, report, object_root=OBJECTS,
                   retail_exe=RETAIL_EXE):
    if outcome not in OUTCOMES:
        raise LayoutAuditError("layout outcome must be open, exact, or blocked")
    row = next((candidate for candidate in rows if candidate["unit"] == unit), None)
    if row is None:
        raise LayoutAuditError("unknown linked unit: %s" % unit)
    if outcome == "exact" and (row["kind"] != "exact" or row["anchor_status"] != "valid"):
        raise LayoutAuditError("exact outcome requires exact local/start layout and a valid anchor")
    record = _current_evidence(row, report, object_root, retail_exe)
    record.update({"outcome": outcome, "note": note})
    _validate_record(record)
    retained = [old for old in state["records"]
                if (old["section"], old["unit"]) != (record["section"], unit)]
    retained.append(record)
    retained.sort(key=lambda value: (_parse_int(value["retail_start_rva"], "RVA"),
                                     value["section"], value["unit"]))
    return {"version": STATE_VERSION, "records": retained}


def _signed_hex(value):
    if value is None:
        return "-"
    return ("+0x%x" % value) if value >= 0 else ("-0x%x" % -value)


def _print_queue(rows, state, output, limit=0, include_exact=False):
    records = {(record["section"], record["unit"]): record for record in state["records"]}
    selected = [row for row in rows if include_exact or row["kind"] != "exact"]
    if limit:
        selected = selected[:limit]
    writer = csv.writer(output, delimiter="\t", lineterminator="\n")
    writer.writerow(("rank", "section", "retail_start", "candidate_start",
                     "upstream_delta", "unit", "retail_size", "candidate_raw",
                     "candidate_aligned", "local_delta", "kind", "blocker",
                     "anchor_status", "record_outcome"))
    for rank, row in enumerate(selected, 1):
        record = records.get((row["section"], row["unit"]))
        writer.writerow((
            rank, row["section"], "0x%x" % row["retail_start"],
            "0x%x" % row["candidate_start"] if row["candidate_start"] is not None else "-",
            _signed_hex(row["upstream_delta"]), row["unit"],
            "0x%x" % row["retail_size"], "0x%x" % row["candidate_raw_size"],
            "0x%x" % row["candidate_aligned_size"], _signed_hex(row["local_delta"]),
            row["kind"], row["blocker"] or "-", row["anchor_status"],
            record["outcome"] if record else "-",
        ))
    return len(selected)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--report", type=Path, default=REPORT)
    parser.add_argument("--map", dest="map_path", type=Path, default=MAP)
    parser.add_argument("--symbols", type=Path, default=SYMBOLS)
    parser.add_argument("--state", type=Path, default=STATE)
    commands = parser.add_subparsers(dest="command", required=True)
    queue = commands.add_parser("queue", help="write the RVA-ordered contribution queue")
    queue.add_argument("--limit", type=int, default=0)
    queue.add_argument("--all", action="store_true", help="include exact contribution rows")
    queue.add_argument("--output", type=Path, help="write TSV here instead of stdout")
    commands.add_parser("summary", help="summarize current roots, blockers, and records")
    explain = commands.add_parser("explain", help="explain one linked contribution")
    explain.add_argument("unit")
    record = commands.add_parser("record", help="record current evidence for one contribution")
    record.add_argument("--unit", required=True)
    record.add_argument("--outcome", choices=sorted(OUTCOMES), required=True)
    record.add_argument("--note", required=True)
    args = parser.parse_args(argv)

    try:
        if getattr(args, "limit", 0) < 0:
            raise LayoutAuditError("--limit must be zero or positive")
        report, rows = current_queue(args.report, args.symbols, args.map_path)
        state = load_state(args.state)
        classified = classify_records(state, rows, report)
        if args.command == "queue":
            if args.output:
                args.output.parent.mkdir(parents=True, exist_ok=True)
                with args.output.open("w", newline="", encoding="utf-8") as stream:
                    count = _print_queue(rows, state, stream, args.limit, args.all)
                print("link layout: wrote %d row(s) to %s" % (count, args.output),
                      file=sys.stderr)
            else:
                _print_queue(rows, state, sys.stdout, args.limit, args.all)
            return 0
        if args.command == "record":
            updated = record_outcome(args.unit, args.outcome, args.note, state, rows, report)
            write_state(updated, args.state)
            print("link layout: recorded %s as %s" % (args.unit, args.outcome))
            return 0
        if args.command == "explain":
            row = next((value for value in rows if value["unit"] == args.unit), None)
            if row is None:
                raise LayoutAuditError("unknown linked unit: %s" % args.unit)
            record = next((value for value in classified
                           if value["record"]["unit"] == args.unit), None)
            for key in ("section", "unit", "kind", "actionable", "blocker", "anchor",
                        "anchor_status", "anchor_note"):
                print("%s: %s" % (key, row[key]))
            for key in ("retail_start", "candidate_start", "upstream_delta", "retail_size",
                        "candidate_raw_size", "candidate_aligned_size", "local_delta"):
                value = row[key]
                print("%s: %s" % (key, "-" if value is None else _signed_hex(value)))
            if record:
                print("record_outcome: %s" % record["record"]["outcome"])
                print("record_status: %s" % ("stale: " + ", ".join(record["stale_fields"])
                                              if record["stale_fields"] else "current"))
                print("record_note: %s" % record["record"]["note"])
            else:
                print("record_status: unrecorded")
            return 0

        counts = Counter(row["kind"] for row in rows)
        actionable_count = sum(row["actionable"] for row in rows)
        actionable = next((row for row in rows if row["actionable"]), None)
        stale = sum(bool(value["stale_fields"]) for value in classified)
        print("link layout: %d units; exact=%d roots=%d actionable=%d downstream=%d "
              "invalid-anchor=%d" % (
            len(rows), counts["exact"], counts["root"], actionable_count,
            counts["downstream"], counts["invalid-anchor"]))
        print("link layout records: %d current, %d stale" %
              (len(classified) - stale, stale))
        if actionable:
            print("earliest actionable: %s %s local %s at retail 0x%x" % (
                actionable["section"], actionable["unit"],
                _signed_hex(actionable["local_delta"]), actionable["retail_start"]))
        else:
            print("earliest actionable: none")
        return 0
    except (LayoutAuditError, OSError, ValueError) as error:
        print("link layout: %s" % error, file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
