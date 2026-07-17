#ifndef HOMM2_BMAP2_H
#define HOMM2_BMAP2_H

#include <Ints.h>
// Declarations of the free functions DEFINED in bmap2.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;

HOMM2_ENUM_VALUES_BEGIN(BitmapDimConstant)
    BITMAP_DIM_PALETTE_LEVEL_STRIDE = 256
HOMM2_ENUM_VALUES_END(BitmapDimConstant)

void FillBitmapArea(class bitmap*, i32, i32, i32, i32, i32);
void FillBitmapAreaClip(class bitmap*, i32, i32, i32, i32, i32, i32, i32, i32, i32);
void BlitBitmap(class bitmap*, i32, i32, i32, i32, class bitmap*, i32, i32);
void DimBitmapArea(class bitmap*, i32, i32, i32, i32, i32);

#endif // HOMM2_BMAP2_H
