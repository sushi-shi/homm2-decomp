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

VA(0x004cfae0, 0x266)
void MonoIconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    i32 clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    u8* data = srcIcon->m_data;
    IconEntry* entry = &srcIcon->Entries()[frame];
    gMonoEntry = entry;
    gMonoSrc = data + entry->srcOffset;
    gMonoX0 = entry->x + x;
    gMonoX = gMonoX0;
    gMonoY = entry->y + y;
    if (clip != IDX(ICON_DRAW_NO_CLIP)) {
        if (clipX > gMonoX0 || gMonoX0 + entry->w > clipX + clipW || clipY > gMonoY
            || gMonoY + entry->h > clipY + clipH) {
            clip = IDX(ICON_DRAW_CLIP);
            gMonoClipR = clipX + clipW - 1;
            gMonoClipB = clipY + clipH - 1;
        } else {
            clip = IDX(ICON_DRAW_NO_CLIP);
        }
    }
    i16 pitch = dest->m_width;
    u8* row = dest->m_pixels + gMonoY * pitch;
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
            if (clip == IDX(ICON_DRAW_NO_CLIP)) {
                memset(row + gMonoX, color, cmd);
            } else {
                i32 right;
                if (clipY <= gMonoY && gMonoClipB >= gMonoY && (right = gMonoX + cmd, clipX < right)
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
