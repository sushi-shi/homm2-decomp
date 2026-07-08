// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block (0x534ca8+).
static IconEntry *gCTEntry;
static unsigned char *gCTSrc;
static unsigned char *gCTSrcCopy;
static unsigned char *gCTDst;
static unsigned char *gCTDimPal;
static int gCTX0;
static int gCTX;
static int gCTY;
static int gCTRow;
static int gCTPitch;
static int gCTClipR;
static int gCTClipB;
static unsigned int gCTRun;
static unsigned int gCTCnt;
static unsigned int gCTCnt2;
static unsigned int gCTDimLen;
static unsigned char gCTColor;

// @early-stop
// Colour-table variant of IconToBitmap: literal pixels are remapped through colorTable[] (per-pixel
// copy loop), solid runs fill colorTable[*src], and the dim step is gated on dimGate. X/row are
// register-locals; fill-before-dim block layout. Residual is the /O2 register-fusion wall.
VA(0x004d32a0, 0x5af)
void IconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                            int clip, int clipX, int clipY, int clipW, int clipH, int color,
                            unsigned char *colorTable, int dimGate)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    gCTEntry = &entries[frame];
    gCTSrc = data + entries[frame].srcOffset;
    gCTX0 = x + entries[frame].x;
    gCTPitch = dest->m_width;
    gCTY = entries[frame].y + y;
    if (clip != 0) {
        if (gCTX0 < clipX || clipW + clipX < entries[frame].w + gCTX0 || gCTY < clipY ||
            clipY + clipH < entries[frame].h + gCTY) {
            clip = 1;
            gCTClipR = clipX + clipW - 1;
            gCTClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    unsigned char *row =
        reinterpret_cast<unsigned char *>(gCTPitch * gCTY + reinterpret_cast<int>(dest->m_pixels));
    int X = gCTX0;
    for (;;) {
        int cmd = *gCTSrc++;
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & 0x40) == 0) {
                // skip run / end-of-sprite
                gCTX = X;
                gCTRow = reinterpret_cast<int>(row);
                gCTRun = cmd;
                if ((cmd & 0x3f) == 0)
                    return;
                X = X + (cmd & 0x3f);
                continue;
            }
            // 0xc0 - 0xff
            gCTRun = cmd;
            unsigned int count = cmd & 0x3f;
            int flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == 0xc1)
                    count = *gCTSrc++;
                gCTColor = colorTable[*gCTSrc];
                gCTSrc = gCTSrc + 1;
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = *gCTSrc++;
            count = flags & 3;
            if (count == 0)
                count = *gCTSrc++;
            gCTCnt2 = count;
            gCTDimLen = count;
            if (color != 0) {
                if (flags & 0x80) {
                    gCTColor = static_cast<unsigned char>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(row + X, gCTColor, count);
            } else {
                int right;
                if (clipY <= gCTY && gCTClipB >= gCTY &&
                    (right = X + count, clipX < right) && gCTClipR >= X) {
                    if (clipX > X) {
                        if (gCTClipR < right)
                            memset(row + clipX, gCTColor, clipW);
                        else
                            memset(row + clipX, gCTColor, (count - clipX) + X);
                    } else {
                        if (gCTClipR < right)
                            memset(row + X, gCTColor, (gCTClipR - X) + 1);
                        else
                            memset(row + X, gCTColor, count);
                    }
                }
            }
            X = X + count;
            gCTRun = count;
            continue;
        do_dim:
            gCTCnt = count;
            gCTRun = flags;
            if (flags & 0x40) {
                unsigned char *palette =
                    reinterpret_cast<unsigned char *>(uDimPal) + (flags & 0x3c) * 0x40;
                gCTDimPal = palette;
                if (clip == 0) {
                    gCTCnt = 0;
                    unsigned char *dp = row + X;
                    gCTDst = dp;
                    gCTCnt = count;
                    for (unsigned int k = count; k != 0; k--) {
                        if (dimGate != 0)
                            *dp = palette[*dp];
                        gCTDst = dp + 1;
                        dp = dp + 1;
                    }
                } else if (clipY <= gCTY && gCTClipB >= gCTY &&
                           (int)(X + count) > clipX && gCTClipR >= X) {
                    int right = X + count;
                    unsigned int cn;
                    unsigned char *dp;
                    if (clipX > X) {
                        cn = clipW;
                        if (right <= gCTClipR)
                            cn = (count - clipX) + X;
                        dp = row + clipX;
                    } else {
                        cn = count;
                        if (gCTClipR < right)
                            cn = (gCTClipR - X) + 1;
                        dp = row + X;
                    }
                    gCTCnt = 0;
                    gCTCnt = cn;
                    gCTCnt2 = cn;
                    gCTDst = dp;
                    for (unsigned int k = cn; k != 0; k--) {
                        if (dimGate != 0)
                            *dp = palette[*dp];
                        gCTDst = dp + 1;
                        dp = dp + 1;
                    }
                }
            }
            X = X + gCTCnt2;
            continue;
        }
        // ---- positive command : colour-table copy / newline ----
        gCTX = X;
        gCTRun = cmd;
        if (cmd != 0) {
            unsigned char *srcCopy = gCTSrc;
            unsigned int cnt = cmd;
            unsigned char *dst = row + X;
            if (clip == 0) {
                dst = row + X;
            } else if (gCTY < clipY || gCTClipB < gCTY ||
                       (int)(X + cmd) <= clipX || gCTClipR < X) {
                cnt = 0;
            } else if (clipX > X) {
                cnt = clipW;
                if (X + cmd <= gCTClipR)
                    cnt = (cmd - clipX) + X;
                dst = row + clipX;
                srcCopy = gCTSrc + (clipX - X);
            } else {
                dst = row + X;
                if (gCTClipR < X + cmd)
                    cnt = (gCTClipR - X) + 1;
            }
            gCTCnt = cnt;
            if (cnt != 0) {
                gCTSrcCopy = srcCopy;
                gCTCnt2 = 0;
                unsigned char *dp = dst;
                do {
                    unsigned char b = *gCTSrcCopy;
                    gCTDst = dp + 1;
                    gCTSrcCopy = gCTSrcCopy + 1;
                    *dp = colorTable[b];
                    cnt--;
                    dp = dp + 1;
                } while (cnt != 0);
            }
            gCTSrc = gCTSrc + cmd;
            X = X + cmd;
            gCTRun = cmd;
            continue;
        }
        // newline
        X = gCTX0;
        row = row + gCTPitch;
        gCTY = gCTY + 1;
    }
}
