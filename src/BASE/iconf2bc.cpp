#include <Ints.h>
#include <BASE/iconf2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
#include <SOURCE/KB.h>


static i32 s_loopCount = 0;
static i32 s_clipR = 0;
static IconEntry* s_entry = 0;
static u8* s_dimPal = 0;
static i32 s_right = 0;
static i32 s_y = 0;
static i32 s_srcSkip = 0;
static u8* s_dst = 0;
static i32 s_spanCount = 0;
static i32 s_clipB = 0;
static u8* s_row = 0;
static u8* s_copyDst = 0;
static i32 s_x = 0;
static u8* s_src = 0;
static i32 s_left = 0;
static i32 s_run = 0;
static i32 s_dimLen = 0;
static u8 s_color = 0;

void FlipIconToBitmapColorTable(
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
    u8* colorTable
) {
    s_entry = reinterpret_cast<IconEntry*>(srcIcon->m_data) + frame;
    s_src = srcIcon->m_data + s_entry->srcOffset;
    s_left = x - s_entry->x - s_entry->w + 1;
    s_right = s_left + s_entry->w - 1;
    s_x = s_right;
    s_y = y + s_entry->y;

    if (clip != ICON_DRAW_NO_CLIP) {
        if (s_left >= clipX && s_left + s_entry->w <= clipX + clipW && s_y >= clipY
            && s_y + s_entry->h <= clipY + clipH) {
            clip = ICON_DRAW_NO_CLIP;
        } else {
            clip = ICON_DRAW_CLIP;
            s_clipR = clipX + clipW - 1;
            s_clipB = clipY + clipH - 1;
        }
    }

    s_row = dest->m_pixels + s_y * dest->m_width;

    for (;;) {
        s_run = *s_src;
        s_src++;
        if (static_cast<i8>(s_run) < 0) {
            if ((s_run & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                if ((s_run & ICON_RLE_COMMAND_RUN_MASK) != 0)
                    s_x -= s_run & ICON_RLE_COMMAND_RUN_MASK;
                else
                    return;
            } else if ((s_run & ICON_RLE_COMMAND_RUN_MASK) != 0) {
                if (s_run == ICON_RLE_LONG_SOLID_COMMAND) {
                    s_run = *s_src;
                    s_src++;
                } else {
                    s_run &= ICON_RLE_COMMAND_RUN_MASK;
                }
                s_color = colorTable[*s_src];
                s_src++;
            fill_run:
                if (clip == ICON_DRAW_NO_CLIP) {
                    memset(s_row + s_x - s_run + 1, s_color, s_run);
                } else if (s_y >= clipY && s_y <= s_clipB && s_x - s_run + 1 >= clipX
                           && s_x <= s_clipR) {
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
            } else {
                s_run = *s_src;
                s_src++;
                if ((s_run & ICON_RLE_DIM_SHORT_COUNT_MASK) != 0) {
                    s_dimLen = s_run & ICON_RLE_DIM_SHORT_COUNT_MASK;
                } else {
                    s_dimLen = *s_src;
                    s_src++;
                }
                s_spanCount = s_dimLen;
                if (color != 0 && (s_run & ICON_RLE_DIM_RECOLOR_FLAG) != 0) {
                    s_run = s_dimLen;
                    s_color = static_cast<u8>(color);
                    goto fill_run;
                }
                if ((s_run & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                    s_dimPal =
                        reinterpret_cast<u8*>(uDimPal)
                        + (s_run & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                    if (clip == ICON_DRAW_NO_CLIP) {
                        s_dst = s_row + s_x - s_dimLen + 1;
                        for (s_loopCount = 0; s_loopCount < s_dimLen; s_loopCount++) {
                            *s_dst = s_dimPal[*s_dst];
                            s_dst++;
                        }
                    } else if (s_y >= clipY && s_y <= s_clipB && s_x - s_dimLen + 1 >= clipX
                               && s_x <= s_clipR) {
                        if (s_x <= s_clipR) {
                            if (s_x - s_dimLen + 1 >= clipX) {
                                s_dst = s_row + s_x - s_dimLen + 1;
                            } else {
                                s_dimLen = s_x - clipX + 1;
                                s_dst = s_row + clipX;
                            }
                        } else {
                            if (s_x - s_dimLen + 1 >= clipX) {
                                s_dimLen = s_clipR - (s_x - s_dimLen);
                                s_dst = s_row + s_x - s_dimLen + 1;
                            } else {
                                s_dimLen = clipW;
                                s_dst = s_row + clipX;
                            }
                        }
                        for (s_loopCount = 0; s_loopCount < s_dimLen; s_loopCount++) {
                            *s_dst = s_dimPal[*s_dst];
                            s_dst++;
                        }
                    }
                }
                s_x = s_x - s_spanCount;
            }
        } else if (s_run != 0) {
            if (clip == ICON_DRAW_NO_CLIP) {
                s_copyDst = s_row + s_x;
                for (s_loopCount = 0; s_loopCount < s_run; s_loopCount++) {
                    *s_copyDst = colorTable[*s_src];
                    s_src++;
                    s_copyDst--;
                }
            } else if (s_y >= clipY && s_y <= s_clipB && s_x - s_run + 1 <= s_clipR
                       && s_x >= clipX) {
                if (s_x <= s_clipR) {
                    s_copyDst = s_row + s_x;
                    if (s_x - s_run + 1 >= clipX) {
                        s_dimLen = s_run;
                        s_srcSkip = 0;
                    } else {
                        s_dimLen = s_x - clipX + 1;
                        s_srcSkip = s_run - s_dimLen;
                    }
                } else {
                    s_src = s_src + (s_x - s_clipR);
                    s_copyDst = s_row + s_clipR;
                    if (s_x - s_run >= clipX) {
                        s_dimLen = s_clipR - (s_x - s_run);
                        s_srcSkip = 0;
                    } else {
                        s_dimLen = clipW;
                        s_srcSkip = s_run - s_dimLen - (s_x - s_clipR);
                    }
                }
                for (s_loopCount = 0; s_loopCount < s_dimLen; s_loopCount++) {
                    *s_copyDst = colorTable[*s_src];
                    s_src++;
                    s_copyDst--;
                }
                s_src = s_src + s_srcSkip;
            } else {
                s_src = s_src + s_run;
            }
            s_x = s_x - s_run;
        } else {
            s_x = s_right;
            s_y = s_y + 1;
            s_row = s_row + dest->m_width;
        }
    }
}
