"""Recover DIR32 relocation sites for config/delink_relocs.tsv by donation.

The stripped target has no base-relocation directory, so the manifest is the
only DIR32 channel. Every claimed function whose compiled bytes are
masked-identical to the retail span at the same length donates its own
relocation sites: the compiled object records where each DIR32 field sits,
the offsets transfer one-to-one onto the retail body, and the retail dword at
each site must be a linked VA inside the image. A row is emitted only when
all three hold, so the output stays a reviewed list rather than a heuristic
sweep (the Ghidra-reference and aligned-VA channels cover what donation
cannot reach: unclaimed code and the data segment).

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


def function_bodies(obj_path: Path):
    """name -> (bytes, [(offset, type, symbol, addend)]) per function."""
    coff = CoffFile(str(obj_path))
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
        bodies = function_bodies(obj_path)
        for rva, size, name in rows:
            body = bodies.get(name)
            if body is None:
                continue
            ours, sites = body
            retail = exe[rva:rva + size]
            if not masked_equal(ours, retail, sites):
                continue
            dir32 = [entry for entry in sites
                     if entry[1] == IMAGE_DIR32 and entry[0] + 4 <= size
                     and not (entry[2] or "").startswith(
                         ("__ehhandler", "__unwindfunclet", "__catch"))]
            keep = True
            fn_sites = []
            for off, _kind, symbol, addend in dir32:
                target = struct.unpack_from("<I", retail, off)[0]
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
                # TU-local artifacts never vote: $L jump-table/branch labels
                # (their name is a compile accident and planting them as PDB
                # data owners re-splits the delinked table relocs), string and
                # float literals, and per-function EH funclet labels.
                if symbol and not symbol.startswith(
                        ("__real@", "??_C@", "$SG", "$L",
                         "__ehhandler", "__unwindfunclet", "__catch")):
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

    # target-name evidence: unanimous owners become real names for the
    # synthetic manifest-target rows (const_<RVA> otherwise). Distinct owners
    # sharing one spelling (per-TU statics with plain C names) get an @<rva>
    # suffix so the PDB stays one-name-one-address.
    names_path = REPO / "build/gen/reloc_target_names.tsv"
    solo = {rva: entries for rva, votes in named.items()
            if len(entries := sorted(votes.items(), key=lambda kv: -kv[1])) == 1}
    by_name = {}
    for rva, entries in solo.items():
        by_name.setdefault(entries[0][0], []).append(rva)
    owner_name = {}
    for name, rvas in by_name.items():
        for rva in rvas:
            owner_name[rva] = name if len(rvas) == 1 else f"{name}@0x{rva:x}"
    with names_path.open("w") as stream:
        print("owner_rva\tsymbol\tvotes", file=stream)
        for owner_rva in sorted(named):
            if owner_rva in owner_name:
                votes = solo[owner_rva][0][1]
                print(f"0x{owner_rva:x}\t{owner_name[owner_rva]}\t{votes}",
                      file=stream)
            else:
                entries = sorted(named[owner_rva].items(), key=lambda kv: -kv[1])
                spellings = "|".join(f"{n}:{v}" for n, v in entries)
                print(f"0x{owner_rva:x}\t(conflict)\t{spellings}", file=stream)
    print(f"[reloc-donation] {len(owner_name)} unanimous data-owner names -> "
          f"{names_path}")

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
