#ifndef HOMM2_BASE_DISPLAY_H
#define HOMM2_BASE_DISPLAY_H

#include <Ints.h>


typedef enum LogicalScreenConstant {
    LOGICAL_SCREEN_WIDTH  = 640,
    LOGICAL_SCREEN_HEIGHT = 480,
    LOGICAL_SCREEN_MAX_X   = LOGICAL_SCREEN_WIDTH - 1,
    LOGICAL_SCREEN_MAX_Y   = LOGICAL_SCREEN_HEIGHT - 1,
} LogicalScreenConstant;


typedef enum PaletteFormatConstant {
    PALETTE_COLOR_COUNT = 256,
    PALETTE_LEVEL_COUNT = 64,
    PALETTE_CHANNEL_MAX = PALETTE_LEVEL_COUNT - 1,
} PaletteFormatConstant;

#endif
