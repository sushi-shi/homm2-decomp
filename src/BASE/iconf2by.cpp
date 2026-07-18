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
static IconEntry* gFYEntry;
static u8* gFYSrc;
static i32 gFYX0;
static i32 gFYXEnd;
static i32 gFYY;
static i32 gFYX;
static i32 gFYClipB;
static u8* gFYRow;
static i32 gFYRun;
static u8 gFYColor;
static i32 gFYDimLen;
static u32 gFYDimLen2;
static u8* gFYDimPal;
static i32 gFYDimIdx;
static u8* gFYDimDst;
static u8* gFYDst;
static i32 gFYSkip;
static i32 gFYClipR;

static inline i32 IconRowVisible(i8* shear, i32 clipTop) {
    return shear[gFYY] != ICON_SHEAR_SKIP_ROW && clipTop <= gFYY && gFYY <= gFYClipB;
}

// @semantic
// The decoder is structured as one command-fetch loop with `continue` edges. Replacing the former
// `goto do_fill` reconstruction with an if/else fallthrough to one shared fill block is byte-neutral;
// extracting that block into an inline function instead duplicates it (0x628, 156 relocations), so
// the shared block is intentional CFG rather than a helper. The literal paths likewise need no
// labels: nested setup arms flow into one copy tail, then continue the outer loop.
// Current structured source has the retail four-byte frame home and candidate 0x586 versus retail
// 0x58d. Retail code operands recover the complete permutation of all 18 otherwise indistinguishable
// four-byte private BSS owners; section-order pairing had mislabeled them. With that mapping fixed,
// all 142 candidate data-target occurrences match retail and only retail's gFYClipR at +0x4b3 and
// gFYX at +0x56e remain absent. The repeated complete row-visibility predicate is retained as the
// only helper supported by both reuse and codegen evidence; nested, row-only, one-use geometry,
// fill, and literal helpers were rejected. First raw divergence is the setup lifetime at +0x5c:
// retail keeps shear in ESI and clipW in EBP, while candidate keeps shear in EBP and later reloads
// width. Do not reintroduce labels merely to chase fuzzy scheduling.
VA(0x004d9ce0, 0x58d)
void FlipIconToBitmapYModify(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    i32 clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 color,
    i8* shear
) {
    i32 clipWidth = clipW;
    IconEntry* entries = srcIcon->Entries();
    gFYEntry = &entries[frame];
    gFYSrc = srcIcon->m_data + gFYEntry->srcOffset;
    gFYX0 = ((x - gFYEntry->w) - gFYEntry->x) + 1;
    gFYXEnd = gFYEntry->w + gFYX0 - 1;
    gFYY = gFYEntry->y + y;
    gFYX = gFYXEnd - shear[gFYY];
    gFYClipB = clipY + clipH - 1;
    gFYClipR = clipX + clipWidth - 1;
    gFYRow = dest->m_pixels + dest->m_width * gFYY;
    for (;;) {
        gFYRun = *gFYSrc++;
        if (static_cast<i8>(gFYRun) < 0) {
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
                    gFYColor = static_cast<u8>(color);
                } else {
                    if ((gFYRun & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                        gFYDimPal = reinterpret_cast<u8*>(uDimPal)
                                    + (gFYRun & ICON_RLE_DIM_LEVEL_MASK)
                                          * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                        if (IconRowVisible(shear, clipY)) {
                            if (clipX <= (gFYX - gFYDimLen) + 1 && gFYX <= gFYClipR) {
                                u8* dimDst;
                                if (clipX <= (gFYX - gFYDimLen) + 1) {
                                    dimDst = (gFYRow - gFYDimLen) + gFYX + 1;
                                } else {
                                    gFYDimLen = (gFYX - clipX) + 1;
                                    dimDst = gFYRow + clipX;
                                }
                                gFYDimIdx = 0;
                                gFYDimDst = dimDst;
                                if (0 < static_cast<i32>(gFYDimLen)) {
                                    do {
                                        *gFYDimDst = gFYDimPal[*gFYDimDst];
                                        gFYDimDst = gFYDimDst + 1;
                                        gFYDimIdx = gFYDimIdx + 1;
                                    } while (gFYDimIdx < static_cast<i32>(gFYDimLen));
                                }
                            }
                        }
                    }
                    gFYX = gFYX - gFYDimLen2;
                    continue;
                }
            }
            if (IconRowVisible(shear, clipY)) {
                u32 fillCount = gFYRun;
                if (clipX <= static_cast<i32>((gFYX - fillCount) + 1) && gFYX <= gFYClipR) {
                    if (clipX <= static_cast<i32>((gFYX - fillCount) + 1)) {
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
                i32 left = (gFYX - gFYRun) + 1;
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
                            i32 pendingSkip = ((gFYRun - gFYX) - clipWidth) + gFYClipR;
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
