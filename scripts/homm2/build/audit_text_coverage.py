#!/usr/bin/env python3
"""Audit exact delinker spans and every nested executable-entry candidate."""
import bisect
import csv
import os
import re
import struct
import subprocess
import sys
from pathlib import Path


PADDING = {0x00, 0x90, 0xcc}


def rows_without_comments(path):
    return csv.DictReader(line for line in open(path, newline="")
                          if not line.lstrip().startswith("#"))


def text_section(path):
    data = open(path, "rb").read()
    pe = struct.unpack_from("<I", data, 0x3c)[0]
    count = struct.unpack_from("<H", data, pe + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe + 20)[0]
    image_base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    first = pe + 24 + optional_size
    for index in range(count):
        off = first + index * 40
        name = data[off:off + 8].rstrip(b"\0")
        if name != b".text":
            continue
        virtual_size, rva, raw_size, raw = struct.unpack_from("<IIII", data, off + 8)
        return image_base, rva, data[raw:raw + min(virtual_size, raw_size)]
    raise SystemExit("PE has no .text section")


def highlow_pointer_targets(path, image_base):
    data = open(path, "rb").read()
    pe = struct.unpack_from("<I", data, 0x3c)[0]
    count = struct.unpack_from("<H", data, pe + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe + 20)[0]
    optional = pe + 24
    sections = []
    for index in range(count):
        offset = optional + optional_size + index * 40
        _virtual_size, rva, raw_size, raw = struct.unpack_from(
            "<IIII", data, offset + 8)
        sections.append((rva, raw_size, raw))

    def raw_offset(rva):
        for start, size, raw in sections:
            if start <= rva < start + size:
                return raw + rva - start
        return None

    reloc_rva, reloc_size = struct.unpack_from("<II", data, optional + 96 + 5 * 8)
    cursor = raw_offset(reloc_rva)
    if cursor is None:
        return {}
    limit = cursor + reloc_size
    targets = {}
    while cursor + 8 <= limit:
        page, block_size = struct.unpack_from("<II", data, cursor)
        if block_size < 8 or cursor + block_size > limit:
            raise ValueError("invalid PE base-relocation block")
        for offset in range(cursor + 8, cursor + block_size, 2):
            entry = struct.unpack_from("<H", data, offset)[0]
            if entry >> 12 != 3:
                continue
            site = page + (entry & 0xFFF)
            raw = raw_offset(site)
            if raw is None or raw + 4 > len(data):
                continue
            target = (struct.unpack_from("<I", data, raw)[0] - image_base) & 0xFFFFFFFF
            targets.setdefault(target, []).append(site)
        cursor += block_size
    return targets


def parse_disassembly(exe, image_base):
    output = subprocess.run(["llvm-objdump", "-d", exe], capture_output=True,
                            text=True, check=True).stdout
    instructions = {}
    calls = {}
    for line in output.splitlines():
        match = re.match(r"^\s*([0-9a-fA-F]+):\s+((?:[0-9a-fA-F]{2} )+)\s+(\S.*)$", line)
        if not match:
            continue
        rva = int(match.group(1), 16) - image_base
        raw = bytes.fromhex(match.group(2))
        asm = match.group(3)
        instructions[rva] = (raw, asm)
        target = re.search(r"\bcalll?\s+0x([0-9a-fA-F]+)", asm)
        if target:
            calls.setdefault(int(target.group(1), 16) - image_base, []).append(rva)
    return instructions, calls


def boundary_description(rva, text_rva, text, instruction_starts, instructions, exclusions):
    index = bisect.bisect_left(instruction_starts, rva) - 1
    if index < 0:
        return "no-predecessor"
    previous = instruction_starts[index]
    raw, asm = instructions[previous]
    for left, right, kind, _reason in exclusions:
        if left <= previous < right:
            if rva > text_rva and text[rva - text_rva - 1] in PADDING:
                return "padding-after-excluded-%s" % kind
            return "after-excluded-%s" % kind
    if previous + len(raw) == rva:
        mnemonic = asm.split(None, 1)[0]
        if mnemonic == "int3":
            return "padding"
        if mnemonic.startswith("ret") or mnemonic in ("jmp", "jmpl", "ud2"):
            return "terminator-0x%x:%s" % (previous, mnemonic)
        return "fallthrough-0x%x:%s" % (previous, mnemonic)
    if rva > text_rva and text[rva - text_rva - 1] in PADDING:
        return "padding"
    return "undecoded-gap-after-0x%x" % previous


