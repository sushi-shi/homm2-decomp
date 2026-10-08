#ifndef HOMM2_BASE_ICON2BC_H
#define HOMM2_BASE_ICON2BC_H

#include <Domains.h>
#include <BASE/IconDraw.h>

class bitmap;
class icon;

void IconToBitmapColorTable(
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
    u8* colorTable,
    i32 drawShadows
);

#endif // HOMM2_BASE_ICON2BC_H
