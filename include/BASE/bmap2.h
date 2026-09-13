#ifndef HOMM2_BMAP2_H
#define HOMM2_BMAP2_H

#include <Ints.h>

class bitmap;

void FillBitmapArea(class bitmap* bmp, i32 x, i32 y, i32 width, i32 height, i32 color);
void FillBitmapAreaClip(
    class bitmap* bmp,
    i32 x,
    i32 y,
    i32 width,
    i32 height,
    i32 color,
    i32 clipX,
    i32 clipY,
    i32 clipWidth,
    i32 clipHeight
);
void BlitBitmap(
    class bitmap* src,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    class bitmap* dst,
    i32 destinationX,
    i32 destinationY
);
void DimBitmapArea(class bitmap* bmp, i32 x, i32 y, i32 width, i32 height, i32 level);

#endif
