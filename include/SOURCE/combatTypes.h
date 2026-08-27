#ifndef HOMM2_SOURCE_COMBATTYPES_H
#define HOMM2_SOURCE_COMBATTYPES_H

#include <Ints.h>
#include <EnumCode.h>

enum class CombatSide : i8 {
    COMBAT_SIDE_NONE        = -1,
    COMBAT_SIDE_VALID_BEGIN = 0,
    COMBAT_ATTACKER_SIDE    = COMBAT_SIDE_VALID_BEGIN,
    COMBAT_DEFENDER_SIDE    = 1
};
using enum CombatSide;
ENABLE_ENUM_STEPS(CombatSide)

typedef enum CombatSideConstant {
    COMBAT_SIDE_COUNT = 2
} CombatSideConstant;

inline CombatSide& operator^=(CombatSide& side, i32 mask) {
    side = CombatSideFromCode(static_cast<i32>(side) ^ mask);
    return side;
}

inline CombatSide OppositeCombatSide(CombatSide side) {
    return side == COMBAT_ATTACKER_SIDE ? COMBAT_DEFENDER_SIDE : COMBAT_ATTACKER_SIDE;
}

inline CombatSide CombatSideForDefender(bool defender) {
    return defender ? COMBAT_DEFENDER_SIDE : COMBAT_ATTACKER_SIDE;
}

enum class CombatResult : i8 {
    COMBAT_RESULT_DRAW     = -1,
    COMBAT_RESULT_ATTACKER = 0,
    COMBAT_RESULT_DEFENDER = 1,
    COMBAT_RESULT_PENDING  = 3
};
using enum CombatResult;

inline CombatResult CombatResultForSide(CombatSide side) {
    return side == COMBAT_ATTACKER_SIDE ? COMBAT_RESULT_ATTACKER : COMBAT_RESULT_DEFENDER;
}

inline CombatResult OppositeCombatResult(CombatResult result) {
    return result == COMBAT_RESULT_ATTACKER ? COMBAT_RESULT_DEFENDER : COMBAT_RESULT_ATTACKER;
}

inline CombatSide CombatSideForResult(CombatResult result) {
    return result == COMBAT_RESULT_DEFENDER ? COMBAT_DEFENDER_SIDE : COMBAT_ATTACKER_SIDE;
}

enum class ArmyFacing : i8 {
    ARMY_FACING_NONE  = -1,
    ARMY_FACING_LEFT  = 0,
    ARMY_FACING_RIGHT = 1,
    ARMY_FACING_COUNT = 2
};
using enum ArmyFacing;

inline ArmyFacing OppositeArmyFacing(ArmyFacing facing) {
    return facing == ARMY_FACING_RIGHT ? ARMY_FACING_LEFT : ARMY_FACING_RIGHT;
}

inline ArmyFacing ArmyFacingForSide(CombatSide side) {
    return side == COMBAT_ATTACKER_SIDE ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
}

inline i32 ArmyFacingRearHexOffset(ArmyFacing facing) {
    return facing == ARMY_FACING_RIGHT ? 1 : -1;
}

enum class ArmyDrawState : i32 {
    ARMY_DRAW_BEHIND      = 0,
    ARMY_DRAW_NORMAL      = 1,
    ARMY_DRAW_EFFECT      = 2,
    ARMY_DRAW_IN_FRONT    = 3,
    ARMY_DRAW_PHASE_COUNT = 4,
    ARMY_DRAW_ALL         = 100
};
using enum ArmyDrawState;
ENABLE_ENUM_STEPS(ArmyDrawState)

enum class CombatHexDirection : i32 {
    COMBAT_DIRECTION_INVALID   = -1,
    COMBAT_DIRECTION_NORTHEAST = 0,
    COMBAT_DIRECTION_EAST      = 1,
    COMBAT_DIRECTION_SOUTHEAST = 2,
    COMBAT_DIRECTION_SOUTHWEST = 3,
    COMBAT_DIRECTION_WEST      = 4,
    COMBAT_DIRECTION_NORTHWEST = 5,
    COMBAT_DIRECTION_WIDE_WEST = 6,
    COMBAT_DIRECTION_WIDE_EAST = 7
};
using enum CombatHexDirection;
ENABLE_ENUM_STEPS(CombatHexDirection)

typedef enum CombatHexDirectionConstant {
    COMBAT_DIRECTION_OPPOSITE_OFFSET = 3,
    COMBAT_DIRECTION_ADJACENT_COUNT  = 6,
    COMBAT_DIRECTION_COUNT           = 8
} CombatHexDirectionConstant;

enum class ArmyPathTarget : i32 {
    COMBAT_AI_PATH_TO_TARGET   = -1,
    ARMY_PATH_ANY_TARGET_HEX   = 0,
    ARMY_PATH_EXACT_TARGET_HEX = 1,
    COMBAT_AI_PATH_TO_FRONT    = ARMY_PATH_EXACT_TARGET_HEX
};
using enum ArmyPathTarget;

#endif
