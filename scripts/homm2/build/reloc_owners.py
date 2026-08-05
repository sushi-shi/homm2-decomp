"""Reviewed data-owner ranges and relocation sites for the stripped target.

The generated symbol inventory contains synthetic ``const_<RVA>`` aliases for
reviewed relocation targets.  Some aliases point into a real reconstructed
global.  A relocation to such an alias is better represented as the owner plus
an implicit COFF addend, for example ``gConfig + 0x30``.

Ownership is a reviewed claim: a data symbol is accepted as an owner only when
its source ``DATA()`` definition names the same RVA and an explicit extent is
recorded in config/reloc_data_owners.tsv.  The inventory's own sizes are not
ownership proof by themselves.

This module also owns the reader for config/delink_relocs.tsv - the reviewed
absolute-relocation site list that substitutes for the image's missing
base-relocation directory.
"""
import csv
import glob
import re
from pathlib import Path
from typing import NamedTuple


IMAGE_BASE = 0x400000
OWNER_EXTENTS = "config/reloc_data_owners.tsv"
RELOC_MANIFEST = "config/delink_relocs.tsv"


class DataOwner(NamedTuple):
    rva: int
    size: int
    symbol: str
    source_name: str


def load_reviewed_highlow_sites(path=None):
    """Reviewed absolute-relocation site RVAs (the image has no .reloc)."""
    path = Path(path or RELOC_MANIFEST)
    sites = []
    with path.open(newline="") as stream:
        for row in csv.DictReader(
                (line for line in stream if not line.lstrip().startswith("#")),
                delimiter="\t"):
            sites.append(int(row["site_rva"], 16))
    return sorted(sites)


def load_definition_rvas(source_glob="src/**/*.cpp"):
    definitions = {}
    pattern = re.compile(
        r"DATA\(0x([0-9a-fA-F]+)\).*?\b([A-Za-z_]\w*)\s*(?:\[|;|=)"
    )
    for path in glob.glob(source_glob, recursive=True):
        with open(path, encoding="latin-1") as stream:
            for line in stream:
                match = pattern.search(line)
                if match:
                    definitions[match.group(2)] = int(match.group(1), 16) - IMAGE_BASE
    return definitions


def load_explicit_extents(path=OWNER_EXTENTS):
    extents = {}
    with open(path, encoding="utf-8", newline="") as stream:
        for row in csv.reader((line for line in stream if not line.startswith("#")),
                              delimiter="\t"):
            if len(row) < 3:
                continue
            try:
                extents[row[0]] = (int(row[1], 0), int(row[2], 0))
            except ValueError:
                continue
    return extents


def _source_name(symbol):
    """The source identifier behind an inventory symbol name.

    The inventory carries linkage names: clang's MS mangling for C++
    (``?gConfig@@3...``) or a plain identifier for extern "C" objects
    (with or without the cdecl underscore).
    """
    match = re.match(r"\?([A-Za-z_]\w*)@@", symbol)
    if match:
        return match.group(1)
    match = re.fullmatch(r"_?([A-Za-z_]\w*)", symbol)
    return match.group(1) if match else None


def owners_from_rows(rows, definitions, extents):
    # A reviewed extent that names an address the source claim contradicts is a stale
    # row, not a missing owner: it silently switches the owner-extent rule off for the
    # one symbol it was written for. Say so instead of degrading quietly.
    for name, (rva, _size) in sorted(extents.items()):
        claimed = definitions.get(name)
        if claimed is not None and claimed != rva:
            raise ValueError(
                "%s records %s at 0x%x, but its DATA() definition claims 0x%x"
                % (OWNER_EXTENTS, name, rva, claimed))
    owners = []
    for row in rows:
        if row.get("kind") != "data":
            continue
        try:
            rva = int(row["rva"], 16)
            size = int(row.get("size") or "0", 16)
        except (KeyError, ValueError):
            continue
        source_name = _source_name(row.get("name", ""))
        if source_name is None:
            continue
        extent = extents.get(source_name)
        if (definitions.get(source_name) != rva or extent is None or
                extent[0] != rva or extent[1] <= 0):
            continue
        # A zero inventory size is "no claim"; a positive size smaller than the
        # reviewed extent contradicts it and disqualifies the owner.
        if 0 < size < extent[1]:
            continue
        owners.append(DataOwner(rva, extent[1], row["name"], source_name))
    return sorted(owners, key=lambda owner: (owner.rva, owner.size, owner.symbol))


def load_owner_ranges(symbols_path="build/gen/symbol_names.csv",
                      source_glob="src/**/*.cpp"):
    with open(symbols_path, encoding="latin-1", newline="") as stream:
        rows = list(csv.DictReader(stream))
    return owners_from_rows(
        rows, load_definition_rvas(source_glob), load_explicit_extents())


def owner_for_rva(owners, rva):
    """Return the most specific reviewed data owner containing ``rva``."""
    matches = [owner for owner in owners
               if owner.rva <= rva < owner.rva + owner.size]
    if not matches:
        return None
    return max(matches, key=lambda owner: (owner.rva, -owner.size))


def is_interior_reloc_alias(row, owners):
    if row.get("provenance") != "pe-reloc-constant":
        return False
    try:
        rva = int(row["rva"], 16)
    except (KeyError, ValueError):
        return False
    return owner_for_rva(owners, rva) is not None
