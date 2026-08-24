#include <Ints.h>
#include <BASE/Icon2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

static i32 s_clipB;
static u32 s_loopCount;
static i32 s_pitch;
static i32 s_y;
static i32 s_x;
static i32 s_left;
static u32 s_run;
static u8* s_dimPal;
static u8* s_row;
static u8* s_dst;
static u8* s_src;
static IconEntry* s_entry;
static u32 s_spanCount;
static u8 s_color;
static u32 s_dimLen;
static i32 s_clipR;

static inline i32 IconRowVisible(i32 currentY, i32 clipBottom, i32 clipTop) {
    return clipTop <= currentY && currentY <= clipBottom;
}

void IconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 color
) {
    IconEntry* entries = srcIcon->Entries();
    u8* data = srcIcon->m_data;
    u8* savedDst;
    s_entry = &entries[frame];
    s_src = data + entries[frame].srcOffset;
    s_left = x + s_entry->x;
    s_pitch = dest->m_width;
    s_y = y + s_entry->y;
    i32 X = s_left;

    if (clip != ICON_DRAW_NO_CLIP) {
        if (s_left < clipX || clipW + clipX < s_entry->w + s_left || s_y < clipY
            || clipY + clipH < s_entry->h + s_y) {
            clip = ICON_DRAW_CLIP;
            s_clipR = clipX + clipW - 1;
            s_clipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }

    u8* row = dest->m_pixels + s_pitch * s_y;
    i32 command;

    for (;;) {
        command = ReadIconRleByte(s_src);
        if (static_cast<i8>(command) < 0) {
            if ((command & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                s_x = X;
                s_row = row;
                s_run = command;
                if ((command & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                X = X + (command & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }

            s_run = command;
            u32 count = command & ICON_RLE_COMMAND_RUN_MASK;
            i32 flags = 0;
            if (count != 0) {
                if (command == ICON_RLE_LONG_SOLID_COMMAND)
                    count = ReadIconRleByte(s_src);
                s_color = ReadIconRleByte(s_src);
                goto fill_run;
            }

            flags = ReadIconRleByte(s_src);
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0)
                count = ReadIconRleByte(s_src);
            s_dimLen = count;

            if (color != 0) {
                s_run = flags;
                if ((flags & ICON_RLE_DIM_RECOLOR_FLAG) != 0) {
                    s_spanCount = count;
                    s_color = static_cast<u8>(color);
                    goto fill_run;
                }
            }
            goto dim_run;

        fill_run:
            if (clip == ICON_DRAW_NO_CLIP) {
                memset(row + X, s_color, count);
            } else {
                i32 right;
                i32 fillLen;
                if (IconRowVisible(s_y, s_clipB, clipY)
                    && (right = X + count, clipX < right)
                    && s_clipR >= X) {
                    if (X >= clipX) {
                        if (s_clipR >= right) {
                            fillLen = count;
                            memset(row + X, s_color, fillLen);
                        } else {
                            fillLen = (s_clipR - X) + 1;
                            memset(row + X, s_color, fillLen);
                        }
                    } else {
                        if (s_clipR >= right)
                            fillLen = (count - clipX) + X;
                        else
                            fillLen = clipW;
                        memset(row + clipX, s_color, fillLen);
                    }
                }
            }
            X = X + count;
            s_run = count;
            continue;

        dim_run:
            s_spanCount = count;
            s_run = flags;
            if ((flags & ICON_RLE_DIM_APPLY_FLAG) != 0) {
                u8* palette =
                    reinterpret_cast<u8*>(uDimPal)
                    + (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                if (clip == ICON_DRAW_NO_CLIP) {
                    savedDst = row + X;
                    s_dimPal = palette;
                    s_dst = savedDst;
                    s_loopCount = 0;
                    i32 dimCount = count;
                    if (dimCount > 0) {
                        s_loopCount = dimCount;
                        do {
                            i32 b = *savedDst;
                            savedDst = savedDst + 1;
                            s_dst = savedDst;
                            count--;
                            s_dimPal = palette;
                            savedDst[-1] = palette[b];
                        } while (count != 0);
                    }
                } else {
                    s_spanCount = count;
                    s_dimPal = palette;
                    if (clipY <= s_y && s_clipB >= s_y && static_cast<i32>(X + count) > clipX
                        && s_clipR >= X) {
                        i32 right = X + count;
                        u32 cn;
                        if (X >= clipX) {
                            cn = count;
                            if (s_clipR < right)
                                cn = (s_clipR - X) + 1;
                            savedDst = row + X;
                        } else {
                            i32 clipRight = s_clipR;
                            s_spanCount = count;
                            if (right <= clipRight)
                                count = (count - clipX) + X;
                            else
                                count = clipW;
                            cn = count;
                            savedDst = row + clipX;
                        }
                        s_spanCount = cn;
                        s_dst = savedDst;
                        s_dimPal = palette;
                        s_loopCount = 0;
                        i32 dimCount = cn;
                        if (dimCount > 0) {
                            s_loopCount = dimCount;
                            do {
                                i32 b = *savedDst;
                                savedDst = savedDst + 1;
                                s_dst = savedDst;
                                cn--;
                                s_dimPal = palette;
                                savedDst[-1] = palette[b];
                            } while (cn != 0);
                        }
                    }
                }
            }
            X = X + s_dimLen;
            continue;
        }

        s_x = X;
        s_run = command;
        if (command != 0) {
            u32 count = command;
            if (clip == ICON_DRAW_NO_CLIP) {
                memcpy(row + X, s_src, count);
            } else if (!(clipY > s_y || s_clipB < s_y
                         || clipX >= X + static_cast<i32>(count) || s_clipR < X)) {
                if (clipX <= X) {
                    if (s_clipR >= X + static_cast<i32>(count)) {
                        memcpy(row + X, s_src, count);
                    } else {
                        count = s_clipR - X + 1;
                        memcpy(row + X, s_src, count);
                    }
                } else {
                    if (s_clipR >= X + static_cast<i32>(count)) {
                        count -= clipX - X;
                    } else {
                        count = clipW;
                    }
                    memcpy(row + clipX, s_src + clipX - X, count);
                }
            }

            X += command;
            s_run = command;
            s_src += command;
        } else {
            X = s_left;
            s_y++;
            row += s_pitch;
        }
    }
}
