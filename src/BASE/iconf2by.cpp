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

// @semantic
// /O2 structural checkpoint in the corrected 1470-row delinker universe with consumer-only enum
// ownership. Reconstructing gFYRow as the byte pointer owned by every sibling decoder removes the
// integer-address casts and raises live match from 89.77212% to 90.20107%; candidate remains 0x57f
// versus retail 0x58d with one four-byte frame word. The required named IconShear typedef is kept;
// omitting that consumer header can recover a higher temporary TU-state score but leaves raw 0x7f
// sentinels and is rejected. First raw divergence remains +0x5c: candidate keeps shear in EBP,
// while retail loads shear into ESI and then clipW into EBP. Relocations remain 142/144 with no
// wrong or excess target; missing occurrences are retail gFYX in the full dim destination and
// gFYClipR in the right-clipped literal count. Direct shear indexing and the typed byte-row model
// agree with the non-flipped/color-table siblings and related H2X lineage. Revisit only after new
// retail-evidenced lifetime/type structure; no regex or AST permuter was used, and dummy reloads,
// aliases, volatile state, and count-only locals are forbidden.
// A 2026-07-15 256-state exact-only typedef/enum/record/member/prototype/include sweep produced no
// improvement over the 87.839140% direct baseline and no exact closure. Generated state was removed;
// do not repeat or record it as MAX. The setup/body residual requires new source structure.
// A later live-baseline audit at 88.806970%, 0x582, 142/144 exhaustively tested all 32 semantic
// suffixes and 18 plausible whole-name spellings for clipWidth; every rename was byte-identical.
// `register`, `const`, direct/split initialization, and moving that declaration beside Y or ClipR
// were also byte-identical. Extracting the setup shear subtraction into 16 inline helper identities
// fell to 86.206436%; all 127 safe scalar/nested helper sites topped out at 88.552280%. Generated
// helper source was restored. These results rule out local spelling/scope and simple predecessor
// helpers, but do not prove a wall or justify changing the correct enum/header ownership.
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
        gFYRun = *gFYSrc;
        gFYSrc = gFYSrc + 1;
        if (static_cast<signed char>(gFYRun) < 0) {
            if ((gFYRun & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                if ((gFYRun & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                gFYX = gFYX - (gFYRun & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }
            if ((gFYRun & ICON_RLE_COMMAND_RUN_MASK) != 0) {
                if (gFYRun == ICON_RLE_LONG_SOLID_COMMAND) {
                    gFYRun = *gFYSrc;
                    gFYSrc = gFYSrc + 1;
                } else {
                    gFYRun = gFYRun & ICON_RLE_COMMAND_RUN_MASK;
                }
                gFYColor = *gFYSrc;
                gFYSrc = gFYSrc + 1;
                goto do_fill;
            }
            gFYRun = *gFYSrc;
            gFYSrc = gFYSrc + 1;
            if ((gFYRun & ICON_RLE_DIM_SHORT_COUNT_MASK) != 0) {
                gFYDimLen = gFYRun & ICON_RLE_DIM_SHORT_COUNT_MASK;
            } else {
                gFYDimLen = *gFYSrc;
                gFYSrc = gFYSrc + 1;
            }
            gFYDimLen2 = gFYDimLen;
            if (color != 0 && (gFYRun & ICON_RLE_DIM_RECOLOR_FLAG) != 0) {
                gFYRun = gFYDimLen;
                gFYColor = static_cast<unsigned char>(color);
                goto do_fill;
            }
            if ((gFYRun & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                gFYDimPal =
                    reinterpret_cast<unsigned char *>(uDimPal) +
                    (gFYRun & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                if (shear[gFYY] != ICON_SHEAR_SKIP_ROW && clipY <= gFYY && gFYY <= gFYClipB) {
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
        do_fill:
            if (shear[gFYY] != ICON_SHEAR_SKIP_ROW && clipY <= gFYY && gFYY <= gFYClipB) {
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
            if (shear[gFYY] != ICON_SHEAR_SKIP_ROW && clipY <= gFYY && gFYY <= gFYClipB) {
                int left = (gFYX - gFYRun) + 1;
                int pendingSkip;
                if (left <= gFYClipR && clipX <= gFYX) {
                    if (gFYX <= gFYClipR) {
                        gFYDst = gFYRow + gFYX;
                        if (clipX <= left) {
                            gFYSkip = 0;
                            gFYDimLen = gFYRun;
                            goto copy_literal;
                        } else {
                            gFYDimLen = (gFYX - clipX) + 1;
                            pendingSkip = gFYRun - gFYDimLen;
                            goto publish_literal_skip;
                        }
                    } else {
                        gFYSrc = gFYSrc + (gFYX - gFYClipR);
                        unsigned char *rightDst = gFYRow + gFYClipR;
                        if (clipX <= (gFYX - gFYRun)) {
                            gFYDst = rightDst;
                            gFYSkip = 0;
                            gFYDimLen = (gFYRun - gFYX) + gFYClipR;
                            goto copy_literal;
                        } else {
                            gFYDst = rightDst;
                            pendingSkip = ((gFYRun - gFYX) - clipWidth) + gFYClipR;
                            gFYDimLen = clipWidth;
                            goto publish_literal_skip;
                        }
                    }
                publish_literal_skip:
                    gFYSkip = pendingSkip;
                copy_literal:
                    gFYDimIdx = 0;
                    if (0 < static_cast<int>(gFYDimLen)) {
                        do {
                            *gFYDst = *gFYSrc;
                            gFYSrc = gFYSrc + 1;
                            gFYDst = gFYDst - 1;
                            gFYDimIdx = gFYDimIdx + 1;
                        } while (gFYDimIdx < static_cast<int>(gFYDimLen));
                    }
                    gFYSrc = gFYSrc + gFYSkip;
                    goto literal_advance_done;
                }
            }
            gFYSrc = gFYSrc + gFYRun;
        literal_advance_done:
            gFYX = gFYX - gFYRun;
            continue;
        }
        // newline
        gFYX = gFYXEnd - shear[gFYY];
        gFYY = gFYY + 1;
        gFYRow = gFYRow + dest->m_width;
    }
}
