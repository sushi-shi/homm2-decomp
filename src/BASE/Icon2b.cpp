#include <va.h>
#include <BASE/Icon2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
DATA(0x00534c20) static u8* gIcRow;
DATA(0x00534c24) static i32 gIcPitch;
DATA(0x00534c28) static u8 gIcColor;
DATA(0x00534c2c) static u8* gIcDimPal;
DATA(0x00534c30) static u32 gIcRun;
DATA(0x00534c34) static u32 gIcCnt;
DATA(0x00534c38) static u8* gIcSrc;
DATA(0x00534c3c) static u8* gIcDimDst;
DATA(0x00534c40) static i32 gIcClipR;
DATA(0x00534c44) static i32 gIcClipB;
DATA(0x00534c48) static i32 gIcX0;
DATA(0x00534c4c) static u32 gIcDimLen;
DATA(0x00534c50) static i32 gIcY;
DATA(0x00534c54) static i32 gIcX;
DATA(0x00534c58) static IconEntry* gIcEntry;
DATA(0x00534c5c) static u32 gIcCnt2;

// @semantic: First divergence is +0x11: ours hoists x before the indexed entry fields.
VA(0x004d0570, 0x4ed)
void IconToBitmap(
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
    i32 color
) {
    u8* data = srcIcon->m_data;
    IconEntry* entry = &srcIcon->Entries()[frame];
    i32 entryX = entry->x;
    i32 srcOffset = entry->srcOffset;
    u8* cursor = data + srcOffset;
    gIcEntry = entry;
    gIcSrc = cursor;
    i32 X = entryX + x;
    i32 Y = entry->y + y;
    gIcX0 = X;
    gIcPitch = dest->m_width;
    gIcY = Y;
    if (clip != 0) {
        if (gIcX0 < clipX || clipW + clipX < entry->w + gIcX0 || gIcY < clipY
            || clipY + clipH < entry->h + gIcY) {
            clip = 1;
            gIcClipR = clipX + clipW - 1;
            gIcClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    u8* row = dest->m_pixels + gIcPitch * gIcY;
    i32 cmd;
    for (;;) {
        cmd = ReadIconRleByte(gIcSrc);
        if (static_cast<i8>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                gIcX = X;
                gIcRow = row;
                gIcRun = cmd;
                if ((cmd & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                X = X + (cmd & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }
            gIcRun = cmd;
            u32 count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            i32 flags;
            if (count != 0) {
                // 0xc1 - 0xFF : solid colour run
                if (cmd == ICON_RLE_LONG_SOLID_COMMAND) {
                    count = ReadIconRleByte(gIcSrc);
                }
                gIcColor = ReadIconRleByte(gIcSrc);
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = ReadIconRleByte(gIcSrc);
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0) {
                count = ReadIconRleByte(gIcSrc);
            }
            gIcDimLen = count;
            if (color != 0) {
                gIcRun = flags;
                if (flags & ICON_RLE_DIM_RECOLOR_FLAG) {
                    gIcCnt2 = count;
                    gIcColor = static_cast<u8>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(row + X, gIcColor, count);
            } else {
                i32 right;
                if (clipY <= gIcY && gIcClipB >= gIcY && (right = X + count, clipX < right)
                    && gIcClipR >= X) {
                    if (clipX <= X) {
                        if (gIcClipR >= right)
                            memset(row + X, gIcColor, count);
                        else
                            memset(row + X, gIcColor, (gIcClipR - X) + 1);
                    } else {
                        u32 cn = clipW;
                        if (right <= gIcClipR)
                            cn = (count - clipX) + X;
                        memset(row + clipX, gIcColor, cn);
                    }
                }
            }
            X = X + count;
            gIcRun = count;
            continue;
        do_dim:
            gIcCnt2 = count;
            gIcRun = flags;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                u32 lvl = (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                u8* palette = &uDimPal[0][0][0] + lvl;
                if (clip == 0) {
                    u8* dp = row + X;
                    gIcDimPal = palette;
                    gIcDimDst = dp;
                    gIcCnt = 0;
                    if (static_cast<i32>(count) > 0) {
                        gIcCnt = count;
                        do {
                            i32 px = *dp++;
                            gIcDimDst = dp;
                            count--;
                            gIcDimPal = palette;
                            dp[-1] = palette[px];
                        } while (count != 0);
                    }
                } else {
                    gIcCnt2 = count;
                    gIcDimPal = palette;
                    i32 right;
                    if (clipY <= gIcY && gIcClipB >= gIcY && (right = X + count, clipX < right)
                        && gIcClipR >= X) {
                        u32 cn;
                        u8* dst;
                        if (clipX <= X) {
                            cn = count;
                            if (gIcClipR < right)
                                cn = (gIcClipR - X) + 1;
                            dst = row + X;
                        } else {
                            gIcCnt2 = count;
                            if (gIcClipR < right)
                                cn = clipW;
                            else
                                cn = (count - clipX) + X;
                            dst = row + clipX;
                        }
                        gIcCnt2 = cn;
                        gIcDimDst = dst;
                        gIcDimPal = palette;
                        gIcCnt = 0;
                        if (static_cast<i32>(cn) > 0) {
                            gIcCnt = cn;
                            do {
                                i32 px = *dst++;
                                gIcDimDst = dst;
                                cn--;
                                gIcDimPal = palette;
                                dst[-1] = palette[px];
                            } while (cn != 0);
                        }
                    }
                }
            }
            X = X + gIcDimLen;
            continue;
        }
        gIcX = X;
        gIcRun = cmd;
        if (cmd != 0) {
            i32 right;
            u32 copyCount;
            u8* copyDst;
            u8* copySrc;
            do {
                if (clip == 0) {
                    copyCount = cmd;
                    copyDst = row + X;
                    copySrc = gIcSrc;
                } else {
                    if (gIcY < clipY || gIcClipB < gIcY)
                        break;
                    right = X + cmd;
                    if (right <= clipX || gIcClipR < X)
                        break;
                    if (clipX <= X) {
                        if (gIcClipR >= right) {
                            copyCount = cmd;
                            copyDst = row + X;
                            copySrc = gIcSrc;
                        } else {
                            copyCount = (gIcClipR - X) + 1;
                            copyDst = row + X;
                            copySrc = gIcSrc;
                        }
                    } else {
                        if (gIcClipR >= right)
                            copyCount = (cmd - clipX) + X;
                        else
                            copyCount = clipW;
                        copySrc = gIcSrc + (clipX - X);
                        copyDst = row + clipX;
                    }
                }
                memcpy(copyDst, copySrc, copyCount);
            } while (0);
            X = X + cmd;
            gIcSrc = gIcSrc + cmd;
            gIcRun = cmd;
            continue;
        }
        X = gIcX0;
        gIcY = gIcY + 1;
        row = row + gIcPitch;
    }
}
