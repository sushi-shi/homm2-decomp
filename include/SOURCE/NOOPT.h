#ifndef HOMM2_NOOPT_H
#define HOMM2_NOOPT_H

#include <Ints.h>
// Declarations of the free functions DEFINED in NOOPT.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

void DelayTil(i32*);
void DelayMilli(i32l);
void DelayTilMilli(i32l);

#endif // HOMM2_NOOPT_H
