#ifndef HOMM2_TILE_H
#define HOMM2_TILE_H

#include <Ints.h>
// Declarations of the free functions DEFINED in TILE.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
class bitmap;
class tileset;

extern "C" void __cdecl TileToBitmap(tileset* src, u32 flags, bitmap* dst, i32 x, i32 y);

#endif // HOMM2_TILE_H
