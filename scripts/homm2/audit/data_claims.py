"""Derive retail addresses for reconstructed data globals, and audit their extents.

This is the donation evidence transposed. `homm2 audit reloc-donation` asks
"which name owns this address", which a balanced operand transposition makes
ambiguous: `MAP_WIDTH * MAP_HEIGHT` compiles with either factor first, so the
site that should have named `MAP_WIDTH` unanimously names it 217 times out of
228 and donation -- which requires unanimity -- recovers nothing. Asking the
other question, "which address does this symbol own", turns those eleven
dissenting sites into a losing minority instead of a veto.

The vote is only half the evidence. A majority address is then checked against
the retail payload with the object's own pointer fields masked, which is what
separates a real owner from a plausible neighbour: an initialized allocation
must match retail byte for byte, a `.bss`/common allocation must land on zero
storage, and a `.rdata` allocation must land in `.rdata`.

    homm2 audit data-claims                  # derive and report, write nothing
    homm2 audit data-claims --write          # insert the accepted DATA() markers
    homm2 audit data-claims extents          # audit the extents of existing claims

`extents` answers the question the derivation raises but cannot settle: a claim
fixes where an allocation starts, and the retail payload and the next claimed
address then bound how long it may be. Where a reconstruction is longer than
that bound the surplus is storage the source invented -- a `NULL` sentinel, a
zero pad -- and although no code byte depends on it, every later allocation in
the final link moves by exactly that much.

Votes are counted per `(unit, symbol)` for file statics and program-wide for
external ones, because a static's linker name is unique only inside its unit:
ten icon decoders each define their own `s_dst`, and one pooled ballot would
hand them all the same address.

Nothing here writes a build artifact. `--write` edits `src/` and is the only
side effect; the derived tables land under `build/gen`.
"""
from __future__ import annotations

import argparse
import bisect
import csv
import hashlib
import json
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

from homm2.build.canonicalize_relocs import CoffFile
from homm2.core.paths import REPO

IMAGE_BASE = 0x400000
IMAGE_DIR32 = 6
MEM_EXECUTE = 0x20000000
FUNCTION_TYPE = 0x20
SYM_CLASS_STATIC = 3

EXE = REPO / "build/orig/HMM2PL.exe"
SYMBOLS = REPO / "build/gen/symbol_names.csv"
BASE_OBJS = REPO / "build/objdiff/base"
SOURCE_ROOT = REPO / "src"
GEN = REPO / "build/gen"

# Compiler-owned symbols: literal pools, floating-point constants, vtables, RTTI
# and the EH funclet family. None of them is a reconstructed source global, and
# several are emitted per call site rather than per definition, so a vote from
# one says nothing about where any single allocation lives.
SKIP_PREFIX = ("__real@", "??_C@", "$SG", "$L", "__ehhandler",
               "__unwindfunclet", "__catch", "__except_list", "??_7", "??_R")

# `?name@@3<type>A` for external linkage, `_name` for internal. Anything else is
# a member, a template instance, or a mangled local static; those are claimed by
# their own markers, not by this derivation.
GLOBAL_NAME = re.compile(r"^(?:\?([A-Za-z_]\w*)@@3|_([A-Za-z_]\w*)$)")

VARDECL_CACHE_SCHEMA = 4


# --------------------------------------------------------------------------
# the retail image
# --------------------------------------------------------------------------

