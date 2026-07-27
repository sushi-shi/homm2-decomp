#ifndef HOMM2_BASE_ICONMACRO_H
#define HOMM2_BASE_ICONMACRO_H

#include <BASE/IconRle.h>

#define H2_ICON_RLE_DIM_PALETTE(flags)                                                          \
    (reinterpret_cast<u8*>(uDimPal)                                                             \
     + ((flags) & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE)

#define H2_ICON_RLE_CLIPPED_FILL(                                                               \
    runVisible, row, currentX, color, runLength, clipX, clipW, clipRight)                        \
    if (runVisible) {                                                                            \
        i32 fillRight = currentX + runLength;                                                     \
        if (clipX <= currentX) {                                                                  \
            if (clipRight >= fillRight) {                                                         \
                memset(row + currentX, color, runLength);                                         \
            } else {                                                                              \
                memset(row + currentX, color, (clipRight - currentX) + 1);                        \
            }                                                                                     \
        } else {                                                                                  \
            if (clipRight >= fillRight) {                                                         \
                memset(row + clipX, color, (runLength - clipX) + currentX);                       \
            } else {                                                                              \
                memset(row + clipX, color, clipW);                                                 \
            }                                                                                     \
        }                                                                                         \
    }

#define H2_ICON_RLE_FILL_RUN(                                                                    \
    clipMode, runVisible, row, currentX, color, runLength, clipX, clipW, clipRight, publishedRun) \
    if (clipMode == ICON_DRAW_NO_CLIP) {                                                          \
        memset(row + currentX, color, runLength);                                                  \
    } else                                                                                        \
        H2_ICON_RLE_CLIPPED_FILL(                                                                 \
            runVisible, row, currentX, color, runLength, clipX, clipW, clipRight                  \
        );                                                                                        \
    currentX = currentX + runLength;                                                              \
    publishedRun = runLength;                                                                     \
    continue

#endif
