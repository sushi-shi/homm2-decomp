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
#include <windows.h>
#include <SOURCE/comwin.h>

// VAs pinned from the retail disassembly of the referencing fns (no CodeView symbol of their own):
DATA(0x004faeb8) extern SCreatureInfo gCreatureInfo[];   // monster-DB view (game::ExperienceValueOfStack)
DATA(0x004faeb2) extern monsterRV gMonsterInfo[100];     // monster-DB view, 26B recs (philAI::ComputeUpgradeValue)
// The retail base includes the zero-level entry used by the one-based skill
// level index, so an expert value aliases the next row's leading entry.
#define HERO_SECONDARY_SKILL_VALUE_LEVELS 3
DATA(0x004fabf4) extern int gSecondarySkillRV[][HERO_SECONDARY_SKILL_VALUE_LEVELS];
DATA(0x004fdf52) extern signed char gCombatFormations[][17];
// (gRVWeightOre/Merc/Crystal removed — they were just gafAITurnCostResource[2|1|4]; the code
//  now indexes the array directly via the RES_* enum, which matches the retail byte-for-byte.)
DATA(0x0051ef24) extern char gDefaultCursorName[]; // default cursor resource name (mouseManager::SetColorMice)
DATA(0x00528dca) extern int gCdMusic; // CD-vs-MIDI music mode (dword-accessed) (soundManager::MusicPlaying)
DATA(0x0052928c) extern int iCombatControlGamePos;
DATA(0x00528d28) extern int const_00128d28; // MIDI music volume 0-10 (0=off)
#define gMidiEnabled const_00128d28
#define gMusicVolume const_00128d28
DATA(0x00528d2c) extern int const_00128d2c; // sample/CD music volume 0-10
#define gSampleVolume const_00128d2c
#define gSoundVolume const_00128d2c
DATA(0x00528d24) extern int const_00128d24;
#define gCursorSampleSet const_00128d24
DATA(0x00528e46) extern int gSoundTransition;
DATA(0x00522f7c) extern signed char bExpansionSmackNum;
DATA(0x004f59a8) extern short const_000f59a8;
DATA(0x004fa684) extern int gMapTypeFlags; // map format flags (game::NewMap)


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
DATA(0x00528d34) extern int gShowRoute;
DATA(0x00528d38) extern int const_00128d38;
#define gBlackoutComputer const_00128d38
DATA(0x00528d3c) extern exeGfxConfig gExecutableGraphics[];
DATA(0x00528d88) extern int gbShowCombatGrid;
DATA(0x00528d8c) extern int gbShowCombatMouseHex;
DATA(0x00528d90) extern int giCombatShadeLevel;
DATA(0x00528d10) extern long gCombatCycleTimer;

// TILE.cpp inline-asm scratch (no CodeView symbol): the blitter parks the mode word and a
// mirror-path row counter in static storage between its __asm sections.

// bmap2.cpp FillBitmapArea file-static fill state (no CodeView symbol):
// bmap2.cpp DimBitmapArea file-static dim state (no CodeView symbol):
// Icon2b.cpp IconToBitmap RLE-decoder file-static state (no CodeView symbols) - the sprite blit
// state machine shared by the Icon*2b* family. Base 0x534c20.
// bmap2.cpp BlitBitmap file-static blit state (no CodeView symbol):

// netwin.cpp netbios probe state (no CodeView symbol): the LANA number netbios was found on,
// and the "netbios present" flag, set by is_netbios_avail and read by nb_init.

// netwin.cpp netbios subsystem state (no CodeView symbols — file-scope statics in netwin.c).
// Sizes are confirmed by the address tiling (NCB=0x40, CRITICAL_SECTION=0x18, buffers adjacent).


#endif // HOMM2_GLOBALS_MODEL_H
