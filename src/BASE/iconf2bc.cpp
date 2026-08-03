#include <va.h>
#include <BASE/iconf2bc.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
#include <SOURCE/KB.h>

static i32 s_clipB;
static u32 s_loopCount;
static i32 s_y;
static i32 s_x;
static u8* s_dimDst;
static i32 s_left;
static i32 s_right;
static u32 s_run;
static u8* s_dimPal;
static u8* s_row;
static u8* s_dst;
static i32 s_srcSkip;
static u8* s_src;
static IconEntry* s_entry;
static u32 s_spanCount;
static u8 s_color;
static u32 s_dimLen;
static i32 s_clipR;

static inline i32 FlipColorRowVisible(i32 clipTop, i32 currentY, i32 clipBottom) {
    return clipTop <= currentY && currentY <= clipBottom;
}

static inline u8 FlipColorDimValue(u8* dst, u8* palette) {
    return palette[*dst];
}

VA(0x004d4960, 0x8bb)
void FlipIconToBitmapColorTable(
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
    i32 color,
    u8* colorTable
) {
    IconEntry* entries = srcIcon->Entries();
    u8* data = srcIcon->m_data;
    i32 w = entries[frame].w;
    i32 x0 = x - entries[frame].x;
    x0 = x0 - w;
    s_entry = &entries[frame];
    s_src = data + entries[frame].srcOffset;
    u8* src = s_src;
    i32 pitch;
    x0++;
    s_left = x0;
    w = w + x0;
    i32 X = (s_right = w - 1);
    i32 Y = y + s_entry->y;
    s_y = Y;
    if (clip != ICON_DRAW_NO_CLIP) {
        i32 currentY = s_y;
        if (x0 < clipX || clipW + clipX < x0 + s_entry->w || currentY < clipY
            || clipY + clipH < s_entry->h + currentY) {
            clip = ICON_DRAW_CLIP;
            s_clipR = clipX + clipW - 1;
            s_clipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
    pitch = dest->m_width;
    s_row = dest->m_pixels + s_y * pitch;
    u8* dp;
    i32 cmd;
    for (;;) {
        cmd = *src++;
        if (static_cast<i8>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                s_run = cmd;
                i32 n = cmd & ICON_RLE_COMMAND_RUN_MASK;
                s_x = X;
                s_src = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            s_run = cmd;
            u32 count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            i32 flags = 0;
            if (count != 0) {
                if (cmd == ICON_RLE_LONG_SOLID_COMMAND)
                    count = *src++;
                s_color = colorTable[*src++];
                goto do_fill;
            }
            flags = *src++;
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0)
                count = *src++;
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
                memset((s_row - count) + 1 + X, s_color, count);
            } else {
                i32 left;
                if (FlipColorRowVisible(clipY, s_y, s_clipB)
                    && (left = (X - count) + 1, clipX <= left)
                    && X <= s_clipR) {
                    if (clipX <= left) {
                        memset((s_row - count) + 1 + X, s_color, count);
                    } else {
                        memset(s_row + clipX, s_color, (X - clipX) + 1);
                    }
                }
            }
            X = X - count;
            s_run = count;
            continue;
        do_dim:
            s_run = flags;
            s_spanCount = count;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                u8* palette =
                    &uDimPal[0][0][0]
                    + (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                s_dimPal = palette;
                if (clip == ICON_DRAW_NO_CLIP) {
                    dp = (s_row - count) + 1 + X;
                    s_loopCount = 0;
                    i32 dimCount = count;
                    s_dimDst = dp;
                    if (dimCount > 0) {
                        s_loopCount = dimCount;
                        do {
                            u8* dimPalette = s_dimPal;
                            u8 mapped = FlipColorDimValue(dp, dimPalette);
                            dp++;
                            count--;
                            s_dimDst = dp;
                            dp[-1] = mapped;
                        } while (count != 0);
                    }
                } else {
                    s_spanCount = count;
                    if (FlipColorRowVisible(clipY, s_y, s_clipB)
                        && clipX <= static_cast<i32>((X - count) + 1) && X <= s_clipR) {
                        i32 left = (X - count) + 1;
                        if (clipX <= left) {
                            dp = (s_row - count) + 1 + X;
                        } else {
                            count = (X - clipX) + 1;
                            dp = s_row + clipX;
                        }
                        s_spanCount = count;
                        s_dimDst = dp;
                        i32 dimCount = count;
                        s_loopCount = 0;
                        if (dimCount > 0) {
                            s_loopCount = dimCount;
                            do {
                                u8* dimPalette = s_dimPal;
                                u8 mapped = FlipColorDimValue(dp, dimPalette);
                                dp++;
                                count--;
                                s_dimDst = dp;
                                dp[-1] = mapped;
                            } while (count != 0);
                        }
                    }
                }
            }
            X = X - s_dimLen;
            continue;
        }
        s_run = cmd;
        s_x = X;
        if (cmd != 0) {
            if (clip == ICON_DRAW_NO_CLIP) {
                s_loopCount = 0;
                u8* dst = s_row + X;
                s_dst = dst;
                if (cmd > 0) {
                    s_loopCount = cmd;
                    i32 k = cmd;
                    do {
                        i32 c = *src++;
                        dst--;
                        s_dst = dst;
                        dst[1] = colorTable[c];
                        k--;
                    } while (k != 0);
                }
            } else {
                if (FlipColorRowVisible(clipY, s_y, s_clipB)) {
                    i32 left = (X - cmd) + 1;
                    if (left <= s_clipR && clipX <= X) {
                        u32 cn;
                        u8* selectedDst;
                        i32 skip;
                        if (X <= s_clipR) {
                            selectedDst = s_row + X;
                            s_dst = selectedDst;
                            if (clipX <= left) {
                                s_srcSkip = 0;
                                cn = cmd;
                            } else {
                                cn = (X - clipX) + 1;
                                skip = cmd - cn;
                                s_srcSkip = skip;
                            }
                        } else {
                            i32 right = s_clipR;
                            src = src + (X - right);
                            selectedDst = s_row + right;
                            s_dst = selectedDst;
                            if (clipX <= (X - cmd)) {
                                s_srcSkip = 0;
                                cn = (cmd - X) + s_clipR;
                            } else {
                                cn = clipW;
                                skip = s_clipR + ((cmd - X) - clipW);
                                s_srcSkip = skip;
                            }
                        }
                        i32 copyCount = cn;
                        s_spanCount = cn;
                        s_loopCount = 0;
                        if (copyCount > 0) {
                            s_loopCount = copyCount;
                            do {
                                u8* dst = s_dst;
                                i32 c = *src++;
                                dst--;
                                cn--;
                                u8 mapped = colorTable[c];
                                s_dst = dst;
                                dst[1] = mapped;
                            } while (cn != 0);
                        }
                        src = src + s_srcSkip;
                    } else {
                        src = src + cmd;
                    }
                } else {
                    src = src + cmd;
                }
            }
            X = X - cmd;
            s_run = cmd;
            continue;
        }
        X = s_right;
        s_y = s_y + 1;
        s_row = s_row + pitch;
    }
}
