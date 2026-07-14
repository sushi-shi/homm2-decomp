// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\iconf2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconf2bc.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
DATA(0x005380d4) static int gFCSkip;
DATA(0x00538104) static unsigned int gFCRun;
DATA(0x005380f8) static int gFCX0;
DATA(0x005380e4) static int gFCXEnd;
DATA(0x005380e8) static unsigned int gFCCnt;
DATA(0x005380e0) static unsigned int gFCCnt2;
DATA(0x005380d8) static int gFCY;
DATA(0x005380ec) static IconEntry *gFCEntry;
DATA(0x005380d0) static int gFCX;
DATA(0x005380f0) static unsigned char *gFCSrc;
DATA(0x005380dc) static unsigned char *gFCDimPal;
DATA(0x005380f4) static unsigned char *gFCDimDst;
DATA(0x005380fc) static int gFCClipB;
DATA(0x00538100) static unsigned char *gFCRow;
DATA(0x005380c4) static unsigned int gFCDimLen;
DATA(0x005380cc) static unsigned char gFCColor;
DATA(0x005380c8) static int gFCClipR;
DATA(0x005380c0) static unsigned char *gFCDst;

// @match-note
// Functionally complete typed reconstruction with canonical IconRle enum constants, dim-palette
// owner, byte-pointer row storage, and retail-proven DATA owners for every file-static scratch.
// Fresh ee1391f audit: live 85.27% (retained maximum 86.2544%), candidate 0x543 versus retail
// 0x54d, 395/397 instructions, the same 76 ordered blocks/59 branches/successor vectors, and the
// same eight-byte frame. Relocations are 84/83 with no wrong owner: every occurrence count agrees
// except gFCY 9/8. Source has one snapshot shared by the initial vertical clauses, but candidate reloads
// gFCY at +0x7b/+0x89 while retail loads it once at +0x77 and retains ECX for the bottom test.
// Removing that excess from the ordered static-owner stream leaves one adjacent order difference
// in the right-clipped literal arm: candidate ClipR/Skip versus retail Skip/ClipR. Source already
// stores zero to Skip before the ClipR expression; the recorded staged/common-tail forms optimize
// away or regress, so this is part of the same allocator residual rather than missing side effects.
// First raw divergence remains +0x0b: candidate keeps icon data in EBX while retail uses ESI.
// Retail then spills icon width at [esp+0x14] for the horizontal upper edge and retains the Y load;
// the documented width/pitch/snapshot/staged-skip variants either optimize away or add a false
// third frame word. The retail CodeView `.c` basename and exact `C:\proj\BASE` path in C++ mode are
// text-identical. C89-style within-scope declaration ordering regresses to 82.56%/85:83; isolating
// its downstream-only portion gives 83.60%/84:83. Splitting the general/dim/mono serialized
// constants into separate typedef enums gives 84.33%/84:83, while
// splitting only mono gives 84.10%/85:83; both were reverted. Declaring statics in ascending proven
// retail address order gives 84.43%/85:83, so only the DATA annotations were retained.
// This is an unresolved setup/source-shape residual, not a byte-proven wall. No permuter was used.
VA(0x004d9790, 0x54d)
void FlipIconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                                int clip, int clipX, int clipY, int clipW, int clipH, int color,
                                unsigned char *colorTable)
{
    unsigned char *src = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    int x0 = x;
    int w;
    IconEntry *entry = reinterpret_cast<IconEntry *>(src) + frame;
    int pitch;
    w = entry->w;
    x0 = x0 - entry->x;
    x0 = x0 - w;
    gFCEntry = entry;
    src += entry->srcOffset;
    x0++;
    pitch = w;
    gFCX0 = x0;
    w = w + x0;
    int X = (gFCXEnd = w - 1);
    int Y = y + entry->y;
    gFCY = Y;
    if (clip != 0) {
        int currentY = gFCY;
        if (x0 < clipX || clipW + clipX < x0 + pitch ||
            currentY < clipY || clipY + clipH < entry->h + currentY) {
            clip = 1;
            gFCClipR = clipX + clipW - 1;
            gFCClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    pitch = dest->m_width;
    gFCRow = dest->m_pixels + gFCY * pitch;
    for (;;) {
        int cmd = *src++;
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                gFCRun = cmd;
                int n = cmd & ICON_RLE_COMMAND_RUN_MASK;
                gFCX = X;
                gFCSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            // 0xc0 - 0xff
            gFCRun = cmd;
            unsigned int count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            int flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == ICON_RLE_LONG_SOLID_COMMAND)
                    count = *src++;
                gFCColor = colorTable[*src++];
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = *src++;
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0)
                count = *src++;
            gFCDimLen = count;
            if (color != 0) {
                gFCRun = flags;
                if (flags & ICON_RLE_DIM_RECOLOR_FLAG) {
                    gFCCnt2 = count;
                    gFCColor = static_cast<unsigned char>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset((gFCRow - count) + 1 + X, gFCColor, count);
            } else {
                int currentY = gFCY;
                int left;
                if (currentY >= clipY && currentY <= gFCClipB &&
                    (left = (X - count) + 1, clipX <= left) && X <= gFCClipR) {
                    if (clipX <= left) {
                        memset((gFCRow - count) + 1 + X, gFCColor, count);
                    } else {
                        memset(gFCRow + clipX, gFCColor, (X - clipX) + 1);
                    }
                }
            }
            X = X - count;
            gFCRun = count;
            continue;
        do_dim:
            gFCRun = flags;
            gFCCnt2 = count;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                unsigned char *palette =
                    &uDimPal[0][0][0] +
                    (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                gFCDimPal = palette;
                if (clip == 0) {
                    unsigned char *dp = (gFCRow - count) + 1 + X;
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
                    int currentY = gFCY;
                    gFCCnt2 = count;
                    if (currentY >= clipY && currentY <= gFCClipB &&
                        clipX <= static_cast<int>((X - count) + 1) && X <= gFCClipR) {
                        int left = (X - count) + 1;
                        unsigned char *dp;
                        if (clipX <= left) {
                            dp = (gFCRow - count) + 1 + X;
                        } else {
                            count = (X - clipX) + 1;
                            dp = gFCRow + clipX;
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
                unsigned char *dst = gFCRow + X;
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
                        unsigned char *selectedDst;
                        int skip;
                        if (X <= gFCClipR) {
                            selectedDst = gFCRow + X;
                            gFCDst = selectedDst;
                            if (clipX <= left) {
                                gFCSkip = 0;
                                cn = cmd;
                            } else {
                                cn = (X - clipX) + 1;
                                skip = cmd - cn;
                                goto set_skip;
                            }
                        } else {
                            int right = gFCClipR;
                            src = src + (X - right);
                            selectedDst = gFCRow + right;
                            gFCDst = selectedDst;
                            if (clipX <= (X - cmd)) {
                                gFCSkip = 0;
                                cn = (cmd - X) + gFCClipR;
                            } else {
                                skip = gFCClipR + ((cmd - X) - clipW);
                                cn = clipW;
                                goto set_skip;
                            }
                        }
                        goto skip_set;
                    set_skip:
                        gFCSkip = skip;
                    skip_set:
                        int copyCount = cn;
                        gFCCnt2 = cn;
                        gFCCnt = 0;
                        if (copyCount > 0) {
                            gFCCnt = copyCount;
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
