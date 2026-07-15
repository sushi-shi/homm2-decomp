"""Shared encoding for paired public-owner relocation aliases."""

import csv
from pathlib import Path
from typing import NamedTuple


ALIAS_PREFIX = "__homm2_data_alias$"


class RelocAlias(NamedTuple):
    target_rva: int
    owner: str
    addend: int
    proofs: int
    alternatives: int


def encode_pdb_alias(alias):
    return "%s%08X$%s" % (
        ALIAS_PREFIX, alias.addend & 0xFFFFFFFF, alias.owner)


def load_aliases(path):
    aliases = {}
    with open(path, encoding="latin-1") as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            alias = RelocAlias(
                int(row["target_rva"], 0), row["owner"],
                int(row["addend"], 0), int(row["proofs"]),
                int(row["alternatives"]))
            previous = aliases.setdefault(alias.target_rva, alias)
            if previous != alias:
                raise ValueError("conflicting alias rows for RVA 0x%x" %
                                 alias.target_rva)
    return aliases


def write_aliases(path, aliases):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="latin-1", newline="") as stream:
        writer = csv.writer(stream, delimiter="\t", lineterminator="\n")
        writer.writerow((
            "target_rva", "owner", "addend", "proofs", "alternatives"))
        for alias in sorted(aliases.values()):
            writer.writerow((
                "0x%08x" % alias.target_rva, alias.owner,
                "0x%08x" % (alias.addend & 0xFFFFFFFF),
                alias.proofs, alias.alternatives))
