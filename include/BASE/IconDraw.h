#ifndef HOMM2_BASE_ICONDRAW_H
#define HOMM2_BASE_ICONDRAW_H

// Rendering modes and fixed framebuffer extents used by the icon dispatch layer.
// Keep this implementation-only domain out of icon.h: most icon consumers do not
// compile the dispatch body, and VC4.2 allocation is sensitive to unused types.
HOMM2_ENUM_BEGIN(IconDrawClipMode)
    ICON_DRAW_NO_CLIP = 0,
    ICON_DRAW_CLIP = 1
HOMM2_ENUM_END(IconDrawClipMode)

HOMM2_ENUM_BEGIN(IconDrawOrientation)
    ICON_DRAW_NORMAL = 0,
    ICON_DRAW_FLIPPED = 1
HOMM2_ENUM_END(IconDrawOrientation)

HOMM2_ENUM_BEGIN(IconDrawResult)
    ICON_DRAW_SKIPPED = 0,
    ICON_DRAW_COMPLETED = 1
HOMM2_ENUM_END(IconDrawResult)

HOMM2_ENUM_BEGIN(IconColorTableMode)
    ICON_COLOR_TABLE_SKIP_DIM = 0,
    ICON_COLOR_TABLE_APPLY_DIM = 1
HOMM2_ENUM_END(IconColorTableMode)

HOMM2_ENUM_VALUES_BEGIN(IconDrawExtentConstant)
    ICON_DRAW_SCREEN_WIDTH = 640,
    ICON_DRAW_SCREEN_HEIGHT = 480,
    ICON_DRAW_COMBAT_HEIGHT = 444
HOMM2_ENUM_VALUES_END(IconDrawExtentConstant)

#endif // HOMM2_BASE_ICONDRAW_H
