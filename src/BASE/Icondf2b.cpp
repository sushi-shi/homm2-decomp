#include <Ints.h>
#include <BASE/Icondf2b.h>
#include <BASE/IconDraw.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMonoRle.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>

static i32 s_clipB;
static u32 s_loopCount;
static i32 s_y;
static i32 s_x;
static i32 s_left;
static i32 s_right;
static u32 s_run;
static u8* s_row;
static u8* s_dst;
static u8* s_src;
static IconEntry* s_entry;
static u32 s_spanCount;
static i32 s_clipR;

static inline i32 FlipDimRowVisible(i32 clipTop, i32 currentY, i32 clipBottom) {
    return clipTop <= currentY && currentY <= clipBottom;
}

void FlipDimIconToBitmap(
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
    IconEntry* entries = srcIcon->Entries();
    u8* data = srcIcon->m_data;
    IconEntry* entry = &entries[frame];
    s_entry = entry;
    s_src = data + entries[frame].srcOffset;
    i32 width = entry->w;
    i32 x0 = x - entry->x;
    s_y = y + entry->y;
    x0 = x0 - width;
    x0++;
    s_left = x0;
    i32 X = (width - 1) + x0;
    s_right = X;
    if (clip != ICON_DRAW_NO_CLIP) {
        if (x0 < clipX || clipW + clipX < width + x0 || clipY > s_y
            || entry->h + s_y > clipY + clipH) {
            clip = ICON_DRAW_CLIP;
            s_clipR = clipX + clipW - 1;
            s_clipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
    i16 pitch = dest->m_width;
    s_row = dest->m_pixels + s_y * pitch;
    for (;;) {
        s_x = X;
        i32 cmd = ReadIconRleByte(s_src);
        if (static_cast<i8>(cmd) < 0) {
            s_run = cmd;
            i32 n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == ICON_RLE_MONO_END_COUNT)
                return;
            X = X - n;
            continue;
        }
        s_run = cmd;
        if (cmd != ICON_RLE_MONO_NEWLINE_COMMAND) {
            if (clip == ICON_DRAW_NO_CLIP) {
                u32 cnt;
                s_loopCount = 0;
                u8* dst = (s_row - cmd) + X + 1;
                s_dst = dst;
                if (cmd > 0) {
                    cnt = cmd;
                    s_loopCount = cmd;
                    do {
                        i32 px = *dst++;
                        cnt--;
                        s_dst = dst;
                        dst[-1] = uDimPal[0][color][px];
                    } while (cnt != 0);
                }
            } else {
                i32 left;
                if (FlipDimRowVisible(clipY, s_y, s_clipB) && (left = (X - cmd) + 1, clipX <= left)
                    && X <= s_clipR) {
                    i32 cn;
                    u8* dst;
                    if (clipX <= left) {
                        cn = cmd;
                        dst = (s_row - cmd) + X + 1;
                    } else {
                        cn = (X - clipX) + 1;
                        dst = s_row + clipX;
                    }
                    s_spanCount = cn;
                    s_dst = dst;
                    s_loopCount = 0;
                    if (cn > 0) {
                        s_loopCount = cn;
                        u32 cnt = cn;
                        do {
                            i32 px = *dst++;
                            cnt--;
                            s_dst = dst;
                            dst[-1] = uDimPal[0][color][px];
                        } while (cnt != 0);
                    }
                }
            }
            X = X - cmd;
            s_run = cmd;
            continue;
        }
        X = s_right;
        s_y = s_y + 1;
        s_row = s_row + pitch;
    }
}
