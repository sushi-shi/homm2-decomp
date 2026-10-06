#ifndef HOMM2_ICOND2B_H
#define HOMM2_ICOND2B_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void DimIconToBitmap(
    class icon* sourceIcon,
    class bitmap* destination,
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
);

#endif
