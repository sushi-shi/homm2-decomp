#include <va.h>
#include <BASE/Icon2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <SOURCE/KB.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

// The decoder keeps every piece of working state in file statics: retail's
// frame is exactly the two __fastcall spills, with no local slots at all.
DATA(0x00536118) static i32 s_loopCount;
DATA(0x0053611c) static u8* s_src;
DATA(0x00536120) static i32 s_left;
DATA(0x00536124) static u8* s_dimPal;
DATA(0x00536128) static i32 s_pitch;
DATA(0x0053612c) static i32 s_run;
DATA(0x00536130) static u8* s_dst;
DATA(0x00536134) static u8 s_color;
DATA(0x00536138) static i32 s_spanCount;
DATA(0x0053613c) static i32 s_x;
DATA(0x00536140) static IconEntry* s_entry;
DATA(0x00536144) static i32 s_y;
DATA(0x00536148) static i32 s_clipR;
DATA(0x0053614c) static i32 s_clipB;
DATA(0x00536150) static i32 s_dimLen;
DATA(0x00536154) static u8* s_row;

VA(0x004c14c0, 0x750)
void IconToBitmap(
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
    i32 color
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
                s_color = *s_src;
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
                    s_dimPal =
                        reinterpret_cast<u8*>(uDimPal)
                        + (s_run & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                    if (clip == ICON_DRAW_NO_CLIP) {
                        s_dst = s_row + s_x;
                        for (s_loopCount = 0; s_loopCount < s_dimLen; s_loopCount++) {
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
                            *s_dst = s_dimPal[*s_dst];
                            s_dst++;
                        }
                    }
                }
                s_x = s_x + s_spanCount;
            }
        } else if (s_run != 0) {
            if (clip == ICON_DRAW_NO_CLIP) {
                memcpy(s_row + s_x, s_src, s_run);
            } else if (s_y >= clipY && s_y <= s_clipB && s_x + s_run > clipX && s_x <= s_clipR) {
                if (s_x >= clipX) {
                    if (s_x + s_run <= s_clipR)
                        memcpy(s_row + s_x, s_src, s_run);
                    else
                        memcpy(s_row + s_x, s_src, s_clipR - s_x + 1);
                } else {
                    if (s_x + s_run <= s_clipR)
                        memcpy(s_row + clipX, s_src + clipX - s_x, s_x + s_run - clipX);
                    else
                        memcpy(s_row + clipX, s_src + clipX - s_x, clipW);
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
