#ifndef HOMM2_ICONF2B_H
#define HOMM2_ICONF2B_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void FlipIconToBitmap(
    class icon*,
    class bitmap*,
    i32,
    i32,
    i32,
    IconDrawClipMode,
    i32,
    i32,
    i32,
    i32,
    i32
);

#endif
