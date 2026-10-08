#ifndef HOMM2_BASE_ICONF2BC_H
#define HOMM2_BASE_ICONF2BC_H

#include <Domains.h>
#include <BASE/IconDraw.h>

class bitmap;
class icon;

void FlipIconToBitmapColorTable(
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
    u8* colorTable
);

#endif
