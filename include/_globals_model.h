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
#include <nb30.h>
#include <SOURCE/comwin.h>

// VAs pinned from the retail disassembly of the referencing fns (no CodeView symbol of their own):
DATA(0x004faeb8) extern SCreatureInfo gCreatureInfo[];   // monster-DB view (game::ExperienceValueOfStack)
DATA(0x004faeb2) extern monsterRV gMonsterInfo[100];     // monster-DB view, 26B recs (philAI::ComputeUpgradeValue)
// (gRVWeightOre/Merc/Crystal removed — they were just gafAITurnCostResource[2|1|4]; the code
//  now indexes the array directly via the RES_* enum, which matches the retail byte-for-byte.)
DATA(0x0051ef24) extern char gDefaultCursorName[]; // default cursor resource name (mouseManager::SetColorMice)
DATA(0x00528dca) extern int gCdMusic; // CD-vs-MIDI music mode (dword-accessed) (soundManager::MusicPlaying)
DATA(0x00528d28) extern int gMidiEnabled; // MIDI music volume 0-10 (0=off) (soundManager::MIDIIsPlaying/ConvertVolume)
DATA(0x00528d2c) extern int gSampleVolume; // sample/CD music volume 0-10 (soundManager::ConvertVolume)


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
DATA(0x00528d38) extern int const_00128d38;            // 13-byte BSS flag buffer (??_C@_0N@PMOM@...)

// TILE.cpp inline-asm scratch (no CodeView symbol): the blitter parks the mode word and a
// mirror-path row counter in static storage between its __asm sections.

// bmap2.cpp FillBitmapArea file-static fill state (no CodeView symbol):
// bmap2.cpp DimBitmapArea file-static dim state (no CodeView symbol):
// Icon2b.cpp IconToBitmap RLE-decoder file-static state (no CodeView symbols) - the sprite blit
// state machine shared by the Icon*2b* family. Base 0x534c20.
// bmap2.cpp BlitBitmap file-static blit state (no CodeView symbol):

// netwin.cpp netbios per-session status byte array (no CodeView symbol), indexed by session id.
DATA(0x005173b4) extern char gNetStatus[];

// netwin.cpp netbios probe state (no CodeView symbol): the LANA number netbios was found on,
// and the "netbios present" flag, set by is_netbios_avail and read by nb_init.

// netwin.cpp netbios subsystem state (no CodeView symbols — file-scope statics in netwin.c).
// Sizes are confirmed by the address tiling (NCB=0x40, CRITICAL_SECTION=0x18, buffers adjacent).


#endif // HOMM2_GLOBALS_MODEL_H
