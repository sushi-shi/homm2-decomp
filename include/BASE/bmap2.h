#ifndef HOMM2_BMAP2_H
#define HOMM2_BMAP2_H

#include <Ints.h>

class bitmap;

void FillBitmapArea(class bitmap* image, i32 x, i32 y, i32 width, i32 height, i32 color);
void FillBitmapAreaClip(
    class bitmap* image,
    i32 x,
    i32 y,
    i32 width,
    i32 height,
    i32 color,
    i32 clipx,
    i32 clipy,
    i32 clipw,
    i32 cliph
);
void BlitBitmap(
    class bitmap* source,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    class bitmap* destination,
    i32 dx,
    i32 dy
);
void DimBitmapArea(class bitmap* image, i32 x, i32 y, i32 width, i32 height, i32 level);

#endif
