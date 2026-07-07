#ifndef HOMM2_ICON2BC_H
#define HOMM2_ICON2BC_H
// Declarations of the free functions DEFINED in icon2bc.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void IconToBitmapColorTable(class icon *, class bitmap *, int, int, int, int, int, int, int, int, int, unsigned char *, int);

#endif // HOMM2_ICON2BC_H
