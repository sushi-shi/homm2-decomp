#ifndef HOMM2_BMAP2_H
#define HOMM2_BMAP2_H

#include <Ints.h>

class bitmap;

void FillBitmapArea(class bitmap* bmp, i32 x, i32 y, i32 w, i32 h, i32 color);
void FillBitmapAreaClip(
    class bitmap* bmp,
    i32 x,
    i32 y,
    i32 w,
    i32 h,
    i32 color,
    i32 clipx,
    i32 clipy,
    i32 clipw,
    i32 cliph
);
void BlitBitmap(
    class bitmap* source,
    i32 sx,
    i32 sy,
    i32 w,
    i32 h,
    class bitmap* destination,
    i32 dx,
    i32 dy
);
void DimBitmapArea(class bitmap* bmp, i32 x, i32 y, i32 w, i32 h, i32 level);

#endif
