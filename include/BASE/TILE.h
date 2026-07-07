#ifndef HOMM2_TILE_H
#define HOMM2_TILE_H
// Declarations of the free functions DEFINED in TILE.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
class bitmap;

extern "C" void __cdecl TileToBitmap(bitmap *src, unsigned int flags, bitmap *dst, int x, int y);

#endif // HOMM2_TILE_H
