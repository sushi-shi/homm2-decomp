#include <va.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>
DATA(0x005348f0) static i32 gFillRow;
DATA(0x005348f8) static u8* gFillPtr;
DATA(0x005348e8) static i32 gDimRow;
DATA(0x005348e4) static i32 gDimCol;
DATA(0x005348f4) static u8* gDimPtr;
DATA(0x00534900) static u8* gDimNext;
DATA(0x005348ec) static u8* gBlitSrc;
DATA(0x005348e0) static u8* gBlitDst;
DATA(0x005348fc) static i32 gBlitRow;

VA(0x004ca3d0, 0x80)
void FillBitmapArea(class bitmap* bmp, i32 x, i32 y, i32 w, i32 h, i32 color) {
    gFillPtr = bmp->m_pixels + bmp->m_width * y + x;
    gFillRow = 0;
    if (h > 0) {
        do {
            memset(gFillPtr, color, w);
            gFillPtr += bmp->m_width;
            gFillRow++;
        } while (gFillRow < h);
    }
}

// @semantic: compiler-shape residual.
VA(0x004ca450, 0x114)
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
    i32 cx2 = clipx - 1 + clipw;
    i32 cy2;
    if (x < cx2 && clipx < x - 1 + w && (cy2 = clipy - 1 + cliph, y < cy2) && clipy < y - 1 + h) {
        if (cx2 <= x - 1 + w)
            w = clipx + (clipw - x);
        if (x < clipx) {
            w = (w - clipx) + x;
            x = clipx;
        }
        if (cy2 <= y - 1 + h)
            h = (cliph - y) + clipy;
        if (y < clipy) {
            h = (h - clipy) + y;
            y = clipy;
        }
        gFillPtr = bmp->m_pixels + bmp->m_width * y + x;
        gFillRow = 0;
        if (h > 0) {
            do {
                memset(gFillPtr, color, w);
                gFillPtr += bmp->m_width;
                gFillRow++;
            } while (gFillRow < h);
        }
    }
}

VA(0x004ca570, 0xa6)
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
    gBlitSrc = src->m_pixels + src->m_width * sy + sx;
    gBlitDst = dst->m_pixels + dst->m_width * dy + dx;
    gBlitRow = 0;
    if (h > 0) {
        do {
            memcpy(gBlitDst, gBlitSrc, w);
            gBlitSrc += src->m_width;
            gBlitDst += dst->m_width;
            gBlitRow++;
        } while (gBlitRow < h);
    }
}

VA(0x004ca620, 0xa8)
void DimBitmapArea(class bitmap* bmp, i32 x, i32 y, i32 w, i32 h, i32 level) {
    gDimPtr = bmp->m_pixels + bmp->m_width * y + x;
    for (gDimRow = 0; gDimRow < h; gDimRow++) {
        gDimNext = gDimPtr + bmp->m_width;
        for (gDimCol = 0; gDimCol < w; gDimCol++) {
            *gDimPtr = uDimPal[0][0][level * BITMAP_DIM_PALETTE_LEVEL_STRIDE + *gDimPtr];
            gDimPtr++;
        }
        gDimPtr = gDimNext;
    }
}
