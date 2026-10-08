#ifndef HOMM2_ICON2BSD_H
#define HOMM2_ICON2BSD_H

#include <Domains.h>
#include <BASE/IconDraw.h>

class bitmap;
class icon;

// IconToBitmapScale for a frame two cells square: draws it 2 * scale pixels
// wide (the editor's heroes); EDT2PL.exe 0x00439b00, a BASE object the game
// does not link.
void IconToBitmapScaleDouble(
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
