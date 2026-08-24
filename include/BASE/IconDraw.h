#ifndef HOMM2_BASE_ICONDRAW_H
#define HOMM2_BASE_ICONDRAW_H

#include <Ints.h>

enum {
    ICON_DRAW_NO_CLIP = 0,
    ICON_DRAW_CLIP    = 1
};
typedef i32 IconDrawClipMode;
enum {
    ICON_DRAW_NORMAL  = 0,
    ICON_DRAW_FLIPPED = 1
};
typedef i32 IconDrawOrientation;
enum {
    ICON_DRAW_SKIPPED   = 0,
    ICON_DRAW_COMPLETED = 1
};
typedef i32 IconDrawResult;
#endif
