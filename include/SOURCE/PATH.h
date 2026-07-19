#ifndef HOMM2_PATH_H
#define HOMM2_PATH_H

#include <Ints.h>

i32 GetAdjacentCellIndexNoArmy(i32, i32);
i32 OppositeDirection(i32);

H2_ENUM_BEGIN(CombatHexDirection)
    COMBAT_DIRECTION_NORTHEAST       = 0,
    COMBAT_DIRECTION_EAST            = 1,
    COMBAT_DIRECTION_SOUTHEAST       = 2,
    COMBAT_DIRECTION_SOUTHWEST       = 3,
    COMBAT_DIRECTION_WEST            = 4,
    COMBAT_DIRECTION_NORTHWEST       = 5,
    COMBAT_DIRECTION_WIDE_WEST       = 6,
    COMBAT_DIRECTION_WIDE_EAST       = 7,
    COMBAT_DIRECTION_OPPOSITE_OFFSET = 3,
    COMBAT_DIRECTION_ADJACENT_COUNT  = 6,
    COMBAT_DIRECTION_COUNT           = 8
H2_ENUM_END(CombatHexDirection)

#endif
