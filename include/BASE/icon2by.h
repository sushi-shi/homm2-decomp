#ifndef HOMM2_ICON2BY_H
#define HOMM2_ICON2BY_H
// Declarations of the free functions DEFINED in icon2by.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void IconToBitmapYModify(class icon *, class bitmap *, int, int, int, int, int, int, int, int, int, signed char *);

#endif // HOMM2_ICON2BY_H
