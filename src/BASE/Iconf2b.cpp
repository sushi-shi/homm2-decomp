#include <va.h>
#include <BASE/Iconf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconRle.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/IconEntry.h>
#include <string.h>

DATA(0x00534c8c) static i32 s_clipB;
DATA(0x00534c6c) static u32 s_loopCount;
DATA(0x00534c74) static i32 s_y;
DATA(0x00534c7c) static i32 s_x;
DATA(0x00534c88) static u8* s_dimDst;
DATA(0x00534c68) static i32 s_left;
DATA(0x00534ca4) static i32 s_right;
DATA(0x00534c64) static u32 s_run;
DATA(0x00534c84) static u8* s_dimPal;
DATA(0x00534c90) static u8* s_row;
DATA(0x00534ca0) static u8* s_dst;
DATA(0x00534c60) static i32 s_srcSkip;
DATA(0x00534c80) static u8* s_src;
DATA(0x00534c78) static IconEntry* s_entry;
DATA(0x00534c94) static u32 s_spanCount;
DATA(0x00534c98) static u8 s_color;
DATA(0x00534c70) static u32 s_dimLen;
DATA(0x00534c9c) static i32 s_clipR;

VA(0x004d1ba0, 0x4f1)
void FlipIconToBitmap(
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
    IconEntry* entries = srcIcon->Entries();
    u8* src = srcIcon->m_data;
    i32 width = entries[frame].w;
    IconEntry* entry = &entries[frame];
    i32 entryX = entry->x;
    i32 x0 = x - entryX;
    src += entry->srcOffset;
    i32 entryY = entry->y;
    x0 = x0 - width;
    s_entry = entry;
    x0++;
    s_left = x0;
    i32 currentX = (width - 1) + x0;
    s_y = y + entryY;
    s_right = currentX;
    if (clip != ICON_DRAW_NO_CLIP) {
        if (s_left < clipX || clipW + clipX < s_left + width || s_y < clipY
            || clipY + clipH < entry->h + s_y) {
            clip = ICON_DRAW_CLIP;
            s_clipR = clipX + clipW - 1;
            s_clipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
    i16 pitch = dest->m_width;
    s_row = dest->m_pixels + s_y * pitch;
    for (;;) {
        i32 command = *src++;
        if (static_cast<i8>(command) < 0) {
            if ((command & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                s_run = command;
                command &= ICON_RLE_COMMAND_RUN_MASK;
                s_x = currentX;
                s_src = src;
                if (command == 0)
                    return;
                currentX -= command;
                continue;
            }
            s_run = command;
            u32 count = command & ICON_RLE_COMMAND_RUN_MASK;
            if (count != 0) {
                // 0xc1 - 0xFF : solid colour run
                if (command == ICON_RLE_LONG_SOLID_COMMAND)
                    count = *src++;
                s_color = *src++;
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            command = *src++;
            count = command & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0)
                count = *src++;
            s_spanCount = count;
            if (color != 0) {
                s_run = command;
                if (command & ICON_RLE_DIM_RECOLOR_FLAG) {
                    s_dimLen = count;
                    s_color = static_cast<u8>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == ICON_DRAW_NO_CLIP) {
                memset((s_row - count) + 1 + currentX, s_color, count);
            } else {
                if (clipY <= s_y && s_y <= s_clipB
                    && clipX <= static_cast<i32>((currentX - count) + 1)
                    && currentX <= s_clipR) {
                    if (clipX <= static_cast<i32>((currentX - count) + 1)) {
                        memset(
                            (s_row - count) + 1 + currentX,
                            s_color,
                            count
                        );
                    } else {
                        memset(
                            s_row + clipX,
                            s_color,
                            (currentX - clipX) + 1
                        );
                    }
                }
            }
            currentX -= count;
            s_run = count;
            continue;
        do_dim:
            s_run = command;
            s_dimLen = count;
            if (command & ICON_RLE_DIM_APPLY_FLAG) {
                u8* palette = reinterpret_cast<u8*>(uDimPal)
                    + (command & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                s_dimPal = palette;
                u8* dimDestination;
                if (clip == ICON_DRAW_NO_CLIP) {
                    dimDestination = (s_row - count) + 1 + currentX;
                    s_loopCount = 0;
                    i32 dimCount = count;
                    s_dimDst = dimDestination;
                    if (static_cast<i32>(count) > 0) {
                        s_loopCount = count;
                        do {
                            *dimDestination = s_dimPal[*dimDestination];
                            dimDestination++;
                            s_dimDst = dimDestination;
                        } while (--dimCount != 0);
                    }
                } else {
                    s_dimLen = count;
                    if (clipY <= s_y && s_y <= s_clipB
                        && clipX <= static_cast<i32>((currentX - count) + 1)
                        && currentX <= s_clipR) {
                        if (clipX <= static_cast<i32>((currentX - count) + 1)) {
                            dimDestination = (s_row - count) + 1 + currentX;
                        } else {
                            count = (currentX - clipX) + 1;
                            dimDestination = s_row + clipX;
                        }
                        i32 dimCount = count;
                        s_dimLen = count;
                        s_dimDst = dimDestination;
                        s_loopCount = 0;
                        if (static_cast<i32>(count) > 0) {
                            s_loopCount = count;
                            do {
                                *dimDestination = s_dimPal[*dimDestination];
                                dimDestination++;
                                s_dimDst = dimDestination;
                            } while (--dimCount != 0);
                        }
                    }
                }
            }
            currentX -= s_spanCount;
            continue;
        }
        s_run = command;
        s_x = currentX;
        if (command != 0) {
            u32 literalCount;
            if (clip == ICON_DRAW_NO_CLIP) {
                s_loopCount = 0;
                u8* destination = s_row + currentX;
                s_dst = destination;
                if (command > 0) {
                    u32 copyCount = command;
                    s_loopCount = copyCount;
                    do {
                        *destination-- = *src++;
                        s_dst = destination;
                    } while (--copyCount != 0);
                }
            } else {
                if (clipY <= s_y && s_y <= s_clipB) {
                    if ((currentX - command) + 1 <= s_clipR && clipX <= currentX) {
                        u32 pendingSkip;
                        do {
                            if (currentX <= s_clipR) {
                                s_dst = s_row + currentX;
                                if (clipX <= (currentX - command) + 1) {
                                    literalCount = command;
                                    s_srcSkip = 0;
                                    break;
                                }
                                literalCount = (currentX - clipX) + 1;
                                pendingSkip = command - literalCount;
                                s_srcSkip = pendingSkip;
                            } else {
                                literalCount = s_clipR;
                                src += currentX - literalCount;
                                s_dst = s_row + literalCount;
                                if (clipX <= currentX - command) {
                                    s_srcSkip = 0;
                                    literalCount += command - currentX;
                                    break;
                                }
                                pendingSkip = command;
                                pendingSkip -= currentX;
                                pendingSkip -= clipW;
                                pendingSkip += s_clipR;
                                literalCount = clipW;
                                s_srcSkip = pendingSkip;
                            }
                        } while (0);
                        i32 copyCount = literalCount;
                        s_dimLen = literalCount;
                        s_loopCount = 0;
                        if (static_cast<i32>(literalCount) > 0) {
                            s_loopCount = literalCount;
                            do {
                                u8 literalPixel = *src++;
                                s_dst--;
                                s_dst[1] = literalPixel;
                            } while (--copyCount != 0);
                        }
                        src += s_srcSkip;
                    } else {
                        src += command;
                    }
                } else {
                    src += command;
                }
            }
            currentX -= command;
            s_run = command;
            continue;
        }
        currentX = s_right;
        s_y++;
        s_row += pitch;
    }
}
