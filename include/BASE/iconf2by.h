#ifndef HOMM2_ICONF2BY_H
#define HOMM2_ICONF2BY_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void FlipIconToBitmapYModify(
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
    i8* shear
);

#endif
