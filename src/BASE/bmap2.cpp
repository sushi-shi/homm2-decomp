// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\bmap2.obj   from: .\basewin.lib
// functions: 4   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>
DATA(0x005348f0) static int gFillRow;
DATA(0x005348f8) static unsigned char *gFillPtr;
DATA(0x005348e8) static int gDimRow;
DATA(0x005348e4) static int gDimCol;
DATA(0x005348f4) static unsigned char *gDimPtr;
DATA(0x00534900) static unsigned char *gDimNext;
DATA(0x005348ec) static unsigned char *gBlitSrc;
DATA(0x005348e0) static unsigned char *gBlitDst;
DATA(0x005348fc) static int gBlitRow;



VA(0x004ca3d0, 0x80)
void FillBitmapArea(class bitmap *bmp, int x, int y, int w, int h, int color)
{
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

// @semantic: the retail/base CodeView spans are both 0x114 bytes, with the
// same 0x8-byte local area, saved-register set, CFG, and 7/7 external
// relocation occurrences. Masking those relocations leaves only four byte
// differences: +0x1a ca/d1 and +0x1c 8e/8d are base `cmp ecx,edx; jle`
// versus retail `cmp edx,ecx; jge`; +0x4a df/fb and +0x4c 8e/8d repeat the
// same equivalent reversal for EBX/EDI. Publishing gFillPtr before clearing
// gFillRow is required: it makes the entire later address-calculation/store
// schedule exact and raises the live match from 94.44% to 98.44%.
// Relational polarity/reassociation, combined/split/const bound declarations,
// positive and early-return overlap forms, semantic parameter/local names,
// and exact-preserving predecessor expression/loop/include-order variants did
// not steer the remaining compares. Moving or narrowing later-only headers
// regressed this target (and in some cases the exact predecessor). A bounded
// libclang AST pass found no gain. Revisit only after a genuine shared-header
// or predecessor declaration-state change; this is not a certified wall.
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
void BlitBitmap(class bitmap *src, int sx, int sy, int w, int h, class bitmap *dst, int dx, int dy)
{
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

// @semantic: retail/base are both 0xa8 bytes, save EBX/ESI/EDI/EBP with no
// local frame, share the same CFG, and have 15/15 relocations by manual COFF
// audit, including uDimPal at +0x5f on both sides (the helper misresolves its
// addend). The first non-relocation byte divergence is +0x80: base emits
// `cmp eax,ebx; jg`, retail `cmp ebx,eax; jl`; the outer loop repeats that at
// +0x9d. Naming the 256-entry dim-palette level stride moved the canonical
// live match to 97.20%, but relational reversals and a bounded libclang AST
// pass found no further gain. A corrected 300-trial parser-visible TU-state
// pass also found no exact closure and retained nothing. Revisit only after a
// predecessor or shared-header state change.
VA(0x004ca620, 0xa8)
void DimBitmapArea(class bitmap *bmp, int x, int y, int w, int h, int level)
{
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
