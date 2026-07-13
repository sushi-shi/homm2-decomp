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
DATA(0x00534cb8) static IconEntry *gCTEntry;
DATA(0x00534cd8) static unsigned char *gCTSrc;
DATA(0x00534ccc) static unsigned char *gCTSrcCopy;
DATA(0x00534cd4) static unsigned char *gCTDst;
DATA(0x00534cc0) static unsigned char *gCTDimPal;
DATA(0x00534ce0) static int gCTX0;
DATA(0x00534cb0) static int gCTX;
DATA(0x00534cb4) static int gCTY;
DATA(0x00534cbc) static int gCTRow;
DATA(0x00534ca8) static int gCTPitch;
DATA(0x00534cc8) static int gCTClipR;
DATA(0x00534cc4) static int gCTClipB;
DATA(0x00534ce8) static unsigned int gCTRun;
DATA(0x00534cac) static unsigned int gCTCnt;
DATA(0x00534cdc) static unsigned int gCTCnt2;
DATA(0x00534cd0) static unsigned int gCTDimLen;
DATA(0x00534ce4) static unsigned char gCTColor;

// Colour-table variant of IconToBitmap: literal pixels are remapped through colorTable[], solid
// runs fill colorTable[*src], and the dim step is gated on dimGate.
// @early-stop
// /O2 source wall: base/retail block ends are dispatch 0x1b1/0x1ca, fill 0x2f7/0x331,
// dim 0x427/0x46a, and literal 0x537/0x587; .text is 0x560/0x5b0 with 89/91 relocs.
// Every decoder edge is present; the residual is duplicated inline-memset tails plus scratch-spill
// register allocation after verified branch reversal, cursor, signed-loop, and scratch-lifetime forms.
VA(0x004d32a0, 0x5af)
void IconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                            int clip, int clipX, int clipY, int clipW, int clipH, int color,
                            unsigned char *colorTable, int dimGate)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    int savedDimLen;
    IconEntry *entry = &entries[frame];
    gCTEntry = entry;
    gCTSrc = data + entry->srcOffset;
    int X = x + entry->x;
    gCTX0 = X;
    gCTPitch = dest->m_width;
    gCTY = entry->y + y;
    if (clip != 0) {
        if (gCTX0 < clipX || clipW + clipX < entry->w + gCTX0 || gCTY < clipY ||
            clipY + clipH < entry->h + gCTY) {
            clip = 1;
            gCTClipR = clipX + clipW - 1;
            gCTClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    unsigned char *row =
        reinterpret_cast<unsigned char *>(gCTPitch * gCTY + reinterpret_cast<int>(dest->m_pixels));
    for (;;) {
        savedDimLen = gCTDimLen;
        gCTSrc++;
        int cmd = gCTSrc[-1];
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & 0x40) == 0) {
                // skip run / end-of-sprite
                gCTX = X;
                gCTRow = reinterpret_cast<int>(row);
                gCTDimLen = savedDimLen;
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
                if (cmd == 0xc1) {
                    gCTSrc++;
                    count = gCTSrc[-1];
                }
                gCTSrc++;
                gCTColor = colorTable[gCTSrc[-1]];
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            gCTSrc++;
            flags = gCTSrc[-1];
            count = flags & 3;
            if (count == 0) {
                gCTSrc++;
                count = gCTSrc[-1];
            }
            gCTCnt = count;
            if (color != 0) {
                gCTRun = flags;
                if (flags & 0x80) {
                    gCTCnt2 = count;
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
                    if (X >= clipX) {
                        if (gCTClipR < right)
                            memset(row + X, gCTColor, (gCTClipR - X) + 1);
                        else
                            memset(row + X, gCTColor, count);
                    } else {
                        if (gCTClipR < right)
                            memset(row + clipX, gCTColor, clipW);
                        else
                            memset(row + clipX, gCTColor, (count - clipX) + X);
                    }
                }
            }
            X = X + count;
            gCTRun = count;
            continue;
        do_dim:
            gCTCnt2 = count;
            gCTRun = flags;
            gCTDimLen = savedDimLen;
            if (flags & 0x40) {
                unsigned char *palette =
                    reinterpret_cast<unsigned char *>(uDimPal) + (flags & 0x3c) * 0x40;
                gCTDimPal = palette;
                if (clip == 0) {
                    gCTDst = row + X;
                    gCTDimLen = 0;
                    if (static_cast<int>(count) > 0) {
                        gCTDimLen = count;
                        for (unsigned int k = count; k != 0; k--) {
                            if (dimGate != 0)
                                *gCTDst = palette[*gCTDst];
                            gCTDst = gCTDst + 1;
                        }
                    }
                } else if (clipY <= gCTY && gCTClipB >= gCTY &&
                           (int)(X + count) > clipX && gCTClipR >= X) {
                    int right = X + count;
                    unsigned int cn;
                    unsigned char *dp;
                    if (X >= clipX) {
                        cn = count;
                        if (gCTClipR < right)
                            cn = (gCTClipR - X) + 1;
                        dp = row + X;
                    } else {
                        cn = clipW;
                        if (right <= gCTClipR)
                            cn = (count - clipX) + X;
                        dp = row + clipX;
                    }
                    gCTDst = dp;
                    gCTDimLen = 0;
                    gCTCnt = cn;
                    gCTDimLen = cn;
                    if (static_cast<int>(cn) > 0) {
                        for (unsigned int k = cn; k != 0; k--) {
                            if (dimGate != 0)
                                *gCTDst = palette[*gCTDst];
                            gCTDst = gCTDst + 1;
                        }
                    }
                }
            }
            X = X + gCTCnt2;
            continue;
        }
        // ---- positive command : colour-table copy / newline ----
        gCTX = X;
        gCTRun = cmd;
        gCTDimLen = savedDimLen;
        if (cmd != 0) {
            gCTSrcCopy = gCTSrc;
            unsigned int cnt = cmd;
            unsigned char *dst = row + X;
            if (clip == 0) {
                dst = row + X;
            } else if (gCTY < clipY || gCTClipB < gCTY ||
                       (int)(X + cmd) <= clipX || gCTClipR < X) {
                cnt = 0;
            } else if (X >= clipX) {
                dst = row + X;
                if (gCTClipR < X + cmd)
                    cnt = (gCTClipR - X) + 1;
            } else {
                cnt = clipW;
                if (X + cmd <= gCTClipR)
                    cnt = (cmd - clipX) + X;
                dst = row + clipX;
                gCTSrcCopy = gCTSrc + (clipX - X);
            }
            gCTCnt = cnt;
            if (cnt != 0) {
                gCTDimLen = 0;
                unsigned char *dp = dst;
                if (static_cast<int>(cnt) > 0) {
                    gCTDimLen = cnt;
                    do {
                        unsigned char b = *gCTSrcCopy;
                        gCTDst = dp + 1;
                        gCTSrcCopy = gCTSrcCopy + 1;
                        *dp = colorTable[b];
                        cnt--;
                        dp = dp + 1;
                    } while (cnt != 0);
                }
            }
            X = X + cmd;
            gCTSrc = gCTSrc + cmd;
            gCTRun = cmd;
            continue;
        }
        // newline
        row = row + gCTPitch;
        X = gCTX0;
        gCTY = gCTY + 1;
    }
}
