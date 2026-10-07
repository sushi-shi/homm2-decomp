// The scenario editor's program unit: start-up and shut-down, the main
// classes, the delay and dialog helpers, the status bar and the
// application-menu hooks kbwin calls. It began as a copy of the game's KB.cpp
// and keeps its names for what both programs define; its .data opens with
// KB.cpp's tables in KB.cpp's order.
// Descriptive names: ProtectShippedMap, IncrementArgumentA,
// IncrementArgumentB, EditorIdleHook, DelayTicks, ShowStatusText,
// ClearStatusText, gMaps, gMapFileName, gStatusText,
// gStatusTextShown, gStatusTextHoldTime, gStatusTextClearTime,
// gCommandLineInterpreted, gShowMapInfo, gClearFlags, gObjectClass,
// gGenerateUnseen, gGeneratingMap, gRandomMapPlayers, the gUnusedData
// holders of unreferenced retail storage, and the editor table names.

#include <va.h>
#include <EDITOR/EDITOR.h>
#include <BASE/MiscEnums.h>
#include <EDITOR/clearManager.h>
#include <EDITOR/editManager.h>
#include <EDITOR/EVENTMGR.h>
#include <EDITOR/lineManager.h>
#include <EDITOR/setup.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>
#include <BASE/Misc.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <BASE/dialog.h>
#include <BASE/executive.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/mouseManager.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <BASE/widget.h>
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

H2_ENUM_BEGIN(EditorStartupConstant)
    // DelayTicks counts in 15-millisecond ticks.
    EDITOR_DELAY_TICK_MILLISECONDS = 15,
    EDITOR_DELAY_TIMER_SLOT     = 1,
    EDITOR_MOUSE_UPDATE_INTERVAL = 13,
    EDITOR_COLOR_CYCLE_INTERVAL = 200,
    EDITOR_NON_PALETTED_CYCLE_DELAY = 300,
    // ShutDown's message buffer and FileError's.
    EDITOR_SHUTDOWN_TEXT_SIZE   = 768,
    EDITOR_FILE_ERROR_TEXT_SIZE = 200
H2_ENUM_END(EditorStartupConstant)

H2_ENUM_BEGIN(EditorMenuCommand)
    // The editor's own option toggles in the system menu.
    EDITOR_MENU_PALETTE_CYCLING = 0x9cd1,
    EDITOR_MENU_SCREEN_ANIMATION = 0x9cd2,
    EDITOR_MENU_OBJECT_BOXES    = 0x9cd3
H2_ENUM_END(EditorMenuCommand)

H2_ENUM_BEGIN(EditorNormalDialogConstant)
    // The editor's message boxes: narrower than the game's and without
    // resource panels.
    EDITOR_DIALOG_TEXT_LINE_WIDTH  = 0xf0,
    EDITOR_DIALOG_BUTTON_AREA      = 0x27,
    EDITOR_DIALOG_ROW_OFFSET       = 12,
    EDITOR_DIALOG_WINDOW_WIDTH     = 0x11e,
    EDITOR_DIALOG_WINDOW_BASE      = 0x81,
    EDITOR_DIALOG_DEFAULT_X        = 0x9f,
H2_ENUM_END(EditorNormalDialogConstant)

// KB.cpp's tables, in KB.cpp's order.
#define GROUND_REPEAT_2(value) value, value
#define GROUND_REPEAT_4(value) GROUND_REPEAT_2(value), GROUND_REPEAT_2(value)
#define GROUND_REPEAT_8(value) GROUND_REPEAT_4(value), GROUND_REPEAT_4(value)
#define GROUND_REPEAT_16(value) GROUND_REPEAT_8(value), GROUND_REPEAT_8(value)
#define GROUND_REPEAT_32(value) GROUND_REPEAT_16(value), GROUND_REPEAT_16(value)
#define GROUND_SHAPE_STANDARD_FRAME_SET                                                            \
    GROUND_REPEAT_4(1), GROUND_REPEAT_4(2), GROUND_REPEAT_4(3), GROUND_REPEAT_4(4),                \
        GROUND_REPEAT_4(5), GROUND_REPEAT_4(6), GROUND_REPEAT_4(7), GROUND_REPEAT_4(8), 10, 11,    \
        12, 13, 14, 15, GROUND_REPEAT_8(0)

DATA(0x0047e1c8) H2_ENUM_STORAGE(TerrainType, u8)
giGroundToTerrain[GROUND_TILE_IMAGE_COUNT] = {
    GROUND_REPEAT_16(TERRAIN_WATER),
    GROUND_REPEAT_8(TERRAIN_WATER),
    GROUND_REPEAT_4(TERRAIN_WATER),
    GROUND_REPEAT_2(TERRAIN_WATER),
    GROUND_REPEAT_32(TERRAIN_GRASS),
    GROUND_REPEAT_16(TERRAIN_GRASS),
    GROUND_REPEAT_8(TERRAIN_GRASS),
    GROUND_REPEAT_4(TERRAIN_GRASS),
    GROUND_REPEAT_2(TERRAIN_GRASS),
    GROUND_REPEAT_32(TERRAIN_SNOW),
    GROUND_REPEAT_16(TERRAIN_SNOW),
    GROUND_REPEAT_4(TERRAIN_SNOW),
    GROUND_REPEAT_2(TERRAIN_SNOW),
    GROUND_REPEAT_32(TERRAIN_SWAMP),
    GROUND_REPEAT_16(TERRAIN_SWAMP),
    GROUND_REPEAT_8(TERRAIN_SWAMP),
    GROUND_REPEAT_4(TERRAIN_SWAMP),
    GROUND_REPEAT_2(TERRAIN_SWAMP),
    GROUND_REPEAT_32(TERRAIN_LAVA),
    GROUND_REPEAT_16(TERRAIN_LAVA),
    GROUND_REPEAT_4(TERRAIN_LAVA),
    GROUND_REPEAT_2(TERRAIN_LAVA),
    GROUND_REPEAT_32(TERRAIN_DESERT),
    GROUND_REPEAT_16(TERRAIN_DESERT),
    GROUND_REPEAT_8(TERRAIN_DESERT),
    GROUND_REPEAT_2(TERRAIN_DESERT),
    TERRAIN_DESERT,
    GROUND_REPEAT_32(TERRAIN_DIRT),
    GROUND_REPEAT_8(TERRAIN_DIRT),
    GROUND_REPEAT_32(TERRAIN_WASTELAND),
    GROUND_REPEAT_16(TERRAIN_WASTELAND),
    GROUND_REPEAT_4(TERRAIN_WASTELAND),
    GROUND_REPEAT_2(TERRAIN_WASTELAND),
    GROUND_REPEAT_16(TERRAIN_BEACH),
    TERRAIN_BEACH
};
DATA(0x0047e378) u8 giGroundShape[GROUND_TILE_IMAGE_COUNT] = {
    GROUND_REPEAT_2(16),
    GROUND_REPEAT_2(1),
    GROUND_REPEAT_4(2),
    GROUND_REPEAT_2(17),
    GROUND_REPEAT_2(3),
    GROUND_REPEAT_4(4),
    GROUND_REPEAT_4(0),
    GROUND_REPEAT_4(18),
    GROUND_REPEAT_2(20),
    GROUND_REPEAT_2(21),
    GROUND_REPEAT_2(19),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_16(GROUND_SHAPE_VARIED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_VARIED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_16(GROUND_SHAPE_VARIED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_VARIED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_VARIED),
    GROUND_REPEAT_4(GROUND_SHAPE_VARIED),
    GROUND_SHAPE_VARIED,
    GROUND_REPEAT_4(5),
    GROUND_REPEAT_4(6),
    GROUND_REPEAT_4(7),
    GROUND_REPEAT_4(8),
    GROUND_REPEAT_8(0),
    GROUND_REPEAT_16(GROUND_SHAPE_VARIED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_VARIED),
    GROUND_REPEAT_8(0),
    GROUND_REPEAT_8(GROUND_SHAPE_VARIED),
    GROUND_SHAPE_VARIED
};

#undef GROUND_SHAPE_STANDARD_FRAME_SET
#undef GROUND_REPEAT_32
#undef GROUND_REPEAT_16
#undef GROUND_REPEAT_8
#undef GROUND_REPEAT_4
#undef GROUND_REPEAT_2

DATA(0x0047e528) u8 gColorTableTan[PALETTE_COLOR_COUNT] = {
    0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc8, 0xc9, 0xcb, 0xcc, 0xce, 0xcf, 0xd0, 0xd1, 0xd2,
    0xd3, 0xd5, 0xd5, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc7, 0xc8, 0xc9, 0xcb, 0xcc, 0xcd, 0xcf, 0xcf, 0xd1, 0xd2, 0xd3, 0xd4, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc9, 0xcb, 0xcd, 0xcf, 0xd0, 0xd2, 0xd2, 0xd3, 0xd4, 0xd4, 0xd5, 0xd5,
    0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc7, 0xc9, 0xcb, 0xcc, 0xce, 0xcf, 0xd0, 0xd1,
    0xd2, 0xd3, 0xd4, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc8, 0xc9, 0xcb,
    0xcc, 0xce, 0xcf, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc8, 0xc9, 0xcb, 0xcc,
    0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd4, 0xd5, 0xd5, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc7, 0xc9, 0xcb, 0xcc, 0xce, 0xcf, 0xd0, 0xd1, 0xd3, 0xd4, 0xd5, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc7, 0xc9, 0xca, 0xcc, 0xce, 0xcf, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc6, 0xc6, 0xc7, 0xca, 0xcd, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6,
    0xc6, 0xc8, 0xcb, 0xcc, 0xcf, 0xd0, 0xd1, 0xc9, 0xcb, 0xcf, 0xd1, 0xce, 0xd1, 0xd0, 0xc6, 0xc6,
    0xcf, 0xd5, 0xc6, 0xc9, 0xce, 0xd0, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5, 0xd5
};
DATA(0x0047e628) u8 gColorTableGray[PALETTE_COLOR_COUNT] = {
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x0a, 0x0b, 0x0c, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x12,
    0x13, 0x14, 0x14, 0x15, 0x16, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1a, 0x1b, 0x1c, 0x1d, 0x1f, 0x0e,
    0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1a, 0x1c, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x20,
    0x21, 0x21, 0x21, 0x22, 0x22, 0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
    0x1c, 0x1e, 0x1f, 0x20, 0x20, 0x21, 0x22, 0x23, 0x23, 0x24, 0x24, 0x24, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x0c, 0x0d, 0x0e, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x18, 0x19,
    0x1a, 0x1b, 0x1c, 0x1d, 0x1f, 0x20, 0x20, 0x21, 0x0b, 0x0c, 0x0c, 0x0d, 0x0e, 0x0e, 0x10, 0x10,
    0x11, 0x12, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1d, 0x1e, 0x20, 0x0a,
    0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x1a, 0x0a, 0x0a, 0x0b, 0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0c, 0x0e,
    0x10, 0x11, 0x12, 0x14, 0x16, 0x18, 0x11, 0x0a, 0x0c, 0x0f, 0x13, 0x0a, 0x0a, 0x0f, 0x11, 0x12,
    0x14, 0x15, 0x16, 0x17, 0x19, 0x1a, 0x1b, 0x1b, 0x18, 0x15, 0x16, 0x1a, 0x1a, 0x1b, 0x24, 0x0c,
    0x12, 0x19, 0x13, 0x15, 0x18, 0x1a, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24
};
DATA(0x0047e728) u8 gColorTableYellow[PALETTE_COLOR_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x73, 0x73, 0x74, 0x75, 0x75,
    0x76, 0x77, 0x77, 0x78, 0x79, 0x79, 0x7a, 0x7b, 0x7b, 0x7c, 0x7d, 0x7d, 0x7e, 0x7f, 0x7f, 0x80,
    0x81, 0x81, 0x82, 0x82, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
DATA(0x0047e828) u8 gColorTableScenWin[PALETTE_COLOR_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
DATA(0x0047e928) u8 gColorTableDarkGray[PALETTE_COLOR_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x24, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
DATA(0x0047ea28) u8 gColorTableRed[PALETTE_COLOR_COUNT] = {
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xb4, 0xb6, 0xb8, 0xba, 0xd0, 0xd1,
    0xd2, 0xd2, 0xd3, 0xd3, 0xd4, 0xd5, 0xd5, 0xc4, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5,
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xb4, 0xb4, 0xb6, 0xb6, 0xb8, 0xba, 0xd0, 0xd1, 0xd1, 0xd2, 0xd2,
    0xd2, 0xd3, 0xd3, 0xc1, 0xd4, 0xd4, 0xd5, 0xd5, 0xc4, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xd0,
    0xd1, 0xd2, 0xd3, 0xc1, 0xd4, 0xd5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5,
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xd1, 0xd2, 0xd2, 0xd3, 0xd4, 0xd4, 0xd5, 0xc4, 0xc5, 0xc5, 0xc5,
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xb4, 0xb4, 0xb4, 0xb4,
    0xb4, 0xb6, 0xb6, 0xb7, 0xb8, 0xb9, 0xd0, 0xbc, 0xbd, 0xbe, 0xd2, 0xbf, 0xd3, 0xc1, 0xc1, 0xc2,
    0xc3, 0xc4, 0xc5, 0xb6, 0xb8, 0xd0, 0xd1, 0xd1, 0xd2, 0xd3, 0xd3, 0xd4, 0xd5, 0xd5, 0xc4, 0xc5,
    0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xd1, 0xd1, 0xd2, 0xd2, 0xd3, 0xd3, 0xc1, 0xd4,
    0xd5, 0xd5, 0xd5, 0xc4, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xb4,
    0xb4, 0xb4, 0xb6, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xbb, 0xd3, 0xb4, 0xb4, 0xd7, 0xbf,
    0xc0, 0xc1, 0xc2, 0xd5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5,
    0xd3, 0xc5, 0xd3, 0xd4, 0xc4, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5
};
DATA(0x0047eb28) u8 gColorTableDarkBrown[PALETTE_COLOR_COUNT] = {
    0x32, 0x2a, 0x2a, 0x2a, 0x2a, 0x32, 0x32, 0x32, 0x32, 0x35, 0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2d,
    0x2e, 0x2e, 0x2f, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a,
    0x3c, 0x3e, 0x3e, 0x3e, 0x3e, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x2d,
    0x2e, 0x2f, 0x30, 0x32, 0x33, 0x34, 0x36, 0x37, 0x38, 0x3a, 0x3a, 0x3b, 0x3c, 0x3c, 0x3d, 0x3e,
    0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x39, 0x3a,
    0x3a, 0x3c, 0x3c, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x2b, 0x2b, 0x2c, 0x2c,
    0x2c, 0x2d, 0x2d, 0x2d, 0x2e, 0x2f, 0x30, 0x30, 0x31, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x3b, 0x3c, 0x2c, 0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x35,
    0x36, 0x37, 0x38, 0x39, 0x3a, 0x3c, 0x3e, 0x3e, 0x2b, 0x2b, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2f,
    0x2f, 0x30, 0x31, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3c, 0x3d, 0x2b,
    0x2c, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x34, 0x35, 0x36, 0x37, 0x39, 0x3a, 0x3c, 0x3c, 0x3e, 0x3e,
    0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,
    0x35, 0x36, 0x37, 0x39, 0x3a, 0x3b, 0x36, 0x3a, 0x3e, 0x3e, 0x2a, 0x2d, 0x31, 0x37, 0x2f, 0x31,
    0x32, 0x33, 0x34, 0x35, 0x37, 0x38, 0x3a, 0x33, 0x34, 0x37, 0x39, 0x36, 0x39, 0x38, 0x2c, 0x30,
    0x36, 0x3e, 0x31, 0x33, 0x36, 0x39, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x2a
};
DATA(0x0047ec28) i32 MAP_WIDTH = MAP_DIMENSION_MEDIUM;
DATA(0x0047ec2c) i32 MAP_HEIGHT = MAP_DIMENSION_MEDIUM;
DATA(0x004a498c) b32 gbClosingApp = false;
DATA(0x004a4990) b32 gbForegroundApp = false;
DATA(0x0047ec30) i32 giMainVideoModeColorDepth = WINGRAPH_COLOR_DEPTH;
DATA(0x0047ec34) i32 giMainVideoModeWidth = LOGICAL_SCREEN_WIDTH;
DATA(0x0047ec38) i32 giMainVideoModeHeight = LOGICAL_SCREEN_HEIGHT;
DATA(0x0047ec3c) u8 gMapColors[RADAR_MAP_COLOR_COUNT] = {77, 98, 13, 104, 32, 118, 54, 206, 41, 0, 0, 0};
DATA(0x0047ec48) u8 gObjectColors[RADAR_OBJECT_COLOR_COUNT] =
    {16, 48, 98, 160, 126, 74, 110, 179, 100, 218, 12, 12, 12, 12, 12, 12};
DATA(0x0047ec58) u8 gOwnerColors[RADAR_OWNER_COLOR_COUNT] = {73, 105, 190, 114, 205, 138, 10, 0};
DATA(0x0047ec60) H2_CONST char* gTilesetFiles[IDX(TILESET_COUNT)] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "objnhaun.icn",
    "objnarti.icn",
    "mons32.icn",
    "art32.icn",
    "flag32.icn",
    "ressmall.icn",
    "hourglas.icn",
    "route.icn",
    "",
    "stonback.icn",
    "minimon.icn",
    "minihero.icn",
    "mtnsnow.icn",
    "mtnswmp.icn",
    "mtnlava.icn",
    "mtndsrt.icn",
    "mtndirt.icn",
    "mtnmult.icn",
    "",
    "extraovr.icn",
    "road.icn",
    "mtncrck.icn",
    "mtngras.icn",
    "trejngl.icn",
    "treevil.icn",
    "objntown.icn",
    "objntwba.icn",
    "objntwsh.icn",
    "objntwrd.icn",
    "objnxtra.icn",
    "objnwat2.icn",
    "objnmul2.icn",
    "tresnow.icn",
    "trefir.icn",
    "trefall.icn",
    "stream.icn",
    "objnrsrc.icn",
    "dummy.icn",
    "objngra2.icn",
    "tredeci.icn",
    "objnwatr.icn",
    "objngras.icn",
    "objnsnow.icn",
    "objnswmp.icn",
    "objnlava.icn",
    "objndsrt.icn",
    "objndirt.icn",
    "objncrck.icn",
    "objnlav3.icn",
    "objnmult.icn",
    "objnlav2.icn",
    "x_loc1.icn",
    "x_loc2.icn",
    "x_loc3.icn"
};
DATA(0x0047ed60) u8 bPuzzleDraw[PUZZLE_DRAW_TABLE_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
DATA(0x0047eda0) u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][PALETTE_COLOR_COUNT] = {
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14,
      0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
      0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32,
      0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3e, 0x3e, 0x3e,
      0x3e, 0x3e, 0x3e, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
      0x50, 0x51, 0x52, 0x53, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
      0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6b, 0x6b, 0x6b,
      0x6b, 0x6b, 0x6b, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d,
      0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x88, 0x89, 0x8a, 0x8b,
      0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97, 0x97, 0x97,
      0x97, 0x97, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa,
      0xab, 0xac, 0xad, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xae, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8,
      0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc5, 0xc5,
      0xc5, 0xc5, 0xc5, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5,
      0xd5, 0xd5, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xe1, 0xe2, 0xe3,
      0xe4, 0xe5, 0xe6, 0xe6, 0xe6, 0xe6, 0x49, 0x4b, 0x4d, 0x4f, 0x51, 0x4c, 0x4e, 0x4a, 0x4c,
      0x4e, 0x50, 0xf4, 0xf5, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0f, 0x10, 0x11, 0x12,
      0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21,
      0x22, 0x23, 0x24, 0x24, 0x24, 0x24, 0x24, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
      0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3e,
      0x3e, 0x3e, 0x3e, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d,
      0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x54, 0x54, 0x54, 0x59, 0x5a, 0x5b, 0x5c, 0x5d,
      0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6b,
      0x6b, 0x6b, 0x6b, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b,
      0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x82, 0x82, 0x82, 0x86, 0x87, 0x88, 0x89,
      0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97,
      0x97, 0x97, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
      0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xae, 0xae, 0xae, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
      0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5,
      0xc5, 0xc5, 0xc5, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
      0xd5, 0xd5, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xe0, 0xe1, 0xe2,
      0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0xe6, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c,
      0x4c, 0x4e, 0xf4, 0xf5, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
      0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
      0x20, 0x21, 0x22, 0x23, 0x24, 0x24, 0x24, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e,
      0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,
      0x3e, 0x3e, 0x3e, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c,
      0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x54, 0x54, 0x57, 0x58, 0x59, 0x5a, 0x5b,
      0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
      0x6b, 0x6b, 0x6b, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
      0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x82, 0x85, 0x86, 0x87, 0x88,
      0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
      0x97, 0x97, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
      0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xae, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5,
      0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4,
      0xc5, 0xc5, 0xc5, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3,
      0xd4, 0xd5, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xdf, 0xe0, 0xe1,
      0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c,
      0x4c, 0x4c, 0xf3, 0xf4, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
      0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
      0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x24, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
      0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c,
      0x3d, 0x3e, 0x3e, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b,
      0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x54, 0x56, 0x57, 0x58, 0x59, 0x5a,
      0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
      0x6a, 0x6b, 0x6b, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
      0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x82, 0x84, 0x85, 0x86, 0x87,
      0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
      0x97, 0x97, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5,
      0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xae, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
      0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3,
      0xc4, 0xc5, 0xc5, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2,
      0xd3, 0xd4, 0xd5, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xdf, 0xe0, 0xe1,
      0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe6, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b,
      0x4b, 0x4b, 0xf3, 0xf4, 0xf5, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x0b, 0x0b, 0x0b,
      0x0c, 0x0d, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x0f, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12,
      0x13, 0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x0b, 0x25, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
      0x27, 0x28, 0x28, 0x29, 0x29, 0x29, 0x29, 0x2a, 0x2a, 0x13, 0x2a, 0x14, 0x14, 0x14, 0x14,
      0x14, 0x14, 0x15, 0x0c, 0x83, 0x3f, 0x3f, 0x3f, 0x40, 0x40, 0x40, 0x41, 0x41, 0x41, 0x41,
      0x41, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0x0d, 0x0e, 0x0f, 0x0f, 0x10,
      0x55, 0x11, 0x55, 0x55, 0x55, 0x55, 0x13, 0x56, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
      0x15, 0x15, 0x15, 0x0a, 0x6c, 0x6c, 0x6d, 0x6d, 0x6d, 0x6e, 0x6e, 0x6e, 0x6e, 0xc7, 0x28,
      0x29, 0x29, 0x29, 0x29, 0x29, 0x2a, 0x2a, 0x2a, 0x2a, 0x14, 0x14, 0x0b, 0x0b, 0x83, 0x83,
      0x84, 0x84, 0x84, 0x85, 0x85, 0x86, 0x86, 0x86, 0x87, 0x87, 0x12, 0x88, 0x13, 0x13, 0x14,
      0x14, 0x14, 0x0a, 0x0b, 0x0b, 0x0b, 0x0c, 0x0c, 0x0d, 0x0d, 0x0d, 0x0e, 0x0f, 0x0f, 0x0f,
      0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x13, 0x13, 0x14, 0x14, 0x0b, 0xaf, 0xaf, 0xb0, 0xb0,
      0x26, 0xb1, 0xb1, 0xb2, 0xb2, 0xb2, 0xb3, 0xb3, 0xb3, 0xb3, 0xb4, 0xb4, 0xb4, 0xb4, 0xb4,
      0xb4, 0x15, 0x15, 0x6c, 0x6c, 0x26, 0x6d, 0x26, 0x6d, 0x27, 0x28, 0x28, 0x29, 0x29, 0x29,
      0x2a, 0x2a, 0x2a, 0x14, 0xc7, 0xb3, 0xb4, 0xb4, 0x6e, 0x6e, 0x28, 0x2a, 0x6e, 0x6e, 0x56,
      0x56, 0x56, 0x56, 0x12, 0x12, 0x13, 0x41, 0x41, 0x41, 0x42, 0x41, 0x42, 0x41, 0x98, 0x9b,
      0x41, 0xf2, 0x0f, 0x10, 0x11, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c,
      0x0d, 0x0d, 0x0e, 0x0f, 0x0f, 0x10, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x14, 0x15, 0x15,
      0x16, 0x16, 0x17, 0x18, 0x18, 0x19, 0x19, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28, 0x29,
      0x29, 0x29, 0x2a, 0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x17, 0x18, 0x18,
      0x18, 0x18, 0x18, 0x83, 0x3f, 0x3f, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0xf2, 0x43, 0x43,
      0x44, 0x44, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0x0f, 0x0f, 0x55, 0x55, 0x55,
      0x55, 0x56, 0x56, 0x57, 0x57, 0x58, 0x58, 0x58, 0x58, 0x59, 0x18, 0x5a, 0x19, 0x19, 0x19,
      0x19, 0x19, 0x19, 0x25, 0x6c, 0x6d, 0x6d, 0x6e, 0x6e, 0x6f, 0x6f, 0xc8, 0xc8, 0xc9, 0xc9,
      0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x2e, 0x0b, 0x83, 0x84, 0x84,
      0x84, 0x85, 0x85, 0x86, 0x87, 0x87, 0x88, 0xf2, 0x89, 0x89, 0x8a, 0xf3, 0xf3, 0xf3, 0xf3,
      0xf3, 0x18, 0x98, 0x98, 0x99, 0x99, 0x9a, 0x9a, 0x9b, 0x9c, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f,
      0x12, 0x13, 0x13, 0x14, 0x14, 0x15, 0x16, 0x16, 0x17, 0x18, 0x25, 0xaf, 0xb0, 0xb0, 0xb1,
      0xb1, 0xb2, 0xb3, 0xb3, 0xb4, 0xb4, 0xb4, 0xb5, 0xb5, 0xb5, 0xb6, 0xb6, 0xb6, 0x2e, 0x2f,
      0x2f, 0x30, 0x19, 0x6c, 0x6d, 0x6d, 0x6d, 0xc6, 0xc7, 0xc7, 0xc9, 0xc9, 0x2a, 0x2b, 0x2b,
      0x2c, 0x2d, 0x2e, 0x2e, 0xc9, 0xb5, 0xb6, 0xb7, 0x6f, 0x6f, 0xca, 0x2d, 0x6f, 0x6f, 0x57,
      0x58, 0x58, 0x58, 0x58, 0x15, 0x16, 0x42, 0x42, 0x44, 0x44, 0x43, 0x44, 0x44, 0x98, 0x9d,
      0x42, 0x45, 0x10, 0x12, 0x14, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0d,
      0x0e, 0x0e, 0x0f, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18,
      0x19, 0x1a, 0x1a, 0x1b, 0x1c, 0x1d, 0x1d, 0x25, 0x25, 0x26, 0x27, 0x28, 0x28, 0x29, 0x2a,
      0x2a, 0x2b, 0x2c, 0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x2f, 0x30, 0x30, 0x31, 0x32, 0x32, 0x1b,
      0x1c, 0x1c, 0x1c, 0x3f, 0x3f, 0x40, 0x41, 0x41, 0x42, 0x43, 0x43, 0x44, 0x45, 0x45, 0x45,
      0x46, 0x46, 0x46, 0xf4, 0x47, 0xf4, 0xf4, 0xf4, 0xf4, 0xf5, 0x10, 0x55, 0x55, 0x56, 0x57,
      0x57, 0x58, 0x58, 0x59, 0x5a, 0x5a, 0x5b, 0x5b, 0x5b, 0x5c, 0x5d, 0x5d, 0x5d, 0x1d, 0x1d,
      0x1d, 0x1d, 0x1d, 0x25, 0x6d, 0x6d, 0x6e, 0x6f, 0x6f, 0x70, 0x71, 0x70, 0x70, 0xcb, 0xcb,
      0xcb, 0x2c, 0x2d, 0x2e, 0x2f, 0x2f, 0x2f, 0x30, 0x30, 0x31, 0x32, 0x83, 0x83, 0x84, 0x85,
      0x85, 0x86, 0x87, 0x88, 0x88, 0x89, 0x89, 0x8b, 0x8b, 0x8b, 0x8d, 0x8d, 0x8d, 0x8f, 0x8f,
      0xf5, 0xf5, 0x98, 0x98, 0x99, 0x9a, 0x9b, 0x9b, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0xa0, 0xa1,
      0xa2, 0xa3, 0xa3, 0xa4, 0xa5, 0xa5, 0xa6, 0x1a, 0x1a, 0x1b, 0xaf, 0x0d, 0xb0, 0xb1, 0xb2,
      0xb2, 0xb3, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb7, 0xb7, 0xb7, 0xb8, 0xb8, 0xb9, 0xb9, 0x32,
      0x32, 0x34, 0x34, 0x6d, 0x6d, 0xc6, 0xc7, 0xc8, 0xc9, 0xc9, 0xca, 0xca, 0x2c, 0x2d, 0x2e,
      0x2f, 0x30, 0x30, 0x31, 0xcc, 0xcd, 0xb9, 0xb9, 0x70, 0x70, 0xcc, 0x2f, 0x70, 0x71, 0x58,
      0x59, 0x5b, 0x5c, 0x5d, 0x5d, 0x19, 0x42, 0x44, 0x45, 0x45, 0x44, 0x45, 0x45, 0x99, 0x9f,
      0x44, 0x47, 0x12, 0xf2, 0xf3, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0d,
      0x0e, 0x0f, 0x10, 0x11, 0x11, 0x13, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x1a, 0x1a,
      0x1b, 0x1c, 0x1c, 0x1e, 0x1e, 0x1f, 0x20, 0x25, 0x26, 0x27, 0x27, 0x28, 0x29, 0x2a, 0x2b,
      0x2b, 0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x32, 0x33, 0x34, 0x34, 0x35, 0x36,
      0x36, 0x1e, 0x1f, 0x3f, 0x40, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x45, 0x46, 0x47, 0x47,
      0x47, 0x48, 0x48, 0x48, 0x49, 0x49, 0x49, 0xa8, 0xa8, 0xa8, 0x55, 0x55, 0x56, 0x57, 0x58,
      0x58, 0x59, 0x5a, 0x5b, 0x5b, 0x5c, 0x5d, 0x5d, 0x5e, 0x5f, 0x5f, 0x60, 0x60, 0x60, 0x1f,
      0x20, 0x20, 0x20, 0x6c, 0x6d, 0xc6, 0x6e, 0x6f, 0x70, 0x71, 0x71, 0x71, 0x74, 0x75, 0x76,
      0x77, 0x78, 0x79, 0x2f, 0x30, 0x32, 0x32, 0x33, 0x33, 0x34, 0x34, 0x83, 0x84, 0x84, 0x85,
      0x86, 0x87, 0x88, 0x89, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8d, 0x8f, 0x8f, 0x90, 0x91, 0x92,
      0x93, 0x1e, 0x98, 0x99, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2,
      0xa3, 0xa4, 0xa5, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0x1c, 0x1d, 0xaf, 0xb0, 0xb1, 0xb1, 0xb2,
      0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb9, 0xb9, 0xb9, 0xba, 0xba, 0xbb, 0x32, 0x34,
      0x34, 0x36, 0x37, 0x6d, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xca, 0xcc, 0xcc, 0xcd, 0xcf, 0x2f,
      0x31, 0x32, 0x33, 0x34, 0xce, 0xce, 0xbb, 0xbc, 0x71, 0x71, 0x76, 0x31, 0xde, 0xde, 0xdf,
      0xe0, 0xe1, 0xe2, 0x5f, 0xe3, 0xe4, 0x43, 0x44, 0x46, 0x47, 0x45, 0x47, 0x46, 0x99, 0x41,
      0x45, 0x49, 0xf2, 0x16, 0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c,
      0x0d, 0x0e, 0x0e, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x12, 0x13, 0x14, 0xf2, 0xf2, 0x16, 0x16,
      0x17, 0xf3, 0xf3, 0x19, 0xf4, 0xf4, 0xf4, 0x0b, 0x25, 0x26, 0x26, 0x27, 0x28, 0x28, 0x29,
      0xb2, 0x12, 0x13, 0x14, 0x14, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17,
      0x17, 0x18, 0xf4, 0x3f, 0x3f, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0x43, 0x43, 0x44, 0x44,
      0x44, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x46, 0x46, 0x46, 0x0f, 0x0f, 0x10, 0x55, 0x56,
      0x12, 0x13, 0x13, 0x14, 0x9f, 0x15, 0x15, 0xa1, 0x16, 0xa3, 0xa3, 0xa3, 0x17, 0x17, 0xa5,
      0xa5, 0xf4, 0xf4, 0x25, 0x6c, 0x26, 0x6d, 0x6d, 0x6e, 0xc7, 0xc8, 0xc7, 0x28, 0x29, 0x2a,
      0x2a, 0x2a, 0x2b, 0x2b, 0x2c, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 0x83, 0x83, 0x84, 0x84,
      0x85, 0x85, 0x86, 0x86, 0x87, 0x87, 0x88, 0x88, 0x89, 0x89, 0x8a, 0x8a, 0x8b, 0x8b, 0x8c,
      0x8d, 0xf4, 0x98, 0x98, 0x99, 0x99, 0x9a, 0x9a, 0x9b, 0x9b, 0x9c, 0x9c, 0x9d, 0x9e, 0x9e,
      0x9f, 0xf2, 0x9f, 0xa1, 0xa1, 0xf3, 0xf3, 0xf3, 0xf3, 0xa4, 0x0b, 0xaf, 0xb0, 0xb0, 0xb1,
      0xb1, 0xb2, 0xb3, 0xb3, 0xb4, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb6, 0xb6, 0xb7, 0x16, 0x17,
      0x17, 0x17, 0x17, 0x6c, 0x26, 0x26, 0x27, 0x27, 0x28, 0x28, 0x29, 0xb2, 0xb4, 0x2a, 0x2c,
      0x2d, 0x17, 0x17, 0x17, 0xb4, 0xb5, 0xb5, 0xb7, 0x6e, 0xc8, 0x2a, 0x2d, 0x55, 0x56, 0x57,
      0x57, 0x57, 0x15, 0x16, 0x16, 0x17, 0x42, 0x42, 0x43, 0x44, 0x43, 0x44, 0x44, 0x99, 0x9e,
      0x43, 0x46, 0x40, 0x41, 0xf2, 0xf3, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f,
      0x0a},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0c, 0x0d,
      0x0e, 0x0e, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x13, 0x13, 0x14, 0xf2, 0x16, 0x16, 0xf3, 0xf3,
      0xf3, 0xf4, 0xf4, 0xf4, 0xf4, 0xf5, 0xf5, 0x25, 0x25, 0x26, 0xb0, 0x27, 0xb1, 0x29, 0x29,
      0x2a, 0xb3, 0x14, 0xb4, 0x2d, 0x16, 0x17, 0x17, 0x18, 0x18, 0x18, 0x18, 0x19, 0x19, 0x19,
      0x19, 0x1a, 0x1a, 0x3f, 0x3f, 0x40, 0x40, 0x41, 0x42, 0x43, 0x43, 0x44, 0x44, 0x45, 0x45,
      0x46, 0x46, 0x46, 0x46, 0x46, 0x47, 0x47, 0x47, 0x47, 0x47, 0x0f, 0x55, 0x55, 0x56, 0x56,
      0x57, 0x14, 0x58, 0x59, 0x16, 0xa1, 0xa2, 0xa3, 0xa3, 0xa4, 0xa4, 0xa5, 0xa5, 0xa6, 0xa6,
      0xa7, 0xa7, 0xa7, 0x25, 0x6c, 0x6d, 0x6d, 0x6e, 0xc7, 0x6f, 0x6f, 0xc8, 0xc9, 0x29, 0x2b,
      0x2b, 0x2b, 0x2c, 0x2c, 0x2e, 0x2e, 0x18, 0x18, 0x19, 0x19, 0x19, 0x83, 0x83, 0x84, 0x84,
      0x85, 0x86, 0x87, 0x87, 0x88, 0x88, 0x89, 0x8a, 0x8a, 0x8b, 0x8b, 0x8c, 0x8c, 0x8d, 0x8d,
      0x8e, 0x8f, 0x98, 0x98, 0x99, 0x99, 0x9a, 0x9b, 0x9b, 0x9c, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f,
      0x9f, 0xa1, 0xa1, 0xa2, 0xa2, 0xa3, 0xa4, 0xf4, 0xf4, 0xf4, 0xaf, 0xaf, 0xb0, 0xb1, 0xb1,
      0xb2, 0xb3, 0xb3, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb7, 0xb7, 0xb7, 0xb8, 0xb8, 0xb8, 0x19,
      0x19, 0x19, 0x19, 0x6c, 0x6d, 0x6d, 0x27, 0x28, 0x29, 0x29, 0x2a, 0x2a, 0x2b, 0x2b, 0x2d,
      0x2e, 0x2f, 0x19, 0x19, 0xb5, 0xb6, 0xb7, 0xb9, 0x6f, 0x6f, 0x2a, 0x2e, 0x6f, 0x57, 0x58,
      0x58, 0x58, 0x16, 0x17, 0x18, 0x19, 0x42, 0x43, 0x44, 0x45, 0x44, 0x45, 0x45, 0x99, 0x00,
      0x44, 0x47, 0x41, 0xf2, 0xf2, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0a},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0b, 0x0d, 0x0d,
      0x0e, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x16, 0x17, 0xf3, 0x19, 0xf4,
      0xf4, 0xf4, 0x1c, 0xf5, 0xf5, 0xf5, 0x1f, 0x25, 0x26, 0x26, 0x27, 0x28, 0x29, 0x29, 0x2a,
      0x2a, 0xb4, 0x2d, 0x2e, 0x2e, 0x2f, 0x2f, 0x19, 0x1a, 0x1a, 0x1a, 0x1a, 0x1b, 0x1b, 0x1b,
      0x1b, 0x1b, 0xf5, 0x3f, 0x3f, 0x40, 0x41, 0x42, 0x42, 0x43, 0x44, 0x45, 0x45, 0x46, 0x46,
      0x47, 0x47, 0x48, 0x48, 0x48, 0x48, 0x48, 0x49, 0x49, 0x49, 0x10, 0x55, 0x55, 0x56, 0x57,
      0x58, 0x58, 0x5a, 0x5a, 0x5b, 0x5b, 0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7, 0xa7, 0xa8,
      0xa9, 0xa9, 0xaa, 0x25, 0x6c, 0x6d, 0xc6, 0xc7, 0x6f, 0x70, 0x70, 0xc9, 0xca, 0xca, 0x2b,
      0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x1a, 0x1b, 0x1b, 0x1b, 0x83, 0x83, 0x84, 0x85,
      0x86, 0x87, 0x87, 0x88, 0x89, 0x89, 0x8a, 0x8b, 0x8b, 0x8c, 0x8d, 0x8d, 0x8e, 0x8f, 0x8f,
      0x90, 0x91, 0x98, 0x98, 0x99, 0x9a, 0x9b, 0x9b, 0x9c, 0x9c, 0x9e, 0x9e, 0x9f, 0xa0, 0xa0,
      0xa1, 0xa2, 0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7, 0xa7, 0xaf, 0xb0, 0xb0, 0xb1, 0xb2,
      0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb8, 0xb8, 0xb9, 0xba, 0xba, 0x32, 0x33,
      0x1b, 0x1b, 0x1b, 0x6d, 0x6d, 0x6d, 0x28, 0x28, 0x29, 0x2a, 0x2b, 0x2b, 0x2c, 0x2d, 0x2e,
      0x2f, 0x31, 0x1b, 0x1b, 0xb6, 0xb7, 0xb8, 0xbb, 0x70, 0x70, 0x2b, 0x2f, 0x70, 0x57, 0x59,
      0x5a, 0x5b, 0x5b, 0x18, 0x1a, 0x1a, 0x43, 0x44, 0x45, 0x46, 0x45, 0x46, 0x46, 0x99, 0x00,
      0x00, 0x49, 0x41, 0xf2, 0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0a},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0d,
      0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x17, 0xf3, 0x18, 0x19, 0xf4, 0xf4,
      0x1b, 0xf5, 0xf5, 0x1f, 0xaa, 0x95, 0x95, 0x25, 0x26, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2a,
      0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d,
      0x1d, 0x1d, 0x1e, 0x3f, 0x40, 0x41, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x46, 0x47, 0x48,
      0x48, 0x49, 0x49, 0x4a, 0x4a, 0x4b, 0x4b, 0x4c, 0x4c, 0x4c, 0x55, 0x55, 0x56, 0x57, 0x58,
      0x59, 0x5a, 0x5b, 0x5b, 0x5c, 0x5d, 0x5d, 0xa6, 0xa6, 0x60, 0xa8, 0xa8, 0xa9, 0xaa, 0xaa,
      0xab, 0xab, 0xab, 0x25, 0x6d, 0x6d, 0x6e, 0xc8, 0x6f, 0x70, 0x71, 0xca, 0xca, 0xcb, 0x2c,
      0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x1d, 0x1d, 0x83, 0x84, 0x85, 0x85,
      0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8b, 0x8c, 0x8d, 0x8e, 0x8e, 0x90, 0x90, 0x91, 0x91,
      0x92, 0x93, 0x98, 0x99, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0x9f, 0xa0, 0xa1, 0xa2,
      0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa8, 0xa8, 0xa9, 0xaf, 0xb0, 0xb1, 0xb1, 0xb2,
      0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xba, 0xbb, 0xbb, 0xbd, 0xbd, 0xc1,
      0xc1, 0x92, 0x92, 0x6d, 0x6d, 0xc6, 0xc7, 0xc9, 0xc9, 0xc9, 0x2c, 0xcd, 0x2d, 0x2e, 0x2f,
      0x30, 0x32, 0x34, 0x1d, 0xb7, 0xb9, 0xba, 0xbd, 0x70, 0x70, 0xcd, 0x31, 0xde, 0x58, 0x59,
      0x5b, 0x5c, 0x5d, 0x1a, 0x1b, 0x1c, 0x43, 0x44, 0x46, 0x47, 0x45, 0x47, 0x47, 0x9a, 0x00,
      0x00, 0x4b, 0xf2, 0xf2, 0xf3, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x0a}}
};
DATA(0x0047f9a0) u8 gColorTableLighten[PALETTE_COLOR_COUNT] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
    0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x25, 0x25, 0x25, 0x25, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b,
    0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x55, 0x55, 0x55, 0x55, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b,
    0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x6c, 0x6c, 0x6c, 0x6c,
    0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b,
    0x7c, 0x7d, 0x7e, 0x83, 0x83, 0x83, 0x83, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
    0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x98, 0x98, 0x98, 0x98, 0x98, 0x99, 0x9a, 0x9b,
    0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xaf,
    0xaf, 0xaf, 0xaf, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb,
    0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb,
    0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xde,
    0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xee,
    0xef, 0xf0, 0xf2, 0xf2, 0xf3, 0xf4, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xFF
};
DATA(0x0047faa0) u8 gColorTableNoCycle[PALETTE_COLOR_COUNT] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xbc, 0xbc, 0xbc, 0xbc, 0x76, 0x76, 0x76, 0x76, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45,
    0x45, 0x45, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xFF
};
DATA(0x004a4994) font* smallFont = NULL;
DATA(0x004a4998) font* bigFont = NULL;
DATA(0x004a499c) b32 gbReturnAfterComputeExtent = false;
DATA(0x0047fba0) b32 gbAllowTextEntryEscape = true;
DATA(0x004a49a0) WindowColorCycleMode giCycleType = WINDOW_COLOR_CYCLE_DEFAULT;
DATA(0x0047fba4) b32 giScreenScroll = true;
DATA(0x0047fba8) i32 giMenuCommand = -1;
DATA(0x004a49a4) b32 gbSendMouseMoveMessages = false;
DATA(0x0047fbac) b32 gbColorMice = true;
DATA(0x0047fbb0) u32l gTownEligibleBuildMask[IDX(FACTION_COUNT)] = {
    TOWN_ELIGIBLE_BUILD_KNIGHT_MASK,
    TOWN_ELIGIBLE_BUILD_BARBARIAN_MASK,
    TOWN_ELIGIBLE_BUILD_SORCERESS_MASK,
    TOWN_ELIGIBLE_BUILD_WARLOCK_MASK,
    TOWN_ELIGIBLE_BUILD_WIZARD_MASK,
    TOWN_ELIGIBLE_BUILD_NECROMANCER_MASK
};
DATA(0x0047fbc8) u8 giMapSizes[KB_MAP_SIZE_COUNT] =
    {MAP_DIMENSION_SMALL, MAP_DIMENSION_MEDIUM, MAP_DIMENSION_LARGE, MAP_DIMENSION_XLARGE};
