#ifndef HOMM2_ICONM2B_H
#define HOMM2_ICONM2B_H
// Declarations of the free functions DEFINED in Iconm2b.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

void MonoIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                      int color, int clip, int clipX, int clipY, int clipW, int clipH);

#endif // HOMM2_ICONM2B_H
