#ifndef HOMM2_FINDPATH_H
#define HOMM2_FINDPATH_H

#include <Ints.h>

H2_ENUM_BEGIN(FindPathConstant)
    PATH_ATTACK_MASK_SURROUNDED = 0xff,
    PATH_LENGTH_LIMIT = 0xff
H2_ENUM_END(FindPathConstant)

i32 CalcTerrainCost(i32, i32, i32, i32, i32, i32);

#endif