DATA(0x004a49a8) b32 gbUseEvilInterface = false;
DATA(0x0047fbcc) H2_CONST char* cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT] = {
    {
        "advbord.icn",
        "advborde.icn"
    },
    {
        "heroextg.icn",
        "heroexte.icn"
    },
    {
        "buybuild.icn",
        "buybuile.icn"
    },
    {
        "advbtns.icn",
        "advebtns.icn"
    },
    {
        "herologo.icn",
        "herologe.icn"
    },
    {
        "sunmoon.icn",
        "sunmoone.icn"
    },
    {
        "stonback.icn",
        "stonbake.icn"
    },
    {
        "scroll.icn",
        "scrolle.icn"
    },
    {
        "locators.icn",
        "locatore.icn"
    },
    {
        "system.icn",
        "systeme.icn"
    },
    {
        "CPANBKG.ICN",
        "CPANBKGE.ICN"
    },
    {
        "CPANEL.ICN",
        "CPANELE.ICN"
    },
    {
        "APANBKG.ICN",
        "APANBKGE.ICN"
    },
    {
        "APANEL.ICN",
        "APANELE.ICN"
    },
    {
        "VIEWWRLD.ICN",
        "EVIWWRLD.ICN"
    },
    {
        "VIEWRSRC.ICN",
        "EVIWRSRC.ICN"
    },
    {
        "VIEWRTFX.ICN",
        "EVIWRTFX.ICN"
    },
    {
        "VIEWTWNS.ICN",
        "EVIWTWNS.ICN"
    },
    {
        "VIEWHROS.ICN",
        "EVIWHROS.ICN"
    },
    {
        "VIEW_ALL.ICN",
        "EVIW_ALL.ICN"
    },
    {
        "VIEWMINE.ICN",
        "EVIWMINE.ICN"
    },
    {
        "VIEWDDOR.ICN",
        "EVIWDDOR.ICN"
    },
    {
        "VIEWPUZL.ICN",
        "EVIWPUZL.ICN"
    },
    {
        "LGNDXTRA.ICN",
        "LGNDXTRE.ICN"
    },
    {
        "SPANBKG.ICN",
        "SPANBKGE.ICN"
    },
    {
        "SPANBTN.ICN",
        "SPANBTNE.ICN"
    },
    {
        "CSPANBKG.ICN",
        "CSPANBKE.ICN"
    },
    {
        "CSPANBTN.ICN",
        "CSPANBTE.ICN"
    },
    {
        "TRADPOST.ICN",
        "TRADPOSE.ICN"
    },
    {
        "VIEWARMY.ICN",
        "VIEWARME.ICN"
    },
    {
        "WINLOSE.ICN",
        "WINLOSEE.ICN"
    },
    {
        "WINCMBTB.ICN",
        "WINCMBBE.ICN"
    },
    {
        "SURRENDR.ICN",
        "SURRENDE.ICN"
    },
    {
        "SURDRBKG.ICN",
        "SURDRBKE.ICN"
    },
    {
        "VGENBKG.ICN",
        "VGENBKGE.ICN"
    },
    {
        "campbkgg.ICN",
        "campbkge.ICN"
    },
    {
        "campxtrg.ICN",
        "campxtre.ICN"
    }
};
DATA(0x0047fcf4) char gcAnimPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\ANIM2\\";
DATA(0x0047fe54) char gcGamePath[GLOBAL_GAME_PATH_SIZE] = ".\\GAMES\\";
DATA(0x0047fe68) char gcMapPath[GLOBAL_MAP_PATH_SIZE] = ".\\MAPS\\";
DATA(0x0047fe7c) char gcMusicPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\TRACKS2\\";
DATA(0x004a49ac) i32 gbPutzingWithMouseCtr = 0;
DATA(0x0047ffdc) float gfCombatSpeedMod[KB_COMBAT_SPEED_COUNT] = {1.0f, 0.7f, 0.35f};
DATA(0x004a49b0) icon* gShingleAnim = NULL;
DATA(0x004a49b4) i32 iNextShingleAnim = 0;
DATA(0x004a49b8) i32 giDialogTimeout = 0;
DATA(0x004a49bc) i32 giNewMonsterCycleFrame = 0;
DATA(0x004a49c0) b32 gbNoCDRom = false;
DATA(0x004a49c4) b32 gbLeaveNetBoxAlone = false;
DATA(0x0047ffe8) b32 gbDrawWindowBackground = true;
DATA(0x004a49c8) b32 gbCheatMenus = false;
DATA(0x004a49cc) b32 gbUseWaveout = false;
DATA(0x004a49d0) b32 gbShowAllMaps = false;


DATA(0x0047ffec) i16 gVesaMode[VESA_MODE_VALUE_COUNT] =
    {640, 480, 256, VESA_SET_MODE_FUNCTION, VESA_MODE_640_480_256, 0};
