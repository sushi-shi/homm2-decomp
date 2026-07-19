#ifndef HOMM2_BMAP2_H
#define HOMM2_BMAP2_H

#include <Ints.h>

class bitmap;

void FillBitmapArea(class bitmap*, i32, i32, i32, i32, i32);
void FillBitmapAreaClip(class bitmap*, i32, i32, i32, i32, i32, i32, i32, i32, i32);
void BlitBitmap(class bitmap*, i32, i32, i32, i32, class bitmap*, i32, i32);
void DimBitmapArea(class bitmap*, i32, i32, i32, i32, i32);

#endif
