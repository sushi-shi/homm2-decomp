#ifndef HOMM2_ICON2BC_H
#define HOMM2_ICON2BC_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void IconToBitmapColorTable(
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
    u8* colorTable,
    i32 dimGate
);

#endif