DATA(0x0047fff8) b32 bShowIt = true;
DATA(0x0047fffc) b32 gbEnlargeScreenBlit = true;
// No retail code reads these two; they keep their retail .data places.
DATA(0x00480000) b32 gCommandLineInterpreted = true;
DATA(0x00480004) b32 gShowMapInfo = true;
DATA(0x00480008) ConfigExecutable giCurExe = CONFIG_EXECUTABLE_EDITOR;
DATA(0x0048000c) i32 gClearFlags = EDITOR_CLEAR_FLAGS_DEFAULT;
// The drag selection the map view outlines (EDIT_NO_CELL when there is none).
DATA(0x00480010) i32 gSelectionX = EDIT_NO_CELL;
// The random map generator's settings (EVENTMGR's dialog, RANDOM).
DATA(0x00480014) i32 gRandomMapPlayers = NEW_MAP_DEFAULT_PLAYERS;
DATA(0x00480018) double gTerrainPercent[RANDOM_MAP_TERRAIN_COUNT] = {30.0, 30.0, 20.0, 0.0, 0.0, 0.0, 20.0, 0.0};
DATA(0x00480058) double gDensityPercent[RANDOM_MAP_DENSITY_COUNT] = {50.0, 50.0, 50.0, 50.0, 50.0};
DATA(0x00480080) b32 gScatterTerrain = true;
DATA(0x00480088) struct SMenuEnableStatus gsMenuEnableStatus[MENU_ENABLE_STATUS_COUNT] = {
    {APP_MENU_NONE, 0, 0, 0},
    {IDX(KBWIN_MENU_SIZE_640_480), 1, 1, 0},
    {IDX(KBWIN_MENU_SIZE_800_600), 1, 1, 0},
    {IDX(KBWIN_MENU_SIZE_1024_768), 1, 1, 0},
    {IDX(KBWIN_MENU_SIZE_1280_1024), 1, 1, 0},
    {IDX(KBWIN_MENU_FULLSCREEN), 1, 1, 0},
    {APP_MENU_VIEW_WORLD, 0, 0, 0},
    {APP_MENU_VIEW_PUZZLE, 0, 0, 0},
    {APP_MENU_CAST_SPELL, 0, 0, 0},
    {APP_MENU_SEARCH, 0, 0, 0},
    {APP_MENU_MUSIC_FIRST, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 1, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 2, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 3, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 4, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 5, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 6, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 7, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 8, 1, 0, 0},
    {APP_MENU_MUSIC_FIRST + 9, 1, 0, 0},
    {APP_MENU_MUSIC_LAST, 1, 0, 0},
    {APP_MENU_SOUND_FIRST, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 1, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 2, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 3, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 4, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 5, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 6, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 7, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 8, 1, 0, 0},
    {APP_MENU_SOUND_FIRST + 9, 1, 0, 0},
    {APP_MENU_SOUND_LAST, 1, 0, 0},
    {APP_MENU_SPEED_FIRST, 0, 0, 0},
    {APP_MENU_SPEED_FIRST + 1, 0, 0, 0},
    {APP_MENU_SPEED_FIRST + 2, 0, 0, 0},
    {APP_MENU_SPEED_FIRST + 3, 0, 0, 0},
    {APP_MENU_SPEED_LAST, 0, 0, 0},
    {APP_MENU_UNKNOWN_9C6D, 0, 0, 0},
    {APP_MENU_TOGGLE_ROUTE, 0, 0, 0},
    {APP_MENU_TOGGLE_BLACKOUT, 0, 0, 0},
    {IDX(KBWIN_MENU_HELP), 1, 1, 0},
    {IDX(KBWIN_MENU_ABOUT), 1, 1, 0},
    {APP_MENU_RESTART_0, 0, 1, 0},
    {APP_MENU_RESTART_1, 0, 1, 0},
    {APP_MENU_RESTART_2, 0, 1, 0},
    {APP_MENU_RESTART_3, 0, 1, 0},
    {APP_MENU_RESTART_4, 0, 1, 0},
    {APP_MENU_UNKNOWN_9CAD, 0, 1, 0},
    {APP_MENU_RESTART_5, 0, 1, 0},
    {APP_MENU_RESTART_6, 0, 1, 0},
    {APP_MENU_RESTART_7, 0, 1, 0},
    {APP_MENU_RESTART_8, 0, 1, 0},
    {APP_MENU_RESTART_9, 0, 1, 0},
    {APP_MENU_RESTART_10, 0, 1, 0},
    {APP_MENU_RESTART_11, 0, 1, 0},
    {APP_MENU_RESTART_12, 0, 1, 0},
    {APP_MENU_RESTART_13, 0, 1, 0},
    {APP_MENU_LOAD_0, 0, 1, 0},
    {APP_MENU_LOAD_1, 0, 1, 0},
    {APP_MENU_LOAD_2, 0, 1, 0},
    {APP_MENU_LOAD_3, 0, 1, 0},
    {APP_MENU_LOAD_4, 0, 1, 0},
    {APP_MENU_LOAD_5, 0, 1, 0},
    {APP_MENU_LOAD_6, 0, 1, 0},
    {APP_MENU_LOAD_7, 0, 1, 0},
    {APP_MENU_LOAD_8, 0, 1, 0},
    {APP_MENU_LOAD_9, 0, 1, 0},
    {APP_MENU_LOAD_10, 0, 1, 0},
    {APP_MENU_SAVE, 0, 0, 0},
    {APP_MENU_EXIT, 0, 0, 0}
};
// The map view's three zoom levels: the cell scale, the cell and tile
// sizes in pixels, and the cells the view spans.
DATA(0x00480274) i32 gZoomScale[EDIT_ZOOM_COUNT] = {1, 2, 4};
DATA(0x00480280) i32 gZoomCellSize[EDIT_ZOOM_COUNT] = {32, 16, 8};
DATA(0x0048028c) i32 gZoomViewCells[EDIT_ZOOM_COUNT] = {14, 28, 56};
DATA(0x00480298) i32 gZoomTileSize[EDIT_ZOOM_COUNT] = {32, 16, 8};
// The road and stream tools' tiles by neighbour mask (eight neighbours, the
// second set for a cell whose neighbours need the edge variants; four
// neighbours for a stream end), and which road tiles join their neighbours.
DATA(0x004802a4) u8 gLineTiles[LINE_NEIGHBOUR_MASKS] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    24, 24, 255, 255, 24, 24, 255, 255, 24, 24, 255, 255, 24, 24, 255, 255,
    25, 25, 25, 25, 255, 255, 255, 255, 25, 25, 25, 25, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 8, 255, 1, 255, 8, 255, 8, 15, 1, 15, 1, 15, 1, 15, 1,
    8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1,
    15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1,
    255, 8, 255, 8, 255, 8, 255, 8, 15, 1, 15, 1, 15, 1, 15, 1,
    255, 8, 255, 8, 255, 8, 255, 8, 15, 1, 15, 1, 15, 1, 15, 1,
    8, 8, 8, 8, 8, 8, 8, 8, 15, 1, 15, 1, 15, 1, 15, 1,
    15, 8, 15, 8, 15, 8, 15, 8, 15, 1, 15, 1, 15, 1, 15, 1,
    255, 8, 255, 8, 255, 8, 255, 8, 15, 1, 15, 1, 15, 1, 15, 1
};
DATA(0x004803a4) u8 gLineEdgeTiles[LINE_NEIGHBOUR_MASKS] = {
    0, 18, 17, 10, 18, 18, 18, 10, 17, 18, 17, 10, 11, 11, 11, 10,
    10, 10, 17, 10, 2, 2, 2, 2, 17, 17, 17, 17, 17, 17, 17, 17,
    11, 18, 2, 18, 18, 18, 18, 18, 11, 18, 2, 18, 11, 18, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    0, 9, 0, 9, 0, 9, 0, 9, 12, 9, 12, 9, 12, 9, 12, 9,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    0, 5, 7, 7, 16, 16, 7, 7, 13, 4, 7, 7, 16, 16, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 17, 7, 17, 17, 7, 7, 17, 17,
    16, 18, 16, 16, 16, 18, 16, 18, 16, 16, 16, 16, 16, 18, 16, 18,
    3, 21, 3, 21, 3, 21, 3, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    0, 5, 0, 5, 0, 5, 0, 5, 13, 4, 13, 4, 13, 4, 13, 4,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    3, 21, 3, 21, 3, 21, 3, 21, 21, 21, 21, 21, 21, 21, 21, 21
};
DATA(0x004804a4) u8 gLineEndTiles[LINE_END_MASKS] = {
    3, 2, 3, 1, 2, 2, 0, 11, 3, 4, 3, 9, 7, 8, 10, 6
};
DATA(0x004804b4) u8 gRoadTileJoins[LINE_ROAD_TILES] = {
    1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1
};
DATA(0x004804d4) u8 gRoadTileJoinsAlt[LINE_ROAD_TILES] = {
    1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1
};
// The terrain tool panel: its terrains, then its brushes.
DATA(0x004804f4) H2_CONST char* gTerrainHelp[EDITOR_TERRAIN_HELP_COUNT] = {
    "",
    localization::Tr("editor.table.gTerrainHelp.1"),
    localization::Tr("editor.table.gTerrainHelp.2"),
    localization::Tr("editor.table.gTerrainHelp.3"),
    localization::Tr("editor.table.gTerrainHelp.4"),
    localization::Tr("editor.table.gTerrainHelp.5"),
    localization::Tr("editor.table.gTerrainHelp.6"),
    localization::Tr("editor.table.gTerrainHelp.7"),
    localization::Tr("editor.table.gTerrainHelp.8"),
    localization::Tr("editor.table.gTerrainHelp.9"),
    localization::Tr("editor.table.gTerrainHelp.10"),
    localization::Tr("editor.table.gTerrainHelp.11"),
    localization::Tr("editor.table.gTerrainHelp.12"),
    localization::Tr("editor.table.gTerrainHelp.13")
};
// The eraser panel: its brushes, then the object classes it erases.
DATA(0x0048052c) H2_CONST char* gClearHelp[CLEAR_HELP_COUNT] = {
    localization::Tr("editor.table.gClearHelp.0"),
    localization::Tr("editor.table.gClearHelp.1"),
    localization::Tr("editor.table.gClearHelp.2"),
    localization::Tr("editor.table.gClearHelp.3"),
    "",
    localization::Tr("editor.table.gClearHelp.5"),
    localization::Tr("editor.table.gClearHelp.6"),
    localization::Tr("editor.table.gClearHelp.7"),
    localization::Tr("editor.table.gClearHelp.8"),
    localization::Tr("editor.table.gClearHelp.9"),
    localization::Tr("editor.table.gClearHelp.10"),
    localization::Tr("editor.table.gClearHelp.11"),
    localization::Tr("editor.table.gClearHelp.12"),
    localization::Tr("editor.table.gClearHelp.13"),
    localization::Tr("editor.table.gClearHelp.14"),
    localization::Tr("editor.table.gClearHelp.15"),
    localization::Tr("editor.table.gClearHelp.16"),
    localization::Tr("editor.table.gClearHelp.17"),
    localization::Tr("editor.table.gClearHelp.18"),
    ""
};
// The main panel's controls.
DATA(0x0048057c) H2_CONST char* gEditPanelHelp[EDIT_PANEL_HELP_COUNT] = {
    "",
    localization::Tr("editor.table.gEditPanelHelp.1"),
    localization::Tr("editor.table.gEditPanelHelp.2"),
    localization::Tr("editor.table.gEditPanelHelp.3"),
    localization::Tr("editor.table.gEditPanelHelp.4"),
    localization::Tr("editor.table.gEditPanelHelp.5"),
    localization::Tr("editor.table.gEditPanelHelp.6"),
    localization::Tr("editor.table.gEditPanelHelp.7"),
    localization::Tr("editor.table.gEditPanelHelp.8"),
    localization::Tr("editor.table.gEditPanelHelp.9"),
    localization::Tr("editor.table.gEditPanelHelp.10"),
    localization::Tr("editor.table.gEditPanelHelp.11"),
    localization::Tr("editor.table.gEditPanelHelp.12"),
    localization::Tr("editor.table.gEditPanelHelp.13"),
    localization::Tr("editor.table.gEditPanelHelp.14"),
    localization::Tr("editor.table.gEditPanelHelp.15")
};
// The terrains the random map dialog names.
DATA(0x004805bc) H2_CONST char* gEditTerrainNames[EDITOR_TERRAIN_NAME_COUNT] = {
    localization::Tr("editor.table.gEditTerrainNames.0"),
    localization::Tr("editor.table.gEditTerrainNames.1"),
    localization::Tr("editor.table.gEditTerrainNames.2"),
    localization::Tr("editor.table.gEditTerrainNames.3"),
    localization::Tr("editor.table.gEditTerrainNames.4"),
    localization::Tr("editor.table.gEditTerrainNames.5"),
    localization::Tr("editor.table.gEditTerrainNames.6"),
    localization::Tr("editor.table.gEditTerrainNames.7"),
    localization::Tr("editor.table.gEditTerrainNames.8")
};
// The object tool's object classes.
DATA(0x004805e0) H2_CONST char* gObjectClassNames[EDITOR_OBJECT_CLASS_COUNT] = {
    localization::Tr("editor.table.gObjectClassNames.0"),
    localization::Tr("editor.table.gObjectClassNames.1"),
    localization::Tr("editor.table.gObjectClassNames.2"),
    localization::Tr("editor.table.gObjectClassNames.3"),
    localization::Tr("editor.table.gObjectClassNames.4"),
    localization::Tr("editor.table.gObjectClassNames.5"),
    localization::Tr("editor.table.gObjectClassNames.6"),
    localization::Tr("editor.table.gObjectClassNames.7"),
    localization::Tr("editor.table.gObjectClassNames.8"),
    localization::Tr("editor.table.gObjectClassNames.9"),
    localization::Tr("editor.table.gObjectClassNames.10"),
    localization::Tr("editor.table.gObjectClassNames.11"),
    localization::Tr("editor.table.gObjectClassNames.12"),
    localization::Tr("editor.table.gObjectClassNames.13"),
    localization::Tr("editor.table.gObjectClassNames.14"),
    localization::Tr("editor.table.gObjectClassNames.15")
};
// stpenew.bin: from scratch, random, cancel.
DATA(0x00480620) H2_CONST char* gSetupNewMapHelp[SETUP_NEW_MAP_HELP_COUNT] = {
    localization::Tr("editor.table.gSetupNewMapHelp.0"),
    localization::Tr("editor.table.gSetupNewMapHelp.1"),
    localization::Tr("editor.table.gSetupNewMapHelp.2")
};
// stpesize.bin: the four map sizes, cancel.
DATA(0x0048062c) H2_CONST char* gSetupMapSizeHelp[SETUP_MAP_SIZE_HELP_COUNT] = {
    localization::Tr("editor.table.gSetupMapSizeHelp.0"),
    localization::Tr("editor.table.gSetupMapSizeHelp.1"),
    localization::Tr("editor.table.gSetupMapSizeHelp.2"),
    localization::Tr("editor.table.gSetupMapSizeHelp.3"),
    localization::Tr("editor.table.gSetupMapSizeHelp.4")
};
// stpemain.bin: new map, load map, quit.
DATA(0x00480640) H2_CONST char* gSetupMainHelp[SETUP_MAIN_HELP_COUNT] = {
    localization::Tr("editor.table.gSetupMainHelp.0"),
    localization::Tr("editor.table.gSetupMainHelp.1"),
    localization::Tr("editor.table.gSetupMainHelp.2")
};
// How often a timed event repeats.
DATA(0x0048064c) H2_CONST char* gEventFrequencyNames[EVENT_FREQUENCY_COUNT] = {
    localization::Tr("editor.table.gEventFrequencyNames.0"),
    localization::Tr("editor.table.gEventFrequencyNames.1"),
    localization::Tr("editor.table.gEventFrequencyNames.2"),
    localization::Tr("editor.table.gEventFrequencyNames.3"),
    localization::Tr("editor.table.gEventFrequencyNames.4"),
    localization::Tr("editor.table.gEventFrequencyNames.5"),
    localization::Tr("editor.table.gEventFrequencyNames.6"),
    localization::Tr("editor.table.gEventFrequencyNames.7"),
    localization::Tr("editor.table.gEventFrequencyNames.8"),
    localization::Tr("editor.table.gEventFrequencyNames.9"),
    localization::Tr("editor.table.gEventFrequencyNames.10")
};
// The names a new town draws from.
DATA(0x00480678) H2_CONST char* gTownNames[EDITOR_TOWN_NAME_COUNT] = {
    localization::Tr("editor.table.gTownNames.0"),
    localization::Tr("editor.table.gTownNames.1"),
    localization::Tr("editor.table.gTownNames.2"),
    localization::Tr("editor.table.gTownNames.3"),
    localization::Tr("editor.table.gTownNames.4"),
    localization::Tr("editor.table.gTownNames.5"),
    localization::Tr("editor.table.gTownNames.6"),
    localization::Tr("editor.table.gTownNames.7"),
    localization::Tr("editor.table.gTownNames.8"),
    localization::Tr("editor.table.gTownNames.9"),
    localization::Tr("editor.table.gTownNames.10"),
    localization::Tr("editor.table.gTownNames.11"),
    localization::Tr("editor.table.gTownNames.12"),
    localization::Tr("editor.table.gTownNames.13"),
    localization::Tr("editor.table.gTownNames.14"),
    localization::Tr("editor.table.gTownNames.15"),
    localization::Tr("editor.table.gTownNames.16"),
    localization::Tr("editor.table.gTownNames.17"),
    localization::Tr("editor.table.gTownNames.18"),
    localization::Tr("editor.table.gTownNames.19"),
    localization::Tr("editor.table.gTownNames.20"),
    localization::Tr("editor.table.gTownNames.21"),
    localization::Tr("editor.table.gTownNames.22"),
    localization::Tr("editor.table.gTownNames.23"),
    localization::Tr("editor.table.gTownNames.24"),
    localization::Tr("editor.table.gTownNames.25"),
    localization::Tr("editor.table.gTownNames.26"),
    localization::Tr("editor.table.gTownNames.27"),
    localization::Tr("editor.table.gTownNames.28"),
    localization::Tr("editor.table.gTownNames.29"),
    localization::Tr("editor.table.gTownNames.30"),
    localization::Tr("editor.table.gTownNames.31"),
    localization::Tr("editor.table.gTownNames.32"),
    localization::Tr("editor.table.gTownNames.33"),
    localization::Tr("editor.table.gTownNames.34"),
    localization::Tr("editor.table.gTownNames.35"),
    localization::Tr("editor.table.gTownNames.36"),
    localization::Tr("editor.table.gTownNames.37"),
    localization::Tr("editor.table.gTownNames.38"),
    localization::Tr("editor.table.gTownNames.39"),
    localization::Tr("editor.table.gTownNames.40"),
    localization::Tr("editor.table.gTownNames.41"),
    localization::Tr("editor.table.gTownNames.42"),
    localization::Tr("editor.table.gTownNames.43"),
    localization::Tr("editor.table.gTownNames.44"),
    localization::Tr("editor.table.gTownNames.45"),
    localization::Tr("editor.table.gTownNames.46"),
    localization::Tr("editor.table.gTownNames.47"),
    localization::Tr("editor.table.gTownNames.48"),
    localization::Tr("editor.table.gTownNames.49"),
    localization::Tr("editor.table.gTownNames.50"),
    localization::Tr("editor.table.gTownNames.51"),
    localization::Tr("editor.table.gTownNames.52"),
    localization::Tr("editor.table.gTownNames.53"),
    localization::Tr("editor.table.gTownNames.54"),
    localization::Tr("editor.table.gTownNames.55"),
    localization::Tr("editor.table.gTownNames.56"),
    localization::Tr("editor.table.gTownNames.57"),
    localization::Tr("editor.table.gTownNames.58"),
    localization::Tr("editor.table.gTownNames.59"),
    localization::Tr("editor.table.gTownNames.60"),
    localization::Tr("editor.table.gTownNames.61"),
    localization::Tr("editor.table.gTownNames.62"),
    localization::Tr("editor.table.gTownNames.63"),
    localization::Tr("editor.table.gTownNames.64"),
    localization::Tr("editor.table.gTownNames.65"),
    localization::Tr("editor.table.gTownNames.66"),
    localization::Tr("editor.table.gTownNames.67"),
    localization::Tr("editor.table.gTownNames.68"),
    localization::Tr("editor.table.gTownNames.69"),
    localization::Tr("editor.table.gTownNames.70"),
    localization::Tr("editor.table.gTownNames.71")
};
// The file options menu.
DATA(0x00480798) H2_CONST char* gFileMenuHelp[EDIT_FILE_MENU_HELP_COUNT] = {
    localization::Tr("editor.table.gFileMenuHelp.0"),
    localization::Tr("editor.table.gFileMenuHelp.1"),
    localization::Tr("editor.table.gFileMenuHelp.2"),
    localization::Tr("editor.table.gFileMenuHelp.3"),
    localization::Tr("editor.table.gFileMenuHelp.4")
};
// The editor's system options.
DATA(0x004807ac) H2_CONST char* gSystemOptionsHelp[EDIT_SYSTEM_OPTIONS_HELP_COUNT] = {
    localization::Tr("editor.table.gSystemOptionsHelp.0"),
    localization::Tr("editor.table.gSystemOptionsHelp.1"),
    localization::Tr("editor.table.gSystemOptionsHelp.2"),
    localization::Tr("editor.table.gSystemOptionsHelp.3"),
    localization::Tr("editor.table.gSystemOptionsHelp.4")
};
// The special victory conditions.
DATA(0x004807c0) H2_CONST char* gVictoryConditionNames[SPEC_VICTORY_CONDITION_COUNT] = {
    localization::Tr("editor.table.gVictoryConditionNames.0"),
    localization::Tr("editor.table.gVictoryConditionNames.1"),
    localization::Tr("editor.table.gVictoryConditionNames.2"),
    localization::Tr("editor.table.gVictoryConditionNames.3"),
    localization::Tr("editor.table.gVictoryConditionNames.4"),
    localization::Tr("editor.table.gVictoryConditionNames.5")
};
// The special loss conditions.
DATA(0x004807d8) H2_CONST char* gLossConditionNames[SPEC_LOSS_CONDITION_COUNT] = {
    localization::Tr("editor.table.gLossConditionNames.0"),
    localization::Tr("editor.table.gLossConditionNames.1"),
    localization::Tr("editor.table.gLossConditionNames.2"),
    localization::Tr("editor.table.gLossConditionNames.3")
};
// The editor dialogs' captions (SetWinText): dialog, widget id and text.
DATA(0x004807e8) SWinSetup gWinSetup[EDITOR_DIALOG_WIN_SETUP_COUNT] = {
    {EDITOR_WIN_TEXT_ULTIMATE_ARTIFACT, 500, localization::Tr("editor.table.gWinSetup.0")},
    {EDITOR_WIN_TEXT_SYSTEM_OPTIONS, 100, localization::Tr("editor.table.gWinSetup.1")},
    {EDITOR_WIN_TEXT_SYSTEM_OPTIONS, 101, localization::Tr("editor.table.gWinSetup.2")},
    {EDITOR_WIN_TEXT_SYSTEM_OPTIONS, 103, localization::Tr("editor.table.gWinSetup.3")},
    {EDITOR_WIN_TEXT_SYSTEM_OPTIONS, 105, localization::Tr("editor.table.gWinSetup.4")},
    {EDITOR_WIN_TEXT_EVENT, 100, localization::Tr("editor.table.gWinSetup.5")},
    {EDITOR_WIN_TEXT_EVENT, 101, localization::Tr("editor.table.gWinSetup.6")},
    {EDITOR_WIN_TEXT_EVENT, 102, localization::Tr("editor.table.gWinSetup.7")},
    {EDITOR_WIN_TEXT_EVENT, 103, localization::Tr("editor.table.gWinSetup.8")},
    {EDITOR_WIN_TEXT_EVENT, 104, localization::Tr("editor.table.gWinSetup.9")},
    {EDITOR_WIN_TEXT_EVENT, 105, localization::Tr("editor.table.gWinSetup.10")},
    {EDITOR_WIN_TEXT_EVENT, 106, localization::Tr("editor.table.gWinSetup.11")},
    {EDITOR_WIN_TEXT_EVENT, 107, localization::Tr("editor.table.gWinSetup.12")},
    {EDITOR_WIN_TEXT_EVENT, 108, localization::Tr("editor.table.gWinSetup.13")},
    {EDITOR_WIN_TEXT_EVENT, 109, localization::Tr("editor.table.gWinSetup.14")},
    {EDITOR_WIN_TEXT_EVENT, 400, localization::Tr("editor.table.gWinSetup.15")},
    {EDITOR_WIN_TEXT_EVENT, 420, localization::Tr("editor.table.gWinSetup.16")},
    {EDITOR_WIN_TEXT_EVENT, 300, localization::Tr("editor.table.gWinSetup.17")},
    {EDITOR_WIN_TEXT_EVENT, 305, localization::Tr("editor.table.gWinSetup.18")},
    {EDITOR_WIN_TEXT_EVENT, 302, localization::Tr("editor.table.gWinSetup.19")},
    {EDITOR_WIN_TEXT_EVENT, 600, localization::Tr("editor.table.gWinSetup.20")},
    {EDITOR_WIN_TEXT_HERO, 100, localization::Tr("editor.table.gWinSetup.21")},
    {EDITOR_WIN_TEXT_HERO, 200, localization::Tr("editor.table.gWinSetup.22")},
    {EDITOR_WIN_TEXT_HERO, 201, localization::Tr("editor.table.gWinSetup.23")},
    {EDITOR_WIN_TEXT_HERO, 202, localization::Tr("editor.table.gWinSetup.24")},
    {EDITOR_WIN_TEXT_HERO, 210, localization::Tr("editor.table.gWinSetup.25")},
    {EDITOR_WIN_TEXT_HERO, 211, localization::Tr("editor.table.gWinSetup.26")},
    {EDITOR_WIN_TEXT_HERO, 212, localization::Tr("editor.table.gWinSetup.27")},
    {EDITOR_WIN_TEXT_HERO, 213, localization::Tr("editor.table.gWinSetup.28")},
    {EDITOR_WIN_TEXT_HERO, 214, localization::Tr("editor.table.gWinSetup.29")},
    {EDITOR_WIN_TEXT_HERO, 215, localization::Tr("editor.table.gWinSetup.30")},
    {EDITOR_WIN_TEXT_HERO, 216, localization::Tr("editor.table.gWinSetup.31")},
    {EDITOR_WIN_TEXT_HERO, 300, localization::Tr("editor.table.gWinSetup.32")},
    {EDITOR_WIN_TEXT_HERO, 304, localization::Tr("editor.table.gWinSetup.33")},
    {EDITOR_WIN_TEXT_HERO, 305, localization::Tr("editor.table.gWinSetup.34")},
    {EDITOR_WIN_TEXT_HERO, 306, localization::Tr("editor.table.gWinSetup.35")},
    {EDITOR_WIN_TEXT_HERO, 800, localization::Tr("editor.table.gWinSetup.36")},
    {EDITOR_WIN_TEXT_HERO, 400, localization::Tr("editor.table.gWinSetup.37")},
    {EDITOR_WIN_TEXT_HERO, 500, localization::Tr("editor.table.gWinSetup.38")},
    {EDITOR_WIN_TEXT_HERO, 501, localization::Tr("editor.table.gWinSetup.39")},
    {EDITOR_WIN_TEXT_HERO, 502, localization::Tr("editor.table.gWinSetup.40")},
    {EDITOR_WIN_TEXT_HERO, 510, localization::Tr("editor.table.gWinSetup.41")},
    {EDITOR_WIN_TEXT_HERO, 511, localization::Tr("editor.table.gWinSetup.42")},
    {EDITOR_WIN_TEXT_HERO, 512, localization::Tr("editor.table.gWinSetup.43")},
    {EDITOR_WIN_TEXT_HERO, 513, localization::Tr("editor.table.gWinSetup.44")},
    {EDITOR_WIN_TEXT_HERO, 514, localization::Tr("editor.table.gWinSetup.45")},
    {EDITOR_WIN_TEXT_HERO, 515, localization::Tr("editor.table.gWinSetup.46")},
    {EDITOR_WIN_TEXT_HERO, 516, localization::Tr("editor.table.gWinSetup.47")},
    {EDITOR_WIN_TEXT_HERO, 517, localization::Tr("editor.table.gWinSetup.48")},
    {EDITOR_WIN_TEXT_HERO, 600, localization::Tr("editor.table.gWinSetup.49")},
    {EDITOR_WIN_TEXT_HERO, 601, localization::Tr("editor.table.gWinSetup.50")},
    {EDITOR_WIN_TEXT_HERO, 602, localization::Tr("editor.table.gWinSetup.51")},
    {EDITOR_WIN_TEXT_HERO, 700, localization::Tr("editor.table.gWinSetup.52")},
    {EDITOR_WIN_TEXT_MONSTER, 500, localization::Tr("editor.table.gWinSetup.53")},
    {EDITOR_WIN_TEXT_SPHINX, 100, localization::Tr("editor.table.gWinSetup.54")},
    {EDITOR_WIN_TEXT_SPHINX, 101, localization::Tr("editor.table.gWinSetup.55")},
    {EDITOR_WIN_TEXT_SPHINX, 102, localization::Tr("editor.table.gWinSetup.56")},
    {EDITOR_WIN_TEXT_SPHINX, 103, localization::Tr("editor.table.gWinSetup.57")},
    {EDITOR_WIN_TEXT_SPHINX, 104, localization::Tr("editor.table.gWinSetup.58")},
    {EDITOR_WIN_TEXT_SPHINX, 105, localization::Tr("editor.table.gWinSetup.59")},
    {EDITOR_WIN_TEXT_SPHINX, 106, localization::Tr("editor.table.gWinSetup.60")},
    {EDITOR_WIN_TEXT_SPHINX, 107, localization::Tr("editor.table.gWinSetup.61")},
    {EDITOR_WIN_TEXT_SPHINX, 108, localization::Tr("editor.table.gWinSetup.62")},
    {EDITOR_WIN_TEXT_SPHINX, 109, localization::Tr("editor.table.gWinSetup.63")},
    {EDITOR_WIN_TEXT_SPHINX, 300, localization::Tr("editor.table.gWinSetup.64")},
    {EDITOR_WIN_TEXT_SPHINX, 400, localization::Tr("editor.table.gWinSetup.65")},
    {EDITOR_WIN_TEXT_RUMOUR, 100, localization::Tr("editor.table.gWinSetup.66")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 200, localization::Tr("editor.table.gWinSetup.67")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 220, localization::Tr("editor.table.gWinSetup.68")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 221, localization::Tr("editor.table.gWinSetup.69")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 250, localization::Tr("editor.table.gWinSetup.70")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 300, localization::Tr("editor.table.gWinSetup.71")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 320, localization::Tr("editor.table.gWinSetup.72")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 400, localization::Tr("editor.table.gWinSetup.73")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 401, localization::Tr("editor.table.gWinSetup.74")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 500, localization::Tr("editor.table.gWinSetup.75")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 600, localization::Tr("editor.table.gWinSetup.76")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 610, localization::Tr("editor.table.gWinSetup.77")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 611, localization::Tr("editor.table.gWinSetup.78")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 612, localization::Tr("editor.table.gWinSetup.79")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 613, localization::Tr("editor.table.gWinSetup.80")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 100, localization::Tr("editor.table.gWinSetup.81")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 700, localization::Tr("editor.table.gWinSetup.82")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 800, localization::Tr("editor.table.gWinSetup.83")},
    {EDITOR_WIN_TEXT_SPECIFICATIONS, 900, localization::Tr("editor.table.gWinSetup.84")},
    {EDITOR_WIN_TEXT_TOWN, 100, localization::Tr("editor.table.gWinSetup.85")},
    {EDITOR_WIN_TEXT_TOWN, 200, localization::Tr("editor.table.gWinSetup.86")},
    {EDITOR_WIN_TEXT_TOWN, 201, localization::Tr("editor.table.gWinSetup.87")},
    {EDITOR_WIN_TEXT_TOWN, 202, localization::Tr("editor.table.gWinSetup.88")},
    {EDITOR_WIN_TEXT_TOWN, 210, localization::Tr("editor.table.gWinSetup.89")},
    {EDITOR_WIN_TEXT_TOWN, 211, localization::Tr("editor.table.gWinSetup.90")},
    {EDITOR_WIN_TEXT_TOWN, 212, localization::Tr("editor.table.gWinSetup.91")},
    {EDITOR_WIN_TEXT_TOWN, 213, localization::Tr("editor.table.gWinSetup.92")},
    {EDITOR_WIN_TEXT_TOWN, 214, localization::Tr("editor.table.gWinSetup.93")},
    {EDITOR_WIN_TEXT_TOWN, 215, localization::Tr("editor.table.gWinSetup.94")},
    {EDITOR_WIN_TEXT_TOWN, 216, localization::Tr("editor.table.gWinSetup.95")},
    {EDITOR_WIN_TEXT_TOWN, 600, localization::Tr("editor.table.gWinSetup.96")},
    {EDITOR_WIN_TEXT_TOWN, 601, localization::Tr("editor.table.gWinSetup.97")},
    {EDITOR_WIN_TEXT_TOWN, 602, localization::Tr("editor.table.gWinSetup.98")},
    {EDITOR_WIN_TEXT_TOWN, 300, localization::Tr("editor.table.gWinSetup.99")},
    {EDITOR_WIN_TEXT_TOWN, 310, localization::Tr("editor.table.gWinSetup.100")},
    {EDITOR_WIN_TEXT_TOWN, 400, localization::Tr("editor.table.gWinSetup.101")},
    {EDITOR_WIN_TEXT_TOWN, 401, localization::Tr("editor.table.gWinSetup.102")},
    {EDITOR_WIN_TEXT_TOWN, 402, localization::Tr("editor.table.gWinSetup.103")},
    {EDITOR_WIN_TEXT_TOWN, 470, localization::Tr("editor.table.gWinSetup.104")},
    {EDITOR_WIN_TEXT_TOWN, 510, localization::Tr("editor.table.gWinSetup.105")},
    {EDITOR_WIN_TEXT_TOWN, 512, localization::Tr("editor.table.gWinSetup.106")},
    {EDITOR_WIN_TEXT_TOWN, 513, localization::Tr("editor.table.gWinSetup.107")},
    {EDITOR_WIN_TEXT_TOWN, 514, localization::Tr("editor.table.gWinSetup.108")},
    {EDITOR_WIN_TEXT_TOWN, 515, localization::Tr("editor.table.gWinSetup.109")},
    {EDITOR_WIN_TEXT_TOWN, 516, localization::Tr("editor.table.gWinSetup.110")},
    {EDITOR_WIN_TEXT_TOWN, 517, localization::Tr("editor.table.gWinSetup.111")},
    {EDITOR_WIN_TEXT_TOWN, 518, localization::Tr("editor.table.gWinSetup.112")},
    {EDITOR_WIN_TEXT_TOWN, 519, localization::Tr("editor.table.gWinSetup.113")},
    {EDITOR_WIN_TEXT_TOWN, 520, localization::Tr("editor.table.gWinSetup.114")},
    {EDITOR_WIN_TEXT_TOWN, 521, localization::Tr("editor.table.gWinSetup.115")}
};
// KB.cpp's text tables, in KB.cpp's order.
DATA(0x00480b14) H2_CONST char* gArtifactNames[IDX(ARTIFACT_COUNT)] = {
    localization::Tr("table.gArtifactNames.0"),
    localization::Tr("table.gArtifactNames.1"),
    localization::Tr("table.gArtifactNames.2"),
    localization::Tr("table.gArtifactNames.3"),
    localization::Tr("table.gArtifactNames.4"),
    localization::Tr("table.gArtifactNames.5"),
    localization::Tr("table.gArtifactNames.6"),
    localization::Tr("table.gArtifactNames.7"),
    localization::Tr("table.gArtifactNames.8"),
    localization::Tr("table.gArtifactNames.9"),
    localization::Tr("table.gArtifactNames.10"),
    localization::Tr("table.gArtifactNames.11"),
    localization::Tr("table.gArtifactNames.12"),
    localization::Tr("table.gArtifactNames.13"),
    localization::Tr("table.gArtifactNames.14"),
    localization::Tr("table.gArtifactNames.15"),
    localization::Tr("table.gArtifactNames.16"),
    localization::Tr("table.gArtifactNames.17"),
    localization::Tr("table.gArtifactNames.18"),
    localization::Tr("table.gArtifactNames.19"),
    localization::Tr("table.gArtifactNames.20"),
    localization::Tr("table.gArtifactNames.21"),
    localization::Tr("table.gArtifactNames.22"),
    localization::Tr("table.gArtifactNames.23"),
    localization::Tr("table.gArtifactNames.24"),
    localization::Tr("table.gArtifactNames.25"),
    localization::Tr("table.gArtifactNames.26"),
    localization::Tr("table.gArtifactNames.27"),
    localization::Tr("table.gArtifactNames.28"),
    localization::Tr("table.gArtifactNames.29"),
    localization::Tr("table.gArtifactNames.30"),
    localization::Tr("table.gArtifactNames.31"),
    localization::Tr("table.gArtifactNames.32"),
    localization::Tr("table.gArtifactNames.33"),
    localization::Tr("table.gArtifactNames.34"),
    localization::Tr("table.gArtifactNames.35"),
    localization::Tr("table.gArtifactNames.36"),
    localization::Tr("table.gArtifactNames.37"),
    localization::Tr("table.gArtifactNames.38"),
    localization::Tr("table.gArtifactNames.39"),
    localization::Tr("table.gArtifactNames.40"),
    localization::Tr("table.gArtifactNames.41"),
    localization::Tr("table.gArtifactNames.42"),
    localization::Tr("table.gArtifactNames.43"),
    localization::Tr("table.gArtifactNames.44"),
    localization::Tr("table.gArtifactNames.45"),
    localization::Tr("table.gArtifactNames.46"),
    localization::Tr("table.gArtifactNames.47"),
    localization::Tr("table.gArtifactNames.48"),
    localization::Tr("table.gArtifactNames.49"),
    localization::Tr("table.gArtifactNames.50"),
    localization::Tr("table.gArtifactNames.51"),
    localization::Tr("table.gArtifactNames.52"),
    localization::Tr("table.gArtifactNames.53"),
    localization::Tr("table.gArtifactNames.54"),
    localization::Tr("table.gArtifactNames.55"),
    localization::Tr("table.gArtifactNames.56"),
    localization::Tr("table.gArtifactNames.57"),
    localization::Tr("table.gArtifactNames.58"),
    localization::Tr("table.gArtifactNames.59"),
    localization::Tr("table.gArtifactNames.60"),
    localization::Tr("table.gArtifactNames.61"),
    localization::Tr("table.gArtifactNames.62"),
    localization::Tr("table.gArtifactNames.63"),
    localization::Tr("table.gArtifactNames.64"),
    localization::Tr("table.gArtifactNames.65"),
    localization::Tr("table.gArtifactNames.66"),
    localization::Tr("table.gArtifactNames.67"),
    localization::Tr("table.gArtifactNames.68"),
    localization::Tr("table.gArtifactNames.69"),
    localization::Tr("table.gArtifactNames.70"),
    localization::Tr("table.gArtifactNames.71"),
    localization::Tr("table.gArtifactNames.72"),
    localization::Tr("table.gArtifactNames.73"),
    localization::Tr("table.gArtifactNames.74"),
    localization::Tr("table.gArtifactNames.75"),
    localization::Tr("table.gArtifactNames.76"),
    localization::Tr("table.gArtifactNames.77"),
    localization::Tr("table.gArtifactNames.78"),
    localization::Tr("table.gArtifactNames.79"),
    localization::Tr("table.gArtifactNames.80"),
    localization::Tr("table.gArtifactNames.81"),
    "ERROR : Artifact 82" /* "ERROR : Artifact 82" */,
    "ERROR : Artifact 83" /* "ERROR : Artifact 83" */,
    "ERROR : Artifact 84" /* "ERROR : Artifact 84" */,
    "ERROR : Artifact 85" /* "ERROR : Artifact 85" */,
    localization::Tr("table.gArtifactNames.86"),
    localization::Tr("table.gArtifactNames.87"),
    localization::Tr("table.gArtifactNames.88"),
    localization::Tr("table.gArtifactNames.89"),
    localization::Tr("table.gArtifactNames.90"),
    localization::Tr("table.gArtifactNames.91"),
    localization::Tr("table.gArtifactNames.92"),
    localization::Tr("table.gArtifactNames.93"),
    localization::Tr("table.gArtifactNames.94"),
    localization::Tr("table.gArtifactNames.95"),
    localization::Tr("table.gArtifactNames.96"),
    localization::Tr("table.gArtifactNames.97"),
    localization::Tr("table.gArtifactNames.98"),
    localization::Tr("table.gArtifactNames.99"),
    localization::Tr("table.gArtifactNames.100"),
    localization::Tr("table.gArtifactNames.101"),
    localization::Tr("table.gArtifactNames.102")
};
DATA(0x00480cb0) H2_CONST char* gArtifactDesc[IDX(ARTIFACT_COUNT)] = {
    localization::Tr("table.gArtifactDesc.0"),
    localization::Tr("table.gArtifactDesc.1"),
    localization::Tr("table.gArtifactDesc.2"),
    localization::Tr("table.gArtifactDesc.3"),
    localization::Tr("table.gArtifactDesc.4"),
    localization::Tr("table.gArtifactDesc.5"),
    localization::Tr("table.gArtifactDesc.6"),
    localization::Tr("table.gArtifactDesc.7"),
    localization::Tr("table.gArtifactDesc.8"),
    localization::Tr("table.gArtifactDesc.9"),
    localization::Tr("table.gArtifactDesc.10"),
    localization::Tr("table.gArtifactDesc.11"),
    localization::Tr("table.gArtifactDesc.12"),
    localization::Tr("table.gArtifactDesc.13"),
    localization::Tr("table.gArtifactDesc.14"),
    localization::Tr("table.gArtifactDesc.15"),
    localization::Tr("table.gArtifactDesc.16"),
    localization::Tr("table.gArtifactDesc.17"),
    localization::Tr("table.gArtifactDesc.18"),
    localization::Tr("table.gArtifactDesc.19"),
    localization::Tr("table.gArtifactDesc.20"),
    localization::Tr("table.gArtifactDesc.21"),
    localization::Tr("table.gArtifactDesc.22"),
    localization::Tr("table.gArtifactDesc.23"),
    localization::Tr("table.gArtifactDesc.24"),
    localization::Tr("table.gArtifactDesc.25"),
    localization::Tr("table.gArtifactDesc.26"),
    localization::Tr("table.gArtifactDesc.27"),
    localization::Tr("table.gArtifactDesc.28"),
    localization::Tr("table.gArtifactDesc.29"),
    localization::Tr("table.gArtifactDesc.30"),
    localization::Tr("table.gArtifactDesc.31"),
    localization::Tr("table.gArtifactDesc.32"),
    localization::Tr("table.gArtifactDesc.33"),
    localization::Tr("table.gArtifactDesc.34"),
    localization::Tr("table.gArtifactDesc.35"),
    localization::Tr("table.gArtifactDesc.36"),
    localization::Tr("table.gArtifactDesc.37"),
    localization::Tr("table.gArtifactDesc.38"),
    localization::Tr("table.gArtifactDesc.39"),
    localization::Tr("table.gArtifactDesc.40"),
    localization::Tr("table.gArtifactDesc.41"),
    localization::Tr("table.gArtifactDesc.42"),
    localization::Tr("table.gArtifactDesc.43"),
    localization::Tr("table.gArtifactDesc.44"),
    localization::Tr("table.gArtifactDesc.45"),
    localization::Tr("table.gArtifactDesc.46"),
    localization::Tr("table.gArtifactDesc.47"),
    localization::Tr("table.gArtifactDesc.48"),
    localization::Tr("table.gArtifactDesc.49"),
    localization::Tr("table.gArtifactDesc.50"),
    localization::Tr("table.gArtifactDesc.51"),
    localization::Tr("table.gArtifactDesc.52"),
    localization::Tr("table.gArtifactDesc.53"),
    localization::Tr("table.gArtifactDesc.54"),
    localization::Tr("table.gArtifactDesc.55"),
    localization::Tr("table.gArtifactDesc.56"),
    localization::Tr("table.gArtifactDesc.57"),
    localization::Tr("table.gArtifactDesc.58"),
    localization::Tr("table.gArtifactDesc.59"),
    localization::Tr("table.gArtifactDesc.60"),
    localization::Tr("table.gArtifactDesc.61"),
    localization::Tr("table.gArtifactDesc.62"),
    localization::Tr("table.gArtifactDesc.63"),
    localization::Tr("table.gArtifactDesc.64"),
    localization::Tr("table.gArtifactDesc.65"),
    localization::Tr("table.gArtifactDesc.66"),
    localization::Tr("table.gArtifactDesc.67"),
    localization::Tr("table.gArtifactDesc.68"),
    localization::Tr("table.gArtifactDesc.69"),
    localization::Tr("table.gArtifactDesc.70"),
    localization::Tr("table.gArtifactDesc.71"),
    localization::Tr("table.gArtifactDesc.72"),
    localization::Tr("table.gArtifactDesc.73"),
    localization::Tr("table.gArtifactDesc.74"),
    localization::Tr("table.gArtifactDesc.75"),
    localization::Tr("table.gArtifactDesc.76"),
    localization::Tr("table.gArtifactDesc.77"),
    localization::Tr("table.gArtifactDesc.78"),
    localization::Tr("table.gArtifactDesc.79"),
    localization::Tr("table.gArtifactDesc.80"),
    localization::Tr("table.gArtifactDesc.81"),
    "{ERROR}\n\nArtifact 82." /* "{ERROR}\n\nArtifact 82." */,
    "{ERROR}\n\nArtifact 83." /* "{ERROR}\n\nArtifact 83." */,
    "{ERROR}\n\nArtifact 84." /* "{ERROR}\n\nArtifact 84." */,
    "{ERROR}\n\nArtifact 85." /* "{ERROR}\n\nArtifact 85." */,
    localization::Tr("table.gArtifactDesc.86"),
    localization::Tr("table.gArtifactDesc.87"),
    localization::Tr("table.gArtifactDesc.88"),
    localization::Tr("table.gArtifactDesc.89"),
    localization::Tr("table.gArtifactDesc.90"),
    localization::Tr("table.gArtifactDesc.91"),
    localization::Tr("table.gArtifactDesc.92"),
    localization::Tr("table.gArtifactDesc.93"),
    localization::Tr("table.gArtifactDesc.94"),
    localization::Tr("table.gArtifactDesc.95"),
    localization::Tr("table.gArtifactDesc.96"),
    localization::Tr("table.gArtifactDesc.97"),
    localization::Tr("table.gArtifactDesc.98"),
    localization::Tr("table.gArtifactDesc.99"),
    localization::Tr("table.gArtifactDesc.100"),
    localization::Tr("table.gArtifactDesc.101"),
    localization::Tr("table.gArtifactDesc.102")};
