"""Classify the (unmatched) module's functions by masked-byte identity.

Two identity sources:

  A. our own base objects (build/objdiff/base/**.obj) - game statics, missed
     claims, and compiler-generated bodies, whether their names are stable
     (``??_G`` deleting destructors) or volatile (``$E`` thunks). A hit here
     is the "game build side" symbol a future RA_COMPGEN_SYM claim needs.
  B. the toolchain's static runtime archives (LIBCMT.LIB and friends) - the
     CRT bodies the image links.

Image-side bytes come from build/delink/(unmatched).c.obj with the delinker's
recovered relocation sites zeroed; candidate bytes are zeroed at their own
relocation sites. Equal size + equal masked bytes is CANDIDATE EVIDENCE, not
proof - identical small bodies collide, and the report keeps every colliding
name visible rather than picking one.

Functions link in definition order, so after exact matching the census walks
each source object's definition order against the image addresses: maximal
order-consistent runs are reported, and an unmatched image span bracketed by
two consecutive matches from one source object yields "order-inferred"
candidates for the functions between them (the DNA pass).

    homm2 audit unmatched-census        # -> build/gen/unmatched_census.tsv
"""
from __future__ import annotations

import csv
import hashlib
import struct
from collections import defaultdict
from pathlib import Path

from homm2.build.canonicalize_relocs import CoffFile
from homm2.core.paths import REPO


INVENTORY = REPO / "config/retail_functions.csv"
SYMBOLS = REPO / "build/gen/symbol_names.csv"
UNMATCHED_OBJ = REPO / "build/delink/(unmatched).c.obj"
BASE_ROOT = REPO / "build/objdiff/base"
RUNTIME_LIBS = ("LIBCMT.LIB", "LIBCPMT.LIB", "OLDNAMES.LIB")
OUTPUT = REPO / "build/gen/unmatched_census.tsv"

IMAGE_BASE = 0x400000
FUNCTION_TYPE = 0x0020
MEM_EXECUTE = 0x20000000
HEADER = ("rva", "size", "name", "class", "source", "symbol", "detail")


def trim_padding(payload: bytes) -> bytes:
    """Drop trailing alignment fill so carved and compiled spans compare."""
    end = len(payload)
    while end > 1 and payload[end - 1] in (0x90, 0xCC):
        end -= 1
    return payload[:end]


def mask_bytes(payload: bytes, sites, width: int = 4) -> bytes:
    """Zero every relocation field so linked/unlinked copies compare equal."""
    data = bytearray(payload)
    for site in sites:
        end = min(site + width, len(data))
        if site < len(data):
            data[site:end] = bytes(end - max(site, 0))
    return bytes(data)


def function_spans(symbols, section_size):
    """(symbol, start, end) spans in definition order for one section."""
    ordered = sorted(symbols, key=lambda symbol: symbol.value)
    spans = []
    for index, symbol in enumerate(ordered):
        end = (ordered[index + 1].value if index + 1 < len(ordered)
               else section_size)
        if end > symbol.value:
            spans.append((symbol, symbol.value, end))
    return spans


def order_runs(matches):
    """Maximal runs of (source_order, image_rva) increasing in both."""
    ordered = sorted(matches)
    runs = []
    current = []
    for entry in ordered:
        if current and entry[1] <= current[-1][1]:
            runs.append(current)
            current = []
        current.append(entry)
    if current:
        runs.append(current)
    return runs


def _load_inventory():
    sizes = {}
    with INVENTORY.open(newline="") as stream:
        for row in csv.DictReader(
                line for line in stream if not line.lstrip().startswith("#")):
            sizes[int(row["entry_rva"], 16)] = int(row["byte_size"])
    return sizes


def _load_claimed():
    claimed = set()
    with SYMBOLS.open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            if row.get("kind") == "func":
                claimed.add(int(row["rva"], 0))
    return claimed


