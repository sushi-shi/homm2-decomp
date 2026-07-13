// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\iconf2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconf2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block (0x5380c0+).
static int gFCSkip;
static unsigned int gFCRun;
static int gFCX0;
static int gFCXEnd;
static unsigned int gFCCnt;
static unsigned int gFCCnt2;
static int gFCY;
static IconEntry *gFCEntry;
static int gFCX;
static unsigned char *gFCSrc;
static unsigned char *gFCDimPal;
static unsigned char *gFCDimDst;
static int gFCClipB;
static int gFCRow;
static unsigned int gFCDimLen;
static unsigned char gFCColor;
static int gFCClipR;
static unsigned char *gFCDst;

// @early-stop
// /O2 regalloc/intrinsic wall: base 0x532 vs retail 0x54d; all 83 reloc occurrences and targets
// agree. Residual spans are setup +0x18..+0xec, fill/dim +0x181..+0x394, and clipped copy
// +0x395..+0x542. Tried local/global/mutable setup, signed/unsigned counts, merged/split memset,
// pointer-update orders, global palette/destination loads, and 136 AST variants over five rounds.
VA(0x004d9790, 0x54d)
void FlipIconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                                int clip, int clipX, int clipY, int clipW, int clipH, int color,
                                unsigned char *colorTable)
{
    unsigned char *src = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entry = reinterpret_cast<IconEntry *>(src) + frame;
    int width = entry->w;
    int x0 = x - entry->x - width + 1;
    gFCEntry = entry;
    src += entry->srcOffset;
    gFCX0 = x0;
    int X = x0 + width - 1;
    gFCXEnd = X;
    int Y = y + entry->y;
    gFCY = Y;
    if (clip != 0) {
        if (x0 < clipX || clipW + clipX <= x0 + width - 1 || Y < clipY ||
            clipY + clipH < entry->h + Y) {
            clip = 1;
            gFCClipR = clipX + clipW - 1;
            gFCClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    int pitch = dest->m_width;
    gFCRow = gFCY * pitch + reinterpret_cast<int>(dest->m_pixels);
    for (;;) {
        int cmd = *src++;
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & 0x40) == 0) {
                gFCRun = cmd;
                int n = cmd & 0x3f;
                gFCX = X;
                gFCSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            gFCRun = cmd;
            unsigned int count = cmd & 0x3f;
            int flags = 0;
            if (count != 0) {
                if (cmd == 0xc1)
                    count = *src++;
                gFCColor = colorTable[*src];
                src = src + 1;
                goto do_fill;
            }
            flags = *src++;
            count = flags & 3;
            if (count == 0)
                count = *src++;
            gFCDimLen = count;
            if (color != 0) {
                gFCRun = flags;
                if (flags & 0x80) {
                    gFCCnt2 = count;
                    gFCColor = static_cast<unsigned char>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X), gFCColor, count);
            } else {
                int left;
                if (clipY <= gFCY && gFCY <= gFCClipB &&
                    (left = (X - count) + 1, clipX <= left) && X <= gFCClipR) {
                    if (left < clipX) {
                        memset(reinterpret_cast<unsigned char *>(gFCRow + clipX), gFCColor,
                               (X - clipX) + 1);
                    } else {
                        memset(reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X),
                               gFCColor, count);
                    }
                }
            }
            X = X - count;
            gFCRun = count;
            continue;
        do_dim:
            gFCRun = flags;
            gFCCnt2 = count;
            if (flags & 0x40) {
                unsigned char *palette =
                    reinterpret_cast<unsigned char *>(uDimPal) + (flags & 0x3c) * 0x40;
                gFCDimPal = palette;
                if (clip == 0) {
                    gFCCnt = 0;
                    unsigned char *dp = reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X);
                    gFCDimDst = dp;
                    if (static_cast<int>(count) > 0) {
                        gFCCnt = count;
                        for (unsigned int k = count; k != 0; k--) {
                            *dp = gFCDimPal[*dp];
                            dp = dp + 1;
                            gFCDimDst = dp;
                        }
                    }
                } else {
                    gFCCnt2 = count;
                    if (clipY <= gFCY && gFCY <= gFCClipB &&
                        (int)((X - count) + 1) >= clipX && X <= gFCClipR) {
                        int left = (X - count) + 1;
                        unsigned char *dp;
                        if (left < clipX) {
                            count = (X - clipX) + 1;
                            dp = reinterpret_cast<unsigned char *>(gFCRow + clipX);
                        } else {
                            dp = reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X);
                        }
                        gFCCnt2 = count;
                        gFCCnt = 0;
                        gFCDimDst = dp;
                        if (static_cast<int>(count) > 0) {
                            gFCCnt = count;
                            for (unsigned int k = count; k != 0; k--) {
                                *dp = gFCDimPal[*dp];
                                dp = dp + 1;
                                gFCDimDst = dp;
                            }
                        }
                    }
                }
            }
            X = X - gFCDimLen;
            continue;
        }
        // ---- positive command : colour-table backward copy / newline ----
        gFCRun = cmd;
        gFCX = X;
        if (cmd != 0) {
            if (clip == 0) {
                gFCCnt = 0;
                unsigned char *dst = reinterpret_cast<unsigned char *>(gFCRow + X);
                gFCDst = dst;
                if (cmd > 0) {
                    gFCCnt = cmd;
                    for (int k = cmd; k != 0; k--) {
                        unsigned char c = colorTable[*src++];
                        *dst-- = c;
                        gFCDst = dst;
                    }
                }
            } else if (clipY <= gFCY && gFCY <= gFCClipB) {
                int left = 1 + (X - cmd);
                if (left <= gFCClipR && clipX <= X + 1 - 1) {
                    unsigned int cn;
                    unsigned char *dst;
                    int skip;
                    if (gFCClipR < X - 1 + 1) {
                        dst = reinterpret_cast<unsigned char *>(gFCRow + gFCClipR);
                        src = src + (X - gFCClipR);
                        gFCDst = dst;
                        if ((X - cmd) < clipX) {
                            skip = gFCClipR + ((cmd - X) - clipW);
                            cn = clipW;
                            goto set_skip;
                        } else {
                            gFCSkip = 0;
                            cn = (cmd - X) + gFCClipR;
                        }
                    } else {
                        dst = reinterpret_cast<unsigned char *>(gFCRow + X);
                        gFCDst = dst;
                        if (left < clipX) {
                            cn = (X - clipX) + 1;
                            skip = cmd - cn;
                            goto set_skip;
                        } else {
                            gFCSkip = 0;
                            cn = cmd;
                        }
                    }
                    goto skip_set;
                set_skip:
                    gFCSkip = skip;
                skip_set:
                    gFCCnt2 = cn;
                    gFCCnt = 0;
                    if (static_cast<int>(cn) >= 0 + 1) {
                        gFCCnt = cn;
                        dst = gFCDst;
                        for (unsigned int k = cn; k != 0; k--) {
                            unsigned char c = colorTable[*src++];
                            *dst-- = c;
                            gFCDst = dst;
                        }
                    }
                    src = src + gFCSkip;
                }
            }
            X = X - cmd;
            gFCRun = cmd;
            continue;
        }
        // newline
        X = gFCXEnd;
        gFCY = gFCY + 1;
        gFCRow = gFCRow + pitch;
    }
}
