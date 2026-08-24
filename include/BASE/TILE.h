#ifndef HOMM2_TILE_H
#define HOMM2_TILE_H

#include <Ints.h>

class bitmap;
class tileset;

typedef enum TileFlag {
    TILE_INDEX_MASK      = 0x0fff,
    TILE_FLIP_VERTICAL   = 0x4000,
    TILE_FLIP_HORIZONTAL = 0x8000
} TileFlag;

extern "C" void __cdecl TileToBitmap(tileset* src, u32 flags, bitmap* dst, i32 x, i32 y);

#endif