class RetailImage:
    """Section-addressed read access to the linked target."""

    def __init__(self, blob: bytes):
        self.blob = blob
        pe = struct.unpack_from("<I", blob, 0x3C)[0]
        count = struct.unpack_from("<H", blob, pe + 6)[0]
        optional = struct.unpack_from("<H", blob, pe + 20)[0]
        self.extent = struct.unpack_from("<I", blob, pe + 0x50)[0]
        self.sections = []
        for index in range(count):
            off = pe + 24 + optional + index * 40
            name = bytes(blob[off:off + 8]).rstrip(b"\0").decode("latin-1")
            virtual_size, rva, raw_size, raw_offset = struct.unpack_from(
                "<4I", blob, off + 8)
            self.sections.append((name, rva, virtual_size, raw_size, raw_offset))

    @classmethod
    def load(cls, path: Path = EXE) -> "RetailImage":
        return cls(path.read_bytes())

    def payload(self, rva: int, size: int) -> bytes | None:
        """Bytes at `rva`, or None when the RVA is not mapped.

        A section's virtual size may exceed its raw size; the loader zero-fills
        that tail, so a `.bss`-style allocation reads as zeros rather than as a
        short read.
        """
        for _name, base, virtual_size, raw_size, raw_offset in self.sections:
            if not base <= rva < base + virtual_size:
                continue
            raw = min(virtual_size, raw_size)
            start = raw_offset + rva - base
            if rva + size <= base + raw:
                return self.blob[start:start + size]
            if rva >= base + raw:
                return b"\0" * size
            head = self.blob[start:raw_offset + raw]
            return bytes(head) + b"\0" * (size - len(head))
        return None

    def storage(self, rva: int) -> str:
        """Section name for `rva`, suffixed when it falls in the loader-zero tail."""
        for name, base, virtual_size, raw_size, raw_offset in self.sections:
            if base <= rva < base + virtual_size:
                return name if rva < base + min(virtual_size, raw_size) \
                    else name + "-zero-tail"
        return "?"


# --------------------------------------------------------------------------
# our objects
# --------------------------------------------------------------------------

def section_flags(coff: CoffFile, section) -> int:
    first = 20 + struct.unpack_from("<H", coff.data, 16)[0]
    return struct.unpack_from("<I", coff.data, first + (section.index - 1) * 40 + 36)[0]


def function_bodies(coff: CoffFile) -> dict:
    """name -> (bytes, [(offset, type, symbol, addend)]) for every code symbol."""
    out = {}
    for section in coff.sections:
        if not section_flags(coff, section) & MEM_EXECUTE or not section.raw_size:
            continue
        symbols = [symbol for symbol in coff.symbols.values()
                   if symbol.section == section.index
                   and struct.unpack_from(
                       "<H", coff.data, symbol.offset + 14)[0] == FUNCTION_TYPE]
        if not symbols:
            continue
        payload = bytes(coff.data[section.raw_offset:
                                  section.raw_offset + section.raw_size])
        relocs = []
        for (index, site), reloc in coff.relocations.items():
            if index != section.index:
                continue
            target = coff.symbols.get(reloc.symbol_index)
            addend = struct.unpack_from("<i", payload, site)[0] \
                if site + 4 <= len(payload) else 0
            relocs.append((site, reloc.typ, target.name if target else None, addend))
        relocs.sort()
        bounds = sorted({symbol.value for symbol in symbols} | {section.raw_size})
        for symbol in symbols:
            start = symbol.value
            end = min(bound for bound in bounds if bound > start)
            local = [(site - start, kind, name, addend)
                     for site, kind, name, addend in relocs if start <= site < end]
            out[symbol.name] = (payload[start:end], local)
    return out


def data_symbols(coff: CoffFile, unit: str) -> dict:
    """Defined non-code symbols: name -> record with payload and pointer sites.

    The span runs to the next symbol in the same section, which is the object's
    own idea of the allocation length and can include tail padding. `pointers`
    lists the relocated fields inside it, relative to the symbol, so a payload
    comparison can mask exactly the words whose value the linker chooses.
    """
    out = {}
    bounds = defaultdict(set)
    for symbol in coff.symbols.values():
        if symbol.section > 0:
            bounds[symbol.section].add(symbol.value)
    reloc_sites = defaultdict(list)
    for (index, site), _reloc in coff.relocations.items():
        reloc_sites[index].append(site)
    for symbol in coff.symbols.values():
        if symbol.section < 0:
            continue
        if symbol.section == 0:
            # An uninitialized external is a COMMON declaration whose value is
            # its size; it has no section and no payload to compare.
            if symbol.value > 0:
                out[symbol.name] = {
                    "unit": unit, "section": "<common>", "size": symbol.value,
                    "internal": False, "payload": None, "pointers": []}
            continue
        section = coff.sections[symbol.section - 1]
        if section_flags(coff, section) & MEM_EXECUTE:
            continue
        if section.name.startswith((".drectve", ".debug")):
            continue
        later = sorted(bound for bound in bounds[symbol.section]
                       if bound > symbol.value)
        end = later[0] if later else section.raw_size
        payload = None
        if section.raw_offset and section.raw_size:
            payload = bytes(coff.data[section.raw_offset + symbol.value:
                                      section.raw_offset + end])
        out[symbol.name] = {
            "unit": unit, "section": section.name, "size": end - symbol.value,
            "internal": coff.data[symbol.offset + 16] == SYM_CLASS_STATIC,
            "payload": payload,
            "pointers": sorted(site - symbol.value
                               for site in reloc_sites[symbol.section]
                               if symbol.value <= site < end)}
    return out


