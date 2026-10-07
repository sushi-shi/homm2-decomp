#ifndef HOMM2_TILE2BS_H
#define HOMM2_TILE2BS_H

#include <Ints.h>

class bitmap;
class tileset;


void TileToBitmapScale(tileset* source, u32 flags, bitmap* destination, i32 x, i32 y, i32 scale);

#endif
