#ifndef HOMM2_BASE_ICONMACRO_H
#define HOMM2_BASE_ICONMACRO_H

#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>

/*
 * Decoder macro contract
 *
 * TUs call setup and handler macros only. Handler expansions share the names
 * X, row, command, count, flags, clip*, and the semantic s_* scratch bundle.
 * Expression, pixel-loop, and clipping tiers are implementation details used
 * only by the handler tier below.
 */

/* Tier 0: expressions. */

#define RLE_EXPR_DIM_PALETTE(flags)                                                                \
    (reinterpret_cast<u8*>(uDimPal)                                                                \
     + ((flags) & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE)

/* Tier 1: pixel walks. Direction names describe destination-address motion. */

#define RLE_PIXEL_ASCENDING_LOOP(destination, remaining, publishedDestination, pixelCode)          \
    publishedDestination = destination;                                                            \
    s_loopCount = 0;                                                                               \
    i32 loopCount = remaining;                                                                     \
    if (loopCount > 0) {                                                                           \
        s_loopCount = loopCount;                                                                   \
        do {                                                                                       \
            pixelCode;                                                                             \
            destination = destination + 1;                                                         \
            publishedDestination = destination;                                                    \
            remaining--;                                                                           \
        } while (remaining != 0);                                                                  \
    }

#define RLE_PIXEL_DESCENDING_LOOP(source, destination, remaining, pixelCode)                       \
    s_dst = destination;                                                                           \
    s_loopCount = 0;                                                                               \
    i32 loopCount = remaining;                                                                     \
    if (loopCount > 0) {                                                                           \
        s_loopCount = loopCount;                                                                   \
        do {                                                                                       \
            pixelCode;                                                                             \
            source = source + 1;                                                                   \
            destination = destination - 1;                                                         \
            s_dst = destination;                                                                   \
            remaining--;                                                                           \
        } while (remaining != 0);                                                                  \
    }

#define RLE_PIXEL_INDEXED_ASCENDING_LOOP(                                                          \
    destination,                                                                                   \
    count,                                                                                         \
    publishedDestination,                                                                          \
    publishedIndex,                                                                                \
    pixelCode                                                                                      \
)                                                                                                  \
    publishedDestination = destination;                                                            \
    publishedIndex = 0;                                                                            \
    if (0 < static_cast<i32>(count)) {                                                             \
        do {                                                                                       \
            pixelCode;                                                                             \
            destination = destination + 1;                                                         \
            publishedDestination = destination;                                                    \
            publishedIndex = publishedIndex + 1;                                                   \
        } while (publishedIndex < static_cast<i32>(count));                                        \
    }

#define RLE_PIXEL_INDEXED_DESCENDING_LOOP(                                                         \
    source,                                                                                        \
    destination,                                                                                   \
    count,                                                                                         \
    publishedDestination,                                                                          \
    publishedIndex,                                                                                \
    pixelCode                                                                                      \
)                                                                                                  \
    publishedDestination = destination;                                                            \
    publishedIndex = 0;                                                                            \
    if (0 < static_cast<i32>(count)) {                                                             \
        do {                                                                                       \
            pixelCode;                                                                             \
            source = source + 1;                                                                   \
            destination = destination - 1;                                                         \
            publishedDestination = destination;                                                    \
            publishedIndex = publishedIndex + 1;                                                   \
        } while (publishedIndex < static_cast<i32>(count));                                        \
    }

/* Tier 2: clipping arithmetic used by handlers. */

#define RLE_CLIP_FORWARD_FILL(rowVisible)                                                          \
    i32 fillRight;                                                                                 \
    if ((rowVisible) && (fillRight = X + count, clipX < fillRight) && s_clipR >= X) {              \
        if (clipX <= X) {                                                                          \
            if (s_clipR >= fillRight) {                                                            \
                memset(row + X, s_color, count);                                                   \
            } else {                                                                               \
                memset(row + X, s_color, (s_clipR - X) + 1);                                       \
            }                                                                                      \
        } else {                                                                                   \
            if (s_clipR >= fillRight)                                                              \
                memset(row + clipX, s_color, (count - clipX) + X);                                 \
            else                                                                                   \
                memset(row + clipX, s_color, clipW);                                               \
        }                                                                                          \
    }

#define RLE_CLIP_FORWARD_DIM_SPAN(clipRight, publishedCount)                                       \
    do {                                                                                           \
        i32 runRight = X + count;                                                                  \
        if (X >= clipX) {                                                                          \
            i32 currentClipRight = clipRight;                                                      \
            clippedCount = count;                                                                  \
            if (currentClipRight < runRight)                                                       \
                clippedCount = (currentClipRight - X) + 1;                                         \
            savedDestination = row + X;                                                            \
        } else {                                                                                   \
            i32 currentClipRight = clipRight;                                                      \
            publishedCount = count;                                                                \
            if (runRight <= currentClipRight)                                                      \
                count = (count - clipX) + X;                                                       \
            else                                                                                   \
                count = clipW;                                                                     \
            clippedCount = count;                                                                  \
            savedDestination = row + clipX;                                                        \
        }                                                                                          \
    } while (0)

