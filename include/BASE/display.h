#ifndef HOMM2_BASE_DISPLAY_H
#define HOMM2_BASE_DISPLAY_H

#include <va.h>

// Logical framebuffer coordinates, before scaling to the live client window.
// Adventure/combat subviews retain their own clipping extents.
H2_ENUM_BEGIN(LogicalScreenConstant)
    LOGICAL_SCREEN_WIDTH  = 640,
    LOGICAL_SCREEN_HEIGHT = 480,
    LOGICAL_SCREEN_MAX_X   = LOGICAL_SCREEN_WIDTH - 1,
    LOGICAL_SCREEN_MAX_Y   = LOGICAL_SCREEN_HEIGHT - 1,
H2_ENUM_END(LogicalScreenConstant)

// Native RGB palette values use six bits per channel. Windows palette version
// 0x300 is a separate API field, not a palette byte count.
H2_ENUM_BEGIN(PaletteFormatConstant)
    PALETTE_COLOR_COUNT = 256,
    PALETTE_LEVEL_COUNT = 64,
    PALETTE_CHANNEL_MAX = PALETTE_LEVEL_COUNT - 1,
H2_ENUM_END(PaletteFormatConstant)

#endif
