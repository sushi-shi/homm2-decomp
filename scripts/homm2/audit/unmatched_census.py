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
DELINK_ROOT = REPO / "build/delink"
RETAIL_EXE = REPO / "build/orig/HMM2PL.exe"
RUNTIME_LIBS = ("LIBCMT.LIB", "LIBCPMT.LIB", "OLDNAMES.LIB")
OUTPUT = REPO / "build/gen/unmatched_census.tsv"
CRT_OUTPUT = REPO / "config/crt_functions.csv"
THUNK_OUTPUT = REPO / "config/import_thunks.csv"
COMPGEN_OUTPUT = REPO / "config/compgen_functions.csv"
FUNCLET_OUTPUT = REPO / "config/eh_funclets.csv"

IMAGE_BASE = 0x400000
FUNCTION_TYPE = 0x0020
MEM_EXECUTE = 0x20000000
HEADER = ("rva", "size", "name", "class", "source", "symbol", "callers",
          "callees", "detail")


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


def prefer_candidate(candidates, ghidra_name):
    """Prefer the collision candidate that agrees with Ghidra's own name."""
    if ghidra_name and not ghidra_name.startswith(("FUN_", "Unwind@")):
        wanted = ghidra_name.lstrip("_").lower()
        special = {"operator_new": "??2@", "operator_delete": "??3@"}
        for candidate in candidates:
            symbol = candidate[1]
            if symbol.lstrip("_").lower() == wanted:
                return candidate
            prefix = special.get(ghidra_name)
            if prefix and symbol.startswith(prefix):
                return candidate
    return candidates[0]


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


def _import_table():
    """IAT slot RVA -> (dll, symbol) from the retail import directory."""
    data = RETAIL_EXE.read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    section_count = struct.unpack_from("<H", data, pe + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe + 20)[0]
    optional = pe + 24
    sections = []
    for index in range(section_count):
        offset = optional + optional_size + index * 40
        virtual_size, rva, raw_size, raw = struct.unpack_from(
            "<IIII", data, offset + 8)
        sections.append((rva, max(virtual_size, raw_size), raw_size, raw))

    def raw_offset(rva):
        for start, span, raw_size, raw in sections:
            if start <= rva < start + span and rva - start < raw_size:
                return raw + rva - start
        return None

    def read_string(rva):
        offset = raw_offset(rva)
        end = data.index(b"\0", offset)
        return data[offset:end].decode("latin-1")

    import_rva = struct.unpack_from("<I", data, optional + 96 + 1 * 8)[0]
    slots = {}
    descriptor = raw_offset(import_rva)
    while descriptor is not None:
        original, _stamp, _chain, name_rva, first = struct.unpack_from(
            "<IIIII", data, descriptor)
        if not (original or first or name_rva):
            break
        dll = read_string(name_rva)
        lookup = raw_offset(original or first)
        slot = first
        while True:
            entry = struct.unpack_from("<I", data, lookup)[0]
            if not entry:
                break
            symbol = ("ordinal_%d" % (entry & 0xFFFF)
                      if entry & 0x80000000 else read_string(entry + 2))
            slots[slot] = (dll, symbol)
            lookup += 4
            slot += 4
        descriptor += 20
    return slots, data, raw_offset


def _resolve_thunk(data, raw_offset, rva, slots):
    """(dll, symbol, iat_rva) for a ff25 thunk, from the unmasked image."""
    offset = raw_offset(rva)
    if offset is None or data[offset:offset + 2] != b"\xff\x25":
        return None
    target_va = struct.unpack_from("<I", data, offset + 2)[0]
    iat_rva = target_va - IMAGE_BASE
    entry = slots.get(iat_rva)
    if entry is None:
        return None
    return entry[0], entry[1], iat_rva


