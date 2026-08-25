"""Cross-check DIR32 relocation sites from already exact functions.

SUPERSEDED as the site channel by `homm2 audit reloc-sweep`, which recovers
every site this donates and ~26,000 more, from the image alone. Donation is
site-anchored only in appearance: it fires on a function whose compiled bytes
are already masked-identical to retail, so a site arrived *after* exactness
rather than enabling it, and the data sections -- where nothing is compiled to
compare against -- could never start at all.

Donation is no longer a build identity channel. Source claims, PE imports and
reviewed compiler-generated providers feed the symbol inventory directly.
This command remains a report and an optional cross-check for reviewed DIR32
sites and interior owner/addend aliases.

Every claimed function whose compiled bytes are masked-identical to the retail
span at the same length donates its own relocation sites: the compiled object
records where each DIR32 field sits, the offsets transfer one-to-one onto the
retail body, and the retail dword at each site must be a linked VA inside the
image. A row is emitted only when all three hold.

`--write` still merges sites into the manifest, which is now a no-op against a
freshly swept one; it stays because a non-empty merge means the sweep lost a
site and a rule needs looking at.

Usage:
    python3 -m homm2.audit.reloc_donation            # report only
    python3 -m homm2.audit.reloc_donation --write    # merge into the manifest
"""
from __future__ import annotations

import argparse
import csv
import struct
from pathlib import Path

from homm2.audit.unmatched_census import (
    FUNCTION_TYPE, MEM_EXECUTE, CoffFile, function_spans)
from homm2.core.paths import REPO

EXE = REPO / "build/orig/HMM2PL.exe"
SYMBOLS = REPO / "build/gen/symbol_names.csv"
BASE_OBJS = REPO / "build/objdiff/base"
MANIFEST = REPO / "config/delink_relocs.tsv"
ALIASES = REPO / "config/delink_reloc_aliases.tsv"

IMAGE_BASE = 0x400000
IMAGE_DIR32 = 6
IMAGE_REL32 = 20


def is_donatable_owner(symbol: str | None) -> bool:
    """Whether a relocation target spelling can identify durable storage.

    A leading ``$`` is MSVC's namespace for translation-unit-local compiler
    labels.  Some labels are descriptive (rather than the familiar ``$L`` or
    ``$SG`` forms), but they are still compile accidents and must never become
    global identities in the reconstructed PDB.
    """
    return bool(symbol) and not symbol.startswith((
        "$", "__real@", "??_C@", "__ehhandler", "__unwindfunclet",
        "__catch",
    ))


def load_claims():
    claims = {}
    with SYMBOLS.open() as stream:
        for row in csv.DictReader(stream):
            if row["kind"] != "func":
                continue
            claims.setdefault(row["unit"], []).append(
                (int(row["rva"], 16), int(row["size"], 16), row["name"]))
    return claims


def image_extent(exe: bytes) -> int:
    """Highest mapped VA (SizeOfImage from the optional header)."""
    pe = struct.unpack_from("<I", exe, 0x3C)[0]
    return IMAGE_BASE + struct.unpack_from("<I", exe, pe + 0x50)[0]


def function_bodies(obj_path: Path, coff=None):
    """name -> (bytes, [(offset, type, symbol, addend)]) per function."""
    coff = coff or CoffFile(str(obj_path))
    first_header = 20 + struct.unpack_from("<H", coff.data, 16)[0]
    out = {}
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
        payload = bytes(
            coff.data[section.raw_offset:section.raw_offset + section.raw_size])
        section_relocs = []
        for (index, site), reloc in coff.relocations.items():
            if index != section.index:
                continue
            target = coff.symbols.get(reloc.symbol_index)
            addend = struct.unpack_from("<i", payload, site)[0] \
                if site + 4 <= len(payload) else 0
            # COFF DIR32 fields are symbol-relative addends for defined and
            # external symbols alike (verified against EXEC.obj/globalCrc);
            # the raw field is the addend. One symbol voting several owner
            # RVAs therefore signals a layout divergence, not a read error.
            section_relocs.append(
                (site, reloc.typ, target.name if target else None, addend))
        section_relocs.sort()
        for symbol, start, end in function_spans(symbols, section.raw_size):
            local = [(site - start, kind, name, addend)
                     for site, kind, name, addend in section_relocs
                     if start <= site < end]
            out[symbol.name] = (payload[start:end], local)
    return out