def subtract_ranges(start, end, exclusions):
    pieces = [(start, end)]
    for left, right, _kind, _reason in exclusions:
        next_pieces = []
        for piece_start, piece_end in pieces:
            if right <= piece_start or piece_end <= left:
                next_pieces.append((piece_start, piece_end))
                continue
            if piece_start < left:
                next_pieces.append((piece_start, left))
            if right < piece_end:
                next_pieces.append((right, piece_end))
        pieces = next_pieces
    return pieces


def write_coverage_partition(path, text_rva, text_end, spans, gaps, exclusions,
                             jump_tables, unit_by_name):
    """Write the exact successful .text byte partition for canonical review."""
    rows = []
    table_ranges = []
    for left, right in sorted(jump_tables):
        if table_ranges and left <= table_ranges[-1][1]:
            previous = table_ranges[-1]
            table_ranges[-1] = (
                previous[0], max(previous[1], right), "jump-table",
                previous[3] + ",0x%x" % left)
        else:
            table_ranges.append((left, right, "jump-table", "0x%x" % left))
    for start, end, aliases in spans:
        names = sorted(name for name, _provenance in aliases)
        owners = sorted({unit_by_name.get(name, "-") for name in names})
        cuts = [(left, right, kind, identity)
                for left, right, kind, identity in table_ranges
                if start <= left and right <= end]
        for left, right in subtract_ranges(start, end, cuts):
            rows.append((left, right, "function", ",".join(owners),
                         ",".join(names), "retained-or-recovered-procedure"))
        for left, right, kind, identity in cuts:
            rows.append((left, right, kind, ",".join(owners), identity,
                         "build/gen/jump_tables.csv"))
    for start, end, kind, reason in exclusions:
        cuts = [(left, right, table_kind, identity)
                for left, right, table_kind, identity in table_ranges
                if start <= left and right <= end]
        for left, right in subtract_ranges(start, end, cuts):
            rows.append((left, right, kind, "-", reason,
                         "config/delink_text_exclusions.csv"))
        for left, right, table_kind, identity in cuts:
            rows.append((left, right, table_kind, "-", identity,
                         "build/gen/jump_tables.csv"))
    for start, end in gaps:
        for left, right in subtract_ranges(start, end, exclusions):
            rows.append((left, right, "padding", "-", "-", "retail-padding-bytes"))
    rows.sort()
    cursor = text_rva
    for start, end, _kind, _owner, _identity, _provenance in rows:
        if start != cursor or end <= start:
            raise RuntimeError("text coverage partition gap/overlap at 0x%x" % cursor)
        cursor = end
    if cursor != text_end:
        raise RuntimeError("text coverage partition ends at 0x%x, expected 0x%x" %
                           (cursor, text_end))
    lines = [
        "domain\towner\tstorage\tstart\tsize\tkind\tidentity\tprovenance",
    ]
    for start, end, kind, owner, identity, provenance in rows:
        lines.append("text\t%s\ttext\t0x%x\t0x%x\t%s\t%s\t%s" % (
            owner, start, end - start, kind,
            identity.replace("\t", " "), provenance.replace("\t", " ")))
    payload = ("\n".join(lines) + "\n").encode("utf-8")
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(".%s.tmp" % path.name)
    temporary.write_bytes(payload)
    os.replace(temporary, path)