/* Tier 3: complete opcode handlers. These are the TU-facing loop blocks. */

#define RLE_HANDLER_FORWARD_SKIP_OR_END(runMask)                                                   \
    s_x = X;                                                                                       \
    s_row = row;                                                                                   \
    s_run = command;                                                                               \
    i32 skipCount = command & (runMask);                                                           \
    if (skipCount == 0)                                                                            \
        return;                                                                                    \
    X = X + skipCount;                                                                             \
    continue

#define RLE_HANDLER_REVERSE_SKIP_OR_END(source)                                                    \
    s_run = command;                                                                               \
    i32 skipCount = command & ICON_RLE_COMMAND_RUN_MASK;                                           \
    s_x = X;                                                                                       \
    s_src = source;                                                                                \
    if (skipCount == 0)                                                                            \
        return;                                                                                    \
    X = X - skipCount;                                                                             \
    continue

#define RLE_HANDLER_MONO_REVERSE_SKIP_OR_END(source)                                               \
    s_run = command;                                                                               \
    i32 skipCount = command & ICON_RLE_MONO_RUN_MASK;                                              \
    s_x = X;                                                                                       \
    s_src = source;                                                                                \
    if (skipCount == ICON_RLE_MONO_END_COUNT)                                                      \
        return;                                                                                    \
    X = X - skipCount;                                                                             \
    continue

#define RLE_HANDLER_FORWARD_NEW_ROW(rowPitch)                                                      \
    X = s_left;                                                                                    \
    s_y = s_y + 1;                                                                                 \
    row = row + (rowPitch)

#define RLE_HANDLER_REVERSE_NEW_ROW(rowPitch)                                                      \
    X = s_right;                                                                                   \
    s_y = s_y + 1;                                                                                 \
    s_row = s_row + (rowPitch)

#define RLE_HANDLER_REVERSE_LITERAL_RUN(source, rowVisible, pixelCode)                             \
    u32 literalCount = command;                                                                    \
    u8* literalDestination = s_row + X;                                                            \
    i32 trailingSourceSkip = 0;                                                                    \
    if (clip != ICON_DRAW_NO_CLIP) {                                                               \
        literalCount = 0;                                                                          \
        trailingSourceSkip = command;                                                              \
        if (rowVisible) {                                                                          \
            i32 literalLeft = (X - command) + 1;                                                   \
            i32 literalRight = X < s_clipR ? X : s_clipR;                                          \
            i32 visibleLeft = clipX < literalLeft ? literalLeft : clipX;                           \
            if (visibleLeft <= literalRight) {                                                     \
                i32 leadingSourceSkip = X - literalRight;                                          \
                source = source + leadingSourceSkip;                                               \
                literalDestination = s_row + literalRight;                                         \
                literalCount = literalRight - visibleLeft + 1;                                     \
                trailingSourceSkip = command - leadingSourceSkip - literalCount;                   \
            }                                                                                      \
        }                                                                                          \
        s_spanCount = literalCount;                                                                \
        s_srcSkip = trailingSourceSkip;                                                            \
    }                                                                                              \
    if (literalCount != 0) {                                                                       \
        RLE_PIXEL_DESCENDING_LOOP(source, literalDestination, literalCount, pixelCode);            \
    }                                                                                              \
    source = source + trailingSourceSkip;                                                          \
    X = X - command;                                                                               \
    s_run = command;                                                                               \
    continue

#define RLE_HANDLER_SHEARED_FORWARD_SKIP_OR_END()                                                  \
    s_run = command;                                                                               \
    i32 skipCount = command & ICON_RLE_COMMAND_RUN_MASK;                                           \
    if (skipCount == 0)                                                                            \
        return;                                                                                    \
    s_x = s_x + skipCount;                                                                         \
    continue

#define RLE_HANDLER_SHEARED_REVERSE_SKIP_OR_END()                                                  \
    s_run = command;                                                                               \
    i32 skipCount = command & ICON_RLE_COMMAND_RUN_MASK;                                           \
    if (skipCount == 0)                                                                            \
        return;                                                                                    \
    s_x = s_x - skipCount;                                                                         \
    continue

