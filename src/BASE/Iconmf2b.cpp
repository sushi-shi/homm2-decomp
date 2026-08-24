#include <Ints.h>
#include <BASE/Iconmf2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <string.h>

static i32 s_clipB;
static i32 s_y;
static i32 s_x;
static i32 s_left;
static i32 s_right;
static u32 s_run;
static u8* s_row;
static u8* s_src;
static IconEntry* s_entry;
static i32 s_clipR;

static inline u8* FlipMonoInitialRow(bitmap* dest, i16 pitch, i32 currentY) {
    return dest->m_pixels + currentY * pitch;
}

static inline i32 FlipMonoRowVisible(i32 clipTop, i32 currentY, i32 clipBottom) {
    return clipTop <= currentY && currentY <= clipBottom;
}

void FlipMonoIconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    IconEntry* entry = &srcIcon->Entries()[frame];
    u8* data = srcIcon->m_data;
    s_entry = entry;
    i32 entryX = entry->x;
    i32 x0 = x - entryX;
    s_src = data + entry->srcOffset;
    i32 width = entry->w;
    x0 = (x0 - width) + 1;
    s_left = x0;
    i32 X = (width - 1) + x0;
    s_right = X;
    s_y = y + entry->y;
    if (clip != ICON_DRAW_NO_CLIP) {
        if (clipX > x0 || clipW + clipX < width + x0 || clipY > s_y
            || entry->h + s_y > clipY + clipH) {
            clip = ICON_DRAW_CLIP;
            s_clipR = clipX + clipW - 1;
            s_clipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
    i16 pitch = dest->m_width;
    s_row = FlipMonoInitialRow(dest, pitch, s_y);
    for (;;) {
        s_x = X;
        i32 cmd = ReadIconRleByte(s_src);
        if (static_cast<i8>(cmd) < 0) {
            s_run = cmd;
            i32 n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == 0)
                return;
            X = X - n;
            continue;
        }
        s_run = cmd;
        if (cmd != 0) {
            if (clip == ICON_DRAW_NO_CLIP) {
                memset((s_row - cmd) + 1 + X, color, cmd);
            } else {
                i32 left;
                if (FlipMonoRowVisible(clipY, s_y, s_clipB)
                    && (left = (X - cmd) + 1, clipX <= left) && s_clipR >= X) {
                    if (clipX <= left) {
                        memset((s_row - cmd) + 1 + X, color, cmd);
                    } else {
                        memset(s_row + clipX, color, (X - clipX) + 1);
                    }
                }
            }
            s_run = cmd;
            X = X - cmd;
            continue;
        }
        X = s_right;
        s_y = s_y + 1;
        s_row = s_row + pitch;
    }
}
