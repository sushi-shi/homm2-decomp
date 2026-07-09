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

// @early-stop
// Horizontal-flip variant of IconToBitmap: sprite drawn right-to-left. X starts at gFlipXEnd and
// decreases; literal copy is a backward byte loop (*dst-- = *src++); solid fill/dim run leftward from
// (X-count+1). row base lives in the global gFlipRow; X and src are register-locals. /O2 reg wall.
VA(0x004d1ba0, 0x4f1)
void FlipIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                      int clip, int clipX, int clipY, int clipW, int clipH, int color)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    int w = entries[frame].w;
    gFlipEntry = &entries[frame];
    gFlipX0 = ((x - entries[frame].x) - w) + 1;
    gFlipXEnd = w + gFlipX0 - 1;
    gFlipY = y + entries[frame].y;
    if (clip != 0) {
        if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w || gFlipY < clipY ||
            clipY + clipH < entries[frame].h + gFlipY) {
            clip = 1;
            gFlipClipB = clipY + clipH - 1;
            gFlipClipR = clipX + clipW - 1;
        } else {
            clip = 0;
        }
    }
    short pitch = dest->m_width;
    gFlipRow = gFlipY * pitch + reinterpret_cast<int>(dest->m_pixels);
    int X = gFlipXEnd;
    unsigned char *src = data + entries[frame].srcOffset;
    for (;;) {
        int cmd = *src++;
        if (static_cast<signed char>(cmd) < 0) {
            gFlipRun = cmd;
            if ((cmd & 0x40) == 0) {
                // skip run / end-of-sprite
                int n = cmd & 0x3f;
                gFlipX = X;
                gFlipSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            // 0xc0 - 0xff
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
            gFlipDimLen = count;
            if (color != 0) {
                if (flags & 0x80) {
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
                if (clipY <= gFlipY && gFlipY <= gFlipClipB &&
                    (left = (X - count) + 1, left <= gFlipClipR) && clipX <= X) {
                    unsigned int cn = count;
                    unsigned char *dst;
                    if (left < clipX) {
                        cn = (X - clipX) + 1;
                        dst = reinterpret_cast<unsigned char *>(gFlipRow + clipX);
                    } else {
                        dst = reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X);
                    }
                    memset(dst, gFlipColor, cn);
                }
            }
            gFlipSrc = src;
            X = X - count;
            continue;
        do_dim:
            gFlipRun = flags;
            if (flags & 0x40) {
                unsigned char *palette =
                    reinterpret_cast<unsigned char *>(uDimPal) + (flags & 0x3c) * 0x40;
                gFlipDimPal = palette;
                if (clip == 0) {
                    gFlipCnt = 0;
                    unsigned char *dp = reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X);
                    gFlipDimDst = dp;
                    gFlipCnt = count;
                    for (unsigned int k = count; k != 0; k--) {
                        gFlipDimPal = palette;
                        gFlipDimDst = dp + 1;
                        *dp = palette[*dp];
                        dp = dp + 1;
                    }
                } else if (clipY <= gFlipY && gFlipY <= gFlipClipB &&
                           (int)((X - count) + 1) >= clipX && X <= gFlipClipR) {
                    int left = (X - count) + 1;
                    unsigned char *dp;
                    if (left < clipX) {
                        dp = reinterpret_cast<unsigned char *>(gFlipRow + clipX);
                        count = (X - clipX) + 1;
                    } else {
                        dp = reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X);
                    }
                    gFlipCnt = 0;
                    gFlipCnt = count;
                    gFlipCnt2 = count;
                    gFlipDimDst = dp;
                    if (static_cast<int>(count) > 0) {
                        for (unsigned int k = count; k != 0; k--) {
                            gFlipDimDst = dp + 1;
                            *dp = palette[*dp];
                            dp = dp + 1;
                        }
                    }
                }
            }
            gFlipSrc = src;
            X = X - gFlipDimLen;
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
                gFlipCnt = cmd;
                for (int k = cmd; k != 0; k--) {
                    unsigned char c = *src++;
                    *dst-- = c;
                    gFlipDst = dst;
                }
            } else if (clipY <= gFlipY && gFlipY <= gFlipClipB) {
                int left = (X - cmd) + 1;
                if (left <= gFlipClipR && clipX <= X) {
                    unsigned int cn;
                    unsigned char *dst;
                    if (gFlipClipR < X) {
                        dst = reinterpret_cast<unsigned char *>(gFlipRow + gFlipClipR);
                        src = src + (X - gFlipClipR);
                        if ((X - cmd) < clipX) {
                            gFlipSkip = gFlipClipR + ((cmd - X) - clipW);
                            cn = clipW;
                        } else {
                            gFlipSkip = 0;
                            cn = (cmd - X) + gFlipClipR;
                        }
                    } else {
                        dst = reinterpret_cast<unsigned char *>(gFlipRow + X);
                        if (left < clipX) {
                            cn = (X - clipX) + 1;
                            gFlipSkip = cmd - cn;
                        } else {
                            gFlipSkip = 0;
                            cn = cmd;
                        }
                    }
                    gFlipCnt = 0;
                    gFlipCnt2 = cn;
                    if (static_cast<int>(cn) > 0) {
                        gFlipCnt = cn;
                        for (unsigned int k = cn; k != 0; k--) {
                            unsigned char c = *src++;
                            *dst-- = c;
                            gFlipDst = dst;
                        }
                    }
                    src = src + gFlipSkip;
                }
            }
            gFlipSrc = src;
            X = X - cmd;
            continue;
        }
        // newline
        gFlipRow = gFlipRow + pitch;
        X = gFlipXEnd;
        gFlipY = gFlipY + 1;
    }
}
