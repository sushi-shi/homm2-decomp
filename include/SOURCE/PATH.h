#ifndef HOMM2_PATH_H
#define HOMM2_PATH_H

#include <Ints.h>
// Declarations of the free functions DEFINED in PATH.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

i32 GetAdjacentCellIndexNoArmy(i32, i32);
i32 OppositeDirection(i32);

typedef enum CombatHexDirection {
    COMBAT_DIRECTION_NORTHWEST = 0,
    COMBAT_DIRECTION_NORTHEAST = 1,
    COMBAT_DIRECTION_EAST = 2,
    COMBAT_DIRECTION_SOUTHEAST = 3,
    COMBAT_DIRECTION_SOUTHWEST = 4,
    COMBAT_DIRECTION_WEST = 5,
    COMBAT_DIRECTION_WIDE_WEST = 6,
    COMBAT_DIRECTION_WIDE_EAST = 7
} CombatHexDirection;

typedef enum CombatPathConstant {
    COMBAT_PATH_DIRECTION_COUNT = 6,
    COMBAT_PATH_WIDE_DIRECTION_COUNT = 8,
    COMBAT_PATH_SPECIAL_DIRECTION_MASK = 0xc0,
    COMBAT_PATH_IGNORE_SPEED = 99,
    COMBAT_PATH_INVALID_HEX = -1
} CombatPathConstant;

#endif // HOMM2_PATH_H