DATA(0x00480e4c) H2_CONST char* gArtifactEvent[IDX(ARTIFACT_COUNT)] = {
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    "" /* "" */,
    localization::Tr("table.gArtifactEvent.8"),
    localization::Tr("table.gArtifactEvent.9"),
    localization::Tr("table.gArtifactEvent.10"),
    localization::Tr("table.gArtifactEvent.11"),
    localization::Tr("table.gArtifactEvent.12"),
    localization::Tr("table.gArtifactEvent.13"),
    localization::Tr("table.gArtifactEvent.14"),
    localization::Tr("table.gArtifactEvent.15"),
    localization::Tr("table.gArtifactEvent.16"),
    localization::Tr("table.gArtifactEvent.17"),
    localization::Tr("table.gArtifactEvent.18"),
    localization::Tr("table.gArtifactEvent.19"),
    localization::Tr("table.gArtifactEvent.20"),
    localization::Tr("table.gArtifactEvent.21"),
    localization::Tr("table.gArtifactEvent.22"),
    localization::Tr("table.gArtifactEvent.23"),
    localization::Tr("table.gArtifactEvent.24"),
    localization::Tr("table.gArtifactEvent.25"),
    localization::Tr("table.gArtifactEvent.26"),
    localization::Tr("table.gArtifactEvent.27"),
    localization::Tr("table.gArtifactEvent.28"),
    localization::Tr("table.gArtifactEvent.29"),
    localization::Tr("table.gArtifactEvent.30"),
    localization::Tr("table.gArtifactEvent.31"),
    localization::Tr("table.gArtifactEvent.32"),
    localization::Tr("table.gArtifactEvent.33"),
    localization::Tr("table.gArtifactEvent.34"),
    localization::Tr("table.gArtifactEvent.35"),
    localization::Tr("table.gArtifactEvent.36"),
    localization::Tr("table.gArtifactEvent.37"),
    localization::Tr("table.gArtifactEvent.38"),
    localization::Tr("table.gArtifactEvent.39"),
    localization::Tr("table.gArtifactEvent.40"),
    localization::Tr("table.gArtifactEvent.41"),
    localization::Tr("table.gArtifactEvent.42"),
    localization::Tr("table.gArtifactEvent.43"),
    localization::Tr("table.gArtifactEvent.44"),
    localization::Tr("table.gArtifactEvent.45"),
    localization::Tr("table.gArtifactEvent.46"),
    localization::Tr("table.gArtifactEvent.47"),
    localization::Tr("table.gArtifactEvent.48"),
    localization::Tr("table.gArtifactEvent.49"),
    localization::Tr("table.gArtifactEvent.50"),
    localization::Tr("table.gArtifactEvent.51"),
    localization::Tr("table.gArtifactEvent.52"),
    localization::Tr("table.gArtifactEvent.53"),
    localization::Tr("table.gArtifactEvent.54"),
    localization::Tr("table.gArtifactEvent.55"),
    localization::Tr("table.gArtifactEvent.56"),
    localization::Tr("table.gArtifactEvent.57"),
    localization::Tr("table.gArtifactEvent.58"),
    localization::Tr("table.gArtifactEvent.59"),
    localization::Tr("table.gArtifactEvent.60"),
    localization::Tr("table.gArtifactEvent.61"),
    localization::Tr("table.gArtifactEvent.62"),
    localization::Tr("table.gArtifactEvent.63"),
    localization::Tr("table.gArtifactEvent.64"),
    localization::Tr("table.gArtifactEvent.65"),
    localization::Tr("table.gArtifactEvent.66"),
    localization::Tr("table.gArtifactEvent.67"),
    localization::Tr("table.gArtifactEvent.68"),
    localization::Tr("table.gArtifactEvent.69"),
    localization::Tr("table.gArtifactEvent.70"),
    localization::Tr("table.gArtifactEvent.71"),
    localization::Tr("table.gArtifactEvent.72"),
    localization::Tr("table.gArtifactEvent.73"),
    localization::Tr("table.gArtifactEvent.74"),
    localization::Tr("table.gArtifactEvent.75"),
    localization::Tr("table.gArtifactEvent.76"),
    localization::Tr("table.gArtifactEvent.77"),
    localization::Tr("table.gArtifactEvent.78"),
    localization::Tr("table.gArtifactEvent.79"),
    localization::Tr("table.gArtifactEvent.80"),
    "" /* "" */,
    "ERROR : Artifact event 82." /* "ERROR : Artifact event 82." */,
    "ERROR : Artifact event 83." /* "ERROR : Artifact event 83." */,
    "ERROR : Artifact event 84." /* "ERROR : Artifact event 84." */,
    "ERROR : Artifact event 85." /* "ERROR : Artifact event 85." */,
    localization::Tr("table.gArtifactEvent.86"),
    localization::Tr("table.gArtifactEvent.87"),
    localization::Tr("table.gArtifactEvent.88"),
    localization::Tr("table.gArtifactEvent.89"),
    localization::Tr("table.gArtifactEvent.90"),
    localization::Tr("table.gArtifactEvent.91"),
    localization::Tr("table.gArtifactEvent.92"),
    localization::Tr("table.gArtifactEvent.93"),
    localization::Tr("table.gArtifactEvent.94"),
    localization::Tr("table.gArtifactEvent.95"),
    localization::Tr("table.gArtifactEvent.96"),
    localization::Tr("table.gArtifactEvent.97"),
    localization::Tr("table.gArtifactEvent.98"),
    localization::Tr("table.gArtifactEvent.99"),
    localization::Tr("table.gArtifactEvent.100"),
    localization::Tr("table.gArtifactEvent.101"),
    localization::Tr("table.gArtifactEvent.102")};
