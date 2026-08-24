#ifndef HOMM2_BASE_ICONDRAW_H
#define HOMM2_BASE_ICONDRAW_H

#include <Ints.h>

enum class IconDrawClipMode : i32 {
    ICON_DRAW_NO_CLIP = 0,
    ICON_DRAW_CLIP    = 1
};
using enum IconDrawClipMode;

enum class IconDrawOrientation : i8 {
    ICON_DRAW_NORMAL  = 0,
    ICON_DRAW_FLIPPED = 1
};
using enum IconDrawOrientation;

enum class IconDrawResult : i32 {
    ICON_DRAW_SKIPPED   = 0,
    ICON_DRAW_COMPLETED = 1
};
using enum IconDrawResult;

#endif
