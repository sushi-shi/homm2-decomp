#ifndef HOMM2_ICON2BC_H
#define HOMM2_ICON2BC_H

#include <Ints.h>

class bitmap;
class icon;

void IconToBitmapColorTable(
    class icon*,
    class bitmap*,
    i32,
    i32,
    i32,
    i32,
    i32,
    i32,
    i32,
    i32,
    i32,
    u8*,
    i32
);

#endif
