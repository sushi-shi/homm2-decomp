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
DATA(0x00534ca8) static int gCTPitch;
DATA(0x00534cac) static unsigned int gCTCnt;
DATA(0x00534cb0) static int gCTX;
DATA(0x00534cb4) static int gCTY;
DATA(0x00534cb8) static IconEntry *gCTEntry;
DATA(0x00534cbc) static int gCTRow;
DATA(0x00534cc0) static unsigned char *gCTDimPal;
DATA(0x00534cc4) static int gCTClipB;
DATA(0x00534cc8) static int gCTClipR;
DATA(0x00534ccc) static unsigned char *gCTSrcCopy;
DATA(0x00534cd0) static unsigned int gCTDimLen;
DATA(0x00534cd4) static unsigned char *gCTDst;
DATA(0x00534cd8) static unsigned char *gCTSrc;
DATA(0x00534cdc) static unsigned int gCTCnt2;
DATA(0x00534ce0) static int gCTX0;
DATA(0x00534ce4) static unsigned char gCTColor;
DATA(0x00534ce8) static unsigned int gCTRun;

// Colour-table variant: literals and solid runs use colorTable[], and dimming is gated by dimGate.
// Header-state replay guard: docs/matching-matrices/icon2bc-tu-state-a0b17fb.tsv.
// @match-note
// /O2 structural checkpoint: both sides are FPO with one four-byte temporary and the complete
// skip/solid/shadow/fill/dim/literal/newline CFG has been traced; all 91 relocation occurrences
// and targets agree. The canonical first differs at +0x12: ours forms 13*frame in ECX and folds
// the entry base there, while retail retains data=ESI and 13*frame=EBX, loads entry.x in ECX and
// srcOffset in EAX, then forms entry=EDI and reuses ESI for X. A fresh byte-offset/SSA-lifetime
// family (split raw entry X, delayed typed-entry formation, pointer/reference/global views, and
// declaration splits) reached only 70.00% and either pushed EBP early or allocated entry=ECX;
// see icon2bc-structural-214bd52.tsv. No decoder publication/order change was indicated. Revisit
// only after a real declaration-surface change or with a model that directly yields that register split.
VA(0x004d32a0, 0x5af)
void IconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                            int clip, int clipX, int clipY, int clipW, int clipH, int color,
                            unsigned char *colorTable, int dimGate)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entries = reinterpret_cast<IconEntry *>(data);
    unsigned char *savedDst;
    gCTSrc = data + entries[frame].srcOffset;
    int X = x + entries[frame].x;
    gCTEntry = &entries[frame];
    gCTX0 = X;
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
    for (;;) {
        savedDst = gCTDst;
        gCTSrc++;
        int cmd = gCTSrc[-1];
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & 0x40) == 0) {
                // skip run / end-of-sprite
                gCTX = X;
                gCTRow = reinterpret_cast<int>(row);
                gCTDst = savedDst;
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
            gCTCnt2 = count;
            if (color != 0) {
                gCTRun = flags;
                if (flags & 0x80) {
                    gCTCnt = count;
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
                int fillLen;
                if (clipY <= gCTY && gCTClipB >= gCTY &&
                    (right = X + count, clipX < right) && gCTClipR >= X) {
                    if (X >= clipX) {
                        if (gCTClipR < right) {
                            fillLen = (gCTClipR - X) + 1;
                            memset(row + X, gCTColor, fillLen);
                        } else {
                            fillLen = count;
                            memset(row + X, gCTColor, fillLen);
                        }
                    } else {
                        if (gCTClipR < right) {
                            fillLen = clipW;
                            memset(row + clipX, gCTColor, fillLen);
                        } else {
                            fillLen = (count - clipX) + X;
                            memset(row + clipX, gCTColor, fillLen);
                        }
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
                gCTDst = savedDst;
                gCTDimPal = palette;
                if (clip == 0) {
                    savedDst = row + X;
                    gCTDimLen = 0;
                    if (static_cast<int>(count) > 0) {
                        gCTDimLen = count;
                        for (unsigned int k = count; k != 0; k--) {
                            if (dimGate != 0)
                                *savedDst = palette[*savedDst];
                            savedDst = savedDst + 1;
                            gCTDimPal = palette;
                        }
                    }
                } else {
                    gCTCnt = count;
                    if (clipY <= gCTY && gCTClipB >= gCTY &&
                        (int)(X + count) > clipX &&
                        (gCTDst = savedDst, gCTClipR >= X)) {
                        int right = X + count;
                        unsigned int cn;
                        if (X >= clipX) {
                            cn = count;
                            if (gCTClipR < right)
                                cn = (gCTClipR - X) + 1;
                            savedDst = row + X;
                        } else {
                            gCTCnt = count;
                            cn = clipW;
                            if (right <= gCTClipR)
                                cn = (count - clipX) + X;
                            savedDst = row + clipX;
                        }
                        gCTDimPal = palette;
                        gCTCnt = cn;
                        gCTDimLen = 0;
                        if (static_cast<int>(cn) > 0) {
                            gCTDimLen = cn;
                            for (unsigned int k = cn; k != 0; k--) {
                                if (dimGate != 0)
                                    *savedDst = palette[*savedDst];
                                savedDst = savedDst + 1;
                                gCTDimPal = palette;
                            }
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
        if (cmd != 0) {
            gCTSrcCopy = gCTSrc;
            gCTDst = savedDst;
            unsigned int cnt;
            if (clip == 0) {
                savedDst = row + X;
                cnt = cmd;
            } else if (gCTY < clipY || gCTClipB < gCTY ||
                       (int)(X + cmd) <= clipX ||
                       (gCTDst = savedDst, gCTClipR < X)) {
                cnt = 0;
            } else if (X >= clipX) {
                savedDst = row + X;
                cnt = cmd;
                if (gCTClipR < X + cmd)
                    cnt = (gCTClipR - X) + 1;
            } else {
                cnt = clipW;
                if (X + cmd <= gCTClipR)
                    cnt = (cmd - clipX) + X;
                savedDst = row + clipX;
                gCTSrcCopy = gCTSrc + (clipX - X);
            }
            gCTCnt = cnt;
            if (cnt != 0) {
                gCTDimLen = 0;
                if (static_cast<int>(cnt) > 0) {
                    gCTDimLen = cnt;
                    do {
                        unsigned char b = *gCTSrcCopy;
                        gCTSrcCopy = gCTSrcCopy + 1;
                        *savedDst = colorTable[b];
                        cnt--;
                        savedDst = savedDst + 1;
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