def _reference_graph():
    """callers[name] and callees[name] across every delinked object.

    Names are the delinker's: claimed functions carry their claimed symbol,
    unmatched ones their inventory name. Callers are recorded per source
    object ("SOURCE/KB" or "(unmatched)"), callees per referenced symbol.
    """
    callers = defaultdict(set)
    callees = defaultdict(set)
    for path in sorted(DELINK_ROOT.rglob("*.c.obj")):
        source = str(path.relative_to(DELINK_ROOT))[:-len(".c.obj")]
        try:
            coff = CoffFile(path)
        except (ValueError, IndexError, struct.error):
            continue
        text_sections = {section.index for section in coff.sections
                         if section.name == ".text"}
        functions = sorted(
            (symbol.value, symbol.name) for symbol in coff.symbols.values()
            if symbol.section in text_sections)
        starts = [value for value, _name in functions]
        import bisect
        for (section_index, site), reloc in sorted(coff.relocations.items()):
            if section_index not in text_sections:
                continue
            target = coff.symbols.get(reloc.symbol_index)
            if target is None:
                continue
            slot = bisect.bisect_right(starts, site) - 1
            owner = functions[slot][1] if slot >= 0 else source
            callees[owner].add(target.name)
            callers[target.name].add(
                owner if source == "(unmatched)" else source)
    return callers, callees


PREFIX = 24


def _candidate_index():
    index = defaultdict(list)
    definition_orders = defaultdict(list)
    candidate_sizes = {}
    prefix_index = defaultdict(set)

    def register(rows):
        for source, symbol, order, masked in rows:
            index[(len(masked), hashlib.sha256(masked).hexdigest())].append(
                (source, symbol, order))
            definition_orders[source].append((order, symbol))
            candidate_sizes[(source, order)] = len(masked)
            if len(masked) >= PREFIX:
                prefix_index[hashlib.sha256(masked[:PREFIX]).hexdigest()].add(
                    (source, symbol))

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
    return index, definition_orders, candidate_sizes, prefix_index


def align_sizes(source_sizes, image_sizes, tolerance=0.15, floor=16):
    """Monotone greedy alignment of two in-order size sequences.

    Returns index pairs (i, j) where source function i plausibly occupies
    image slot j: sizes agree within max(floor, tolerance * larger). Both
    sequences advance monotonically; a mismatch advances whichever side has
    the smaller current size (it cannot pair with anything later either).
    """
    pairs = []
    i = j = 0
    while i < len(source_sizes) and j < len(image_sizes):
        a, b = source_sizes[i], image_sizes[j]
        if abs(a - b) <= max(floor, tolerance * max(a, b)):
            pairs.append((i, j))
            i += 1
            j += 1
        elif a < b:
            i += 1
        else:
            j += 1
    return pairs