# --------------------------------------------------------------------------
# evidence rules
# --------------------------------------------------------------------------

def masked_equal(ours: bytes, retail: bytes, sites) -> bool:
    """Compare a compiled body to retail with every relocated field blanked.

    Our object stores an addend where the linker stores a target, so the four
    bytes at a relocation site can never agree; masking both sides is what makes
    "these are the same instructions" decidable before the symbol is placed.
    """
    if len(ours) < len(retail):
        return False
    ours = bytearray(ours[:len(retail)])
    retail = bytearray(retail)
    for offset, _kind, _symbol, _addend in sites:
        for index in range(offset, min(offset + 4, len(retail))):
            ours[index] = retail[index] = 0
    return bytes(ours) == bytes(retail)


def payload_verdict(record: dict, rva: int, image: RetailImage):
    """Check a candidate address against the retail bytes.

    Returns `(verdict, span)`, where `span` is `(first, last, length)` over the
    differing byte offsets so a caller can tell a wholesale contradiction from
    an overrun confined to the final word.
    """
    payload = record["payload"]
    if not payload:
        actual = image.payload(rva, max(record["size"], 1))
        if actual is None:
            return "unreadable", None
        return ("zero" if not any(actual) else "nonzero-bss"), None
    actual = image.payload(rva, len(payload))
    if actual is None:
        return "unreadable", None
    ours = bytearray(payload)
    theirs = bytearray(actual)
    for site in record["pointers"]:
        for index in range(site, min(site + 4, len(ours))):
            ours[index] = theirs[index] = 0
    differing = [index for index in range(len(ours)) if ours[index] != theirs[index]]
    if not differing:
        return ("exact" if not record["pointers"] else "exact-masked"), None
    return ("differs(%d/%d)" % (len(differing), len(ours)),
            (differing[0], differing[-1], len(ours)))


def owner_votes(retail: bytes, sites, extent: int):
    """`(symbol, owner rva)` for each DIR32 site of one masked-identical body.

    Our object stores an addend at the site and the linker stored the target, so
    `target - addend` is where the symbol itself was placed. Sites naming a
    compiler-owned symbol, and dwords that are not linked addresses at all, carry
    no owner and are dropped rather than voting for a nonsense address.
    """
    out = []
    for offset, kind, symbol, addend in sites:
        if kind != IMAGE_DIR32 or offset + 4 > len(retail) or not symbol:
            continue
        if symbol.startswith(SKIP_PREFIX):
            continue
        target = struct.unpack_from("<I", retail, offset)[0]
        if not IMAGE_BASE <= target < IMAGE_BASE + extent:
            continue
        owner = target - IMAGE_BASE - addend
        if 0 <= owner < extent:
            out.append((symbol, owner))
    return out


def has_majority(ranked, total: int) -> bool:
    """A clear owner: at least twice the runner-up and at least half of all votes.

    Unanimity is unreachable for the most-referenced globals, because a balanced
    operand transposition puts the wrong symbol under the site's addend in a
    minority of call sites. A doubled runner-up and an absolute majority are the
    two independent ways that minority can be recognised as one.
    """
    if not ranked:
        return False
    votes = ranked[0][1]
    runner_up = ranked[1][1] if len(ranked) > 1 else 0
    return votes >= 2 * runner_up and votes * 2 >= total


def identifier(symbol: str) -> str | None:
    """Source identifier behind a namespace-scope linker name, else None."""
    match = GLOBAL_NAME.match(symbol)
    if not match:
        return None
    return match.group(1) or match.group(2)


