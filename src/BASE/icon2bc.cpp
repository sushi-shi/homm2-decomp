#include <Ints.h>
#include <BASE/icon2bc.h>
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
static u8* s_literalSource;
static u8* s_src;
static IconEntry* s_entry;
static u32 s_spanCount;
static u8 s_color;
static u32 s_dimLen;
static i32 s_clipR;

static inline u8* ColorTableDimPixel(u8* dst, u8* palette, i32 dimGate) {
    if (dimGate != 0)
        *dst = palette[*dst];
    return dst + 1;
}

static inline u8* ColorTableOutsideSource(u8* src, i32 currentX, i32 clipX) {
    return src + (clipX - currentX);
}

void IconToBitmapColorTable(
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
    i32 color,
    u8* colorTable,
    i32 dimGate
) {
    IconEntry* entries = srcIcon->Entries();
    u8* data = srcIcon->m_data;
    i32 entryX = entries[frame].x;
    s_entry = &entries[frame];
    s_src = data + entries[frame].srcOffset;
    s_left = x + entryX;
    s_pitch = dest->m_width;
    s_y = y + s_entry->y;
    i32 X = s_left;
    if (clip != ICON_DRAW_NO_CLIP) {
        i32 currentY = s_y;
        if (X < clipX || clipW + clipX < s_entry->w + s_left || currentY < clipY
            || clipY + clipH < s_entry->h + currentY) {
            clip = ICON_DRAW_CLIP;
            s_clipR = clipX + clipW - 1;
            s_clipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
    u8* row = dest->m_pixels + s_pitch * s_y;
    u8* savedDst = s_dst;
    i32 cmd;
    for (;;) {
        s_src = s_src + 1;
        cmd = s_src[-1];
        if (static_cast<i8>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {

                s_x = X;
                s_row = row;
                s_dst = savedDst;
                s_run = cmd;
                if ((cmd & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                X = X + (cmd & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }
            s_run = cmd;
            u32 count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            i32 flags = 0;
            if (count != 0) {

                if (cmd == ICON_RLE_LONG_SOLID_COMMAND) {
                    count = *s_src++;
                }
                s_color = colorTable[*s_src++];
                goto do_fill;
            }

            flags = *s_src++;
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0) {
                count = *s_src++;
            }
            s_dimLen = count;
            if (color != 0) {
                s_run = flags;
                if (flags & ICON_RLE_DIM_RECOLOR_FLAG) {
                    s_spanCount = count;
                    s_color = static_cast<u8>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == ICON_DRAW_NO_CLIP) {
                memset(row + X, s_color, count);
            } else if (clipY <= s_y && s_y <= s_clipB
                       && static_cast<i32>(X + count) > clipX && s_clipR >= X) {
                i32 fillRight = X + count;
                if (clipX <= X) {
                    if (s_clipR >= fillRight) {
                        memset(row + X, s_color, count);
                    } else {
                        memset(row + X, s_color, (s_clipR - X) + 1);
                    }
                } else {
                    if (s_clipR >= fillRight) {
                        memset(row + clipX, s_color, (count - clipX) + X);
                    } else {
                        memset(row + clipX, s_color, clipW);
                    }
                }
            }
            X = X + count;
            s_run = count;
            continue;
        do_dim:
            s_spanCount = count;
            s_run = flags;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                u8* palette =
                    reinterpret_cast<u8*>(uDimPal)
                    + (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                s_dst = savedDst;
                s_dimPal = palette;
                if (clip == ICON_DRAW_NO_CLIP) {
                    savedDst = row + X;
                    s_loopCount = 0;
                    i32 dimCount = count;
                    if (dimCount > 0) {
                        s_loopCount = dimCount;
                        do {
                            savedDst = ColorTableDimPixel(savedDst, palette, dimGate);
                            s_dimPal = palette;
                            count--;
                        } while (count != 0);
                    }
                } else {
                    s_spanCount = count;
                    if (clipY <= s_y && s_clipB >= s_y && static_cast<i32>(X + count) > clipX
                        && (s_dst = savedDst, s_clipR >= X)) {
                        i32 right = X + count;
                        u32 cn;
                        if (X >= clipX) {
                            i32 clipRight = s_clipR;
                            cn = count;
                            if (clipRight < right)
                                cn = (clipRight - X) + 1;
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
                        s_dimPal = palette;
                        s_spanCount = cn;
                        s_loopCount = 0;
                        i32 dimCount = cn;
                        if (dimCount > 0) {
                            s_loopCount = dimCount;
                            do {
                                savedDst = ColorTableDimPixel(savedDst, palette, dimGate);
                                s_dimPal = palette;
                                cn--;
                            } while (cn != 0);
                        }
                    }
                }
            }
            X = X + s_dimLen;
            continue;
        }
        s_x = X;
        s_run = cmd;
        if (cmd != 0) {
            u32 cnt;
            do {
                s_dst = savedDst;
                s_literalSource = s_src;
                if (clip == ICON_DRAW_NO_CLIP) {
                    savedDst = row + X;
                    cnt = cmd;
                } else {
                    if (s_y < clipY || s_clipB < s_y) {
                        cnt = 0;
                        break;
                    }
                    i32 right = X + cmd;
                    if (right <= clipX || (s_dst = savedDst, s_clipR < X)) {
                        cnt = 0;
                        break;
                    }
                    i32 clipRight = s_clipR;
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
                        s_literalSource = ColorTableOutsideSource(s_src, X, clipX);
                    }
                }
            } while (0);
            s_spanCount = cnt;
            if (cnt != 0) {
                s_loopCount = 0;
                if (static_cast<i32>(cnt) > 0) {
                    s_loopCount = cnt;
                    do {
                        i32 b = *s_literalSource;
                        savedDst = savedDst + 1;
                        savedDst[-1] = colorTable[b];
                        s_literalSource = s_literalSource + 1;
                    } while (--cnt != 0);
                }
            }
            X = X + cmd;
            s_src = s_src + cmd;
            s_run = cmd;
            continue;
        }
        X = s_left;
        s_y = s_y + 1;
        row = row + s_pitch;
    }
}
