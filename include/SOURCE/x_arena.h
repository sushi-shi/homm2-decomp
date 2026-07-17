#ifndef HOMM2_X_ARENA_H
#define HOMM2_X_ARENA_H

#include <Ints.h>
// Declarations of the free functions DEFINED in x_arena.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

H2_ENUM_BEGIN(ArenaConstant)
    ARENA_CHOICE_COUNT = 3,
    ARENA_WINDOW_RESOURCE = 5,
    ARENA_WINDOW_WIDTH = 306,
    ARENA_WINDOW_BASE_HEIGHT = 180,
    ARENA_WINDOW_ROW_HEIGHT = 45,
    ARENA_WINDOW_X = 159,
    ARENA_WINDOW_SCREEN_HEIGHT = 480,
    ARENA_WINDOW_MAX_Y = 28,
    ARENA_WINDOW_NAME_SIZE = 16,
    ARENA_TEXT_WIDTH = 244,
    ARENA_TEXT_LINE_HEIGHT = 16,
    ARENA_TEXT_LINE_SHIFT = 4,
    ARENA_ICON_FIRST_X = 84,
    ARENA_TEXT_FIRST_X = 79,
    ARENA_WIDGET_X_STEP = 60,
    ARENA_ICON_Y = 244,
    ARENA_ICON_WIDTH = 39,
    ARENA_ICON_HEIGHT = 34,
    ARENA_TEXT_Y = 282,
    ARENA_TEXT_WIDTH_PIXELS = 49,
    ARENA_TEXT_HEIGHT = 24,
    ARENA_WIDGET_FIRST_ID = 100,
    ARENA_WIDGET_LAST_ID = 102,
    ARENA_SELECTED_FRAME_OFFSET = 4,
    ARENA_ICON_FILL_COLOR = 0x10,
    ARENA_TEXT_COLOR = 1,
    ARENA_TEXT_BACKGROUND = -1,
    ARENA_TEXT_FLAGS = 0x200,
    ARENA_CYCLE_KEY = 15,
    ARENA_BROADCAST_TEXT = 3,
    ARENA_BROADCAST_CONTROL = 6,
    ARENA_BROADCAST_TEXT_ID = 1,
    ARENA_BROADCAST_CONTROL_VALUE = 6,
    ARENA_DRAW_MODE = 1
H2_ENUM_END(ArenaConstant)

i32 DoArenaDialog(void);
i32 ArenaWindowHandler(struct tag_message&);
void UpdateArenaIcons(void);

// ---- globals (declarations, RVA order) ----
extern i32 choice;
extern class iconWidget* skillWidget[3];
extern class heroWindow* arenaWinPtr;

#endif // HOMM2_X_ARENA_H
