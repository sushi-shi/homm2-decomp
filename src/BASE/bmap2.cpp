#include <Ints.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>
#include <SOURCE/KB.h>

static i32 gFillRow = 0;
static u8* gFillPtr = NULL;
static i32 gDimRow = 0;
static u8* gDimPtr = NULL;
static i32 gDimCol = 0;
static i32 gBlitRow = 0;
static u8* gDimNext = NULL;
static u8* gBlitSrc = NULL;
static u8* gBlitDst = NULL;

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
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    if (x >= clipX + clipW - 1 || x + width - 1 <= clipX || y >= clipY + clipH - 1
        || y + height - 1 <= clipY)
        return;
    if (x + width - 1 >= clipX + clipW - 1)
        width = clipX + clipW - x;
    if (x < clipX) {
        width = width - (clipX - x);
        x = clipX;
    }
    if (y + height - 1 >= clipY + clipH - 1)
        height = clipY + clipH - y;
    if (y < clipY) {
        height = height - (clipY - y);
        y = clipY;
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
    i32 destinationX,
    i32 destinationY
) {
    gBlitSrc = source->m_pixels + sourceX + sourceY * source->m_width;
    gBlitDst = destination->m_pixels + destinationX + destinationY * destination->m_width;
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
