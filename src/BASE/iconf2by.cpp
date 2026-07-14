// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\iconf2by.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconf2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block.
static int gFYXEnd;
static unsigned char *gFYDimPal;
static int gFYX0;
static unsigned int gFYDimLen2;
static IconEntry *gFYEntry;
static unsigned char *gFYDst;
static int gFYX;
static int gFYSkip;
static unsigned char *gFYSrc;
static unsigned char gFYColor;
static int gFYDimLen;
static int gFYClipB;
static int gFYDimIdx;
static int gFYClipR;
static int gFYRow;
static int gFYRun;
static int gFYY;
static unsigned char *gFYDimDst;

VA(0x004d9ce0, 0x58d)
void FlipIconToBitmapYModify(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                             int clip, int clipX, int clipY, int clipW, int clipH, int color,
                             signed char *shear)
{
    int clipWidth = clipW;
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    gFYEntry = &entries[frame];
    gFYSrc = reinterpret_cast<unsigned char *>(srcIcon->m_data) + gFYEntry->srcOffset;
    gFYX0 = ((x - gFYEntry->w) - gFYEntry->x) + 1;
    gFYXEnd = gFYEntry->w + gFYX0 - 1;
    gFYY = gFYEntry->y + y;
    gFYX = gFYXEnd - shear[gFYY];
    gFYClipB = clipY + clipH - 1;
    gFYClipR = clipX + clipWidth - 1;
    gFYRow = dest->m_width * gFYY + reinterpret_cast<int>(dest->m_pixels);
    for (;;) {
        gFYRun = *gFYSrc;
        gFYSrc = gFYSrc + 1;
        if (static_cast<signed char>(gFYRun) < 0) {
            if ((gFYRun & 0x40) == 0) {
                if ((gFYRun & 0x3f) == 0)
                    return;
                gFYX = gFYX - (gFYRun & 0x3f);
                continue;
            }
            if ((gFYRun & 0x3f) != 0) {
                if (gFYRun == 0xc1) {
                    gFYRun = *gFYSrc;
                    gFYSrc = gFYSrc + 1;
                } else {
                    gFYRun = gFYRun & 0x3f;
                }
                gFYColor = *gFYSrc;
                gFYSrc = gFYSrc + 1;
                goto do_fill;
            }
            gFYRun = *gFYSrc;
            gFYSrc = gFYSrc + 1;
            if ((gFYRun & 3) != 0) {
                gFYDimLen = gFYRun & 3;
            } else {
                gFYDimLen = *gFYSrc;
                gFYSrc = gFYSrc + 1;
            }
            gFYDimLen2 = gFYDimLen;
            if (color != 0 && (gFYRun & 0x80) != 0) {
                gFYRun = gFYDimLen;
                gFYColor = static_cast<unsigned char>(color);
                goto do_fill;
            }
            if ((gFYRun & 0x40) != 0) {
                gFYDimPal =
                    reinterpret_cast<unsigned char *>(uDimPal) + (gFYRun & 0x3c) * 0x40;
                if (shear[gFYY] != 0x7f && clipY <= gFYY && gFYY <= gFYClipB) {
                    if (clipX <= (gFYX - gFYDimLen) + 1 && gFYX <= gFYClipR) {
                        unsigned char *dimDst;
                        if (clipX <= (gFYX - gFYDimLen) + 1) {
                            dimDst = reinterpret_cast<unsigned char *>((gFYRow - gFYDimLen) + gFYX + 1);
                        } else {
                            gFYDimLen = (gFYX - clipX) + 1;
                            dimDst = reinterpret_cast<unsigned char *>(gFYRow + clipX);
                        }
                        gFYDimIdx = 0;
                        gFYDimDst = dimDst;
                        if (0 < static_cast<int>(gFYDimLen)) {
                            do {
                                *gFYDimDst = gFYDimPal[*gFYDimDst];
                                gFYDimDst = gFYDimDst + 1;
                                gFYDimIdx = gFYDimIdx + 1;
                            } while (gFYDimIdx < static_cast<int>(gFYDimLen));
                        }
                    }
                }
            }
            gFYX = gFYX - gFYDimLen2;
            continue;
        do_fill:
            if (shear[gFYY] != 0x7f && clipY <= gFYY && gFYY <= gFYClipB) {
                if (clipX <= (gFYX - gFYRun) + 1 && gFYX <= gFYClipR) {
                    if (clipX <= (gFYX - gFYRun) + 1) {
                        memset(reinterpret_cast<unsigned char *>((gFYRow - gFYRun) + 1 + gFYX),
                               gFYColor, gFYRun);
                    } else {
                        memset(reinterpret_cast<unsigned char *>(gFYRow + clipX), gFYColor,
                               (gFYX - clipX) + 1);
                    }
                }
            }
            gFYX = gFYX - gFYRun;
            continue;
        }
        // ---- positive command : backward literal copy / newline ----
        if (gFYRun != 0) {
            unsigned int advance = gFYRun;
            if (shear[gFYY] != 0x7f && clipY <= gFYY && gFYY <= gFYClipB) {
                int left = (gFYX - gFYRun) + 1;
                if (left <= gFYClipR && clipX <= gFYX) {
                    if (gFYX <= gFYClipR) {
                        gFYDst = reinterpret_cast<unsigned char *>(gFYRow + gFYX);
                        if (clipX <= left) {
                            gFYSkip = 0;
                            gFYDimLen = gFYRun;
                        } else {
                            gFYDimLen = (gFYX - clipX) + 1;
                            gFYSkip = gFYRun - gFYDimLen;
                        }
                    } else {
                        gFYSrc = gFYSrc + (gFYX - gFYClipR);
                        gFYDst = reinterpret_cast<unsigned char *>(gFYClipR + gFYRow);
                        if ((gFYX - gFYRun) < clipX) {
                            gFYSkip = ((gFYRun - gFYX) - clipWidth) + gFYClipR;
                            gFYDimLen = clipWidth;
                        } else {
                            gFYSkip = 0;
                            gFYDimLen = (gFYRun - gFYX) + gFYClipR;
                        }
                    }
                    gFYDimIdx = 0;
                    if (0 < static_cast<int>(gFYDimLen)) {
                        do {
                            *gFYDst = *gFYSrc;
                            gFYSrc = gFYSrc + 1;
                            gFYDst = gFYDst - 1;
                            gFYDimIdx = gFYDimIdx + 1;
                        } while (gFYDimIdx < static_cast<int>(gFYDimLen));
                    }
                    advance = gFYSkip;
                }
            }
            gFYSrc = gFYSrc + advance;
            gFYX = gFYX - gFYRun;
            continue;
        }
        // newline
        gFYX = gFYXEnd - shear[gFYY];
        gFYY = gFYY + 1;
        gFYRow = gFYRow + dest->m_width;
    }
}