def run_census():
    unmatched = _unmatched_functions()
    index, definition_orders, candidate_sizes, prefix_index = _candidate_index()
    slots, image, raw_offset = _import_table()
    callers, callees = _reference_graph()
    masked_by_rva = {rva: masked for rva, _size, _name, masked in unmatched}

    rows = []
    matches_by_source = defaultdict(list)
    for rva, size, name, masked in unmatched:
        row = {"rva": rva, "size": size, "name": name, "class": "unknown",
               "source": "", "symbol": "", "callers": "", "callees": "",
               "detail": ""}
        rows.append(row)
        key = (len(masked), hashlib.sha256(masked).hexdigest())
        candidates = index.get(key, [])
        if candidates:
            sources = {source for source, _symbol, _order in candidates}
            primary = prefer_candidate(candidates, name)
            row["class"] = ("crt-exact" if all(
                s.split(":")[0].endswith(".lib") for s in sources)
                else "game-exact")
            row["source"], row["symbol"] = primary[0], primary[1]
            if len(candidates) > 1:
                row["detail"] = "%d identical candidates: %s" % (
                    len(candidates), "|".join(
                        "%s!%s" % (s, y) for s, y, _o in candidates[:4]))
            if len(sources) == 1:
                matches_by_source[primary[0]].append((primary[2], rva))
            continue
        thunk = _resolve_thunk(image, raw_offset, rva, slots)
        if thunk is not None:
            row["class"] = "import-thunk"
            row["source"], row["symbol"] = thunk[0], thunk[1]
            row["detail"] = "IAT 0x%x" % thunk[2]
            continue
        if name.startswith("Unwind@"):
            row["class"] = "eh-funclet"
            row["detail"] = "SEH unwind funclet (EH-compiled TU)"

    # DNA pass: definition order against image order. A gap bracketed by two
    # consecutive matches from one source object is filled directly when the
    # counts agree ("order-inferred"), otherwise by monotone size alignment
    # ("order-aligned" - weaker evidence, size deltas recorded).
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
                if len(between_orders) == len(between_rvas):
                    for order, rva in zip(between_orders, between_rvas):
                        row = by_rva[rva]
                        row["class"] = "order-inferred"
                        row["source"] = source
                        row["symbol"] = names.get(order, "?")
                        row["detail"] = ("between 0x%x and 0x%x" %
                                         (left_rva, right_rva))
                        inferred += 1
                    continue
                source_sizes = [candidate_sizes.get((source, order), 0)
                                for order in between_orders]
                image_sizes = [by_rva[rva]["size"] for rva in between_rvas]
                for i, j in align_sizes(source_sizes, image_sizes):
                    row = by_rva[between_rvas[j]]
                    row["class"] = "order-aligned"
                    row["source"] = source
                    row["symbol"] = names.get(between_orders[i], "?")
                    row["detail"] = (
                        "between 0x%x and 0x%x; size ours=%d cand=%d" % (
                            left_rva, right_rva, image_sizes[j],
                            source_sizes[i]))
                    inferred += 1

    # Prefix pass: a shared first-24-byte masked prefix is weak evidence, but
    # it names a concrete candidate for review where nothing else does.
    for row in rows:
        if row["class"] != "unknown":
            continue
        masked = masked_by_rva[row["rva"]]
        if len(masked) < PREFIX:
            continue
        prefix_candidates = prefix_index.get(
            hashlib.sha256(masked[:PREFIX]).hexdigest(), ())
        symbols = {symbol for _source, symbol in prefix_candidates}
        if not symbols:
            continue
        if len(symbols) == 1:
            source, symbol = sorted(prefix_candidates)[0]
            row["class"] = ("crt-prefix"
                            if source.split(":")[0].endswith(".lib")
                            else "game-prefix")
            row["source"], row["symbol"] = source, symbol
            row["detail"] = "prefix-%d only; size ours=%d" % (
                PREFIX, row["size"])
        else:
            row["detail"] = "prefix-%d candidates: %s" % (
                PREFIX, "|".join(sorted(symbols)[:4]))

    # Reference heuristics: contact with the CRT leans CRT, contact with only
    # claimed game units leans game. Evidence columns always; class refinement
    # only when the signal is one-sided.
    class_by_name = {row["name"]: row["class"] for row in rows}

    def kind_of(entity):
        if entity in class_by_name:
            kind = class_by_name[entity]
            if kind.startswith(("crt", "import")):
                return "crt"
            if kind.startswith("game"):
                return "game"
            if kind == "eh-funclet":
                return "eh"
            return "un"
        if entity == "(unmatched)":
            return "un"
        return "game"   # claimed unit source or claimed symbol name

    def summarize(entities):
        counts = defaultdict(int)
        for entity in entities:
            counts[kind_of(entity)] += 1
        return ",".join("%s:%d" % (kind, counts[kind])
                        for kind in ("game", "crt", "eh", "un")
                        if counts.get(kind))

    for row in rows:
        row["callers"] = summarize(callers.get(row["name"], ()))
        row["callees"] = summarize(callees.get(row["name"], ()))
        if row["class"] != "unknown":
            continue
        caller_kinds = {kind_of(e) for e in callers.get(row["name"], ())}
        callee_kinds = {kind_of(e) for e in callees.get(row["name"], ())}
        contact = caller_kinds | callee_kinds
        if "crt" in contact and "game" not in contact:
            row["class"] = "unknown-crt-linked"
        elif "game" in contact and "crt" not in contact and contact:
            row["class"] = "unknown-game-linked"
    return rows, inferred, callers, callees


CARVING_OUTPUT = REPO / "docs/buka-new-tu-carving.tsv"


