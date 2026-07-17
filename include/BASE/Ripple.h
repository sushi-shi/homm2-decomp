#ifndef HOMM2_RIPPLE_H
#define HOMM2_RIPPLE_H

#include <Ints.h>
// Declarations of the free functions DEFINED in Ripple.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;

void DoRipple(class bitmap *, class bitmap *, i32, i32);

#endif // HOMM2_RIPPLE_H
