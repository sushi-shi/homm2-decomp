#ifndef HOMM2_ICON2BSD_H
#define HOMM2_ICON2BSD_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;


void IconToBitmapScaleDouble(
    class icon* sourceIcon,
    class bitmap* destination,
    i32 destinationX,
    i32 destinationY,
    i32 frame,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 scale
);

#endif
