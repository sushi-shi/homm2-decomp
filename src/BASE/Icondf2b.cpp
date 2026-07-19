#include <va.h>
#include <BASE/Icondf2b.h>
#include <BASE/IconDraw.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMonoRle.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
DATA(0x005381b8) static i32 gFDX0;
DATA(0x005381bc) static i32 gFDXEnd;
DATA(0x005381c0) static u32 gFDCnt;
DATA(0x005381c4) static i32 gFDX;
DATA(0x005381c8) static i32 gFDClipR;
DATA(0x005381cc) static u32 gFDCnt2;
DATA(0x005381d0) static u8* gFDRow;
DATA(0x005381d4) static i32 gFDClipB;
DATA(0x005381d8) static u8* gFDSrc;
DATA(0x005381dc) static u8* gFDDst;
DATA(0x005381e0) static i32 gFDY;
DATA(0x005381e4) static IconEntry* gFDEntry;
DATA(0x005381e8) static u32 gFDRun;

VA(0x004daa20, 0x23b)
void FlipDimIconToBitmap(
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
    IconEntry* entries = srcIcon->Entries();
    IconEntry* entry = &entries[frame];
    u8* srcData = reinterpret_cast<u8*>(entries) + entry->srcOffset;
    i32 x0 = x;
    gFDEntry = entry;
    gFDSrc = srcData;
    i32 w = entry->w;
    i32 entryY = entry->y;
    x0 = x0 - entry->x;
    x0 = x0 - w;
    x0++;
    gFDX0 = x0;
    gFDY = y + entryY;
    i32 X = w + x0 - 1;
    gFDXEnd = X;
    if (clip != IDX(ICON_DRAW_NO_CLIP)) {
        i32 currentY;
        if (x0 < clipX || clipW + clipX < w + x0 || (currentY = gFDY) < clipY
            || entry->h + currentY > clipY + clipH) {
            clip = IDX(ICON_DRAW_CLIP);
            gFDClipR = clipX + clipW - 1;
            gFDClipB = clipY + clipH - 1;
        } else {
            clip = IDX(ICON_DRAW_NO_CLIP);
        }
    }
    i16 pitch = dest->m_width;
    gFDRow = dest->m_pixels + gFDY * pitch;
    for (;;) {
        gFDX = X;
        i32 cmd = ReadIconRleByte(gFDSrc);
        if (static_cast<i8>(cmd) < 0) {
            gFDRun = cmd;
            i32 n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == ICON_RLE_MONO_END_COUNT)
                return;
            X = X - n;
            continue;
        }
        gFDRun = cmd;
        if (cmd != ICON_RLE_MONO_NEWLINE_COMMAND) {
            if (clip == IDX(ICON_DRAW_NO_CLIP)) {
                u32 cnt;
                gFDCnt = 0;
                u8* dst = (gFDRow - cmd) + X + 1;
                gFDDst = dst;
                if (static_cast<i32>(cmd) > 0) {
                    cnt = cmd;
                    gFDCnt = cmd;
                    do {
                        i32 px = *dst++;
                        cnt--;
                        gFDDst = dst;
                        dst[-1] = uDimPal[0][color][px];
                    } while (cnt != 0);
                }
            } else {
                i32 left;
                i32 currentY = gFDY;
                if (clipY <= currentY && currentY <= gFDClipB
                    && (left = (X - cmd) + 1, clipX <= left) && X <= gFDClipR) {
                    i32 cn;
                    u8* dst;
                    if (clipX <= left) {
                        cn = cmd;
                        dst = (gFDRow - cmd) + X + 1;
                    } else {
                        dst = gFDRow + clipX;
                        cn = (X - clipX) + 1;
                    }
                    gFDCnt2 = cn;
                    gFDDst = dst;
                    gFDCnt = 0;
                    if (static_cast<i32>(cn) > 0) {
                        gFDCnt = cn;
                        u32 cnt = cn;
                        do {
                            i32 px = *dst++;
                            cnt--;
                            gFDDst = dst;
                            dst[-1] = uDimPal[0][color][px];
                        } while (cnt != 0);
                    }
                }
            }
            X = X - cmd;
            gFDRun = cmd;
            continue;
        }
        X = gFDXEnd;
        gFDY = gFDY + 1;
        gFDRow = gFDRow + pitch;
    }
}