#define RLE_HANDLER_SHEARED_FORWARD_FILL_RUN(rowVisible)                                           \
    s_run = count;                                                                                 \
    if ((rowVisible) && static_cast<i32>(s_x + count) > clipX && s_clipR >= s_x) {                 \
        i32 fillRight = s_x + count;                                                               \
        if (clipX <= s_x) {                                                                        \
            if (s_clipR >= fillRight)                                                              \
                memset(s_row + s_x, s_color, count);                                               \
            else                                                                                   \
                memset(s_row + s_x, s_color, (s_clipR - s_x) + 1);                                 \
        } else {                                                                                   \
            if (s_clipR >= fillRight)                                                              \
                memset(s_row + clipX, s_color, (count - clipX) + s_x);                             \
            else                                                                                   \
                memset(s_row + clipX, s_color, clipW);                                             \
        }                                                                                          \
    }                                                                                              \
    s_x = s_x + count;                                                                             \
    continue

#define RLE_HANDLER_SHEARED_REVERSE_FILL_RUN(rowVisible)                                           \
    s_run = count;                                                                                 \
    if (rowVisible) {                                                                              \
        i32 fillLeft = (s_x - count) + 1;                                                          \
        if (clipX <= fillLeft && s_x <= s_clipR) {                                                 \
            if (clipX <= fillLeft)                                                                 \
                memset((s_row - count) + s_x + 1, s_color, count);                                 \
            else                                                                                   \
                memset(s_row + clipX, s_color, (s_x - clipX) + 1);                                 \
        }                                                                                          \
    }                                                                                              \
    s_x = s_x - count;                                                                             \
    continue

#define RLE_HANDLER_SHEARED_FORWARD_DIM_RUN(rowVisible)                                            \
    s_run = flags;                                                                                 \
    s_spanCount = count;                                                                           \
    if ((flags & ICON_RLE_DIM_APPLY_FLAG) != 0) {                                                  \
        s_dimPal = RLE_EXPR_DIM_PALETTE(flags);                                                    \
        if ((rowVisible) && static_cast<i32>(s_x + s_spanCount) > clipX && s_clipR >= s_x) {       \
            i32 dimRight = s_x + s_spanCount;                                                      \
            u8* dimDestination;                                                                    \
            if (clipX <= s_x) {                                                                    \
                if (s_clipR < dimRight)                                                            \
                    s_spanCount = (s_clipR - s_x) + 1;                                             \
                dimDestination = s_row + s_x;                                                      \
            } else {                                                                               \
                if (s_clipR >= dimRight)                                                           \
                    s_spanCount = s_spanCount + (s_x - clipX);                                     \
                else                                                                               \
                    s_spanCount = clipW;                                                           \
                dimDestination = s_row + clipX;                                                    \
            }                                                                                      \
            RLE_PIXEL_INDEXED_ASCENDING_LOOP(dimDestination, s_spanCount, s_dimDst, s_loopIndex, { \
                *dimDestination = s_dimPal[*dimDestination];                                       \
            });                                                                                    \
        }                                                                                          \
    }                                                                                              \
    s_x = s_x + s_dimLen;                                                                          \
    continue

