"""Shared encoding for function-scoped public-owner relocation aliases."""

import csv
from pathlib import Path
from typing import NamedTuple


class RelocAlias(NamedTuple):
    function_rva: int
    target_rva: int
    owner: str
    addend: int
    occurrences: int
    provenance: str

def load_aliases(path):
    aliases = []
    seen = set()
    with open(path, encoding="latin-1") as stream:
        for row in csv.DictReader(stream, delimiter="\t"):
            alias = RelocAlias(
                int(row["function_rva"], 0), int(row["target_rva"], 0), row["owner"],
                int(row["addend"], 0), int(row["occurrences"]),
                row["provenance"])
            key = (alias.function_rva, alias.target_rva, alias.owner, alias.addend)
            if key in seen:
                raise ValueError(
                    "duplicate alias row for function RVA 0x%x target RVA 0x%x" %
                    (alias.function_rva, alias.target_rva))
            seen.add(key)
            aliases.append(alias)
    return aliases


def write_aliases(path, aliases):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="latin-1", newline="") as stream:
        writer = csv.writer(stream, delimiter="\t", lineterminator="\n")
        writer.writerow((
            "function_rva", "target_rva", "owner", "addend", "occurrences", "provenance"))
        for alias in sorted(aliases):
            writer.writerow((
                "0x%08x" % alias.function_rva,
                "0x%08x" % alias.target_rva, alias.owner,
                "0x%08x" % (alias.addend & 0xFFFFFFFF),
                alias.occurrences, alias.provenance))
