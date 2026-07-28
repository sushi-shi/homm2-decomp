#include <va.h>
#include <BASE/Iconm2b.h>
#include <BASE/IconDraw.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <string.h>

DATA(0x00534be8) static i32 s_clipB;
DATA(0x00534be4) static i32 s_y;
DATA(0x00534bec) static i32 s_x;
DATA(0x00534bdc) static i32 s_left;
DATA(0x00534be0) static u32 s_run;
DATA(0x00534bd0) static u8* s_row;
DATA(0x00534bd8) static u8* s_src;
DATA(0x00534bd4) static IconEntry* s_entry;
DATA(0x00534bcc) static i32 s_clipR;

static inline i32 MonoNeedsClipping(
    IconEntry* entry,
    i32 x0,
    i32 currentY,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    return clipX > x0 || x0 + entry->w > clipX + clipW || clipY > currentY
           || currentY + entry->h > clipY + clipH;
}

static inline i32 MonoRowVisible(i32 clipTop, i32 currentY, i32 clipBottom) {
    return clipTop <= currentY && currentY <= clipBottom;
}

static inline u8* MonoInitialRow(bitmap* dest, i16 pitch, i32 currentY) {
    return dest->m_pixels + currentY * pitch;
}

VA(0x004cfae0, 0x266)
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
    IconEntry* entries = srcIcon->Entries();
    u8* data = srcIcon->m_data;
    s_entry = &entries[frame];
    s_src = data + s_entry->srcOffset;
    s_left = x + s_entry->x;
    s_x = s_left;
    s_y = y + s_entry->y;
    if (clip != ICON_DRAW_NO_CLIP) {
        if (MonoNeedsClipping(s_entry, s_left, s_y, clipX, clipY, clipW, clipH)) {
            clip = ICON_DRAW_CLIP;
            s_clipR = clipX + clipW - 1;
            s_clipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
    i16 pitch = dest->m_width;
    u8* row = MonoInitialRow(dest, pitch, s_y);
    for (;;) {
        i32 cmd = ReadIconRleByte(s_src);
        if (static_cast<i8>(cmd) < 0) {
            // skip run / end-of-sprite (negative command masks 7 bits)
            s_row = row;
            s_run = cmd;
            i32 n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == ICON_RLE_MONO_END_COUNT)
                return;
            s_x = s_x + n;
            continue;
        }
        s_run = cmd;
        if (cmd != ICON_RLE_MONO_NEWLINE_COMMAND) {
            // solid mono fill of `cmd` pixels
            if (clip == ICON_DRAW_NO_CLIP) {
                memset(row + s_x, color, cmd);
            } else {
                i32 right;
                if (MonoRowVisible(clipY, s_y, s_clipB) && (right = s_x + cmd, clipX < right)
                    && s_clipR >= s_x) {
                    if (clipX <= s_x) {
                        if (s_clipR >= right) {
                            memset(row + s_x, color, cmd);
                        } else {
                            memset(row + s_x, color, (s_clipR - s_x) + 1);
                        }
                    } else {
                        if (s_clipR >= right) {
                            memset(row + clipX, color, (cmd - clipX) + s_x);
                        } else {
                            memset(row + clipX, color, clipW);
                        }
                    }
                }
            }
            s_run = cmd;
            s_x = s_x + cmd;
            continue;
        }
        s_x = s_left;
        row = row + pitch;
        s_y = s_y + 1;
    }
}
