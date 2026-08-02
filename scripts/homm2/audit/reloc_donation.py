"""Recover DIR32 relocation sites and target identities by donation.

SUPERSEDED as the site channel by `homm2 audit reloc-sweep`, which recovers
every site this donates and ~26,000 more, from the image alone. Donation is
site-anchored only in appearance: it fires on a function whose compiled bytes
are already masked-identical to retail, so a site arrived *after* exactness
rather than enabling it, and the data sections -- where nothing is compiled to
compare against -- could never start at all.

What donation still uniquely provides is target *identity*. The sweep sees a
dword; this reads the symbol our own object relocated against, which is the
only channel that yields build/gen/reloc_target_names.tsv (unanimous data-owner
names), build/gen/string_cells.tsv (content-verified literal cells), and
config/delink_reloc_aliases.tsv (interior sites as owner + addend). Keep running
it as claims close: every newly exact function names targets the sweep has
already placed. Source DATA()/VTBL() markers will retire even that.

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


def string_data(coff) -> dict:
    """String symbol -> NUL-terminated bytes from the object's data sections.

    Covers both static $SG cells (/Gf off) and pooled ??_C@ COMDATs (/Gf on).
    """
    out = {}
    for sym in coff.symbols.values():
        if (not sym.name.startswith(("$SG", "??_C@"))) or sym.section <= 0:
            continue
        section = coff.sections[sym.section - 1]
        raw = bytes(coff.data[section.raw_offset + sym.value:
                              section.raw_offset + min(sym.value + 512,
                                                       section.raw_size)])
        cut = raw.find(b"\0")
        if cut >= 0:
            out[sym.name] = raw[:cut]
    return out


def pe_sections(exe: bytes):
    pe = struct.unpack_from("<I", exe, 0x3C)[0]
    count = struct.unpack_from("<H", exe, pe + 6)[0]
    optional = struct.unpack_from("<H", exe, pe + 20)[0]
    out = []
    for index in range(count):
        offset = pe + 24 + optional + index * 40
        vsize, va, rsize, roff = struct.unpack_from("<4I", exe, offset + 8)
        out.append((va, min(vsize, rsize), roff))
    return out


def retail_cstring(exe: bytes, secs, rva: int):
    for va, size, roff in secs:
        if va <= rva < va + size:
            raw = exe[roff + rva - va:roff + rva - va + 512]
            cut = raw.find(b"\0")
            return raw[:cut] if cut >= 0 else None
    return None


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

    secs = pe_sections(exe)
    donated = {}
    named = {}
    interior = []
    string_cells = {}
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
        strings = string_data(coff)
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
                if (symbol or "").startswith(("$SG", "??_C@")) and addend == 0:
                    # string-content evidence: the retail cell holds exactly
                    # the literal our object compiled -> the cell is a
                    # string even if its first dword happens to parse as an
                    # in-image pointer (e.g. "ATA").
                    ours_text = strings.get(symbol)
                    cell_rva = target - IMAGE_BASE
                    if (ours_text is not None and
                            retail_cstring(exe, secs, cell_rva) == ours_text):
                        string_cells[cell_rva] = True
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

    cells_path = REPO / "build/gen/string_cells.tsv"
    with cells_path.open("w") as stream:
        print("cell_rva", file=stream)
        for cell_rva in sorted(string_cells):
            print(f"0x{cell_rva:x}", file=stream)
    print(f"[reloc-donation] {len(string_cells)} content-verified string "
          f"cells -> {cells_path}")

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
