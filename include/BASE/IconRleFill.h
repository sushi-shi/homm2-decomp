#ifndef HOMM2_BASE_ICONRLEFILL_H
#define HOMM2_BASE_ICONRLEFILL_H

#include <BASE/IconRle.h>

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

#endif