DATA(0x00480fe8) H2_CONST char* gStatNames[HERO_PRIMARY_STAT_COUNT] = {
    localization::Tr("table.gStatNames.0"),
    localization::Tr("table.gStatNames.1"),
    localization::Tr("table.gStatNames.2"),
    localization::Tr("table.gStatNames.3")
};
DATA(0x00480ff8) H2_CONST char* gStatDesc[HERO_PRIMARY_STAT_COUNT] = {
    localization::Tr("table.gStatDesc.0"),
    localization::Tr("table.gStatDesc.1"),
    localization::Tr("table.gStatDesc.2"),
    localization::Tr("table.gStatDesc.3")
};
DATA(0x00481008) H2_CONST char* gAlignmentNames[KB_ALIGNMENT_NAME_COUNT] = {
    localization::Tr("table.gAlignmentNames.0"),
    localization::Tr("table.gAlignmentNames.1"),
    localization::Tr("table.gAlignmentNames.2"),
    localization::Tr("table.gAlignmentNames.3"),
    localization::Tr("table.gAlignmentNames.4"),
    localization::Tr("table.gAlignmentNames.5"),
    localization::Tr("table.gAlignmentNames.6"),
    localization::Tr("table.gAlignmentNames.7")
};
DATA(0x00481028) H2_CONST char* gArmyShortNames[IDX(CREATURE_COUNT)] = {
    "peasn",
    "archr",
    "arch2",
    "pikmn",
    "pikm2",
    "swman",
    "swma2",
    "cvlry",
    "cvlr2",
    "paldn",
    "pald2",
    "gobln",
    "orc__",
    "orc_2",
    "Wolf_",
    "Ogre_",
    "Ogre2",
    "Troll",
    "trol2",
    "cyclp",
    "sprit",
    "Dwarf",
    "dwar2",
    "elf__",
    "elf_2",
    "druid",
    "drui2",
    "uncrn",
    "phoen",
    "centr",
    "gargl",
    "griff",
    "mintr",
    "mint2",
    "Hydra",
    "dragn",
    "drag2",
    "drag3",
    "hlflg",
    "Boar_",
    "irong",
    "iron2",
    "roc__",
    "archm",
    "arch2",
    "titan",
    "tita2",
    "skel_",
    "zomb_",
    "zomb2",
    "Mummy",
    "mumm2",
    "vampr",
    "vamp2",
    "lich_",
    "lich2",
    "boned",
    "Rogue",
    "Nomad",
    "Ghost",
    "Genie",
    "medus",
    "eleme",
    "elema",
    "elemf",
    "elemw"
};
DATA(0x00481130) H2_CONST char* gArmyNames[IDX(CREATURE_COUNT)] = {
    localization::Tr("table.gArmyNames.0"),
    localization::Tr("table.gArmyNames.1"),
    localization::Tr("table.gArmyNames.2"),
    localization::Tr("table.gArmyNames.3"),
    localization::Tr("table.gArmyNames.4"),
    localization::Tr("table.gArmyNames.5"),
    localization::Tr("table.gArmyNames.6"),
    localization::Tr("table.gArmyNames.7"),
    localization::Tr("table.gArmyNames.8"),
    localization::Tr("table.gArmyNames.9"),
    localization::Tr("table.gArmyNames.10"),
    localization::Tr("table.gArmyNames.11"),
    localization::Tr("table.gArmyNames.12"),
    localization::Tr("table.gArmyNames.13"),
    localization::Tr("table.gArmyNames.14"),
    localization::Tr("table.gArmyNames.15"),
    localization::Tr("table.gArmyNames.16"),
    localization::Tr("table.gArmyNames.17"),
    localization::Tr("table.gArmyNames.18"),
    localization::Tr("table.gArmyNames.19"),
    localization::Tr("table.gArmyNames.20"),
    localization::Tr("table.gArmyNames.21"),
    localization::Tr("table.gArmyNames.22"),
    localization::Tr("table.gArmyNames.23"),
    localization::Tr("table.gArmyNames.24"),
    localization::Tr("table.gArmyNames.25"),
    localization::Tr("table.gArmyNames.26"),
    localization::Tr("table.gArmyNames.27"),
    localization::Tr("table.gArmyNames.28"),
    localization::Tr("table.gArmyNames.29"),
    localization::Tr("table.gArmyNames.30"),
    localization::Tr("table.gArmyNames.31"),
    localization::Tr("table.gArmyNames.32"),
    localization::Tr("table.gArmyNames.33"),
    localization::Tr("table.gArmyNames.34"),
    localization::Tr("table.gArmyNames.35"),
    localization::Tr("table.gArmyNames.36"),
    localization::Tr("table.gArmyNames.37"),
    localization::Tr("table.gArmyNames.38"),
    localization::Tr("table.gArmyNames.39"),
    localization::Tr("table.gArmyNames.40"),
    localization::Tr("table.gArmyNames.41"),
    localization::Tr("table.gArmyNames.42"),
    localization::Tr("table.gArmyNames.43"),
    localization::Tr("table.gArmyNames.44"),
    localization::Tr("table.gArmyNames.45"),
    localization::Tr("table.gArmyNames.46"),
    localization::Tr("table.gArmyNames.47"),
    localization::Tr("table.gArmyNames.48"),
    localization::Tr("table.gArmyNames.49"),
    localization::Tr("table.gArmyNames.50"),
    localization::Tr("table.gArmyNames.51"),
    localization::Tr("table.gArmyNames.52"),
    localization::Tr("table.gArmyNames.53"),
    localization::Tr("table.gArmyNames.54"),
    localization::Tr("table.gArmyNames.55"),
    localization::Tr("table.gArmyNames.56"),
    localization::Tr("table.gArmyNames.57"),
    localization::Tr("table.gArmyNames.58"),
    localization::Tr("table.gArmyNames.59"),
    localization::Tr("table.gArmyNames.60"),
    localization::Tr("table.gArmyNames.61"),
    localization::Tr("table.gArmyNames.62"),
    localization::Tr("table.gArmyNames.63"),
    localization::Tr("table.gArmyNames.64"),
    localization::Tr("table.gArmyNames.65")
};
DATA(0x00481238) H2_CONST char* gArmyNamesPlural[IDX(CREATURE_COUNT)] = {
    localization::Tr("table.gArmyNamesPlural.0"),
    localization::Tr("table.gArmyNamesPlural.1"),
    localization::Tr("table.gArmyNamesPlural.2"),
    localization::Tr("table.gArmyNamesPlural.3"),
    localization::Tr("table.gArmyNamesPlural.4"),
    localization::Tr("table.gArmyNamesPlural.5"),
    localization::Tr("table.gArmyNamesPlural.6"),
    localization::Tr("table.gArmyNamesPlural.7"),
    localization::Tr("table.gArmyNamesPlural.8"),
    localization::Tr("table.gArmyNamesPlural.9"),
    localization::Tr("table.gArmyNamesPlural.10"),
    localization::Tr("table.gArmyNamesPlural.11"),
    localization::Tr("table.gArmyNamesPlural.12"),
    localization::Tr("table.gArmyNamesPlural.13"),
    localization::Tr("table.gArmyNamesPlural.14"),
    localization::Tr("table.gArmyNamesPlural.15"),
    localization::Tr("table.gArmyNamesPlural.16"),
    localization::Tr("table.gArmyNamesPlural.17"),
    localization::Tr("table.gArmyNamesPlural.18"),
    localization::Tr("table.gArmyNamesPlural.19"),
    localization::Tr("table.gArmyNamesPlural.20"),
    localization::Tr("table.gArmyNamesPlural.21"),
    localization::Tr("table.gArmyNamesPlural.22"),
    localization::Tr("table.gArmyNamesPlural.23"),
    localization::Tr("table.gArmyNamesPlural.24"),
    localization::Tr("table.gArmyNamesPlural.25"),
    localization::Tr("table.gArmyNamesPlural.26"),
    localization::Tr("table.gArmyNamesPlural.27"),
    localization::Tr("table.gArmyNamesPlural.28"),
    localization::Tr("table.gArmyNamesPlural.29"),
    localization::Tr("table.gArmyNamesPlural.30"),
    localization::Tr("table.gArmyNamesPlural.31"),
    localization::Tr("table.gArmyNamesPlural.32"),
    localization::Tr("table.gArmyNamesPlural.33"),
    localization::Tr("table.gArmyNamesPlural.34"),
    localization::Tr("table.gArmyNamesPlural.35"),
    localization::Tr("table.gArmyNamesPlural.36"),
    localization::Tr("table.gArmyNamesPlural.37"),
    localization::Tr("table.gArmyNamesPlural.38"),
    localization::Tr("table.gArmyNamesPlural.39"),
    localization::Tr("table.gArmyNamesPlural.40"),
    localization::Tr("table.gArmyNamesPlural.41"),
    localization::Tr("table.gArmyNamesPlural.42"),
    localization::Tr("table.gArmyNamesPlural.43"),
    localization::Tr("table.gArmyNamesPlural.44"),
    localization::Tr("table.gArmyNamesPlural.45"),
    localization::Tr("table.gArmyNamesPlural.46"),
    localization::Tr("table.gArmyNamesPlural.47"),
    localization::Tr("table.gArmyNamesPlural.48"),
    localization::Tr("table.gArmyNamesPlural.49"),
    localization::Tr("table.gArmyNamesPlural.50"),
    localization::Tr("table.gArmyNamesPlural.51"),
    localization::Tr("table.gArmyNamesPlural.52"),
    localization::Tr("table.gArmyNamesPlural.53"),
    localization::Tr("table.gArmyNamesPlural.54"),
    localization::Tr("table.gArmyNamesPlural.55"),
    localization::Tr("table.gArmyNamesPlural.56"),
    localization::Tr("table.gArmyNamesPlural.57"),
    localization::Tr("table.gArmyNamesPlural.58"),
    localization::Tr("table.gArmyNamesPlural.59"),
    localization::Tr("table.gArmyNamesPlural.60"),
    localization::Tr("table.gArmyNamesPlural.61"),
    localization::Tr("table.gArmyNamesPlural.62"),
    localization::Tr("table.gArmyNamesPlural.63"),
    localization::Tr("table.gArmyNamesPlural.64"),
    localization::Tr("table.gArmyNamesPlural.65")
};
DATA(0x00481340) H2_CONST char* gTerrainNames[IDX(TERRAIN_COUNT)] = {
    localization::Tr("table.gTerrainNames.0"),
    localization::Tr("table.gTerrainNames.1"),
    localization::Tr("table.gTerrainNames.2"),
    localization::Tr("table.gTerrainNames.3"),
    localization::Tr("table.gTerrainNames.4"),
    localization::Tr("table.gTerrainNames.5"),
    localization::Tr("table.gTerrainNames.6"),
    localization::Tr("table.gTerrainNames.7"),
    localization::Tr("table.gTerrainNames.8")
};
DATA(0x00481364) H2_CONST char* gResourceNames[IDX(RES_COUNT)] = {
    localization::Tr("table.gResourceNames.0"),
    localization::Tr("table.gResourceNames.1"),
    localization::Tr("table.gResourceNames.2"),
    localization::Tr("table.gResourceNames.3"),
    localization::Tr("table.gResourceNames.4"),
    localization::Tr("table.gResourceNames.5"),
    localization::Tr("table.gResourceNames.6")
};
// The localised build names the mine, not the resource it yields, in the
// adventure-map quick info; the English 2.1 tree has no such table and reads
// gResourceNames there. See docs/versions/gold-2.1-buka.md.
DATA(0x00481380) H2_CONST char* gMineNames[IDX(RES_COUNT)] = {
    localization::Tr("table.gMineNames.0"),
    localization::Tr("table.gMineNames.1"),
    localization::Tr("table.gMineNames.2"),
    localization::Tr("table.gMineNames.3"),
    localization::Tr("table.gMineNames.4"),
    localization::Tr("table.gMineNames.5"),
    localization::Tr("table.gMineNames.6")
};
DATA(0x0048139c) H2_CONST char* gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT] = {
    "" /* "" */,
    localization::Tr("table.gQuickViewText.1"),
    localization::Tr("table.gQuickViewText.2"),
    localization::Tr("table.gQuickViewText.3"),
    localization::Tr("table.gQuickViewText.4"),
    localization::Tr("table.gQuickViewText.5"),
    localization::Tr("table.gQuickViewText.6"),
    localization::Tr("table.gQuickViewText.7"),
    localization::Tr("table.gQuickViewText.8"),
    localization::Tr("table.gQuickViewText.9"),
    localization::Tr("table.gQuickViewText.10"),
    localization::Tr("table.gQuickViewText.11"),
    localization::Tr("table.gQuickViewText.12"),
    localization::Tr("table.gQuickViewText.13"),
    localization::Tr("table.gQuickViewText.14"),
    localization::Tr("table.gQuickViewText.15"),
    localization::Tr("table.gQuickViewText.16"),
    localization::Tr("table.gQuickViewText.17"),
    localization::Tr("table.gQuickViewText.18"),
    localization::Tr("table.gQuickViewText.19"),
    localization::Tr("table.gQuickViewText.20"),
    localization::Tr("table.gQuickViewText.21"),
    localization::Tr("table.gQuickViewText.22"),
    localization::Tr("table.gQuickViewText.23"),
    localization::Tr("table.gQuickViewText.24"),
    localization::Tr("table.gQuickViewText.25"),
    localization::Tr("table.gQuickViewText.26"),
    localization::Tr("table.gQuickViewText.27"),
    "" /* "" */,
    localization::Tr("table.gQuickViewText.29"),
    localization::Tr("table.gQuickViewText.30"),
    localization::Tr("table.gQuickViewText.31"),
    localization::Tr("table.gQuickViewText.32"),
    localization::Tr("table.gQuickViewText.33"),
    localization::Tr("table.gQuickViewText.34"),
    localization::Tr("table.gQuickViewText.35"),
    localization::Tr("table.gQuickViewText.36"),
    localization::Tr("table.gQuickViewText.37"),
    localization::Tr("table.gQuickViewText.38"),
    localization::Tr("table.gQuickViewText.39"),
    localization::Tr("table.gQuickViewText.40"),
    localization::Tr("table.gQuickViewText.41"),
    localization::Tr("table.gQuickViewText.42"),
    localization::Tr("table.gQuickViewText.43"),
    localization::Tr("artifact.ultimate_generic"),
    localization::Tr("table.gQuickViewText.45"),
    localization::Tr("table.gQuickViewText.46"),
    localization::Tr("table.gQuickViewText.47"),
    localization::Tr("table.gQuickViewText.48"),
    localization::Tr("table.gQuickViewText.49"),
    "" /* "" */,
    localization::Tr("table.gQuickViewText.51"),
    localization::Tr("table.gQuickViewText.52"),
    localization::Tr("table.gQuickViewText.53"),
    localization::Tr("table.gQuickViewText.54"),
    localization::Tr("table.gQuickViewText.55"),
    localization::Tr("table.gQuickViewText.56"),
    "" /* "" */,
    localization::Tr("table.gQuickViewText.58"),
    localization::Tr("table.gQuickViewText.59"),
    localization::Tr("table.gQuickViewText.60"),
    localization::Tr("table.gQuickViewText.61"),
    localization::Tr("table.gQuickViewText.62"),
    localization::Tr("table.gQuickViewText.63"),
    localization::Tr("table.gQuickViewText.64"),
    localization::Tr("table.gQuickViewText.65"),
    localization::Tr("table.gQuickViewText.66"),
    localization::Tr("table.gQuickViewText.67"),
    localization::Tr("table.gQuickViewText.68"),
    localization::Tr("table.gQuickViewText.69"),
    localization::Tr("table.gQuickViewText.70"),
    localization::Tr("table.gQuickViewText.71"),
    localization::Tr("table.gQuickViewText.72"),
    localization::Tr("table.gQuickViewText.73"),
    localization::Tr("table.gQuickViewText.74"),
    localization::Tr("table.gQuickViewText.75"),
    localization::Tr("table.gQuickViewText.76"),
    localization::Tr("table.gQuickViewText.77"),
    localization::Tr("table.gQuickViewText.78"),
    localization::Tr("table.gQuickViewText.79"),
    localization::Tr("table.gQuickViewText.80"),
    localization::Tr("table.gQuickViewText.81"),
    localization::Tr("table.gQuickViewText.82"),
    localization::Tr("table.gQuickViewText.83"),
    localization::Tr("table.gQuickViewText.84"),
    localization::Tr("table.gQuickViewText.85"),
    localization::Tr("table.gQuickViewText.86"),
    localization::Tr("table.gQuickViewText.87"),
    localization::Tr("table.gQuickViewText.88"),
    localization::Tr("table.gQuickViewText.89"),
    localization::Tr("table.gQuickViewText.90"),
    localization::Tr("table.gQuickViewText.91"),
    localization::Tr("table.gQuickViewText.92"),
    localization::Tr("table.gQuickViewText.93"),
    localization::Tr("table.gQuickViewText.94"),
    localization::Tr("table.gQuickViewText.95"),
    localization::Tr("table.gQuickViewText.96"),
    localization::Tr("table.gQuickViewText.97"),
    localization::Tr("table.gQuickViewText.98"),
    localization::Tr("table.gQuickViewText.99"),
    localization::Tr("table.gQuickViewText.100"),
    localization::Tr("table.gQuickViewText.101"),
    localization::Tr("table.gQuickViewText.102"),
    localization::Tr("table.gQuickViewText.103"),
    localization::Tr("table.gQuickViewText.104"),
    localization::Tr("table.gQuickViewText.105"),
    localization::Tr("table.gQuickViewText.106"),
    localization::Tr("table.gQuickViewText.107"),
    localization::Tr("table.gQuickViewText.108"),
    localization::Tr("table.gQuickViewText.109"),
    localization::Tr("table.gQuickViewText.110"),
    localization::Tr("table.gQuickViewText.111"),
    localization::Tr("table.gQuickViewText.112"),
    localization::Tr("table.gQuickViewText.113"),
    localization::Tr("table.gQuickViewText.114"),
    localization::Tr("table.gQuickViewText.115"),
    localization::Tr("table.gQuickViewText.116"),
    localization::Tr("table.gQuickViewText.117"),
    localization::Tr("table.gQuickViewText.118"),
    localization::Tr("table.gQuickViewText.119"),
    localization::Tr("table.gQuickViewText.120"),
    "%s" /* "%s" */,
    "%s" /* "%s" */,
    localization::Tr("table.gQuickViewText.123")
};
DATA(0x0048158c) H2_CONST char* gEventText[KB_EVENT_TEXT_TABLE_COUNT] = {
    // Алхимик\n\nВы стали хозяином лаборатории местного алхимика. Она будет приносить вам по одной
    // единице ртути в день.
    localization::Tr("table.gEventText.0"),
    // Указатель\n\nНа указателе написано:\n\n%s находится неподалеку отсюда.
    localization::Tr("table.gEventText.1"),
    // Буй\n\nВаши спутники замечают морской буй. Он указывает верный курс.
    localization::Tr("table.gEventText.2"),
    // Буй\n\nВаши спутники замечают морской буй. Он указывает верный курс, и это повышает их боевой
    // дух.
    localization::Tr("table.gEventText.3"),
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    // Кольцо фейри\n\nВаше войско вступает внутрь кольца фейри, но ничего не происходит.
    localization::Tr("table.gEventText.12"),
    // Кольцо фейри\n\nВаше войско вступает внутрь кольца фейри, чары которого принесут вам удачу в
    // грядущем сражении.
    localization::Tr("table.gEventText.13"),
    // Костер\n\nОбыскав вражеский лагерь, вы находите спрятанный клад.
    localization::Tr("table.gEventText.14"),
    // Фонтан\n\nВы припадаете к струям волшебного фонтана, но ничего не происходит.
    localization::Tr("table.gEventText.15"),
    // Фонтан\n\nБлагоуханная влага волшебного фонтана принесет вам удачу в грядущем сражении.
    localization::Tr("table.gEventText.16"),
    // Беседка\n\nНа ступенях беседки появляется старый рыцарь. \"Мне жаль, храбрый воин, но я уже
    // научил тебя всему, что знаю сам.\"
    localization::Tr("table.gEventText.17"),
    // Беседка\n\nНа ступенях беседки появляется старый рыцарь. \"О храбрый воин, я научу тебя всему,
    // что знаю сам; пусть мой опыт поможет тебе в твоих странствиях.\"
    localization::Tr("table.gEventText.18"),
    // Лампа джинна\n\nВы находите засыпанную землей помятую и закопченную лампа. Хотите ее потереть?
    localization::Tr("table.gEventText.19"),
    // Кладбище\n\nВы осторожно приближаетесь к захоронению древних воинов. Хотите вскрыть их могилы?
    localization::Tr("table.gEventText.20"),
    // Одержав победу над зомби, вы несколько часов подряд обыскиваете могилы, но ничего не находите.
    // Ваш недостойный поступок отрицательно влияет на боевой дух войска.
    localization::Tr("table.gEventText.21"),
    // Одержав победу над зомби, вы обыскиваете могилы и удаляетесь с находкой!
    localization::Tr("table.gEventText.22"),
    // {Дом стрелков}\n\nГруппа стрелков в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.23"),
    // В вашем войске нет места для новых рекрутов.
    localization::Tr("table.gEventText.24"),
    // {Дом стрелков}\n\nПриблизившись к жилищу, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.25"),
    // Хибара гоблинов\n\nГруппа гоблинов в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.26"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.27"),
    // Хибара гоблинов\n\nПриблизившись к жилищу гоблинов, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.28"),
    // Хижина крестьян\n\nГруппа крестьян в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.29"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.30"),
    // Хижина крестьян\n\nПриблизившись к жилищу крестьян, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.31"),
    // Избушка гномов\n\nГруппа стрелков в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.32"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.33"),
    // Избушка гномов\n\nПриблизившись к жилищу стрелков, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.34"),
    // {Мазанка}\n\nГруппа крестьян в поисках славы желает примкнуть к вашему войску. Согласны ли вы
    // принять их?
    localization::Tr("table.gEventText.35"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.36"),
    // {Мазанка}\n\nПриблизившись к жилищу Крестьян, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.37"),
    // {Древо-дом}\n\nГруппа фей в поисках славы желает примкнуть к вашему войску. Согласны ли вы
    // принять их?
    localization::Tr("table.gEventText.38"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.39"),
    // {Древо-дом}\n\nПриблизившись к древесному дому Фей, вы обнаруживаете, что он пустует.
    localization::Tr("table.gEventText.40"),
    // {Нора полуросликов}\n\nГруппа полуросликов в поисках славы желает примкнуть к вашему войску.
    // Согласны ли вы принять их?
    localization::Tr("table.gEventText.41"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.42"),
    // {Нора полуросликов}\n\nПриблизившись к норе полуросликов, вы обнаруживаете, что она пустует.
    localization::Tr("table.gEventText.43"),
    // {Сторожевая вышка}\n\nГруппа орков в поисках славы желает примкнуть к вашему войску. Согласны ли
    // вы принять их?
    localization::Tr("table.gEventText.44"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.45"),
    // {Сторожевая вышка}\n\nПриблизившись к сторожевой вышке орков, вы обнаруживаете, что она пустует.
    localization::Tr("table.gEventText.46"),
    // {Снежная пещера}\n\nГруппа кентавров в поисках славы желает примкнуть к вашему войску. Согласны
    // ли вы принять их?
    localization::Tr("table.gEventText.47"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.48"),
    // {Пещера}\n\nПриблизившись к пещере кентавров, вы обнаруживаете, что она пустует.
    localization::Tr("table.gEventText.49"),
    // {Раскопки}\n\nГруппа скелетов в поисках славы желает примкнуть к вашему войску. Согласны ли вы
    // принять их?
    localization::Tr("table.gEventText.50"),
    // Вы не можете принять новых рекрутов в свое войско, его ряды полны.
    localization::Tr("table.gEventText.51"),
    // {Раскопки}\n\nПриблизившись к захоронению скелетов, вы обнаруживаете, что оно пустует.
    localization::Tr("table.gEventText.52"),
    "",
    "",
    "",
    "",
    "",
    // Маяк\n\nТеперь маяк ваш, и все ваши корабли будут преодолевать большее расстояние за один ход.
    localization::Tr("table.gEventText.58"),
    // Водяная мельница\n\nМельник обращается к вам со словами: \"Сожалею, господин, но сегодня золота
    // у меня нет. Приходите на следующей неделе.\"
    localization::Tr("table.gEventText.59"),
    // Водяная мельница\n\nМельник обращается к вам со словами: \"Господин, я трудился в поте лица и
    // прошу вас принять мою скромную лепту. Приходите на следующей неделе, и вы получите еще столько
    // же.\"
    localization::Tr("table.gEventText.60"),
    // Рудная шахта\n\nВы стали хозяином рудной шахты. Она будет приносить вам по две меры руды в день.
    localization::Tr("table.gEventText.61"),
    // Серная шахта\n\nВы стали хозяином серной шахты. Она будут приносить вам по 1 единице серы в
    // день.
    localization::Tr("table.gEventText.62"),
    // Кристальная шахта\n\nВы стали хозяином кристальной шахты. Она будет приносить вам по одной мере
    // кристаллов в день.
    localization::Tr("table.gEventText.63"),
    // Самоцветная шахта\n\nВы стали хозяином самоцветной шахты. Она будет приносить вам по 1 единице
    // самоцветов в день.
    localization::Tr("table.gEventText.64"),
    // Золотая шахта\n\nВы стали хозяином золотой шахты. Она будет приносить вам по 1000 золотых в
    // день.
    localization::Tr("table.gEventText.65"),
    // Последователи\n\nГруппа %s в поисках славы желает примкнуть к вашему войску. Вы согласны принять
    // их?
    localization::Tr("table.gEventText.66"),
    // Оскорбленные отказом быть принятыми в ваши ряды, они нападают на вас!
    localization::Tr("table.gEventText.67"),
    // Обелиск\n\nПеред вами обелиск, высеченный из невиданного камня. Вы вглядываетесь в его гладкую
    // поверхность и вдруг замечаете, что на ней начинают проступать таинственные знаки. Знаки
    // складываются во фрагмент древней карты. Вы торопливо срисовываете его, и знаки исчезают так же
    // внезапно, как и появились.
    localization::Tr("table.gEventText.68"),
    // Обелиск\n\nВы уже посещали этот обелиск.
    localization::Tr("table.gEventText.69"),
    "",
    "",
    // Вы нашли ресурс (%s).
    localization::Tr("table.gEventText.72"),
    // Лесопилка\n\nВы стали хозяином лесопилки. Она будет приносить вам по 2 единицы древесины в день.
    localization::Tr("table.gEventText.73"),
    // {Оракул}\n\nНа поляне в окружении деревьев восседает слепой оракул. Вы рассказываете ему о целях
    // вашего похода, и он показывает вам сильные и слабые стороны ваших противников в магическом
    // хрустальном шаре.
    localization::Tr("table.gEventText.74"),
    "",
    "",
    "",
    "",
    "",
    "",
    // {Шатер}\n\nВаше внимание привлекает шатер, пологи которых трепещут на жарком ветру пустыни. В
    // нем никого нет. Пройдет время, и, быть может, сюда придет новый отряд кочевников.
    localization::Tr("table.gEventText.81"),
    // {Шатер}\n\nВаше внимание привлекают шатер, пологи которого трепещут на жарком ветру пустыни. Вы
    // хотите принять в ваше войско отряд кочевников?
    localization::Tr("table.gEventText.82"),
    // {Повозка}\n\nЦветастая повозка разбойников пуста. Пройдет время, и, быть может, здесь обоснуется
    // новая шайка.
    localization::Tr("table.gEventText.83"),
    // {Повозка}\n\nВдалеке слышится музыка и смех. Вы идете на звуки и видите цветастую повозку, в
    // которой живут разбойники. Вы хотите принять в ваше войско шайку разбойников?
    localization::Tr("table.gEventText.84"),
    // {Водоворот}\n\nВаш корабль попадает в водоворот. Часть вашего войска исчезает в пучине.
    localization::Tr("table.gEventText.85"),
    // {Ветряная мельница}\n\nМельник обращается к вам со словами: \"Сожалею, господин, но сегодня у
    // меня ничего нет. Приходите на следующей неделе.\"
    localization::Tr("table.gEventText.86"),
    // {Ветряная мельница}\n\nМельник обращается к вам со словами: \"Господин, я работал не покладая
    // рук, и прошу вас принять мой скромный дар. Приходите на следующей неделе, у меня опять найдется,
    // чем вас порадовать.\"
    localization::Tr("table.gEventText.87"),
    "",
    "",
    "",
    "",
    "",
    // {Скелет}\n\nВы находите останки незадачливого искателя приключений. Пошарив в груде лохмотьев,
    // вы ничего не находите.
    localization::Tr("table.gEventText.93"),
    // {Скелет}\n\nВы находите останки незадачливого искателя приключений. Пошарив в груде лохмотьев,
    // вы находите.
    localization::Tr("table.gEventText.94")
};
DATA(0x00481708) H2_CONST char* gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT] = {
    // Начать одиночную или сетевую игру.
    localization::Tr("table.gCPanelHelp.0"),
    // Загрузить сохраненную игру.
    localization::Tr("table.gCPanelHelp.1"),
    // Сохранить игру.
    localization::Tr("table.gCPanelHelp.2"),
    // Выйти из Героев Меча и Магии II.
    localization::Tr("table.gCPanelHelp.3"),
    // Закрыть меню, ничего не делая.
    localization::Tr("table.gCPanelHelp.4")
};
DATA(0x0048171c) H2_CONST char* gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT] = {
    // {ОК}\n\nЗакрыть это меню.
    localization::Tr("table.gCSPanelHelp.0"),
    // {Скорость}\n\nУстановить скорость действий и анимации воинов в бою.
    localization::Tr("table.gCSPanelHelp.1"),
    // {Информация о воине}\n\nВключить или выключить отображение окна с информацией о выбранном и
    // атакуемом воине.
    localization::Tr("table.gCSPanelHelp.2"),
    // {Магия в автобое}\n\nЕсли эта опция включена, ваш герой будет использовать заклинания во время
    // автобоя. (Примечание: Эта опция не влияет на использование заклинаний компьютерными игроками, и
    // на быстрый бой.)
    localization::Tr("table.gCSPanelHelp.3"),
    // {Сетка}\n\nВключает или выключает отображение сетки. Все перемещения на поле боя происходят по
    // гексагональной сетке, даже если ее отображение отключено.
    localization::Tr("table.gCSPanelHelp.4"),
    // {Затенение сетки}\n\nВключает или выключает режим обозначения возможной дальности передвижения
    // выбранного отряда воинов.
    localization::Tr("table.gCSPanelHelp.5"),
    // {Курсор с тенью}\n\nВключает или выключает отрисовку тени от курсора на сетке координат.
    localization::Tr("table.gCSPanelHelp.6")
};
DATA(0x00481738) H2_CONST char* gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT] = {
    // Осмотреть весь мир.
    localization::Tr("table.gAPanelHelp.0"),
    // Посмотреть головоломку.
    localization::Tr("table.gAPanelHelp.1"),
    // Показать информацию о сценарии, на котором идет игра.
    localization::Tr("table.gAPanelHelp.2"),
    // Копать в поисках Великого артефакта.
    (localization::Tr("table.gAPanelHelp.3")),
    // Закрыть это меню.
    localization::Tr("table.gAPanelHelp.4")
};
DATA(0x0048174c) H2_CONST char* gInitMenuHelp[KB_INIT_MENU_HELP_COUNT] = {
    // {Новая игра}\n\nНачать отдельный сценарий или сетевую игру.
    localization::Tr("table.gInitMenuHelp.0"),
    // {Игры}\n\nЗагрузить ранее сохраненную игру.
    localization::Tr("table.gInitMenuHelp.1"),
    // {Рекорды}\n\nПоказать таблицу рекордов.
    localization::Tr("table.gInitMenuHelp.2"),
    // {Авторы}\n\nПоказать перечень авторов игры.
    localization::Tr("table.gInitMenuHelp.3"),
    // {Выйти}\n\nВыйти из героев Меча и Магии II и вернуться в операционную систему.
    localization::Tr("table.gInitMenuHelp.4")
};
DATA(0x00481760) H2_CONST char* gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT] = {
    // {Следующий герой}\n\nВыбрать следующего героя.
    localization::Tr("table.gAdvMenuHelp.0"),
    // {Продолжить движение}\n\nПродолжить движение героя по намеченному пути.
    localization::Tr("table.gAdvMenuHelp.1"),
    // {Обзор королевства}\n\nОсмотреть ваши владения.
    localization::Tr("table.gAdvMenuHelp.2"),
    // {Окончить ход}\n\nОкончить ход и передать управление компьютеру.
    localization::Tr("table.gAdvMenuHelp.3"),
    // {Игровые действия}\n\nОткрыть окно доступных игровых действий.
    localization::Tr("table.gAdvMenuHelp.4"),
    // {Окно файлов}\n\nОткрывает меню, где вы можете загружать или сохранять игры.
    localization::Tr("table.gAdvMenuHelp.5"),
    // {Системные настройки}\n\nОткрывает окно системных настроек, позволяющих настроить игру.
    localization::Tr("table.gAdvMenuHelp.6"),
    // {Направить заклинание}\n\nНаправить заклинание на стратегической карте.
    localization::Tr("table.gAdvMenuHelp.7")
};
DATA(0x00481780) H2_CONST char* gLuckText[KB_LUCK_TEXT_COUNT] = {
    // Проклятая
    localization::Tr("table.gLuckText.0"),
    // Ужасная
    localization::Tr("table.gLuckText.1"),
    // Плохая
    localization::Tr("table.gLuckText.2"),
    // Обычная
    localization::Tr("table.gLuckText.3"),
    // Хорошая
    localization::Tr("table.gLuckText.4"),
    // Отличная
    localization::Tr("table.gLuckText.5"),
    // Божественная
    localization::Tr("table.gLuckText.6")
};
DATA(0x0048179c) H2_CONST char* gMoraleText[KB_MORALE_TEXT_COUNT] = {
    // Предательская
    localization::Tr("table.gMoraleText.0"),
    // Ужасная
    localization::Tr("table.gMoraleText.1"),
    // Плохая
    localization::Tr("table.gMoraleText.2"),
    // Обычная
    localization::Tr("table.gMoraleText.3"),
    // Хорошая
    localization::Tr("table.gMoraleText.4"),
    // Отличная
    localization::Tr("table.gMoraleText.5"),
    // Кровавая!
    localization::Tr("table.gMoraleText.6")
};
DATA(0x004817b8) H2_CONST char* onOffText[KB_ON_OFF_TEXT_COUNT] = {
    // Выкл.
    localization::Tr("table.onOffText.0"),
    // Вкл.
    localization::Tr("table.onOffText.1"),
    // Вкл.\nГромкость 9
    localization::Tr("table.onOffText.2"),
    // Вкл.\nГромкость 8
    localization::Tr("table.onOffText.3"),
    // Вкл.\nГромкость 7
    localization::Tr("table.onOffText.4"),
    // Вкл.\nГромкость 6
    localization::Tr("table.onOffText.5"),
    // Вкл.\nГромкость 5
    localization::Tr("table.onOffText.6"),
    // Вкл.\nГромкость 4
    localization::Tr("table.onOffText.7"),
    // Вкл.\nГромкость 3
    localization::Tr("table.onOffText.8"),
    // Вкл.\nГромкость 2
    localization::Tr("table.onOffText.9"),
    // Вкл.\nГромкость 1
    localization::Tr("table.onOffText.10")
};
DATA(0x004817e4) H2_CONST char* walkSpeedText[KB_WALK_SPEED_TEXT_COUNT] = {
    // Шагом
    localization::Tr("table.walkSpeedText.0"),
    // Рысью
    localization::Tr("table.walkSpeedText.1"),
    // Аллюром
    localization::Tr("table.walkSpeedText.2"),
    // Галопом
    localization::Tr("table.walkSpeedText.3"),
    // Прыжками
    localization::Tr("table.walkSpeedText.4")
};
DATA(0x004817f8) H2_CONST char* gColors[IDX(FACTION_COUNT)] = {
    localization::Tr("table.gColors.0"),
    localization::Tr("table.gColors.1"),
    localization::Tr("table.gColors.2"),
    localization::Tr("table.gColors.3"),
    localization::Tr("table.gColors.4"),
    localization::Tr("table.gColors.5")
};
DATA(0x00481810) H2_CONST char* gColorAbbreviations[PLAYER_COLOR_COUNT] = {
    localization::Tr("color.abbreviated.blue"),
    localization::Tr("color.abbreviated.green"),
    localization::Tr("color.abbreviated.red"),
    localization::Tr("color.abbreviated.yellow"),
    localization::Tr("color.abbreviated.orange"),
    localization::Tr("color.abbreviated.purple")
};
DATA(0x00481828) H2_CONST char* gMonthNames[KB_MONTH_NAME_COUNT] = {
    // Кузнечика
    localization::Tr("table.gMonthNames.0"),
    // Муравья
    localization::Tr("table.gMonthNames.1"),
    // Стрекозы
    localization::Tr("table.gMonthNames.2"),
    // Паука
    localization::Tr("table.gMonthNames.3"),
    // Бабочки
    localization::Tr("table.gMonthNames.4"),
    // Шмеля
    localization::Tr("table.gMonthNames.5"),
    // Цикады
    localization::Tr("table.gMonthNames.6"),
    // Земляного червя
    localization::Tr("table.gMonthNames.7"),
    // Шершня
    localization::Tr("table.gMonthNames.8"),
    // Жука
    localization::Tr("table.gMonthNames.9")
};
DATA(0x00481850) H2_CONST char* gWeekNames[KB_WEEK_NAME_COUNT] = {
    // Белки
    localization::Tr("table.gWeekNames.0"),
    // Кролика
    localization::Tr("table.gWeekNames.1"),
    // Суслика
    localization::Tr("table.gWeekNames.2"),
    // Барсука
    localization::Tr("table.gWeekNames.3"),
    // Крысы
    localization::Tr("table.gWeekNames.4"),
    // Орла
    localization::Tr("table.gWeekNames.5"),
    // Горностая
    localization::Tr("table.gWeekNames.6"),
    // Ворона
    localization::Tr("table.gWeekNames.7"),
    // Мангуста
    localization::Tr("table.gWeekNames.8"),
    // Собаки
    localization::Tr("table.gWeekNames.9"),
    // Муравьеда
    localization::Tr("table.gWeekNames.10"),
    // Ящерицы
    localization::Tr("table.gWeekNames.11"),
    // Черепахи
    localization::Tr("table.gWeekNames.12"),
    // Дикобраза
    localization::Tr("table.gWeekNames.13"),
    // Кондора
    localization::Tr("table.gWeekNames.14")
};
DATA(0x0048188c) H2_CONST char* cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT] = {
    // Обзор королевства
    localization::Tr("table.cHeroScreen.0"),
    // %s - информация
    localization::Tr("table.cHeroScreen.1"),
    // Дополнительная статистика героя
    localization::Tr("table.cHeroScreen.2"),
    // Информация о высокой морали
    localization::Tr("table.cHeroScreen.3"),
    // Информация об обычной морали
    localization::Tr("table.cHeroScreen.4"),
    // Информация о плохой морали
    localization::Tr("table.cHeroScreen.5"),
    // Информация о хорошей удаче
    localization::Tr("table.cHeroScreen.6"),
    // Информация об обычной удаче
    localization::Tr("table.cHeroScreen.7"),
    // Информация о плохой удаче
    localization::Tr("table.cHeroScreen.8"),
    // Показать опыт
    localization::Tr("table.cHeroScreen.9"),
    // Выбрать %s
    localization::Tr("table.cHeroScreen.10"),
    // Пусто
    localization::Tr("table.cHeroScreen.11"),
    // Перенести сюда отряд %s
    localization::Tr("table.cHeroScreen.12"),
    // Отряды %s и %s меняются местами
    localization::Tr("table.cHeroScreen.13"),
    // Показать заклинания
    localization::Tr("table.cHeroScreen.14"),
    // Посмотреть информацию об: %s
    localization::Tr("table.cHeroScreen.15"),
    // %s %s - уволить
    localization::Tr("table.cHeroScreen.16"),
    // Закрыть экран героя
    localization::Tr("table.cHeroScreen.17"),
    // Экран героя
    localization::Tr("table.cHeroScreen.18"),
    // %s в один отряд
    localization::Tr("table.cHeroScreen.19"),
    // Разделить отряд %s
    localization::Tr("table.cHeroScreen.20"),
    // %s %s - информация
    localization::Tr("table.cHeroScreen.21"),
    // Информация об очках магии
    localization::Tr("table.cHeroScreen.22"),
    // Выбрать широкие ряды в бою
    localization::Tr("table.cHeroScreen.23"),
    // Сгруппировать воинов
    localization::Tr("table.cHeroScreen.24")
};
DATA(0x004818f0) H2_CONST char* cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT] = {
    // Построить Гильдию магов
    localization::Tr("table.cCastleInfo.0"),
    // Построены все этажи Гильдии магов.
    localization::Tr("table.cCastleInfo.1"),
    // Нельзя построить следующий этаж.
    localization::Tr("table.cCastleInfo.2"),
    // Построить следующий этаж Гильдии магов
    localization::Tr("table.cCastleInfo.3"),
    // Постройка '%s' уже возведена
    localization::Tr("table.cCastleInfo.4"),
    // Нельзя возвести постройку '%s'
    localization::Tr("table.cCastleInfo.5"),
    // Нельзя возвести постройку '%s'
    localization::Tr("table.cCastleInfo.6"),
    // Возвести постройку '%s'
    localization::Tr("table.cCastleInfo.7"),
    // Герой вам не по карману.
    localization::Tr("table.cCastleInfo.8"),
    // Нельзя нанять - у вас уже %d героев.
    localization::Tr("table.cCastleInfo.9"),
    // Нельзя нанять - в этом городе у вас уже есть герой.
    localization::Tr("table.cCastleInfo.10"),
    // Нанять нового героя
    localization::Tr("town.recruit.new_hero"),
    // Выйти из замка
    localization::Tr("table.cCastleInfo.12"),
    // Возможности замка
    localization::Tr("table.cCastleInfo.13"),
    // Сгруппировать гарнизон
    localization::Tr("table.cCastleInfo.14"),
    // Выбрать широкие ряды для гарнизона
    localization::Tr("table.cCastleInfo.15")
};
DATA(0x00481930) H2_CONST char* cLuckInfo[KB_LUCK_INFO_TEXT_COUNT] = {
    // {Хорошая удача}\n\nЕсли удача вашего войска выше обычной, атаки отдельных отрядов на поле боя
    // иногда оказываются более результативными (их сила удваивается).
    localization::Tr("table.cLuckInfo.0"),
    // {Обычная удача}\n\nС обычной удачей ваше войско не имеет ни преимуществ, ни недостатков на поле
    // боя.
    localization::Tr("table.cLuckInfo.1"),
    // {Плохая удача}\n\nЕсли вашему войску не везет, урон, наносимый  отдельными отрядами на поле боя,
    // может оказаться вдвое меньше обычного.
    localization::Tr("table.cLuckInfo.2"),
    // %s\n\n\nМодификаторы удачи:
    localization::Tr("table.cLuckInfo.3"),
    // \nЛапка кролика +1
    localization::Tr("table.cLuckInfo.4"),
    // \nЗолотая подкова +1
    localization::Tr("table.cLuckInfo.5"),
    // \nМонета +1
    localization::Tr("table.cLuckInfo.6"),
    // \nКлевер +1
    localization::Tr("table.cLuckInfo.7"),
    // \nПосещен Круг фейри +1
    localization::Tr("table.cLuckInfo.8"),
    // \nПосещен фонтан +1
    localization::Tr("table.cLuckInfo.9"),
    // \nНет
    localization::Tr("table.cLuckInfo.10"),
    // \nГрабитель могил -1
    localization::Tr("table.cLuckInfo.11"),
    // \nРадуга магов +2
    localization::Tr("table.cLuckInfo.12"),
    // \nПосещен идол +1
    localization::Tr("table.cLuckInfo.13"),
    // \nОграблена пирамида -2
    localization::Tr("table.cLuckInfo.14"),
    // \nБазовая удача +1
    localization::Tr("table.cLuckInfo.15"),
    // \nВысокая удача +2
    localization::Tr("table.cLuckInfo.16"),
    // \nЭксперт удачи +3
    localization::Tr("table.cLuckInfo.17"),
    // \nБонус мачты на море +1
    localization::Tr("table.cLuckInfo.18"),
    // \nПосещена русалка +1
    localization::Tr("table.cLuckInfo.19"),
    // \nБоевое одеяние Андурана дает максимальную удачу.
    localization::Tr("table.cLuckInfo.20")
};
DATA(0x00481984) H2_CONST char* IQnames[KB_IQ_NAME_COUNT] = {
    // Нет
    localization::Tr("table.IQnames.0"),
    // Глупый
    localization::Tr("table.IQnames.1"),
    // Средний
    localization::Tr("table.IQnames.2"),
    // Умный
    localization::Tr("table.IQnames.3"),
    // Гений
    localization::Tr("table.IQnames.4")
};
DATA(0x00481998) H2_CONST char* cSpellHelp[KB_SPELL_HELP_TEXT_COUNT] = {
    // Предыдущая страница
    localization::Tr("table.cSpellHelp.0"),
    // Следующая страница
    localization::Tr("table.cSpellHelp.1"),
    // Небоевые заклинания
    localization::Tr("table.cSpellHelp.2"),
    // Боевые заклинания
    localization::Tr("table.cSpellHelp.3"),
    // Закрыть волшебную книгу
    localization::Tr("table.cSpellHelp.4"),
    // Заклинания
    localization::Tr("table.cSpellHelp.5"),
    // Выбрать заклинание
    localization::Tr("table.cSpellHelp.6"),
    // Боевые заклинания
    localization::Tr("table.cSpellHelp.7"),
    // У вашего героя осталось %d оч. магии
    (localization::Tr("table.cSpellHelp.8"))
};
DATA(0x004819bc) H2_CONST char* speedText[KB_SPEED_TEXT_COUNT] = {
    /*  */ "",
     localization::Tr("table.speedText.1"),
     localization::Tr("table.speedText.2"),
     localization::Tr("table.speedText.3"),
     localization::Tr("table.speedText.4"),
     localization::Tr("table.speedText.5"),
     localization::Tr("table.speedText.6"),
     localization::Tr("table.speedText.7"),
     localization::Tr("table.speedText.8"),
     localization::Tr("table.speedText.9")
};
DATA(0x004819e4) H2_CONST char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
     localization::Tr("table.cArmyDetail.0"),
     localization::Tr("table.cArmyDetail.1"),
    /* Выстрелов:  */ localization::Tr("table.cArmyDetail.2"),
    /* Урон:  */ localization::Tr("table.cArmyDetail.3"),
    /* Здоровье:  */ localization::Tr("table.cArmyDetail.4"),
    /* Скорость:  */ localization::Tr("table.cArmyDetail.5"),
    /* Мораль:  */ localization::Tr("table.cArmyDetail.6"),
    /* Удача:  */ localization::Tr("table.cArmyDetail.7"),
    /* Выстрелов:  */ localization::Tr("table.cArmyDetail.8")
};
DATA(0x00481a08) H2_CONST char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
     localization::Tr("table.cWellDetail.0"),
     localization::Tr("table.cWellDetail.1"),
    /* Выстр.:  */ localization::Tr("table.cWellDetail.2"),
    /* Урон:  */ localization::Tr("table.cWellDetail.3"),
    /* ЗД:  */ localization::Tr("table.cWellDetail.4"),
    /* Скор.:  */ localization::Tr("table.cWellDetail.5"),
    /* Всего:  */ localization::Tr("table.cWellDetail.6"),
     localization::Tr("table.cWellDetail.7"),
     localization::Tr("table.cWellDetail.8")
};
DATA(0x00481a2c) H2_CONST char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {

    (localization::Tr("table.cKingdomOverview.0")),
     localization::Tr("table.cKingdomOverview.1"),
     localization::Tr("table.cKingdomOverview.2")
};
DATA(0x00481a38) H2_CONST char* cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
     localization::Tr("table.cNewTurn.0"),
     localization::Tr("table.cNewTurn.1"),
     localization::Tr("table.cNewTurn.2"),
     localization::Tr("table.cNewTurn.3"),
     localization::Tr("table.cNewTurn.4"),
     localization::Tr("table.cNewTurn.5"),
     localization::Tr("table.cNewTurn.6")
};
DATA(0x00481a54) H2_CONST char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
     localization::Tr("table.cViewGeneralLabels.0"),
     localization::Tr("table.cViewGeneralLabels.1"),
     localization::Tr("table.cViewGeneralLabels.2"),
     localization::Tr("table.cViewGeneralLabels.3"),
    /* Мораль:  */ localization::Tr("table.cViewGeneralLabels.4"),
    /* Удача:  */ localization::Tr("table.cViewGeneralLabels.5"),
     localization::Tr("table.cViewGeneralLabels.6")
};
DATA(0x00481a70) H2_CONST char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
     localization::Tr("table.cViewGeneralHelp.0"),
     localization::Tr("table.cViewGeneralHelp.1"),
     localization::Tr("table.cViewGeneralHelp.2"),
     localization::Tr("table.cViewGeneralHelp.3"),
     localization::Tr("table.cViewGeneralHelp.4"),
     localization::Tr("table.cViewGeneralHelp.5"),
     localization::Tr("table.cViewGeneralHelp.6")
};
DATA(0x00481a8c) H2_CONST char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
     localization::Tr("table.cViewGeneralLongHelp.0"),
     localization::Tr("table.cViewGeneralLongHelp.1"),
     localization::Tr("table.cViewGeneralLongHelp.2"),
     localization::Tr("table.cViewGeneralLongHelp.3")
};
DATA(0x00481a9c) H2_CONST char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
    /*  */ "",
     localization::Tr("table.cCombatMessage.1"),
     localization::Tr("table.cCombatMessage.2"),
     localization::Tr("table.cCombatMessage.3"),
     localization::Tr("table.cCombatMessage.4"),
     localization::Tr("table.cCombatMessage.5"),
     localization::Tr("table.cCombatMessage.6"),
     localization::Tr("table.cCombatMessage.7"),
     localization::Tr("table.cCombatMessage.8"),
     localization::Tr("table.cCombatMessage.9"),
     localization::Tr("table.cCombatMessage.10"),
     localization::Tr("table.cCombatMessage.11")
};
DATA(0x00481acc) H2_CONST char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] =
    { localization::Tr("table.cHeroLevel.0"), /*  уровень опыта.\n */ localization::Tr("table.cHeroLevel.1"), /*  %d уровней опыта.\n */ localization::Tr("table.cHeroLevel.2")};
