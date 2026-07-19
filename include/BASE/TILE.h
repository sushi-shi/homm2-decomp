#ifndef HOMM2_TILE_H
#define HOMM2_TILE_H

#include <Ints.h>

class bitmap;
class tileset;

extern "C" void __cdecl TileToBitmap(tileset* src, u32 flags, bitmap* dst, i32 x, i32 y);

#endif
