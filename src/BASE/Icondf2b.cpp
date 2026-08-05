#include <va.h>
#include <BASE/Icondf2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMonoRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <SOURCE/KB.h>

H2_ENUM_BEGIN(IconFlipDimConstant)
    DIM_PALETTE_LEVEL_STRIDE = 0x100
H2_ENUM_END(IconFlipDimConstant)

// Retail's frame is exactly the two __fastcall spills: every working value of
// the decoder lives in a file static, not a local.
DATA(0x005398a0) static i32 s_loopCount;
DATA(0x005398a4) static i32 s_x;
DATA(0x005398a8) static u8* s_src;
DATA(0x005398ac) static u8* s_dst;
DATA(0x005398b0) static i32 s_y;
DATA(0x005398b4) static u8* s_row;
DATA(0x005398b8) static i32 s_left;
DATA(0x005398bc) static IconEntry* s_entry;
DATA(0x005398c0) static i32 s_run;
DATA(0x005398c4) static i32 s_spanCount;
DATA(0x005398c8) static i32 s_clipR;
DATA(0x005398cc) static i32 s_clipB;
DATA(0x005398d0) static i32 s_right;

VA(0x004d64b0, 0x3c3)
void FlipDimIconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
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
            if ((s_run & ICON_RLE_MONO_RUN_MASK) != ICON_RLE_MONO_END_COUNT)
                s_x -= s_run & ICON_RLE_MONO_RUN_MASK;
            else
                return;
        } else if (s_run != ICON_RLE_MONO_NEWLINE_COMMAND) {
            if (clip == ICON_DRAW_NO_CLIP) {
                s_dst = s_row + s_x - s_run + 1;
                for (s_loopCount = 0; s_loopCount < s_run; s_loopCount++) {
                    *s_dst = uDimPal[0][0][color * DIM_PALETTE_LEVEL_STRIDE + *s_dst];
                    s_dst++;
                }
            } else if (s_y >= clipY && s_y <= s_clipB && s_x - s_run + 1 >= clipX
                       && s_x <= s_clipR) {
                if (s_x <= s_clipR) {
                    if (s_x - s_run + 1 >= clipX) {
                        s_spanCount = s_run;
                        s_dst = s_row + s_x - s_run + 1;
                    } else {
                        s_spanCount = s_x - clipX + 1;
                        s_dst = s_row + clipX;
                    }
                } else {
                    if (s_x - s_run + 1 >= clipX) {
                        s_spanCount = s_clipR - (s_x - s_run) + 1;
                        s_dst = s_row + s_x - s_run + 1;
                    } else {
                        s_spanCount = clipW;
                        s_dst = s_row + clipX;
                    }
                }
                for (s_loopCount = 0; s_loopCount < s_spanCount; s_loopCount++) {
                    *s_dst = uDimPal[0][0][color * DIM_PALETTE_LEVEL_STRIDE + *s_dst];
                    s_dst++;
                }
            }
            s_x = s_x - s_run;
        } else {
            s_x = s_right;
            s_y = s_y + 1;
            s_row = s_row + dest->m_width;
        }
    }
}
