// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icon2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icon2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
// Per-call decoder scratch — its own 0x534c20+ file-static block.
DATA(0x00534c20) static unsigned char *gIcRow;
DATA(0x00534c24) static int gIcPitch;
DATA(0x00534c28) static unsigned char gIcColor;
DATA(0x00534c2c) static unsigned char *gIcDimPal;
DATA(0x00534c30) static unsigned int gIcRun;
DATA(0x00534c34) static unsigned int gIcCnt;
DATA(0x00534c38) static unsigned char *gIcSrc;
DATA(0x00534c3c) static unsigned char *gIcDimDst;
DATA(0x00534c40) static int gIcClipR;
DATA(0x00534c44) static int gIcClipB;
DATA(0x00534c48) static int gIcX0;
DATA(0x00534c4c) static unsigned int gIcDimLen;
DATA(0x00534c50) static int gIcY;
DATA(0x00534c54) static int gIcX;
DATA(0x00534c58) static IconEntry *gIcEntry;
DATA(0x00534c5c) static unsigned int gIcCnt2;

// @match-note
// Macro-neutral structural checkpoint: complete no-frame CFG and correct relocation targets.
// First divergence is +0x11: ours hoists x before the indexed entry fields; retail retains the
// 13-byte frame offset in EBX, reads entry.x/srcOffset, forms EDI, then publishes entry/source/X0.
// Counts are 80/83: only X0 2/3, Y 7/8, and Cnt2 4/5 differ. Retail reloads X0 at +0x5a
// (reloc +0x5c), Y for clipping at +0x78 (reloc +0x7a), and Y for the row at +0xcf
// (reloc +0xd1); ours forwards the live X/Y values. Retail's fifth Cnt2 store is +0x387
// (reloc +0x389), in the right-clipped dim arm after the ClipR load and before its comparison;
// the corresponding source publication is removed as redundant. Scoped X/Y snapshots collapse
// byte-identically, while direct scratch ownership overshoots (X0 5/3, Y 10/8). Making Cnt2 feed
// the arm emits six occurrences with a non-retail global reload; reversed/separate assignments
// collapse to four. A typed IconEntry array root also remains 80/83 and worsens the setup shape.
// This is an unresolved compiler-state residual, not a proven wall. Revisit after a real reachable
// type/header change; no permutation tool was used.
VA(0x004d0570, 0x4ed)
void IconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                  int clip, int clipX, int clipY, int clipW, int clipH, int color)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    int entryOffset = frame * sizeof(IconEntry);
    int entryX = reinterpret_cast<IconEntry *>(data + entryOffset)->x;
    int srcOffset = reinterpret_cast<IconEntry *>(data + entryOffset)->srcOffset;
    IconEntry *entry = reinterpret_cast<IconEntry *>(data + entryOffset);
    unsigned char *cursor = data + srcOffset;
    gIcEntry = entry;
    gIcSrc = cursor;
    int X = x;
    X += entryX;
    int Y = entry->y;
    Y += y;
    gIcX0 = X;
    gIcPitch = dest->m_width;
    gIcY = Y;
    if (clip != 0) {
        if (gIcX0 < clipX || clipW + clipX < entry->w + gIcX0 || gIcY < clipY ||
            clipY + clipH < entry->h + gIcY) {
            clip = 1;
            gIcClipR = clipX + clipW - 1;
            gIcClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    unsigned char *row = dest->m_pixels + gIcPitch * gIcY;
    for (;;) {
        gIcSrc++;
        int cmd = gIcSrc[-1];
        if (static_cast<signed char>(cmd) < 0) {
            // ---- negative command ----
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                gIcX = X;
                gIcRow = row;
                gIcRun = cmd;
                if ((cmd & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                X = X + (cmd & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }
            // 0xc0 - 0xff
            gIcRun = cmd;
            unsigned int count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            int flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == ICON_RLE_LONG_SOLID_COMMAND) {
                    gIcSrc++;
                    count = gIcSrc[-1];
                }
                gIcSrc++;
                gIcColor = gIcSrc[-1];
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            gIcSrc++;
            flags = gIcSrc[-1];
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0) {
                gIcSrc++;
                count = gIcSrc[-1];
            }
            gIcDimLen = count;
            if (color != 0) {
                gIcRun = flags;
                if (flags & ICON_RLE_DIM_RECOLOR_FLAG) {
                    gIcCnt2 = count;
                    gIcColor = static_cast<unsigned char>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(row + X, gIcColor, count);
            } else {
                int right;
                if (clipY <= gIcY && gIcClipB >= gIcY &&
                    (right = X + count, clipX < right) && gIcClipR >= X) {
                    if (clipX <= X) {
                        if (gIcClipR >= right)
                            memset(row + X, gIcColor, count);
                        else
                            memset(row + X, gIcColor, (gIcClipR - X) + 1);
                    } else {
                        unsigned int cn = clipW;
                        if (right <= gIcClipR)
                            cn = (count - clipX) + X;
                        memset(row + clipX, gIcColor, cn);
                    }
                }
            }
            X = X + count;
            gIcRun = count;
            continue;
        do_dim:
            gIcCnt2 = count;
            gIcRun = flags;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                unsigned int lvl =
                    (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                unsigned char *palette = &uDimPal[0][0][0] + lvl;
                if (clip == 0) {
                    unsigned char *dp = row + X;
                    gIcDimPal = palette;
                    gIcDimDst = dp;
                    gIcCnt = 0;
                    if (static_cast<int>(count) > 0) {
                        gIcCnt = count;
                        do {
                            int px = *dp++;
                            gIcDimDst = dp;
                            count--;
                            gIcDimPal = palette;
                            dp[-1] = palette[px];
                        } while (count != 0);
                    }
                } else {
                    gIcCnt2 = count;
                    gIcDimPal = palette;
                    int right;
                    if (clipY <= gIcY && gIcClipB >= gIcY &&
                        (right = X + count, clipX < right) && gIcClipR >= X) {
                        unsigned int cn;
                        unsigned char *dst;
                        if (clipX <= X) {
                            cn = count;
                            if (gIcClipR < right)
                                cn = (gIcClipR - X) + 1;
                            dst = row + X;
                        } else {
                            gIcCnt2 = count;
                            if (gIcClipR < right)
                                cn = clipW;
                            else
                                cn = (count - clipX) + X;
                            dst = row + clipX;
                        }
                        gIcCnt2 = cn;
                        gIcDimDst = dst;
                        gIcDimPal = palette;
                        gIcCnt = 0;
                        if (static_cast<int>(cn) > 0) {
                            gIcCnt = cn;
                            do {
                                int px = *dst++;
                                gIcDimDst = dst;
                                cn--;
                                gIcDimPal = palette;
                                dst[-1] = palette[px];
                            } while (cn != 0);
                        }
                    }
                }
            }
            X = X + gIcDimLen;
            continue;
        }
        // ---- positive command : literal copy / newline ----
        gIcX = X;
        gIcRun = cmd;
        if (cmd != 0) {
            int right;
            unsigned int copyCount;
            unsigned char *copyDst;
            unsigned char *copySrc;
            do {
                if (clip == 0) {
                    copyCount = cmd;
                    copyDst = row + X;
                    copySrc = gIcSrc;
                } else {
                    if (gIcY < clipY || gIcClipB < gIcY)
                        break;
                    right = X + cmd;
                    if (right <= clipX || gIcClipR < X)
                        break;
                    if (clipX <= X) {
                        if (gIcClipR >= right) {
                            copyCount = cmd;
                            copyDst = row + X;
                            copySrc = gIcSrc;
                        } else {
                            copyCount = (gIcClipR - X) + 1;
                            copyDst = row + X;
                            copySrc = gIcSrc;
                        }
                    } else {
                        if (gIcClipR >= right)
                            copyCount = (cmd - clipX) + X;
                        else
                            copyCount = clipW;
                        copySrc = gIcSrc + (clipX - X);
                        copyDst = row + clipX;
                    }
                }
                memcpy(copyDst, copySrc, copyCount);
            } while (0);
            X = X + cmd;
            gIcSrc = gIcSrc + cmd;
            gIcRun = cmd;
            continue;
        }
        // newline
        X = gIcX0;
        gIcY = gIcY + 1;
        row = row + gIcPitch;
    }
}
