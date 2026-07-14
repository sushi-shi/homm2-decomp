// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block (0x534c60+).
static int gFlipSkip;
static unsigned int gFlipRun;
static int gFlipX0;
static int gFlipXEnd;
static unsigned int gFlipCnt;
static unsigned int gFlipCnt2;
static int gFlipY;
static IconEntry *gFlipEntry;
static int gFlipX;
static unsigned char *gFlipSrc;
static unsigned char *gFlipDimPal;
static unsigned char *gFlipDimDst;
static int gFlipClipB;
static int gFlipRow;
static unsigned int gFlipDimLen;
static unsigned char gFlipColor;
static int gFlipClipR;
static unsigned char *gFlipDst;

VA(0x004d1ba0, 0x4f1)
void FlipIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                      int clip, int clipX, int clipY, int clipW, int clipH, int color)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    IconEntry *entry = &entries[frame];
    int x0 = x;
    int w = entry->w;
    unsigned char *src = data + entry->srcOffset;
    int entryY = entry->y;
    x0 = x0 - entry->x;
    x0 = x0 - w;
    gFlipEntry = entry;
    x0++;
    gFlipX0 = x0;
    int X = w + x0 - 1;
    gFlipXEnd = X;
    gFlipY = y + entryY;
    if (clip != 0) {
        if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w || gFlipY < clipY ||
            clipY + clipH < entry->h + gFlipY) {
            clip = 1;
            gFlipClipR = clipX + clipW - 1;
            gFlipClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    gFlipRow = gFlipY * pitch + reinterpret_cast<int>(dest->m_pixels);
    for (;;) {
        int cmd = *src++;
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & 0x40) == 0) {
                // skip run / end-of-sprite
                gFlipRun = cmd;
                int n = cmd & 0x3f;
                gFlipX = X;
                gFlipSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            // 0xc0 - 0xff
            gFlipRun = cmd;
            unsigned int count = cmd & 0x3f;
            int flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == 0xc1)
                    count = *src++;
                gFlipColor = *src++;
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = *src++;
            count = flags & 3;
            if (count == 0)
                count = *src++;
            gFlipCnt2 = count;
            if (color != 0) {
                gFlipRun = flags;
                if (flags & 0x80) {
                    gFlipDimLen = count;
                    gFlipColor = static_cast<unsigned char>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X), gFlipColor,
                       count);
            } else {
                int left;
                if (clipY <= (left = gFlipY) && left <= gFlipClipB &&
                    (left = (X - count) + 1, clipX <= left) && X <= gFlipClipR) {
                    if (clipX <= left) {
                        memset(reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X),
                               gFlipColor, count);
                    } else {
                        memset(reinterpret_cast<unsigned char *>(gFlipRow + clipX), gFlipColor,
                               (X - clipX) + 1);
                    }
                }
            }
            X = X - count;
            gFlipRun = count;
            continue;
        do_dim:
            gFlipRun = flags;
            gFlipDimLen = count;
            if (flags & 0x40) {
                unsigned char *palette =
                    reinterpret_cast<unsigned char *>(uDimPal) + (flags & 0x3c) * 0x40;
                gFlipDimPal = palette;
                if (clip == 0) {
                    unsigned char *dp =
                        reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X);
                    gFlipCnt = 0;
                    int dimCount = count;
                    gFlipDimDst = dp;
                    if (dimCount > 0) {
                        gFlipCnt = dimCount;
                        do {
                            unsigned char *dimPalette = gFlipDimPal;
                            int px = *dp++;
                            count--;
                            gFlipDimDst = dp;
                            dp[-1] = dimPalette[px];
                        } while (count != 0);
                    }
                } else {
                    const int currentY = gFlipY;
                    gFlipDimLen = count;
                    if (clipY <= currentY && currentY <= gFlipClipB &&
                        clipX <= static_cast<int>((X - count) + 1) && X <= gFlipClipR) {
                        int left = (X - count) + 1;
                        unsigned char *dp;
                        if (clipX <= left) {
                            dp = reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X);
                        } else {
                            count = (X - clipX) + 1;
                            dp = reinterpret_cast<unsigned char *>(gFlipRow + clipX);
                        }
                        int dimCount = count;
                        gFlipDimLen = count;
                        gFlipDimDst = dp;
                        gFlipCnt = 0;
                        if (dimCount > 0) {
                            gFlipCnt = dimCount;
                            do {
                                unsigned char *dimPalette = gFlipDimPal;
                                int px = *dp++;
                                count--;
                                gFlipDimDst = dp;
                                dp[-1] = dimPalette[px];
                            } while (count != 0);
                        }
                    }
                }
            }
            X = X - gFlipCnt2;
            continue;
        }
        // ---- positive command : literal copy / newline ----
        gFlipRun = cmd;
        gFlipX = X;
        if (cmd != 0) {
            if (clip == 0) {
                gFlipCnt = 0;
                unsigned char *dst = reinterpret_cast<unsigned char *>(gFlipRow + X);
                gFlipDst = dst;
                if (cmd > 0) {
                    gFlipCnt = cmd;
                    int k = cmd;
                    do {
                        unsigned char c = *src++;
                        *dst-- = c;
                        gFlipDst = dst;
                        k--;
                    } while (k != 0);
                }
            } else {
                int currentY = gFlipY;
                if (clipY <= currentY && currentY <= gFlipClipB) {
                    int left = (X - cmd) + 1;
                    if (left <= gFlipClipR && clipX <= X) {
                        unsigned int cn;
                        if (X <= gFlipClipR) {
                            gFlipDst = reinterpret_cast<unsigned char *>(gFlipRow + X);
                            if (clipX <= left) {
                                gFlipSkip = 0;
                                cn = cmd;
                            } else {
                                cn = (X - clipX) + 1;
                                gFlipSkip = cmd - cn;
                            }
                        } else {
                            cn = gFlipClipR;
                            src = src + (X - cn);
                            gFlipDst =
                                reinterpret_cast<unsigned char *>(gFlipRow + cn);
                            if (clipX <= (X - cmd)) {
                                gFlipSkip = 0;
                                cn = (cmd - X) + cn;
                            } else {
                                gFlipSkip = gFlipClipR + ((cmd - X) - clipW);
                                cn = clipW;
                            }
                        }
                        gFlipCnt = 0;
                        gFlipDimLen = cn;
                        if (static_cast<int>(cn) > 0) {
                            gFlipCnt = cn;
                            do {
                                unsigned char c = *src++;
                                (gFlipDst = gFlipDst - 1)[1] = c;
                                cn--;
                            } while (cn != 0);
                        }
                        src = src + gFlipSkip;
                    } else {
                        src = src + cmd;
                    }
                } else {
                    src = src + cmd;
                }
            }
            X = X - cmd;
            gFlipRun = cmd;
            continue;
        }
        // newline
        X = gFlipXEnd;
        gFlipY = gFlipY + 1;
        gFlipRow = gFlipRow + pitch;
    }
}
