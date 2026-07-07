#ifndef HOMM2_GLOBALS_MODEL_H
#define HOMM2_GLOBALS_MODEL_H
#include <va.h>
// Synthetic / MODELLING globals — readable aliases the matcher introduced for data that has
// NO CodeView symbol of its own: overlay views of the monster-database region, and .rdata
// float literals (MSVC-hashed ??_C@ names) referenced as named consts for reloc control.
// They DO have retail addresses; those marked DATA-PENDING are pinned from the reloc when
// their user function is byte-matched (they alias object-local literals / computed bases).
// Kept OUT of _globals.h (which is CodeView-backed) — assert_globals_data exempts these.
#include <_types.h>

// VAs pinned from the retail disassembly of the referencing fns (no CodeView symbol of their own):
DATA(0x004faeb8) extern SCreatureInfo gCreatureInfo[];   // monster-DB view (game::ExperienceValueOfStack)
DATA(0x004faeb2) extern monsterRV gMonsterInfo[100];     // monster-DB view, 26B recs (philAI::ComputeUpgradeValue)
// (gRVWeightOre/Merc/Crystal removed — they were just gafAITurnCostResource[2|1|4]; the code
//  now indexes the array directly via the RES_* enum, which matches the retail byte-for-byte.)


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
DATA(0x00528d38) extern int const_00128d38;            // 13-byte BSS flag buffer (??_C@_0N@PMOM@...)

// TILE.cpp inline-asm scratch (no CodeView symbol): the blitter parks the mode word and a
// mirror-path row counter in static storage between its __asm sections.
DATA(0x0051fec0) extern unsigned int gTileMode;
DATA(0x0051fec4) extern int gTileRowCtr;

#endif // HOMM2_GLOBALS_MODEL_H
