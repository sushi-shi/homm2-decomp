#ifndef HOMM2_FINDPATH_H
#define HOMM2_FINDPATH_H

#include <Ints.h>
// Declarations of the free functions DEFINED in FINDPATH.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

HOMM2_ENUM_VALUES_BEGIN(FindPathConstant)
    PATH_ATTACK_MASK_SURROUNDED = 0xff,
    PATH_LENGTH_LIMIT = 0xff
HOMM2_ENUM_VALUES_END(FindPathConstant)

i32 CalcTerrainCost(i32, i32, i32, i32, i32, i32);

#endif // HOMM2_FINDPATH_H
