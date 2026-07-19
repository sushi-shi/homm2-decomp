#include <va.h>
#include <BASE/icon2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
DATA(0x00534ca8) static i32 gCTPitch;
DATA(0x00534cac) static u32 gCTCnt;
DATA(0x00534cb0) static i32 gCTX;
DATA(0x00534cb4) static i32 gCTY;
DATA(0x00534cb8) static IconEntry* gCTEntry;
DATA(0x00534cbc) static u8* gCTRow;
DATA(0x00534cc0) static u8* gCTDimPal;
DATA(0x00534cc4) static i32 gCTClipB;
DATA(0x00534cc8) static i32 gCTClipR;
DATA(0x00534ccc) static u8* gCTSrcCopy;
DATA(0x00534cd0) static u32 gCTDimLen;
DATA(0x00534cd4) static u8* gCTDst;
DATA(0x00534cd8) static u8* gCTSrc;
DATA(0x00534cdc) static u32 gCTCnt2;
DATA(0x00534ce0) static i32 gCTX0;
DATA(0x00534ce4) static u8 gCTColor;
DATA(0x00534ce8) static u32 gCTRun;

VA(0x004d32a0, 0x5af)
void IconToBitmapColorTable(
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
    u8* colorTable,
    i32 dimGate
) {
    u8* data = srcIcon->m_data;
    IconEntry* entry = &srcIcon->Entries()[frame];
    i32 entryX = entry->x;
    i32 sourceOffset = entry->srcOffset;
    u8* savedDst;
    gCTEntry = entry;
    gCTSrc = data + sourceOffset;
    i32 X = x + entryX;
    gCTX0 = X;
    gCTPitch = dest->m_width;
    gCTY = entry->y + y;
    if (clip != 0) {
        i32 currentY = gCTY;
        if (X < clipX || clipW + clipX < entry->w + gCTX0 || currentY < clipY
            || clipY + clipH < entry->h + currentY) {
            clip = 1;
            gCTClipR = clipX + clipW - 1;
            gCTClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    u8* row = dest->m_pixels + gCTPitch * gCTY;
    savedDst = gCTDst;
    i32 cmd;
    for (;;) {
        cmd = ReadIconRleByte(gCTSrc);
        if (static_cast<i8>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                gCTX = X;
                gCTRow = row;
                gCTDst = savedDst;
                gCTRun = cmd;
                if ((cmd & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                X = X + (cmd & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }
            gCTRun = cmd;
            u32 count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            i32 flags = 0;
            if (count != 0) {
                // 0xc1 - 0xFF : solid colour run
                if (cmd == ICON_RLE_LONG_SOLID_COMMAND) {
                    count = ReadIconRleByte(gCTSrc);
                }
                gCTColor = colorTable[ReadIconRleByte(gCTSrc)];
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = ReadIconRleByte(gCTSrc);
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0) {
                count = ReadIconRleByte(gCTSrc);
            }
            gCTCnt2 = count;
            if (color != 0) {
                gCTRun = flags;
                if (flags & ICON_RLE_DIM_RECOLOR_FLAG) {
                    gCTCnt = count;
                    gCTColor = static_cast<u8>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(row + X, gCTColor, count);
            } else {
                i32 right;
                i32 fillLen;
                if (clipY <= gCTY && gCTClipB >= gCTY && (right = X + count, clipX < right)
                    && gCTClipR >= X) {
                    if (X >= clipX) {
                        if (gCTClipR >= right) {
                            fillLen = count;
                            memset(row + X, gCTColor, fillLen);
                        } else {
                            fillLen = (gCTClipR - X) + 1;
                            memset(row + X, gCTColor, fillLen);
                        }
                    } else {
                        if (gCTClipR >= right) {
                            fillLen = (count - clipX) + X;
                            memset(row + clipX, gCTColor, fillLen);
                        } else {
                            fillLen = clipW;
                            memset(row + clipX, gCTColor, fillLen);
                        }
                    }
                }
            }
            X = X + count;
            gCTRun = count;
            continue;
        do_dim:
            gCTCnt = count;
            gCTRun = flags;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                u8* palette =
                    reinterpret_cast<u8*>(uDimPal)
                    + (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                gCTDst = savedDst;
                gCTDimPal = palette;
                if (clip == 0) {
                    savedDst = row + X;
                    gCTDimLen = 0;
                    i32 dimCount = count;
                    if (dimCount > 0) {
                        gCTDimLen = dimCount;
                        do {
                            if (dimGate != 0)
                                *savedDst = palette[*savedDst];
                            savedDst = savedDst + 1;
                            gCTDimPal = palette;
                            count--;
                        } while (count != 0);
                    }
                } else {
                    gCTCnt = count;
                    if (clipY <= gCTY && gCTClipB >= gCTY && static_cast<i32>(X + count) > clipX
                        && (gCTDst = savedDst, gCTClipR >= X)) {
                        i32 right = X + count;
                        u32 cn;
                        if (X >= clipX) {
                            i32 clipRight = gCTClipR;
                            cn = count;
                            if (clipRight < right)
                                cn = (clipRight - X) + 1;
                            savedDst = row + X;
                        } else {
                            i32 clipRight = gCTClipR;
                            gCTCnt = count;
                            if (right <= clipRight)
                                count = (count - clipX) + X;
                            else
                                count = clipW;
                            cn = count;
                            savedDst = row + clipX;
                        }
                        gCTDimPal = palette;
                        gCTCnt = cn;
                        gCTDimLen = 0;
                        i32 dimCount = cn;
                        if (dimCount > 0) {
                            gCTDimLen = dimCount;
                            do {
                                if (dimGate != 0)
                                    *savedDst = palette[*savedDst];
                                savedDst = savedDst + 1;
                                gCTDimPal = palette;
                                cn--;
                            } while (cn != 0);
                        }
                    }
                }
            }
            X = X + gCTCnt2;
            continue;
        }
        gCTX = X;
        gCTRun = cmd;
        if (cmd != 0) {
            u32 cnt;
            do {
                gCTDst = savedDst;
                gCTSrcCopy = gCTSrc;
                if (clip == 0) {
                    savedDst = row + X;
                    cnt = cmd;
                } else {
                    if (gCTY < clipY || gCTClipB < gCTY) {
                        cnt = 0;
                        break;
                    }
                    i32 right = X + cmd;
                    if (right <= clipX || (gCTDst = savedDst, gCTClipR < X)) {
                        cnt = 0;
                        break;
                    }
                    i32 clipRight = gCTClipR;
                    if (clipX <= X) {
                        savedDst = row + X;
                        if (clipRight >= right)
                            cnt = cmd;
                        else
                            cnt = (clipRight - X) + 1;
                    } else {
                        if (clipRight >= right) {
                            savedDst = row + clipX;
                            cnt = (cmd - clipX) + X;
                        } else {
                            savedDst = row + clipX;
                            cnt = clipW;
                        }
                        gCTSrcCopy = gCTSrc + (clipX - X);
                    }
                }
            } while (0);
            gCTCnt = cnt;
            if (cnt != 0) {
                gCTDimLen = 0;
                if (static_cast<i32>(cnt) > 0) {
                    gCTDimLen = cnt;
                    do {
                        i32 b = *gCTSrcCopy;
                        savedDst = savedDst + 1;
                        savedDst[-1] = colorTable[b];
                        gCTSrcCopy = gCTSrcCopy + 1;
                    } while (--cnt != 0);
                }
            }
            X = X + cmd;
            gCTSrc = gCTSrc + cmd;
            gCTRun = cmd;
            continue;
        }
        X = gCTX0;
        gCTY = gCTY + 1;
        row = row + gCTPitch;
    }
}
