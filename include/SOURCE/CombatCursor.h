#ifndef HOMM2_SOURCE_COMBATCURSOR_H
#define HOMM2_SOURCE_COMBATCURSOR_H

#include <Ints.h>
#include <SOURCE/combatTypes.h>

enum class CombatMessageCommand : i32 {
    COMBAT_INVALID_COMMAND                    = -99,
    COMBAT_MESSAGE_COMMAND_DEFAULT            = 0,
    COMBAT_MESSAGE_COMMAND_MOVE               = 1,
    COMBAT_MESSAGE_COMMAND_FLY                = 2,
    COMBAT_MESSAGE_COMMAND_SHOOT              = 3,
    COMBAT_MESSAGE_COMMAND_OPTIONS            = 4,
    COMBAT_MESSAGE_COMMAND_VIEW_INFO          = 5,
    COMBAT_MESSAGE_COMMAND_ATTACK             = 7,
    COMBAT_MESSAGE_COMMAND_CAST_SPELL         = 10,
    COMBAT_MESSAGE_COMMAND_RETREAT            = 11,
    COMBAT_MESSAGE_COMMAND_SURRENDER          = 12,
    COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS   = 13,
    COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL = 15
};
using enum CombatMessageCommand;

// CMSECO.ICN is an artwork domain, not a command protocol. Retail deliberately
// assigned many commands the same numbers as their cursor frames, but keeping
// that correspondence explicit prevents either domain from silently changing
// the other.
enum class CombatCursorFrame : i32 {
    COMBAT_CURSOR_INVALID                = -99,
    COMBAT_CURSOR_TARGET_INVALID         = 0,
    COMBAT_CURSOR_MOVE                   = 1,
    COMBAT_CURSOR_FLY                    = 2,
    COMBAT_CURSOR_SHOOT                  = 3,
    COMBAT_CURSOR_OPTIONS                = 4,
    COMBAT_CURSOR_VIEW                   = 5,
    COMBAT_CURSOR_DEFAULT                = 6,
    COMBAT_CURSOR_ATTACK_NORTHEAST       = 7,
    COMBAT_CURSOR_ATTACK_EAST            = 8,
    COMBAT_CURSOR_ATTACK_SOUTHEAST       = 9,
    COMBAT_CURSOR_ATTACK_SOUTHWEST       = 10,
    COMBAT_CURSOR_ATTACK_WEST            = 11,
    COMBAT_CURSOR_ATTACK_NORTHWEST       = 12,
    COMBAT_CURSOR_ATTACK_WIDE_WEST       = 13,
    COMBAT_CURSOR_ATTACK_WIDE_EAST       = 14,
    COMBAT_CURSOR_SHOOT_THROUGH_WALL     = 15
};
using enum CombatCursorFrame;

constexpr CombatCursorFrame CombatCursorForCommand(CombatMessageCommand command) {
    switch (command) {
        case COMBAT_INVALID_COMMAND: return COMBAT_CURSOR_INVALID;
        case COMBAT_MESSAGE_COMMAND_DEFAULT: return COMBAT_CURSOR_TARGET_INVALID;
        case COMBAT_MESSAGE_COMMAND_MOVE: return COMBAT_CURSOR_MOVE;
        case COMBAT_MESSAGE_COMMAND_FLY: return COMBAT_CURSOR_FLY;
        case COMBAT_MESSAGE_COMMAND_SHOOT: return COMBAT_CURSOR_SHOOT;
        case COMBAT_MESSAGE_COMMAND_OPTIONS: return COMBAT_CURSOR_OPTIONS;
        case COMBAT_MESSAGE_COMMAND_VIEW_INFO: return COMBAT_CURSOR_VIEW;
        case COMBAT_MESSAGE_COMMAND_ATTACK: return COMBAT_CURSOR_ATTACK_NORTHEAST;
        case COMBAT_MESSAGE_COMMAND_CAST_SPELL: return COMBAT_CURSOR_ATTACK_SOUTHWEST;
        case COMBAT_MESSAGE_COMMAND_RETREAT: return COMBAT_CURSOR_ATTACK_WEST;
        case COMBAT_MESSAGE_COMMAND_SURRENDER: return COMBAT_CURSOR_ATTACK_NORTHWEST;
        case COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS: return COMBAT_CURSOR_VIEW;
        case COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL:
            return COMBAT_CURSOR_SHOOT_THROUGH_WALL;
    }
    return COMBAT_CURSOR_INVALID;
}

constexpr CombatCursorFrame CombatCursorForAttackDirection(CombatHexDirection direction) {
    switch (direction) {
        case COMBAT_DIRECTION_NORTHEAST: return COMBAT_CURSOR_ATTACK_NORTHEAST;
        case COMBAT_DIRECTION_EAST: return COMBAT_CURSOR_ATTACK_EAST;
        case COMBAT_DIRECTION_SOUTHEAST: return COMBAT_CURSOR_ATTACK_SOUTHEAST;
        case COMBAT_DIRECTION_SOUTHWEST: return COMBAT_CURSOR_ATTACK_SOUTHWEST;
        case COMBAT_DIRECTION_WEST: return COMBAT_CURSOR_ATTACK_WEST;
        case COMBAT_DIRECTION_NORTHWEST: return COMBAT_CURSOR_ATTACK_NORTHWEST;
        case COMBAT_DIRECTION_WIDE_WEST: return COMBAT_CURSOR_ATTACK_WIDE_WEST;
        case COMBAT_DIRECTION_WIDE_EAST: return COMBAT_CURSOR_ATTACK_WIDE_EAST;
        case COMBAT_DIRECTION_INVALID: return COMBAT_CURSOR_INVALID;
    }
    return COMBAT_CURSOR_INVALID;
}

#endif
