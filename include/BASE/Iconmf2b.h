#ifndef HOMM2_ICONMF2B_H
#define HOMM2_ICONMF2B_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void FlipMonoIconToBitmap(
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
