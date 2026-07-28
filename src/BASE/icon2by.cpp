#include <va.h>
#include <BASE/icon2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconShear.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
DATA(0x00538150) static i32 s_clipB;
DATA(0x00538154) static i32 s_loopIndex;
DATA(0x0053815c) static i32 s_pitch;
DATA(0x00538158) static i32 s_y;
DATA(0x00538160) static i32 s_x;
DATA(0x00538164) static u8* s_dimDst;
DATA(0x0053816c) static i32 s_left;
DATA(0x00538168) static u32 s_run;
DATA(0x00538170) static u8* s_dimPal;
DATA(0x00538174) static u8* s_row;
DATA(0x00538178) static u8* s_src;
DATA(0x00538180) static IconEntry* s_entry;
DATA(0x0053817c) static u32 s_spanCount;
DATA(0x00538184) static u8 s_color;
DATA(0x00538188) static u32 s_dimLen;
DATA(0x0053818c) static i32 s_clipR;

static inline i32 IconRowVisible(i8* shear, i32 clipTop, i32 currentY, i32 clipBottom) {
    return shear[currentY] != ICON_SHEAR_SKIP_ROW && clipTop <= currentY && currentY <= clipBottom;
}

static inline u8* IconOutsideCopySource(u8* src, i32 currentX, i32 clipX) {
    return src + (clipX - currentX);
}

VA(0x004da270, 0x588)
void IconToBitmapYModify(
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
    s_entry = &srcIcon->Entries()[frame];
    s_src = srcIcon->m_data + s_entry->srcOffset;
    s_left = s_entry->x + x;
    s_pitch = dest->m_width;
    s_y = s_entry->y + y;
    s_x = shear[s_y] + s_left;
    s_clipR = clipX + clipW - 1;
    s_clipB = clipY + clipH - 1;
    s_row = dest->m_pixels + s_pitch * s_y;
    for (;;) {
        s_run = *s_src++;
        if (static_cast<i8>(s_run) < 0) {
            if ((s_run & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                if ((s_run & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                s_x += s_run & ICON_RLE_COMMAND_RUN_MASK;
                continue;
            }
            if ((s_run & ICON_RLE_COMMAND_RUN_MASK) != 0) {
                if (s_run == ICON_RLE_LONG_SOLID_COMMAND) {
                    s_run = *s_src++;
                } else {
                    s_run &= ICON_RLE_COMMAND_RUN_MASK;
                }
                s_color = *s_src++;
                goto do_fill;
            }
            s_run = *s_src++;
            if ((s_run & ICON_RLE_DIM_SHORT_COUNT_MASK) != 0) {
                s_spanCount = s_run & ICON_RLE_DIM_SHORT_COUNT_MASK;
            } else {
                s_spanCount = *s_src++;
            }
            s_dimLen = s_spanCount;
            if (color != 0 && (s_run & ICON_RLE_DIM_RECOLOR_FLAG) != 0) {
                s_run = s_spanCount;
                s_color = static_cast<u8>(color);
                goto do_fill;
            }
            if ((s_run & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                s_dimPal =
                    reinterpret_cast<u8*>(uDimPal)
                    + (s_run & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                if (IconRowVisible(shear, clipY, s_y, s_clipB)
                    && static_cast<i32>(s_spanCount + s_x) > clipX && s_clipR >= s_x) {
                    i32 dimRight = s_spanCount + s_x;
                    if (clipX <= s_x) {
                        if (s_clipR < dimRight)
                            s_spanCount = (s_clipR - s_x) + 1;
                        s_dimDst = s_row + s_x;
                    } else {
                        if (s_clipR >= dimRight)
                            s_spanCount += s_x - clipX;
                        else
                            s_spanCount = clipW;
                        s_dimDst = s_row + clipX;
                    }
                    s_loopIndex = 0;
                    if (0 < static_cast<i32>(s_spanCount)) {
                        do {
                            *s_dimDst = s_dimPal[*s_dimDst];
                            s_dimDst += 1;
                            s_loopIndex += 1;
                        } while (s_loopIndex < static_cast<i32>(s_spanCount));
                    }
                }
            }
            s_x += s_dimLen;
            continue;
        do_fill:
            if (IconRowVisible(shear, clipY, s_y, s_clipB)
                && static_cast<i32>(s_x + s_run) > clipX
                && s_clipR >= s_x) {
                i32 fillRight = s_x + s_run;
                if (clipX <= s_x) {
                    if (s_clipR >= fillRight) {
                        memset(s_row + s_x, s_color, s_run);
                    } else {
                        memset(s_row + s_x, s_color, (s_clipR - s_x) + 1);
                    }
                } else {
                    if (s_clipR >= fillRight) {
                        memset(s_row + clipX, s_color, (s_run - clipX) + s_x);
                    } else {
                        memset(s_row + clipX, s_color, clipW);
                    }
                }
            }
            s_x += s_run;
            continue;
        }
        if (s_run != 0) {
            if (IconRowVisible(shear, clipY, s_y, s_clipB)
                && static_cast<i32>(s_x + s_run) > clipX
                && s_clipR >= s_x) {
                i32 copyRight = s_x + s_run;
                if (clipX <= s_x) {
                    if (s_clipR >= copyRight) {
                        memcpy(s_row + s_x, s_src, s_run);
                    } else {
                        memcpy(s_row + s_x, s_src, (s_clipR - s_x) + 1);
                    }
                } else {
                    if (s_clipR >= copyRight) {
                        memcpy(s_row + clipX, s_src + (clipX - s_x), (s_run - clipX) + s_x);
                    } else {
                        u8* copySrc = IconOutsideCopySource(s_src, s_x, clipX);
                        memcpy(s_row + clipX, copySrc, clipW);
                    }
                }
            }
            s_x += s_run;
            s_src += s_run;
            continue;
        }
        s_x = shear[s_y] + s_left;
        s_y += 1;
        s_row += s_pitch;
    }
}
