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

// The editor's monster and hero icons: the frame drawn as the black
// shadow-masked variant (EDT2PL.exe 0x00439b00; the game does not link it).
void IconToBitmapScaleShadow(
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
