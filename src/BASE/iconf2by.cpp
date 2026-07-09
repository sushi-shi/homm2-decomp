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
static unsigned int gFYDimLen;
static int gFYClipB;
static int gFYDimIdx;
static int gFYClipR;
static int gFYRow;
static unsigned int gFYRun;
static int gFYY;
static unsigned char *gFYDimDst;

// @early-stop
// Flip + Y-shear variant: horizontal-flip decoder whose per-scanline X origin is gFYXEnd - shear[Y]
// (0x7f = skip line). Always clips; X/row live in globals gFYX/gFYRow. Backward byte copy, memset
// solid runs, per-pixel dim. Residual is the /O2 register-fusion wall.
VA(0x004d9ce0, 0x58d)
void FlipIconToBitmapYModify(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                             int clip, int clipX, int clipY, int clipW, int clipH, int color,
                             signed char *shear)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    gFYEntry = &entries[frame];
    gFYSrc = reinterpret_cast<unsigned char *>(srcIcon->m_data) + gFYEntry->srcOffset;
    gFYX0 = ((x - gFYEntry->w) - gFYEntry->x) + 1;
    gFYXEnd = gFYEntry->w + gFYX0 - 1;
    gFYY = gFYEntry->y + y;
    gFYX = gFYXEnd - shear[gFYY];
    gFYClipR = clipX + clipW - 1;
    gFYClipB = clipY + clipH - 1;
    short pitch = dest->m_width;
    gFYRow = pitch * gFYY + reinterpret_cast<int>(dest->m_pixels);
    for (;;) {
        int cmd = *gFYSrc++;
        if (static_cast<signed char>(cmd) < 0) {
            gFYRun = cmd;
            if ((cmd & 0x40) == 0) {
                if ((cmd & 0x3f) == 0)
                    return;
                gFYX = gFYX - (cmd & 0x3f);
                continue;
            }
            unsigned int count = cmd & 0x3f;
            int flags = 0;
            if (count != 0) {
                gFYRun = count;
                if (cmd == 0xc1) {
                    gFYRun = *gFYSrc;
                    gFYSrc = gFYSrc + 1;
                }
                gFYColor = *gFYSrc;
                gFYSrc = gFYSrc + 1;
                goto do_fill;
            }
            flags = *gFYSrc;
            gFYSrc = gFYSrc + 1;
            gFYDimLen = flags & 3;
            if ((flags & 3) == 0) {
                gFYDimLen = *gFYSrc;
                gFYSrc = gFYSrc + 1;
            }
            gFYDimLen2 = gFYDimLen;
            if (color != 0 && (flags & 0x80) != 0) {
                gFYColor = static_cast<unsigned char>(color);
                gFYRun = gFYDimLen;
                goto do_fill;
            }
            if ((flags & 0x40) != 0) {
                gFYDimPal = reinterpret_cast<unsigned char *>(uDimPal) + (flags & 0x3c) * 0x40;
                if (shear[gFYY] != 0x7f && clipY <= gFYY && gFYY <= gFYClipB) {
                    int left = (gFYX - gFYDimLen) + 1;
                    if (clipX <= left && gFYX <= gFYClipR) {
                        if (left < clipX) {
                            gFYDimLen = (gFYX - clipX) + 1;
                            gFYDimDst = reinterpret_cast<unsigned char *>(gFYRow + clipX);
                        } else {
                            gFYDimDst = reinterpret_cast<unsigned char *>((gFYRow - gFYDimLen) + gFYX + 1);
                        }
                        gFYDimIdx = 0;
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
                int left = (gFYX - gFYRun) + 1;
                if (clipX <= left && gFYX <= gFYClipR) {
                    unsigned int cn;
                    unsigned char *dst;
                    if (left < clipX) {
                        cn = (gFYX - clipX) + 1;
                        dst = reinterpret_cast<unsigned char *>(gFYRow + clipX);
                    } else {
                        cn = gFYRun;
                        dst = reinterpret_cast<unsigned char *>((gFYRow - gFYRun) + 1 + gFYX);
                    }
                    memset(dst, gFYColor, cn);
                }
            }
            gFYX = gFYX - gFYRun;
            continue;
        }
        // ---- positive command : backward literal copy / newline ----
        gFYRun = cmd;
        if (cmd != 0) {
            unsigned int adv = cmd;
            if (shear[gFYY] != 0x7f && clipY <= gFYY && gFYY <= gFYClipB) {
                int left = (gFYX - cmd) + 1;
                if (left <= gFYClipR && clipX <= gFYX) {
                    if (gFYClipR < gFYX) {
                        gFYSrc = gFYSrc + (gFYX - gFYClipR);
                        gFYDst = reinterpret_cast<unsigned char *>(gFYClipR + gFYRow);
                        if ((gFYX - cmd) < clipX) {
                            gFYDimLen = clipW;
                            gFYSkip = ((cmd - gFYX) - clipW) + gFYClipR;
                        } else {
                            gFYSkip = 0;
                            gFYDimLen = (cmd - gFYX) + gFYClipR;
                        }
                    } else {
                        gFYDst = reinterpret_cast<unsigned char *>(gFYRow + gFYX);
                        if (left < clipX) {
                            gFYDimLen = (gFYX - clipX) + 1;
                            gFYSkip = cmd - gFYDimLen;
                        } else {
                            gFYSkip = 0;
                            gFYDimLen = cmd;
                        }
                    }
                    gFYDimIdx = 0;
                    adv = gFYSkip;
                    if (0 < static_cast<int>(gFYDimLen)) {
                        do {
                            *gFYDst = *gFYSrc;
                            gFYDst = gFYDst - 1;
                            gFYSrc = gFYSrc + 1;
                            gFYDimIdx = gFYDimIdx + 1;
                        } while (gFYDimIdx < static_cast<int>(gFYDimLen));
                    }
                }
            }
            gFYSrc = gFYSrc + adv;
            gFYX = gFYX - cmd;
            continue;
        }
        // newline
        gFYX = gFYXEnd - shear[gFYY];
        gFYY = gFYY + 1;
        gFYRow = gFYRow + pitch;
    }
}
