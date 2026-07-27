#include <va.h>
#include <BASE/Iconm2b.h>
#include <BASE/IconDraw.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <string.h>
DATA(0x00534bcc) static i32 gMonoClipR;
DATA(0x00534bd0) static u8* gMonoRow;
DATA(0x00534bd4) static IconEntry* gMonoEntry;
DATA(0x00534bd8) static u8* gMonoSrc;
DATA(0x00534bdc) static i32 gMonoX0;
DATA(0x00534be0) static u32 gMonoRun;
DATA(0x00534be4) static i32 gMonoY;
DATA(0x00534be8) static i32 gMonoClipB;
DATA(0x00534bec) static i32 gMonoX;

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

static inline i32 MonoRowVisible(i32 clipTop) {
    return clipTop <= gMonoY && gMonoY <= gMonoClipB;
}

static inline u8* MonoInitialRow(bitmap* dest, i16 pitch) {
    return dest->m_pixels + gMonoY * pitch;
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
    i32 entryX = entries[frame].x;
    IconEntry* const entry = &entries[frame];
    u8* const srcData = reinterpret_cast<u8*>(entries) + entries[frame].srcOffset;
    gMonoEntry = entry;
    const i32 entryY = entry->y;
    gMonoSrc = srcData;
    gMonoX0 = x + entryX;
    gMonoX = gMonoX0;
    gMonoY = y + entryY;
    if (clip != ICON_DRAW_NO_CLIP) {
        if (MonoNeedsClipping(entry, gMonoX0, gMonoY, clipX, clipY, clipW, clipH)) {
            clip = ICON_DRAW_CLIP;
            gMonoClipR = clipX + clipW - 1;
            gMonoClipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
    i16 pitch = dest->m_width;
    u8* row = MonoInitialRow(dest, pitch);
    for (;;) {
        i32 cmd = ReadIconRleByte(gMonoSrc);
        if (static_cast<i8>(cmd) < 0) {
            // skip run / end-of-sprite (negative command masks 7 bits)
            gMonoRow = row;
            gMonoRun = cmd;
            i32 n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == ICON_RLE_MONO_END_COUNT)
                return;
            gMonoX = gMonoX + n;
            continue;
        }
        gMonoRun = cmd;
        if (cmd != ICON_RLE_MONO_NEWLINE_COMMAND) {
            // solid mono fill of `cmd` pixels
            if (clip == ICON_DRAW_NO_CLIP) {
                memset(row + gMonoX, color, cmd);
            } else {
                i32 right;
                if (MonoRowVisible(clipY) && (right = gMonoX + cmd, clipX < right)
                    && gMonoClipR >= gMonoX) {
                    if (clipX <= gMonoX) {
                        if (gMonoClipR >= right) {
                            memset(row + gMonoX, color, cmd);
                        } else {
                            memset(row + gMonoX, color, (gMonoClipR - gMonoX) + 1);
                        }
                    } else {
                        if (gMonoClipR >= right) {
                            memset(row + clipX, color, (cmd - clipX) + gMonoX);
                        } else {
                            memset(row + clipX, color, clipW);
                        }
                    }
                }
            }
            gMonoRun = cmd;
            gMonoX = gMonoX + cmd;
            continue;
        }
        gMonoX = gMonoX0;
        row = row + pitch;
        gMonoY = gMonoY + 1;
    }
}
