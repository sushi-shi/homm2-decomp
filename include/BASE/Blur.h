#ifndef HOMM2_BLUR_H
#define HOMM2_BLUR_H

#include <Ints.h>
// Declarations of the free functions DEFINED in Blur.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;

void DoBlur(class bitmap*, class bitmap*, i32, i32, i32, i32);

#endif // HOMM2_BLUR_H