#define RLE_HANDLER_SHEARED_REVERSE_DIM_RUN(rowVisible)                                            \
    s_run = flags;                                                                                 \
    s_spanCount = count;                                                                           \
    if ((flags & ICON_RLE_DIM_APPLY_FLAG) != 0) {                                                  \
        s_dimPal = RLE_EXPR_DIM_PALETTE(flags);                                                    \
        if ((rowVisible)) {                                                                        \
            i32 dimLeft = (s_x - s_spanCount) + 1;                                                 \
            if (clipX <= dimLeft && s_x <= s_clipR) {                                              \
                u8* dimDestination;                                                                \
                if (clipX <= dimLeft) {                                                            \
                    dimDestination = (s_row - s_spanCount) + s_x + 1;                              \
                } else {                                                                           \
                    s_spanCount = (s_x - clipX) + 1;                                               \
                    dimDestination = s_row + clipX;                                                \
                }                                                                                  \
                RLE_PIXEL_INDEXED_ASCENDING_LOOP(                                                  \
                    dimDestination,                                                                \
                    s_spanCount,                                                                   \
                    s_dimDst,                                                                      \
                    s_loopIndex,                                                                   \
                    { *dimDestination = s_dimPal[*dimDestination]; }                               \
                );                                                                                 \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
    s_x = s_x - s_dimLen;                                                                          \
    continue

#define RLE_HANDLER_SHEARED_FORWARD_LITERAL_RUN(rowVisible)                                        \
    if ((rowVisible) && static_cast<i32>(s_x + command) > clipX && s_clipR >= s_x) {               \
        i32 literalRight = s_x + command;                                                          \
        if (clipX <= s_x) {                                                                        \
            if (s_clipR >= literalRight)                                                           \
                memcpy(s_row + s_x, s_src, command);                                               \
            else                                                                                   \
                memcpy(s_row + s_x, s_src, (s_clipR - s_x) + 1);                                   \
        } else {                                                                                   \
            if (s_clipR >= literalRight)                                                           \
                memcpy(s_row + clipX, s_src + (clipX - s_x), (command - clipX) + s_x);             \
            else                                                                                   \
                memcpy(s_row + clipX, s_src + (clipX - s_x), clipW);                               \
        }                                                                                          \
    }                                                                                              \
    s_x = s_x + command;                                                                           \
    s_src = s_src + command;                                                                       \
    continue

#define RLE_HANDLER_SHEARED_REVERSE_LITERAL_RUN(rowVisible)                                        \
    u32 literalCount = command;                                                                    \
    u8* literalDestination = s_row + s_x;                                                          \
    i32 trailingSourceSkip = 0;                                                                    \
    if (rowVisible) {                                                                              \
        i32 literalLeft = (s_x - command) + 1;                                                     \
        i32 literalRight = s_x < s_clipR ? s_x : s_clipR;                                          \
        i32 visibleLeft = clipX < literalLeft ? literalLeft : clipX;                               \
        if (visibleLeft <= literalRight) {                                                         \
            i32 leadingSourceSkip = s_x - literalRight;                                            \
            s_src = s_src + leadingSourceSkip;                                                     \
            literalDestination = s_row + literalRight;                                             \
            literalCount = literalRight - visibleLeft + 1;                                         \
            trailingSourceSkip = command - leadingSourceSkip - literalCount;                       \
        } else {                                                                                   \
            literalCount = 0;                                                                      \
            trailingSourceSkip = command;                                                          \
        }                                                                                          \
    } else {                                                                                       \
        literalCount = 0;                                                                          \
        trailingSourceSkip = command;                                                              \
    }                                                                                              \
    s_spanCount = literalCount;                                                                    \
    s_srcSkip = trailingSourceSkip;                                                                \
    if (literalCount != 0) {                                                                       \
        RLE_PIXEL_INDEXED_DESCENDING_LOOP(                                                         \
            s_src,                                                                                 \
            literalDestination,                                                                    \
            literalCount,                                                                          \
            s_dst,                                                                                 \
            s_loopIndex,                                                                           \
            { *literalDestination = *s_src; }                                                      \
        );                                                                                         \
    }                                                                                              \
    s_src = s_src + trailingSourceSkip;                                                            \
    s_x = s_x - command;                                                                           \
    continue

#define RLE_HANDLER_SHEARED_FORWARD_NEW_ROW()                                                      \
    s_x = shear[s_y] + s_left;                                                                     \
    s_y = s_y + 1;                                                                                 \
    s_row = s_row + s_pitch

#define RLE_HANDLER_SHEARED_REVERSE_NEW_ROW()                                                      \
    s_x = s_right - shear[s_y];                                                                    \
    s_y = s_y + 1;                                                                                 \
    s_row = s_row + dest->m_width

#define RLE_HANDLER_MONO_FORWARD_SPAN_RUN(rowVisible, spanCode)                                    \
    if (clip == ICON_DRAW_NO_CLIP) {                                                               \
        u32 spanCount = command;                                                                   \
        u8* spanDestination = row + X;                                                             \
        spanCode;                                                                                  \
    } else {                                                                                       \
        i32 spanRight;                                                                             \
        if ((rowVisible) && (spanRight = X + command, clipX < spanRight) && X <= s_clipR) {        \
            u32 spanCount;                                                                         \
            u8* spanDestination;                                                                   \
            if (X >= clipX) {                                                                      \
                spanCount = s_clipR < spanRight ? (s_clipR - X) + 1 : command;                     \
                spanDestination = row + X;                                                         \
            } else {                                                                               \
                spanCount = s_clipR < spanRight ? clipW : (command - clipX) + X;                   \
                spanDestination = row + clipX;                                                     \
            }                                                                                      \
            spanCode;                                                                              \
        }                                                                                          \
    }                                                                                              \
    X = X + command;                                                                               \
    s_run = command;                                                                               \
    continue

#define RLE_HANDLER_MONO_REVERSE_SPAN_RUN(rowVisible, spanCode)                                    \
    if (clip == ICON_DRAW_NO_CLIP) {                                                               \
        u32 spanCount = command;                                                                   \
        u8* spanDestination = (s_row - command) + X + 1;                                           \
        spanCode;                                                                                  \
    } else {                                                                                       \
        i32 spanLeft;                                                                              \
        /* Retail redundantly requires clipX <= spanLeft before testing it again. */               \
        /* The unreachable clamp branch is intentional byte-accurate behavior. */                  \
        if ((rowVisible) && (spanLeft = (X - command) + 1, clipX <= spanLeft) && X <= s_clipR) {   \
            u32 spanCount;                                                                         \
            u8* spanDestination;                                                                   \
            if (clipX <= spanLeft) {                                                               \
                spanCount = command;                                                               \
                spanDestination = (s_row - command) + X + 1;                                       \
            } else {                                                                               \
                spanCount = (X - clipX) + 1;                                                       \
                spanDestination = s_row + clipX;                                                   \
            }                                                                                      \
            spanCode;                                                                              \
        }                                                                                          \
    }                                                                                              \
    X = X - command;                                                                               \
    s_run = command;                                                                               \
    continue

#define RLE_HANDLER_MONO_STATIC_FORWARD_SKIP_OR_END()                                              \
    s_row = row;                                                                                   \
    s_run = command;                                                                               \
    i32 skipCount = command & ICON_RLE_MONO_RUN_MASK;                                              \
    if (skipCount == ICON_RLE_MONO_END_COUNT)                                                      \
        return;                                                                                    \
    s_x = s_x + skipCount;                                                                         \
    continue

#define RLE_HANDLER_MONO_FORWARD_FILL_RUN(rowVisible)                                              \
    if (clip == ICON_DRAW_NO_CLIP) {                                                               \
        memset(row + s_x, color, command);                                                         \
    } else {                                                                                       \
        i32 fillRight;                                                                             \
        if ((rowVisible) && (fillRight = s_x + command, clipX < fillRight) && s_clipR >= s_x) {    \
            if (clipX <= s_x) {                                                                    \
                if (s_clipR >= fillRight)                                                          \
                    memset(row + s_x, color, command);                                             \
                else                                                                               \
                    memset(row + s_x, color, (s_clipR - s_x) + 1);                                 \
            } else {                                                                               \
                if (s_clipR >= fillRight)                                                          \
                    memset(row + clipX, color, (command - clipX) + s_x);                           \
                else                                                                               \
                    memset(row + clipX, color, clipW);                                             \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
    s_run = command;                                                                               \
    s_x = s_x + command;                                                                           \
    continue

#define RLE_HANDLER_MONO_STATIC_FORWARD_NEW_ROW(rowPitch)                                          \
    s_x = s_left;                                                                                  \
    row = row + (rowPitch);                                                                        \
    s_y = s_y + 1

#define RLE_HANDLER_MONO_REVERSE_FILL_RUN(rowVisible)                                              \
    if (clip == ICON_DRAW_NO_CLIP) {                                                               \
        memset((s_row - command) + X + 1, color, command);                                         \
    } else {                                                                                       \
        i32 fillLeft;                                                                              \
        /* Preserve retail's redundant edge test and unreachable clamp branch. */                  \
        if ((rowVisible) && (fillLeft = (X - command) + 1, clipX <= fillLeft) && X <= s_clipR) {   \
            if (clipX <= fillLeft)                                                                 \
                memset((s_row - command) + X + 1, color, command);                                 \
            else                                                                                   \
                memset(s_row + clipX, color, (X - clipX) + 1);                                     \
        }                                                                                          \
    }                                                                                              \
    s_run = command;                                                                               \
    X = X - command;                                                                               \
    continue

#define RLE_HANDLER_FORWARD_DIM_RUN(applyDim)                                                      \
    s_spanCount = count;                                                                           \
    s_run = flags;                                                                                 \
    if ((flags & ICON_RLE_DIM_APPLY_FLAG) != 0) {                                                  \
        u8* palette = RLE_EXPR_DIM_PALETTE(flags);                                                 \
        if (clip == ICON_DRAW_NO_CLIP) {                                                           \
            savedDestination = row + X;                                                            \
            s_dimPal = palette;                                                                    \
            RLE_PIXEL_ASCENDING_LOOP(savedDestination, count, s_dst, {                             \
                s_dimPal = palette;                                                                \
                if ((applyDim) != 0)                                                               \
                    *savedDestination = palette[*savedDestination];                                \
            });                                                                                    \
        } else {                                                                                   \
            s_spanCount = count;                                                                   \
            s_dimPal = palette;                                                                    \
            if (clipY <= s_y && s_clipB >= s_y && static_cast<i32>(X + count) > clipX              \
                && s_clipR >= X) {                                                                 \
                u32 clippedCount;                                                                  \
                RLE_CLIP_FORWARD_DIM_SPAN(s_clipR, s_spanCount);                                   \
                s_spanCount = clippedCount;                                                        \
                s_dimPal = palette;                                                                \
                RLE_PIXEL_ASCENDING_LOOP(savedDestination, clippedCount, s_dst, {                  \
                    s_dimPal = palette;                                                            \
                    if ((applyDim) != 0)                                                           \
                        *savedDestination = palette[*savedDestination];                            \
                });                                                                                \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
    X = X + s_dimLen;                                                                              \
    continue

#define RLE_HANDLER_FILL_RUN(rowVisible)                                                           \
    if (clip == ICON_DRAW_NO_CLIP) {                                                               \
        memset(row + X, s_color, count);                                                           \
    } else {                                                                                       \
        RLE_CLIP_FORWARD_FILL(rowVisible);                                                         \
    }                                                                                              \
    X = X + count;                                                                                 \
    s_run = count;                                                                                 \
    continue

#define RLE_HANDLER_REVERSE_FILL_RUN(rowVisible)                                                   \
    if (clip == ICON_DRAW_NO_CLIP) {                                                               \
        memset((s_row - count) + X + 1, s_color, count);                                           \
    } else {                                                                                       \
        i32 left = (X - count) + 1;                                                                \
        if ((rowVisible) && clipX <= left && X <= s_clipR) {                                       \
            if (clipX <= left)                                                                     \
                memset((s_row - count) + X + 1, s_color, count);                                   \
            else                                                                                   \
                memset(s_row + clipX, s_color, (X - clipX) + 1);                                   \
        }                                                                                          \
    }                                                                                              \
    X = X - count;                                                                                 \
    s_run = count;                                                                                 \
    continue

#define RLE_HANDLER_REVERSE_DIM_RUN(rowVisible)                                                    \
    s_run = flags;                                                                                 \
    s_spanCount = count;                                                                           \
    if ((flags & ICON_RLE_DIM_APPLY_FLAG) != 0) {                                                  \
        u8* palette = RLE_EXPR_DIM_PALETTE(flags);                                                 \
        s_dimPal = palette;                                                                        \
        if (clip == ICON_DRAW_NO_CLIP) {                                                           \
            u8* dimDestination = (s_row - count) + X + 1;                                          \
            RLE_PIXEL_ASCENDING_LOOP(dimDestination, count, s_dimDst, {                            \
                *dimDestination = palette[*dimDestination];                                        \
            });                                                                                    \
        } else {                                                                                   \
            i32 left = (X - count) + 1;                                                            \
            if ((rowVisible) && clipX <= left && X <= s_clipR) {                                   \
                u8* dimDestination;                                                                \
                if (clipX <= left) {                                                               \
                    dimDestination = (s_row - count) + X + 1;                                      \
                } else {                                                                           \
                    count = (X - clipX) + 1;                                                       \
                    dimDestination = s_row + clipX;                                                \
                }                                                                                  \
                s_spanCount = count;                                                               \
                RLE_PIXEL_ASCENDING_LOOP(dimDestination, count, s_dimDst, {                        \
                    *dimDestination = palette[*dimDestination];                                    \
                });                                                                                \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
    X = X - s_dimLen;                                                                              \
    continue

#define RLE_HANDLER_FORWARD_COPY_LITERAL_RUN()                                                     \
    u32 count = command;                                                                           \
    if (clip == ICON_DRAW_NO_CLIP) {                                                               \
        memcpy(row + X, s_src, count);                                                             \
    } else if (!(clipY > s_y || s_clipB < s_y || clipX >= X + static_cast<i32>(count)              \
                 || s_clipR < X)) {                                                                \
        if (clipX <= X) {                                                                          \
            if (s_clipR >= X + static_cast<i32>(count)) {                                          \
                memcpy(row + X, s_src, count);                                                     \
            } else {                                                                               \
                count = s_clipR - X + 1;                                                           \
                memcpy(row + X, s_src, count);                                                     \
            }                                                                                      \
        } else {                                                                                   \
            if (s_clipR >= X + static_cast<i32>(count))                                            \
                count -= clipX - X;                                                                \
            else                                                                                   \
                count = clipW;                                                                     \
            memcpy(row + clipX, s_src + clipX - X, count);                                         \
        }                                                                                          \
    }                                                                                              \
    X = X + command;                                                                               \
    s_src = s_src + command;                                                                       \
    s_run = command;                                                                               \
    continue

#define RLE_HANDLER_FORWARD_MAPPED_LITERAL_RUN()                                                   \
    u32 count;                                                                                     \
    do {                                                                                           \
        s_dst = savedDestination;                                                                  \
        s_literalSource = s_src;                                                                   \
        if (clip == ICON_DRAW_NO_CLIP) {                                                           \
            savedDestination = row + X;                                                            \
            count = command;                                                                       \
        } else {                                                                                   \
            if (s_y < clipY || s_clipB < s_y) {                                                    \
                count = 0;                                                                         \
                break;                                                                             \
            }                                                                                      \
            i32 literalRight = X + command;                                                        \
            if (literalRight <= clipX || (s_dst = savedDestination, s_clipR < X)) {                \
                count = 0;                                                                         \
                break;                                                                             \
            }                                                                                      \
            i32 clipRight = s_clipR;                                                               \
            if (clipX <= X) {                                                                      \
                savedDestination = row + X;                                                        \
                if (clipRight >= literalRight)                                                     \
                    count = command;                                                               \
                else                                                                               \
                    count = (clipRight - X) + 1;                                                   \
            } else {                                                                               \
                if (clipRight >= literalRight) {                                                   \
                    savedDestination = row + clipX;                                                \
                    count = (command - clipX) + X;                                                 \
                } else {                                                                           \
                    savedDestination = row + clipX;                                                \
                    count = clipW;                                                                 \
                }                                                                                  \
                s_literalSource = s_src + (clipX - X);                                             \
            }                                                                                      \
        }                                                                                          \
    } while (0);                                                                                   \
    s_spanCount = count;                                                                           \
    if (count != 0) {                                                                              \
        s_loopCount = 0;                                                                           \
        if (static_cast<i32>(count) > 0) {                                                         \
            s_loopCount = count;                                                                   \
            do {                                                                                   \
                i32 pixel = *s_literalSource;                                                      \
                savedDestination = savedDestination + 1;                                           \
                savedDestination[-1] = colorTable[pixel];                                          \
                s_literalSource = s_literalSource + 1;                                             \
            } while (--count != 0);                                                                \
        }                                                                                          \
    }                                                                                              \
    X = X + command;                                                                               \
    s_src = s_src + command;                                                                       \
    s_run = command;                                                                               \
    continue

/*
 * Tier 3.5: decode and dispatch.
 *
 * RLE_DISPATCH_SOLID_OR_DIM transfers control to caller-owned fill_run and
 * dim_run labels. Both labels must immediately precede complete Tier-3
 * handlers inside the same decoder loop.
 */

#define RLE_DISPATCH_SOLID_OR_DIM(source, colorExpression)                                         \
    s_run = command;                                                                               \
    u32 count = command & ICON_RLE_COMMAND_RUN_MASK;                                               \
    i32 flags = 0;                                                                                 \
    if (count != 0) {                                                                              \
        if (command == ICON_RLE_LONG_SOLID_COMMAND)                                                \
            count = ReadIconRleByte(source);                                                       \
        u8 encodedColor = ReadIconRleByte(source);                                                 \
        s_color = colorExpression;                                                                 \
        goto fill_run;                                                                             \
    }                                                                                              \
    flags = ReadIconRleByte(source);                                                               \
    count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;                                                 \
    if (count == 0)                                                                                \
        count = ReadIconRleByte(source);                                                           \
    s_dimLen = count;                                                                              \
    if (color != 0) {                                                                              \
        s_run = flags;                                                                             \
        if ((flags & ICON_RLE_DIM_RECOLOR_FLAG) != 0) {                                            \
            s_spanCount = count;                                                                   \
            s_color = static_cast<u8>(color);                                                      \
            goto fill_run;                                                                         \
        }                                                                                          \
    }                                                                                              \
    goto dim_run

/*
 * Tier 4: decoder setup. These are the other TU-facing macros.
 * docs/icon-rle-state.csv owns placement for the macro-owned state roles.
 */

#define RLE_STATE_ROW static u8* s_row
#define RLE_STATE_PITCH static i32 s_pitch
#define RLE_STATE_COLOR static u8 s_color
#define RLE_STATE_DIM_PALETTE static u8* s_dimPal
#define RLE_STATE_RUN static u32 s_run
#define RLE_STATE_SIGNED_RUN static i32 s_run
#define RLE_STATE_LOOP_COUNT static u32 s_loopCount
#define RLE_STATE_LOOP_INDEX static i32 s_loopIndex
#define RLE_STATE_SOURCE static u8* s_src
#define RLE_STATE_LITERAL_SOURCE static u8* s_literalSource
#define RLE_STATE_SOURCE_SKIP static i32 s_srcSkip
#define RLE_STATE_DESTINATION static u8* s_dst
#define RLE_STATE_DIM_DESTINATION static u8* s_dimDst
#define RLE_STATE_CLIP_RIGHT static i32 s_clipR
#define RLE_STATE_CLIP_BOTTOM static i32 s_clipB
#define RLE_STATE_LEFT static i32 s_left
#define RLE_STATE_RIGHT static i32 s_right
#define RLE_STATE_DIM_LENGTH static u32 s_dimLen
#define RLE_STATE_Y static i32 s_y
#define RLE_STATE_X static i32 s_x
#define RLE_STATE_ENTRY static IconEntry* s_entry
#define RLE_STATE_SPAN_COUNT static u32 s_spanCount
#define RLE_STATE_SIGNED_SPAN_COUNT static i32 s_spanCount

#define RLE_SETUP_GENERAL                                                                          \
    RLE_STATE_ROW;                                                                                 \
    RLE_STATE_SOURCE;                                                                              \
    RLE_STATE_CLIP_RIGHT;                                                                          \
    RLE_STATE_CLIP_BOTTOM;                                                                         \
    RLE_STATE_LEFT;                                                                                \
    RLE_STATE_Y;                                                                                   \
    RLE_STATE_X;                                                                                   \
    RLE_STATE_ENTRY

#define RLE_SETUP_RUN RLE_STATE_RUN
#define RLE_SETUP_SIGNED_RUN RLE_STATE_SIGNED_RUN
#define RLE_SETUP_FORWARD_ROW RLE_STATE_PITCH
#define RLE_SETUP_REVERSE_ROW RLE_STATE_RIGHT
#define RLE_SETUP_COLOR RLE_STATE_COLOR

#define RLE_SETUP_DIM                                                                              \
    RLE_STATE_DIM_PALETTE;                                                                         \
    RLE_STATE_DIM_LENGTH;                                                                          \
    RLE_STATE_SPAN_COUNT

#define RLE_SETUP_SIGNED_DIM                                                                       \
    RLE_STATE_DIM_PALETTE;                                                                         \
    RLE_STATE_DIM_LENGTH;                                                                          \
    RLE_STATE_SIGNED_SPAN_COUNT

#define RLE_SETUP_COUNTED_PIXELS                                                                   \
    RLE_STATE_LOOP_COUNT;                                                                          \
    RLE_STATE_DESTINATION

#define RLE_SETUP_INDEXED_DIM_PIXELS                                                               \
    RLE_STATE_LOOP_INDEX;                                                                          \
    RLE_STATE_DIM_DESTINATION

#define RLE_SETUP_DIM_DESTINATION RLE_STATE_DIM_DESTINATION
#define RLE_SETUP_REVERSE_SOURCE_SKIP RLE_STATE_SOURCE_SKIP

#define RLE_SETUP_REVERSE_LITERAL                                                                  \
    RLE_STATE_SOURCE_SKIP;                                                                         \
    RLE_STATE_DESTINATION

#define RLE_SETUP_COLOR_TABLE_LITERAL RLE_STATE_LITERAL_SOURCE

#define RLE_SETUP_MONO_DIM                                                                         \
    RLE_STATE_LOOP_COUNT;                                                                          \
    RLE_STATE_DESTINATION;                                                                         \
    RLE_STATE_SPAN_COUNT

#define RLE_SETUP_FORWARD_ENTRY_STATE                                                              \
    IconEntry* entry = &srcIcon->Entries()[frame];                                                 \
    s_entry = entry;                                                                               \
    s_src = srcIcon->m_data + entry->srcOffset

#define RLE_SETUP_FORWARD_ENTRY(savedDestination)                                                  \
    RLE_SETUP_FORWARD_ENTRY_STATE;                                                                 \
    u8* savedDestination

#define RLE_SETUP_FORWARD_ORIGIN                                                                   \
    i32 X = x + entry->x;                                                                          \
    i32 x0 = X;                                                                                    \
    s_left = X;                                                                                    \
    s_pitch = dest->m_width;                                                                       \
    s_y = entry->y + y;                                                                            \
    i32 currentY = s_y

#define RLE_SETUP_FORWARD_OUTPUT_ROW                                                               \
    u8* row = dest->m_pixels + s_pitch * s_y;                                                      \
    i32 command

#define RLE_SETUP_NORMALIZE_CLIP(clipRight, clipBottom)                                            \
    if (clip != ICON_DRAW_NO_CLIP) {                                                               \
        if (x0 < clipX || clipW + clipX < entry->w + x0 || currentY < clipY                        \
            || clipY + clipH < entry->h + currentY) {                                              \
            clip = ICON_DRAW_CLIP;                                                                 \
            clipRight = clipX + clipW - 1;                                                         \
            clipBottom = clipY + clipH - 1;                                                        \
        } else {                                                                                   \
            clip = ICON_DRAW_NO_CLIP;                                                              \
        }                                                                                          \
    }

#define RLE_SETUP_REVERSE_NORMALIZE_CLIP(clipRight, clipBottom, rightExclusive)                    \
    if (clip != ICON_DRAW_NO_CLIP) {                                                               \
        if (x0 < clipX || clipW + clipX < rightExclusive || currentY < clipY                       \
            || clipY + clipH < entry->h + currentY) {                                              \
            clip = ICON_DRAW_CLIP;                                                                 \
            clipRight = clipX + clipW - 1;                                                         \
            clipBottom = clipY + clipH - 1;                                                        \
        } else {                                                                                   \
            clip = ICON_DRAW_NO_CLIP;                                                              \
        }                                                                                          \
    }

#endif