def inventory_key(unit: str, symbol: str):
    """How the public-data inventory tells two definitions apart.

    An external definition is unique in the program, so its identifier alone
    names it. An internal one is unique only within its translation unit, and
    the decoder family really does define ten distinct `s_clipB`s -- so the unit
    is part of its identity, and a vote from one decoder must never be counted
    towards another's allocation.
    """
    name = identifier(symbol)
    if name is None:
        return None
    return (unit if symbol.startswith("_") else "", name)


def select_claims(proposals, vardecls):
    """Apply the reviewed evidence bar to derived proposals.

    Returns `(accepted, rejected)`; every rejected row keeps the rule that
    dropped it, because a reason that starts appearing in bulk is usually a
    reconstruction fault rather than a derivation one.
    """
    file_scope = [row for row in vardecls
                  if row["scope"] in ("TRANSLATION_UNIT", "STRUCT_DECL", "CLASS_DECL")]
    by_symbol = defaultdict(list)
    for row in file_scope:
        by_symbol[(row["unit"], row["symbol"])].append(row)
    # The public-data inventory tells definitions apart by inventory_key, so two
    # that share one key would both be required to carry the same address.
    identifier_units = defaultdict(set)
    for row in file_scope:
        key = inventory_key(row["unit"], row["symbol"])
        if key:
            identifier_units[key].add((row["unit"], row["symbol"]))

    accepted, rejected = [], []

    def drop(proposal, reason):
        rejected.append(dict(proposal, reason=reason))

    for proposal in proposals:
        symbol = proposal["symbol"]
        name = identifier(symbol)
        if name is None:
            drop(proposal, "not a namespace-scope global spelling")
            continue
        rows = by_symbol.get((proposal["unit"], symbol))
        if not rows:
            elsewhere = sorted(unit for unit, other in by_symbol if other == symbol)
            if not elsewhere:
                drop(proposal, "no file-scope definition")
            else:
                drop(proposal, "object owner %s != source owner %s"
                     % (proposal["unit"], ", ".join(elsewhere)))
            continue
        if len(rows) > 1 or proposal["ambiguous_unit"]:
            drop(proposal, "defined in more than one place")
            continue
        row = rows[0]
        if row.get("marked"):
            drop(proposal, "definition already carries a DATA marker")
            continue
        if len(identifier_units[inventory_key(row["unit"], symbol)]) > 1:
            drop(proposal, "identifier is shared by another definition")
            continue
        if proposal["collides_function"]:
            drop(proposal, "address is a claimed function")
            continue
        if proposal.get("collides_claim"):
            drop(proposal, "address is already claimed by %s"
                 % proposal["collides_claim"])
            continue
        if row["size"] != proposal["size"]:
            # The COFF span runs to the next symbol; a mismatch means the object
            # laid the allocation out with padding or an alias.
            proposal = dict(proposal, size=row["size"])
        if not row["line_prefix_blank"] or row["siblings_in_span"] != 1:
            drop(proposal, "declaration is not a lone statement")
            continue
        if not has_majority(proposal["ranked"], proposal["total"]):
            drop(proposal, "no clear owner majority")
            continue

        verdict = proposal["payload"]
        section, storage = proposal["section"], proposal["storage"]
        if verdict in ("exact", "exact-masked"):
            pass
        elif verdict == "zero":
            if section not in (".bss", "<common>"):
                drop(proposal, "initialized allocation lands on zero storage")
                continue
        elif verdict.startswith("differs"):
            span = proposal["diff_span"]
            # The compared window runs to the next symbol in OUR object; when
            # retail's next allocation starts earlier the overrun shows up as a
            # differing final dword and says nothing about the start address.
            if span is None or span[2] <= 4 or span[0] < span[2] - 4:
                drop(proposal, "payload contradicts the address")
                continue
            proposal = dict(proposal, payload="tail-overrun")
        else:
            drop(proposal, "payload verdict " + verdict)
            continue
        if section == ".rdata" and storage != ".rdata":
            drop(proposal, "read-only allocation lands outside .rdata")
            continue
        if section == ".data" and storage not in (".data", ".data-zero-tail"):
            drop(proposal, "writable allocation lands in " + storage)
            continue
        accepted.append(dict(proposal, file=row["file"], start=row["start"],
                             line=row["line"], identifier=name,
                             type=row["type"], static=row["static"]))

    # One address, one owner. Where two survivors want the same RVA the one whose
    # payload is proven wins and the other is left for review.
    order = {"exact": 0, "exact-masked": 1, "tail-overrun": 2, "zero": 3}
    winners = {}
    for proposal in accepted:
        best = winners.get(proposal["rva"])
        if best is None or order[proposal["payload"]] < order[best["payload"]]:
            winners[proposal["rva"]] = proposal
    final = []
    for proposal in accepted:
        if winners[proposal["rva"]] is proposal:
            final.append(proposal)
        else:
            drop(proposal, "address also claimed by %s"
                 % winners[proposal["rva"]]["symbol"])
    final.sort(key=lambda row: (row["file"], row["start"]))
    return final, rejected


