"""Recover source-owned primary and secondary vtable identities."""

from __future__ import annotations

import re
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

from homm2.build.annotated_functions import _mask_lexical_noise


IMAGE_BASE = 0x400000
IDENTIFIER = rb"[A-Za-z_][A-Za-z0-9_]*"
PRIMARY = re.compile(
    rb"^[ \t]*VTBL\(\s*(" + IDENTIFIER + rb")\s*,\s*"
    rb"(0x[0-9a-fA-F]+)\s*\)", re.M)
SECONDARY = re.compile(
    rb"^[ \t]*VTBL2\(\s*(" + IDENTIFIER + rb")\s*,\s*(" + IDENTIFIER +
    rb")\s*,\s*(0x[0-9a-fA-F]+)\s*\)", re.M)


@dataclass(frozen=True, order=True)
class AnnotatedVtable:
    unit: str
    rva: int
    derived: str
    base: str | None
    mangled_name: str
    location: str


def _mangled_name(derived: str, base: str | None) -> str:
    if base is None:
        return f"??_7{derived}@@6B@"
    return f"??_7{derived}@@6B{base}@@@"


def source_vtables(source_root: Path, repo: Path) -> list[AnnotatedVtable]:
    source_root = Path(source_root).resolve()
    repo = Path(repo).resolve()
    rows = []
    for path in sorted(source_root.rglob("*.cpp")):
        blob = path.read_bytes()
        masked = _mask_lexical_noise(blob)
        unit = path.relative_to(source_root).with_suffix("").as_posix()
        matches = [
            (match.start(), match, False) for match in PRIMARY.finditer(masked)
        ]
        matches.extend(
            (match.start(), match, True) for match in SECONDARY.finditer(masked))
        for offset, match, secondary in sorted(matches):
            derived = match.group(1).decode("ascii")
            base = match.group(2).decode("ascii") if secondary else None
            va_group = 3 if secondary else 2
            va = int(match.group(va_group), 16)
            line = blob.count(b"\n", 0, offset) + 1
            if va < IMAGE_BASE:
                raise ValueError(f"{path}:{line}: invalid vtable VA")
            rows.append(AnnotatedVtable(
                unit=unit,
                rva=va - IMAGE_BASE,
                derived=derived,
                base=base,
                mangled_name=_mangled_name(derived, base),
                location=f"{path.relative_to(repo).as_posix()}:{line}",
            ))
    duplicates = sorted(
        name for name, count in Counter(row.mangled_name for row in rows).items()
        if count > 1)
    if duplicates:
        raise ValueError("duplicate source vtable identities: " + ", ".join(duplicates))
    return sorted(rows, key=lambda row: (row.unit, row.rva, row.mangled_name))
