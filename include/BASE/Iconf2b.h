#ifndef HOMM2_ICONF2B_H
#define HOMM2_ICONF2B_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void FlipIconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32,
    i32,
    i32,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
    i32,
    i32,
    i32,
    i32,
    i32
);

#endif
