#ifndef HOMM2_ICONF2BY_H
#define HOMM2_ICONF2BY_H
// Declarations of the free functions DEFINED in iconf2by.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void FlipIconToBitmapYModify(class icon *, class bitmap *, int, int, int, int, int, int, int, int, int, signed char *);

#endif // HOMM2_ICONF2BY_H
