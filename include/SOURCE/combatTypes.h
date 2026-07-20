#ifndef HOMM2_SOURCE_COMBATTYPES_H
#define HOMM2_SOURCE_COMBATTYPES_H

#include <va.h>

H2_ENUM_CLASS_BEGIN(ArmyFacing)
    ARMY_FACING_NONE  = -1,
    ARMY_FACING_LEFT  = 0,
    ARMY_FACING_RIGHT = 1
H2_ENUM_CLASS_END(ArmyFacing)

inline ArmyFacing OppositeArmyFacing(ArmyFacing facing) {
    return facing == ARMY_FACING_RIGHT ? ARMY_FACING_LEFT : ARMY_FACING_RIGHT;
}

inline i32 ArmyFacingRearHexOffset(ArmyFacing facing) {
    return facing == ARMY_FACING_RIGHT ? 1 : -1;
}

H2_ENUM_CLASS_BEGIN(ArmyDrawState)
    ARMY_DRAW_BEHIND      = 0,
    ARMY_DRAW_NORMAL      = 1,
    ARMY_DRAW_EFFECT      = 2,
    ARMY_DRAW_IN_FRONT    = 3,
    ARMY_DRAW_PHASE_COUNT = 4,
    ARMY_DRAW_ALL         = 100
H2_ENUM_CLASS_END(ArmyDrawState)
H2_ENUM_STEPPED(ArmyDrawState)

H2_ENUM_BEGIN(CombatHexDirection)
    COMBAT_DIRECTION_INVALID         = -1,
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
H2_ENUM_STEPPED(CombatHexDirection)

#endif
