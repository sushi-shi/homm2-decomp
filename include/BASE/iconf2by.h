#ifndef HOMM2_ICONF2BY_H
#define HOMM2_ICONF2BY_H

#include <Ints.h>

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void FlipIconToBitmapYModify(
    class icon* sourceIcon,
    class bitmap* destination,
    i32 x,
    i32 y,
    i32 frame,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 color,
    i8* shear
);

#endif
