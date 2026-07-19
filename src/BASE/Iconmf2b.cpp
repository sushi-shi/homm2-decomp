#include <va.h>
#include <BASE/Iconmf2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <string.h>
DATA(0x00538190) static u32 gFMRun;
DATA(0x00538194) static u8* gFMSrc;
DATA(0x00538198) static IconEntry* gFMEntry;
DATA(0x0053819c) static i32 gFMX;
DATA(0x005381a0) static i32 gFMX0;
DATA(0x005381a4) static i32 gFMClipB;
DATA(0x005381a8) static u8* gFMRow;
DATA(0x005381ac) static i32 gFMY;
DATA(0x005381b0) static i32 gFMClipR;
DATA(0x005381b4) static i32 gFMXEnd;

VA(0x004da800, 0x212)
void FlipMonoIconToBitmap(
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
    i32 entryX = entry->x;
    i32 srcOffset = entry->srcOffset;
    gFMEntry = entry;
    i32 x0 = x;
    x0 = x0 - entryX;
    gFMSrc = data + srcOffset;
    i32 w = entry->w;
    x0 = x0 - w;
    i32 entryY = entry->y;
    i32 right = w + x0 + 1;
    x0++;
    gFMX0 = x0;
    i32 X = right - 1;
    gFMXEnd = X;
    gFMY = y + entryY;
    if (clip != 0) {
        if (x0 < clipX || clipX + clipW < right || gFMY < clipY
            || entry->h + gFMY > clipY + clipH) {
            clip = 1;
            gFMClipR = clipX + clipW - 1;
            gFMClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    i16 pitch = dest->m_width;
    gFMRow = dest->m_pixels + gFMY * pitch;
    for (;;) {
        i32 cmd = ReadIconRleByte(gFMSrc);
        gFMX = X;
        if (static_cast<i8>(cmd) < 0) {
            gFMRun = cmd;
            i32 n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == 0)
                return;
            X = X - n;
            continue;
        }
        gFMRun = cmd;
        if (cmd != 0) {
            if (clip == 0) {
                memset((gFMRow - cmd) + 1 + X, color, cmd);
            } else {
                i32 left;
                i32 currentY = gFMY;
                if (clipY <= currentY && currentY <= gFMClipB
                    && (left = (X - cmd) + 1, clipX <= left) && gFMClipR >= X) {
                    if (clipX <= left) {
                        memset((gFMRow - cmd) + 1 + X, color, cmd);
                    } else {
                        memset(gFMRow + clipX, color, (X - clipX) + 1);
                    }
                }
            }
            gFMRun = cmd;
            X = X - cmd;
            continue;
        }
        X = gFMXEnd;
        gFMY = gFMY + 1;
        gFMRow = gFMRow + pitch;
    }
}