def _unmatched_functions():
    """(rva, size, name, masked_bytes) for every function in (unmatched).c.obj.

    The delinker packs functions in ascending retail order, so the symbol list
    sorted by section offset aligns 1:1 with the unclaimed inventory rows
    sorted by RVA (verified: every FUN_<va> name agrees). That mapping also
    covers the rows whose names Ghidra recognised (Ordinal thunks, templates).
    """
    coff = CoffFile(UNMATCHED_OBJ)
    sizes = _load_inventory()
    claimed = _load_claimed()
    unclaimed = sorted(rva for rva in sizes if rva not in claimed)
    text = next(section for section in coff.sections if section.name == ".text")
    symbols = sorted(
        (symbol.value, symbol.name) for symbol in coff.symbols.values()
        if symbol.section == text.index)
    if len(symbols) != len(unclaimed):
        raise RuntimeError(
            "(unmatched).c.obj carries %d functions but the inventory has %d "
            "unclaimed rows; redelink first" % (len(symbols), len(unclaimed)))
    for (value, name), rva in zip(symbols, unclaimed):
        if name.startswith("FUN_") and int(name[4:], 16) - IMAGE_BASE != rva:
            raise RuntimeError(
                "(unmatched).c.obj order diverges from the inventory at %s" % name)
    sites_by_offset = sorted(
        site for (section_index, site), _reloc in coff.relocations.items()
        if section_index == text.index)
    payload = bytes(coff.data[text.raw_offset:text.raw_offset + text.raw_size])
    rows = []
    for (value, name), rva in zip(symbols, unclaimed):
        end = min(value + sizes[rva], len(payload))
        local_sites = [site - value for site in sites_by_offset
                       if value <= site < end]
        rows.append((rva, end - value, name,
                     trim_padding(mask_bytes(payload[value:end], local_sites))))
    return sorted(rows)


def _coff_candidates(payload: bytes, source: str):
    """(source, symbol_name, order, masked_bytes) per function in one COFF."""
    try:
        coff = CoffFile_from_bytes(payload)
    except (ValueError, IndexError, struct.error):
        return []
    rows = []
    order = 0
    first_header = 20 + struct.unpack_from("<H", coff.data, 16)[0]
    for section in coff.sections:
        characteristics = struct.unpack_from(
            "<I", coff.data, first_header + (section.index - 1) * 40 + 36)[0]
        if not characteristics & MEM_EXECUTE or section.raw_size == 0:
            continue
        symbols = [
            symbol for symbol in coff.symbols.values()
            if symbol.section == section.index
            and struct.unpack_from(
                "<H", coff.data, symbol.offset + 14)[0] == FUNCTION_TYPE]
        if not symbols:
            continue
        section_payload = bytes(
            coff.data[section.raw_offset:section.raw_offset + section.raw_size])
        sites = sorted(
            site for (section_index, site), _r in coff.relocations.items()
            if section_index == section.index)
        for symbol, start, end in function_spans(symbols, section.raw_size):
            local = [site - start for site in sites if start <= site < end]
            rows.append((source, symbol.name, order, trim_padding(
                mask_bytes(section_payload[start:end], local))))
            order += 1
    return rows


class _BytesCoff(CoffFile):
    def __init__(self, payload: bytes):
        import tempfile, os
        handle, name = tempfile.mkstemp(suffix=".obj")
        try:
            with os.fdopen(handle, "wb") as stream:
                stream.write(payload)
            super().__init__(name)
        finally:
            os.unlink(name)


def CoffFile_from_bytes(payload: bytes) -> CoffFile:
    return _BytesCoff(payload)


def _archive_members(path: Path):
    """(member_name, payload) for every object member of a COFF archive."""
    data = path.read_bytes()
    if not data.startswith(b"!<arch>\n"):
        raise ValueError("not a COFF archive: %s" % path)
    longnames = b""
    offset = 8
    while offset + 60 <= len(data):
        header = data[offset:offset + 60]
        if header[58:60] != b"`\n":
            raise ValueError("invalid archive member trailer in %s" % path)
        size = int(header[48:58].decode("ascii").strip())
        name = header[:16].decode("ascii", "replace").rstrip()
        start = offset + 60
        payload = data[start:start + size]
        if name == "//":
            longnames = payload
        elif name != "/":
            if name.startswith("/") and name[1:].isdigit():
                begin = int(name[1:])
                # MS longname entries are NUL-terminated; GNU uses "/\n".
                terminators = [i for i in (longnames.find(b"\0", begin),
                                           longnames.find(b"\n", begin))
                               if i != -1]
                end = min(terminators) if terminators else len(longnames)
                name = longnames[begin:end].decode("ascii", "replace").rstrip("/")
            else:
                name = name.rstrip("/")
            yield name, payload
        offset = start + size + (size & 1)


def _candidate_index():
    index = defaultdict(list)
    definition_orders = defaultdict(list)

    def register(rows):
        for source, symbol, order, masked in rows:
            index[(len(masked), hashlib.sha256(masked).hexdigest())].append(
                (source, symbol, order))
            definition_orders[source].append((order, symbol))

    for path in sorted(BASE_ROOT.rglob("*.obj")):
        source = "base:%s" % path.relative_to(BASE_ROOT).with_suffix("")
        register(_coff_candidates(path.read_bytes(), source))
    lib_dir = REPO / "build/toolchain/msvc/lib"
    for library in RUNTIME_LIBS:
        path = lib_dir / library
        if not path.is_file():
            continue
        for member, payload in _archive_members(path):
            register(_coff_candidates(
                payload, "%s:%s" % (library.lower(), member)))
    return index, definition_orders


