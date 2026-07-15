// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\iconf2by.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconf2by.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconShear.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block.
static IconEntry *gFYEntry;
static unsigned char *gFYSrc;
static int gFYX0;
static int gFYXEnd;
static int gFYY;
static int gFYX;
static int gFYClipB;
static unsigned char *gFYRow;
static int gFYRun;
static unsigned char gFYColor;
static int gFYDimLen;
static unsigned int gFYDimLen2;
static unsigned char *gFYDimPal;
static int gFYDimIdx;
static unsigned char *gFYDimDst;
static unsigned char *gFYDst;
static int gFYSkip;
static int gFYClipR;

static inline int IconRowVisible(signed char *shear, int clipTop)
{
    return shear[gFYY] != ICON_SHEAR_SKIP_ROW && clipTop <= gFYY && gFYY <= gFYClipB;
}

// @semantic
// The decoder is structured as one command-fetch loop with `continue` edges. Replacing the former
// `goto do_fill` reconstruction with an if/else fallthrough to one shared fill block is byte-neutral;
// extracting that block into an inline function instead duplicates it (0x628, 156 relocations), so
// the shared block is intentional CFG rather than a helper. The literal paths likewise need no
// labels: nested setup arms flow into one copy tail, then continue the outer loop.
// Current structured source has the retail four-byte frame home, candidate 0x586 versus retail
// 0x58d, and exact 144/144 external relocation occurrences. The repeated complete row-visibility
// predicate is retained as the only helper supported by both reuse and codegen evidence; nested,
// row-only, one-use geometry, fill, and literal helpers were rejected. First raw divergence is the
// setup lifetime at +0x5c: retail keeps shear in ESI and clipW in EBP, while candidate keeps shear
// in EBP and later reloads width. Do not reintroduce labels merely to chase fuzzy scheduling.
VA(0x004d9ce0, 0x58d)
void FlipIconToBitmapYModify(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                             int clip, int clipX, int clipY, int clipW, int clipH, int color,
                             signed char *shear)
{
    int clipWidth = clipW;
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    gFYEntry = &entries[frame];
    gFYSrc = reinterpret_cast<unsigned char *>(srcIcon->m_data) + gFYEntry->srcOffset;
    gFYX0 = ((x - gFYEntry->w) - gFYEntry->x) + 1;
    gFYXEnd = gFYEntry->w + gFYX0 - 1;
    gFYY = gFYEntry->y + y;
    gFYX = gFYXEnd - shear[gFYY];
    gFYClipB = clipY + clipH - 1;
    gFYClipR = clipX + clipWidth - 1;
    gFYRow = dest->m_pixels + dest->m_width * gFYY;
    for (;;) {
        gFYRun = *gFYSrc++;
        if (static_cast<signed char>(gFYRun) < 0) {
            if ((gFYRun & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                if ((gFYRun & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                gFYX = gFYX - (gFYRun & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }
            if ((gFYRun & ICON_RLE_COMMAND_RUN_MASK) != 0) {
                if (gFYRun == ICON_RLE_LONG_SOLID_COMMAND) {
                    gFYRun = *gFYSrc++;
                } else {
                    gFYRun = gFYRun & ICON_RLE_COMMAND_RUN_MASK;
                }
                gFYColor = *gFYSrc++;
            } else {
                gFYRun = *gFYSrc++;
                if ((gFYRun & ICON_RLE_DIM_SHORT_COUNT_MASK) != 0) {
                    gFYDimLen = gFYRun & ICON_RLE_DIM_SHORT_COUNT_MASK;
                } else {
                    gFYDimLen = *gFYSrc++;
                }
                gFYDimLen2 = gFYDimLen;
                if (color != 0 && (gFYRun & ICON_RLE_DIM_RECOLOR_FLAG) != 0) {
                    gFYRun = gFYDimLen;
                    gFYColor = static_cast<unsigned char>(color);
                } else {
                    if ((gFYRun & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                        gFYDimPal =
                            reinterpret_cast<unsigned char *>(uDimPal) +
                            (gFYRun & ICON_RLE_DIM_LEVEL_MASK) *
                                ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                        if (IconRowVisible(shear, clipY)) {
                            if (clipX <= (gFYX - gFYDimLen) + 1 && gFYX <= gFYClipR) {
                                unsigned char *dimDst;
                                if (clipX <= (gFYX - gFYDimLen) + 1) {
                                    dimDst = (gFYRow - gFYDimLen) + gFYX + 1;
                                } else {
                                    gFYDimLen = (gFYX - clipX) + 1;
                                    dimDst = gFYRow + clipX;
                                }
                                gFYDimIdx = 0;
                                gFYDimDst = dimDst;
                                if (0 < static_cast<int>(gFYDimLen)) {
                                    do {
                                        *gFYDimDst = gFYDimPal[*gFYDimDst];
                                        gFYDimDst = gFYDimDst + 1;
                                        gFYDimIdx = gFYDimIdx + 1;
                                    } while (gFYDimIdx < static_cast<int>(gFYDimLen));
                                }
                            }
                        }
                    }
                    gFYX = gFYX - gFYDimLen2;
                    continue;
                }
            }
            if (IconRowVisible(shear, clipY)) {
                unsigned int fillCount = gFYRun;
                if (clipX <= static_cast<int>((gFYX - fillCount) + 1) && gFYX <= gFYClipR) {
                    if (clipX <= static_cast<int>((gFYX - fillCount) + 1)) {
                        memset((gFYRow - fillCount) + 1 + gFYX, gFYColor, fillCount);
                    } else {
                        memset(gFYRow + clipX, gFYColor, (gFYX - clipX) + 1);
                    }
                }
            }
            gFYX = gFYX - gFYRun;
            continue;
        }
        // ---- positive command : backward literal copy / newline ----
        if (gFYRun != 0) {
            if (IconRowVisible(shear, clipY)) {
                int left = (gFYX - gFYRun) + 1;
                if (left <= gFYClipR && clipX <= gFYX) {
                    if (gFYX <= gFYClipR) {
                        gFYDst = gFYRow + gFYX;
                        if (clipX <= left) {
                            gFYSkip = 0;
                            gFYDimLen = gFYRun;
                        } else {
                            gFYDimLen = (gFYX - clipX) + 1;
                            gFYSkip = gFYRun - gFYDimLen;
                        }
                    } else {
                        gFYSrc += gFYX - gFYClipR;
                        gFYDst = gFYRow + gFYClipR;
                        if (clipX <= gFYX - gFYRun) {
                            gFYSkip = 0;
                            gFYDimLen = gFYRun - gFYX + gFYClipR;
                        } else {
                            int pendingSkip = ((gFYRun - gFYX) - clipWidth) + gFYClipR;
                            gFYDimLen = clipWidth;
                            gFYSkip = pendingSkip;
                        }
                    }
                    gFYDimIdx = 0;
                    if (gFYDimLen > 0) {
                        do {
                            *gFYDst-- = *gFYSrc++;
                            gFYDimIdx++;
                        } while (gFYDimIdx < gFYDimLen);
                    }
                    gFYSrc += gFYSkip;
                } else {
                    gFYSrc += gFYRun;
                }
            } else {
                gFYSrc += gFYRun;
            }
            gFYX -= gFYRun;
            continue;
        }
        // newline
        gFYX = gFYXEnd - shear[gFYY];
        gFYY = gFYY + 1;
        gFYRow = gFYRow + dest->m_width;
    }
}
