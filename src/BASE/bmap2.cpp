#include <va.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>
#include <SOURCE/KB.h>
H2_ENUM_BEGIN(BitmapDimConstant)
    DIM_PALETTE_LEVEL_STRIDE = 256
H2_ENUM_END(BitmapDimConstant)

static i32 gFillRow;
static u8* gFillPtr;
static i32 gDimRow;
static i32 gDimCol;
static u8* gDimPtr;
static u8* gDimNext;
static u8* gBlitSrc;
static u8* gBlitDst;
static i32 gBlitRow;

VA(0x004c6450, 0x82)
void FillBitmapArea(class bitmap* bmp, i32 x, i32 y, i32 w, i32 h, i32 color) {
    gFillPtr = bmp->m_pixels + x + y * bmp->m_width;
    for (gFillRow = 0; gFillRow < h; gFillRow++) {
        memset(gFillPtr, color, w);
        gFillPtr += bmp->m_width;
    }
}

VA(0x004c64e0, 0xee)
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
) {
    if (x >= clipx + clipw - 1 || x + w - 1 <= clipx || y >= clipy + cliph - 1
        || y + h - 1 <= clipy)
        return;
    if (x + w - 1 >= clipx + clipw - 1)
        w = clipx + clipw - x;
    if (x < clipx) {
        w = w - (clipx - x);
        x = clipx;
    }
    if (y + h - 1 >= clipy + cliph - 1)
        h = clipy + cliph - y;
    if (y < clipy) {
        h = h - (clipy - y);
        y = clipy;
    }
    FillBitmapArea(bmp, x, y, w, h, color);
}

VA(0x004c65d0, 0xb7)
void BlitBitmap(
    class bitmap* src,
    i32 sx,
    i32 sy,
    i32 w,
    i32 h,
    class bitmap* dst,
    i32 dx,
    i32 dy
) {
    gBlitSrc = src->m_pixels + sx + sy * src->m_width;
    gBlitDst = dst->m_pixels + dx + dy * dst->m_width;
    for (gBlitRow = 0; gBlitRow < h; gBlitRow++) {
        memcpy(gBlitDst, gBlitSrc, w);
        gBlitSrc += src->m_width;
        gBlitDst += dst->m_width;
    }
}

VA(0x004c6690, 0xcd)
void DimBitmapArea(class bitmap* bmp, i32 x, i32 y, i32 w, i32 h, i32 level) {
    gDimPtr = bmp->m_pixels + y * bmp->m_width + x;
    for (gDimRow = 0; gDimRow < h; gDimRow++) {
        gDimNext = gDimPtr + bmp->m_width;
        for (gDimCol = 0; gDimCol < w; gDimCol++) {
            *gDimPtr = uDimPal[0][0][level * DIM_PALETTE_LEVEL_STRIDE + *gDimPtr];
            gDimPtr++;
        }
        gDimPtr = gDimNext;
    }
}
