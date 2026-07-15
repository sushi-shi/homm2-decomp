// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2by.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconShear.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
// Per-call decoder scratch.
DATA(0x00538150) static int gYMClipB;
DATA(0x00538154) static int gYMDimIdx;
DATA(0x0053815c) static int gYMPitch;
DATA(0x00538158) static int gYMY;
DATA(0x00538160) static int gYMX;
DATA(0x00538164) static unsigned char *gYMDimDst;
DATA(0x0053816c) static int gYMX0;
DATA(0x00538168) static unsigned int gYMRun;
DATA(0x00538170) static unsigned char *gYMDimPal;
DATA(0x00538174) static unsigned char *gYMRow;
DATA(0x00538178) static unsigned char *gYMSrc;
DATA(0x00538180) static IconEntry *gYMEntry;
DATA(0x0053817c) static unsigned int gYMDimLen;
DATA(0x00538184) static unsigned char gYMColor;
DATA(0x00538188) static unsigned int gYMDimLen2;
DATA(0x0053818c) static int gYMClipR;

// @semantic
// Complete /O2 decoder at 90.177666%: candidate 0x579 versus retail 0x588. Both are frame-free and
// have the same 64 basic blocks, 99 directed CFG edges, and ordered successor topology (388 versus
// 394 instructions). Relocation-union masking leaves 669 comparable bytes, 394 different, first
// +0xb3 and last +0x577.
// The first code divergence is the bottom-clip comparison: candidate loads ClipB into EAX and uses
// `cmp Y,EAX; jg`, while retail preserves Y in EAX and uses `cmp ClipB,EAX; jl`. Relocations are
// candidate 129 versus retail 130 with every external target agreeing. The sole occurrence deficit
// is retail gYMX at target +0x48f (the absolute operand of the load at +0x48e) in the full literal-
// copy destination. Candidate reuses gYMX held in ESI from +0x468 at `add ESI,EDI` +0x48d; do not
// manufacture that reload. Setup also reverses Pitch/Y relocation order inside an otherwise
// identical multiply. The separate canonical IconShearConstant typedef enum intentionally lowers
// the prior macro state from 95.101524%, 0x584, 131/130 relocs to this state; correctness wins.
// In the prior macro state, a branch-local current-X removed its different excess relocation but
// fell to 94.42% and disrupted Src/Row/X order. Commuting the row multiply and using a typed row
// pointer were byte-neutral.
// Historical closed families: merged/four-arm copy/fill forms, combined/nested/goto clipping,
// comparison polarities, per-arm dim destinations, semantic locals, inclusive bounds, and 129 AST
// variants. Revisit only after a shared-header/TU-state change produces structural alignment at
// 96%+; do not treat this broad scheduling residual as a proven wall.
VA(0x004da270, 0x588)
void IconToBitmapYModify(class icon *srcIcon, class bitmap *dest, int x, int y, int frame, int clip,
                         int clipX, int clipY, int clipW, int clipH, int color, signed char *shear)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    gYMEntry = &entries[frame];
    gYMSrc = reinterpret_cast<unsigned char *>(srcIcon->m_data) + gYMEntry->srcOffset;
    gYMX0 = gYMEntry->x + x;
    gYMPitch = dest->m_width;
    gYMY = gYMEntry->y + y;
    gYMX = shear[gYMY] + gYMX0;
    gYMClipR = clipX + clipW - 1;
    gYMClipB = clipY + clipH - 1;
    gYMRow = dest->m_pixels + gYMPitch * gYMY;
    for (;;) {
        gYMRun = *gYMSrc;
        gYMSrc = gYMSrc + 1;
        if (static_cast<signed char>(gYMRun) < 0) {
            if ((gYMRun & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                if ((gYMRun & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                gYMX = gYMX + (gYMRun & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }
            // 0xc0 - 0xff
            if ((gYMRun & ICON_RLE_COMMAND_RUN_MASK) != 0) {
                if (gYMRun == ICON_RLE_LONG_SOLID_COMMAND) {
                    gYMRun = *gYMSrc;
                    gYMSrc = gYMSrc + 1;
                } else {
                    gYMRun = gYMRun & ICON_RLE_COMMAND_RUN_MASK;
                }
                gYMColor = *gYMSrc;
                gYMSrc = gYMSrc + 1;
                goto do_fill;
            }
            gYMRun = *gYMSrc;
            gYMSrc = gYMSrc + 1;
            if ((gYMRun & ICON_RLE_DIM_SHORT_COUNT_MASK) != 0) {
                gYMDimLen = gYMRun & ICON_RLE_DIM_SHORT_COUNT_MASK;
            } else {
                gYMDimLen = *gYMSrc;
                gYMSrc = gYMSrc + 1;
            }
            gYMDimLen2 = gYMDimLen;
            if (color != 0 && (gYMRun & ICON_RLE_DIM_RECOLOR_FLAG) != 0) {
                gYMRun = gYMDimLen;
                gYMColor = static_cast<unsigned char>(color);
                goto do_fill;
            }
            if ((gYMRun & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                gYMDimPal = reinterpret_cast<unsigned char *>(uDimPal) +
                            (gYMRun & ICON_RLE_DIM_LEVEL_MASK) *
                                ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                if (shear[gYMY] != ICON_SHEAR_SKIP_ROW && clipY <= gYMY &&
                    gYMClipB >= gYMY &&
                    static_cast<int>(gYMDimLen + gYMX) > clipX && gYMClipR >= gYMX) {
                    int dimRight = gYMDimLen + gYMX;
                    if (clipX <= gYMX) {
                        if (gYMClipR < dimRight)
                            gYMDimLen = (gYMClipR - gYMX) + 1;
                        gYMDimDst = gYMRow + gYMX;
                    } else {
                        if (gYMClipR >= dimRight)
                            gYMDimLen = gYMDimLen + (gYMX - clipX);
                        else
                            gYMDimLen = clipW;
                        gYMDimDst = gYMRow + clipX;
                    }
                    gYMDimIdx = 0;
                    if (0 < static_cast<int>(gYMDimLen)) {
                        do {
                            *gYMDimDst = gYMDimPal[*gYMDimDst];
                            gYMDimDst = 1 + gYMDimDst;
                            gYMDimIdx = gYMDimIdx + 1;
                        } while (gYMDimIdx < static_cast<int>(gYMDimLen));
                    }
                }
            }
            gYMX = gYMX + gYMDimLen2;
            continue;
        do_fill:
            if (shear[gYMY] != ICON_SHEAR_SKIP_ROW && clipY <= gYMY &&
                gYMClipB >= gYMY &&
                static_cast<int>(gYMX + gYMRun) > clipX && gYMClipR >= gYMX) {
                int fillRight = gYMX + gYMRun;
                if (clipX <= gYMX) {
                    if (gYMClipR >= fillRight) {
                        memset(gYMRow + gYMX, gYMColor, gYMRun);
                    } else {
                        memset(gYMRow + gYMX, gYMColor, (gYMClipR - gYMX) + 1);
                    }
                } else {
                    if (gYMClipR >= fillRight) {
                        memset(gYMRow + clipX, gYMColor, (gYMRun - clipX) + gYMX);
                    } else {
                        memset(gYMRow + clipX, gYMColor, clipW);
                    }
                }
            }
            gYMX = gYMX + gYMRun;
            continue;
        }
        // ---- positive command : literal copy / newline ----
        if (gYMRun != 0) {
            if (shear[gYMY] != ICON_SHEAR_SKIP_ROW && clipY <= gYMY &&
                gYMClipB >= gYMY &&
                static_cast<int>(gYMX + gYMRun) > clipX && gYMClipR >= gYMX) {
                int copyRight = gYMX + gYMRun;
                if (clipX <= gYMX) {
                    if (gYMClipR >= copyRight) {
                        memcpy(gYMRow + gYMX, gYMSrc, gYMRun);
                    } else {
                        memcpy(gYMRow + gYMX, gYMSrc, (gYMClipR - gYMX) + 1);
                    }
                } else {
                    if (gYMClipR >= copyRight) {
                        memcpy(gYMRow + clipX, gYMSrc + (clipX - gYMX),
                               (gYMRun - clipX) + gYMX);
                    } else {
                        memcpy(gYMRow + clipX, gYMSrc + (clipX - gYMX), clipW);
                    }
                }
            }
            gYMX = gYMX + gYMRun;
            gYMSrc = gYMSrc + gYMRun;
            continue;
        }
        // newline
        gYMX = shear[gYMY] + gYMX0;
        gYMY = gYMY + 1;
        gYMRow = gYMRow + gYMPitch;
    }
}
