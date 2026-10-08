#ifndef HOMM2_BASE_RIPPLE_H
#define HOMM2_BASE_RIPPLE_H

#include <H2/Ints.h>

class bitmap;

void DoRipple(class bitmap* source, class bitmap* destination, i32 height, i32 strength);

#endif