def masked_equal(ours: bytes, retail: bytes, sites) -> bool:
    if len(ours) < len(retail):
        return False
    ours = bytearray(ours[:len(retail)])
    retail = bytearray(retail)
    for offset, _kind, _symbol, _addend in sites:
        for k in range(offset, min(offset + 4, len(retail))):
            ours[k] = retail[k] = 0
    return bytes(ours) == bytes(retail)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write", action="store_true",
                        help="merge verified rows into config/delink_relocs.tsv")
    args = parser.parse_args(argv)

    exe = EXE.read_bytes()
    end_va = image_extent(exe)
    claims = load_claims()

    donated = {}
    named = {}
    interior = []
    functions_used = 0
    rejected_target = 0
    for unit, rows in sorted(claims.items()):
        if unit.startswith("("):
            continue
        obj_path = BASE_OBJS / f"{unit}.obj"
        if not obj_path.exists():
            continue
        coff = CoffFile(str(obj_path))
        bodies = function_bodies(obj_path, coff)
        for rva, size, name in rows:
            body = bodies.get(name)
            if body is None:
                continue
            ours, sites = body
            retail = exe[rva:rva + size]
            if not masked_equal(ours, retail, sites):
                continue
            dir32 = [entry for entry in sites
                     if entry[1] == IMAGE_DIR32 and entry[0] + 4 <= size]
            keep = True
            fn_sites = []
            for off, _kind, symbol, addend in dir32:
                target = struct.unpack_from("<I", retail, off)[0]
                if symbol == "__except_list":
                    # fs:[0] chain link: the operand is the absolute address
                    # of TIB slot zero, which is 0. Correct without a
                    # relocation record; never a manifest row, never a
                    # reason to reject the donor.
                    if target == 0:
                        continue
                    keep = False
                    rejected_target += 1
                    break
                if not IMAGE_BASE <= target < end_va:
                    keep = False
                    rejected_target += 1
                    break
                fn_sites.append((rva + off, target, symbol, addend))
            if not keep or not fn_sites:
                continue
            functions_used += 1
            for site, target, symbol, addend in fn_sites:
                donated[site] = target
                # TU-local artifacts never vote: all $-prefixed compiler
                # labels (including descriptive labels as well as $L/$SG),
                # string/float literals, and per-function EH funclet labels.
                if is_donatable_owner(symbol):
                    # our object stores the addend in the field, so the
                    # symbol's linked address is target - addend
                    owner_rva = target - IMAGE_BASE - addend
                    if not 0 <= owner_rva < end_va - IMAGE_BASE:
                        continue  # biased-pointer idiom, not a plain owner
                    named.setdefault(owner_rva, {}).setdefault(symbol, 0)
                    named[owner_rva][symbol] += 1
                    if addend:
                        interior.append(
                            (rva, target - IMAGE_BASE, site, owner_rva,
                             symbol, addend))

    print(f"[reloc-donation] {functions_used} masked-identical functions "
          f"donated {len(donated)} DIR32 sites "
          f"({rejected_target} functions rejected on out-of-image targets)")

    # Unanimous names are useful only for the optional owner/addend alias
    # cross-check below. They are never serialized or consumed by regeneration.
    solo = {rva: entries for rva, votes in named.items()
            if len(entries := sorted(votes.items(), key=lambda kv: -kv[1])) == 1}
    by_name = {}
    for rva, entries in solo.items():
        by_name.setdefault(entries[0][0], []).append(rva)
    owner_name = {}
    for name, rvas in by_name.items():
        for rva in rvas:
            owner_name[rva] = name if len(rvas) == 1 else f"{name}@0x{rva:x}"
    print(f"[reloc-donation] {len(owner_name)} unanimous data-owner names "
          "observed (report-only)")

    if not args.write:
        for site in sorted(donated)[:20]:
            print(f"   0x{site:06x} -> 0x{donated[site]:08x}")
        if len(donated) > 20:
            print(f"   ... {len(donated) - 20} more (use --write to merge)")
        return 0

    existing = set()
    header = []
    for line in MANIFEST.read_text().splitlines():
        if line.startswith("#") or line.startswith("site_rva"):
            header.append(line)
        elif line.strip():
            existing.add(int(line.split("\t")[0], 16))
    merged = existing | set(donated)
    with MANIFEST.open("w") as stream:
        for line in header:
            print(line, file=stream)
        if "site_rva" not in "\n".join(header):
            print("site_rva\tkind", file=stream)
        for site in sorted(merged):
            print(f"0x{site:x}\tdir32", file=stream)
    print(f"[reloc-donation] manifest now lists {len(merged)} sites "
          f"({len(merged) - len(existing)} new)")

    # interior sites: represent the field as owner + addend so the delinked
    # relocation reads exactly like our compiled one (symbol name and
    # displacement both visible). Only unanimous owners qualify.
    alias_rows = [(fn, tgt, site, owner_name[owner], addend)
                  for fn, tgt, site, owner, sym, addend in interior
                  if owner in owner_name]
    header_lines = []
    for line in ALIASES.read_text().splitlines():
        if line.startswith("#"):
            header_lines.append(line)
    with ALIASES.open("w") as stream:
        for line in header_lines:
            print(line, file=stream)
        print("function_rva\ttarget_rva\tsite_rva\towner\taddend\toccurrences",
              file=stream)
        for fn, tgt, site, sym, addend in sorted(alias_rows, key=lambda r: r[2]):
            print(f"0x{fn:x}\t0x{tgt:x}\t0x{site:x}\t{sym}\t0x{addend & 0xffffffff:x}\t1",
                  file=stream)
    print(f"[reloc-donation] {len(alias_rows)} interior sites aliased to "
          f"owner+addend -> {ALIASES}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
