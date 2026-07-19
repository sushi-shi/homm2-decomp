#ifndef HOMM2_BLUR_H
#define HOMM2_BLUR_H

#include <Ints.h>

class bitmap;

void DoBlur(
    class bitmap* destination,
    class bitmap* source,
    i32 height,
    i32 redAdjust,
    i32 greenAdjust,
    i32 blueAdjust
);

#endif