def run_census():
    unmatched = _unmatched_functions()
    index, definition_orders = _candidate_index()

    rows = []
    matches_by_source = defaultdict(list)
    for rva, size, name, masked in unmatched:
        key = (len(masked), hashlib.sha256(masked).hexdigest())
        candidates = index.get(key, [])
        if not candidates:
            kind = "unknown"
            detail = ""
            if masked[:2] == b"\xff\x25" and len(masked) <= 6:
                kind = "import-thunk"
                detail = "jmp [IAT]"
            rows.append({"rva": rva, "size": size, "name": name,
                         "class": kind, "source": "", "symbol": "",
                         "detail": detail})
            continue
        sources = {source for source, _symbol, _order in candidates}
        primary = candidates[0]
        kind = ("crt-exact" if all(s.split(":")[0].endswith(".lib")
                                   for s in sources) else "game-exact")
        detail = ("" if len(candidates) == 1 else
                  "%d identical candidates: %s" % (
                      len(candidates), "|".join(
                          "%s!%s" % (s, y)
                          for s, y, _o in candidates[:4])))
        rows.append({"rva": rva, "size": size, "name": name, "class": kind,
                     "source": primary[0], "symbol": primary[1],
                     "detail": detail})
        if len(sources) == 1:
            matches_by_source[primary[0]].append((primary[2], rva))

    # DNA pass: definition order against image order.
    inferred = 0
    by_rva = {row["rva"]: row for row in rows}
    ordered_rvas = [row["rva"] for row in rows]
    for source, matches in sorted(matches_by_source.items()):
        for run in order_runs(matches):
            if len(run) < 2:
                continue
            names = dict(definition_orders[source])
            for (left_order, left_rva), (right_order, right_rva) in zip(
                    run, run[1:]):
                between_orders = list(range(left_order + 1, right_order))
                between_rvas = [rva for rva in ordered_rvas
                                if left_rva < rva < right_rva
                                and by_rva[rva]["class"] == "unknown"]
                if not between_orders or not between_rvas:
                    continue
                if len(between_orders) != len(between_rvas):
                    continue
                for order, rva in zip(between_orders, between_rvas):
                    row = by_rva[rva]
                    row["class"] = "order-inferred"
                    row["source"] = source
                    row["symbol"] = names.get(order, "?")
                    row["detail"] = ("between 0x%x and 0x%x" %
                                     (left_rva, right_rva))
                    inferred += 1
    return rows, inferred


def main(argv=None):
    rows, inferred = run_census()
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    with OUTPUT.open("w", newline="") as stream:
        stream.write(
            "# Masked-byte identity census of the (unmatched) module.\n"
            "# Candidate evidence only - identical bodies collide and order\n"
            "# inference assumes definition-order linking. Regenerate with\n"
            "# `homm2 audit unmatched-census` after a redelink.\n")
        writer = csv.DictWriter(stream, fieldnames=HEADER, delimiter="\t",
                                lineterminator="\n")
        writer.writeheader()
        for row in rows:
            row = dict(row)
            row["rva"] = "0x%x" % row["rva"]
            row["size"] = "0x%x" % row["size"]
            writer.writerow(row)

    totals = defaultdict(lambda: [0, 0])
    for row in rows:
        totals[row["class"]][0] += 1
        totals[row["class"]][1] += int(row["size"], 0) if isinstance(
            row["size"], str) else row["size"]
    print("[unmatched-census] %d functions -> %s" % (len(rows), OUTPUT))
    for kind in ("game-exact", "crt-exact", "order-inferred", "import-thunk",
                 "unknown"):
        count, size = totals.get(kind, (0, 0))
        print("[unmatched-census]   %-15s %5d functions  %7d bytes" %
              (kind, count, size))

    unknown = [row for row in rows if row["class"] == "unknown"]
    clusters = []
    current = []
    for row in unknown:
        if current and row["rva"] - (current[-1]["rva"] + current[-1]["size"]) > 0x200:
            clusters.append(current)
            current = []
        current.append(row)
    if current:
        clusters.append(current)
    clusters.sort(key=lambda cluster: -sum(row["size"] for row in cluster))
    for cluster in clusters[:8]:
        print("[unmatched-census]   unknown cluster 0x%x..0x%x: %d functions, "
              "%d bytes" % (
                  cluster[0]["rva"],
                  cluster[-1]["rva"] + cluster[-1]["size"],
                  len(cluster), sum(row["size"] for row in cluster)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
