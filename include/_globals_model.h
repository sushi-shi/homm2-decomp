#ifndef HOMM2_GLOBALS_MODEL_H
#define HOMM2_GLOBALS_MODEL_H
// Synthetic / MODELLING globals — readable aliases the matcher introduced for data that has
// NO CodeView symbol of its own: overlay views of the monster-database region, and .rdata
// float literals (MSVC-hashed ??_C@ names) referenced as named consts for reloc control.
// They DO have retail addresses; those marked DATA-PENDING are pinned from the reloc when
// their user function is byte-matched (they alias object-local literals / computed bases).
// Kept OUT of _globals.h (which is CodeView-backed) — assert_globals_data exempts these.
#include <_types.h>

DATA(0x004faeb8) extern SCreatureInfo gCreatureInfo[];   // monster-DB view (game::ExperienceValueOfStack reloc)

// DATA-PENDING (matcher resolves exact VA when the referencing fn is matched):
extern monsterRV gMonsterInfo[100];   // monster-DB view (philAI::ComputeUpgradeValue, computed base)
extern float gRVWeightOre;            // .rdata float literal (??_C@_..), p[2] weight in philAI::RVConversion
extern float gRVWeightMerc;           // .rdata float literal, p[1] weight
extern float gRVWeightCrystal;        // .rdata float literal, p[4] weight

#endif // HOMM2_GLOBALS_MODEL_H
