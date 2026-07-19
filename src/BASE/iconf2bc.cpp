#include <va.h>
#include <BASE/iconf2bc.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
DATA(0x005380d4) static i32 gFCSkip;
DATA(0x00538104) static u32 gFCRun;
DATA(0x005380f8) static i32 gFCX0;
DATA(0x005380e4) static i32 gFCXEnd;
DATA(0x005380e8) static u32 gFCCnt;
DATA(0x005380e0) static u32 gFCCnt2;
DATA(0x005380d8) static i32 gFCY;
DATA(0x005380ec) static IconEntry* gFCEntry;
DATA(0x005380d0) static i32 gFCX;
DATA(0x005380f0) static u8* gFCSrc;
DATA(0x005380dc) static u8* gFCDimPal;
DATA(0x005380f4) static u8* gFCDimDst;
DATA(0x005380fc) static i32 gFCClipB;
DATA(0x00538100) static u8* gFCRow;
DATA(0x005380c4) static u32 gFCDimLen;
DATA(0x005380cc) static u8 gFCColor;
DATA(0x005380c8) static i32 gFCClipR;
DATA(0x005380c0) static u8* gFCDst;

VA(0x004d9790, 0x54d)
void FlipIconToBitmapColorTable(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    i32 clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 color,
    u8* colorTable
) {
    IconEntry* entries = srcIcon->Entries();
    u8* src = reinterpret_cast<u8*>(entries);
    i32 x0 = x;
    i32 w;
    i32 pitch;
    w = entries[frame].w;
    x0 = x0 - entries[frame].x;
    IconEntry* entry = &entries[frame];
    x0 = x0 - w;
    gFCEntry = entry;
    src += entry->srcOffset;
    x0++;
    gFCX0 = x0;
    w = w + x0;
    i32 X = (gFCXEnd = w - 1);
    i32 Y = y + entry->y;
    gFCY = Y;
    if (clip != 0) {
        i32 currentY = gFCY;
        if (x0 < clipX || clipW + clipX < x0 + entry->w || currentY < clipY
            || clipY + clipH < entry->h + currentY) {
            clip = 1;
            gFCClipR = clipX + clipW - 1;
            gFCClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    pitch = dest->m_width;
    gFCRow = dest->m_pixels + gFCY * pitch;
    i32 cmd;
    for (;;) {
        cmd = *src++;
        if (static_cast<i8>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                gFCRun = cmd;
                i32 n = cmd & ICON_RLE_COMMAND_RUN_MASK;
                gFCX = X;
                gFCSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            gFCRun = cmd;
            u32 count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            i32 flags = 0;
            if (count != 0) {
                // 0xc1 - 0xFF : solid colour run
                if (cmd == ICON_RLE_LONG_SOLID_COMMAND)
                    count = *src++;
                gFCColor = colorTable[*src++];
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = *src++;
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0)
                count = *src++;
            gFCDimLen = count;
            if (color != 0) {
                gFCRun = flags;
                if (flags & ICON_RLE_DIM_RECOLOR_FLAG) {
                    gFCCnt2 = count;
                    gFCColor = static_cast<u8>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset((gFCRow - count) + 1 + X, gFCColor, count);
            } else {
                i32 currentY = gFCY;
                i32 left;
                if (currentY >= clipY && currentY <= gFCClipB
                    && (left = (X - count) + 1, clipX <= left) && X <= gFCClipR) {
                    if (clipX <= left) {
                        memset((gFCRow - count) + 1 + X, gFCColor, count);
                    } else {
                        memset(gFCRow + clipX, gFCColor, (X - clipX) + 1);
                    }
                }
            }
            X = X - count;
            gFCRun = count;
            continue;
        do_dim:
            gFCRun = flags;
            gFCCnt2 = count;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                u8* palette =
                    &uDimPal[0][0][0]
                    + (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                gFCDimPal = palette;
                if (clip == 0) {
                    u8* dp = (gFCRow - count) + 1 + X;
                    gFCCnt = 0;
                    i32 dimCount = count;
                    gFCDimDst = dp;
                    if (dimCount > 0) {
                        gFCCnt = dimCount;
                        do {
                            u8* dimPalette = gFCDimPal;
                            i32 px = *dp;
                            dp++;
                            count--;
                            gFCDimDst = dp;
                            dp[-1] = dimPalette[px];
                        } while (count != 0);
                    }
                } else {
                    i32 currentY = gFCY;
                    gFCCnt2 = count;
                    if (currentY >= clipY && currentY <= gFCClipB
                        && clipX <= static_cast<i32>((X - count) + 1) && X <= gFCClipR) {
                        i32 left = (X - count) + 1;
                        u8* dp;
                        if (clipX <= left) {
                            dp = (gFCRow - count) + 1 + X;
                        } else {
                            count = (X - clipX) + 1;
                            dp = gFCRow + clipX;
                        }
                        gFCCnt2 = count;
                        gFCDimDst = dp;
                        i32 dimCount = count;
                        gFCCnt = 0;
                        if (dimCount > 0) {
                            gFCCnt = dimCount;
                            do {
                                u8* dimPalette = gFCDimPal;
                                i32 px = *dp;
                                dp++;
                                count--;
                                gFCDimDst = dp;
                                dp[-1] = dimPalette[px];
                            } while (count != 0);
                        }
                    }
                }
            }
            X = X - gFCDimLen;
            continue;
        }
        gFCRun = cmd;
        gFCX = X;
        if (cmd != 0) {
            if (clip == 0) {
                gFCCnt = 0;
                u8* dst = gFCRow + X;
                gFCDst = dst;
                if (cmd > 0) {
                    gFCCnt = cmd;
                    i32 k = cmd;
                    do {
                        i32 c = *src++;
                        dst--;
                        gFCDst = dst;
                        dst[1] = colorTable[c];
                        k--;
                    } while (k != 0);
                }
            } else {
                i32 currentY = gFCY;
                if (currentY >= clipY && currentY <= gFCClipB) {
                    i32 left = (X - cmd) + 1;
                    if (left <= gFCClipR && clipX <= X) {
                        u32 cn;
                        u8* selectedDst;
                        i32 skip;
                        if (X <= gFCClipR) {
                            selectedDst = gFCRow + X;
                            gFCDst = selectedDst;
                            if (clipX <= left) {
                                gFCSkip = 0;
                                cn = cmd;
                            } else {
                                cn = (X - clipX) + 1;
                                skip = cmd - cn;
                                goto set_skip;
                            }
                        } else {
                            i32 right = gFCClipR;
                            src = src + (X - right);
                            selectedDst = gFCRow + right;
                            gFCDst = selectedDst;
                            if (clipX <= (X - cmd)) {
                                gFCSkip = 0;
                                cn = (cmd - X) + gFCClipR;
                            } else {
                                skip = gFCClipR + ((cmd - X) - clipW);
                                cn = clipW;
                                goto set_skip;
                            }
                        }
                        goto skip_set;
                    set_skip:
                        gFCSkip = skip;
                    skip_set:
                        i32 copyCount = cn;
                        gFCCnt2 = cn;
                        gFCCnt = 0;
                        if (copyCount > 0) {
                            gFCCnt = copyCount;
                            do {
                                u8* dst = gFCDst;
                                i32 c = *src++;
                                dst--;
                                cn--;
                                u8 mapped = colorTable[c];
                                gFCDst = dst;
                                dst[1] = mapped;
                            } while (cn != 0);
                        }
                        src = src + gFCSkip;
                    } else {
                        src = src + cmd;
                    }
                } else {
                    src = src + cmd;
                }
            }
            X = X - cmd;
            gFCRun = cmd;
            continue;
        }
        X = gFCXEnd;
        gFCY = gFCY + 1;
        gFCRow = gFCRow + pitch;
    }
}
