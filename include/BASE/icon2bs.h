#ifndef HOMM2_ICON2BS_H
#define HOMM2_ICON2BS_H

#include <Ints.h>
class bitmap;
class icon;

H2_ENUM_BEGIN(IconScaleConstant)
    ICON_SCALE_NATIVE_SIZE = 0x20,
    ICON_SCALE_WORK_BITMAP_SIZE = 0x40
H2_ENUM_END(IconScaleConstant)

void IconToBitmapScale(class icon*, class bitmap*, i32, i32, i32, i32, i32, i32, i32, i32, i32);

#endif
