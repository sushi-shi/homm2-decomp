#ifndef HOMM2_ICONM2B_H
#define HOMM2_ICONM2B_H

#include <Ints.h>
// Declarations of the free functions DEFINED in Iconm2b.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void MonoIconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    i32 clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
);

#endif // HOMM2_ICONM2B_H
