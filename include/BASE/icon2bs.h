#ifndef HOMM2_ICON2BS_H
#define HOMM2_ICON2BS_H

#include <Ints.h>
// Declarations of the free functions DEFINED in icon2bs.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class bitmap;
class icon;

typedef enum IconScaleConstant {
    ICON_SCALE_NATIVE_SIZE = 0x20,
    ICON_SCALE_WORK_BITMAP_SIZE = 0x40
} IconScaleConstant;

void IconToBitmapScale(class icon *, class bitmap *, i32, i32, i32, i32, i32, i32, i32, i32, i32);

#endif // HOMM2_ICON2BS_H
