#ifndef HOMM2_ICON2BS_H
#define HOMM2_ICON2BS_H
// Declarations of the free functions DEFINED in icon2bs.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void IconToBitmapScale(class icon *, class bitmap *, int, int, int, int, int, int, int, int, int);

#endif // HOMM2_ICON2BS_H
