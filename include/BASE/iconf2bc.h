#ifndef HOMM2_ICONF2BC_H
#define HOMM2_ICONF2BC_H

#include <Ints.h>

class bitmap;
class icon;

void FlipIconToBitmapColorTable(
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
    u8*
);

#endif