DATA(0x00481ad8) H2_CONST char* cCombatHelp[KB_COMBAT_HELP_COUNT] = {
     localization::Tr("table.cCombatHelp.0"),
     localization::Tr("table.cCombatHelp.1"),
     localization::Tr("table.cCombatHelp.2"),
     localization::Tr("table.cCombatHelp.3"),
    /*  */ ""
};
DATA(0x00481aec) H2_CONST char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
     localization::Tr("table.cLongCombatHelp.0"),
     localization::Tr("table.cLongCombatHelp.1"),
     localization::Tr("table.cLongCombatHelp.2"),
     localization::Tr("table.cLongCombatHelp.3"),
     localization::Tr("table.cLongCombatHelp.4")
};
DATA(0x00481b00) H2_CONST char* cTownCommand[KB_TOWN_COMMAND_COUNT] = {
     localization::Tr("table.cTownCommand.0"),
    /* Нельзя отнять последних воинов у героя  */ localization::Tr("table.cTownCommand.1"),
     localization::Tr("table.cTownCommand.2"),
     localization::Tr("table.cTownCommand.3"),
     localization::Tr("table.cTownCommand.4"),
     localization::Tr("table.cTownCommand.5"),
     localization::Tr("table.cTownCommand.6"),
     localization::Tr("table.cTownCommand.7"),
     localization::Tr("table.cTownCommand.8"),
    /*  */ "",
     localization::Tr("table.cTownCommand.10"),
     localization::Tr("table.cTownCommand.11"),
    /* %s */ "%s",
     localization::Tr("table.cTownCommand.13"),
     localization::Tr("table.cTownCommand.14"),
     localization::Tr("table.cTownCommand.15"),
     localization::Tr("table.cTownCommand.16"),
     localization::Tr("table.cTownCommand.17"),
     localization::Tr("table.cTownCommand.18"),
     localization::Tr("table.cTownCommand.19"),
     localization::Tr("table.cTownCommand.20"),
     localization::Tr("table.cTownCommand.21"),
     localization::Tr("table.cTownCommand.22"),
     localization::Tr("table.cTownCommand.23"),
     localization::Tr("table.cTownCommand.24"),
     localization::Tr("table.cTownCommand.25"),
     localization::Tr("table.cTownCommand.26"),
     localization::Tr("table.cTownCommand.27")
};
DATA(0x00481b70) H2_CONST char* gHeroDefaultNames[GAME_HERO_COUNT] = {
     localization::Tr("table.gHeroDefaultNames.0"), localization::Tr("table.gHeroDefaultNames.1"), localization::Tr("table.gHeroDefaultNames.2"), localization::Tr("table.gHeroDefaultNames.3"), localization::Tr("table.gHeroDefaultNames.4"), localization::Tr("table.gHeroDefaultNames.5"), localization::Tr("table.gHeroDefaultNames.6"),
     localization::Tr("table.gHeroDefaultNames.7"), localization::Tr("table.gHeroDefaultNames.8"), localization::Tr("table.gHeroDefaultNames.9"), localization::Tr("table.gHeroDefaultNames.10"), localization::Tr("table.gHeroDefaultNames.11"), localization::Tr("table.gHeroDefaultNames.12"), localization::Tr("table.gHeroDefaultNames.13"),
     localization::Tr("table.gHeroDefaultNames.14"), localization::Tr("table.gHeroDefaultNames.15"), localization::Tr("table.gHeroDefaultNames.16"), localization::Tr("table.gHeroDefaultNames.17"), localization::Tr("table.gHeroDefaultNames.18"), localization::Tr("table.gHeroDefaultNames.19"), localization::Tr("table.gHeroDefaultNames.20"),
     localization::Tr("table.gHeroDefaultNames.21"), localization::Tr("table.gHeroDefaultNames.22"), localization::Tr("table.gHeroDefaultNames.23"), localization::Tr("table.gHeroDefaultNames.24"), localization::Tr("table.gHeroDefaultNames.25"), localization::Tr("table.gHeroDefaultNames.26"), localization::Tr("table.gHeroDefaultNames.27"),
     localization::Tr("table.gHeroDefaultNames.28"), localization::Tr("table.gHeroDefaultNames.29"), localization::Tr("table.gHeroDefaultNames.30"), localization::Tr("table.gHeroDefaultNames.31"), localization::Tr("table.gHeroDefaultNames.32"), localization::Tr("table.gHeroDefaultNames.33"), localization::Tr("table.gHeroDefaultNames.34"),
     localization::Tr("table.gHeroDefaultNames.35"), localization::Tr("table.gHeroDefaultNames.36"), localization::Tr("table.gHeroDefaultNames.37"), localization::Tr("table.gHeroDefaultNames.38"), localization::Tr("table.gHeroDefaultNames.39"), localization::Tr("table.gHeroDefaultNames.40"), localization::Tr("table.gHeroDefaultNames.41"),
     localization::Tr("table.gHeroDefaultNames.42"), localization::Tr("table.gHeroDefaultNames.43"), localization::Tr("table.gHeroDefaultNames.44"), localization::Tr("table.gHeroDefaultNames.45"), localization::Tr("table.gHeroDefaultNames.46"), localization::Tr("table.gHeroDefaultNames.47"), localization::Tr("table.gHeroDefaultNames.48"),
     localization::Tr("table.gHeroDefaultNames.49"), localization::Tr("table.gHeroDefaultNames.50"), localization::Tr("table.gHeroDefaultNames.51"), localization::Tr("table.gHeroDefaultNames.52"), localization::Tr("table.gHeroDefaultNames.53")
};
DATA(0x00481c48) H2_CONST char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
     localization::Tr("table.gNewGameHelp.0"),
     localization::Tr("table.gNewGameHelp.1"),
     localization::Tr("table.gNewGameHelp.2"),
     localization::Tr("table.gNewGameHelp.3"),
     localization::Tr("table.gNewGameHelp.4"),
     localization::Tr("table.gNewGameHelp.5"),
     localization::Tr("table.gNewGameHelp.6"),
     localization::Tr("table.gNewGameHelp.7")
};
DATA(0x00481c68) H2_CONST char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
     localization::Tr("table.gSetupBaudHelp.0"),
     localization::Tr("table.gSetupBaudHelp.1"),
     localization::Tr("table.gSetupBaudHelp.2"),
     localization::Tr("table.gSetupBaudHelp.3"),
     localization::Tr("table.gSetupBaudHelp.4")
};
DATA(0x00481c7c) H2_CONST char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
     localization::Tr("table.gSetupComPortHelp.0"),
     localization::Tr("table.gSetupComPortHelp.1"),
     localization::Tr("table.gSetupComPortHelp.2"),
     localization::Tr("table.gSetupComPortHelp.3"),
     localization::Tr("table.gSetupComPortHelp.4")
};
DATA(0x00481c90) H2_CONST char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
     localization::Tr("table.gSetupDCBaudHelp.0"),
     localization::Tr("table.gSetupDCBaudHelp.1"),
     localization::Tr("table.gSetupDCBaudHelp.2"),
     localization::Tr("table.gSetupDCBaudHelp.3"),
     localization::Tr("table.gSetupDCBaudHelp.4")
};
DATA(0x00481ca4) H2_CONST char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
     localization::Tr("table.gSetupDCComPortHelp.0"),
     localization::Tr("table.gSetupDCComPortHelp.1"),
     localization::Tr("table.gSetupDCComPortHelp.2"),
     localization::Tr("table.gSetupDCComPortHelp.3"),
     localization::Tr("table.gSetupDCComPortHelp.4")
};
DATA(0x00481cb8) H2_CONST char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
     localization::Tr("table.gSetupHotSeatGameHelp.0"),
     localization::Tr("table.gSetupHotSeatGameHelp.1"),
     localization::Tr("table.gSetupHotSeatGameHelp.2"),
     localization::Tr("table.gSetupHotSeatGameHelp.3"),
     localization::Tr("table.gSetupHotSeatGameHelp.4"),
     localization::Tr("table.gSetupHotSeatGameHelp.5")
};
DATA(0x00481cd0) H2_CONST char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
     localization::Tr("table.gSetupModemGameHelp.0"),

    (localization::Tr("table.gSetupModemGameHelp.1")),
     localization::Tr("table.gSetupModemGameHelp.2"),
     localization::Tr("table.gSetupModemGameHelp.3")
};
DATA(0x00481ce0) H2_CONST char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
     localization::Tr("table.gSetupDCGameHelp.0"),

    (localization::Tr("table.gSetupDCGameHelp.1")),
     localization::Tr("table.gSetupDCGameHelp.2"),
     localization::Tr("table.gSetupDCGameHelp.3")
};
DATA(0x00481cf0) H2_CONST char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
     localization::Tr("table.gSetupMultiPlayerGameHelp.0"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.1"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.2"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.3"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.4")
};
DATA(0x00481d04) H2_CONST char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
     localization::Tr("table.gSetupNetworkGameHelp.0"),
     localization::Tr("table.gSetupNetworkGameHelp.1"),
     localization::Tr("table.gSetupNetworkGameHelp.2")
};
DATA(0x00481d10) H2_CONST char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
     localization::Tr("table.gSetupNetworkGame2Help.0"),
     localization::Tr("table.gSetupNetworkGame2Help.1"),
     localization::Tr("table.gSetupNetworkGame2Help.2"),
     localization::Tr("table.gSetupNetworkGame2Help.3")
};
DATA(0x00481d20) H2_CONST char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
     localization::Tr("table.gSetupGameHelp.0"),
     localization::Tr("table.gSetupGameHelp.1"),
     localization::Tr("table.gSetupGameHelp.2"),
     localization::Tr("table.gSetupGameHelp.3")
};
DATA(0x00481d30) H2_CONST char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
     localization::Tr("table.cBattleResults.0"),
     localization::Tr("table.cBattleResults.1"),
     localization::Tr("table.cBattleResults.2"),
     localization::Tr("table.cBattleResults.3"),
     localization::Tr("table.cBattleResults.4"),
     localization::Tr("table.cBattleResults.5"),
     localization::Tr("table.cBattleResults.6"),
     localization::Tr("table.cBattleResults.7"),
     localization::Tr("table.cBattleResults.8"),
     localization::Tr("table.cBattleResults.9"),
     localization::Tr("table.cBattleResults.10")
};
DATA(0x00481d5c) H2_CONST char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
     localization::Tr("table.cMoraleInfo.0"),
     localization::Tr("table.cMoraleInfo.1"),
     localization::Tr("table.cMoraleInfo.2"),
     localization::Tr("table.cMoraleInfo.3"),
     localization::Tr("table.cMoraleInfo.4"),
     localization::Tr("table.cMoraleInfo.5"),
     localization::Tr("table.cMoraleInfo.6"),
     localization::Tr("table.cMoraleInfo.7"),
     localization::Tr("table.cMoraleInfo.8"),
     localization::Tr("table.cMoraleInfo.9"),
     localization::Tr("table.cMoraleInfo.10"),
     localization::Tr("table.cMoraleInfo.11"),
     localization::Tr("table.cMoraleInfo.12"),
     localization::Tr("table.cMoraleInfo.13"),
     localization::Tr("table.cMoraleInfo.14"),
     localization::Tr("table.cMoraleInfo.15"),
     localization::Tr("table.cMoraleInfo.16"),
     localization::Tr("table.cMoraleInfo.17"),
     localization::Tr("table.cMoraleInfo.18"),
     localization::Tr("table.cMoraleInfo.19"),
     localization::Tr("table.cMoraleInfo.20"),
     localization::Tr("table.cMoraleInfo.21"),
     localization::Tr("table.cMoraleInfo.22"),
     localization::Tr("table.cMoraleInfo.23"),
     localization::Tr("table.cMoraleInfo.24"),
     localization::Tr("table.cMoraleInfo.25"),
     localization::Tr("table.cMoraleInfo.26"),
     localization::Tr("table.cMoraleInfo.27"),
     localization::Tr("table.cMoraleInfo.28"),
     localization::Tr("table.cMoraleInfo.29"),
     localization::Tr("table.cMoraleInfo.30"),
     localization::Tr("table.cMoraleInfo.31")
};
DATA(0x00481ddc) H2_CONST char* cMapSize[KB_MAP_SIZE_TEXT_COUNT] = { localization::Tr("table.cMapSize.0"), localization::Tr("table.cMapSize.1"), localization::Tr("table.cMapSize.2"), localization::Tr("table.cMapSize.3")};
DATA(0x00481dec) H2_CONST char* cDifficulty[IDX(DIFFICULTY_COUNT)] =
    { localization::Tr("table.cDifficulty.0"), localization::Tr("table.cDifficulty.1"), localization::Tr("table.cDifficulty.2"), localization::Tr("table.cDifficulty.3"), localization::Tr("table.cDifficulty.4")};
DATA(0x00481e00) H2_CONST char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] = { localization::Tr("table.cStartDifficulty.0"), localization::Tr("table.cStartDifficulty.1"), localization::Tr("table.cStartDifficulty.2"), localization::Tr("table.cStartDifficulty.3")};
DATA(0x00481e10) H2_CONST char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] =
    { localization::Tr("table.cCampaignLeaders.0"), localization::Tr("table.cCampaignLeaders.1"), localization::Tr("table.cCampaignLeaders.2"), localization::Tr("table.cCampaignLeaders.3")};
DATA(0x00481e20) H2_CONST char* cWinText[KB_WIN_TEXT_COUNT] =
    { localization::Tr("table.cWinText.0"), localization::Tr("table.cWinText.1"), localization::Tr("table.cWinText.2"), localization::Tr("table.cWinText.3"), localization::Tr("table.cWinText.4")};
DATA(0x00481e34) H2_CONST char* cHumanDifficulty[IDX(DIFFICULTY_COUNT)] =
    { localization::Tr("table.cHumanDifficulty.0"), localization::Tr("table.cHumanDifficulty.1"), localization::Tr("table.cHumanDifficulty.2"), localization::Tr("table.cHumanDifficulty.3"), localization::Tr("table.cHumanDifficulty.4")};
DATA(0x00481e48) H2_CONST char* cHumanInfoDifficulty[IDX(DIFFICULTY_COUNT)] =
    { localization::Tr("table.cHumanInfoDifficulty.0"), localization::Tr("table.cHumanInfoDifficulty.1"), localization::Tr("table.cHumanInfoDifficulty.2"), localization::Tr("table.cHumanInfoDifficulty.3"), localization::Tr("table.cHumanInfoDifficulty.4")};
DATA(0x00481e5c) H2_CONST char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] =
    {/* MIDI */ "MIDI", localization::Tr("table.musicQualityText.1"), localization::Tr("table.musicQualityText.2")};
DATA(0x00481e68) H2_CONST char* gSpellDesc[IDX(SPELL_COUNT)] = {
     localization::Tr("table.gSpellDesc.0"),
     localization::Tr("table.gSpellDesc.1"),
     localization::Tr("table.gSpellDesc.2"),
     localization::Tr("table.gSpellDesc.3"),
     localization::Tr("table.gSpellDesc.4"),
     localization::Tr("table.gSpellDesc.5"),
     localization::Tr("table.gSpellDesc.6"),
     localization::Tr("table.gSpellDesc.7"),
     localization::Tr("table.gSpellDesc.8"),
     localization::Tr("table.gSpellDesc.9"),
     localization::Tr("table.gSpellDesc.10"),
     localization::Tr("table.gSpellDesc.11"),
     localization::Tr("table.gSpellDesc.12"),
     localization::Tr("table.gSpellDesc.13"),
     localization::Tr("table.gSpellDesc.14"),
     localization::Tr("table.gSpellDesc.15"),
     localization::Tr("table.gSpellDesc.16"),
     localization::Tr("table.gSpellDesc.17"),
     localization::Tr("table.gSpellDesc.18"),
     localization::Tr("table.gSpellDesc.19"),
     localization::Tr("table.gSpellDesc.20"),
     localization::Tr("table.gSpellDesc.21"),
     localization::Tr("table.gSpellDesc.22"),
     localization::Tr("table.gSpellDesc.23"),
     localization::Tr("table.gSpellDesc.24"),
     localization::Tr("table.gSpellDesc.25"),
     localization::Tr("table.gSpellDesc.26"),
     localization::Tr("table.gSpellDesc.27"),
     localization::Tr("table.gSpellDesc.28"),
     localization::Tr("table.gSpellDesc.29"),
     localization::Tr("table.gSpellDesc.30"),
     localization::Tr("table.gSpellDesc.31"),
     localization::Tr("table.gSpellDesc.32"),
     localization::Tr("table.gSpellDesc.33"),
     localization::Tr("table.gSpellDesc.34"),
     localization::Tr("table.gSpellDesc.35"),
     localization::Tr("table.gSpellDesc.36"),
     localization::Tr("table.gSpellDesc.37"),
     localization::Tr("table.gSpellDesc.38"),
     localization::Tr("table.gSpellDesc.39"),
     localization::Tr("table.gSpellDesc.40"),
     localization::Tr("table.gSpellDesc.41"),
     localization::Tr("table.gSpellDesc.42"),
     localization::Tr("table.gSpellDesc.43"),
     localization::Tr("table.gSpellDesc.44"),
     localization::Tr("table.gSpellDesc.45"),
     localization::Tr("table.gSpellDesc.46"),
     localization::Tr("table.gSpellDesc.47"),
     localization::Tr("table.gSpellDesc.48"),
     localization::Tr("table.gSpellDesc.49"),
     localization::Tr("table.gSpellDesc.50"),
     localization::Tr("table.gSpellDesc.51"),
     localization::Tr("table.gSpellDesc.52"),
     localization::Tr("table.gSpellDesc.53"),
     localization::Tr("table.gSpellDesc.54"),
     localization::Tr("table.gSpellDesc.55"),
     localization::Tr("table.gSpellDesc.56"),
     localization::Tr("table.gSpellDesc.57"),
     localization::Tr("table.gSpellDesc.58"),
     localization::Tr("table.gSpellDesc.59"),
     localization::Tr("table.gSpellDesc.60"),
     localization::Tr("table.gSpellDesc.61"),
     localization::Tr("table.gSpellDesc.62"),
     localization::Tr("table.gSpellDesc.63"),
     localization::Tr("table.gSpellDesc.64")
};
DATA(0x00481f6c) H2_CONST char* gSpellNames[IDX(SPELL_COUNT)] = {
     localization::Tr("table.gSpellNames.0"),
     localization::Tr("table.gSpellNames.1"),
     localization::Tr("table.gSpellNames.2"),
     localization::Tr("table.gSpellNames.3"),
     localization::Tr("table.gSpellNames.4"),
     localization::Tr("table.gSpellNames.5"),
     localization::Tr("table.gSpellNames.6"),
     localization::Tr("table.gSpellNames.7"),
     localization::Tr("table.gSpellNames.8"),
     localization::Tr("table.gSpellNames.9"),
     localization::Tr("table.gSpellNames.10"),
     localization::Tr("table.gSpellNames.11"),
     localization::Tr("table.gSpellNames.12"),
     localization::Tr("table.gSpellNames.13"),
     localization::Tr("table.gSpellNames.14"),
     localization::Tr("table.gSpellNames.15"),
     localization::Tr("table.gSpellNames.16"),
     localization::Tr("table.gSpellNames.17"),
     localization::Tr("table.gSpellNames.18"),
     localization::Tr("table.gSpellNames.19"),
     localization::Tr("table.gSpellNames.20"),
     localization::Tr("table.gSpellNames.21"),
     localization::Tr("table.gSpellNames.22"),
     localization::Tr("table.gSpellNames.23"),
     localization::Tr("table.gSpellNames.24"),
     localization::Tr("table.gSpellNames.25"),
     localization::Tr("table.gSpellNames.26"),
     localization::Tr("table.gSpellNames.27"),
     localization::Tr("table.gSpellNames.28"),
     localization::Tr("table.gSpellNames.29"),
     localization::Tr("table.gSpellNames.30"),
     localization::Tr("table.gSpellNames.31"),
     localization::Tr("table.gSpellNames.32"),
     localization::Tr("table.gSpellNames.33"),
     localization::Tr("table.gSpellNames.34"),
     localization::Tr("table.gSpellNames.35"),
     localization::Tr("table.gSpellNames.36"),
     localization::Tr("table.gSpellNames.37"),
     localization::Tr("table.gSpellNames.38"),
     localization::Tr("table.gSpellNames.39"),
     localization::Tr("table.gSpellNames.40"),
     localization::Tr("table.gSpellNames.41"),
     localization::Tr("table.gSpellNames.42"),
     localization::Tr("table.gSpellNames.43"),
     localization::Tr("table.gSpellNames.44"),
     localization::Tr("table.gSpellNames.45"),
     localization::Tr("table.gSpellNames.46"),
     localization::Tr("table.gSpellNames.47"),
     localization::Tr("table.gSpellNames.48"),
     localization::Tr("table.gSpellNames.49"),
     localization::Tr("table.gSpellNames.50"),
     localization::Tr("table.gSpellNames.51"),
     localization::Tr("table.gSpellNames.52"),
     localization::Tr("table.gSpellNames.53"),
     localization::Tr("table.gSpellNames.54"),
     localization::Tr("table.gSpellNames.55"),
     localization::Tr("table.gSpellNames.56"),
     localization::Tr("table.gSpellNames.57"),
     localization::Tr("table.gSpellNames.58"),
     localization::Tr("table.gSpellNames.59"),
     localization::Tr("table.gSpellNames.60"),
     localization::Tr("table.gSpellNames.61"),
     localization::Tr("table.gSpellNames.62"),
     localization::Tr("table.gSpellNames.63"),
     localization::Tr("table.gSpellNames.64")
};
DATA(0x00482070) H2_CONST char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] =
    { localization::Tr("table.gSecondarySkillLevels.0"), localization::Tr("table.gSecondarySkillLevels.1"), localization::Tr("table.gSecondarySkillLevels.2")};
