#include <va.h>
#include <BASE/Iconm2b.h>
#include <BASE/IconDraw.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMonoRle.h>
#include <string.h>
#include <SOURCE/KB.h>

// Retail's frame is exactly the two __fastcall spills: every working value of
// the decoder lives in a file static, not a local.
DATA(0x005363ac) static i32 s_x;
DATA(0x005363b0) static i32 s_run;
DATA(0x005363b4) static i32 s_y;
DATA(0x005363b8) static i32 s_left;
DATA(0x005363bc) static u8* s_src;
DATA(0x005363c0) static u8* s_row;
DATA(0x005363c4) static i32 s_clipR;
DATA(0x005363c8) static i32 s_clipB;
DATA(0x005363cc) static IconEntry* s_entry;

VA(0x004c67b0, 0x2e8)
void MonoIconToBitmap(
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
                memset(s_row + s_x, color, s_run);
            } else if (s_y >= clipY && s_y <= s_clipB && s_x + s_run > clipX && s_x <= s_clipR) {
                if (s_x >= clipX) {
                    if (s_x + s_run <= s_clipR)
                        memset(s_row + s_x, color, s_run);
                    else
                        memset(s_row + s_x, color, s_clipR - s_x + 1);
                } else {
                    if (s_x + s_run <= s_clipR)
                        memset(s_row + clipX, color, s_x + s_run - clipX);
                    else
                        memset(s_row + clipX, color, clipW);
                }
            }
            s_x = s_x + s_run;
        } else {
            s_x = s_left;
            s_row = s_row + dest->m_width;
            s_y = s_y + 1;
        }
    }
}