def write_carving(rows, callers):
    """Cluster the game-side residue into new-TU proposals.

    Eligible rows are game-linked or unclassified functions; clusters split at
    0x200-byte address gaps (functions link in definition order, so a cluster
    is plausibly one TU's contribution). The dominant claimed caller unit is
    the suggested home; wide or absent caller sets suggest a fresh TU.
    """
    eligible = [row for row in rows if row["class"] in
                ("unknown-game-linked", "game-prefix", "unknown")]
    clusters = []
    current = []
    for row in sorted(eligible, key=lambda value: value["rva"]):
        if current and row["rva"] - (current[-1]["rva"] + current[-1]["size"]) > 0x200:
            clusters.append(current)
            current = []
        current.append(row)
    if current:
        clusters.append(current)

    with CARVING_OUTPUT.open("w", newline="") as stream:
        stream.write(
            "# Game-side residue carved into address clusters for new-TU\n"
            "# planning. Functions link in definition order, so one cluster\n"
            "# is plausibly one TU's contribution. Generated by\n"
            "# `homm2 audit unmatched-census --write-carving`.\n"
            "cluster\tstart_rva\tend_rva\tfunctions\tbytes\tcaller_units\t"
            "suggested_home\tnamed_functions\n")
        for number, cluster in enumerate(
                sorted(clusters,
                       key=lambda value: -sum(r["size"] for r in value)), 1):
            units = defaultdict(int)
            for row in cluster:
                for caller in callers.get(row["name"], ()):
                    if "/" in caller:
                        units[caller] += 1
            ranked = sorted(units.items(), key=lambda item: -item[1])
            total = sum(count for _unit, count in ranked)
            if ranked and ranked[0][1] >= max(2, 0.6 * total):
                home = "join %s" % ranked[0][0]
            elif ranked:
                home = "new TU (callers span %d units)" % len(ranked)
            else:
                home = "new TU (no claimed callers)"
            named = [row["name"] for row in cluster
                     if not row["name"].startswith(("FUN_", "Unwind@"))]
            stream.write("%d\t0x%x\t0x%x\t%d\t%d\t%s\t%s\t%s\n" % (
                number, cluster[0]["rva"],
                cluster[-1]["rva"] + cluster[-1]["size"], len(cluster),
                sum(row["size"] for row in cluster),
                ",".join("%s:%d" % item for item in ranked[:4]),
                home, ",".join(named[:6])))
    return len(clusters)


def write_compgen_rows(rows, callers):
    """Interim VA_COMPGEN store: game-side identified bodies with owner units.

    The owner is the dominant claimed caller unit when one exists, else the
    base object the bytes matched (a TU that also instantiates the body),
    else the catch-all "(compgen)" module. Rows return to source markers when
    the campaign reaches them.
    """
    eligible = [row for row in rows
                if row["class"] in ("game-exact", "order-inferred",
                                    "order-aligned", "game-prefix")
                and row["source"].startswith("base:")]
    with COMPGEN_OUTPUT.open("w", newline="") as stream:
        stream.write(
            "# Compiler-generated / unclaimed game bodies identified against\n"
            "# our own base objects; interim store until VA_COMPGEN markers\n"
            "# land in source. Generated by `homm2 audit unmatched-census\n"
            "# --write-config`.\n"
            "entry_rva,size,symbol,unit,source_object,evidence\n")
        for row in sorted(eligible, key=lambda value: value["rva"]):
            units = defaultdict(int)
            for caller in callers.get(row["name"], ()):
                if "/" in caller:
                    units[caller] += 1
            ranked = sorted(units.items(), key=lambda item: -item[1])
            if ranked and (len(ranked) == 1
                           or ranked[0][1] > ranked[1][1]):
                owner = ranked[0][0]
            else:
                owner = row["source"].removeprefix("base:") or "(compgen)"
            evidence = {"game-exact": "masked-exact",
                        "order-inferred": "order-inferred",
                        "order-aligned": "order-aligned",
                        "game-prefix": "prefix-%d" % PREFIX}[row["class"]]
            stream.write("0x%x,%d,%s,%s,%s,%s\n" % (
                row["rva"], row["size"], row["symbol"], owner,
                row["source"].removeprefix("base:"), evidence))
    with FUNCLET_OUTPUT.open("w", newline="") as stream:
        stream.write(
            "# SEH unwind funclets (destructor-call handlers of EH-compiled\n"
            "# TUs). Names are the analysis placeholders until each funclet\n"
            "# is attributed to its parent function. Generated by\n"
            "# `homm2 audit unmatched-census --write-config`.\n"
            "entry_rva,size,name\n")
        funclets = [row for row in rows if row["class"] == "eh-funclet"]
        for row in sorted(funclets, key=lambda value: value["rva"]):
            stream.write("0x%x,%d,%s\n" % (row["rva"], row["size"],
                                            row["name"]))
    return len(eligible), len(funclets)


