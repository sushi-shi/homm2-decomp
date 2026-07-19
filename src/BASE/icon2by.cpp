#include <va.h>
#include <BASE/icon2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconShear.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
DATA(0x00538150) static i32 gYMClipB;
DATA(0x00538154) static i32 gYMDimIdx;
DATA(0x0053815c) static i32 gYMPitch;
DATA(0x00538158) static i32 gYMY;
DATA(0x00538160) static i32 gYMX;
DATA(0x00538164) static u8* gYMDimDst;
DATA(0x0053816c) static i32 gYMX0;
DATA(0x00538168) static u32 gYMRun;
DATA(0x00538170) static u8* gYMDimPal;
DATA(0x00538174) static u8* gYMRow;
DATA(0x00538178) static u8* gYMSrc;
DATA(0x00538180) static IconEntry* gYMEntry;
DATA(0x0053817c) static u32 gYMDimLen;
DATA(0x00538184) static u8 gYMColor;
DATA(0x00538188) static u32 gYMDimLen2;
DATA(0x0053818c) static i32 gYMClipR;

VA(0x004da270, 0x588)
void IconToBitmapYModify(
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
    IconEntry* entries = srcIcon->Entries();
    gYMEntry = &entries[frame];
    gYMSrc = srcIcon->m_data + gYMEntry->srcOffset;
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
        if (static_cast<i8>(gYMRun) < 0) {
            if ((gYMRun & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                if ((gYMRun & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                gYMX = gYMX + (gYMRun & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }
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
                gYMColor = static_cast<u8>(color);
                goto do_fill;
            }
            if ((gYMRun & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                gYMDimPal =
                    reinterpret_cast<u8*>(uDimPal)
                    + (gYMRun & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                i32 currentY;
                if (shear[gYMY] != ICON_SHEAR_SKIP_ROW && clipY <= (currentY = gYMY)
                    && currentY <= gYMClipB && static_cast<i32>(gYMDimLen + gYMX) > clipX
                    && gYMClipR >= gYMX) {
                    i32 dimRight = gYMDimLen + gYMX;
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
                    if (0 < static_cast<i32>(gYMDimLen)) {
                        do {
                            *gYMDimDst = gYMDimPal[*gYMDimDst];
                            gYMDimDst = 1 + gYMDimDst;
                            gYMDimIdx = gYMDimIdx + 1;
                        } while (gYMDimIdx < static_cast<i32>(gYMDimLen));
                    }
                }
            }
            gYMX = gYMX + gYMDimLen2;
            continue;
        do_fill:
            i32 currentY;
            if (shear[gYMY] != ICON_SHEAR_SKIP_ROW && clipY <= (currentY = gYMY)
                && currentY <= gYMClipB && static_cast<i32>(gYMX + gYMRun) > clipX
                && gYMClipR >= gYMX) {
                i32 fillRight = gYMX + gYMRun;
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
        if (gYMRun != 0) {
            i32 currentY;
            if (shear[gYMY] != ICON_SHEAR_SKIP_ROW && clipY <= (currentY = gYMY)
                && currentY <= gYMClipB && static_cast<i32>(gYMX + gYMRun) > clipX
                && gYMClipR >= gYMX) {
                i32 copyRight = gYMX + gYMRun;
                if (clipX <= gYMX) {
                    if (gYMClipR >= copyRight) {
                        memcpy(gYMRow + gYMX, gYMSrc, gYMRun);
                    } else {
                        memcpy(gYMRow + gYMX, gYMSrc, (gYMClipR - gYMX) + 1);
                    }
                } else {
                    if (gYMClipR >= copyRight) {
                        memcpy(gYMRow + clipX, gYMSrc + (clipX - gYMX), (gYMRun - clipX) + gYMX);
                    } else {
                        memcpy(gYMRow + clipX, gYMSrc + (clipX - gYMX), clipW);
                    }
                }
            }
            gYMX = gYMX + gYMRun;
            gYMSrc = gYMSrc + gYMRun;
            continue;
        }
        gYMX = shear[gYMY] + gYMX0;
        gYMY = gYMY + 1;
        gYMRow = gYMRow + gYMPitch;
    }
}
