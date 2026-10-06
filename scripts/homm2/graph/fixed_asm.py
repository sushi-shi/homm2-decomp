"""The two proven MASM translation units and their fixed retail claims.

This is deliberately not an assembly annotation framework.  BITS and TILE are
the only reconstructed units retail assembled from MASM, so their four code
claims and TILE's scratch allocation live in one small, reviewed table.
"""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class FixedAsmClaim:
    rva: int
    size: int
    name: str
    kind: str


@dataclass(frozen=True)
class FixedAsmUnit:
    source: str
    claims: tuple[FixedAsmClaim, ...]


UNITS = {
    "BASE/BITS": FixedAsmUnit(
        source="src/BASE/BITS.asm",
        claims=(
            FixedAsmClaim(0x0C2ED4, 0x2E, "_BitTest", "func"),
            FixedAsmClaim(0x0C2F02, 0x20, "_BitSet", "func"),
            FixedAsmClaim(0x0C2F22, 0x22, "_BitClear", "func"),
        ),
    ),
    "BASE/TILE": FixedAsmUnit(
        source="src/BASE/TILE.asm",
        claims=(
            FixedAsmClaim(0x0C2554, 0x18F, "_TileToBitmap", "func"),
            FixedAsmClaim(0x11F2EC, 0x08, "_gTileScratch", "data"),
        ),
    ),
}


def unit(unit: str, configured_source: str | None = None) -> FixedAsmUnit | None:
    """Return a fixed unit and reject manifest drift when one is configured."""
    result = UNITS.get(unit)
    if result is not None and configured_source is not None:
        if Path(configured_source).as_posix() != result.source:
            raise ValueError(
                f"{unit} must use fixed MASM source {result.source}, got "
                f"{configured_source}"
            )
    return result


def claims(kind: str | None = None):
    for unit_name, record in UNITS.items():
        for claim in record.claims:
            if kind is None or claim.kind == kind:
                yield unit_name, record.source, claim
