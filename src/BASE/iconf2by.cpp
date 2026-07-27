#include <va.h>
#include <BASE/iconf2by.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconShear.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
DATA(0x00538118) static IconEntry* gFYEntry;
DATA(0x00538128) static u8* gFYSrc;
DATA(0x00538110) static i32 gFYX0;
DATA(0x00538108) static i32 gFYXEnd;
DATA(0x00538148) static i32 gFYY;
DATA(0x00538120) static i32 gFYX;
DATA(0x00538134) static i32 gFYClipB;
DATA(0x00538140) static u8* gFYRow;
DATA(0x00538144) static i32 gFYRun;
DATA(0x0053812c) static u8 gFYColor;
DATA(0x00538130) static i32 gFYDimLen;
DATA(0x00538114) static u32 gFYDimLen2;
DATA(0x0053810c) static u8* gFYDimPal;
DATA(0x00538138) static i32 gFYDimIdx;
DATA(0x0053814c) static u8* gFYDimDst;
DATA(0x0053811c) static u8* gFYDst;
DATA(0x00538124) static i32 gFYSkip;
DATA(0x0053813c) static i32 gFYClipR;

static inline i32 IconRowVisible(i8* shear, i32 clipTop) {
    return shear[gFYY] != ICON_SHEAR_SKIP_ROW && clipTop <= gFYY && gFYY <= gFYClipB;
}

VA(0x004d9ce0, 0x58d)
void FlipIconToBitmapYModify(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
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
                                if (0 < gFYDimLen) {
                                    do {
                                        *gFYDimDst = gFYDimPal[*gFYDimDst];
                                        gFYDimDst = gFYDimDst + 1;
                                        gFYDimIdx = gFYDimIdx + 1;
                                    } while (gFYDimIdx < gFYDimLen);
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
        if (gFYRun != 0) {
            if (IconRowVisible(shear, clipY)) {
                i32 left = (gFYX - gFYRun) + 1;
                i32 pendingSkip;
                if (left <= gFYClipR && clipX <= gFYX) {
                    if (gFYX <= gFYClipR) {
                        gFYDst = gFYRow + gFYX;
                        if (clipX <= left) {
                            gFYSkip = 0;
                            gFYDimLen = gFYRun;
                            goto copy_literal;
                        }
                        gFYDimLen = (gFYX - clipX) + 1;
                        pendingSkip = gFYRun - gFYDimLen;
                        goto publish_literal_skip;
                    } else {
                        gFYSrc = gFYSrc + (gFYX - gFYClipR);
                        u8* rightDst = gFYRow + gFYClipR;
                        if (clipX <= (gFYX - gFYRun)) {
                            gFYDst = rightDst;
                            gFYSkip = 0;
                            gFYDimLen = (gFYRun - gFYX) + gFYClipR;
                            goto copy_literal;
                        }
                        gFYDst = rightDst;
                        pendingSkip = gFYRun;
                        pendingSkip = pendingSkip - gFYX;
                        pendingSkip = pendingSkip - clipWidth;
                        pendingSkip = pendingSkip + gFYClipR;
                        gFYDimLen = clipWidth;
                        goto publish_literal_skip;
                    }
                publish_literal_skip:
                    gFYSkip = pendingSkip;
                copy_literal:
                    gFYDimIdx = 0;
                    if (0 < gFYDimLen) {
                        do {
                            *gFYDst = *gFYSrc;
                            gFYSrc = gFYSrc + 1;
                            gFYDst = gFYDst - 1;
                            gFYDimIdx = gFYDimIdx + 1;
                        } while (gFYDimIdx < gFYDimLen);
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
        gFYX = gFYXEnd - shear[gFYY];
        gFYY = gFYY + 1;
        gFYRow = gFYRow + dest->m_width;
    }
}
