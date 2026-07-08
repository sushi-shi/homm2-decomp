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
// Flip + colour-table variant: horizontal-flip decoder (X decreases, backward copy) where literal
// pixels are remapped through colorTable[] and solid runs fill colorTable[*src]. row is global
// gFCRow; X and src are register-locals; fill-before-dim layout. Residual is the /O2 reg wall.
VA(0x004d9790, 0x54d)
void FlipIconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                                int clip, int clipX, int clipY, int clipW, int clipH, int color,
                                unsigned char *colorTable)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    int w = entries[frame].w;
    gFCEntry = &entries[frame];
    gFCX0 = ((x - entries[frame].x) - w) + 1;
    gFCXEnd = w + gFCX0 - 1;
    gFCY = y + entries[frame].y;
    if (clip != 0) {
        if (gFCX0 < clipX || clipW + clipX < gFCX0 + w || gFCY < clipY ||
            clipY + clipH < entries[frame].h + gFCY) {
            clip = 1;
            gFCClipR = clipX + clipW - 1;
            gFCClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    gFCRow = gFCY * pitch + reinterpret_cast<int>(dest->m_pixels);
    int X = gFCXEnd;
    unsigned char *src = data + entries[frame].srcOffset;
    for (;;) {
        int cmd = *src++;
        if (static_cast<signed char>(cmd) < 0) {
            gFCRun = cmd;
            if ((cmd & 0x40) == 0) {
                int n = cmd & 0x3f;
                gFCX = X;
                gFCSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
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
            gFCCnt2 = count;
            gFCDimLen = count;
            if (color != 0) {
                if (flags & 0x80) {
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
                    (left = (X - count) + 1, left <= gFCClipR) && clipX <= X) {
                    unsigned int cn = count;
                    unsigned char *dst;
                    if (left < clipX) {
                        cn = (X - clipX) + 1;
                        dst = reinterpret_cast<unsigned char *>(gFCRow + clipX);
                    } else {
                        dst = reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X);
                    }
                    memset(dst, gFCColor, cn);
                }
            }
            gFCSrc = src;
            X = X - count;
            continue;
        do_dim:
            gFCRun = flags;
            if (flags & 0x40) {
                unsigned char *palette =
                    reinterpret_cast<unsigned char *>(uDimPal) + (flags & 0x3c) * 0x40;
                gFCDimPal = palette;
                if (clip == 0) {
                    gFCCnt = 0;
                    unsigned char *dp = reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X);
                    gFCDimDst = dp;
                    gFCCnt = count;
                    for (unsigned int k = count; k != 0; k--) {
                        gFCDimDst = dp + 1;
                        *dp = palette[*dp];
                        dp = dp + 1;
                    }
                } else if (clipY <= gFCY && gFCY <= gFCClipB &&
                           (int)((X - count) + 1) >= clipX && X <= gFCClipR) {
                    int left = (X - count) + 1;
                    unsigned char *dp;
                    if (left < clipX) {
                        dp = reinterpret_cast<unsigned char *>(gFCRow + clipX);
                        count = (X - clipX) + 1;
                    } else {
                        dp = reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X);
                    }
                    gFCCnt = 0;
                    gFCCnt = count;
                    gFCCnt2 = count;
                    gFCDimDst = dp;
                    if (static_cast<int>(count) > 0) {
                        for (unsigned int k = count; k != 0; k--) {
                            gFCDimDst = dp + 1;
                            *dp = palette[*dp];
                            dp = dp + 1;
                        }
                    }
                }
            }
            gFCSrc = src;
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
                gFCCnt = cmd;
                for (int k = cmd; k != 0; k--) {
                    unsigned char c = colorTable[*src++];
                    *dst-- = c;
                    gFCDst = dst;
                }
            } else if (clipY <= gFCY && gFCY <= gFCClipB) {
                int left = (X - cmd) + 1;
                if (left <= gFCClipR && clipX <= X) {
                    unsigned int cn;
                    unsigned char *dst;
                    if (gFCClipR < X) {
                        dst = reinterpret_cast<unsigned char *>(gFCRow + gFCClipR);
                        src = src + (X - gFCClipR);
                        if ((X - cmd) < clipX) {
                            gFCSkip = gFCClipR + ((cmd - X) - clipW);
                            cn = clipW;
                        } else {
                            gFCSkip = 0;
                            cn = (cmd - X) + gFCClipR;
                        }
                    } else {
                        dst = reinterpret_cast<unsigned char *>(gFCRow + X);
                        if (left < clipX) {
                            cn = (X - clipX) + 1;
                            gFCSkip = cmd - cn;
                        } else {
                            gFCSkip = 0;
                            cn = cmd;
                        }
                    }
                    gFCCnt = 0;
                    gFCCnt2 = cn;
                    if (static_cast<int>(cn) > 0) {
                        gFCCnt = cn;
                        for (unsigned int k = cn; k != 0; k--) {
                            unsigned char c = colorTable[*src++];
                            *dst-- = c;
                            gFCDst = dst;
                        }
                    }
                    src = src + gFCSkip;
                }
            }
            gFCSrc = src;
            X = X - cmd;
            continue;
        }
        // newline
        X = gFCXEnd;
        gFCRow = gFCRow + pitch;
        gFCY = gFCY + 1;
    }
}
