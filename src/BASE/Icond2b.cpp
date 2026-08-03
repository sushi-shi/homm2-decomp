#include <va.h>
#include <BASE/Icond2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <SOURCE/KB.h>

static i32 s_clipB;
static u32 s_loopCount;
static i32 s_y;
static i32 s_x;
static i32 s_left;
static u32 s_run;
static u8* s_row;
static u8* s_dst;
static u8* s_src;
static IconEntry* s_entry;
static u32 s_spanCount;
static i32 s_clipR;

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
    IconEntry* entries = srcIcon->Entries();
    i32 entryX = entries[frame].x;
    s_entry = &entries[frame];
    s_src = srcIcon->m_data + entries[frame].srcOffset;
    s_left = x + entryX;
    i32 X = s_left;
    s_y = y + s_entry->y;
    i32 right;
    u32 cnt;
    if (clip != ICON_DRAW_NO_CLIP) {
        if (s_left < clipX || s_left + s_entry->w > clipX + clipW || clipY > s_y
            || s_y + s_entry->h > clipY + clipH) {
            clip = ICON_DRAW_CLIP;
            s_clipR = clipX + clipW - 1;
            s_clipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
    i32 rowOffset = s_y;
    i16 pitch = dest->m_width;
    rowOffset = rowOffset * pitch;
    u8* row = dest->m_pixels + rowOffset;
    for (;;) {
        s_x = X;
        i32 cmd = ReadIconRleByte(s_src);
        if (static_cast<i8>(cmd) < 0) {
            s_row = row;
            s_run = cmd;
            i32 n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == 0)
                return;
            X = X + n;
            continue;
        }
        s_run = cmd;
        if (cmd != 0) {
            if (clip == ICON_DRAW_NO_CLIP) {
                u8* dst = row + X;
                u32 paletteOffset;
                s_loopCount = 0;
                s_dst = dst;
                if (cmd > 0) {
                    paletteOffset = color * sizeof(uDimPal[0][0]);
                    s_loopCount = cmd;
                    cnt = cmd;
                    do {
                        i32 px = *dst++;
                        cnt--;
                        s_dst = dst;
                        dst[-1] = (&uDimPal[0][0][0])[paletteOffset + px];
                    } while (cnt != 0);
                }
            } else {
                if (s_y >= clipY && s_y <= s_clipB && (right = X + cmd, clipX < right)
                    && X <= s_clipR) {
                    u32 palOffset;
                    u8* dst;
                    if (X >= clipX) {
                        right = s_clipR < right ? (s_clipR - X) + 1 : cmd;
                        dst = row + X;
                    } else {
                        right = s_clipR < right ? clipW : (cmd - clipX) + X;
                        dst = row + clipX;
                    }
                    cnt = right;
                    i32 cn = cnt;
                    s_dst = dst;
                    s_spanCount = right;
                    s_loopCount = 0;
                    if (cn > 0) {
                        palOffset = color * sizeof(uDimPal[0][0]);
                        s_loopCount = cn;
                        do {
                            i32 px = *dst++;
                            cnt--;
                            s_dst = dst;
                            dst[-1] = (&uDimPal[0][0][0])[palOffset + px];
                        } while (cnt != 0);
                    }
                }
            }
            X = X + cmd;
            s_run = cmd;
            continue;
        }
        X = s_left;
        row = row + pitch;
        s_y = s_y + 1;
    }
}
