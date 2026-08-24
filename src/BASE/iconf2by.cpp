#include <Ints.h>
#include <BASE/iconf2by.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconShear.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

static i32 s_clipB;
static i32 s_loopIndex;
static i32 s_y;
static i32 s_x;
static u8* s_dimDst;
static i32 s_left;
static i32 s_right;
static i32 s_run;
static u8* s_dimPal;
static u8* s_row;
static u8* s_dst;
static u8* s_src;
static i32 s_srcSkip;
static IconEntry* s_entry;
static i32 s_spanCount;
static u8 s_color;
static u32 s_dimLen;
static i32 s_clipR;

static inline i32 IconRowVisible(i8* shear, i32 clipTop, i32 currentY, i32 clipBottom) {
    return shear[currentY] != ICON_SHEAR_SKIP_ROW && clipTop <= currentY && currentY <= clipBottom;
}

void FlipIconToBitmapYModify(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 color,
    i8* shear
) {
    s_entry = reinterpret_cast<IconEntry*>(srcIcon->m_data) + frame;
    s_src = srcIcon->m_data + s_entry->srcOffset;
    s_left = x - s_entry->x - s_entry->w + 1;
    s_right = s_left + s_entry->w - 1;
    s_y = y + s_entry->y;
    s_x = s_right - shear[s_y];
    s_clipR = clipX + clipW - 1;
    s_clipB = clipY + clipH - 1;
    s_row = dest->m_pixels + s_y * dest->m_width;
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
                    goto fill_run;
                }
                if ((s_run & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                    s_dimPal = reinterpret_cast<u8*>(uDimPal)
                               + (s_run & ICON_RLE_DIM_LEVEL_MASK)
                                     * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                    if (shear[s_y] != ICON_SHEAR_SKIP_ROW && s_y >= clipY && s_y <= s_clipB
                        && s_x - s_spanCount + 1 >= clipX && s_x <= s_clipR) {
                        if (s_x <= s_clipR) {
                            if (s_x - s_spanCount + 1 >= clipX) {
                                s_dimDst = s_row + s_x - s_spanCount + 1;
                            } else {
                                s_spanCount = s_x - clipX + 1;
                                s_dimDst = s_row + clipX;
                            }
                        } else {
                            if (s_x - s_spanCount + 1 >= clipX) {
                                s_spanCount = s_clipR - (s_x - s_spanCount);
                                s_dimDst = s_row + s_x - s_spanCount + 1;
                            } else {
                                s_spanCount = clipW;
                                s_dimDst = s_row + clipX;
                            }
                        }
                        for (s_loopIndex = 0; s_loopIndex < s_spanCount; s_loopIndex++) {
                            *s_dimDst = s_dimPal[*s_dimDst];
                            s_dimDst++;
                        }
                    }
                }
                s_x = s_x - s_dimLen;
                continue;
            }
        fill_run:
            if (shear[s_y] != ICON_SHEAR_SKIP_ROW && s_y >= clipY && s_y <= s_clipB
                && s_x - s_run + 1 >= clipX && s_x <= s_clipR) {
                if (s_x <= s_clipR) {
                    if (s_x - s_run + 1 >= clipX)
                        memset(s_row + s_x - s_run + 1, s_color, s_run);
                    else
                        memset(s_row + clipX, s_color, s_x - clipX + 1);
                } else {
                    if (s_x - s_run + 1 >= clipX)
                        memset(s_row + s_x - s_run + 1, s_color, s_clipR - (s_x - s_run));
                    else
                        memset(s_row + clipX, s_color, clipW);
                }
            }
            s_x = s_x - s_run;
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