def insert_markers(blob: bytes, claims) -> bytes:
    """Write `DATA(0x........)` in front of each claimed definition.

    Insertion runs back to front so that an earlier claim's recorded offset is
    still valid after a later one has been written.
    """
    for claim in sorted(claims, key=lambda row: -row["start"]):
        start = claim["start"]
        marker = b"DATA(0x%08x) " % (claim["rva"] + IMAGE_BASE)
        blob = blob[:start] + marker + blob[start:]
    return blob


# --------------------------------------------------------------------------
# extents
# --------------------------------------------------------------------------

def proven_extent(size: int, ours: bytes | None, retail: bytes | None,
                  pointers, neighbour: int) -> int:
    """Longest allocation retail can have at this address, from two channels.

    The first is the payload: our bytes and retail's must agree while both
    belong to this object, so the lowest index at which they disagree -- with
    our own relocated fields masked, since those hold an addend rather than a
    target -- is an upper bound on the length.

    The second is the next *claimed* address. Only a function or a `DATA()`
    global counts. A `const_` site is merely an address somebody references, and
    `&array[last]` references the interior of this very allocation, so reading
    those as neighbours reports every array with a taken interior address as
    over-long.
    """
    contradiction = size
    if ours is not None and retail is not None:
        masked = set()
        for site in pointers or ():
            masked.update(range(site, site + 4))
        limit = min(size, len(ours), len(retail))
        for index in range(limit):
            if index not in masked and ours[index] != retail[index]:
                contradiction = index
                break
    return max(0, min(size, contradiction, neighbour))


def extent_finding(size: int, proven: int, ours: bytes | None, element_size: int):
    """Classify a claimed allocation whose reconstruction outruns its evidence.

    A surplus is only a length fault when the source zero-filled it: a trailing
    `NULL` in a pointer table, a `0` closing an initializer list. A surplus that
    holds our own non-zero data means the address or the content is wrong
    instead, which is a different repair and is reported as such.
    """
    overrun = size - proven
    if overrun <= 0:
        return "ok", 0
    if ours is not None and any(ours[proven:min(size, len(ours))]):
        return "nonzero-tail", overrun
    if element_size >= 2 and overrun == element_size:
        return "one-element-too-long", overrun
    return "too-long", overrun


# --------------------------------------------------------------------------
# inputs
# --------------------------------------------------------------------------

