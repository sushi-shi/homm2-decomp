#ifndef HOMM2_GLOBALS_MODEL_H
#define HOMM2_GLOBALS_MODEL_H
#include <va.h>
// Synthetic / MODELLING globals — readable names the matcher introduced for data that has
// NO CodeView symbol of its own: .rdata float literals (MSVC-hashed ??_C@ names) referenced
// as named consts for relocation control, plus anonymous data slots still being reconstructed.
// They DO have retail addresses; those marked DATA-PENDING are pinned from the reloc when
// their user function is byte-matched (they alias object-local literals / computed bases).
// Kept OUT of _globals.h (which is CodeView-backed) — assert_globals_data exempts these.
#include <_types.h>
#include <windows.h>
#include <SOURCE/comwin.h>

class mapCell;
struct searchNode;

// FINDPATH.cpp file-static priority-search scratch, recovered from the contiguous
// 0x52adc0-0x52ae64 storage and its retail relocation sites.
DATA(0x0052adc0) extern int gSearchLow;
DATA(0x0052adc4) extern mapCell *gSearchNextCell;
DATA(0x0052ae40) extern searchNode *gSearchCell;
DATA(0x0052ae44) extern int gSearchNextY;
DATA(0x0052ae48) extern mapCell *gSearchCurrentCell;
DATA(0x0052ae4c) extern int gSearchDirection;
DATA(0x0052ae50) extern int gSearchTriggerType;
DATA(0x0052ae54) extern int gSearchNextX;
DATA(0x0052ae58) extern int gSearchTerrain;
DATA(0x0052ae5c) extern searchNode *gSearchQueueNode;
DATA(0x0052ae60) extern int gSearchMiddle;
DATA(0x0052ae64) extern int gSearchHigh;

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
