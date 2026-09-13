#ifndef HOMM2_ICONMF2B_H
#define HOMM2_ICONMF2B_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void FlipMonoIconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32,
    i32,
    i32,
    i32,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
    i32,
    i32,
    i32,
    i32
);

#endif