def write_config_rows(rows):
    """Persist reviewed identification CSVs for CRT bodies and import thunks."""
    crt = [row for row in rows
           if row["class"] in ("crt-exact", "order-inferred", "order-aligned",
                               "crt-prefix")
           and row["source"].split(":")[0].endswith(".lib")]
    with CRT_OUTPUT.open("w", newline="") as stream:
        stream.write(
            "# CRT bodies identified in the retail image by masked-byte and\n"
            "# definition-order evidence against the VC6 toolchain archives.\n"
            "# Generated by `homm2 audit unmatched-census --write-config`;\n"
            "# review before trusting rows whose alternates list collisions.\n"
            "entry_rva,size,symbol,member,library,evidence,alternates\n")
        for row in sorted(crt, key=lambda value: value["rva"]):
            library, _colon, member = row["source"].partition(":")
            stream.write("0x%x,%d,%s,%s,%s,%s,%s\n" % (
                row["rva"], row["size"], row["symbol"],
                member.replace("\\", "/").rsplit("/", 1)[-1], library,
                {"crt-exact": "masked-exact",
                 "order-inferred": "order-inferred",
                 "order-aligned": "order-aligned",
                 "crt-prefix": "prefix-%d" % PREFIX}[row["class"]],
                row["detail"].replace(",", ";")))
    thunks = [row for row in rows if row["class"] == "import-thunk"]
    with THUNK_OUTPUT.open("w", newline="") as stream:
        stream.write(
            "# Import thunks (jmp [IAT]) resolved through the retail import\n"
            "# directory. Generated by `homm2 audit unmatched-census"
            " --write-config`.\n"
            "entry_rva,size,dll,symbol,iat_rva\n")
        for row in sorted(thunks, key=lambda value: value["rva"]):
            stream.write("0x%x,%d,%s,%s,%s\n" % (
                row["rva"], row["size"], row["source"], row["symbol"],
                row["detail"].replace("IAT ", "")))
    return len(crt), len(thunks)


def main(argv=None):
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write-config", action="store_true",
                        help="persist config/crt_functions.csv and "
                             "config/import_thunks.csv from this census")
    parser.add_argument("--write-carving", action="store_true",
                        help="persist docs/buka-new-tu-carving.tsv cluster "
                             "proposals for the game-side residue")
    args = parser.parse_args(argv)
    rows, inferred, callers, _callees = run_census()
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
        totals[row["class"]][1] += row["size"]
    print("[unmatched-census] %d functions -> %s" % (len(rows), OUTPUT))
    for kind in ("game-exact", "crt-exact", "order-inferred",
                 "order-aligned", "crt-prefix", "game-prefix", "import-thunk",
                 "eh-funclet", "unknown-crt-linked", "unknown-game-linked",
                 "unknown"):
        count, size = totals.get(kind, (0, 0))
        print("[unmatched-census]   %-19s %5d functions  %7d bytes" %
              (kind, count, size))

    unknown = [row for row in rows if row["class"].startswith("unknown")]
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
    if args.write_config:
        crt_count, thunk_count = write_config_rows(rows)
        print("[unmatched-census] wrote %d CRT rows -> %s" %
              (crt_count, CRT_OUTPUT))
        print("[unmatched-census] wrote %d thunk rows -> %s" %
              (thunk_count, THUNK_OUTPUT))
        compgen_count, funclet_count = write_compgen_rows(rows, callers)
        print("[unmatched-census] wrote %d compgen rows -> %s" %
              (compgen_count, COMPGEN_OUTPUT))
        print("[unmatched-census] wrote %d funclet rows -> %s" %
              (funclet_count, FUNCLET_OUTPUT))
    if args.write_carving:
        cluster_count = write_carving(rows, callers)
        print("[unmatched-census] wrote %d clusters -> %s" %
              (cluster_count, CARVING_OUTPUT))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