DATA(0x0048207c) H2_CONST char* gSecondarySkills[IDX(HERO_SKILL_COUNT)] = {
     localization::Tr("table.gSecondarySkills.0"),
     localization::Tr("table.gSecondarySkills.1"),
     localization::Tr("table.gSecondarySkills.2"),
     localization::Tr("table.gSecondarySkills.3"),
     localization::Tr("table.gSecondarySkills.4"),
     localization::Tr("table.gSecondarySkills.5"),
     localization::Tr("table.gSecondarySkills.6"),
     localization::Tr("table.gSecondarySkills.7"),
     localization::Tr("table.gSecondarySkills.8"),
     localization::Tr("table.gSecondarySkills.9"),
     localization::Tr("table.gSecondarySkills.10"),
     localization::Tr("table.gSecondarySkills.11"),
     localization::Tr("table.gSecondarySkills.12"),
     localization::Tr("table.gSecondarySkills.13")
};
DATA(0x004820b4) H2_CONST char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
     localization::Tr("table.gNeutralBuildingNames.0"),
     localization::Tr("table.gNeutralBuildingNames.1"),
     localization::Tr("table.gNeutralBuildingNames.2"),
     localization::Tr("table.gNeutralBuildingNames.3"),
     localization::Tr("table.gNeutralBuildingNames.4"),
     localization::Tr("table.gNeutralBuildingNames.5"),
     localization::Tr("table.gNeutralBuildingNames.6"),
     localization::Tr("table.gNeutralBuildingNames.7"),
     localization::Tr("table.gNeutralBuildingNames.8"),
     localization::Tr("table.gNeutralBuildingNames.9"),
     localization::Tr("table.gNeutralBuildingNames.10"),
    /*  */ "",
     localization::Tr("table.gNeutralBuildingNames.12"),
    /*  */ "",
     localization::Tr("table.gNeutralBuildingNames.14"),
     localization::Tr("table.gNeutralBuildingNames.15"),
    /*  */ "",
    /*  */ "",
    /*  */ ""
};
DATA(0x00482100) H2_CONST char* gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT] = {
     localization::Tr("table.gWellExtraNames.0"),
     localization::Tr("table.gWellExtraNames.1"),
     localization::Tr("table.gWellExtraNames.2"),
     localization::Tr("table.gWellExtraNames.3"),
     localization::Tr("table.gWellExtraNames.4"),
     localization::Tr("table.gWellExtraNames.5"),
     localization::Tr("table.gWellExtraNames.6")
};
DATA(0x0048211c) H2_CONST char* gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT] =
    { localization::Tr("table.gSpecialBuildingNames.0"), localization::Tr("table.gSpecialBuildingNames.1"), localization::Tr("table.gSpecialBuildingNames.2"), localization::Tr("table.gSpecialBuildingNames.3"), localization::Tr("table.gSpecialBuildingNames.4"), localization::Tr("table.gSpecialBuildingNames.5"), localization::Tr("table.gSpecialBuildingNames.6")};
DATA(0x00482138) H2_CONST char* gDwellingNames[IDX(FACTION_COUNT)][DWELLING_TYPE_COUNT] = {
    {localization::Tr("table.gDwellingNames.0.0"),
     localization::Tr("table.gDwellingNames.0.1"),
     localization::Tr("table.gDwellingNames.0.2"),
     localization::Tr("table.gDwellingNames.0.3"),
     localization::Tr("table.gDwellingNames.0.4"),
     localization::Tr("table.gDwellingNames.0.5"),
     localization::Tr("table.gDwellingNames.0.6"),
     localization::Tr("table.gDwellingNames.0.7"),
     localization::Tr("table.gDwellingNames.0.8"),
     localization::Tr("table.gDwellingNames.0.9"),
     localization::Tr("table.gDwellingNames.0.10"),
     /*  */ ""},
    {localization::Tr("table.gDwellingNames.1.0"),
     localization::Tr("table.gDwellingNames.1.1"),
     localization::Tr("table.gDwellingNames.1.2"),
     localization::Tr("table.gDwellingNames.1.3"),
     localization::Tr("table.gDwellingNames.1.4"),
     localization::Tr("table.gDwellingNames.1.5"),
     localization::Tr("table.gDwellingNames.1.6"),
     /*  */ "",
     localization::Tr("table.gDwellingNames.1.8"),
     localization::Tr("table.gDwellingNames.1.9"),
     /*  */ "",
     /*  */ ""},
    {localization::Tr("table.gDwellingNames.2.0"),
     localization::Tr("table.gDwellingNames.2.1"),
     localization::Tr("table.gDwellingNames.2.2"),
     localization::Tr("table.gDwellingNames.2.3"),
     localization::Tr("table.gDwellingNames.2.4"),
     localization::Tr("table.gDwellingNames.2.5"),
     localization::Tr("table.gDwellingNames.2.6"),
     localization::Tr("table.gDwellingNames.2.7"),
     localization::Tr("table.gDwellingNames.2.8"),
     /*  */ "",
     /*  */ "",
     /*  */ ""},
    {localization::Tr("table.gDwellingNames.3.0"),
     localization::Tr("table.gDwellingNames.3.1"),
     localization::Tr("table.gDwellingNames.3.2"),
     localization::Tr("table.gDwellingNames.3.3"),
     localization::Tr("table.gDwellingNames.3.4"),
     localization::Tr("table.gDwellingNames.3.5"),
     /*  */ "",
     /*  */ "",
     localization::Tr("table.gDwellingNames.3.8"),
     /*  */ "",
     localization::Tr("table.gDwellingNames.3.10"),
     localization::Tr("table.gDwellingNames.3.11")},
    {localization::Tr("table.gDwellingNames.4.0"),
     localization::Tr("table.gDwellingNames.4.1"),
     localization::Tr("table.gDwellingNames.4.2"),
     localization::Tr("table.gDwellingNames.4.3"),
     localization::Tr("table.gDwellingNames.4.4"),
     localization::Tr("table.gDwellingNames.4.5"),
     /*  */ "",
     localization::Tr("table.gDwellingNames.4.7"),
     /*  */ "",
     localization::Tr("table.gDwellingNames.4.9"),
     localization::Tr("table.gDwellingNames.4.10"),
     /*  */ ""},
    {localization::Tr("table.gDwellingNames.5.0"),
     localization::Tr("table.gDwellingNames.5.1"),
     localization::Tr("table.gDwellingNames.5.2"),
     localization::Tr("table.gDwellingNames.5.3"),
     localization::Tr("table.gDwellingNames.5.4"),
     localization::Tr("table.gDwellingNames.5.5"),
     localization::Tr("table.gDwellingNames.5.6"),
     localization::Tr("table.gDwellingNames.5.7"),
     localization::Tr("table.gDwellingNames.5.8"),
     localization::Tr("table.gDwellingNames.5.9"),
     /*  */ "",
     /*  */ ""}
};
DATA(0x00482258) H2_CONST char* cSecSkillDesc[IDX(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    { localization::Tr("table.cSecSkillDesc.0.0"),
      localization::Tr("table.cSecSkillDesc.0.1"),
      localization::Tr("table.cSecSkillDesc.0.2")},
    { localization::Tr("table.cSecSkillDesc.1.0"),
      localization::Tr("table.cSecSkillDesc.1.1"),
      localization::Tr("table.cSecSkillDesc.1.2")},
    { localization::Tr("table.cSecSkillDesc.2.0"),
      localization::Tr("table.cSecSkillDesc.2.1"),
      localization::Tr("table.cSecSkillDesc.2.2")},
    { localization::Tr("table.cSecSkillDesc.3.0"),
      localization::Tr("table.cSecSkillDesc.3.1"),
      localization::Tr("table.cSecSkillDesc.3.2")},
    { localization::Tr("table.cSecSkillDesc.4.0"),
      localization::Tr("table.cSecSkillDesc.4.1"),
      localization::Tr("table.cSecSkillDesc.4.2")},
    { localization::Tr("table.cSecSkillDesc.5.0"),
      localization::Tr("table.cSecSkillDesc.5.1"),
      localization::Tr("table.cSecSkillDesc.5.2")},
    { localization::Tr("table.cSecSkillDesc.6.0"),
      localization::Tr("table.cSecSkillDesc.6.1"),
      localization::Tr("table.cSecSkillDesc.6.2")},
    { localization::Tr("table.cSecSkillDesc.7.0"),
      localization::Tr("table.cSecSkillDesc.7.1"),
      localization::Tr("table.cSecSkillDesc.7.2")},
    { localization::Tr("table.cSecSkillDesc.8.0"),
      localization::Tr("table.cSecSkillDesc.8.1"),
      localization::Tr("table.cSecSkillDesc.8.2")},
    { localization::Tr("table.cSecSkillDesc.9.0"),
      localization::Tr("table.cSecSkillDesc.9.1"),
      localization::Tr("table.cSecSkillDesc.9.2")},
    { localization::Tr("table.cSecSkillDesc.10.0"),
      localization::Tr("table.cSecSkillDesc.10.1"),
      localization::Tr("table.cSecSkillDesc.10.2")},
    { localization::Tr("table.cSecSkillDesc.11.0"),
      localization::Tr("table.cSecSkillDesc.11.1"),
      localization::Tr("table.cSecSkillDesc.11.2")},
    { localization::Tr("table.cSecSkillDesc.12.0"),
      localization::Tr("table.cSecSkillDesc.12.1"),
      localization::Tr("table.cSecSkillDesc.12.2")},
    { localization::Tr("table.cSecSkillDesc.13.0"),
      localization::Tr("table.cSecSkillDesc.13.1"),
      localization::Tr("table.cSecSkillDesc.13.2")}
};
DATA(0x00482300) H2_CONST char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
     localization::Tr("table.cBuildingInfoNeutral.0"),
     localization::Tr("table.cBuildingInfoNeutral.1"),
     localization::Tr("table.cBuildingInfoNeutral.2"),
     localization::Tr("table.cBuildingInfoNeutral.3"),
     localization::Tr("table.cBuildingInfoNeutral.4"),
     localization::Tr("table.cBuildingInfoNeutral.5"),
     localization::Tr("table.cBuildingInfoNeutral.6"),
     localization::Tr("table.cBuildingInfoNeutral.7"),
     localization::Tr("table.cBuildingInfoNeutral.8"),
     localization::Tr("table.cBuildingInfoNeutral.9"),
     localization::Tr("table.cBuildingInfoNeutral.10"),
    /*  */ "",
     localization::Tr("table.cBuildingInfoNeutral.12"),
    /*  */ "",
     localization::Tr("table.cBuildingInfoNeutral.14"),
     localization::Tr("table.cBuildingInfoNeutral.15"),
    /*  */ "",
    /*  */ "",
    /*  */ ""
};
DATA(0x0048234c) H2_CONST char* gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT] = {
     localization::Tr("table.gBuildingInfoSpecial.0"),
     localization::Tr("table.gBuildingInfoSpecial.1"),
     localization::Tr("table.gBuildingInfoSpecial.2"),
     localization::Tr("table.gBuildingInfoSpecial.3"),
     localization::Tr("table.gBuildingInfoSpecial.4"),
     localization::Tr("table.gBuildingInfoSpecial.5")
};
DATA(0x00482364) H2_CONST char* cDirections[KB_DIRECTION_TEXT_COUNT] = {
     localization::Tr("table.cDirections.0"),
     localization::Tr("table.cDirections.1"),
     localization::Tr("table.cDirections.2"),
     localization::Tr("table.cDirections.3"),
     localization::Tr("table.cDirections.4"),
     localization::Tr("table.cDirections.5"),
     localization::Tr("table.cDirections.6"),
     localization::Tr("table.cDirections.7"),
     localization::Tr("table.cDirections.8")
};
DATA(0x00482388) H2_CONST char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
     localization::Tr("table.cRumourTerrainDescriptions.0"),
     localization::Tr("table.cRumourTerrainDescriptions.1"),
     localization::Tr("table.cRumourTerrainDescriptions.2"),
     localization::Tr("table.cRumourTerrainDescriptions.3"),
     localization::Tr("table.cRumourTerrainDescriptions.4"),
     localization::Tr("table.cRumourTerrainDescriptions.5"),
     localization::Tr("table.cRumourTerrainDescriptions.6"),
     localization::Tr("table.cRumourTerrainDescriptions.7"),
     localization::Tr("table.cRumourTerrainDescriptions.8")
};
DATA(0x004823ac) H2_CONST char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] = { localization::Tr("table.gInterfaceTypeText.0"), localization::Tr("table.gInterfaceTypeText.1"), localization::Tr("table.gInterfaceTypeText.2")};
DATA(0x004823b8) H2_CONST char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = { localization::Tr("table.cBWMouseText.0"), localization::Tr("table.cBWMouseText.1")};
DATA(0x004823c0) H2_CONST char* combatSpeedText[KB_COMBAT_SPEED_COUNT] = { localization::Tr("table.combatSpeedText.0"), localization::Tr("table.combatSpeedText.1"), localization::Tr("table.combatSpeedText.2")};
DATA(0x004823cc) H2_CONST char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] = { localization::Tr("table.combatMiniInfoText.0"), localization::Tr("table.combatMiniInfoText.1"), localization::Tr("table.combatMiniInfoText.2")};
DATA(0x004823d8) H2_CONST char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
    /* \n\n\n***Command Line Help***\n */ "\n\n\n***Command Line Help***\n",
    /* \n */ "\n",
     localization::Tr("system.command_line.disable_digital_sound"),
     localization::Tr("system.command_line.disable_midi"),
     localization::Tr("system.command_line.disable_music"),
     localization::Tr("system.command_line.skip_intro"),
    /* \n */ "\n",
    /* \n */ "\n",
     localization::Tr("system.command_line.example"),
    /* \n */ "\n",
    /* HEROES2D /R0 /I0\n */ "HEROES2D /R0 /I0\n",
    /* \n */ "\n",
     localization::Tr("system.command_line.dos_example"),
     localization::Tr("system.command_line.disabled_example")
};
DATA(0x00482410) H2_CONST char* cOverviewText[KB_OVERVIEW_TEXT_COUNT] =
    { localization::Tr("table.cOverviewText.0"), localization::Tr("table.cOverviewText.1"), localization::Tr("table.cOverviewText.2"), localization::Tr("table.cOverviewText.3"), localization::Tr("table.cOverviewText.4"), localization::Tr("table.cOverviewText.5")};
DATA(0x00482428) H2_CONST char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
     localization::Tr("table.cWinComError.0"),
     localization::Tr("table.cWinComError.1"),
     localization::Tr("table.cWinComError.2"),
     localization::Tr("table.cWinComError.3"),
     localization::Tr("table.cWinComError.4"),
     localization::Tr("table.cWinComError.5")
};
DATA(0x00482440) H2_CONST char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] =
    { localization::Tr("table.cMiniViewText.0"), localization::Tr("table.cMiniViewText.1"), localization::Tr("table.cMiniViewText.2"), localization::Tr("table.cMiniViewText.3"), localization::Tr("table.cMiniViewText.4"), localization::Tr("table.cMiniViewText.5"), localization::Tr("table.cMiniViewText.6"), localization::Tr("table.cMiniViewText.7"), localization::Tr("table.cMiniViewText.8")};
DATA(0x00482464) H2_CONST char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
     localization::Tr("table.gFileRequestHelp.0"),
     localization::Tr("table.gFileRequestHelp.1"),
     localization::Tr("table.gFileRequestHelp.2"),
     localization::Tr("table.gFileRequestHelp.3"),
     localization::Tr("table.gFileRequestHelp.4"),
     localization::Tr("table.gFileRequestHelp.5"),
     localization::Tr("table.gFileRequestHelp.6"),
     localization::Tr("table.gFileRequestHelp.7"),
     localization::Tr("table.gFileRequestHelp.8"),
     localization::Tr("table.gFileRequestHelp.9"),
     localization::Tr("table.gFileRequestHelp.10"),
     localization::Tr("table.gFileRequestHelp.11"),
     localization::Tr("table.gFileRequestHelp.12"),
     localization::Tr("table.gFileRequestHelp.13"),
     localization::Tr("table.gFileRequestHelp.14")
};
DATA(0x004824a0) H2_CONST char* cPersonality[KB_PERSONALITY_TEXT_COUNT] = { localization::Tr("table.cPersonality.0"), localization::Tr("table.cPersonality.1"), localization::Tr("table.cPersonality.2"), localization::Tr("table.cPersonality.3")};
DATA(0x004824b0) H2_CONST char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
    { localization::Tr("table.gArmySizeNames.0.0"), localization::Tr("table.gArmySizeNames.0.1"), localization::Tr("table.gArmySizeNames.0.2")},
    { localization::Tr("table.gArmySizeNames.1.0"), localization::Tr("table.gArmySizeNames.1.1"), localization::Tr("table.gArmySizeNames.1.2")},
    { localization::Tr("table.gArmySizeNames.2.0"), localization::Tr("table.gArmySizeNames.2.1"), localization::Tr("table.gArmySizeNames.2.2")},
    { localization::Tr("table.gArmySizeNames.3.0"), localization::Tr("table.gArmySizeNames.3.1"), localization::Tr("table.gArmySizeNames.3.2")},
    { localization::Tr("table.gArmySizeNames.4.0"), localization::Tr("table.gArmySizeNames.4.1"), localization::Tr("table.gArmySizeNames.4.2")},
    { localization::Tr("table.gArmySizeNames.5.0"), localization::Tr("table.gArmySizeNames.5.1"), localization::Tr("table.gArmySizeNames.5.2")},
    { localization::Tr("table.gArmySizeNames.6.0"), localization::Tr("table.gArmySizeNames.6.1"), localization::Tr("table.gArmySizeNames.6.2")},
    { localization::Tr("table.gArmySizeNames.7.0"), localization::Tr("table.gArmySizeNames.7.1"), localization::Tr("table.gArmySizeNames.7.2")},
    { localization::Tr("table.gArmySizeNames.8.0"), localization::Tr("table.gArmySizeNames.8.1"), localization::Tr("table.gArmySizeNames.8.2")}
};
DATA(0x0048251c) H2_CONST char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
     localization::Tr("table.cRandomTavernText.0"),
     localization::Tr("table.cRandomTavernText.1"),
     localization::Tr("table.cRandomTavernText.2"),
     localization::Tr("table.cRandomTavernText.3"),
     localization::Tr("table.cRandomTavernText.4"),
     localization::Tr("table.cRandomTavernText.5"),
     localization::Tr("table.cRandomTavernText.6"),
     localization::Tr("table.cRandomTavernText.7")
};
DATA(0x0048253c) H2_CONST char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] =
    { localization::Tr("table.cRandomSignText.0"), localization::Tr("table.cRandomSignText.1"), localization::Tr("table.cRandomSignText.2"), localization::Tr("table.cRandomSignText.3")};
DATA(0x0048254c) H2_CONST char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
     localization::Tr("table.cCampaignAwards.0"),
     localization::Tr("table.cCampaignAwards.1"),
     localization::Tr("table.cCampaignAwards.2"),
     localization::Tr("table.cCampaignAwards.3"),
     localization::Tr("table.cCampaignAwards.4"),
     localization::Tr("table.cCampaignAwards.5"),
     localization::Tr("table.cCampaignAwards.6"),
     localization::Tr("table.cCampaignAwards.7"),
     localization::Tr("table.cCampaignAwards.8"),
     localization::Tr("table.cCampaignAwards.9"),
     localization::Tr("table.cCampaignAwards.10"),
     localization::Tr("table.cCampaignAwards.11")
};
DATA(0x0048257c) H2_CONST char* cCampaignName[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    { localization::Tr("table.cCampaignName.0.0"),
      localization::Tr("table.cCampaignName.0.1"),
      localization::Tr("table.cCampaignName.0.2"),
      localization::Tr("table.cCampaignName.0.3"),
      localization::Tr("table.cCampaignName.0.4"),
      localization::Tr("table.cCampaignName.0.5"),
      localization::Tr("table.cCampaignName.0.6"),
      localization::Tr("table.cCampaignName.0.7"),
      localization::Tr("table.cCampaignName.0.8"),
      localization::Tr("table.cCampaignName.0.9"),
     /*  */ "",
      localization::Tr("table.cCampaignName.0.11")},
    { localization::Tr("table.cCampaignName.1.0"),
      localization::Tr("table.cCampaignName.1.1"),
      localization::Tr("table.cCampaignName.1.2"),
      localization::Tr("table.cCampaignName.1.3"),
      localization::Tr("table.cCampaignName.1.4"),
      localization::Tr("table.cCampaignName.1.5"),
      localization::Tr("table.cCampaignName.1.6"),
      localization::Tr("table.cCampaignName.1.7"),
      localization::Tr("table.cCampaignName.1.8"),
      localization::Tr("table.cCampaignName.1.9"),
      localization::Tr("table.cCampaignName.1.10"),
      localization::Tr("table.cCampaignName.1.11")}
};
DATA(0x004825dc) H2_CONST char* cCampaignDescription[IDX(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    { localization::Tr("table.cCampaignDescription.0.0"),
      localization::Tr("table.cCampaignDescription.0.1"),
      localization::Tr("table.cCampaignDescription.0.2"),
      localization::Tr("table.cCampaignDescription.0.3"),
      localization::Tr("table.cCampaignDescription.0.4"),
      localization::Tr("table.cCampaignDescription.0.5"),
      localization::Tr("table.cCampaignDescription.0.6"),
      localization::Tr("table.cCampaignDescription.0.7"),
      localization::Tr("table.cCampaignDescription.0.8"),
      localization::Tr("table.cCampaignDescription.0.9"),
     /*  */ "",
      localization::Tr("table.cCampaignDescription.0.11")},
    { localization::Tr("table.cCampaignDescription.1.0"),
      localization::Tr("table.cCampaignDescription.1.1"),
      localization::Tr("table.cCampaignDescription.1.2"),
      localization::Tr("table.cCampaignDescription.1.3"),
      localization::Tr("table.cCampaignDescription.1.4"),
      localization::Tr("table.cCampaignDescription.1.5"),
      localization::Tr("table.cCampaignDescription.1.6"),
      localization::Tr("table.cCampaignDescription.1.7"),
      localization::Tr("table.cCampaignDescription.1.8"),
      localization::Tr("table.cCampaignDescription.1.9"),
      localization::Tr("table.cCampaignDescription.1.10"),
      localization::Tr("table.cCampaignDescription.1.11")}
};
DATA(0x0048263c) H2_CONST char* cOutOfMemory =
     localization::Tr("system.memory.requirement");
DATA(0x00482640) H2_CONST char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = { localization::Tr("table.cSlowVideoLevelText.0"), localization::Tr("table.cSlowVideoLevelText.1")};
DATA(0x00482648) H2_CONST char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
     localization::Tr("table.gSPanelHelp.0"),
     localization::Tr("table.gSPanelHelp.1"),
     localization::Tr("table.gSPanelHelp.2"),
     localization::Tr("table.gSPanelHelp.3"),
     localization::Tr("table.gSPanelHelp.4"),
     localization::Tr("table.gSPanelHelp.5"),
     localization::Tr("table.gSPanelHelp.6"),
     localization::Tr("table.gSPanelHelp.7"),
     localization::Tr("table.gSPanelHelp.8"),
     localization::Tr("table.gSPanelHelp.9")
};
DATA(0x00482670) H2_CONST char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] =
    { localization::Tr("table.xBarrierColor.0"), localization::Tr("table.xBarrierColor.1"), localization::Tr("table.xBarrierColor.2"), localization::Tr("table.xBarrierColor.3"), localization::Tr("table.xBarrierColor.4"), localization::Tr("table.xBarrierColor.5"), localization::Tr("table.xBarrierColor.6"), localization::Tr("table.xBarrierColor.7")};
DATA(0x00482690) H2_CONST char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
     localization::Tr("table.xGenericSiteNames.0"),
     localization::Tr("table.xGenericSiteNames.1"),
     localization::Tr("table.xGenericSiteNames.2"),
     localization::Tr("table.xGenericSiteNames.3"),
     localization::Tr("table.xGenericSiteNames.4"),
     localization::Tr("table.xGenericSiteNames.5"),
     localization::Tr("table.xGenericSiteNames.6")
};
DATA(0x004826ac) H2_CONST char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
     localization::Tr("table.xRecruitmentSiteNames.0"),
     localization::Tr("table.xRecruitmentSiteNames.1"),
     localization::Tr("table.xRecruitmentSiteNames.2"),
     localization::Tr("table.xRecruitmentSiteNames.3"),
     localization::Tr("table.xRecruitmentSiteNames.4")
};
// No retail code reads this; it keeps its retail .data place.
DATA(0x004826c0) i32 gUnusedData4826c0 = 250;
// The maps shipped with the game and the names an edited copy takes.
DATA(0x004826c4)
char gShippedMaps[EDITOR_SHIPPED_MAP_COUNT][EDITOR_SHIPPED_MAP_NAMES][EDITOR_SHIPPED_MAP_NAME_SIZE] = {
    {"BELTWAY.MP2", "_BELTWAY.MP2"},
    {"BROKENA.MP2", "_BROKENA.MP2"},
    {"DEATHG.MP2", "_DEATHG.MP2"},
    {"DRAGONR.MP2", "_DRAGONR.MP2"},
    {"DRAGONW.MP2", "_DRAGONW.MP2"},
    {"ENROTH.MP2", "_ENROTH.MP2"},
    {"FORSAKEN.MP2", "_FORSAKE.MP2"},
    {"GOODVS.MP2", "_GOODVS.MP2"},
    {"HEROES.MP2", "_HEROES.MP2"},
    {"HOTSPOT.MP2", "_HOTSPOT.MP2"},
    {"LOSTCON.MP2", "_LOSTCON.MP2"},
    {"LOSTRELI.MP2", "_LOSTREL.MP2"},
    {"MIGHTV.MP2", "_MIGHTV.MP2"},
    {"MINERALW.MP2", "_MINERAL.MP2"},
    {"MOUNT.MP2", "_MOUNT.MP2"},
    {"OVERLORD.MP2", "_OVERLOR.MP2"},
    {"PANDAMON.MP2", "_PANDAMO.MP2"},
    {"PYRAMID.MP2", "_PYRAMID.MP2"},
    {"REVOLU.MP2", "_REVOLU.MP2"},
    {"RIVER.MP2", "_RIVER.MP2"},
    {"SCORCH.MP2", "_SCORCH.MP2"},
    {"SEVENL.MP2", "_SEVENL.MP2"},
    {"SHIPW.MP2", "_SHIPW.MP2"},
    {"SLUGFEST.MP2", "_SLUGFES.MP2"},
    {"SPELLC.MP2", "_SPELLC.MP2"},
    {"TELEPORT.MP2", "_TELEPOR.MP2"},
    {"TERRAF.MP2", "_TERRAF.MP2"},
    {"THECLEAR.MP2", "_THECLEA.MP2"},
    {"THEOTHER.MP2", "_THEOTHE.MP2"},
    {"UNDEADA.MP2", "_UNDEADA.MP2"},
    {"UNHOLY.MP2", "_UNHOLY.MP2"},
    {"VIKINGS.MP2", "_VIKINGS.MP2"},
    {"WARIORK.MP2", "_WARIORK.MP2"},
    {"WASTL.MP2", "_WASTL.MP2"},
    {"WHOAM.MP2", "_WHOAM.MP2"},
    {"WINTERL.MP2", "_WINTERL.MP2"}
};
DATA(0x004a49d4) b32 gbComputeExtent = false;
DATA(0x004a49d8) b32 gbCurrArmyDrawn = false;
DATA(0x004a49dc) i32 gUnusedData4a49dc = 0;
DATA(0x004a49e0) b32 gbLimitToExtent = false;
DATA(0x004a49e4) b32 gbLoadingMonoIcon = false;
DATA(0x004a49e8) b32 gbSaveBiggestExtent = false;
DATA(0x004a49ec) i32 gUnusedData4a49ec = 0;
DATA(0x004a49f0) i32 giScrollX = 0;
DATA(0x004a49f4) i32 giScrollY = 0;
// The object tool's selected object class.
DATA(0x004a49f8) i32 gObjectClass = 0;
DATA(0x004a49fc) i32 gUnusedData4a49fc = 0;
DATA(0x004a4a00) b32 gStatusTextShown = false;
DATA(0x004a4a04) b32 gbInDialog = false;
DATA(0x004a4a08) b32 gbMinimized = false;
DATA(0x004a4a0c) b32 gbInSetupDialog = false;
// The random map generator draws the map only when it is done
// (gGenerateUnseen); gGeneratingMap holds the map view while it works.
DATA(0x004a4a10) b32 gGenerateUnseen = false;
DATA(0x004a4a14) b32 gGeneratingMap = false;
DATA(0x004a4a18) i32 gUnusedData4a4a18 = 0;
DATA(0x004a4a1c) b32 gbInSmackMgr = false;
DATA(0x004a4a20) i32 gStatusTextClearTime = 0;
DATA(0x004a4a24) HMENU hmnuDflt = NULL;
DATA(0x004a4a28) HMENU hmnuCmbt = NULL;
DATA(0x004a4a2c) HMENU hmnuAdv = NULL;
DATA(0x004a4a30) HMENU hmnuTown = NULL;
DATA(0x004a4a34) i32 gUnusedData4a4a34 = 0;
DATA(0x004a4a38) b32 gbFirstTimeThrough = false;
DATA(0x004a4a3c) b32 gbInPollSound = false;
DATA(0x004a4a40) b32 bInShutDown = false;
DATA(0x004a4a44) i32 bEarlySetupDone = 0;
DATA(0x004a4a48) b32 gbInMemError = false;

