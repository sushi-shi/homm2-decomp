// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/IconEntry.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block (0x534c60+).
DATA(0x00534c60) static int gFlipSkip;
DATA(0x00534c64) static unsigned int gFlipRun;
DATA(0x00534c68) static int gFlipX0;
DATA(0x00534c6c) static int gFlipXEnd;
DATA(0x00534c70) static unsigned int gFlipCnt;
DATA(0x00534c74) static unsigned int gFlipCnt2;
DATA(0x00534c78) static int gFlipY;
DATA(0x00534c7c) static IconEntry *gFlipEntry;
DATA(0x00534c80) static int gFlipX;
DATA(0x00534c84) static unsigned char *gFlipSrc;
DATA(0x00534c88) static unsigned char *gFlipDimPal;
DATA(0x00534c8c) static unsigned char *gFlipDimDst;
DATA(0x00534c90) static int gFlipClipB;
DATA(0x00534c94) static int gFlipRow;
DATA(0x00534c98) static unsigned int gFlipDimLen;
DATA(0x00534c9c) static unsigned char gFlipColor;
DATA(0x00534ca0) static int gFlipClipR;
DATA(0x00534ca4) static unsigned char *gFlipDst;

// @match-note
// a0b17fb combined-root state: complete CFG and sub esp,8 frame; candidate .text is 0x4e5
// bytes versus the 0x4f1-byte retail function, and the decoder begins at +0xe9 versus +0xe3.
// The first unmasked divergence is +0x1d: candidate loads entry Y into EBP before forming the
// entry pointer; retail forms EDI, loads entry X into EBX, subtracts it, then loads Y. The
// relocation-union audit masks 82/81 relocation payloads and leaves 609 differing bytes among
// 678 common unmasked bytes plus a 0xc-byte retail tail. The sole count excess is gFlipY (9/8).
// This TU has no function predecessor. Exact-preserving include placements, entry publication,
// and Y-sum order were byte-identical; the measured local/sibling setup variants regressed. This
// is not a wall or permitted stop. Retry after a retained shared-header/declaration/compiler-state
// change; do not replay the a0b17fb matrix: iconf2b-tu-state-a0b17fb.tsv.
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
