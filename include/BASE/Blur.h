#ifndef HOMM2_BLUR_H
#define HOMM2_BLUR_H

#include <H2/Ints.h>

class bitmap;

void DoBlur(
    class bitmap* scratch,
    class bitmap* screen,
    i32 height,
    i32 redAdjust,
    i32 greenAdjust,
    i32 blueAdjust
);

#endif
