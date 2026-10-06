#ifndef HOMM2_RIPPLE_H
#define HOMM2_RIPPLE_H

#include <Ints.h>

class bitmap;

void DoRipple(class bitmap* source, class bitmap* destination, i32 height, i32 strength);

#endif
