#ifndef HOMM2_SOURCE_COMBATTYPES_H
#define HOMM2_SOURCE_COMBATTYPES_H

#include <va.h>

H2_ENUM_CLASS_BEGIN_SPLIT(CombatSide, i8)
    COMBAT_SIDE_NONE     = -1,
    COMBAT_ATTACKER_SIDE = 0,
    COMBAT_DEFENDER_SIDE = 1
H2_ENUM_CLASS_END_SPLIT(CombatSide, i8)
H2_ENUM_STEPPED(CombatSide)

H2_ENUM_BEGIN(CombatSideConstant)
    COMBAT_SIDE_COUNT = 2
H2_ENUM_END(CombatSideConstant)

inline CombatSide OppositeCombatSide(CombatSide side) {
#ifdef HOMM2_STRICT_ENUM_TYPES
    return side == COMBAT_ATTACKER_SIDE ? COMBAT_DEFENDER_SIDE : COMBAT_ATTACKER_SIDE;
#else
    return COMBAT_DEFENDER_SIDE - side;
#endif
}

H2_ENUM_CLASS_BEGIN_SPLIT(CombatResult, i8)
    COMBAT_RESULT_DRAW     = -1,
    COMBAT_RESULT_ATTACKER = 0,
    COMBAT_RESULT_DEFENDER = 1,
    COMBAT_RESULT_PENDING  = 3
H2_ENUM_CLASS_END_SPLIT(CombatResult, i8)

inline CombatResult CombatResultForSide(CombatSide side) {
#ifdef HOMM2_STRICT_ENUM_TYPES
    return side == COMBAT_ATTACKER_SIDE ? COMBAT_RESULT_ATTACKER : COMBAT_RESULT_DEFENDER;
#else
    return side;
#endif
}

inline CombatResult OppositeCombatResult(CombatResult result) {
#ifdef HOMM2_STRICT_ENUM_TYPES
    return result == COMBAT_RESULT_ATTACKER ? COMBAT_RESULT_DEFENDER : COMBAT_RESULT_ATTACKER;
#else
    return COMBAT_RESULT_DEFENDER - result;
#endif
}

H2_ENUM_CLASS_BEGIN_SPLIT(ArmyFacing, i8)
    ARMY_FACING_NONE  = -1,
    ARMY_FACING_LEFT  = 0,
    ARMY_FACING_RIGHT = 1,
    ARMY_FACING_COUNT = 2
H2_ENUM_CLASS_END_SPLIT(ArmyFacing, i8)

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

H2_ENUM_CLASS_BEGIN(ArmyPathTarget)
    COMBAT_AI_PATH_TO_TARGET   = -1,
    ARMY_PATH_ANY_TARGET_HEX   = 0,
    ARMY_PATH_EXACT_TARGET_HEX = 1,
    COMBAT_AI_PATH_TO_FRONT    = ARMY_PATH_EXACT_TARGET_HEX
H2_ENUM_CLASS_END(ArmyPathTarget)

#endif
