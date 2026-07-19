#include <va.h>
#include <BASE/Iconf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconRle.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/IconEntry.h>
#include <string.h>
DATA(0x00534c78) static IconEntry* gFlipEntry;
DATA(0x00534c68) static i32 gFlipX0;
DATA(0x00534ca4) static i32 gFlipXEnd;
DATA(0x00534c74) static i32 gFlipY;
DATA(0x00534c7c) static i32 gFlipX;
DATA(0x00534c80) static u8* gFlipSrc;
DATA(0x00534c9c) static i32 gFlipClipR;
DATA(0x00534c8c) static i32 gFlipClipB;
DATA(0x00534c90) static i32 gFlipRow;
DATA(0x00534c64) static u32 gFlipRun;
DATA(0x00534c98) static u8 gFlipColor;
DATA(0x00534c94) static u32 gFlipCnt2;
DATA(0x00534c70) static u32 gFlipDimLen;
DATA(0x00534c84) static u8* gFlipDimPal;
DATA(0x00534c6c) static u32 gFlipCnt;
DATA(0x00534c88) static u8* gFlipDimDst;
DATA(0x00534ca0) static u8* gFlipDst;
DATA(0x00534c60) static i32 gFlipSkip;

VA(0x004d1ba0, 0x4f1)
void FlipIconToBitmap(
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
    IconEntry* entries = srcIcon->Entries();
    u8* src = reinterpret_cast<u8*>(entries);
    i32 x0 = x;
    i32 w;
    IconEntry* entry = &entries[frame];
    w = entry->w;
    x0 = x0 - entry->x;
    src += entry->srcOffset;
    y += entry->y;
    x0 = x0 - w;
    gFlipEntry = entry;
    x0++;
    gFlipX0 = x0;
    i32 X = w + x0 - 1;
    gFlipXEnd = X;
    gFlipY = y;
    if (clip != 0) {
        if (gFlipX0 < clipX || clipW + clipX < gFlipX0 + w
            || (memcpy(&clip, &gFlipY, sizeof(clip)), clip < clipY)
            || clipY + clipH < entry->h + clip) {
            clip = 1;
            gFlipClipR = clipX + clipW - 1;
            gFlipClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    i16 pitch = dest->m_width;
    gFlipRow = gFlipY * pitch + reinterpret_cast<i32>(dest->m_pixels);
    for (;;) {
        i32 cmd = *src++;
        if (static_cast<i8>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                gFlipRun = cmd;
                i32 n = cmd & ICON_RLE_COMMAND_RUN_MASK;
                gFlipX = X;
                gFlipSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            gFlipRun = cmd;
            u32 count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            i32 flags = 0;
            if (count != 0) {
                // 0xc1 - 0xFF : solid colour run
                if (cmd == ICON_RLE_LONG_SOLID_COMMAND)
                    count = *src++;
                gFlipColor = *src++;
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = *src++;
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0)
                count = *src++;
            gFlipCnt2 = count;
            if (color != 0) {
                gFlipRun = flags;
                if (flags & ICON_RLE_DIM_RECOLOR_FLAG) {
                    gFlipDimLen = count;
                    gFlipColor = static_cast<u8>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(reinterpret_cast<u8*>((gFlipRow - count) + 1 + X), gFlipColor, count);
            } else {
                i32 currentY = gFlipY;
                i32 left;
                if (currentY >= clipY && currentY <= gFlipClipB
                    && (left = (X - count) + 1, clipX <= left) && X <= gFlipClipR) {
                    if (clipX <= left) {
                        memset(
                            reinterpret_cast<u8*>((gFlipRow - count) + 1 + X),
                            gFlipColor,
                            count
                        );
                    } else {
                        memset(
                            reinterpret_cast<u8*>(gFlipRow + clipX),
                            gFlipColor,
                            (X - clipX) + 1
                        );
                    }
                }
            }
            X = X - count;
            gFlipRun = count;
            continue;
        do_dim:
            gFlipRun = flags;
            gFlipDimLen = count;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                u8* palette =
                    reinterpret_cast<u8*>(uDimPal)
                    + (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                gFlipDimPal = palette;
                if (clip == 0) {
                    u8* dp = reinterpret_cast<u8*>((gFlipRow - count) + 1 + X);
                    gFlipCnt = 0;
                    i32 dimCount = count;
                    gFlipDimDst = dp;
                    if (dimCount > 0) {
                        gFlipCnt = dimCount;
                        do {
                            u8* dimPalette = gFlipDimPal;
                            i32 px = *dp++;
                            count--;
                            gFlipDimDst = dp;
                            dp[-1] = dimPalette[px];
                        } while (count != 0);
                    }
                } else {
                    const i32 currentY = gFlipY;
                    gFlipDimLen = count;
                    if (clipY <= currentY && currentY <= gFlipClipB
                        && clipX <= static_cast<i32>((X - count) + 1) && X <= gFlipClipR) {
                        i32 left = (X - count) + 1;
                        u8* dp;
                        if (clipX <= left) {
                            dp = reinterpret_cast<u8*>((gFlipRow - count) + 1 + X);
                        } else {
                            count = (X - clipX) + 1;
                            dp = reinterpret_cast<u8*>(gFlipRow + clipX);
                        }
                        i32 dimCount = count;
                        gFlipDimLen = count;
                        gFlipDimDst = dp;
                        gFlipCnt = 0;
                        if (dimCount > 0) {
                            gFlipCnt = dimCount;
                            do {
                                u8* dimPalette = gFlipDimPal;
                                i32 px = *dp++;
                                count--;
                                gFlipDimDst = dp;
                                dp[-1] = dimPalette[px];
                            } while (count != 0);
                        }
                    }
                }
            }
            X = X - gFlipCnt2;
            continue;
        }
        gFlipRun = cmd;
        gFlipX = X;
        if (cmd != 0) {
            if (clip == 0) {
                gFlipCnt = 0;
                u8* dst = reinterpret_cast<u8*>(gFlipRow + X);
                gFlipDst = dst;
                if (cmd > 0) {
                    gFlipCnt = cmd;
                    i32 k = cmd;
                    do {
                        u8 c = *src++;
                        *dst-- = c;
                        gFlipDst = dst;
                        k--;
                    } while (k != 0);
                }
            } else {
                i32 currentY = gFlipY;
                if (currentY >= clipY && currentY <= gFlipClipB) {
                    i32 left = (X - cmd) + 1;
                    if (left <= gFlipClipR && clipX <= X) {
                        u32 cn;
                        i32 skip;
                        if (X <= gFlipClipR) {
                            gFlipDst = reinterpret_cast<u8*>(gFlipRow + X);
                            if (clipX <= left) {
                                gFlipSkip = 0;
                                cn = cmd;
                            } else {
                                cn = (X - clipX) + 1;
                                skip = cmd - cn;
                                goto set_skip;
                            }
                        } else {
                            cn = gFlipClipR;
                            src = src + (X - cn);
                            gFlipDst = reinterpret_cast<u8*>(gFlipRow + cn);
                            if (clipX <= (X - cmd)) {
                                gFlipSkip = 0;
                                cn = (cmd - X) + cn;
                            } else {
                                cn = clipW;
                                skip = cmd;
                                skip = skip - X;
                                skip = skip - clipW;
                                skip = gFlipClipR + skip;
                                goto set_skip;
                            }
                        }
                        goto skip_set;
                    set_skip:
                        gFlipSkip = skip;
                    skip_set:
                        i32 copyCount = cn;
                        gFlipDimLen = cn;
                        gFlipCnt = 0;
                        if (copyCount > 0) {
                            gFlipCnt = copyCount;
                            do {
                                u8 c = *src++;
                                (gFlipDst = gFlipDst - 1)[1] = c;
                                cn--;
                            } while (cn != 0);
                        }
                        src = src + gFlipSkip;
                    } else {
                        src = src + cmd;
                    }
                } else {
                    src = src + cmd;
                }
            }
            X = X - cmd;
            gFlipRun = cmd;
            continue;
        }
        X = gFlipXEnd;
        gFlipY = gFlipY + 1;
        gFlipRow = gFlipRow + pitch;
    }
}
