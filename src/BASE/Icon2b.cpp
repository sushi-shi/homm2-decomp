#include <va.h>
#include <BASE/Icon2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRleFill.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

static inline i32 IconRowVisible(i32 currentY, i32 clipBottom, i32 clipTop) {
    return clipTop <= currentY && currentY <= clipBottom;
}

VA(0x004d0570, 0x4ed)
void IconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 color
) {
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

    u8* data = srcIcon->m_data;
    i32 entryOffset = frame * sizeof(IconEntry);
    u8* entryData = data + entryOffset;
    i32 entryX = reinterpret_cast<IconEntry*>(entryData)->x;
    i32 sourceOffset = reinterpret_cast<IconEntry*>(entryData)->srcOffset;
    IconEntry* entry = reinterpret_cast<IconEntry*>(entryData);
    u8* savedDst;
    gIcEntry = entry;
    gIcSrc = data + sourceOffset;
    x += entryX;
    y += entry->y;
    i32 pitch = dest->m_width;
    gIcX0 = x;
    gIcPitch = pitch;
    gIcY = y;
    i32 X = x;
    i32 currentY = y;

    if (clip != ICON_DRAW_NO_CLIP) {
        if (gIcX0 < clipX || clipW + clipX < entry->w + gIcX0 || gIcY < clipY
            || clipY + clipH < entry->h + gIcY) {
            clip = ICON_DRAW_CLIP;
            gIcClipR = clipX + clipW - 1;
            gIcClipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }

    i32 rowPitch = gIcPitch;
    i32 rowY = gIcY;
    u8* row = dest->m_pixels + rowPitch * rowY;
    i32 command;

    for (;;) {
        command = ReadIconRleByte(gIcSrc);
        if (static_cast<i8>(command) < 0) {
            if ((command & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                gIcX = X;
                gIcRow = row;
                gIcRun = command;
                if ((command & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                X = X + (command & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }

            gIcRun = command;
            u32 count = command & ICON_RLE_COMMAND_RUN_MASK;
            i32 flags = 0;
            if (count != 0) {
                if (command == ICON_RLE_LONG_SOLID_COMMAND)
                    count = ReadIconRleByte(gIcSrc);
                gIcColor = ReadIconRleByte(gIcSrc);
                goto fill_run;
            }

            flags = ReadIconRleByte(gIcSrc);
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0)
                count = ReadIconRleByte(gIcSrc);
            gIcDimLen = count;

            if (color != 0) {
                gIcRun = flags;
                if ((flags & ICON_RLE_DIM_RECOLOR_FLAG) != 0) {
                    gIcCnt2 = count;
                    gIcColor = static_cast<u8>(color);
                    goto fill_run;
                }
            }
            goto dim_run;

        fill_run:
            if (clip == ICON_DRAW_NO_CLIP) {
                memset(row + X, gIcColor, count);
            } else
                H2_ICON_RLE_CLIPPED_FILL(
                    IconRowVisible(gIcY, gIcClipB, clipY),
                    row,
                    X,
                    gIcColor,
                    count,
                    clipX,
                    clipW,
                    gIcClipR
                );
            X = X + count;
            gIcRun = count;
            continue;

        dim_run:
            gIcCnt2 = count;
            gIcRun = flags;
            if ((flags & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                u8* palette =
                    reinterpret_cast<u8*>(uDimPal)
                    + (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                if (clip == ICON_DRAW_NO_CLIP) {
                    savedDst = row + X;
                    gIcDimPal = palette;
                    gIcDimDst = savedDst;
                    gIcCnt = 0;
                    i32 dimCount = count;
                    if (dimCount > 0) {
                        gIcCnt = dimCount;
                        do {
                            i32 b = *savedDst;
                            savedDst = savedDst + 1;
                            gIcDimDst = savedDst;
                            count--;
                            gIcDimPal = palette;
                            savedDst[-1] = palette[b];
                        } while (count != 0);
                    }
                } else {
                    gIcCnt2 = count;
                    gIcDimPal = palette;
                    if (clipY <= gIcY && gIcClipB >= gIcY && static_cast<i32>(X + count) > clipX
                        && gIcClipR >= X) {
                        i32 right = X + count;
                        u32 cn;
                        if (X >= clipX) {
                            cn = count;
                            if (gIcClipR < right)
                                cn = (gIcClipR - X) + 1;
                            savedDst = row + X;
                        } else {
                            i32 clipRight = gIcClipR;
                            gIcCnt2 = count;
                            if (right <= clipRight)
                                count = (count - clipX) + X;
                            else
                                count = clipW;
                            cn = count;
                            savedDst = row + clipX;
                        }
                        gIcCnt2 = cn;
                        gIcDimDst = savedDst;
                        gIcDimPal = palette;
                        gIcCnt = 0;
                        i32 dimCount = cn;
                        if (dimCount > 0) {
                            gIcCnt = dimCount;
                            do {
                                i32 b = *savedDst;
                                savedDst = savedDst + 1;
                                gIcDimDst = savedDst;
                                cn--;
                                gIcDimPal = palette;
                                savedDst[-1] = palette[b];
                            } while (cn != 0);
                        }
                    }
                }
            }
            X = X + gIcDimLen;
            continue;
        }

        gIcX = X;
        gIcRun = command;
        if (command != 0) {
            u32 count = command;
            if (clip == ICON_DRAW_NO_CLIP) {
                memcpy(row + X, gIcSrc, count);
            } else if (!(clipY > gIcY || gIcClipB < gIcY
                         || clipX >= X + static_cast<i32>(count) || gIcClipR < X)) {
                if (clipX <= X) {
                    if (gIcClipR >= X + static_cast<i32>(count)) {
                        memcpy(row + X, gIcSrc, count);
                    } else {
                        count = gIcClipR - X + 1;
                        memcpy(row + X, gIcSrc, count);
                    }
                } else {
                    if (gIcClipR >= X + static_cast<i32>(count)) {
                        count -= clipX - X;
                    } else {
                        count = clipW;
                    }
                    memcpy(row + clipX, gIcSrc + clipX - X, count);
                }
            }

            X += command;
            gIcRun = command;
            gIcSrc += command;
        } else {
            X = gIcX0;
            gIcY++;
            row += gIcPitch;
        }
    }
}
