#include <va.h>
#include <BASE/icon2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <SOURCE/KB.h>
#include <BASE/IconShear.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

// Retail's frame is exactly the two __fastcall spills: every working value of
// the decoder lives in a file static, not a local.
DATA(0x00539838) static i32 s_y;
DATA(0x0053983c) static i32 s_clipB;
DATA(0x00539840) static i32 s_x;
DATA(0x00539844) static i32 s_left;
DATA(0x00539848) static u8* s_dimPal;
DATA(0x0053984c) static u8* s_row;
DATA(0x00539850) static u8 s_color;
DATA(0x00539854) static IconEntry* s_entry;
DATA(0x00539858) static u8* s_src;
DATA(0x0053985c) static i32 s_dimLen;
DATA(0x00539860) static i32 s_spanCount;
DATA(0x00539864) static i32 s_clipR;
DATA(0x00539868) static u8* s_dst;
DATA(0x0053986c) static i32 s_loopCount;
DATA(0x00539870) static i32 s_run;
DATA(0x00539874) static i32 s_pitch;

VA(0x004d5a50, 0x683)
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
    s_entry = reinterpret_cast<IconEntry*>(srcIcon->m_data) + frame;
    s_src = srcIcon->m_data + s_entry->srcOffset;
    s_left = x + s_entry->x;
    s_pitch = dest->m_width;
    s_y = y + s_entry->y;
    s_x = s_left + shear[s_y];
    s_clipR = clipX + clipW - 1;
    s_clipB = clipY + clipH - 1;
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
                if (shear[s_y] != ICON_SHEAR_SKIP_ROW && s_y >= clipY && s_y <= s_clipB
                    && s_x + s_run > clipX && s_x <= s_clipR) {
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
                    if (shear[s_y] != ICON_SHEAR_SKIP_ROW && s_y >= clipY && s_y <= s_clipB
                        && s_x + s_dimLen > clipX && s_x <= s_clipR) {
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
            if (shear[s_y] != ICON_SHEAR_SKIP_ROW && s_y >= clipY && s_y <= s_clipB
                && s_x + s_run > clipX && s_x <= s_clipR) {
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
            s_x = s_left + shear[s_y];
            s_y++;
            s_row = s_row + s_pitch;
        }
    }
}
