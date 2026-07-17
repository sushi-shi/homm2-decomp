#ifndef HOMM2_ICONMF2B_H
#define HOMM2_ICONMF2B_H

#include <Ints.h>
// Declarations of the free functions DEFINED in Iconmf2b.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void FlipMonoIconToBitmap(class icon*, class bitmap*, i32, i32, i32, i32, i32, i32, i32, i32, i32);

#endif // HOMM2_ICONMF2B_H
