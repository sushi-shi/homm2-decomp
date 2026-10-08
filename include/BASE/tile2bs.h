#ifndef HOMM2_TILE2BS_H
#define HOMM2_TILE2BS_H

#include <H2/Ints.h>

class bitmap;
class tileset;

// A tile scaled down by `scale` (1: TileToBitmap); EDT2PL.exe 0x00439fb0, a
// BASE object the game does not link.
void TileToBitmapScale(tileset* source, u32 flags, bitmap* destination, i32 x, i32 y, i32 scale);

#endif
