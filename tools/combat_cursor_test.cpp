#include <SOURCE/CONFIG_TYPES.h>
#include <SOURCE/CombatCursor.h>

static_assert(CONFIG_DEFAULT_COLOR_MOUSE_CURSOR);
static_assert(CombatCursorForCommand(COMBAT_MESSAGE_COMMAND_DEFAULT)
              == COMBAT_CURSOR_TARGET_INVALID);
static_assert(CombatCursorForCommand(COMBAT_MESSAGE_COMMAND_MOVE) == COMBAT_CURSOR_MOVE);
static_assert(CombatCursorForCommand(COMBAT_MESSAGE_COMMAND_FLY) == COMBAT_CURSOR_FLY);
static_assert(CombatCursorForCommand(COMBAT_MESSAGE_COMMAND_SHOOT) == COMBAT_CURSOR_SHOOT);
static_assert(CombatCursorForCommand(COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL)
              == COMBAT_CURSOR_SHOOT_THROUGH_WALL);
static_assert(CombatCursorForCommand(COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS)
              == COMBAT_CURSOR_VIEW);
static_assert(CombatCursorForAttackDirection(COMBAT_DIRECTION_NORTHEAST)
              == COMBAT_CURSOR_ATTACK_NORTHEAST);
static_assert(CombatCursorForAttackDirection(COMBAT_DIRECTION_WIDE_EAST)
              == COMBAT_CURSOR_ATTACK_WIDE_EAST);

int main() {
    return 0;
}