// Uninitialized storage: VC6 orders it by name hash, not by definition.
DATA(0x004a3ac8) i32 giDebugLevel;
// KB's override driver names, kept by the editor's copy and never read.
DATA(0x004a3acc) char cOverrideMIDIDriver[GLOBAL_DRIVER_NAME_SIZE];
DATA(0x004a3adc) u8 bSaveMusicPosition[KB_MUSIC_TRACK_COUNT];
DATA(0x004a3b18) u16 gTimeEventExtras[EDITOR_TIME_EVENT_CAPACITY];
DATA(0x004a3b7c) class mouseManager* gpMouseManager;
DATA(0x004a3b80) char gText[GLOBAL_TEXT_BUFFER_SIZE];
// Unread storage; the name is the compiled spelling that keeps the name-hash order.
DATA(0x004a3e80) i32 gUnusedData4a3e80Cache[3];
DATA(0x004a3e8c) char* EXPANSION_AGGREGATE_NAME;
DATA(0x004a3e90) char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x004a3ff0) char* DEFAULT_AGGREGATE_NAME;
DATA(0x004a3ff8) fullMap gMaps[EDIT_MAP_COPIES];
DATA(0x004a4020) i32 gSelectionWidth;
DATA(0x004a4024) b32 gbTextEntryEscaped;
DATA(0x004a4028) class heroWindowManager* gpWindowManager;
DATA(0x004a402c) editManager* gEditManager;
DATA(0x004a4030) char gMapFileName[EDITOR_MAP_FILE_NAME_SIZE];
DATA(0x004a4040) char gStatusText[EDITOR_STATUS_TEXT_SIZE];
DATA(0x004a4108) i32 gSelectionY;
DATA(0x004a410c) resourceManager* gpResourceManager;
DATA(0x004a4110) u16 gRumourExtras[EDITOR_RUMOUR_CAPACITY];
DATA(0x004a414c) heroWindow* pNormalDialogWindow;
DATA(0x004a4150) u8 bMusicIsLooping[KB_MUSIC_TRACK_COUNT];
DATA(0x004a418c) heroWindow* gEditDialog;
DATA(0x004a4190) i32 gSelectionHeight;
DATA(0x004a4194) soundManager* gpSoundManager;
DATA(0x004a4198) char gLastFilename[GLOBAL_LAST_FILENAME_SIZE];
DATA(0x004a42f8) i32 gStatusTextHoldTime;
// Unread storage; the name is the compiled spelling that keeps the name-hash order.
DATA(0x004a42fc) i32 gUnusedData4a42fcBlock[2];
DATA(0x004a4304) mapCell* gEditCell;
DATA(0x004a4308) class palette* gpBufferPalette;
DATA(0x004a430c) palette* gPalette;
DATA(0x004a4310) char cAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x004a4470) char gcRegAppPath[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x004a45d0) i32 giMaxExtentX;
DATA(0x004a45d4) i32 giMaxExtentY;
// Unread storage; the name is the compiled spelling that keeps the name-hash order.
DATA(0x004a45d8) i32 gUnusedData4a45d8Instance[2];
DATA(0x004a45e0) inputManager* gpInputManager;
DATA(0x004a45e4) char cOverrideDigitalDriver[GLOBAL_DRIVER_NAME_SIZE];
// The placement link the next placed object's parts share (mapCell).
DATA(0x004a45f4) i32 gNextObjectLink;
DATA(0x004a45f8) char gcCommandLine[GLOBAL_COMMAND_LINE_SIZE];
DATA(0x004a4638) configStruct gConfig;
// Unread storage; the name is the compiled spelling that keeps the name-hash order.
DATA(0x004a47d8) i32 gUnusedData4a47d8StateBlock;
DATA(0x004a47dc) i32 giMinExtentX;
DATA(0x004a47e0) i32 giMinExtentY;
DATA(0x004a47e4) executive* gpExec;
DATA(0x004a47e8) i32 gLandCellCount;
DATA(0x004a47ec) i32 giCurWindowsStyleFlags;
DATA(0x004a47f0) char gcRegCDRomPath[GLOBAL_AGGREGATE_PATH_SIZE];
DATA(0x004a4950) i32 glTimers[GLOBAL_TIMER_COUNT];
// Unread storage; the name is the compiled spelling that keeps the name-hash order.
DATA(0x004a4978) i32 gUnusedData4a4978Runtime[5];

VA(0x004101b6, 0x9b)
extern "C" void PollSound(void) {
    if (gbInPollSound)
        return;
    gbInPollSound = true;
    if (glTimers[GLOBAL_MOUSE_TIMER_SLOT] < KBTickCount() && !gbPutzingWithMouseCtr) {
        glTimers[GLOBAL_MOUSE_TIMER_SLOT] = KBTickCount() + EDITOR_MOUSE_UPDATE_INTERVAL;
        gpMouseManager->NewUpdate(0);
    }
    if (glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] < KBTickCount()) {
        glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] = KBTickCount() + EDITOR_COLOR_CYCLE_INTERVAL;
        if (giGraphicsType == WINGRAPH_GRAPHICS_WING
            && giMainVideoModeColorDepth != WINGRAPH_COLOR_DEPTH)
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] += EDITOR_NON_PALETTED_CYCLE_DELAY;
        CycleColors(0);
    }
    gbInPollSound = false;
}

// An edited copy of a map shipped with the game gets an underscored file name
// and map name, unless /JVC allows overwriting the shipped maps.
VA(0x00410251, 0x87)
void ProtectShippedMap(void) {
    i32 i;

    if (gbShowAllMaps)
        return;
    for (i = 0; i < EDITOR_SHIPPED_MAP_COUNT; i++) {
        if (!stricmp(gMapFileName, gShippedMaps[i][0])) {
            strcpy(gMapFileName, gShippedMaps[i][1]);
            memmove(gEditMapHeader.name + 1, gEditMapHeader.name, sizeof(gEditMapHeader.name) - 1);
            gEditMapHeader.name[sizeof(gEditMapHeader.name) - 1] = 0;
            gEditMapHeader.name[0] = '_';
        }
    }
}

VA(0x004102d8, 0x368)
i32 oldmain(void) {
    heroWindow* window;
    i32 result;
    b32 keepRunning;
    char loadName[EDITOR_MAP_FILE_NAME_SIZE];

    if (gpExec->InitSystem())
        ShutDown(localization::Tr("editor.startup.initialize_failed"));
    KBChangeMenu(hmnuDflt);
    smallFont = gpResourceManager->GetFont("smalfont.fnt");
    bigFont = gpResourceManager->GetFont("BIGfont.fnt");
    gPalette = gpResourceManager->GetPalette("kb.pal");
    gpResourceManager->GetBackdrop("editor.icn", gpWindowManager->m_screen, true);
    gpWindowManager->UpdateScreen();
    gpWindowManager->FadeScreen(FADE_IN, FADE_SPEED_FINE, gPalette);
    gpMouseManager->SetPointer("editor.mse", EDIT_POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
    gpMouseManager->SetColorMice(gConfig.gfx[IDX(giCurExe)].colorMouseCursor);
    gpMouseManager->ShowColorPointer();
    window = NULL;
    result = -1;
    keepRunning = true;
    while (keepRunning) {
        gbInSetupDialog = true;
        window = new heroWindow(SETUP_WINDOW_X, SETUP_WINDOW_Y, "stpemain.bin");
        if (!window)
            MemError();
        gpWindowManager->DoDialog(window, SetupMainHandler, 0);
        delete window;
        result = gpWindowManager->m_dialogResult;
        gbInSetupDialog = false;
        switch (result) {
            case SETUP_CHOICE_TWO: // load a map
                if (PickMap(FILE_REQUESTER_MAP))
                    keepRunning = false;
                sprintf(loadName, gMapFileName);
                break;
            case SETUP_CHOICE_ONE: // a new map
                if (SetupNewMap())
                    keepRunning = false;
                break;
            case SETUP_CHOICE_QUIT:
            case DIALOG_BUTTON_1:
                gpWindowManager->FadeScreen(FADE_OUT, FADE_SPEED_FINE, gPalette);
                ShutDown(NULL);
                break;
        }
    }
    gpMouseManager->HideColorPointer();
    gpWindowManager->FadeScreen(FADE_OUT, FADE_SPEED_STANDARD, gPalette);
    memset(
        gpWindowManager->m_screen->m_pixels,
        SCREEN_FILL_COLOR,
        LOGICAL_SCREEN_WIDTH * LOGICAL_SCREEN_HEIGHT
    );
    if (gpExec->AddManager(gEditManager, BASE_MANAGER_PRIORITY_UNASSIGNED))
        ShutDown(localization::Tr("system.manager.add_failed"));
    if (result == SETUP_CHOICE_TWO) { // a map to load
        strcpy(gMapFileName, loadName);
        gEditManager->LoadMap(gMapFileName);
        ProtectShippedMap();
    }
    gEditManager->DrawRadar(true);
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
    gpWindowManager->FadeScreen(FADE_IN, FADE_SPEED_STANDARD, gPalette);
    gpWindowManager->m_updateFlags = gConfig.editorPaletteCycling;
    gpExec->MainLoop();
    gpExec->RemoveManager(gEditManager);
    gpWindowManager->FadeScreen(FADE_OUT, FADE_SPEED_FINE, gPalette);
    gpResourceManager->Dispose(gPalette);
    ShutDown(NULL);
    return 0;
}

VA(0x00410640, 0x14)
void IncrementArgumentA(i32 value) {
    value++;
}

VA(0x00410654, 0x5)
void EditorIdleHook(void) {}

VA(0x00410659, 0x14)
void IncrementArgumentB(i32 value) {
    value++;
}

VA(0x0041066d, 0x30)
void DelayTicks(i32 ticks) {
    i32 H2_UNUSED(unused) = 0;

    glTimers[EDITOR_DELAY_TIMER_SLOT] = KBTickCount() + ticks * EDITOR_DELAY_TICK_MILLISECONDS;
    DelayTil(glTimers + EDITOR_DELAY_TIMER_SLOT);
}

VA(0x0041069d, 0x23)
void DelayTil(i32* endTime) {
    while (*endTime > KBTickCount()) {
        Process1WindowsMessage();
        PollSound();
    }
}

VA(0x004106c0, 0x1a)
void DelayMilli(i32l delay) {
    DelayTilMilli(KBTickCount() + delay);
}

VA(0x004106da, 0x21)
void DelayTilMilli(i32l endTime) {
    while (endTime > KBTickCount()) {
        Process1WindowsMessage();
        PollSound();
    }
}

VA(0x004106fb, 0x39)
void FileError(H2_CONST char* filename) {
    char message[EDITOR_FILE_ERROR_TEXT_SIZE];

    sprintf(message, localization::Tr("system.file.open_error"), filename);
    ShutDown(message);
}

VA(0x00410734, 0x180)
void ShutDown(H2_CONST char* message) {
    char buffer[EDITOR_SHUTDOWN_TEXT_SIZE];

    if (bInShutDown)
        return;
    bInShutDown = true;
    gbClosingApp = true;
    buffer[0] = 0;
    gpMouseManager->SetColorMice(false);
    if (message) {
        strcpy(buffer, message);
        SetFullScreenStatus(false);
        LogStr(buffer);
        MessageBoxA(hwndApp, buffer, localization::Tr("system.unexpected_termination"), MB_ICONHAND);
    } else {
        sprintf(buffer, localization::Tr("system.goodbye"));
    }
    gbClosingApp = true;
    if (gLineMap)
        delete gLineMap;
    gLineMap = NULL;
    if (bigFont) {
        gpResourceManager->Dispose(bigFont);
        bigFont = NULL;
    }
    if (smallFont) {
        gpResourceManager->Dispose(smallFont);
        smallFont = NULL;
    }
    gpExec->ShutDownSystem();
    if (gEventHandle) {
        CloseHandle(gEventHandle);
        gEventHandle = NULL;
    }
    DeleteMainClasses();
    gMap.Close();
    gUndoMap.Close();
    AppExit();
    PrintMemoryLeaks();
    exit(0);
}

// The /P (debug level) and /JVC (overwrite the shipped maps) switches.
VA(0x004108b4, 0xf7)
i32 InterpretCommandLine(void) {
    i32 size;
    i32 i;

    giDebugLevel = 0;
    size = strlen(gcCommandLine);
    for (i = 0; i < size; i++) {
        if (gcCommandLine[i] == '/' && i + 1 < size) {
            // The C library's toupper: KB.cpp's char overload is the game's.
            switch (toupper(static_cast<i32>(gcCommandLine[i + 1]))) {
                case 'J':
                    if (i + 3 < size && toupper(static_cast<i32>(gcCommandLine[i + 2])) == 'V'
                        && toupper(static_cast<i32>(gcCommandLine[i + 3])) == 'C')
                        gbShowAllMaps = true;
                    break;
                case 'P':
                    if (i + 2 < size)
                        giDebugLevel = gcCommandLine[i + 2] - '0';
                    break;
            }
        }
    }
    return 1;
}

VA(0x004109ab, 0x2e)
void EarlyShutdown(H2_CONST char* caption, H2_CONST char* text) {
    MessageBoxA(hwndApp, text, caption, MB_ICONHAND);
    exit(0);
}

VA(0x004109d9, 0x149)
i32 EarlySetup(void) {
    CDRomSetupResult result;
    i32 i;

    if (bEarlySetupDone)
        return 0;
    sprintf(cAggPathName, "%s%s", ".\\DATA\\", "heroes2.agg");
    DEFAULT_AGGREGATE_NAME = cAggPathName;
    sprintf(cExpAggPathName, "%s%s", ".\\DATA\\", "heroes2x.agg");
    EXPANSION_AGGREGATE_NAME = cExpAggPathName;
    InitMainClasses();
    GetGraphicsInfo();
    ReadPrefs();
    if (!InterpretCommandLine())
        return 1;
    LogTruncate();
    result = SetupCDDrive();
    if (result == CD_ROM_DRIVE_UNAVAILABLE) {
        EarlyShutdown(
            localization::Tr("system.startup_error.title"),
            localization::Tr("editor.startup.no_cd_drive")
        );
        exit(0);
    }
    if (result == CD_ROM_EXPANSION_DISC_MISSING) {
        EarlyShutdown(
            localization::Tr("system.startup_error.title"),
            localization::Tr("editor.startup.cd_required")
        );
        exit(0);
    }
    if (result == CD_ROM_GAME_DIRECTORY_MISSING) {
        EarlyShutdown(
            localization::Tr("system.startup_error.title"),
            localization::Tr("system.startup_error.game_directory_missing")
        );
        exit(0);
    }
    if (result == CD_ROM_DATA_FILES_MISSING) {
        EarlyShutdown(
            localization::Tr("system.startup_error.title"),
            localization::Tr("system.startup_error.data_files_missing")
        );
        exit(0);
    }
    for (i = 0; i < GLOBAL_TIMER_COUNT; i++)
        glTimers[i] = 0;
    hmnuDflt = LoadMenuA(hInstApp, "mnuDflt");
    return 1;
}

VA(0x00410b22, 0x24)
void MemError(void) {
    if (gbInMemError)
        return;
    gbInMemError = true;
    ShutDown(localization::Tr("system.memory.out_of_memory"));
}

VA(0x00410b46, 0x252)
void InitMainClasses(void) {
    gpExec = new executive;
    gpInputManager = new inputManager;
    gpMouseManager = new mouseManager;
    gpWindowManager = new heroWindowManager;
    gpResourceManager = new resourceManager;
    gpSoundManager = new soundManager;
    gpBufferPalette = new palette;
    gEditManager = new editManager;
}

VA(0x00410d98, 0x18e)
void DeleteMainClasses(void) {
    if (gEditManager)
        delete gEditManager;
    gEditManager = NULL;
    if (gpBufferPalette)
        delete gpBufferPalette;
    gpBufferPalette = NULL;
    if (gpSoundManager)
        delete gpSoundManager;
    gpSoundManager = NULL;
    if (gpWindowManager)
        delete gpWindowManager;
    gpWindowManager = NULL;
    if (gpMouseManager)
        delete gpMouseManager;
    gpMouseManager = NULL;
    if (gpInputManager)
        delete gpInputManager;
    gpInputManager = NULL;
    if (gpExec)
        delete gpExec;
    gpExec = NULL;
    if (gpResourceManager)
        delete gpResourceManager;
    gpResourceManager = NULL;
}

// The editor's message boxes carry text only and are narrower than the
// game's. The game's dialog locals the editor dropped the code for keep their
// frame slots.
#define iconFile iconFile_a                               // frame-slot spelling
#define iconHeight iconHeight_h                           // frame-slot spelling
#define message message_b                                 // frame-slot spelling
#define panelHeight panelHeight_d                         // frame-slot spelling
#define resourceFrame resourceFrame_n                     // frame-slot spelling
#define resourceY resourceY_f                             // frame-slot spelling
#define savedFirstResourceType savedFirstResourceType_k   // frame-slot spelling
#define savedSecondResourceType savedSecondResourceType_m // frame-slot spelling
#define showMessage showMessage_d                         // frame-slot spelling
#define windowHeight windowHeight_h                       // frame-slot spelling
#define windowRows windowRows_b                           // frame-slot spelling
#define windowWidth windowWidth_f                         // frame-slot spelling
VA(0x00410f26, 0x2f4)
void NormalDialog(
    H2_CONST char* text,
    i32 dialogType,
    i32 windowX,
    i32 windowY,
    i32 H2_UNUSED(firstResourceType),
    i32 H2_UNUSED(firstResourceValue),
    i32 H2_UNUSED(secondResourceType),
    i32 H2_UNUSED(secondResourceValue),
    i32 H2_UNUSED(showOrText),
    i32 H2_UNUSED(timeout)
) {
    i32 H2_UNUSED(resourceFrame);
    i16 H2_UNUSED(showMessage);
    i32 H2_UNUSED(textWidgetId);
    i32 windowHeight;
    b32 H2_UNUSED(showPrimaryBonus);
    tag_message message;
    i32 H2_UNUSED(resourceSlot);
    i32 H2_UNUSED(resourceY);
    i32 H2_UNUSED(iconHeight);
    i32 lineCount;
    i32 dialogContentHeight;
    i32 H2_UNUSED(savedFirstResourceType);
    i32 H2_UNUSED(maxIconHeight);
    i32 H2_UNUSED(savedSecondResourceType);
    i32 windowRows;
    char iconFile[NORMAL_DIALOG_FILENAME_LENGTH];
    i32 windowWidth;
    i32 H2_UNUSED(panelHeight);

    textWidgetId = NORMAL_DIALOG_TEXT_WIDGET_FIRST_ID;
    showMessage = 1;
    lineCount = bigFont->LineLength(text, EDITOR_DIALOG_TEXT_LINE_WIDTH);
    dialogContentHeight = lineCount * NORMAL_DIALOG_TEXT_LINE_HEIGHT;
    if (dialogType != NORMAL_DIALOG_QUICK_VIEW)
        dialogContentHeight += EDITOR_DIALOG_BUTTON_AREA;
    windowRows = (dialogContentHeight - EDITOR_DIALOG_ROW_OFFSET) / NORMAL_DIALOG_WINDOW_ROW_HEIGHT;
    if (windowRows > NORMAL_DIALOG_MAX_ROWS)
        windowRows = NORMAL_DIALOG_MAX_ROWS;
    if (windowRows <= 0 && dialogType != NORMAL_DIALOG_QUICK_VIEW)
        windowRows = 1;
    windowWidth = EDITOR_DIALOG_WINDOW_WIDTH;
    windowHeight = windowRows * NORMAL_DIALOG_WINDOW_ROW_HEIGHT + EDITOR_DIALOG_WINDOW_BASE;
    if (windowX == -1 || windowWidth + windowX >= LOGICAL_SCREEN_MAX_X)
        windowX = EDITOR_DIALOG_DEFAULT_X;
    if (windowY == -1 || windowHeight + windowY >= LOGICAL_SCREEN_MAX_Y) {
        windowY = (LOGICAL_SCREEN_HEIGHT - windowHeight) / 2;
        if (windowY > NORMAL_DIALOG_MAX_TOP)
            windowY = NORMAL_DIALOG_MAX_TOP;
    }
    sprintf(iconFile, "evntwin%d.bin", windowRows);
    pNormalDialogWindow = new heroWindow(windowX, windowY, iconFile);
    if (!pNormalDialogWindow)
        MemError();

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED) | IDX(WIDGET_FLAG_DRAW);
    message.payload.widget.id = DIALOG_BUTTON_7;
    pNormalDialogWindow->BroadcastMessage(message);
    message.payload.widget.id = DIALOG_BUTTON_8;
    pNormalDialogWindow->BroadcastMessage(message);
    if (dialogType != NORMAL_DIALOG_WAIT_LAST && dialogType != NORMAL_DIALOG_BUTTON_PAIR) {
        message.payload.widget.id = DIALOG_BUTTON_1;
        pNormalDialogWindow->BroadcastMessage(message);
    }
    if (dialogType != NORMAL_DIALOG_WAIT_FIRST && dialogType != NORMAL_DIALOG_INFO
        && dialogType != NORMAL_DIALOG_BUTTON_PAIR) {
        message.payload.widget.id = DIALOG_BUTTON_2;
        pNormalDialogWindow->BroadcastMessage(message);
    }
    if (dialogType != NORMAL_DIALOG_CONFIRM) {
        message.payload.widget.id = NORMAL_DIALOG_YES;
        pNormalDialogWindow->BroadcastMessage(message);
        message.payload.widget.id = NORMAL_DIALOG_NO;
        pNormalDialogWindow->BroadcastMessage(message);
    }

    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, NORMAL_DIALOG_TEXT_WIDGET_ID);
    message.payload.widget.data.text = text;
    pNormalDialogWindow->BroadcastMessage(message);
    if (dialogType == NORMAL_DIALOG_QUICK_VIEW) {
        gpWindowManager->AddWindow(pNormalDialogWindow, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(pNormalDialogWindow);
    } else {
        gpWindowManager->DoDialog(pNormalDialogWindow, EventWindowHandler, 0);
    }
    delete pNormalDialogWindow;
}
#undef iconFile
#undef iconHeight
#undef message
#undef panelHeight
#undef resourceFrame
#undef resourceY
#undef savedFirstResourceType
#undef savedSecondResourceType
#undef showMessage
#undef windowHeight
#undef windowRows
#undef windowWidth

VA(0x0041121a, 0x94)
MessageDispatchResult EventWindowHandler(struct tag_message& message) {
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_DESELECT:
                switch (message.payload.widget.id) {
                    case DIALOG_BUTTON_0:
                    case DIALOG_BUTTON_1:
                    case DIALOG_BUTTON_2:
                    case DIALOG_BUTTON_3:
                    case DIALOG_BUTTON_5:
                    case DIALOG_BUTTON_6:
                        FINISH_DIALOG_MESSAGE(message);
                        return MESSAGE_DISPATCH_FORWARD;
                }
                break;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004112ae, 0x80)
void QuickViewWait(void) {
    tag_message event;
    i32 done;

    gpMouseManager->ReallyHidePointer();
    done = 0;
    while (!done) {
        PollSound();
        Process1WindowsMessage();
        event = gpInputManager->GetEvent();
        done = event.type == MESSAGE_RIGHT_BUTTON_UP || event.type == MESSAGE_LEFT_BUTTON_DOWN
               || event.type == MESSAGE_LEFT_BUTTON_UP;
    }
    gpMouseManager->ReallyShowPointer();
}

// Shows `text` (or, while a text is shown, the current one) in the status
// line under the map view.
VA(0x0041132e, 0xa5)
void ShowStatusText(H2_CONST char* text) {
    if (gStatusTextShown && !text)
        text = gStatusText;
    else
        strcpy(gStatusText, text);
    gStatusTextShown = true;
    gStatusTextHoldTime = KBTickCount() + EDITOR_STATUS_TEXT_HOLD_MILLISECONDS;
    FillBitmapArea(
        gpWindowManager->m_screen,
        EDITOR_STATUS_BAR_X,
        EDITOR_STATUS_BAR_Y,
        EDITOR_STATUS_BAR_WIDTH,
        EDITOR_STATUS_BAR_HEIGHT,
        0
    );
    smallFont->DrawBoundedString(
        text,
        EDITOR_STATUS_BAR_X,
        EDITOR_STATUS_TEXT_Y,
        EDITOR_STATUS_BAR_WIDTH,
        EDITOR_STATUS_BAR_HEIGHT,
        FONT_DRAW_DEFAULT,
        FONT_ALIGN_CENTER
    );
    gpWindowManager->UpdateScreenRegion(
        EDITOR_STATUS_BAR_X,
        EDITOR_STATUS_BAR_Y,
        EDITOR_STATUS_BAR_WIDTH,
        EDITOR_STATUS_BAR_HEIGHT
    );
}

VA(0x004113d3, 0x4d)
void ClearStatusText(void) {
    gStatusTextClearTime = EDITOR_STATUS_TEXT_KEPT;
    if (gStatusTextShown) {
        gStatusTextShown = false;
        gEditManager->m_window->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
        gpWindowManager->UpdateScreenRegion(
            EDITOR_STATUS_BAR_X,
            EDITOR_STATUS_BAR_Y,
            EDITOR_STATUS_BAR_WIDTH,
            EDITOR_STATUS_BAR_HEIGHT
        );
    }
}

VA(0x00411420, 0xb)
void UpdateAppSpecificMenus(void* H2_UNUSED(hMenu)) {}

VA(0x0041142b, 0x3c)
void CleanUpMenus(void) {
    if (hmnuApp) {
        SetMenu(hwndApp, NULL);
        if (hmnuDflt)
            DestroyMenu(hmnuDflt);
    }
    hmnuApp = NULL;
}

VA(0x00411467, 0x1b)
void EarlyShutDownSystem(void) {
    if (gEditManager)
        gEditManager->SelectTool(EDIT_TOOL_NONE);
}

VA(0x00411482, 0xa)
i32 GameUnsaved(void) {
    return 1;
}

VA(0x0041148c, 0xc2)
i32 HandleAppSpecificMenuCommands(i32 command) {
    switch (command) {
        case EDITOR_MENU_PALETTE_CYCLING:
            gConfig.editorPaletteCycling = 1 - gConfig.editorPaletteCycling;
            gpWindowManager->m_updateFlags = gConfig.editorPaletteCycling;
            WritePrefs();
            break;
        case EDITOR_MENU_SCREEN_ANIMATION:
            gConfig.editorScreenAnimation = 1 - gConfig.editorScreenAnimation;
            WritePrefs();
            break;
        case EDITOR_MENU_OBJECT_BOXES:
            gConfig.showObjectBoxes = 1 - gConfig.showObjectBoxes;
            WritePrefs();
            break;
        case APP_MENU_EXIT:
            PostMessageA(hwndApp, WM_CLOSE, 0, 0);
            break;
        default:
            return 1;
    }
    return 0;
}

VA(0x0041154e, 0x12)
void EarlyResizeWindow(i32 H2_UNUSED(x), i32 H2_UNUSED(y), i32 H2_UNUSED(width),
                       i32 H2_UNUSED(height)) {}

VA(0x00411560, 0x5)
void UpdateSystemOptionsMenu(void) {}

#define matchedWidgets a // frame-slot spelling
#define message msg      // frame-slot spelling
VA(0x00411565, 0x88)
void SetWinText(heroWindow* window, i32 id) {
    i32 H2_UNUSED(matchedWidgets) = 0;
    i32 i;
    tag_message message;

    for (i = 0; i < EDITOR_DIALOG_WIN_SETUP_COUNT; i++) {
        if (gWinSetup[i].windowId == id) {
            matchedWidgets++;
            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, gWinSetup[i].widgetId);
            message.payload.widget.data.text = gWinSetup[i].text;
            window->BroadcastMessage(message);
        }
    }
}
#undef matchedWidgets
#undef message
