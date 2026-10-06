#ifndef HOMM2_TILE_H
#define HOMM2_TILE_H

#include <Ints.h>

class bitmap;
class tileset;

H2_ENUM_BEGIN(TileFlag)
    TILE_INDEX_MASK      = 0x0fff,
    TILE_FLIP_VERTICAL   = 0x4000,
    TILE_FLIP_HORIZONTAL = 0x8000
H2_ENUM_END(TileFlag)

extern "C" void __cdecl TileToBitmap(tileset* source, u32 flags, bitmap* destination, i32 x, i32 y);

// A tile scaled down by `scale` (1: TileToBitmap); EDT2PL.exe 0x00439fb0, a
// BASE object the game does not link.
void TileToBitmapScale(tileset* source, u32 flags, bitmap* destination, i32 x, i32 y, i32 scale);

#endif
