#include <va.h>
#include <BASE/icon2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
#include <SOURCE/KB.h>

// Retail's frame is exactly the two __fastcall spills: every working value of
// the decoder lives in a file static, not a local.
DATA(0x005360d4) static i32 s_left = 0;
DATA(0x005360d8) static i32 s_clipR = 0;
DATA(0x005360dc) static u8* s_dimPal = 0;
DATA(0x005360e0) static u8 s_color = 0;
DATA(0x005360e4) static i32 s_spanCount = 0;
DATA(0x005360e8) static u8* s_dst = 0;
DATA(0x005360ec) static u8* s_row = 0;
DATA(0x005360f0) static i32 s_y = 0;
DATA(0x005360f4) static IconEntry* s_entry = 0;
DATA(0x005360f8) static i32 s_run = 0;
DATA(0x005360fc) static i32 s_x = 0;
DATA(0x00536100) static i32 s_clipB = 0;
DATA(0x00536104) static u8* s_src = 0;
DATA(0x00536108) static i32 s_dimLen = 0;
DATA(0x0053610c) static i32 s_loopCount = 0;
DATA(0x00536110) static u8* s_literalSource = 0;
DATA(0x00536114) static i32 s_pitch = 0;

VA(0x004c0ca0, 0x7cf)
void IconToBitmapColorTable(
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
    u8* colorTable,
    i32 dimGate
) {
    s_entry = reinterpret_cast<IconEntry*>(srcIcon->m_data) + frame;
    s_src = srcIcon->m_data + s_entry->srcOffset;
    s_left = x + s_entry->x;
    s_pitch = dest->m_width;
    s_x = s_left;
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

    s_row = dest->m_pixels + s_y * s_pitch;

    for (;;) {
        s_run = *s_src;
        s_src++;
        if (static_cast<i8>(s_run) < 0) {
            if ((s_run & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                if ((s_run & ICON_RLE_COMMAND_RUN_MASK) != 0)
                    s_x += s_run & ICON_RLE_COMMAND_RUN_MASK;
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
                    memset(s_row + s_x, s_color, s_run);
                } else if (s_y >= clipY && s_y <= s_clipB && s_x + s_run > clipX
                           && s_x <= s_clipR) {
                    if (s_x >= clipX) {
                        if (s_x + s_run <= s_clipR)
                            memset(s_row + s_x, s_color, s_run);
                        else
                            memset(s_row + s_x, s_color, s_clipR - s_x + 1);
                    } else {
                        if (s_x + s_run <= s_clipR)
                            memset(s_row + clipX, s_color, s_x + s_run - clipX);
                        else
                            memset(s_row + clipX, s_color, clipW);
                    }
                }
                s_x = s_x + s_run;
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
                    s_dimPal = reinterpret_cast<u8*>(uDimPal)
                               + (static_cast<u32>(s_run & ICON_RLE_DIM_LEVEL_MASK) >> 2)
                                     * DIM_PALETTE_COLOR_COUNT;
                    if (clip == ICON_DRAW_NO_CLIP) {
                        s_dst = s_row + s_x;
                        for (s_loopCount = 0; s_loopCount < s_dimLen; s_loopCount++) {
                            if (dimGate != 0)
                                *s_dst = s_dimPal[*s_dst];
                            s_dst++;
                        }
                    } else if (s_y >= clipY && s_y <= s_clipB && s_x + s_dimLen > clipX
                               && s_x <= s_clipR) {
                        if (s_x >= clipX) {
                            if (s_x + s_dimLen > s_clipR)
                                s_dimLen = s_clipR - s_x + 1;
                            s_dst = s_row + s_x;
                        } else {
                            if (s_x + s_dimLen <= s_clipR)
                                s_dimLen = s_x + s_dimLen - clipX;
                            else
                                s_dimLen = clipW;
                            s_dst = s_row + clipX;
                        }
                        for (s_loopCount = 0; s_loopCount < s_dimLen; s_loopCount++) {
                            if (dimGate != 0)
                                *s_dst = s_dimPal[*s_dst];
                            s_dst++;
                        }
                    }
                }
                s_x = s_x + s_spanCount;
            }
        } else if (s_run != 0) {
            s_literalSource = s_src;
            if (clip == ICON_DRAW_NO_CLIP) {
                s_dst = s_row + s_x;
                s_dimLen = s_run;
            } else if (s_y >= clipY && s_y <= s_clipB && s_x + s_run > clipX && s_x <= s_clipR) {
                if (s_x >= clipX) {
                    if (s_x + s_run <= s_clipR) {
                        s_dst = s_row + s_x;
                        s_dimLen = s_run;
                    } else {
                        s_dst = s_row + s_x;
                        s_dimLen = s_clipR - s_x + 1;
                    }
                } else {
                    if (s_x + s_run <= s_clipR) {
                        s_dst = s_row + clipX;
                        s_dimLen = s_x + s_run - clipX;
                    } else {
                        s_dst = s_row + clipX;
                        s_dimLen = clipW;
                    }
                    s_literalSource = s_src + clipX - s_x;
                }
            } else {
                s_dimLen = 0;
            }
            if (s_dimLen != 0) {
                for (s_loopCount = 0; s_loopCount < s_dimLen; s_loopCount++) {
                    *s_dst = colorTable[*s_literalSource];
                    s_literalSource++;
                    s_dst++;
                }
            }
            s_x = s_x + s_run;
            s_src = s_src + s_run;
        } else {
            s_x = s_left;
            s_y++;
            s_row = s_row + s_pitch;
        }
    }
}
