#include <Ints.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>
#include <SOURCE/KB.h>

static i32 gFillRow = 0;
static u8* gFillPtr = 0;
static i32 gDimRow = 0;
static u8* gDimPtr = 0;
static i32 gDimCol = 0;
static i32 gBlitRow = 0;
static u8* gDimNext = 0;
static u8* gBlitSrc = 0;
static u8* gBlitDst = 0;

void FillBitmapArea(class bitmap* image, i32 x, i32 y, i32 width, i32 height, i32 color) {
    gFillPtr = image->m_pixels + x + y * image->m_width;
    for (gFillRow = 0; gFillRow < height; gFillRow++) {
        memset(gFillPtr, color, width);
        gFillPtr += image->m_width;
    }
}

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
) {
    if (x >= clipx + clipw - 1 || x + width - 1 <= clipx || y >= clipy + cliph - 1
        || y + height - 1 <= clipy)
        return;
    if (x + width - 1 >= clipx + clipw - 1)
        width = clipx + clipw - x;
    if (x < clipx) {
        width = width - (clipx - x);
        x = clipx;
    }
    if (y + height - 1 >= clipy + cliph - 1)
        height = clipy + cliph - y;
    if (y < clipy) {
        height = height - (clipy - y);
        y = clipy;
    }
    FillBitmapArea(image, x, y, width, height, color);
}

void BlitBitmap(
    class bitmap* source,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    class bitmap* destination,
    i32 dx,
    i32 dy
) {
    gBlitSrc = source->m_pixels + sourceX + sourceY * source->m_width;
    gBlitDst = destination->m_pixels + dx + dy * destination->m_width;
    for (gBlitRow = 0; gBlitRow < height; gBlitRow++) {
        memcpy(gBlitDst, gBlitSrc, width);
        gBlitSrc += source->m_width;
        gBlitDst += destination->m_width;
    }
}

void DimBitmapArea(class bitmap* image, i32 x, i32 y, i32 width, i32 height, i32 level) {
    gDimPtr = image->m_pixels + y * image->m_width + x;
    for (gDimRow = 0; gDimRow < height; gDimRow++) {
        gDimNext = gDimPtr + image->m_width;
        for (gDimCol = 0; gDimCol < width; gDimCol++) {
            *gDimPtr = uDimPal[0][level][*gDimPtr];
            gDimPtr++;
        }
        gDimPtr = gDimNext;
    }
}
