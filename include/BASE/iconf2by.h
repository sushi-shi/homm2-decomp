#ifndef HOMM2_ICONF2BY_H
#define HOMM2_ICONF2BY_H

#include <Ints.h>

#include <BASE/IconDraw.h>
#include <span>

class bitmap;
class icon;

void FlipIconToBitmapYModify(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 color,
    std::span<const i8> shear
);

#endif
