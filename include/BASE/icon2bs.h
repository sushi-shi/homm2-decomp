#ifndef HOMM2_ICON2BS_H
#define HOMM2_ICON2BS_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void IconToBitmapScale(
    class icon* sourceIcon,
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 frame,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 scale
);

#endif
