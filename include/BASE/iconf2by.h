#ifndef HOMM2_ICONF2BY_H
#define HOMM2_ICONF2BY_H

#include <Ints.h>
// Declarations of the free functions DEFINED in iconf2by.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void FlipIconToBitmapYModify(class icon *, class bitmap *, i32, i32, i32, i32, i32, i32, i32, i32, i32, i8 *);

#endif // HOMM2_ICONF2BY_H
