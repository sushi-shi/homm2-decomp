#ifndef HOMM2_ICONF2B_H
#define HOMM2_ICONF2B_H

#include <Ints.h>
// Declarations of the free functions DEFINED in Iconf2b.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void FlipIconToBitmap(class icon*, class bitmap*, i32, i32, i32, i32, i32, i32, i32, i32, i32);

#endif // HOMM2_ICONF2B_H
