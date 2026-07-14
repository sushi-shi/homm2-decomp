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

VA(0x004d9790, 0x54d)
void FlipIconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                                int clip, int clipX, int clipY, int clipW, int clipH, int color,
                                unsigned char *colorTable)
{
    unsigned char *src = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    int x0 = x;
    IconEntry *entry = reinterpret_cast<IconEntry *>(src) + frame;
    int w = entry->w;
    x0 = x0 - entry->x;
    x0 = x0 - w;
    gFCEntry = entry;
    src += entry->srcOffset;
    x0++;
    gFCX0 = x0;
    int X = w + x0 - 1;
    gFCXEnd = X;
    int Y = y + entry->y;
    gFCY = Y;
    if (clip != 0) {
        int currentY;
        if (x0 < clipX || clipW + clipX < x0 + w ||
            clipY > (currentY = gFCY) || clipY + clipH < entry->h + currentY) {
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
                // skip run / end-of-sprite
                gFCRun = cmd;
                int n = cmd & 0x3f;
                gFCX = X;
                gFCSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            // 0xc0 - 0xff
            gFCRun = cmd;
            unsigned int count = cmd & 0x3f;
            int flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == 0xc1)
                    count = *src++;
                gFCColor = colorTable[*src++];
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
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
                int currentY;
                int left;
                if ((currentY = gFCY) >= clipY && currentY <= gFCClipB &&
                    (left = (X - count) + 1, clipX <= left) && X <= gFCClipR) {
                    if (clipX <= left) {
                        memset(reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X),
                               gFCColor, count);
                    } else {
                        memset(reinterpret_cast<unsigned char *>(gFCRow + clipX), gFCColor,
                               (X - clipX) + 1);
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
                    unsigned char *dp = reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X);
                    gFCCnt = 0;
                    int dimCount = count;
                    gFCDimDst = dp;
                    if (dimCount > 0) {
                        gFCCnt = dimCount;
                        do {
                            unsigned char *dimPalette = gFCDimPal;
                            int px = *dp;
                            dp++;
                            count--;
                            gFCDimDst = dp;
                            dp[-1] = dimPalette[px];
                        } while (count != 0);
                    }
                } else {
                    int currentY;
                    gFCCnt2 = count;
                    if ((currentY = gFCY) >= clipY && currentY <= gFCClipB &&
                        clipX <= static_cast<int>((X - count) + 1) && X <= gFCClipR) {
                        int left = (X - count) + 1;
                        unsigned char *dp;
                        if (clipX <= left) {
                            dp = reinterpret_cast<unsigned char *>((gFCRow - count) + 1 + X);
                        } else {
                            count = (X - clipX) + 1;
                            dp = reinterpret_cast<unsigned char *>(gFCRow + clipX);
                        }
                        gFCCnt2 = count;
                        gFCDimDst = dp;
                        int dimCount = count;
                        gFCCnt = 0;
                        if (dimCount > 0) {
                            gFCCnt = dimCount;
                            do {
                                unsigned char *dimPalette = gFCDimPal;
                                int px = *dp;
                                dp++;
                                count--;
                                gFCDimDst = dp;
                                dp[-1] = dimPalette[px];
                            } while (count != 0);
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
                    int k = cmd;
                    do {
                        int c = *src++;
                        dst--;
                        gFCDst = dst;
                        dst[1] = colorTable[c];
                        k--;
                    } while (k != 0);
                }
            } else {
                int currentY = gFCY;
                if (currentY >= clipY && currentY <= gFCClipB) {
                    int left = (X - cmd) + 1;
                    if (left <= gFCClipR && clipX <= X) {
                        unsigned int cn;
                        if (X <= gFCClipR) {
                            gFCDst = reinterpret_cast<unsigned char *>(gFCRow + X);
                            if (clipX <= left) {
                                gFCSkip = 0;
                                cn = cmd;
                            } else {
                                cn = (X - clipX) + 1;
                                gFCSkip = cmd - cn;
                            }
                        } else {
                            int right = gFCClipR;
                            src = src + (X - right);
                            gFCDst = reinterpret_cast<unsigned char *>(gFCRow + right);
                            if (clipX <= (X - cmd)) {
                                gFCSkip = 0;
                                cn = (cmd - X) + gFCClipR;
                            } else {
                                gFCSkip = gFCClipR + ((cmd - X) - clipW);
                                cn = clipW;
                            }
                        }
                        gFCCnt = 0;
                        gFCCnt2 = cn;
                        if (static_cast<int>(cn) > 0) {
                            gFCCnt = cn;
                            do {
                                unsigned char *dst = gFCDst;
                                int c = *src++;
                                dst--;
                                cn--;
                                unsigned char mapped = colorTable[c];
                                gFCDst = dst;
                                dst[1] = mapped;
                            } while (cn != 0);
                        }
                        src = src + gFCSkip;
                    } else {
                        src = src + cmd;
                    }
                } else {
                    src = src + cmd;
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