def main(argv=None):
    argv = list(argv or sys.argv[1:])
    exe = argv[0] if argv else "build/orig/HEROES2W.EXE"
    manifest = argv[1] if len(argv) > 1 else "build/gen/symbol_names.csv"
    procedures = argv[2] if len(argv) > 2 else "config/delink_procedures.csv"
    ghidra_path = Path("build/ghidra/exports/functions.csv")
    rejection_path = Path("config/delink_candidate_rejections.csv")
    exclusion_path = Path("config/delink_text_exclusions.csv")
    jump_table_path = Path("build/gen/jump_tables.csv")
    output_path = Path(argv[3]) if len(argv) > 3 else None

    image_base, text_rva, text = text_section(exe)
    text_end = text_rva + len(text)
    instructions, calls = parse_disassembly(exe, image_base)
    pointer_targets = highlow_pointer_targets(exe, image_base)
    instruction_starts = sorted(instructions)

    manifest_rows = list(csv.DictReader(open(manifest, newline="")))
    intervals = []
    unit_by_name = {}
    public_by_rva = {}
    nb09_starts = set()
    for row in manifest_rows:
        if row.get("kind") != "func":
            continue
        start = int(row["rva"], 16)
        size = int(row["size"], 16)
        provenance = row.get("provenance", "")
        if size and text_rva <= start < text_end:
            intervals.append((start, start + size, row["name"], provenance))
            unit_by_name[row["name"]] = row["unit"]
        if provenance.startswith("cv-public"):
            public_by_rva.setdefault(start, (row["name"], row["unit"]))
            nb09_starts.add(start)
        elif provenance.startswith("cv-thunk"):
            nb09_starts.add(start)

    configured = {}
    for row in rows_without_comments(procedures):
        rva = int(row["rva"], 16)
        configured[rva] = (int(row["size"], 16), row["name"], row["unit"],
                           row["provenance"])
    rejected = {int(row["rva"], 16): row["reason"]
                for row in rows_without_comments(rejection_path)}
    exclusions = []
    for row in rows_without_comments(exclusion_path):
        start = int(row["rva"], 16)
        exclusions.append((start, start + int(row["size"], 16), row["kind"],
                           "%s: %s" % (row["unit"], row["reason"])))
    exclusions.sort()
    jump_tables = []
    if jump_table_path.exists():
        for row in csv.DictReader(open(jump_table_path, newline="")):
            start = int(row["table_rva"], 16)
            jump_tables.append((start, start + int(row["table_size"], 16)))

    ghidra = {}
    if ghidra_path.exists():
        for row in csv.DictReader(open(ghidra_path, newline="")):
            ghidra[int(row["entry_rva"], 16)] = (int(row["byte_size"]), row["name"])

    public_starts = sorted(public_by_rva)
    raw_candidates = set(calls)
    raw_candidates.update(ghidra)
    candidates = []
    for rva in sorted(raw_candidates):
        if rva in nb09_starts or not text_rva <= rva < text_end:
            continue
        index = bisect.bisect_right(public_starts, rva) - 1
        if index < 0:
            continue
        owner_start = public_starts[index]
        owner_end = public_starts[index + 1] if index + 1 < len(public_starts) else text_end
        if owner_start < rva < owner_end:
            candidates.append((rva, owner_start, owner_end))

    candidate_failures = []
    accepted_count = rejected_count = 0
    print("nested-entry candidates: count=%d" % len(candidates))
    for rva, owner_start, owner_end in candidates:
        owner_name, owner_unit = public_by_rva[owner_start]
        incoming = calls.get(rva, [])
        boundary = boundary_description(rva, text_rva, text, instruction_starts,
                                        instructions, exclusions)
        gh_size, gh_name = ghidra.get(rva, (0, "-"))
        if rva in configured:
            size, _name, unit, provenance = configured[rva]
            disposition = "accepted"
            accepted_count += 1
            if unit != owner_unit:
                # Contribution ownership is authoritative; a different containing public is
                # expected at TU boundaries, so the generator separately verifies the row.
                unit_display = "%s (containing public %s)" % (unit, owner_unit)
            else:
                unit_display = unit
            if "direct-rel32-entry" in provenance and not incoming:
                candidate_failures.append((rva, "configured direct entry has no decoded caller"))
        elif rva in rejected:
            disposition = "rejected: " + rejected[rva]
            rejected_count += 1
            unit_display = owner_unit
        else:
            disposition = "UNREVIEWED"
            unit_display = owner_unit
            candidate_failures.append((rva, "unreviewed nested entry candidate"))
        caller_text = ",".join("0x%x" % caller for caller in incoming) or "none"
        pointer_text = ",".join("0x%x" % site for site in pointer_targets.get(rva, [])) or "none"
        extent = "config=0x%x" % configured[rva][0] if rva in configured else "config=-"
        if gh_size:
            extent += ",ghidra=0x%x:%s" % (gh_size, gh_name)
        owned_tables = 0
        if rva in configured:
            end = rva + configured[rva][0]
            owned_tables = sum(rva <= left and right <= end for left, right in jump_tables)
        print("  0x%x in 0x%x..0x%x %s; callers=%s; pointers=%s; boundary=%s; extent=%s; "
              "unit=%s; jump_tables=%d; %s" %
              (rva, owner_start, owner_end, owner_name, caller_text, pointer_text,
               boundary, extent,
               unit_display, owned_tables, disposition))

    for rva, (size, name, _unit, provenance) in configured.items():
        end = rva + size
        if (rva not in nb09_starts and rva not in pointer_targets and
                rva not in {candidate[0] for candidate in candidates}):
            candidate_failures.append((
                rva, "%s has no direct-call, stored-pointer, or Ghidra entry evidence" % name))
        if end not in instructions and end != text_end:
            previous_index = bisect.bisect_left(instruction_starts, end) - 1
            if previous_index < 0 or instruction_starts[previous_index] + \
                    len(instructions[instruction_starts[previous_index]][0]) != end:
                candidate_failures.append((rva, "%s extent ends inside an instruction" % name))
        previous_index = bisect.bisect_left(instruction_starts, end) - 1
        if previous_index >= 0:
            previous = instruction_starts[previous_index]
            raw, asm = instructions[previous]
            mnemonic = asm.split(None, 1)[0]
            terminates = previous + len(raw) == end and \
                (mnemonic.startswith("ret") or mnemonic in ("jmp", "jmpl", "ud2"))
            shared_tail = end in configured and "direct-rel32-entry" in provenance
            if ("disassembly-ret" in provenance or "static-init" in provenance or
                    "callback-disassembly" in provenance) and not terminates:
                candidate_failures.append((rva, "%s extent lacks a terminal return/jump" % name))
            elif "direct-rel32-entry" in provenance and not terminates and not shared_tail:
                candidate_failures.append((rva, "%s direct-entry extent lacks a terminal boundary" % name))

    for rva in rejected:
        if rva not in {candidate[0] for candidate in candidates}:
            candidate_failures.append((rva, "stale rejected-candidate entry"))

    unique = {}
    for start, end, name, provenance in intervals:
        unique.setdefault((start, end), []).append((name, provenance))
    spans = sorted((start, end, aliases) for (start, end), aliases in unique.items())
    overlap = []
    covered = []
    for start, end, aliases in spans:
        if covered and start < covered[-1][1]:
            overlap.append((covered[-1][0], covered[-1][1], start, end, aliases[0][0]))
            covered[-1] = (covered[-1][0], max(covered[-1][1], end))
        elif covered and start == covered[-1][1]:
            covered[-1] = (covered[-1][0], end)
        else:
            covered.append((start, end))

    gaps = []
    cursor = text_rva
    for start, end in covered:
        if start > cursor:
            gaps.append((cursor, start))
        cursor = max(cursor, end)
    if cursor < text_end:
        gaps.append((cursor, text_end))

    unexplained = []
    padding_count = 0
    for start, end in gaps:
        for piece_start, piece_end in subtract_ranges(start, end, exclusions):
            body = text[piece_start - text_rva:piece_end - text_rva]
            if set(body) <= PADDING:
                padding_count += 1
            else:
                unexplained.append((piece_start, piece_end, len(body)))

    exclusion_failures = []
    for index, (start, end, kind, reason) in enumerate(exclusions):
        if not text_rva <= start < end <= text_end:
            exclusion_failures.append((start, "outside .text"))
        if index and start < exclusions[index - 1][1]:
            exclusion_failures.append((start, "overlaps previous exclusion"))
        if any(left < end and start < right for left, right, _name, _proof in intervals):
            exclusion_failures.append((start, "overlaps a procedure span"))
        if any(start <= rva < end for rva, _owner_start, _owner_end in candidates):
            exclusion_failures.append((start, "contains an executable-entry candidate"))
        body = text[start - text_rva:end - text_rva]
        if not body or set(body) <= PADDING:
            exclusion_failures.append((start, "contains padding only"))
        contained_tables = sum(start <= left and right <= end for left, right in jump_tables)
        if jump_tables and kind == "embedded-data" and not contained_tables:
            exclusion_failures.append((start, "embedded-data exclusion has no detected jump table"))
        print("  EXCLUDED 0x%x..0x%x %s jump_tables=%d (%s)" %
              (start, end, kind, contained_tables, reason))

    print("coverage: functions=%d unique_spans=%d padding_gaps=%d unexplained_gaps=%d "
          "overlaps=%d candidates=%d accepted=%d rejected=%d exclusions=%d jump_tables=%d "
          "failures=%d" %
          (len(intervals), len(spans), padding_count, len(unexplained), len(overlap),
           len(candidates), accepted_count, rejected_count, len(exclusions), len(jump_tables),
           len(candidate_failures) + len(exclusion_failures)))
    for start, end, size in unexplained[:40]:
        print("  UNEXPLAINED 0x%x..0x%x (%d bytes)" % (start, end, size))
    for left_start, left_end, start, end, name in overlap[:40]:
        print("  OVERLAP 0x%x..0x%x with 0x%x..0x%x (%s)" %
              (left_start, left_end, start, end, name))
    for rva, problem in candidate_failures + exclusion_failures:
        print("  EVIDENCE 0x%x: %s" % (rva, problem))
    failed = bool(unexplained or overlap or candidate_failures or exclusion_failures)
    if not failed and output_path is not None:
        write_coverage_partition(output_path, text_rva, text_end, spans, gaps,
                                 exclusions, jump_tables, unit_by_name)
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
