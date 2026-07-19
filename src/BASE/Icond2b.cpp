#include <va.h>
#include <BASE/Icond2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
DATA(0x00534bf0) static u8* gDimRow;
DATA(0x00534bf4) static u32 gDimCnt;
DATA(0x00534bf8) static u32 gDimRun;
DATA(0x00534bfc) static i32 gDimY;
DATA(0x00534c00) static u32 gDimCnt2;
DATA(0x00534c04) static IconEntry* gDimEntry;
DATA(0x00534c08) static i32 gDimClipB;
DATA(0x00534c0c) static u8* gDimSrc;
DATA(0x00534c10) static u8* gDimDst;
DATA(0x00534c14) static i32 gDimX;
DATA(0x00534c18) static i32 gDimClipR;
DATA(0x00534c1c) static i32 gDimX0;

VA(0x004cfd50, 0x26e)
void DimIconToBitmap(
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
    i32 entryX = entries[frame].x;
    IconEntry* const entry = &entries[frame];
    u8* const srcData = reinterpret_cast<u8*>(entries) + entries[frame].srcOffset;
    gDimEntry = entry;
    const i32 entryY = entry->y;
    gDimSrc = srcData;
    i32 X = x + entryX;
    gDimX0 = X;
    gDimY = y + entryY;
    i32 right;
    u32 cnt;
    if (clip != 0) {
        if (clipX > gDimX0 || gDimX0 + entry->w > clipX + clipW || gDimY < clipY
            || gDimY + entry->h > clipY + clipH) {
            clip = 1;
            gDimClipR = clipX + clipW - 1;
            gDimClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    i32 rowOffset = gDimY;
    i16 pitch = dest->m_width;
    rowOffset = rowOffset * pitch;
    u8* row = dest->m_pixels + rowOffset;
    for (;;) {
        gDimX = X;
        i32 cmd = ReadIconRleByte(gDimSrc);
        if (static_cast<i8>(cmd) < 0) {
            // skip run / end-of-sprite
            gDimRow = row;
            gDimRun = cmd;
            i32 n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == 0)
                return;
            X = X + n;
            continue;
        }
        gDimRun = cmd;
        if (cmd != 0) {
            if (clip == 0) {
                u8* dst = row + X;
                u32 paletteOffset;
                gDimCnt = 0;
                gDimDst = dst;
                if (static_cast<i32>(cmd) > 0) {
                    paletteOffset = color * sizeof(uDimPal[0][0]);
                    gDimCnt = cmd;
                    cnt = cmd;
                    do {
                        i32 px = *dst++;
                        cnt--;
                        gDimDst = dst;
                        dst[-1] = (&uDimPal[0][0][0])[paletteOffset + px];
                    } while (cnt != 0);
                }
            } else {
                if (gDimY >= clipY && gDimY <= gDimClipB && (right = X + cmd, clipX < right)
                    && X <= gDimClipR) {
                    u32 palOffset;
                    u8* dst;
                    if (X >= clipX) {
                        right = gDimClipR < right ? (gDimClipR - X) + 1 : cmd;
                        dst = row + X;
                    } else {
                        right = gDimClipR < right ? clipW : (cmd - clipX) + X;
                        dst = row + clipX;
                    }
                    cnt = right;
                    i32 cn = cnt;
                    gDimDst = dst;
                    gDimCnt2 = right;
                    gDimCnt = 0;
                    if (cn > 0) {
                        palOffset = color * sizeof(uDimPal[0][0]);
                        gDimCnt = cn;
                        do {
                            i32 px = *dst++;
                            cnt--;
                            gDimDst = dst;
                            dst[-1] = (&uDimPal[0][0][0])[palOffset + px];
                        } while (cnt != 0);
                    }
                }
            }
            X = X + cmd;
            gDimRun = cmd;
            continue;
        }
        X = gDimX0;
        row = row + pitch;
        gDimY = gDimY + 1;
    }
}
