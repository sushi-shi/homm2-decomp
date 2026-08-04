#include <va.h>
#include <BASE/Icond2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMonoRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <SOURCE/KB.h>

H2_ENUM_BEGIN(IconDimConstant)
    DIM_PALETTE_LEVEL_STRIDE = 0x100
H2_ENUM_END(IconDimConstant)

// Retail's frame is exactly the two __fastcall spills: every working value of
// the decoder lives in a file static, not a local.
static i32 s_loopCount;
static i32 s_y;
static IconEntry* s_entry;
static i32 s_left;
static i32 s_x;
static u8* s_src;
static u8* s_dst;
static u8* s_row;
static i32 s_clipR;
static i32 s_clipB;
static i32 s_spanCount;
static i32 s_run;

VA(0x004c6af0, 0x357)
void DimIconToBitmap(
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
    s_left = x + s_entry->x;
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

    s_row = dest->m_pixels + s_y * dest->m_width;

    for (;;) {
        s_run = *s_src;
        s_src++;
        if (static_cast<i8>(s_run) < 0) {
            if ((s_run & ICON_RLE_MONO_RUN_MASK) != ICON_RLE_MONO_END_COUNT)
                s_x += s_run & ICON_RLE_MONO_RUN_MASK;
            else
                return;
        } else if (s_run != ICON_RLE_MONO_NEWLINE_COMMAND) {
            if (clip == ICON_DRAW_NO_CLIP) {
                s_dst = s_row + s_x;
                for (s_loopCount = 0; s_loopCount < s_run; s_loopCount++) {
                    *s_dst = uDimPal[0][0][color * DIM_PALETTE_LEVEL_STRIDE + *s_dst];
                    s_dst++;
                }
            } else if (s_y >= clipY && s_y <= s_clipB && s_x + s_run > clipX && s_x <= s_clipR) {
                if (s_x >= clipX) {
                    if (s_x + s_run <= s_clipR)
                        s_spanCount = s_run;
                    else
                        s_spanCount = s_clipR - s_x + 1;
                    s_dst = s_row + s_x;
                } else {
                    if (s_x + s_run <= s_clipR)
                        s_spanCount = s_x + s_run - clipX;
                    else
                        s_spanCount = clipW;
                    s_dst = s_row + clipX;
                }
                for (s_loopCount = 0; s_loopCount < s_spanCount; s_loopCount++) {
                    *s_dst = uDimPal[0][0][color * DIM_PALETTE_LEVEL_STRIDE + *s_dst];
                    s_dst++;
                }
            }
            s_x = s_x + s_run;
        } else {
            s_x = s_left;
            s_y = s_y + 1;
            s_row = s_row + dest->m_width;
        }
    }
}
