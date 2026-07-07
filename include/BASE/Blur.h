#ifndef HOMM2_BLUR_H
#define HOMM2_BLUR_H
// Declarations of the free functions DEFINED in Blur.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;

void DoBlur(class bitmap *, class bitmap *, int, int, int, int);

#endif // HOMM2_BLUR_H
