#!/usr/bin/env python3
"""Read-only reviewed import ABI evidence for symbol and relocation audits.

Native links generate import libraries directly from imports/*.def. The former
LIB-output patching helpers and their generator CLI have been removed; this
module retains only identities consumed by comparison and symbol providers.
"""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ImportSpec:
    dll: str
    symbol: str
    ordinal_or_hint: int
    noname: bool = False
    lookup_name: str | None = None


# Preserve the reviewed import ABI and hint values. Hints are advisory to the
# loader, but retaining them makes each linked import record directly comparable.
MSS_IMPORTS = (
    ("_AIL_set_sample_loop_count@8", 119),
    ("_AIL_set_sample_playback_rate@8", 121),
    ("_AIL_start_sample@4", 142),
    ("_AIL_set_sample_type@12", 123),
    ("_AIL_sample_volume@4", 100),
    ("_AIL_set_sample_volume@8", 125),
    ("_AIL_midiOutOpen@12", 39),
    ("_AIL_startup@0", 146),
    ("_AIL_set_preference@8", 115),
    ("_AIL_waveOutOpen@16", 162),
    ("_AIL_last_error@0", 31),
    ("_AIL_redbook_tracks@4", 70),
    ("_AIL_redbook_track_info@16", 69),
    ("_AIL_redbook_close@4", 56),
    ("_AIL_redbook_open@4", 59),
    ("_AIL_init_sample@4", 29),
    ("_AIL_serve@0", 109),
    ("_AIL_stop_sequence@4", 149),
    ("_AIL_midiOutClose@4", 38),
    ("_AIL_release_sequence_handle@4", 88),
    ("_AIL_set_sequence_loop_count@8", 126),
    ("_AIL_sequence_status@4", 105),
    ("_AIL_resume_sequence@4", 91),
    ("_AIL_init_sequence@12", 30),
    ("_AIL_start_sequence@4", 143),
    ("_AIL_set_XMIDI_master_volume@8", 111),
    ("_AIL_shutdown@0", 140),
    ("_AIL_allocate_sequence_handle@4", 7),
    ("_AIL_allocate_sample_handle@4", 6),
    ("_AIL_sample_status@4", 98),
    ("_AIL_end_sample@4", 21),
    ("_AIL_set_sample_address@12", 116),
    ("_AIL_get_preference@4", 28),
)

SMACK_IMPORTS = (
    ("_SmackSoundUseDirectSound@4", 38),
    ("_SmackSoundUseMSS@4", 33),
    ("_SmackNextFrame@4", 21),
    ("_SmackToBufferRect@8", 28),
    ("_SmackDoFrame@4", 19),
    ("_SmackToBuffer@28", 23),
    ("_SmackClose@4", 18),
    ("_SmackOpen@12", 14),
    ("_SmackWait@4", 32),
    ("_SmackSummary@8", 20),
)

WING_IMPORTS = (
    ("_WinGCreateDC@0", 2, "WinGCreateDC"),
    ("_WinGCreateBitmap@12", 1, "WinGCreateBitmap"),
    ("_WinGRecommendDIBFormat@4", 7, "WinGRecommendDIBFormat"),
    ("_WinGSetDIBColorTable@16", 8, "WinGSetDIBColorTable"),
    ("_WinGStretchBlt@40", 9, "WinGStretchBlt"),
    ("_WinGBitBlt@32", 0, "WinGBitBlt"),
)

# With the bundled SDK ADVAPI32.LIB, final LINK resolves the object's
# __imp__RegCreateKeyA@12 reference as RegOpenKeyA. Build the small retail
# import set explicitly so startup retains RegCreateKeyA semantics as well as
# retail's ABI and hint values.
ADVAPI_IMPORTS = (
    ("_RegOpenKeyExA@20", 217, "RegOpenKeyExA"),
    ("_RegSetValueExA@24", 236, "RegSetValueExA"),
    ("_RegCreateKeyA@12", 197, "RegCreateKeyA"),
    ("_RegQueryValueExA@24", 225, "RegQueryValueExA"),
    ("_RegCloseKey@4", 194, "RegCloseKey"),
)


def import_specs() -> tuple[ImportSpec, ...]:
    specs = [ImportSpec("mss32.dll", symbol, hint) for symbol, hint in MSS_IMPORTS]
    specs.extend(
        ImportSpec("smackw32.DLL", symbol, ordinal, noname=True)
        for symbol, ordinal in SMACK_IMPORTS
    )
    specs.extend(
        ImportSpec("WING32.dll", symbol, hint, lookup_name=lookup)
        for symbol, hint, lookup in WING_IMPORTS
    )
    specs.extend(
        ImportSpec("ADVAPI32.dll", symbol, hint, lookup_name=lookup)
        for symbol, hint, lookup in ADVAPI_IMPORTS
    )
    return tuple(specs)
