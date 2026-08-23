#!/usr/bin/env python3
"""The reviewed pre-link COFF transforms, applied only by `homm2 link --transform`.

Each transform permutes section headers of an otherwise untouched compiler
object; no section payload byte is ever synthesized or edited. Every entry is
an object-input reconstruction debt with a completed impossibility proof: the
retail emission order is unreachable from the pinned toolchain by any compiler
flag, undocumented front-end option, pragma, environment, PCH/IDE state, or
source arrangement. See docs/compiler-re-allocation-order.md for the front-end
mechanism and the per-unit ledgers for the measured matrices.

`homm2 link` (generic) and `homm2 link --rsrc` never consult this module; they
link the raw compiled objects exactly as ninja produced them.
"""

from __future__ import annotations

from homm2.build.adapt_comdat_link_order import adapt as _adapt_comdat_order
from .adapt_misc_data import adapt as _adapt_misc_data


def _misc_track_section(payload: bytes) -> bytes:
    """BASE/Misc: split the Buka track-name literal out of the leading .data
    section into its own 0x1e-byte section between IsCDDrive's cell group and
    DriveSupportsFreeSpaceQuery (retail section 44).

    Wall: VC6 emits data cells in three fixed phase groups; a file-scope
    initializer literal cannot be created by function-context codegen, which is
    the only emitter that reaches this position.
    Ledger: docs/matching/Misc-track-name-data/reopened-data-segment.cpp
    """
    return _adapt_misc_data(payload)


def _audiere_node_dtor_move(payload: bytes) -> bytes:
    """BASE/AudiereEffects: move the ??1AudiereSampleNode COMDAT section after
    _$E20, the retail tail slot.

    Wall: the destructor's first requirement (Purge's delete, function 1) is
    satisfied in the first drain of the emission fixpoint loop; the only
    post-$E channel carries pending template specializations, never a
    user-called COMDAT.
    Ledger: docs/matching/AudiereSampleNode-destructor/ownership-forms.cpp
    """
    return _adapt_comdat_order(payload, "BASE/AudiereEffects")


def _dimmer_deleting_dtor_move(payload: bytes) -> bytes:
    """BASE/DIMMER: move the ??_GdimmerWidget COMDAT section from its
    first-constructor-adjacent slot to immediately before ??1dimmerWidget.

    Wall: the ??_7/??_G pair is generated eagerly and atomically at the first
    vtable requirement, which a constructor always is; no reachable state
    separates them.
    Ledger: docs/matching/dimmerWidget-destructor/comdat-order-matrix.cpp
    """
    return _adapt_comdat_order(payload, "BASE/DIMMER")


TRANSFORMS = {
    "BASE/Misc": _misc_track_section,
    "BASE/AudiereEffects": _audiere_node_dtor_move,
    "BASE/DIMMER": _dimmer_deleting_dtor_move,
}


def apply(unit: str, payload: bytes) -> bytes:
    """Apply the reviewed transform for `unit` to a raw compiled object."""
    return TRANSFORMS[unit](payload)