def read_inventory(path: Path = SYMBOLS):
    """The claimed inventory: functions per unit, and every claimed address."""
    functions = defaultdict(list)
    rows = []
    with path.open(encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            row["rva"] = int(row["rva"], 16)
            row["size"] = int(row["size"], 16)
            rows.append(row)
            if row["kind"] == "func":
                functions[row["unit"]].append((row["rva"], row["size"], row["name"]))
    return functions, rows


def source_vardecls(cache_path: Path | None, refresh: bool = False):
    """Every file-scope VarDecl definition in `src/`, with its source offset.

    Parsing 99 translation units with libclang costs about a minute, so the
    result is cached per file against its content hash; a run after one edited
    source reparses that one file.
    """
    import clang.cindex as ci

    from homm2.build.annotated_data import (
        _clang_args, _declaration_end, _mask_lexical_noise, configure_libclang)
    from homm2.clang_options import ClangMode

    cache = {}
    if cache_path and cache_path.exists() and not refresh:
        try:
            stored = json.loads(cache_path.read_text())
            if stored.get("schema") == VARDECL_CACHE_SCHEMA:
                cache = stored.get("files", {})
        except (OSError, ValueError):
            cache = {}

    configure_libclang()
    index = ci.Index.create()
    files = {}
    for path in sorted(SOURCE_ROOT.rglob("*.cpp")):
        unit = path.relative_to(SOURCE_ROOT).with_suffix("").as_posix()
        blob = path.read_bytes()
        digest = hashlib.sha256(blob).hexdigest()
        cached = cache.get(unit)
        if cached and cached.get("digest") == digest:
            files[unit] = cached
            continue
        masked = _mask_lexical_noise(blob)
        translation = index.parse(
            str(path), args=_clang_args(REPO, path, mode=ClangMode.RETAIL_ANALYSIS))
        variables = [
            cursor for cursor in translation.cursor.walk_preorder()
            if cursor.kind == ci.CursorKind.VAR_DECL and cursor.is_definition()
            and cursor.location.file is not None
            and Path(str(cursor.location.file)).resolve() == path.resolve()]
        starts = sorted(cursor.extent.start.offset for cursor in variables)
        rows = []
        for cursor in variables:
            parent = cursor.semantic_parent
            start = cursor.extent.start.offset
            try:
                end = _declaration_end(masked, start)
            except ValueError:
                end = start
            line_start = blob.rfind(b"\n", 0, start) + 1
            element = cursor.type
            element_size = 0
            while element.kind == ci.TypeKind.CONSTANTARRAY:
                element = element.element_type
                element_size = element.get_size()
            rows.append({
                "unit": unit,
                # Clang folds DATA()'s annotate attribute into the VarDecl extent,
                # so an already-claimed definition looks like any other; only the
                # marker token itself distinguishes it.
                "marked": blob[start:start + 5] == b"DATA(",
                "file": str(path.relative_to(REPO)),
                "name": cursor.spelling,
                "symbol": cursor.mangled_name,
                "scope": parent.kind.name if parent is not None else "?",
                "static": cursor.storage_class == ci.StorageClass.STATIC,
                "size": cursor.type.get_size(),
                "element_size": max(element_size, 0),
                "type": cursor.type.spelling,
                "start": start,
                "end": end,
                "line": blob.count(b"\n", 0, start) + 1,
                "line_prefix_blank": blob[line_start:start].strip() == b"",
                "siblings_in_span": sum(1 for s in starts if start <= s < end),
            })
        files[unit] = {"digest": digest, "rows": rows}
    if cache_path:
        cache_path.parent.mkdir(parents=True, exist_ok=True)
        cache_path.write_text(json.dumps(
            {"schema": VARDECL_CACHE_SCHEMA, "files": files}, indent=1) + "\n")
    return [row for entry in files.values() for row in entry["rows"]]


def derive_proposals(functions, rows, image: RetailImage):
    """Vote every exactly-reproduced function's DIR32 sites into owner addresses.

    Votes are counted per `(unit, symbol)`. A file-static's linker name is not
    unique in the program -- ten icon decoders each define their own `s_dst` --
    and pooling their sites would hand every one of them the same address.
    """
    taken = {row["rva"]: row["name"] for row in rows if row["kind"] == "func"}
    landed = {row["rva"]: row["name"] for row in rows
              if row["kind"] == "data" and row["provenance"] == "source-annotation"}
    votes = defaultdict(Counter)
    defined = {}
    donors = 0
    for unit, claims in sorted(functions.items()):
        if unit.startswith("("):
            continue
        obj = BASE_OBJS / f"{unit}.obj"
        if not obj.exists():
            continue
        coff = CoffFile(str(obj))
        symbols = data_symbols(coff, unit)
        internal = {name for name, record in symbols.items() if record["internal"]}
        for name, record in symbols.items():
            key = (unit if record["internal"] else "", name)
            previous = defined.get(key)
            if previous is not None and previous["unit"] != unit:
                previous["ambiguous"] = True
                continue
            record.setdefault("ambiguous", False)
            defined[key] = record
        bodies = function_bodies(coff)
        for rva, size, name in claims:
            body = bodies.get(name)
            if body is None:
                continue
            ours, sites = body
            retail = image.payload(rva, size)
            if retail is None or not masked_equal(ours, retail, sites):
                continue
            donors += 1
            for symbol, owner in owner_votes(retail, sites, image.extent):
                votes[(unit if symbol in internal else "", symbol)][owner] += 1

    proposals = []
    for (owner_unit, symbol), counter in sorted(votes.items()):
        record = defined.get((owner_unit, symbol))
        if record is None or symbol.startswith(SKIP_PREFIX):
            continue
        ranked = counter.most_common()
        rva = ranked[0][0]
        verdict, span = payload_verdict(record, rva, image)
        proposals.append({
            "symbol": symbol, "unit": record["unit"], "rva": rva,
            "size": record["size"], "section": record["section"],
            "storage": image.storage(rva), "votes": ranked[0][1],
            "total": sum(counter.values()), "distinct": len(ranked),
            "payload": verdict, "diff_span": span,
            "ambiguous_unit": record["ambiguous"],
            "collides_function": taken.get(rva),
            "collides_claim": landed.get(rva) if landed.get(rva) != symbol else None,
            "ranked": [[owner, count] for owner, count in ranked],
        })
    return proposals, donors


def claimed_payloads(functions, rows):
    """Symbol -> our object's payload, for the units that own claimed data."""
    wanted = defaultdict(set)
    for row in rows:
        if row["kind"] == "data" and row["provenance"] == "source-annotation":
            wanted[row["unit"]].add(row["name"])
    out = {}
    for unit, names in sorted(wanted.items()):
        obj = BASE_OBJS / f"{unit}.obj"
        if not obj.exists():
            continue
        for name, record in data_symbols(CoffFile(str(obj)), unit).items():
            if name in names:
                out[(unit, name)] = record
    return out


# --------------------------------------------------------------------------
# commands
# --------------------------------------------------------------------------

def run_derive(args) -> int:
    image = RetailImage.load()
    functions, rows = read_inventory()
    proposals, donors = derive_proposals(functions, rows, image)
    vardecls = source_vardecls(
        None if args.no_cache else GEN / "data_claim_vardecls.json",
        refresh=args.refresh)
    claimed = {row["name"] for row in rows
               if row["kind"] == "data" and row["provenance"] == "source-annotation"}
    fresh = [proposal for proposal in proposals if proposal["symbol"] not in claimed]
    accepted, rejected = select_claims(fresh, vardecls)

    GEN.mkdir(parents=True, exist_ok=True)
    (GEN / "data_claims.json").write_text(json.dumps(accepted, indent=1) + "\n")
    (GEN / "data_claims_rejected.json").write_text(json.dumps(rejected, indent=1) + "\n")
    print("donor functions: %d; proposals: %d (%d already claimed)"
          % (donors, len(proposals), len(proposals) - len(fresh)))
    print("accepted %d claims in %d files; rejected %d"
          % (len(accepted), len({row["file"] for row in accepted}), len(rejected)))
    print("payload:", dict(Counter(row["payload"] for row in accepted)))
    for reason, count in Counter(row["reason"] for row in rejected).most_common():
        print("  reject %-4d %s" % (count, reason))
    for row in accepted[:args.show]:
        print("  0x%08x %-6s %-46s %s" % (row["rva"] + IMAGE_BASE, row["payload"],
                                          row["symbol"], row["file"]))
    if not args.write:
        print("(report only; --write inserts the markers)")
        return 0
    by_file = defaultdict(list)
    for claim in accepted:
        by_file[claim["file"]].append(claim)
    written = 0
    for name, claims in sorted(by_file.items()):
        path = REPO / name
        path.write_bytes(insert_markers(path.read_bytes(), claims))
        written += len(claims)
    print("inserted %d DATA markers in %d files" % (written, len(by_file)))
    return 0


def element_sizes(vardecls) -> dict:
    """`(unit, linker name)` -> innermost array element size, for claimed arrays.

    The inventory records the type-derived byte size only, so an overrun cannot
    be read as "one element" without this. A non-array definition contributes
    nothing and is scored on bytes instead.
    """
    out = {}
    for row in vardecls:
        if row.get("element_size"):
            out[(row["unit"], row["symbol"])] = row["element_size"]
    return out


def run_extents(args) -> int:
    image = RetailImage.load()
    functions, rows = read_inventory()
    claims = [row for row in rows if row["kind"] == "data"
              and row["provenance"] == "source-annotation"]
    # Only a reviewed claim bounds a neighbour; see proven_extent.
    starts = sorted({row["rva"] for row in claims}
                    | {row["rva"] for row in rows if row["kind"] == "func"})
    labels = {row["rva"]: row["name"] for row in rows}
    payloads = claimed_payloads(functions, rows)
    elements = element_sizes(source_vardecls(
        None if args.no_cache else GEN / "data_claim_vardecls.json",
        refresh=args.refresh))
    findings = []
    for row in claims:
        rva, size = row["rva"], row["size"]
        index = bisect.bisect_right(starts, rva)
        next_rva = starts[index] if index < len(starts) else None
        record = payloads.get((row["unit"], row["name"]))
        ours = record["payload"] if record else None
        neighbour = size if next_rva is None else next_rva - rva
        proven = proven_extent(size, ours, image.payload(rva, size),
                               record["pointers"] if record else (), neighbour)
        verdict, overrun = extent_finding(
            size, proven, ours, elements.get((row["unit"], row["name"]), 0))
        if verdict == "ok":
            continue
        findings.append({
            "rva": rva, "size": size, "symbol": row["name"], "unit": row["unit"],
            "verdict": verdict, "overrun": overrun, "proven": proven,
            "neighbour": neighbour, "next": labels.get(next_rva),
            "element_size": elements.get((row["unit"], row["name"]), 0),
        })
    findings.sort(key=lambda row: (row["verdict"], -row["overrun"],
                                   row["unit"], row["rva"]))
    GEN.mkdir(parents=True, exist_ok=True)
    (GEN / "data_claim_extents.json").write_text(json.dumps(findings, indent=1) + "\n")
    print("claims audited: %d; contradicted: %d" % (len(claims), len(findings)))
    for verdict, count in Counter(row["verdict"] for row in findings).most_common():
        print("  %-24s %d" % (verdict, count))
    print("overrun bytes:", dict(Counter(row["overrun"] for row in findings
                                         if row["verdict"] != "nonzero-tail")))
    for row in findings:
        if args.verdict and row["verdict"] != args.verdict:
            continue
        print("  0x%08x %-20s over=%-5d ours=%-5d retail<=%-5d %-44s next=%s"
              % (row["rva"] + IMAGE_BASE, row["verdict"], row["overrun"],
                 row["size"], row["proven"], row["symbol"], row["next"]))
    return 0


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(
        prog="homm2 audit data-claims", description=__doc__.splitlines()[0])
    sub = parser.add_subparsers(dest="command")
    derive = sub.add_parser("derive", help="derive DATA() addresses (default)")
    derive.add_argument("--write", action="store_true",
                        help="insert the accepted markers into src/")
    derive.add_argument("--show", type=int, default=0,
                        help="print the first N accepted claims")
    derive.add_argument("--refresh", action="store_true",
                        help="reparse every source file instead of using the cache")
    derive.add_argument("--no-cache", action="store_true",
                        help="neither read nor write the vardecl cache")
    derive.set_defaults(handler=run_derive)
    extents = sub.add_parser(
        "extents", help="audit claimed extents against the next claimed address")
    extents.add_argument("--verdict", help="print only rows with this verdict")
    extents.add_argument("--refresh", action="store_true",
                         help="reparse every source file instead of using the cache")
    extents.add_argument("--no-cache", action="store_true",
                         help="neither read nor write the vardecl cache")
    extents.set_defaults(handler=run_extents)

    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv or argv[0].startswith("-"):
        argv = ["derive", *argv]
    args = parser.parse_args(argv)
    return args.handler(args)


if __name__ == "__main__":
    raise SystemExit(main())
