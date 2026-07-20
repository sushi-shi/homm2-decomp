#ifndef HOMM2_ICON2BS_H
#define HOMM2_ICON2BS_H

#include <BASE/IconDraw.h>

class bitmap;
class icon;

void IconToBitmapScale(
    class icon*,
    class bitmap*,
    i32,
    i32,
    i32,
    H2_ENUM_PARAM(IconDrawClipMode, i32),
    i32,
    i32,
    i32,
    i32,
    i32
);

#endif
