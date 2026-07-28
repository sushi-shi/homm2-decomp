#include <va.h>
#include <BASE/iconf2by.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconShear.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

DATA(0x00538134) static i32 s_clipB;
DATA(0x00538138) static i32 s_loopIndex;
DATA(0x00538148) static i32 s_y;
DATA(0x00538120) static i32 s_x;
DATA(0x0053814c) static u8* s_dimDst;
DATA(0x00538110) static i32 s_left;
DATA(0x00538108) static i32 s_right;
DATA(0x00538144) static i32 s_run;
DATA(0x0053810c) static u8* s_dimPal;
DATA(0x00538140) static u8* s_row;
DATA(0x0053811c) static u8* s_dst;
DATA(0x00538128) static u8* s_src;
DATA(0x00538124) static i32 s_srcSkip;
DATA(0x00538118) static IconEntry* s_entry;
DATA(0x00538130) static i32 s_spanCount;
DATA(0x0053812c) static u8 s_color;
DATA(0x00538114) static u32 s_dimLen;
DATA(0x0053813c) static i32 s_clipR;

static inline i32 IconRowVisible(i8* shear, i32 clipTop, i32 currentY, i32 clipBottom) {
    return shear[currentY] != ICON_SHEAR_SKIP_ROW && clipTop <= currentY && currentY <= clipBottom;
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
    s_entry = &srcIcon->Entries()[frame];
    s_src = srcIcon->m_data + s_entry->srcOffset;
    s_left = ((x - s_entry->w) - s_entry->x) + 1;
    s_right = s_entry->w + s_left - 1;
    s_y = s_entry->y + y;
    s_x = s_right - shear[s_y];
    s_clipB = clipY + clipH - 1;
    s_clipR = clipX + clipW - 1;
    s_row = dest->m_pixels + dest->m_width * s_y;
    for (;;) {
        s_run = *s_src++;
        if (static_cast<i8>(s_run) < 0) {
            if ((s_run & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                if ((s_run & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                s_x -= s_run & ICON_RLE_COMMAND_RUN_MASK;
                continue;
            }
            if ((s_run & ICON_RLE_COMMAND_RUN_MASK) != 0) {
                if (s_run == ICON_RLE_LONG_SOLID_COMMAND) {
                    s_run = *s_src++;
                } else {
                    s_run &= ICON_RLE_COMMAND_RUN_MASK;
                }
                s_color = *s_src++;
            } else {
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
                } else {
                    if ((s_run & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                        s_dimPal = reinterpret_cast<u8*>(uDimPal)
                                    + (s_run & ICON_RLE_DIM_LEVEL_MASK)
                                          * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                        if (IconRowVisible(shear, clipY, s_y, s_clipB)) {
                            if (clipX <= (s_x - s_spanCount) + 1 && s_x <= s_clipR) {
                                if (clipX <= (s_x - s_spanCount) + 1) {
                                    s_dimDst = (s_row - s_spanCount) + s_x + 1;
                                } else {
                                    s_spanCount = (s_x - clipX) + 1;
                                    s_dimDst = s_row + clipX;
                                }
                                s_loopIndex = 0;
                                if (0 < s_spanCount) {
                                    do {
                                        *s_dimDst = s_dimPal[*s_dimDst];
                                        s_dimDst += 1;
                                        s_loopIndex += 1;
                                    } while (s_loopIndex < s_spanCount);
                                }
                            }
                        }
                    }
                    s_x -= s_dimLen;
                    continue;
                }
            }
            if (IconRowVisible(shear, clipY, s_y, s_clipB)) {
                u32 fillCount = s_run;
                if (clipX <= static_cast<i32>((s_x - fillCount) + 1) && s_x <= s_clipR) {
                    if (clipX <= static_cast<i32>((s_x - fillCount) + 1)) {
                        memset((s_row - fillCount) + 1 + s_x, s_color, fillCount);
                    } else {
                        memset(s_row + clipX, s_color, (s_x - clipX) + 1);
                    }
                }
            }
            s_x -= s_run;
            continue;
        }
        if (s_run != 0) {
            if (IconRowVisible(shear, clipY, s_y, s_clipB)) {
                i32 left = (s_x - s_run) + 1;
                i32 pendingSkip;
                if (left <= s_clipR && clipX <= s_x) {
                    do {
                        if (s_x <= s_clipR) {
                            s_dst = s_row + s_x;
                            if (clipX <= left) {
                                s_srcSkip = 0;
                                s_spanCount = s_run;
                                break;
                            }
                            s_spanCount = (s_x - clipX) + 1;
                            pendingSkip = s_run - s_spanCount;
                        } else {
                            s_src += s_x - s_clipR;
                            s_dst = s_row + s_clipR;
                            if (clipX <= (s_x - s_run)) {
                                s_srcSkip = 0;
                                s_spanCount = (s_run - s_x) + s_clipR;
                                break;
                            }
                            pendingSkip = s_run;
                            pendingSkip -= s_x;
                            pendingSkip -= clipW;
                            pendingSkip += s_clipR;
                            s_spanCount = clipW;
                        }
                        s_srcSkip = pendingSkip;
                    } while (0);
                    s_loopIndex = 0;
                    if (0 < s_spanCount) {
                        do {
                            *s_dst = *s_src++;
                            s_dst -= 1;
                            s_loopIndex += 1;
                        } while (s_loopIndex < s_spanCount);
                    }
                    s_src += s_srcSkip;
                } else {
                    s_src += s_run;
                }
            } else {
                s_src += s_run;
            }
            s_x -= s_run;
            continue;
        }
        s_x = s_right - shear[s_y];
        s_y += 1;
        s_row += dest->m_width;
    }
}
