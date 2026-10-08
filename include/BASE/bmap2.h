#ifndef HOMM2_BASE_BMAP2_H
#define HOMM2_BASE_BMAP2_H

#include <H2/Ints.h>

class bitmap;

void FillBitmapArea(class bitmap* image, i32 x, i32 y, i32 width, i32 height, i32 color);
void FillBitmapAreaClip(
    class bitmap* image,
    i32 x,
    i32 y,
    i32 width,
    i32 height,
    i32 color,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
);
void BlitBitmap(
    class bitmap* source,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY
);
void DimBitmapArea(class bitmap* image, i32 x, i32 y, i32 width, i32 height, i32 level);

#endif // HOMM2_BASE_BMAP2_H
