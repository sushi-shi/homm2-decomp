#ifndef HOMM2_EDITOR_EDITOR_H
#define HOMM2_EDITOR_EDITOR_H

// The scenario editor's program unit (src/EDITOR/EDITOR.cpp): start-up and
// shut-down, the main classes, the delay and dialog helpers, the status bar
// and the application-menu hooks kbwin calls. It began as a copy of the
// game's KB.cpp and keeps its names for what both programs define.

#include <va.h>
#include <Ints.h>
#include <EDITOR/fullMap.h>

H2_ENUM_BEGIN(EditorStatusBar)
    // The status line under the map view.
    EDITOR_STATUS_BAR_X                 = 0,
    EDITOR_STATUS_BAR_Y                 = 0x1d0,
    EDITOR_STATUS_BAR_WIDTH             = 0x1e0,
    EDITOR_STATUS_BAR_HEIGHT            = 0x10,
    EDITOR_STATUS_TEXT_Y                = 0x1d3,
    EDITOR_STATUS_TEXT_HOLD_MILLISECONDS = 3000,
    EDITOR_STATUS_TEXT_SIZE             = 200,
    // ClearStatusText resets the pending clear time to this.
    EDITOR_STATUS_TEXT_KEPT             = 0
H2_ENUM_END(EditorStatusBar)

H2_ENUM_BEGIN(EditorFileConstant)
    EDITOR_MAP_FILE_NAME_SIZE   = 16,
    // gClearFlags: every eraser layer selected.
    EDITOR_CLEAR_FLAGS_DEFAULT  = 0x3fff
H2_ENUM_END(EditorFileConstant)

H2_ENUM_BEGIN(EditorMapCopy)
    // gMaps: the edited map and the copy SaveUndo keeps.
    EDIT_MAP_CURRENT = 0,
    EDIT_MAP_UNDO    = 1,
    EDIT_MAP_COPIES  = 2
H2_ENUM_END(EditorMapCopy)

H2_ENUM_BEGIN(EditorShippedMap)
    // The maps shipped with the game: ProtectShippedMap gives an edited copy
    // an underscored file name and map name. Each entry holds the shipped
    // file name and its replacement.
    EDITOR_SHIPPED_MAP_COUNT     = 36,
    EDITOR_SHIPPED_MAP_NAME_SIZE = 13,
    EDITOR_SHIPPED_MAP_NAMES     = 2
H2_ENUM_END(EditorShippedMap)

extern fullMap gMaps[EDIT_MAP_COPIES];
#define gMap (gMaps[EDIT_MAP_CURRENT])
#define gUndoMap (gMaps[EDIT_MAP_UNDO])

// The map file being edited (an 8.3 name).
extern char gMapFileName[EDITOR_MAP_FILE_NAME_SIZE];
extern char gShippedMaps[EDITOR_SHIPPED_MAP_COUNT][EDITOR_SHIPPED_MAP_NAMES]
                        [EDITOR_SHIPPED_MAP_NAME_SIZE];
extern i32 gClearFlags;
extern char gStatusText[EDITOR_STATUS_TEXT_SIZE];
extern b32 gStatusTextShown;
extern i32 gStatusTextHoldTime;
extern i32 gStatusTextClearTime;

void ProtectShippedMap(void);
void IncrementArgumentA(i32 value);
void EditorIdleHook(void);
void IncrementArgumentB(i32 value);
void DelayTicks(i32 ticks);
void ShowStatusText(char* text);
void ClearStatusText(void);

#endif
