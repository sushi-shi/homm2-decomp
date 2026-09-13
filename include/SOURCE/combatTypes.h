#ifndef HOMM2_SOURCE_COMBATTYPES_H
#define HOMM2_SOURCE_COMBATTYPES_H

#include <Ints.h>

enum {
    COMBAT_SIDE_NONE        = -1,
    COMBAT_SIDE_VALID_BEGIN = 0,
    COMBAT_ATTACKER_SIDE    = COMBAT_SIDE_VALID_BEGIN,
    COMBAT_DEFENDER_SIDE    = 1
};
typedef i32 CombatSide;

typedef enum CombatSideConstant {
    COMBAT_SIDE_COUNT = 2
} CombatSideConstant;


inline CombatSide OppositeCombatSide(CombatSide side) {
    return side == COMBAT_ATTACKER_SIDE ? COMBAT_DEFENDER_SIDE : COMBAT_ATTACKER_SIDE;
}

enum {
    COMBAT_RESULT_DRAW     = -1,
    COMBAT_RESULT_ATTACKER = 0,
    COMBAT_RESULT_DEFENDER = 1,
    COMBAT_RESULT_PENDING  = 3
};
typedef i32 CombatResult;
inline CombatResult CombatResultForSide(CombatSide side) {
    return side == COMBAT_ATTACKER_SIDE ? COMBAT_RESULT_ATTACKER : COMBAT_RESULT_DEFENDER;
}

inline CombatResult OppositeCombatResult(CombatResult result) {
    return result == COMBAT_RESULT_ATTACKER ? COMBAT_RESULT_DEFENDER : COMBAT_RESULT_ATTACKER;
}

inline CombatSide CombatSideForResult(CombatResult result) {
    return result == COMBAT_RESULT_ATTACKER ? COMBAT_ATTACKER_SIDE : COMBAT_DEFENDER_SIDE;
}

enum {
    ARMY_FACING_NONE  = -1,
    ARMY_FACING_LEFT  = 0,
    ARMY_FACING_RIGHT = 1,
    ARMY_FACING_COUNT = 2
};
typedef i32 ArmyFacing;
inline ArmyFacing OppositeArmyFacing(ArmyFacing facing) {
    return facing == ARMY_FACING_RIGHT ? ARMY_FACING_LEFT : ARMY_FACING_RIGHT;
}

inline ArmyFacing ArmyFacingForSide(CombatSide side) {
    return side == COMBAT_ATTACKER_SIDE ? ARMY_FACING_RIGHT : ARMY_FACING_LEFT;
}

inline i32 ArmyFacingRearHexOffset(ArmyFacing facing) {
    return facing == ARMY_FACING_RIGHT ? 1 : -1;
}

enum {
    ARMY_DRAW_BEHIND      = 0,
    ARMY_DRAW_NORMAL      = 1,
    ARMY_DRAW_EFFECT      = 2,
    ARMY_DRAW_IN_FRONT    = 3,
    ARMY_DRAW_PHASE_COUNT = 4,
    ARMY_DRAW_ALL         = 100
};
typedef i32 ArmyDrawState;

enum {
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
typedef i32 CombatHexDirection;

typedef enum CombatHexDirectionConstant {
    COMBAT_DIRECTION_OPPOSITE_OFFSET = 3,
    COMBAT_DIRECTION_ADJACENT_COUNT  = 6,
    COMBAT_DIRECTION_COUNT           = 8
} CombatHexDirectionConstant;

enum {
    COMBAT_CASTLE_HEX_NONE           = -1,
    COMBAT_CASTLE_HEX_TOP_TOWER      = 9,
    COMBAT_CASTLE_HEX_TOP_WALL       = 22,
    COMBAT_CASTLE_HEX_SECOND_TOWER   = 34,
    COMBAT_CASTLE_HEX_SECOND_WALL    = 47,
    COMBAT_CASTLE_HEX_GATE           = 59,
    COMBAT_CASTLE_HEX_THIRD_WALL     = 73,
    COMBAT_CASTLE_HEX_KEEP           = 77,
    COMBAT_CASTLE_HEX_THIRD_TOWER    = 86,
    COMBAT_CASTLE_HEX_MOAT           = 92,
    COMBAT_CASTLE_HEX_BOTTOM_WALL    = 100,
    COMBAT_CASTLE_HEX_BOTTOM_TOWER   = 113,
    COMBAT_CASTLE_SPECIAL_HEX_FIRST  = 114,
    COMBAT_CASTLE_SPECIAL_HEX_SECOND = 115
};
typedef i32 CombatCastleHex;
enum {
    ARMY_PATH_ASSIGNED_TARGET_HEX = -1,
    ARMY_PATH_ANY_TARGET_HEX      = 0,
    ARMY_PATH_EXACT_TARGET_HEX    = 1,
};
typedef i32 ArmyPathTarget;
typedef enum CombatGeometryConstant {
    COMBAT_GRID_ROW_LENGTH = 13,
    COMBAT_AREA_HEIGHT     = 443,
    COMBAT_MAX_EXTENT_Y    = 442,
    COMBAT_HEX_COUNT       = 117,
} CombatGeometryConstant;

typedef enum CombatStorageConstant {
    COMBAT_WIN_LOSE_WIDGET_COUNT  = 25,
    COMBAT_DEAD_OCCUPANT_CAPACITY = 14,
} CombatStorageConstant;

typedef enum CombatElevationOverlayConstant {
    COMBAT_ELEVATION_OVERLAY_COUNT      = 25,
    COMBAT_ELEVATION_OVERLAY_CELL_COUNT = 15,
} CombatElevationOverlayConstant;

typedef enum CombatMissileAnimationConstant {
    COMBAT_MISSILE_HALF_WIDTH  = 25,
    COMBAT_MISSILE_HALF_HEIGHT = 25,
    COMBAT_MISSILE_SPACING     = 31,
    COMBAT_MISSILE_TIMER_DELAY = 25,
} CombatMissileAnimationConstant;

typedef enum CombatDirectionMask {

    COMBAT_ALL_DIRECTIONS_BLOCKED = 255,
} CombatDirectionMask;

#endif
