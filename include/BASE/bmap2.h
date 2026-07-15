#ifndef HOMM2_BMAP2_H
#define HOMM2_BMAP2_H
// Declarations of the free functions DEFINED in bmap2.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;

typedef enum BitmapDimConstant {
    BITMAP_DIM_PALETTE_LEVEL_STRIDE = 256
} BitmapDimConstant;

void FillBitmapArea(class bitmap *, int, int, int, int, int);
void FillBitmapAreaClip(class bitmap *, int, int, int, int, int, int, int, int, int);
void BlitBitmap(class bitmap *, int, int, int, int, class bitmap *, int, int);
void DimBitmapArea(class bitmap *, int, int, int, int, int);

#endif // HOMM2_BMAP2_H
