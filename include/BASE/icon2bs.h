#ifndef HOMM2_ICON2BS_H
#define HOMM2_ICON2BS_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void IconToBitmapScale(
    class icon* srcIcon,
    class bitmap* dest,
    i32 destX,
    i32 destY,
    i32 frame,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 scale
);

#endif
