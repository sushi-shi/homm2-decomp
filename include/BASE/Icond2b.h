#ifndef HOMM2_ICOND2B_H
#define HOMM2_ICOND2B_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void DimIconToBitmap(
    class icon*,
    class bitmap*,
    i32,
    i32,
    i32,
    i32,
    IconDrawClipMode,
    i32,
    i32,
    i32,
    i32
);

#endif
