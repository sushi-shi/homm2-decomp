#ifndef HOMM2_ICON2B_H
#define HOMM2_ICON2B_H

#include <Ints.h>
// Declarations of the free functions DEFINED in Icon2b.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void IconToBitmap(class icon *, class bitmap *, i32, i32, i32, i32, i32, i32, i32, i32, i32);

#endif // HOMM2_ICON2B_H
