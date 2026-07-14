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
