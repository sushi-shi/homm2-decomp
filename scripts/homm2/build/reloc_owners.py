"""Public-data owner ranges used to normalize interior COFF relocations.

The generated symbol inventory contains synthetic ``const_<RVA>`` aliases for PE
base-relocation targets.  Some aliases point into a real reconstructed global.  A
relocation to such an alias is better represented as the public owner plus an
implicit COFF addend, for example ``gConfig + 0x30``.

Only CodeView public data symbols with a matching source ``DATA()`` definition and
an explicit recovered extent are accepted as owners.  The inventory's data sizes are
often provisional next-public gaps, so they are not ownership proof by themselves.
"""
import csv
import glob
import re
from typing import NamedTuple


IMAGE_BASE = 0x400000
OWNER_EXTENTS = "config/reloc_data_owners.tsv"


class DataOwner(NamedTuple):
    rva: int
    size: int
    symbol: str
    source_name: str


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


def owners_from_rows(rows, definitions, extents):
    owners = []
    for row in rows:
        if (row.get("kind") != "data" or
                row.get("provenance") != "cv-public-data"):
            continue
        try:
            rva = int(row["rva"], 16)
            size = int(row.get("size") or "0", 16)
        except (KeyError, ValueError):
            continue
        match = re.match(r"\?([A-Za-z_]\w*)@@", row.get("name", ""))
        if size <= 0 or match is None:
            continue
        source_name = match.group(1)
        extent = extents.get(source_name)
        if (definitions.get(source_name) != rva or extent is None or
                extent[0] != rva or extent[1] <= 0 or size < extent[1]):
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
    """Return the most specific recovered public-data owner containing ``rva``."""
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
