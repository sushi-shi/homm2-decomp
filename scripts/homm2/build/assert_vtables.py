#!/usr/bin/env python3
"""Hard build gate for source-owned primary and secondary vtables."""

import csv
import sys
from collections import defaultdict
from pathlib import Path

from homm2.build.annotated_vtables import source_vtables
from homm2.build.data_manifest_adapter import candidate_topology


REPO = Path.cwd()
BASE = REPO / "build/objdiff/base"
SYMBOLS = REPO / "build/gen/symbol_names.csv"


def codeview_vtables():
    rows = {}
    with SYMBOLS.open(newline="", encoding="latin-1") as stream:
        for row in csv.DictReader(stream):
            name = row["name"]
            if row["kind"] != "data" or not name.startswith("??_7"):
                continue
            value = (row["unit"], int(row["rva"], 0))
            if name in rows:
                raise ValueError(f"duplicate CodeView vtable identity {name}")
            rows[name] = value
    return rows


def emitted_vtables():
    rows = defaultdict(list)
    for path in sorted(BASE.rglob("*.obj")):
        unit = path.relative_to(BASE).with_suffix("").as_posix()
        definitions, _sections = candidate_topology(path, unit)
        for definition in definitions:
            if definition.symbol.startswith("??_7"):
                rows[definition.symbol].append((unit, definition))
    return rows


def main() -> int:
    claims = source_vtables(REPO / "src", REPO)
    source = {claim.mangled_name: claim for claim in claims}
    codeview = codeview_vtables()
    emitted = emitted_vtables()
    bad = []

    # Retail CodeView is authoritative for every public vtable it records.
    for name, (unit, rva) in sorted(codeview.items()):
        claim = source.get(name)
        if claim is None:
            bad.append(f"A: CodeView vtable {name} ({unit}) has no source marker")
        elif (claim.unit, claim.rva) != (unit, rva):
            bad.append(
                f"A: {claim.location} places {name} at {claim.unit}+0x{claim.rva:x}; "
                f"CodeView says {unit}+0x{rva:x}")

    # Primary markers require CodeView. Secondary VTBL2 identities may be private.
    for name, claim in sorted(source.items()):
        if claim.base is None and name not in codeview:
            bad.append(f"B: {claim.location} primary {name} has no CodeView public")
        definitions = emitted.get(name, [])
        if len(definitions) != 1:
            bad.append(
                f"B: {claim.location} expected one emitted {name}, "
                f"found {len(definitions)}")
        elif definitions[0][0] != claim.unit:
            bad.append(
                f"B: {claim.location} owns {name}, but candidate emits it in "
                f"{definitions[0][0]}")

    # No candidate may emit an unmodeled primary or secondary vtable identity.
    for name, definitions in sorted(emitted.items()):
        if name not in source:
            owners = ", ".join(unit for unit, _definition in definitions)
            bad.append(f"C: candidate emits unmodeled vtable {name} in {owners}")

    for issue in bad:
        print("  " + issue)
    if bad:
        print(f"\nVTABLES FAIL: {len(bad)} issue(s).")
        return 1
    secondary = sum(claim.base is not None for claim in claims)
    print(
        f"vtables OK: {len(claims)} source-owned identities "
        f"({len(claims) - secondary} primary, {secondary} secondary)")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except ValueError as error:
        print(f"VTABLES FAIL: {error}")
        raise SystemExit(1)
