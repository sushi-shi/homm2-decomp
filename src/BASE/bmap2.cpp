// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\bmap2.obj   from: .\basewin.lib
// functions: 4   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <_globals_model.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>
int gFillRow;
unsigned char *gFillPtr;
int gDimRow;
int gDimCol;
unsigned char *gDimPtr;
unsigned char *gDimNext;
unsigned char *gBlitSrc;
unsigned char *gBlitDst;
int gBlitRow;

VA(0x004ca3d0, 0x80)
void FillBitmapArea(class bitmap *bmp, int x, int y, int w, int h, int color)
{
    gFillRow = 0;
    gFillPtr = bmp->field_0x16 + bmp->field_0x12 * y + x;
    if (h > 0) {
        do {
            memset(gFillPtr, color, w);
            gFillPtr += bmp->field_0x12;
            gFillRow++;
        } while (gFillRow < h);
    }
}

VA(0x004ca450, 0x114)
void FillBitmapAreaClip(class bitmap *, int, int, int, int, int, int, int, int, int) {}

VA(0x004ca570, 0xa6)
void BlitBitmap(class bitmap *src, int sx, int sy, int w, int h, class bitmap *dst, int dx, int dy)
{
    gBlitSrc = src->field_0x16 + src->field_0x12 * sy + sx;
    gBlitRow = 0;
    gBlitDst = dst->field_0x16 + dst->field_0x12 * dy + dx;
    if (h > 0) {
        do {
            memcpy(gBlitDst, gBlitSrc, w);
            gBlitSrc += src->field_0x12;
            gBlitDst += dst->field_0x12;
            gBlitRow++;
        } while (gBlitRow < h);
    }
}

VA(0x004ca620, 0xa8)
void DimBitmapArea(class bitmap *bmp, int x, int y, int w, int h, int level)
{
    gDimRow = 0;
    gDimPtr = bmp->field_0x16 + bmp->field_0x12 * y + x;
    if (h > 0) {
        do {
            gDimCol = 0;
            gDimNext = gDimPtr + bmp->field_0x12;
            if (w > 0) {
                do {
                    *gDimPtr = uDimPal[0][0][level * 256 + *gDimPtr];
                    gDimPtr++;
                    gDimCol++;
                } while (gDimCol < w);
            }
            gDimRow++;
            gDimPtr = gDimNext;
        } while (gDimRow < h);
    }
}

