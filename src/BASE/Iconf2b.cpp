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
// /O2 register-allocation wall after semantic recovery. Final base is 0x509 bytes versus retail
// 0x4f1, with the same 8-byte frame; the decode loop starts at +0xde versus +0xe3, the positive-run
// path at +0x394 versus +0x35f, newline at +0x4d5 versus +0x4bf, and return at +0x4ff versus
// +0x4e7. Relocations are 88 versus 81, with no base-only external target. A local dim-pointer form
// reaches the exact 0x4f1 size and 83 relocations but changes the frame to 4 bytes and drops objdiff
// similarity. Also tried local/global x bounds, early/late source formation, volatile width/run,
// split/common clipped fills, local/global dim loops, offset temporaries, and 180 AST permutations.
VA(0x004d1ba0, 0x4f1)
void FlipIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                      int clip, int clipX, int clipY, int clipW, int clipH, int color)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    int w = entries[frame].w;
    gFlipX0 = ((x - entries[frame].x) - w) + 1;
    gFlipEntry = &entries[frame];
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
                if (clipY <= gFlipY && gFlipY <= gFlipClipB &&
                    (left = (X - count) + 1, clipX <= left) && X <= gFlipClipR) {
                    if (left < clipX) {
                        memset(reinterpret_cast<unsigned char *>(gFlipRow + clipX), gFlipColor,
                               (X - clipX) + 1);
                    } else {
                        memset(reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X),
                               gFlipColor, count);
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
                    gFlipCnt = 0;
                    gFlipDimDst = reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X);
                    if (static_cast<int>(count) > 0) {
                        gFlipCnt = count;
                        do {
                            *gFlipDimDst = gFlipDimPal[*gFlipDimDst];
                            gFlipDimDst++;
                            count--;
                        } while (count != 0);
                    }
                } else if (clipY <= gFlipY && gFlipY <= gFlipClipB &&
                           clipX <= static_cast<int>((X - count) + 1) && X <= gFlipClipR) {
                    int left = (X - count) + 1;
                    if (left < clipX) {
                        gFlipDimDst = reinterpret_cast<unsigned char *>(gFlipRow + clipX);
                        count = (X - clipX) + 1;
                    } else {
                        gFlipDimDst =
                            reinterpret_cast<unsigned char *>((gFlipRow - count) + 1 + X);
                    }
                    gFlipCnt2 = count;
                    gFlipCnt = 0;
                    if (static_cast<int>(count) > 0) {
                        gFlipCnt = count;
                        do {
                            *gFlipDimDst = gFlipDimPal[*gFlipDimDst];
                            gFlipDimDst++;
                            count--;
                        } while (count != 0);
                    }
                }
            }
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
                if (cmd >= 0 + 1) {
                    gFlipCnt = cmd;
                    int k = cmd;
                    do {
                        unsigned char c = *src++;
                        *dst-- = c;
                        gFlipDst = dst;
                        k--;
                    } while (k != 0);
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
                } else {
                    src = src + cmd;
                }
            } else {
                src = src + cmd;
            }
            X = X - cmd;
            gFlipRun = cmd;
            continue;
        }
        // newline
        gFlipRow = gFlipRow + pitch;
        X = gFlipXEnd;
        gFlipY = gFlipY + 1;
    }
}
