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
    gFillPtr = bmp->m_pixels + bmp->m_width * y + x;
    if (h > 0) {
        do {
            memset(gFillPtr, color, w);
            gFillPtr += bmp->m_width;
            gFillRow++;
        } while (gFillRow < h);
    }
}

VA(0x004ca450, 0x114)
void FillBitmapAreaClip(class bitmap *bmp, int x, int y, int w, int h, int color, int clipx,
                        int clipy, int clipw, int cliph)
{
    int cx2 = clipx - 1 + clipw;
    int cy2;
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
        gFillRow = 0;
        gFillPtr = bmp->m_pixels + bmp->m_width * y + x;
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
void BlitBitmap(class bitmap *src, int sx, int sy, int w, int h, class bitmap *dst, int dx, int dy)
{
    gBlitSrc = src->m_pixels + src->m_width * sy + sx;
    gBlitRow = 0;
    gBlitDst = dst->m_pixels + dst->m_width * dy + dx;
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
void DimBitmapArea(class bitmap *bmp, int x, int y, int w, int h, int level)
{
    gDimRow = 0;
    gDimPtr = bmp->m_pixels + bmp->m_width * y + x;
    if (h > 0) {
        do {
            gDimCol = 0;
            gDimNext = gDimPtr + bmp->m_width;
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

