#ifndef HOMM2_BASE_ICONF2BY_H
#define HOMM2_BASE_ICONF2BY_H

#include <Domains.h>

#if H2_STRICT_ENUMS
#include <BASE/IconDraw.h>
#endif

class bitmap;
class icon;

void FlipIconToBitmapYModify(
    class icon* sourceIcon,
    class bitmap* destination,
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

#endif // HOMM2_BASE_ICONF2BY_H
