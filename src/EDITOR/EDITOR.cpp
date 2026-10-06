

#include <Ints.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/clearManager.h>
#include <EDITOR/editManager.h>
#include <EDITOR/setup.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/NOOPT.h>
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
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum EditorStartupConstant {


    EDITOR_SETUP_WINDOW_X       = 0x195,
    EDITOR_SETUP_WINDOW_Y       = 8,
    EDITOR_SETUP_NEW_MAP        = 1,
    EDITOR_SETUP_LOAD_MAP       = 2,
    EDITOR_SETUP_QUIT           = 0x69,
    EDITOR_SETUP_PICK_LOAD_MODE = 4,
    EDITOR_FADE_STEPS           = 6,
    EDITOR_SLOW_FADE_STEPS      = 8,

    EDITOR_BACKGROUND_COLOR     = 0x24,
    EDITOR_SCREEN_BYTES         = 640 * 480,

    EDITOR_CD_NO_DRIVE          = 1,
    EDITOR_CD_NOT_FOUND         = 2,
    EDITOR_CD_NO_APP_PATH       = 3,
    EDITOR_CD_NO_DATA           = 4,

    EDITOR_DELAY_TICK_MILLISECONDS = 15,
    EDITOR_DELAY_TIMER_SLOT     = 1,
    EDITOR_MOUSE_UPDATE_INTERVAL = 13,
    EDITOR_COLOR_CYCLE_INTERVAL = 200,
    EDITOR_NON_PALETTED_CYCLE_DELAY = 300,
    EDITOR_PALETTED_COLOR_DEPTH = 8,

    EDITOR_SHUTDOWN_TEXT_SIZE   = 768,
    EDITOR_FILE_ERROR_TEXT_SIZE = 200
} EditorStartupConstant;

typedef enum EditorMenuCommand {

    EDITOR_MENU_PALETTE_CYCLING = 0x9cd1,
    EDITOR_MENU_SCREEN_ANIMATION = 0x9cd2,
    EDITOR_MENU_OBJECT_BOXES    = 0x9cd3
} EditorMenuCommand;

typedef enum EditorNormalDialogConstant {


    EDITOR_DIALOG_TEXT_LINE_WIDTH  = 0xf0,
    EDITOR_DIALOG_BUTTON_AREA      = 0x27,
    EDITOR_DIALOG_ROW_OFFSET       = 12,
    EDITOR_DIALOG_WINDOW_WIDTH     = 0x11e,
    EDITOR_DIALOG_WINDOW_BASE      = 0x81,
    EDITOR_DIALOG_SCREEN_MAX_X     = 0x27f,
    EDITOR_DIALOG_SCREEN_MAX_Y     = 0x1df,
    EDITOR_DIALOG_DEFAULT_X        = 0x9f,
} EditorNormalDialogConstant;


#define GROUND_REPEAT_2(value) value, value
#define GROUND_REPEAT_4(value) GROUND_REPEAT_2(value), GROUND_REPEAT_2(value)
#define GROUND_REPEAT_8(value) GROUND_REPEAT_4(value), GROUND_REPEAT_4(value)
#define GROUND_REPEAT_16(value) GROUND_REPEAT_8(value), GROUND_REPEAT_8(value)
#define GROUND_REPEAT_32(value) GROUND_REPEAT_16(value), GROUND_REPEAT_16(value)
#define GROUND_SHAPE_STANDARD_FRAME_SET                                                            \
    GROUND_REPEAT_4(1), GROUND_REPEAT_4(2), GROUND_REPEAT_4(3), GROUND_REPEAT_4(4),                \
        GROUND_REPEAT_4(5), GROUND_REPEAT_4(6), GROUND_REPEAT_4(7), GROUND_REPEAT_4(8), 10, 11,    \
        12, 13, 14, 15, GROUND_REPEAT_8(0)

u8
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
u8 giGroundShape[GROUND_TILE_IMAGE_COUNT] = {
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
    GROUND_REPEAT_16(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_16(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_REPEAT_4(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_FLIPPED,
    GROUND_REPEAT_4(5),
    GROUND_REPEAT_4(6),
    GROUND_REPEAT_4(7),
    GROUND_REPEAT_4(8),
    GROUND_REPEAT_8(0),
    GROUND_REPEAT_16(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_STANDARD_FRAME_SET,
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_REPEAT_8(0),
    GROUND_REPEAT_8(GROUND_SHAPE_FLIPPED),
    GROUND_SHAPE_FLIPPED
};

#undef GROUND_SHAPE_STANDARD_FRAME_SET
#undef GROUND_REPEAT_32
#undef GROUND_REPEAT_16
#undef GROUND_REPEAT_8
#undef GROUND_REPEAT_4
#undef GROUND_REPEAT_2

u8 gColorTableTan[PALETTE_COLOR_COUNT] = {
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
u8 gColorTableGray[PALETTE_COLOR_COUNT] = {
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
u8 gColorTableYellow[PALETTE_COLOR_COUNT] = {
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
u8 gColorTableScenWin[PALETTE_COLOR_COUNT] = {
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
u8 gColorTableDarkGray[PALETTE_COLOR_COUNT] = {
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
u8 gColorTableRed[PALETTE_COLOR_COUNT] = {
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
u8 gColorTableDarkBrown[PALETTE_COLOR_COUNT] = {
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
i32 MAP_WIDTH = MAP_DIMENSION_MEDIUM;
i32 MAP_HEIGHT = MAP_DIMENSION_MEDIUM;
b32 gbClosingApp = false;
b32 gbForegroundApp = false;
i32 giMainVideoModeColorDepth = WINGRAPH_COLOR_DEPTH;
i32 giMainVideoModeWidth = LOGICAL_SCREEN_WIDTH;
i32 giMainVideoModeHeight = LOGICAL_SCREEN_HEIGHT;
u8 gMapColors[RADAR_MAP_COLOR_COUNT] = {77, 98, 13, 104, 32, 118, 54, 206, 41, 0, 0, 0};
u8 gObjectColors[RADAR_OBJECT_COLOR_COUNT] =
    {16, 48, 98, 160, 126, 74, 110, 179, 100, 218, 12, 12, 12, 12, 12, 12};
u8 gOwnerColors[RADAR_OWNER_COLOR_COUNT] = {73, 105, 190, 114, 205, 138, 10, 0};
const char* gTilesetFiles[(TILESET_COUNT)] = {
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
u8 bPuzzleDraw[PUZZLE_DRAW_TABLE_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][PALETTE_COLOR_COUNT] = {
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
u8 gColorTableLighten[PALETTE_COLOR_COUNT] = {
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
u8 gColorTableNoCycle[PALETTE_COLOR_COUNT] = {
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
font* smallFont = NULL;
font* bigFont = NULL;
b32 gbReturnAfterComputeExtent = false;
b32 gbAllowTextEntryEscape = true;
WindowColorCycleMode giCycleType = WINDOW_COLOR_CYCLE_DEFAULT;
b32 giScreenScroll = true;
i32 giMenuCommand = -1;
b32 gbSendMouseMoveMessages = false;
b32 gbColorMice = true;
u32l gTownEligibleBuildMask[(FACTION_COUNT)] = {
    TOWN_ELIGIBLE_BUILD_KNIGHT_MASK,
    TOWN_ELIGIBLE_BUILD_BARBARIAN_MASK,
    TOWN_ELIGIBLE_BUILD_SORCERESS_MASK,
    TOWN_ELIGIBLE_BUILD_WARLOCK_MASK,
    TOWN_ELIGIBLE_BUILD_WIZARD_MASK,
    TOWN_ELIGIBLE_BUILD_NECROMANCER_MASK
};
u8 giMapSizes[KB_MAP_SIZE_COUNT] =
    {MAP_DIMENSION_SMALL, MAP_DIMENSION_MEDIUM, MAP_DIMENSION_LARGE, MAP_DIMENSION_XLARGE};
b32 gbUseEvilInterface = false;
const char* cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT] = {
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
char gcAnimPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\ANIM2\\";
char gcGamePath[GLOBAL_GAME_PATH_SIZE] = ".\\GAMES\\";
char gcMapPath[GLOBAL_MAP_PATH_SIZE] = ".\\MAPS\\";
char gcMusicPath[GLOBAL_AGGREGATE_PATH_SIZE] = "\\TRACKS2\\";
i32 gbPutzingWithMouseCtr = 0;
float gfCombatSpeedMod[KB_COMBAT_SPEED_COUNT] = {1.0f, 0.7f, 0.35f};
icon* gShingleAnim = NULL;
i32 iNextShingleAnim = 0;
i32 giDialogTimeout = 0;
i32 giNewMonsterCycleFrame = 0;
b32 gbNoCDRom = false;
b32 gbLeaveNetBoxAlone = false;
b32 gbDrawWindowBackground = true;
b32 gbCheatMenus = false;
b32 gbUseWaveout = false;
b32 gbShowAllMaps = false;


i16 gVesaMode[VESA_MODE_VALUE_COUNT] =
    {640, 480, 256, VESA_SET_MODE_FUNCTION, VESA_MODE_640_480_256, 0};
b32 bShowIt = true;
b32 gbEnlargeScreenBlit = true;

b32 gCommandLineInterpreted = true;
b32 gShowMapInfo = true;
ConfigExecutable giCurExe = CONFIG_EXECUTABLE_EDITOR;
i32 gClearFlags = EDITOR_CLEAR_FLAGS_DEFAULT;

i32 gSelectionX = EDIT_NO_CELL;

i32 gRandomMapPlayers = 4;
double gTerrainPercent[RANDOM_MAP_TERRAIN_COUNT] = {30.0, 30.0, 20.0, 0.0, 0.0, 0.0, 20.0, 0.0};
double gDensityPercent[RANDOM_MAP_DENSITY_COUNT] = {50.0, 50.0, 50.0, 50.0, 50.0};
b32 gScatterTowns = true;
struct SMenuEnableStatus gsMenuEnableStatus[MENU_ENABLE_STATUS_COUNT] = {
    {APP_MENU_NONE, 0, 0, 0},
    {(KBWIN_MENU_SIZE_640_480), 1, 1, 0},
    {(KBWIN_MENU_SIZE_800_600), 1, 1, 0},
    {(KBWIN_MENU_SIZE_1024_768), 1, 1, 0},
    {(KBWIN_MENU_SIZE_1280_1024), 1, 1, 0},
    {(KBWIN_MENU_FULLSCREEN), 1, 1, 0},
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
    {(KBWIN_MENU_HELP), 1, 1, 0},
    {(KBWIN_MENU_ABOUT), 1, 1, 0},
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


i32 gZoomScale[EDIT_ZOOM_COUNT] = {1, 2, 4};
i32 gZoomCellSize[EDIT_ZOOM_COUNT] = {32, 16, 8};
i32 gZoomViewCells[EDIT_ZOOM_COUNT] = {14, 28, 56};
i32 gZoomTileSize[EDIT_ZOOM_COUNT] = {32, 16, 8};


u8 gLineTiles[LINE_NEIGHBOUR_MASKS] = {
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
u8 gLineEdgeTiles[LINE_NEIGHBOUR_MASKS] = {
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
u8 gLineEndTiles[LINE_END_MASKS] = {
    3, 2, 3, 1, 2, 2, 0, 11, 3, 4, 3, 9, 7, 8, 10, 6
};
u8 gRoadTileJoins[LINE_ROAD_TILES] = {
    1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1
};
u8 gRoadTileJoinsAlt[LINE_ROAD_TILES] = {
    1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1
};

const char* gTerrainHelp[EDITOR_TERRAIN_HELP_COUNT] = {
    "",
    "{Вода}\n\nПутешествовать только на кораблях.",
    "{Трава}\n\nНет особых модификаторов.",
    "{Снег}\n\nДля всех героев время на передвижение тратится в 1.5 раза больше. (Навык Следопыта снижает или игнорирует штраф.)",
    "{Болото}\n\nДля всех героев время на передвижение тратится в 1.75 раза больше. (Навык Следопыта снижает или игнорирует штраф.)",
    "{Лава}\n\nНет особых можификаторов.",
    "{Пустыня}\n\nДля всех героев время на передвижение тратится в 2 раза больше. (Навык Следопыта снижает или игнорирует штраф.)",
    "{Грязь}\n\nНет особых модификаторов.",
    "{Пустошь}\n\nДля всех героев время на передвижение тратится в 1.25 раза больше. (Навык Следопыта снижает или игнорирует штраф.)",
    "{Побережье}\n\nДля всех героев время на передвижение тратится в 1.25 раза больше. (Навык Следопыта снижает или игнорирует штраф.)",
    "{Малая кисть}\n\nРисует почву в 1 клетке.",
    "{Средняя кисть}\n\nЗакрашивает область 2х2 клетки.",
    "{Большая кисть}\n\nЗакрашивает область 4х4 клетки.",
    "{Заливка}\n\nИспользуется для закраски больших площадей."
};

const char* gClearHelp[CLEAR_HELP_COUNT] = {
    "{Малая кисть}\n\nРисует почву в 1 клетке.",
    "{Средняя кисть}\n\nЗакрашивает область 2х2 клетки.",
    "{Большая кисть}\n\nЗакрашивает область 4х4 клетки.",
    "{Заливка}\n\nИспользуется для закраски больших площадей.",
    "",
    "{Объекты воды}\n\nИспользуется для выбора объектов, чаще всего встречающихся на воде.",
    "{Объекты травы}\n\nИспользуется для выбора объектов, чаще всего встречающихся на траве.",
    "{Снежные объекты}\n\nИспользуется для выбора объектов, чаще всего встречающихся на снегу.",
    "{Болотные объекты}\n\nИспользуется для выбора объектов, чаще всего встречающихся на болотах.",
    "{Объекты лавы}\n\nИспользуется для выбора объектов, чаще всего встречающихся на лаве.",
    "{Объекты пустыни}\n\nИспользуется для выбора объектов, чаще всего встречающихся в пустыне.",
    "{Объекты грязи}\n\nИспользуется для выбора объектов, чаще всего встречающихся на грязи.",
    "{Объекты пустоши}\n\nИспользуется для выбора объектов, чаще всего встречающихся в пустоши.",
    "{Объекты побережья}\n\nИспользуется для выбора объектов, чаще всего встречающихся на побережье.",
    "{Города}\n\nВыбрать для размещения замки или города.",
    "{Монстры}\n\nВыбрать отряды монстров для размещения.",
    "{Герои}\n\nИспользуется для размещения героев.",
    "{Артефакты}\n\nИспользуется для размещения артефактов.",
    "{Сокровища}\n\nИспользуется для размещения сокровищ и ресурсов.",
    ""
};

const char* gEditPanelHelp[EDIT_PANEL_HELP_COUNT] = {
    "",
    "{Миникарта}\n\nПозволяет менять точку обзора на карте.",
    "{Скроллинг}\n\nИспользуется для передвижения экрана по карте, вдоль осей экрана дисплея.",
    "{Почва}\n\nИспользуется для отрисовки травы, грязи, воды и т.п..",
    "{Объекты}\n\nИспользуется для размещения объектов (горы, деревья, сокровища и т.п.) на карте.",
    "{Информация}\n\nИспользуется для определения специальных параметров монстров, героев и городов.",
    "{Ластик}\n\nИспользуется для удаления объектов с карты.",
    "{Потоки}\n\nПозволяет разместить потоки воды и лавы.",
    "{Дороги}\n\nПозволяет проложить дороги.",
    "Скроллинг\n\nПозволяет перемещать экран по карте.",
    "{Лупа}\n\nУправляет режимом масштаба изображения.",
    "{Отменить}\n\nОтменить последнее действие.",
    "{Настройки}\n\nЗадать название карты, описание и другие параметры.",
    "{Новая}\n\nНачать с нуля новую карту.",
    "{Окно файлов}\n\nЗагрузить ранее сохраненную карту.",
    "{Системные настройки}\n\nОткрыть экран системных настроек редактора."
};

const char* gEditTerrainNames[EDITOR_TERRAIN_NAME_COUNT] = {
    "Вода",
    "Трава",
    "Снег",
    "Болото",
    "Лава",
    "Пустыня",
    "Грязь",
    "Пустошь",
    "Побережье"
};

const char* gObjectClassNames[EDITOR_OBJECT_CLASS_COUNT] = {
    "Водные об.",
    "Об. травы",
    "Снежные об.",
    "Болотные об.",
    "Об. лавы",
    "Пустынные об.",
    "Об. грязи",
    "Об. пустоши",
    "Об. побережья",
    "Города",
    "Монстры",
    "Герои",
    "Артефакты",
    "Сокровища",
    "Дорога",
    "Поток"
};

const char* gSetupNewMapHelp[SETUP_NEW_MAP_HELP_COUNT] = {
    "Начать карту с нуля.",
    "Создать случайную карту.",
    "Отменить и вернуться в главное меню."
};

const char* gSetupMapSizeHelp[SETUP_MAP_SIZE_HELP_COUNT] = {
    "Создать карту размером 36 на 36 клеток. (Для сравнения, в Героях 1 все карты были размером 72 x 72)",
    " Создать карту размером 72 на 72 клетки. (Для сравнения, в Героях 1 все карты были размером 72 x 72)",
    " Создать карту размером 108 на 108 клеток. (Для сравнения, в Героях 1 все карты были размером 72 x 72)",
    " Создать карту размером 144 на 144 клетки. (Для сравнения, в Героях 1 все карты были размером 72 x 72)",
    "Отменить и вернуться в главное меню."
};

const char* gSetupMainHelp[SETUP_MAIN_HELP_COUNT] = {
    "Создать новую карту с нуля используя генератор случайных карт.",
    "Загрузить существующую карту.",
    "Выйти из редактора карт."
};

const char* gEventFrequencyNames[EVENT_FREQUENCY_COUNT] = {
    "Никогда",
    "Каждый день",
    "Каждый 2-й день",
    "Каждый 3-й день",
    "Каждый 4-й день",
    "Каждый 5-й день",
    "Каждый 6-й день",
    "Каждый 7-й день",
    "Каждый 14-й день",
    "Каждый 21-й день",
    "Каждый 28-й день"
};

const char* gTownNames[EDITOR_TOWN_NAME_COUNT] = {
    "Блэкбридж",
    "Пайнхёрст",
    "Вудхавен",
    "Хилстон",
    "Вайтшилд",
    "Бладрейн",
    "Драгонтус",
    "Грейвинд",
    "Блэквинд",
    "Портсмис",
    "Мидлгейт",
    "Тундара",
    "Вулкания",
    "Занзобар",
    "Атлантиум",
    "Байвоч",
    "Вилдабар",
    "Фонтанхейд",
    "Вертиго",
    "Винтеркил",
    "Найтшэдоу",
    "Сэндкастер",
    "Лэйксайт",
    "Олимпус",
    "Бриндамур",
    "Бурлок",
    "Ксабран",
    "Драгадун",
    "Аламар",
    "Калиндра",
    "Блэкфанг",
    "Бесенджи",
    "Алгари",
    "Сорпигал",
    "Ньюдавн",
    "Эрликиум",
    "Авон",
    "Биг Оак",
    "Хампшир",
    "Чандлер",
    "Саусмил",
    "Видпас",
    "Рокхавен",
    "Авалон",
    "Антиох",
    "Браунстон",
    "Ведингтон",
    "Ватингем",
    "Вестфорк",
    "Хилтоп",
    "Йорксфорд",
    "Шерман",
    "Роскомон",
    "Элькхеад",
    "Кетчкарт",
    "Гнездо гадюк",
    "Пигай",
    "Блэксфорд",
    "Буртон",
    "Блэкбурн",
    "Ланкшир",
    "Ломбард",
    "Тимберхил",
    "Фентон",
    "Троя",
    "Фордер Оак",
    "Меремек",
    "Квиксильвер",
    "Вестмур",
    "Вилоу",
    "Шелтембург",
    "Коракстоун"
};

const char* gFileMenuHelp[EDIT_FILE_MENU_HELP_COUNT] = {
    "Начать новую карту.",
    "Загрузить ранее сохраненную карту.",
    "Сохранить текущую карту.",
    "Выйти из редактора карт.",
    "Закрыть меню, ничего не делая."
};

const char* gSystemOptionsHelp[EDIT_SYSTEM_OPTIONS_HELP_COUNT] = {
    "{ОК}\n\nЗакрыть это меню.",
    "{Анимация}\n\nВключить или выключить анимацию объектов.",
    "{Мигание}\n\nВключить или выключить мигание цветов.",
    "{Сетка}\n\nВключить или выключить сетку объектов.\n\nЗеленое = Накладываемо\nКрасное = Объект\nРозовое = Триггер объектов\nЧерное = Тень",
    "{Курсор}\n\nВключить или отключить цветной курсор."
};

const char* gVictoryConditionNames[SPEC_VICTORY_CONDITION_COUNT] = {
    "Нет",
    "Захватить замок",
    "Сразить героя",
    "Найти артефакт",
    "Одна сторона сразить другую",
    "Собрать золото"
};

const char* gLossConditionNames[SPEC_LOSS_CONDITION_COUNT] = {
    "Нет",
    "Потерять замок",
    "Потерять героя",
    "Не успеть"
};

SWinSetup gWinSetup[EDITOR_DIALOG_WIN_SETUP_COUNT] = {
    {16, 500, "Свойства Могущественного артефакта\n\n\n\nУкажите допустимый радиус расположения артефакта от этой локации"},
    {3, 100, "Анимация"},
    {3, 101, "По кругу"},
    {3, 103, "Сетка"},
    {3, 105, "Курсор мыши"},
    {4, 100, "Событие"},
    {4, 101, "Текст сообщения"},
    {4, 102, "Дать ресурсов (или забрать, если отрицательное число.)"},
    {4, 103, "Древесина"},
    {4, 104, "Ртуть"},
    {4, 105, "Руда"},
    {4, 106, "Сера"},
    {4, 107, "Кристаллы"},
    {4, 108, "Самоцветы"},
    {4, 109, "Золото"},
    {4, 400, "День первого появления"},
    {4, 420, "Повторять событие"},
    {4, 300, "Дать артефакт"},
    {4, 305, "Событие влияет на компьютер"},
    {4, 302, "Отменить событие после 1 посещения"},
    {4, 600, "Цвет игроков, на кого действует:"},
    {5, 100, "Информация о герое"},
    {5, 200, "Войска"},
    {5, 201, "Обычные"},
    {5, 202, "Выбрать"},
    {5, 210, "Тип"},
    {5, 211, "К-во."},
    {5, 212, "Мон 1"},
    {5, 213, "Мон 2"},
    {5, 214, "Мон 3"},
    {5, 215, "Мон 4"},
    {5, 216, "Мон 5"},
    {5, 300, "Артефакты"},
    {5, 304, "Слот 1"},
    {5, 305, "Слот 2"},
    {5, 306, "Слот 3"},
    {5, 800, "Патрулировать"},
    {5, 400, "Опыт"},
    {5, 500, "Вторичные навыки"},
    {5, 501, "Обычные"},
    {5, 502, "Выбрать"},
    {5, 510, "Навык 1"},
    {5, 511, "Навык 2"},
    {5, 512, "Навык 3"},
    {5, 513, "Навык 4"},
    {5, 514, "Навык 5"},
    {5, 515, "Навык 6"},
    {5, 516, "Навык 7"},
    {5, 517, "Навык 8"},
    {5, 600, "Имя"},
    {5, 601, "Обычное"},
    {5, 602, "Задать"},
    {5, 700, "Портрет"},
    {8, 500, "Монстры\n\n\n\n0 для случайного количества.,\n\nили положительное число."},
    {10, 100, "Загадка"},
    {10, 101, "Текст загадки"},
    {10, 102, "Награда за верный ответ\n(отрицательное число тоже подходит)"},
    {10, 103, "Древесина"},
    {10, 104, "Ртуть"},
    {10, 105, "Руда"},
    {10, 106, "Сера"},
    {10, 107, "Кристаллы"},
    {10, 108, "Самоцветы"},
    {10, 109, "Золото"},
    {10, 300, "Дать артефакт"},
    {10, 400, "Ответ(ы)"},
    {11, 100, "Слух"},
    {13, 200, "Условие победы"},
    {13, 220, "Компьютер также выигрывает через особые условия победы"},
    {13, 221, "Также обычные условия победы"},
    {13, 250, "Особые условия победы"},
    {13, 300, "Условия поражения"},
    {13, 320, "Особые условия поражения"},
    {13, 400, "Название карты"},
    {13, 401, "Название файла:"},
    {13, 500, "Описание"},
    {13, 600, "Сложность"},
    {13, 610, "Легкая"},
    {13, 611, "Обычная"},
    {13, 612, "Тяжелая"},
    {13, 613, "Эксперт"},
    {13, 100, "Игроки"},
    {13, 700, "Начинать с героем в каждом главном замке"},
    {13, 800, "Слухи"},
    {13, 900, "События"},
    {15, 100, "Город"},
    {15, 200, "Войска"},
    {15, 201, "Обычные"},
    {15, 202, "Выбрать"},
    {15, 210, "Тип"},
    {15, 211, "К-во."},
    {15, 212, "Мон 1"},
    {15, 213, "Мон 2"},
    {15, 214, "Мон 3"},
    {15, 215, "Мон 4"},
    {15, 216, "Мон 5"},
    {15, 600, "Название"},
    {15, 601, "Обычное"},
    {15, 602, "Выбрать"},
    {15, 300, "Капитан"},
    {15, 310, "Можно строить замок"},
    {15, 400, "Постройки"},
    {15, 401, "Обычные"},
    {15, 402, "Выбрать"},
    {15, 470, "Гильдия магов"},
    {15, 510, "Жилище 1"},
    {15, 512, "Жилище 2"},
    {15, 513, "Улучш."},
    {15, 514, "Жилище 3"},
    {15, 515, "Улучш."},
    {15, 516, "Жилище 4"},
    {15, 517, "Улучш."},
    {15, 518, "Жилище 5"},
    {15, 519, "Улучш."},
    {15, 520, "Жилище 6"},
    {15, 521, "Улучш."}
};

const char* gArtifactNames[(ARTIFACT_COUNT)] = {
    "Книга всезнания",
    "Меч власти",
    "Защитная накидка",
    "Жезл магии",
    "Всемогущий щит",
    "Всемогущий посох",
    "Корона всевластия",
    "Золотой гусь",
    "Ожерелье тайной магии",
    "Магический браслет",
    "Кольцо мага",
    "Брошь ведьмы",
    "Медаль отваги",
    "Медаль мужества",
    "Медаль доблести",
    "Медаль почета",
    "Символ неудачи",
    "Громовая палица",
    "Защитная перчатка",
    "Шлем защитника",
    "Гигантский цеп",
    "Баллиста",
    "Незримый щит",
    "Драконий меч",
    "Топор власти",
    "Божественный доспех",
    "Малый свиток знания",
    "Большой свиток знания",
    "Могущественный свиток знания",
    "Свиток высшего знания",
    "Бездонный мешок",
    "Бездонная сума",
    "Бездонный кошель",
    "Башмаки кочевника",
    "Башмаки путника",
    "Лапка кролика",
    "Золотая подкова",
    "Счастливая монета",
    "Клевер",
    "Компас",
    "Астролябия",
    "Дурной глаз",
    "Зачарованные часы",
    "Золотые часы",
    "Шапочка",
    "Ледяная накидка",
    "Огненная накидка",
    "Громовой шлем",
    "Нетающий лед",
    "Горячий камень",
    "Жезл молний",
    "Кольцо змеи",
    "Символ жизни",
    "Книга стихий",
    "Кольцо стихий",
    "Святой кулон",
    "Подвеска свободной воли",
    "Кулон жизни",
    "Подвеска покоя",
    "Всевидящий глаз",
    "Кулон движения",
    "Кулон смерти",
    "Посох отрицания",
    "Золотой лук",
    "Телескоп",
    "Перо дипломата",
    "Шляпа мага",
    "Кольцо силы",
    "Обоз",
    "Подать",
    "Ужасная маска",
    "Бездонная сума серы",
    "Бездонная колба ртути",
    "Бездонная сума самоцветов",
    "Нескончаемая вязанка дров",
    "Бездонная вагонетка руды",
    "Бездонная сума кристаллов",
    "Шлем с шипами",
    "Щит с шипами",
    "Белая жемчужина",
    "Черная жемчужина",
    "Волшебная книга",
    "ERROR : Artifact 82"  ,
    "ERROR : Artifact 83"  ,
    "ERROR : Artifact 84"  ,
    "ERROR : Artifact 85"  ,
    "Свиток заклинаний",
    "Рука мученика",
    "Доспех Андурана",
    "Защитная брошь",
    "Боевое одеяние Андурана",
    "Кристальный шар",
    "Сердце огня",
    "Ледяное сердце",
    "Шлем Андурана",
    "Святой молот",
    "Легендарный скипетр",
    "Наконечник мачты",
    "Сфера антимагии",
    "Волшебный посох",
    "Мечелом",
    "Меч Андурана",
    "Лопата могильщика"
};
const char* gArtifactDesc[(ARTIFACT_COUNT)] = {
    "{Книга всезнания}\n(Знания +12)\n\nКнига всезнания увеличивает Знания на 12 единиц.",
    "{Меч власти}\n(Атака +12)\n\nМеч власти увеличивает навык Атаки на 12 единиц.",
    "{Защитная накидка}\n(Защита +12)\n\nЗащитная накидка увеличивает Защиту на 12 единиц.",
    "{Жезл магии}\n(Сила магии +12)\n\nЖезл магии увеличивает Силу заклинаний на 12 единиц.",
    "{Всемогущий щит}\n\nВсемогущий щит увеличивает Атаку и Защиту на 6 единиц каждый.",
    "{Всемогущий посох}\n\nВсемогущий посох увеличивает Силу магии и Знания на 6 единиц каждый.",
    "{Корона всевластия}\n\nКорона всевластия увеличивает каждый из базовых навыков на 4 единицы.",
    "{Золотой гусь}\n\nЗолотой гусь приносит в вашу казну по 10.000 золотых каждый день.",
    "{Ожерелье тайной магии}\n(Сила магии +4)\n\nОжерелье тайной магии увеличивает Силу магии на 4 единицы.",
    "{Магический браслет}\n(Сила магии +2)\n\nМагический браслет увеличивает Силу магии на 2 единицы.",
    "{Кольцо мага}\n(Сила магии +2)\n\nКольцо мага увеличивает Силу магии на 2 единицы.",
    "{Брошь ведьмы}\n(Сила магии +3)\n\nБрошь ведьмы увеличивает Силу магии на 3 единицы.",
    "{Медаль отваги}\n\nМедаль отваги увеличивает мораль.",
    "{Медаль мужества}\n\nМедаль мужества увеличивает мораль.",
    "{Медаль доблести}\n\nМедаль доблести увеличивает мораль.",
    "{Медаль почета}\n\nМедаль почета увеличивает мораль.",
    "{Символ неудачи}\n\nСимвол неудачи сильно уменьшает мораль.",
    "{Громовая палица}\n(Атака +1)\n\nГромовая палица увеличивает навык Атаки на 1 единицу.",
    "{Защитная перчатка}\n(Защита +1)\n\nЗащитная перчатка увеличивает навык Защиты на 1 единицу.",
    "{Шлем защитника}\n(Защита +1)\n\nШлем защитника увеличивает навык Защиты на 1 единицу.",
    "{Гигантский цеп}\n(Атака +1)\n\nГигантский цеп увеличивает навык Атаки на 1 единицу.",
    "{Баллиста}\n\nБаллиста позволяет вашей катапульте дважды стрелять в один ход боя.",
    "{Незримый щит}\n(Защита +2)\n\nНезримый щит увеличивает навык Защиты на 2 единицы.",
    "{Драконий меч}\n(Атака +3)\n\nДраконий меч увеличивает навык Атаки на 3 единицы.",
    "{Топор власти}\n(Атака +2)\n\nТопор власти увеличивает навык Атаки на 2 единицы.",
    "{Божественный доспех}\n(Защита +3)\n\nБожественный доспех увеличивает навык Защиты на 3 единицы.",
    "{Малый свиток знания}\n(Знания +2)\n\nМалый свиток знания увеличивает Знания на 2 единицы.",
    "{Большой свиток знания}\n(Знания +3)\n\nБольшой свиток знания увеличивает Знания на 3 единицы.",
    "{Могущественный свиток знания}\n(Знания +4)\n\nМогущественный свиток Знания увеличивает Знания на 4 единицы.",
    "{Свиток высшего знания}\n(Знания +5)\n\nСвиток высшего знания увеличивает Знания на 5 единиц.",
    "{Бездонный мешок}\n\nБездонный мешок приносит вам 1000 золотых в день.",
    "{Бездонная сума}\n\nБездонная сума приносит вам 750 золотых в день.",
    "{Бездонный кошель}\n\nБездонный кошель приносит вам 500 золотых в день.",
    "{Башмаки кочевника}\n\nБашмаки кочевника увеличивают дальность передвижения по суше.",
    "{Башмаки путника}\n\nБашмаки путника увеличивают подвижность отряда на суше.",
    "{Лапка кролика}\n\nЛапка кролика увеличивает удачу в бою.",
    "{Золотая подкова}\n\nЗолотая подкова увеличивает удачу в бою.",
    "{Счастливая монета}\n\nСчастливая монета увеличивает удачу в бою.",
    "{Клевер}\n\nКлевер увеличивает удачу в бою.",
    "{Компас}\n\nКомпас увеличивает подвижность отряда на суше и на море.",
    "{Астролябия}\n\nАстролябия увеличивает подвижность отряда на море.",
    "{Дурной глаз}\n\nАртефакт снижает вполовину количество магической энергии, требуемой на направление заклинаний-проклятий.",
    "{Зачарованные часы}\n\nАртефакт продлевает действие всех ваших заклинаний на 2 хода.",
    "{Золотые часы}\n\nАртефакт удваивает эффективность использования заклинания гипноза.",
    "{Шапочка}\n\nСнижает вполовину затраты магической энергии на все заклинания влияющие на разум.",
    "{Ледяная накидка}\n\nСнижает вполовину урон, наносимый вашим воинам заклинаниями холода.",
    "{Огненная накидка}\n\nСнижает вполовину урон, наносимый вашим воинам заклинаниями огня.",
    "{Громовой шлем}\n\nСнижает вполовину урон, наносимый вашим воинам заклинаниями молний.",
    "{Нетающий лед}\n\nУвеличивает на 50% урон, наносимый врагу вашими заклинаниями холода.",
    "{Горячий камень}\n\nУвеличивает на 50% урон, наносимый врагу вашими заклинаниями огня.",
    "{Жезл молний}\n\nУвеличивает на 50% урон, наносимый врагу вашими заклинаниями молний.",
    "{Кольцо змеи}\n\nСнижает вполовину затраты магической энергии на заклинания-благословения.",
    "{Символ жизни}\n\nУвеличивает вдвое эффективность всех заклинаний связанных с воскрешением и оживлением существ.",
    "{Книга стихий}\n\nУвеличивает вдвое эффективность всех заклинаний, связанных с призывом существ.",
    "{Кольцо стихий}\n\nСнижает вполовину затраты на все заклинания, связанные с вызовом существ.",
    "{Святой кулон}\n\nНаделяет ваших воинов иммунитетом к заклинаниям-проклятиям.",
    "{Подвеска свободной воли}\n\nНаделяет ваших воинов иммунитетом к заклинаниям, связанным с гипнозом.",
    "{Кулон жизни}\n\nНаделяет ваших воинов иммунитетом ко всем заклинаниям Смерти.",
    "{Подвеска покоя}\n\nНаделяет ваших воинов иммунитетом к заклинанию Берсерк.",
    "{Всевидящий глаз}\n\nНаделяет ваших воинов иммунитетом ко всем заклинаниям ослепления.",
    "{Кулон движения}\n\nНаделяет ваших воинов иммунитетом ко всем парализующим заклинаниям.",
    "{Кулон смерти}\n\nНаделяет ваших воинов иммунитетом ко всем святым заклинаниям.",
    "{Посох отрицания}\n\nАртефакт защищает ваших воинов от заклинания снятия чар.",
    "{Золотой лук}\n\nСнижает вполовину штраф на урон для ваших воинов, стреляющих через препятствия (например, стены замка).",
    "{Телескоп}\n\nУвеличивает радиус обзора странствующего героя на 1 клетку.",
    "{Перо дипломата}\n\nСнижает стоимость сдачи на 10% от общей стоимости армии вашего героя.",
    "{Шляпа мага}\n\nАртефакт продлевает действие ваших заклинаний на 10 ходов!",
    "{Кольцо силы}\n\nАртефакт возвращает герою 2 дополнительных очка магии за ход.",
    "{Обоз}\n\nОбеспечивает ваших воинов-стрелков нескончаемым запасом стрел.",
    "{Подать}\n\nАртефакт принуждает вас выплачивать каждый ход 250 золотых налогов.",
    "{Ужасная маска}\n\nЭтот артефакт не позволяет любым воинам и существам вступить в вашу армию.",
    "{Бездонная сума серы}\n\nАртефакт приносит вам 1 единицу серы в день.",
    "{Бездонная колба ртути}\n\nАртефакт приносит вам 1 единицу ртути в день.",
    "{Бездонная сума самоцветов}\n\nАртефакт приносит вам 1 единицу самоцветов в день.",
    "{Нескончаемая вязанка дров}\n\nАртефакт приносит вам 1 единицу древесины в день.",
    "{Бездонная вагонетка руды}\n\nАртефакт приносит вам 1 единицу руды в день.",
    "{Бездонная сума кристаллов}\n\nАртефакт приносит вам 1 единицу кристаллов в день.",
    "{Шлем с шипами}\n\nАртефакт увеличивает параметры Атаки и Защиты на 1 единицу каждый.",
    "{Щит с шипами}\n\n Артефакт увеличивает параметры Атаки и Защиты на 2 единицы каждый.",
    "{Белая жемчужина}\n\n Артефакт увеличивает параметры Силы магии и Знания на 1 единицу каждый.",
    "{Черная жемчужина}\n\n Артефакт увеличивает параметры Силы магии и Знания на 2 единицы каждый.",
    "{Волшебная книга}\n\nВолшебная книга позволяет направлять заклинания.",
    "{ERROR}\n\nArtifact 82."  ,
    "{ERROR}\n\nArtifact 83."  ,
    "{ERROR}\n\nArtifact 84."  ,
    "{ERROR}\n\nArtifact 85."  ,
    "{Свиток заклинаний}\n\nЭтот Свиток заклинаний позволяет вам направлять заклинание '%s'.",
    "{Рука мученика}\n\nРука мученика увеличивает Силу заклинаний вашего героя на 3 единицы, но дает штраф к морали за присутствия нежити в армии.",
    "{Доспех Андурана}\n\nУвеличивает Защиту на 5 единиц.",
    "{Защитная брошь}\n\nЗащитная брошь снижает на 50 процентов урон, наносимый заклинаниями Армагеддон и Буря Стихий. При этом, артефакт снижает Силу магии на 2 единицы.",
    "{Боевое одеяние}\n\nБоевое одеяние Андурана сочетает в себе силу трех артефактов Андурана. Также, артефакт повышает до максимума удачу и мораль вашей армии и дает возможность направлять заклинание Портал города.",
    "{Кристальный шар}\n\nКристальный шар дает вам более детальную информацию о монстрах, вражеских героях и том, кто защищает близлежащие от героя замки.",
    "{Сердце огня}\n\nСердце огня снижает на 50 процентов урон, наносимый силами огня, но удваивает урон, наносимый вам холодом.",
    "{Ледяное сердце}\n\nЛедяное сердце снижает на 50 процентов урон, наносимый силами холода, но удваивает урон, наносимый вам огнем.",
    "{Шлем Андурана}\n\nУвеличивает Силу заклинаний на 5 единиц.",
    "{Святой молот}\n\nУвеличивает Атаку на 5 единиц.",
    "{Легендарный скипетр}\n\nУвеличивает на 2 все характеристики героя.",
    "{Наконечник мачты}\n\nВ сражении на море увеличивает удачу и мораль вашей армии на 1 единицу.",
    "{Сфера антимагии}\n\nВ бою артефакт не позволяет обеим сторонам направлять заклинания.",
    "{Волшебный посох}\n\nУвеличивает Силу заклинаний на 5 единиц.",
    "{Мечелом}\n\nУвеличивает Защиту на 4 единицы и Атаку на 1 единицу.",
    "{Меч Андурана}\n\nУвеличивает Атаку на 5 единиц.",
    "{Лопата могильщика}\n\nУвеличивает эффективность использования навыка некромантии."};
const char* gArtifactEvent[(ARTIFACT_COUNT)] = {
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    "Вы вызволяете волшебницу, заточенную в проклятой гробнице, и в награду она вручает вам изысканное алмазное ожерелье.",
    "Изучая завалы в заброшенной шахте, вы спасаете артель гномов-старателей. В знак благодарности их старшина дарит вам золотой браслет.",
    "Вы спешите на звук отчаянного вопля боли и видите кентавра, попавшего в западню. Вы помогаете ему освободиться, и он вручает вам кожаный мешочек. Заглянув внутрь, вы видите ослепительное бриллиантовое кольцо.",
    "Рядом с останками сожженной колдуньи лежит изящная брошь прекрасной работы. Осторожно приблизившись к обугленному трупу, вы забираете брошь себе.",
    "В награду за спасение прекрасной девы от посягательств ненавистного барона королевский герольд вручает вам Медаль отваги.",
    "Вы спасаете маленького мальчика от стаи кровожадных волков и провожаете в имение родителей. Счастливый отец награждает вас Медалью мужества.",
    "Вы вырываете принцессу соседнего королевства из мерзких лап презренных работорговцев и в награду за подвиг получаете Медаль доблести.",
    "Вы избавляете округу от ужасного минотавра, добычей которому служили благородные рыцари, и становитесь кавалером Медали почета.",
    "На обочине пустынной дороги вы находите медаль. Вы подобрали ее и обнаружили, что стали несчастным обладателем Символа неудачи, который понижает боевой дух вашей армии.",
    "Во время жуткой грозы молния бьет в дерево, разнося его на мелкие щепки. Среди обломков вы обнаруживаете таинственную палицу.",
    "Вы повстречали печально известного Черного Рыцаря! Ваш поединок заканчивается вничью, и рыцарь в знак уважения дарит вам пару латных перчаток.",
    "Краем глаза вы замечаете золотистый блеск среди пышной зелени. Приглядевшись внимательнее, вы находите под кустами великолепный золотой шлем.",
    "Неуклюжий гигант нанес себе смертельную рану собственным боевым цепом. Вы прекрасно владеете этим оружием и с уверенностью вынимаете цеп из мертвых рук гиганта.",
    "Пробираясь через развалины древней крепости, вы находите орудие, которое превратило ее в руины, удивительную баллисту замысловатой конструкции.",
    "В руках у каменной статуи воина - великолепный серебряный щит. Как только вы забираете щит себе, статуя рассыпается в прах.",
    "Вы пробираетесь узкой тропой, как вдруг ближайший куст загорается ярким пламенем. В огненном смерче появляется прекрасная дама, которая протягивает вам волшебный меч.",
    "Вы видите серебряный топор, вогнанный в землю по самую рукоять. Ваши воины пытаются выдернуть его, но усилия их тщетны. Вам же хватило одного усилия и топор у вас в руках!",
    "Шайка разбойников обыскивает тела мертвых воинов. Вы разгоняете мародеров и вдруг замечаете, что в спешке они потеряли великолепный доспех.",
    "Перед вами возникает парящий в воздухе стеклянный ларец со свитком внутри, лежащем на подушке из пурпурного бархата. От прикосновения, крышка ларца открывается, и свиток оказывается у вас в руках.",
    "Вы навещаете местного мудреца и рассказываете о цели вашего путешествия. Он достает из мешка пожелтевший свиток и передает его вам.",
    "Вы стоите перед останками давно умершей жрицы друидов. Пожелтевшие от времени кости проглядывают через прорехи истлевшего одеяния. Пошевелив груду ветоши, вы находите древний свиток.",
    "Груда пожелтевших костей и обрывки истлевшей материи - вот все, что осталось от жрицы друидов. Среди этих останков вы замечаете таинственный свиток.",
    "Маленький лепрекон пританцовывает у волшебного мешка. Завидев вас, он замирает на месте, затем издает возмущенный возглас, топает ножкой и растворяется в воздухе. Вы забираете мешок себе.",
    "Благородная путешественница, отбившаяся от спутников, просит вас о помощи. Проводив ее до дома, вы получаете в награду суму, полную золота.",
    "Однажды вам в руки попадает наполненный золотом кожаный кошель, принадлежавший великому королю, который умел превращать любой предмет в золото.",
    "Бродячий торговец просит вас защитить его от банды гоблинов. В награду он дарит вам пару изящных башмаков, испещренных загадочными древними письменами.",
    "Обнаружив пару замечательных башмаков украшенных бисером, вы благодарите загадочного благодетеля и оставляете их себе.",
    "В уплату за охрану в пути странствующий торговец предлагает вам лапку кролика. По его словам, она принесет вам удачу в бою.",
    "Попавший в ловушку единорог испуганно кричит. Вы успокаиваете его и освобождаете от пут. Всхрапнув и ударив копытом, он уносится прочь. Там, где он только что стоял, осталась лежать золотая подкова.",
    "Вы поймали озорного бесенка, который не давал покоя всей округе. В обмен на свободу он предлагает вам волшебную монету.",
    "Посреди мертвой лощины, заполненной иссохшей растительностью, вы, к своему удивлению, замечете веселый зеленый побег четырехлистного клевера.",
    "Странноватый старикашка утверждает, что он - великий изобретатель, и просит вас испытать его новое творение. Надувшись от важности, он вручает вам компас.",
    "Старый мореход стал добычей людоедов. Вы спасаете его, и в знак благодарности он дарит вам чудесный инструмент, позволяющий измерять расстояния по звездам.",
    "В заброшенной хижине вы находите скелет давно почившей колдуньи. Приглядевшись, вы замечаете, что в глазнице пожелтевшего черепа зловеще вращается стеклянный глаз.",
    "За невысоким холмом перед вами открывается зловещая картина - стаи стервятников пируют на поле недавней битвы. Среди тел поверженных воинов вы находите волшебные песочные часы.",
    "Вы помогаете бродячему торговцу снадобьями вытащить повозку из придорожной канавы. В знак благодарности он вручает вам золотые часы. Он и не подозревал, что часы волшебные!",
    "Вы делаете короткую остановку в маленькой придорожной харчевне. Под звон монет происходит обмен новостями, а то и редкими вещицами. Вот таким-то образом в вашем багаже и оказывается волшебная шапочка.",
    "Вы спешите на отчаянные крики и видите очаровательную девушку, за которой гонится разъяренный медведь. Через мгновение зверь повержен, и благодарная волшебница шьет вам из его шкуры волшебный плащ.",
    "За поворотом дороги вы видите сражающихся некроманта и паладина. Некромант атакует паладина, и тот падает на колени. Вы спасаете жизнь паладину, убивая его врага. Паладин дарит вам свою огненную накидку.",
    "Бродячий медник, у которого кончилась провизия, предлагает вам шлем с гребнем в виде молнии в обмен на еду и питье. Вы соглашаетесь на обмен, а вскоре обнаруживаете, что шлем обладает еще и магическими свойствами.",
    "Ваше внимание привлекает ледяная сосулька, которая не тает, несмотря на полуденный зной. Вы отламываете ее от карниза и с удивлением обнаруживаете, что даже тепло ваших рук ей нипочем.",
    "В дальней стране вы встречаете племя приматов. Они разжигают костры при помощи волшебного куска лавы. Вы научили их добывать огонь обычным способом. Обезьяны считают вас богом и дарят свой заветный кусок лавы.",
    "Во время ужасной грозы на ваших глазах в громоотвод дома бьет молния. Расплавленный громоотвод падает на землю, но его наконечник остается целым и невредимым. Вы подобрали его - оказалось, это магический предмет!",
    "На пальце мертвого странника вы видите необычное кольцо. Оно имеет форму змеи, вцепившейся зубами в собственный хвост.",
    "Песчаная буря обнажила вход в подземную гробницу. Вы спускаетесь внутрь и обнаруживаете, что здесь уже побывали грабители, однако в темноте они не заметили символ вечной жизни, висящий на серебряной цепи.",
    "Вы встречаете заклинателя, который просит разрешить ему воспользоваться вашим покровительством на опасном участке пути. Вы соглашаетесь, и в награду он дарит вам Книгу Стихий.",
    "Расположившись на отдых под невысоким деревом, вы замечаете дикого кота, который подбирается к вороньему гнезду. Вы прогоняете кота, и сами залезаете на дерево. В гнезде вы находите кольцо тонкой работы.",
    "Странствуя по дальним землям, вы встречаете отшельника, живущего в маленькой аккуратной хижине. Узнав о цели ваших скитаний, он прерывает свои размышления, благословляет вас и дарит амулет, защищающий от злых чар.",
    "Вы слышите крики о помощи и, поспешив на берег реки, видите фей, потешающихся над стариком, окуная его в воду. Вы выручаете старика из беды и вытаскиваете одну фею на берег. В обмен на свободу она отдает вам подвеску.",
    "В дороге вы встречаете небольшой караван. Сыграв с хозяином каравана в кости, вы выигрываете волшебную подвеску. Ее прежний владелец утверждает, что она может противостоять чарам смерти некромантов.",
    "Вы спешите на шум сражения и видите старика-варвара, который с трудом отбивается от гидры. В награду за помощь варвар дарит вам волшебный кулон.",
    "В хижине у дороги вы находите слепую старуху, умирающую в полном одиночестве. Вы обещаете устроить ей достойные похороны. В знак благодарности она дарит вам волшебную подвеску.",
    "Дорогу вам преграждает голем, на шее которого сверкает кулон. Вы перерезаете шнурок, и он падает на землю. Голем рассыпается у вас на глазах, а кулон достается вам.",
    "После короткой ожесточенной схватки с некромантом у вас в руках остается его волшебный кулон. Знакомый чародей объясняет вам, что этот кулон защищает нежить, состоящую в вашей армии, от святого слова.",
    "Навстречу вам попадается старый друг-чародей. Он вручает вам подарок - волшебный жезл, который делает невозможным применение заклинания снятие чар против ваших соратников.",
    "Вы случайно встречаете знаменитого стрелка и предлагаете ему сыграть в кости. Он соглашается и ставит свой лук против вашего коня. Вы выигрываете.",
    "Торговец из далеких земель предлагает вам новейшее изобретение своего народа в обмен на съестные припасы. Эта штука, благодаря которой удаленные предметы кажутся ближе, называется телескопом.",
    "Вы помогаете дипломату починить сломанную ось в его экипаже, и в знак благодарности он дарит вам перо. Он говорит, что это перо заставляет людей смотреть на вещи глазами его обладателя.",
    "Вы видите чародея, который удирает от грифона. Вот он распахнул портал и ринулся внутрь, но при этом зацепился шляпой, и она упала она на землю. Вы поднимаете шляпу, отряхиваете ее от пыли и оставляете себе.",
    "Вы замечаете дерево, похожее на чернокнижника Карнота. На одной из его веток сверкает кольцо. Вы все равно ничем не можете ему помочь, и поэтому забираете кольцо себе.",
    "Ваше внимание привлекает повозка с боеприпасами, стоящая посреди поля, где когда-то гремела битва. Убедившись, что она в хорошем состоянии, вы присоединяете ее к своему обозу.",
    "Ваша налоговая декларация превысила приделы. Мытарь сжалился над вами и согласился ежедневно получать от вас всего по 250 золотых.",
    "Вы вскрыли могилу Синфилия Гардолада, знаменитого чернокнижника, и находите в ней маску. Надев ее, ваше лицо искажает гримаса ужаса. Видимо вам достался маска Громлака Грина. Теперь от нее не избавиться!",
    "Вы посещаете алхимика, который при виде вашей армии незамедлительно признает вас достойнейшим из достойных. Новый подданный дарит вам бездонную сумку серы, которая вам очень даже пригодится.",
    "Вы делаете короткий привал в башне чародея, покинутой хозяином, и находите волшебный сосуд с ртутью, содержимое которого никогда не кончается. Это же настоящее сокровище!",
    "После короткого ливня на небе появляется радуга. Заметив место, где она упирается в землю, вы находите там горшок золота. Его хозяин, маленький эльф, предлагает взамен бездонную суму самоцветов.",
    "Вы останавливаетесь на отдых и разводите костер. Неподалеку лежит куча дров. Вы берете одно полено за другим, но куча не уменьшается. Вы с радостью понимаете, что дрова зачарованы, и забираете их себе.",
    "Вы находите кузницу гоблинов, где они куют оружие. С воинственным кличем, ваши воины нападают на их лагерь и убивают всех врагов. Осмотрев трофеи, вы обнаруживаете волшебную вагонетку с рудой.",
    "Укрывшись от бури в небольшой пещерке, вы замечаете в углу друзу кристаллов. Вы отламываете кусок, а на его месте вырастает новый кристалл. Вы забераете это сокровище с собой.",
    "Небольшой отряд орков нападает на вашу армию. Вы без труда отбиваете атаку. На теле одного из нападавших вы видите блестящий шлем с шипами.",
    "Вы приближаетесь к мосту через глубокий овраг. Неожиданно из-под моста появляется тролль и требует плату за проход. После отказа, тролль нападает на вас. Убив его, вы забираете себе его шит с шипами.",
    "Вы пересекаете пересохшее соляное озеро, и вдруг среди обломков ракушек и кусков коралла замечаете великолепную белую жемчужину.",
    "Слухи об огромном грифоне, нагоняющем ужас на всю округу, приводят вас в его логово. Жестокая схватка заканчивается вашей победой, и в опустевшем гнезде вы находите черную жемчужину.",
    ""  ,
    "ERROR : Artifact event 82."  ,
    "ERROR : Artifact event 83."  ,
    "ERROR : Artifact event 84."  ,
    "ERROR : Artifact event 85."  ,
    "Вы нашли резной ларец, в котором хранился древний свиток. Руны на ларце очень древние. Развернув свиток, вы почувствовали пульсацию магических сил.",
    "Один из ваших воинов подобрал с земли оторванную руку. Несмотря на то, что рука была оторвана от тела, она все еще продолжала шевелиться. Ваши воины испытали великое отвращение к этому предмету, но вы не смогли заставить себя выкинуть ее.",
    "Вы обнаружили указатель, на котором было написано, что здесь покоится великий Андуран. Надпись молвила, что преклонивший чело перед могилой будет вознагражден. Вы поступили, как того требовалось, и получили в награду волшебный доспех.",
    "Добрая колдунья сочла, что ваша армия плохо защищена и даровала вам свою волшебную брошь.",
    "Вы купили у бедняка ящик со всяким барахлом и на свое удивление нашли в нем три вещи из боевого одеяния Андурана! Вот это удача!",
    "Вы проходили мимо труппы бродячих актеров. Они попросили вас станцевать рума-буту. Вы исполнили несколько произвольных движений, и они за храбрость даровали вам кристальный шар.",
    "Вы попали на недавно сгоревшую поляну. Посреди поляны, на камне стоял сосуд, в котором сидел огненный элементал. Вы решили взять с собой эту диковинную находку.",
    "Неожиданно вас сковал пронзительный холод. От неожиданного шока вы упали с коня на землю. Мимо вас промчался огромный ледяной гигант. В спешке он обронил одну ценную вещь!",
    "Вы заметили сверкающий объект невдалеке. Вы послали одного из ваших воинов посмотреть, что это там. Он вернулся с золотым шлемом в руках, который оказался ни чем иным, как шлемом легендарного Андурана!",
    "Вы стали свидетелем поединка, в котором паладин был смертельно ранен отрядом зомби. Он попросил вас взять его молот и завершить начатое им дело. Убив зомби вы повесили молот на свой пояс и удалились.",
    "Минуя небольшой холм, вы увидели, как маленькая фея тащит огромный скипетр. Улыбнувшись, вы спросили, не нужна ли ей помощь. Фея обиженно спросила, мол, думаешь, это смешно? Вспорхнула и улетела, а скипетр остался вам.",
    "Старый моряк рассказал вам, что в былые времена, на его ботике стояла мачта, приносящая ему удачу. Он бал вам схему, где ее можно будет найти. Через несколько часов поиска, вы нашли мачту в старом доке.",
    "На вас налетел торопыга-крестьянин. Он хотел убежать, но вы остановили его. Извинившись, крестьянин вручил вам необычную сферу. Едва вы дотронулись до нее, как почувствовали, что сфера втягивает в себя магию...",
    "Ваши солдаты нашли необычную вещь и решили принести ее вам. Вы отчистили ее от грязи и смогли прочитать на ней необычные слова: \"Ум - лучшая сила, а магия сильнее грубой силы. Помни мои слова, и ты всегда будешь побеждать.\"",
    "Отставной капитан городской стражи узнал о вашем походе и даровал вам свой меч, сослуживший ему добрую службу в былые времена.",
    "Тролль остановил вас, сказав: \"Плати мне 5000 золотых или я убью тебя мечом Анудрана!\" Вы отказались платить. Тролль схватился за клинок меча, взвыл от боли и бросив меч убежал. Хорошо, что он был настолько глуп, что не знал, как правильно держать острые предметы.",
    "В грязи вы подобрали старую лопату. Присмотревшись, вы поняли, что вам посчастливилось найти зачарованную лопату грабителей могил."};
const char* gStatNames[HERO_PRIMARY_STAT_COUNT] = {
    "Атака",
    "Защита",
    "Сила магии",
    "Знания"
};
const char* gStatDesc[HERO_PRIMARY_STAT_COUNT] = {
    "{Атака}\n\nВаш навык атаки - бонус, добавляемый к навыку атаки каждого воина.",
    "{Защита}\n\nВаш навык защиты - бонус, добавляемый к навыку защиты каждого воина.",
    "{Сила магии}\n\nВаш уровень силы магии определяет длительность действия или силу заклинания.",
    "{Знания}\n\nУровень знаний определяет количество очков магии героя."
};
const char* gAlignmentNames[KB_ALIGNMENT_NAME_COUNT] = {
    "Рыцарь",
    "Варвар",
    "Колдунья",
    "Чернокнижник",
    "Чародей",
    "Некромант",
    "Мульти",
    "Случайно"
};
const char* gArmyShortNames[(CREATURE_COUNT)] = {
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
const char* gArmyNames[(CREATURE_COUNT)] = {
    "Крестьянин",
    "Стрелок",
    "Рейнджер",
    "Копейщик",
    "Копейщик ветеран",
    "Мечник",
    "Мечник мастер",
    "Всадник",
    "Чемпион",
    "Паладин",
    "Крестоносец",
    "Гоблин",
    "Орк",
    "Вождь орков",
    "Волк",
    "Огр",
    "Лорд огров",
    "Тролль",
    "Боевой тролль",
    "Циклоп",
    "Фея",
    "Гном",
    "Боевой гном",
    "Эльф",
    "Высокий эльф",
    "Друид",
    "Старший друид",
    "Единорог",
    "Феникс",
    "Кентавр",
    "Горгулья",
    "Грифон",
    "Минотавр",
    "Царь минотавров",
    "Гидра",
    "Зеленый дракон",
    "Красный дракон",
    "Черный дракон",
    "Полурослик",
    "Боров",
    "Железный голем",
    "Стальной голем",
    "Рух",
    "Маг",
    "Архимаг",
    "Гигант",
    "Титан",
    "Скелет",
    "Зомби",
    "Зомби мутант",
    "Мумия",
    "Королевская мумия",
    "Вампир",
    "Лорд вампиров",
    "Лич",
    "Могучий лич",
    "Костяной дракон",
    "Разбойник",
    "Кочевник",
    "Призрак",
    "Джинн",
    "Медуза",
    "Земной элементал",
    "Воздушный элементал",
    "Огненный элементал",
    "Водяной элементал"
};
const char* gArmyNamesPlural[(CREATURE_COUNT)] = {
    "крестьян",
    "стрелков",
    "рейнджеров",
    "копейщиков",
    "копейщиков ветеранов",
    "мечников",
    "мечников мастеров",
    "всадников",
    "чемпионов",
    "паладинов",
    "крестоносцев",
    "гоблинов",
    "орков",
    "вождей орков",
    "волков",
    "огров",
    "лордов огров",
    "троллей",
    "боевых троллей",
    "циклопов",
    "фей",
    "гномов",
    "боевых гномов",
    "эльфов",
    "высоких эльфов",
    "друидов",
    "старших друидов",
    "единорогов",
    "фениксов",
    "кентавров",
    "горгулий",
    "грифонов",
    "минотавров",
    "царей минотавров",
    "гидр",
    "зеленых драконов",
    "красных драконов",
    "черных драконов",
    "полуросликов",
    "боровов",
    "железных големов",
    "стальных големов",
    "рухов",
    "магов",
    "архимагов",
    "гигантов",
    "титанов",
    "скелетов",
    "зомби",
    "зомби мутантов",
    "мумий",
    "королевских мумий",
    "вампиров",
    "лордов вампиров",
    "личей",
    "могучих личей",
    "костяных драконов",
    "разбойников",
    "кочевников",
    "призраков",
    "джиннов",
    "медуз",
    "земных элементалов",
    "воздушных элементалов",
    "огненных элементалов",
    "водных элементалов"
};
const char* gTerrainNames[(TERRAIN_COUNT)] = {
    "Вода",
    "Трава",
    "Снег",
    "Болото",
    "Лава",
    "Пустыня",
    "Грязь",
    "Пустошь",
    "Побережье"
};
const char* gResourceNames[(RES_COUNT)] = {
    "Древесина",
    "Ртуть",
    "Руда",
    "Сера",
    "Кристаллы",
    "Самоцветы",
    "Золото"
};


const char* gMineNames[(RES_COUNT)] = {
    "Лесопилка",
    "Лаборатория алхимика",
    "Рудная шахта",
    "Серная шахта",
    "Кристальная шахта",
    "Самоцветная шахта",
    "Золотая шахта"
};
const char* gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT] = {
    ""  ,
    "Лаборатория алхимика",
    "Указатель",
    "Буй",
    "Скелет",
    "Пещера демона",
    "Ларец с сокровищами",
    "Кольцо фейри",
    "Костер",
    "Фонтан",
    "Беседка",
    "Древняя лампа",
    "Кладбище",
    "Дом стрелков",
    "Хибара гоблина",
    "Избушка гномов",
    "Хижина крестьян",
    "Хижина",
    "Дорога",
    "Событие",
    "Драконий город",
    "Маяк",
    "Водяная мельница",
    "Шахта",
    "Бивуак",
    "Обелиск",
    "Оазис",
    "Ресурсы",
    ""  ,
    "Лесопилка",
    "Оракул",
    "Святилище 1-го Круга",
    "Кораблекрушение",
    "Сундук",
    "Шатер",
    "Город",
    "Менгир",
    "Фургоны",
    "Колодец",
    "Водоворот",
    "Ветряная мельница",
    "Артефакт",
    "Герой",
    "Корабль",
    "Могущественный артефакт",
    "Случайный артефакт",
    "Случайный ресурс",
    "Случайный монстр",
    "Случайный город",
    "Случайный замок",
    ""  ,
    "Случайный монстр - слабый",
    "Случайный монстр - средний",
    "Случайный монстр - сильный",
    "Случайный монстр - очень сильный",
    "Случайный герой",
    "Ничего особенного",
    ""  ,
    "Сторожевая вышка",
    "Древо-город",
    "Древо-город",
    "Руины",
    "Форт",
    "Базар",
    "Заброшенная шахта",
    "Лачуга гномов",
    "Стоячие камни",
    "Идол",
    "Древо знания",
    "Хижина ведьмы",
    "Храм",
    "Форт на холме",
    "Нора полурослика",
    "Лагерь наемников",
    "Святилище 2-го Круга",
    "Святилище 3-го Круга",
    "Пирамида",
    "Город мертвых",
    "Котлован",
    "Сфинкс",
    "Тележка",
    "Смоляная яма",
    "Артезианский источник",
    "Мост троллей",
    "Промоина",
    "Хижина ведьмы",
    "Ксанаду",
    "Пещера",
    "Навес",
    "Карты Магеллана",
    "Обломки",
    "Заброшенный корабль",
    "Потерпевший кораблекрушение",
    "Бутылка",
    "Волшебный колодец",
    "Волшебный сад",
    "Обзорная башня",
    "Литейный цех",
    "Потоки",
    "Деревья",
    "Горы",
    "Вулкан",
    "Цветы",
    "Камень",
    "Озеро",
    "Мандрагора",
    "Мертвое дерево",
    "Пень",
    "Кратер",
    "Кактус",
    "Курган",
    "Дюна",
    "Лавовый бассейн",
    "Куст",
    "Дыра",
    "Пласт",
    "Случайный артефакт - сокровище",
    "Случайный артефакт - обычный",
    "Случайный артефакт - ценный",
    "%s Барьер",
    "%s Шатер путника",
    "%s"  ,
    "%s"  ,
    "Темница"
};
const char* gEventText[KB_EVENT_TEXT_TABLE_COUNT] = {


    "Алхимик\n\nВы стали хозяином лаборатории местного алхимика. Она будет приносить вам по одной единице ртути в день.",

    "Указатель\n\nНа указателе написано:\n\n%s находится неподалеку отсюда.",

    "Буй\n\nВаши спутники замечают морской буй. Он указывает верный курс.",


    "Буй\n\nВаши спутники замечают морской буй. Он указывает верный курс, и это повышает их боевой дух.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    "Кольцо фейри\n\nВаше войско вступает внутрь кольца фейри, но ничего не происходит.",


    "Кольцо фейри\n\nВаше войско вступает внутрь кольца фейри, чары которого принесут вам удачу в грядущем сражении.",

    "Костер\n\nОбыскав вражеский лагерь, вы находите спрятанный клад.",

    "Фонтан\n\nВы припадаете к струям волшебного фонтана, но ничего не происходит.",

    "Фонтан\n\nБлагоуханная влага волшебного фонтана принесет вам удачу в грядущем сражении.",


    "Беседка\n\nНа ступенях беседки появляется старый рыцарь. \"Мне жаль, храбрый воин, но я уже научил тебя всему, что знаю сам.\"",


    "Беседка\n\nНа ступенях беседки появляется старый рыцарь. \"О храбрый воин, я научу тебя всему, что знаю сам; пусть мой опыт поможет тебе в твоих странствиях.\"",

    "Лампа джинна\n\nВы находите засыпанную землей помятую и закопченную лампа. Хотите ее потереть?",

    "Кладбище\n\nВы осторожно приближаетесь к захоронению древних воинов. Хотите вскрыть их могилы?",


    "Одержав победу над зомби, вы несколько часов подряд обыскиваете могилы, но ничего не находите. Ваш недостойный поступок отрицательно влияет на боевой дух войска.",

    "Одержав победу над зомби, вы обыскиваете могилы и удаляетесь с находкой!",


    "{Дом стрелков}\n\nГруппа стрелков в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "В вашем войске нет места для новых рекрутов.",

    "{Дом стрелков}\n\nПриблизившись к жилищу, вы обнаруживаете, что оно пустует.",


    "Хибара гоблинов\n\nГруппа гоблинов в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "Вы не можете принять новых рекрутов в свое войско, его ряды полны.",

    "Хибара гоблинов\n\nПриблизившись к жилищу гоблинов, вы обнаруживаете, что оно пустует.",


    "Хижина крестьян\n\nГруппа крестьян в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "Вы не можете принять новых рекрутов в свое войско, его ряды полны.",

    "Хижина крестьян\n\nПриблизившись к жилищу крестьян, вы обнаруживаете, что оно пустует.",


    "Избушка гномов\n\nГруппа стрелков в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "Вы не можете принять новых рекрутов в свое войско, его ряды полны.",

    "Избушка гномов\n\nПриблизившись к жилищу стрелков, вы обнаруживаете, что оно пустует.",


    "{Мазанка}\n\nГруппа крестьян в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "Вы не можете принять новых рекрутов в свое войско, его ряды полны.",

    "{Мазанка}\n\nПриблизившись к жилищу Крестьян, вы обнаруживаете, что оно пустует.",


    "{Древо-дом}\n\nГруппа фей в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "Вы не можете принять новых рекрутов в свое войско, его ряды полны.",

    "{Древо-дом}\n\nПриблизившись к древесному дому Фей, вы обнаруживаете, что он пустует.",


    "{Нора полуросликов}\n\nГруппа полуросликов в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "Вы не можете принять новых рекрутов в свое войско, его ряды полны.",

    "{Нора полуросликов}\n\nПриблизившись к норе полуросликов, вы обнаруживаете, что она пустует.",


    "{Сторожевая вышка}\n\nГруппа орков в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "Вы не можете принять новых рекрутов в свое войско, его ряды полны.",

    "{Сторожевая вышка}\n\nПриблизившись к сторожевой вышке орков, вы обнаруживаете, что она пустует.",


    "{Снежная пещера}\n\nГруппа кентавров в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "Вы не можете принять новых рекрутов в свое войско, его ряды полны.",

    "{Пещера}\n\nПриблизившись к пещере кентавров, вы обнаруживаете, что она пустует.",


    "{Раскопки}\n\nГруппа скелетов в поисках славы желает примкнуть к вашему войску. Согласны ли вы принять их?",

    "Вы не можете принять новых рекрутов в свое войско, его ряды полны.",

    "{Раскопки}\n\nПриблизившись к захоронению скелетов, вы обнаруживаете, что оно пустует.",
    "",
    "",
    "",
    "",
    "",

    "Маяк\n\nТеперь маяк ваш, и все ваши корабли будут преодолевать большее расстояние за один ход.",


    "Водяная мельница\n\nМельник обращается к вам со словами: \"Сожалею, господин, но сегодня золота у меня нет. Приходите на следующей неделе.\"",


    "Водяная мельница\n\nМельник обращается к вам со словами: \"Господин, я трудился в поте лица и прошу вас принять мою скромную лепту. Приходите на следующей неделе, и вы получите еще столько же.\"",

    "Рудная шахта\n\nВы стали хозяином рудной шахты. Она будет приносить вам по две меры руды в день.",


    "Серная шахта\n\nВы стали хозяином серной шахты. Она будут приносить вам по 1 единице серы в день.",


    "Кристальная шахта\n\nВы стали хозяином кристальной шахты. Она будет приносить вам по одной мере кристаллов в день.",


    "Самоцветная шахта\n\nВы стали хозяином самоцветной шахты. Она будет приносить вам по 1 единице самоцветов в день.",


    "Золотая шахта\n\nВы стали хозяином золотой шахты. Она будет приносить вам по 1000 золотых в день.",


    "Последователи\n\nГруппа %s в поисках славы желает примкнуть к вашему войску. Вы согласны принять их?",

    "Оскорбленные отказом быть принятыми в ваши ряды, они нападают на вас!",


    "Обелиск\n\nПеред вами обелиск, высеченный из невиданного камня. Вы вглядываетесь в его гладкую поверхность и вдруг замечаете, что на ней начинают проступать таинственные знаки. Знаки складываются во фрагмент древней карты. Вы торопливо срисовываете его, и знаки исчезают так же внезапно, как и появились.",

    "Обелиск\n\nВы уже посещали этот обелиск.",
    "",
    "",

    "Вы нашли ресурс (%s).",

    "Лесопилка\n\nВы стали хозяином лесопилки. Она будет приносить вам по 2 единицы древесины в день.",


    "{Оракул}\n\nНа поляне в окружении деревьев восседает слепой оракул. Вы рассказываете ему о целях вашего похода, и он показывает вам сильные и слабые стороны ваших противников в магическом хрустальном шаре.",
    "",
    "",
    "",
    "",
    "",
    "",


    "{Шатер}\n\nВаше внимание привлекает шатер, пологи которых трепещут на жарком ветру пустыни. В нем никого нет. Пройдет время, и, быть может, сюда придет новый отряд кочевников.",


    "{Шатер}\n\nВаше внимание привлекают шатер, пологи которого трепещут на жарком ветру пустыни. Вы хотите принять в ваше войско отряд кочевников?",


    "{Повозка}\n\nЦветастая повозка разбойников пуста. Пройдет время, и, быть может, здесь обоснуется новая шайка.",


    "{Повозка}\n\nВдалеке слышится музыка и смех. Вы идете на звуки и видите цветастую повозку, в которой живут разбойники. Вы хотите принять в ваше войско шайку разбойников?",

    "{Водоворот}\n\nВаш корабль попадает в водоворот. Часть вашего войска исчезает в пучине.",


    "{Ветряная мельница}\n\nМельник обращается к вам со словами: \"Сожалею, господин, но сегодня у меня ничего нет. Приходите на следующей неделе.\"",


    "{Ветряная мельница}\n\nМельник обращается к вам со словами: \"Господин, я работал не покладая рук, и прошу вас принять мой скромный дар. Приходите на следующей неделе, у меня опять найдется, чем вас порадовать.\"",
    "",
    "",
    "",
    "",
    "",


    "{Скелет}\n\nВы находите останки незадачливого искателя приключений. Пошарив в груде лохмотьев, вы ничего не находите.",


    "{Скелет}\n\nВы находите останки незадачливого искателя приключений. Пошарив в груде лохмотьев, вы находите."
};
const char* gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT] = {

    "Начать одиночную или сетевую игру.",

    "Загрузить сохраненную игру.",

    "Сохранить игру.",

    "Выйти из Героев Меча и Магии II.",

    "Закрыть меню, ничего не делая."
};
const char* gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT] = {

    "{ОК}\n\nЗакрыть это меню.",

    "{Скорость}\n\nУстановить скорость действий и анимации воинов в бою.",


    "{Информация о воине}\n\nВключить или выключить отображение окна с информацией о выбранном и атакуемом воине.",


    "{Магия в автобое}\n\nЕсли эта опция включена, ваш герой будет использовать заклинания во время автобоя. (Примечание: Эта опция не влияет на использование заклинаний компьютерными игроками, и на быстрый бой.)",


    "{Сетка}\n\nВключает или выключает отображение сетки. Все перемещения на поле боя происходят по гексагональной сетке, даже если ее отображение отключено.",


    "{Затенение сетки}\n\nВключает или выключает режим обозначения возможной дальности передвижения выбранного отряда воинов.",

    "{Курсор с тенью}\n\nВключает или выключает отрисовку тени от курсора на сетке координат."
};
const char* gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT] = {

    "Осмотреть весь мир.",

    "Посмотреть головоломку.",

    "Показать информацию о сценарии, на котором идет игра.",

    ("Копать в поисках Великого артефакта."),

    "Закрыть это меню."
};
const char* gInitMenuHelp[KB_INIT_MENU_HELP_COUNT] = {

    "{Новая игра}\n\nНачать отдельный сценарий или сетевую игру.",

    "{Игры}\n\nЗагрузить ранее сохраненную игру.",

    "{Рекорды}\n\nПоказать таблицу рекордов.",

    "{Авторы}\n\nПоказать перечень авторов игры.",

    "{Выйти}\n\nВыйти из героев Меча и Магии II и вернуться в операционную систему."
};
const char* gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT] = {

    "{Следующий герой}\n\nВыбрать следующего героя.",

    "{Продолжить движение}\n\nПродолжить движение героя по намеченному пути.",

    "{Обзор королевства}\n\nОсмотреть ваши владения.",

    "{Окончить ход}\n\nОкончить ход и передать управление компьютеру.",

    "{Игровые действия}\n\nОткрыть окно доступных игровых действий.",

    "{Окно файлов}\n\nОткрывает меню, где вы можете загружать или сохранять игры.",

    "{Системные настройки}\n\nОткрывает окно системных настроек, позволяющих настроить игру.",

    "{Направить заклинание}\n\nНаправить заклинание на стратегической карте."
};
const char* gLuckText[KB_LUCK_TEXT_COUNT] = {

    "Проклятая",

    "Ужасная",

    "Плохая",

    "Обычная",

    "Хорошая",

    "Отличная",

    "Божественная"
};
const char* gMoraleText[KB_MORALE_TEXT_COUNT] = {

    "Предательская",

    "Ужасная",

    "Плохая",

    "Обычная",

    "Хорошая",

    "Отличная",

    "Кровавая!"
};
const char* onOffText[KB_ON_OFF_TEXT_COUNT] = {

    "Выкл.",

    "Вкл.",

    "Вкл.\nГромкость 9",

    "Вкл.\nГромкость 8",

    "Вкл.\nГромкость 7",

    "Вкл.\nГромкость 6",

    "Вкл.\nГромкость 5",

    "Вкл.\nГромкость 4",

    "Вкл.\nГромкость 3",

    "Вкл.\nГромкость 2",

    "Вкл.\nГромкость 1"
};
const char* walkSpeedText[KB_WALK_SPEED_TEXT_COUNT] = {

    "Шагом",

    "Рысью",

    "Аллюром",

    "Галопом",

    "Прыжками"
};
const char* gColors[(FACTION_COUNT)] = {
    "синий",
    "зеленый",
    "красный",
    "желтый",
    "оранжевый",
    "фиолетовый"
};
static const char* gColorAbbreviations [[maybe_unused]][(FACTION_COUNT)] = {
    "син.",
    "зел.",
    "кр.",
    "жел.",
    "ор.",
    "фиол."
};
const char* gMonthNames[KB_MONTH_NAME_COUNT] = {

    "Кузнечика",

    "Муравья",

    "Стрекозы",

    "Паука",

    "Бабочки",

    "Шмеля",

    "Цикады",

    "Земляного червя",

    "Шершня",

    "Жука"
};
const char* gWeekNames[KB_WEEK_NAME_COUNT] = {

    "Белки",

    "Кролика",

    "Суслика",

    "Барсука",

    "Крысы",

    "Орла",

    "Горностая",

    "Ворона",

    "Мангуста",

    "Собаки",

    "Муравьеда",

    "Ящерицы",

    "Черепахи",

    "Дикобраза",

    "Кондора"
};
const char* cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT] = {

    "Обзор королевства",

    "%s - информация",

    "Дополнительная статистика героя",

    "Информация о высокой морали",

    "Информация об обычной морали",

    "Информация о плохой морали",

    "Информация о хорошей удаче",

    "Информация об обычной удаче",

    "Информация о плохой удаче",

    "Показать опыт",

    "Выбрать %s",

    "Пусто",

    "Перенести сюда отряд %s",

    "Отряды %s и %s меняются местами",

    "Показать заклинания",

    "Посмотреть информацию об: %s",

    "%s %s - уволить",

    "Закрыть экран героя",

    "Экран героя",

    "%s в один отряд",

    "Разделить отряд %s",

    "%s %s - информация",

    "Информация об очках магии",

    "Выбрать широкие ряды в бою",

    "Сгруппировать воинов"
};
const char* cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT] = {

    "Построить Гильдию магов",

    "Построены все этажи Гильдии магов.",

    "Нельзя построить следующий этаж.",

    "Построить следующий этаж Гильдии магов ",

    "Постройка '%s' уже возведена",

    "Нельзя возвести постройку '%s'",

    "Нельзя возвести постройку '%s'",

    "Возвести постройку '%s'",

    "Герой вам не по карману.",

    "Нельзя нанять - у вас уже %d героев.",

    "Нельзя нанять - в этом городе у вас уже есть герой.",

    "Нанять нового героя",

    "Выйти из замка",

    "Возможности замка",

    "Сгруппировать гарнизон",

    "Выбрать широкие ряды для гарнизона"
};
const char* cLuckInfo[KB_LUCK_INFO_TEXT_COUNT] = {


    "{Хорошая удача}\n\nЕсли удача вашего войска выше обычной, атаки отдельных отрядов на поле боя иногда оказываются более результативными (их сила удваивается).",


    "{Обычная удача}\n\nС обычной удачей ваше войско не имеет ни преимуществ, ни недостатков на поле боя.",


    "{Плохая удача}\n\nЕсли вашему войску не везет, урон, наносимый  отдельными отрядами на поле боя, может оказаться вдвое меньше обычного.",

    "%s\n\n\nМодификаторы удачи:",

    "\nЛапка кролика +1",

    "\nЗолотая подкова +1",

    "\nМонета +1",

    "\nКлевер +1",

    "\nПосещен Круг фейри +1",

    "\nПосещен фонтан +1",

    "\nНет",

    "\nГрабитель могил -1",

    "\nРадуга магов +2",

    "\nПосещен идол +1",

    "\nОграблена пирамида -2",

    "\nБазовая удача +1",

    "\nВысокая удача +2",

    "\nЭксперт удачи +3",

    "\nБонус мачты на море +1",

    "\nПосещена русалка +1",

    "\nБоевое одеяние Андурана дает максимальную удачу."
};
const char* IQnames[KB_IQ_NAME_COUNT] = {

    "Нет",

    "Глупый",

    "Средний",

    "Умный",

    "Гений"
};
const char* cSpellHelp[KB_SPELL_HELP_TEXT_COUNT] = {

    "Предыдущая страница ",

    "Следующая страница",

    "Небоевые заклинания",

    "Боевые заклинания",

    "Закрыть волшебную книгу",

    "Заклинания",

    "Выбрать заклинание",

    "Боевые заклинания",

    ("У вашего героя осталось %d оч. магии")
};
const char* speedText[KB_SPEED_TEXT_COUNT] = {
      "",
     "Ползает",
     "Оч. низкая",
     "Низкая",
     "Средняя",
     "Высокая",
     "Оч. высокая",
     "Ультра высокая",
     "Молниеносная",
     "Абсолютная"
};
const char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
     "Атака: ",
     "Защита: ",
      "Выстрелов: ",
      "Урон: ",
      "Здоровье: ",
      "Скорость: ",
      "Мораль: ",
      "Удача: ",
      "Выстрелов: "
};
const char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
     "Атака: ",
     "Защита: ",
      "Выстр.: ",
      "Урон: ",
      "ЗД: ",
      "Скор.: ",
      "Всего: ",
     "\n\nСкорость:\n%s",
     "\n\nПрирост\n + %d/нед."
};
const char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {

    ("Обзор королевства   Месяц: %d, Неделя: %d, День: %d"),
     "Ваш Драконий город.",
     "Ваш маяк."
};
const char* cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
     "%s, у вас осталось всего %d дней на то, чтобы завоевать хотя бы один город; иначе вы будете навеки изгнаны из страны.",
     "%s, настал последний день, когда вы еще можете завоевать себе город; в противном случае вы будете навеки изгнаны из страны.",
     "Астрологи объявляют месяц %s.\n\nНаселение всех жилищ возросло.",
     "Астрологи объявляют, что этому месяцу покровительствует сила %s.\n\nПопуляция %s удваивается!\n\nНаселение всех жилищ возросло.",
     "Астрологи объявляют месяц ЧУМЫ!\n\nНаселение всех жилищ уменьшилось вдвое.",
     "Астрологи объявляют неделю %s.\n\nНаселение всех жилищ возросло.",
     "Астрологи объявляют, что этой неделе покровительствует сила %s.\n\nПопуляция %s +5.\n\nНаселение всех жилищ возросло."
};
const char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
     "Атака: ",
     "Защита: ",
     "Сила магии: ",
     "Знания: ",
      "Мораль: ",
      "Удача: ",
     "Очки магии: "
};
const char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
     "Остановить катапульту",
     "Направить заклинание",
     "Отступить",
     "Сдаться",
     "Отменить",
     "Возможности героя",
     "Возможности капитана"
};
const char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
     "{Направить заклинание}\n\nНаправить заклинание. В течение каждого раунда боя можно направить лишь одно заклинание. Новый раунд начинается после того, как все отряды на поле боя завершили свой ход.",
     "{Отступить}\n\nГерой отступает с поля боя, бросив свое войско на произвол судьбы. Отступившего героя можно будет снова нанять на службу, но при этом сопровождать его будет лишь очень небольшая армия, как если бы ваш герой был зеленым новичком.",
     "{Сдаться}\n\nКапитуляция стоит денег. Тем не менее, если выкуп будет уплачен, героя можно будет снова нанять на службу вместе со всеми уцелевшими в битве войсками.",
     "{Отмена}\n\nВернуться в бой."
};
const char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
      "",
     "%s: Идти сюда.",
     "%s: Перелететь сюда.",
     "Атаковать %s",
     "Стрелять в %s (осталось %d выстр.)",
     "Возможности героя",
     "Вражеский герой",
     "%s: Показать информацию.",
     "Нет стрел!",
     "Возможности капитана",
     "Показать вражеского капитана",
     "Информация о баллисте"
};
const char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] =
    { "%s получает",   " уровень опыта.\n",   " %d уровней опыта.\n"};
const char* cCombatHelp[KB_COMBAT_HELP_COUNT] = {
     "Подождать, пока походят другие",
     "Пропустить ход этого воина",
     "Автобой",
     "Системные настройки",
      ""
};
const char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
     "{Ждать}\n\nДанный отряд откладывает свой ход и совершает действие после того, как все остальные отряды походили.",
     "{Пропустить ход}\n\nОтряд пропускает свой ход в этом раунде.",
     "{Автобой}\n\nКомпьютер вместо вас управляет вашими войсками во время боя.",
     "{Настройки}\n\nПозволяет изменять настройки боя.",
     "{Информационная строка}\n\nЗдесь отображаются результаты действий отдельных отрядов."
};
const char* cTownCommand[KB_TOWN_COMMAND_COUNT] = {
     "Разделить отряд %s",
      "Нельзя отнять последних воинов у героя ",
     "Соединить отряды %s",
     "Разделить отряд %s",
     "Посмотреть на %s",
     "Нельзя перенести в гарнизон последний отряд.",
     "Передвинуть сюда отряд %s",
     "Отряды %s и %s меняются местами",
     "Выйти из города",
      "",
     "Обзор королевства",
     "Пусто",
      "%s",
     "Показать героя",
     "Гильдия магов",
     "Гильдия воров",
     "Таверна",
     "Верфь",
     "Колодец",
     "Шатер",
     "Замок",
     "Нанять %s",
     "Статуя",
     "Левая башня",
     "Правая башня",
     "Ров",
     "Рынок",
     "Дом капитана"
};
const char* gHeroDefaultNames[GAME_HERO_COUNT] = {
     "Лорд Килбурн", "Сэр Галлант", "Эктор", "Гвеннет", "Тиро", "Амброзий", "Руби",
     "Максимус", "Димитри", "Сундакс", "Финеоз", "Джоджош", "Крэг Хак", "Джезебель",
     "Жаклин", "Эргон", "Тсабу", "Атлас", "Астра", "Наташа", "Троян",
     "Ватавна", "Ребекка", "Гем", "Ариэль", "Карлавн", "Луна", "Арий",
     "Аламар", "Виспер", "Кродо", "Барок", "Кастор", "Агар", "Фалагар",
     "Расмонт", "Мира", "Флинт", "Давн", "Галон", "Мирини", "Вилфрей",
     "Саракин", "Калиндра", "Мандигал", "Зом", "Дарлана", "Зам", "Ранлу",
     "Чарити", "Риалдо", "Роксана", "Сандро", "Келия"
};
const char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
     "{Уровень сложности}\n\nЭта опция позволяет устанавливать стартовый уровень сложности игры. Чем выше уровень сложности, тем с меньшим количеством ресурсов вы начинаете игру, и тем больше ресурсов получают ваши компьютерные противники.",
     "{Фора}\n\nЭта опция позволяет задавать тому или иному игроку-человеку дать фору другим игрокам. Если игрок дает другим фору, он начинает игру с меньшим количеством ресурсов и каждый ход получает на 15 или 30 процентов меньше ресурсов в зависимости от того, насколько большую фору он дает.",
     "{Оппоненты}\n\nЭта опция позволяет вам задать цвет игрока и его стартовую позицию. Каждому цвету соответствует определенная стартовая позиция. Некоторые цвета жестко закреплены либо за компьютерными, либо за живыми игроками.",
     "{Класс}\n\nЭта опция позволяет задавать класс игрока. Классы не всегда можно изменять. В зависимости от сценария игрок может получать дополнительные города и/или героев, направленность которых не совпадает с изначальной направленностью игрока.",
     "{Сценарий}\n\nЭта опция позволяет выбрать игровой сценарий.",
     "{Рейтинг}\n\nРейтинг отражает сочетание различных игровых установок. Он используется при расчете конечного результата, достигнутого игроком.",
     "{ОК}\n\nПодтверждает заданные установки и начинает новую игру.",
     "{Отмена}\n\nНажмите, чтобы вернуться в главное меню."
};
const char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
     "{2400 бод}\n\nИспользовать соединение на скорости 2400 бод.\n\nЗамечание: Для модемов 14400 бод используйте соединение на скорости 19200.  Для модемов 28800 бод используйте соединение на скорости 38400 бод.",
     "{9600 бод}\n\nИспользовать соединение на скорости 9600 бод.\n\nЗамечание: Для модемов 14400 бод используйте соединение на скорости 19200.  Для модемов 28800 бод используйте соединение на скорости 38400 бод.",
     "{19200 бод}\n\nИспользовать соединение на скорости 19200 бод.\n\nЗамечание: Для модемов 14400 бод используйте соединение на скорости 19200.  Для модемов 28800 бод используйте соединение на скорости 38400 бод.",
     "{38400 бод}\n\nИспользовать соединение на скорости 38400 бод.\n\nЗамечание: Для модемов 14400 бод используйте соединение на скорости 19200.  Для модемов 28800 бод используйте соединение на скорости 38400 бод.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
     "{COM 1}\n\nИспользовать для модемного соединения порт COM 1.",
     "{COM 2}\n\nИспользовать для модемного соединения порт COM 2.",
     "{COM 3}\n\nИспользовать для модемного соединения порт COM 3.",
     "{COM 4}\n\nИспользовать для модемного соединения порт COM 4.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
     "{Скорость соединения 2400 бод.}\n\nДля компьютеров с устаревшим чипом UART 8250 следует использовать скорость 19200 бод, а для компьютеров с более современным чипом UART 16550 - скорость 38400 бод. Если вы не уверены, какой у вас чип, начните с более низких скоростей. В большинстве компьютеров, произведенных в 1994 году и позднее, используется чип UART 16550.",
     "{Скорость соединения 9600 бод.}\n\n Для компьютеров с устаревшим чипом UART 8250 следует использовать скорость 19200 бод, а для компьютеров с более современным чипом UART 16550 - скорость 38400 бод. Если вы не уверены, какой у вас чип, начните с более низких скоростей. В большинстве компьютеров, произведенных в 1994 году и позднее, используется чип UART 16550.",
     "{Скорость соединения 19200 бод.}\n\n Для компьютеров с устаревшим чипом UART 8250 следует использовать скорость 19200 бод, а для компьютеров с более современным чипом UART 16550 - скорость 38400 бод. Если вы не уверены, какой у вас чип, начните с более низких скоростей. В большинстве компьютеров, произведенных в 1994 году и позднее, используется чип UART 16550.",
     "{Скорость соединения 38400 бод.}\n\n Для компьютеров с устаревшим чипом UART 8250 следует использовать скорость 19200 бод, а для компьютеров с более современным чипом UART 16550 - скорость 38400 бод. Если вы не уверены, какой у вас чип, начните с более низких скоростей. В большинстве компьютеров, произведенных в 1994 году и позднее, используется чип UART 16550.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
     "{COM 1}\n\nИспользовать для прямого соединения порт COM 1.",
     "{COM 2}\n\nИспользовать для прямого соединения порт COM 2.",
     "{COM 3}\n\nИспользовать для прямого соединения порт COM 3.",
     "{COM 4}\n\nИспользовать для прямого соединения порт COM 4.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
     "{2 игрока}\n\nИграть с 2 людьми и, опционально, до 4 дополнительных компьютерных игроков.",
     "{3 игрока}\n\nИграть с 3 людьми и, опционально, до 3 дополнительных компьютерных игроков.",
     "{4 игрока}\n\nИграть с 4 людьми и, опционально, до 2 дополнительных компьютерных игроков.",
     "{5 игроков}\n\n Играть с 5 людьми и, опционально, с 1 компьютерным игроком.",
     "{6 игроков}\n\n Играть с 6 людьми.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
     "{Сервер}\n\nСервер задает настройки игры. Может быть, только один хост в одном сетевом соединении.",

    ("{Гость}\n\nГость ожидает, пока сервер задаст настройки игры, после чего он автоматически вступит в игру."),
     "{Настройки}\n\nИзменить конфигурацию модема.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
     "{Сервер}\n\nСервер задает настройки игры.",

    ("{Гость}\n\nГость ожидает, пока сервер задаст настройки игры."),
     "{Настройки}\n\nИзменить конфигурацию модема.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
     "{За одной машиной}\n\nИграть за одной машиной, где от 2 до 4 игроков людей.",
     "{Локальная сеть}\n\nИграть по сети, где двое игроков играют по локальной сети, сидя за своими компьютерами.",
     "{Модем}\n\nДвое игроков играют через модемы сидя за своими компьютерами.",
     "{Прямое соединение}\n\nДвое игроков играют через ноль-модем сидя за своими компьютерами.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
     "{Сервер}\n\nОпределяет настройки игры. Может быть только один сервер в одном соединении.",
     "{Гость}\n\n Гость ожидает, пока сервер задаст настройки игры, после чего он автоматически вступит в игру. В игре через TCP/IP и IPX может быть несколько гостей. В игре через NetBIOS - только 1.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
     "{IPX}\n\nIPX является часто используемым сетевым протоколом для Windows. По IPX могут играть до 6 человек одновременно. Протокол IPX поддерживает только версия игры, работающая под Windows 95.",
     "{TCP/IP}\n\nПротокол TCP/IP наиболее часто используется для соединения компьютеров через Интернет. По TCP/IP могут играть до 6 человек одновременно. Протокол TCP/IP поддерживает только версия игры, работающая под Windows.",
     "{NETBios}\n\nПротокол NETBios является единственно возможным для компьютеров, работающим под DOS, но может быть использован и с Windows 95.  Этот протокол обеспечивает соединение не более двух игроков. Мы рекомендуем использовать протокол IPX.",
     "{Отмена}\n\nЗакрыть меню."
};
const char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
     "{Обычная игра}\n\nОдиночная игра на отдельной карте.",
     "{Кампания}\n\nОдиночная игра на серии карт.",
     "{Сетевая игра}\n\nСетевая игра, где несколько игроков-людей сражаются друг против друга на одной карте.",
     "{Отменить}\n\nОтменить и вернуться в главное меню."
};
const char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
     "Враг сдался!",
     "Враг повержен!",
     "Великая победа!",
     "\n\nЗа мужество, проявленное в бою, %s получает %d оч. опыта.",
     "%s сдается врагу и отступает с позором.",
     "%s трусливо бежит с поля боя.",
     "Ваши войска потерпели поражение и %s покидает вас.",
     "Ваши силы сдались врагу и отступили с позором.",
     "Ваши трусливые войска бежали с поля боя.",
     "Ваши войска потерпели поражение.",
     "\n\nЗа мужество, проявленное в бою, %s получает %d оч. опыта, и получает %d уровень(я)."
};
const char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
     "{Высокая мораль}\n\nВысокая мораль может дать в бою вашим бойцам дополнительную атаку.",
     "{Обычная мораль}\n\nС обычной моралью ваши армии никогда не получат дополнительную атаку и не будут прокляты.",
     "{Плохая мораль}\n\nПлохая мораль может привести к потере хода в бою вашими бойцами.",
     "%s\n\n\nМодификаторы морали:",
     "\nБонус рыцаря +1",
     "\n%s со своей армией +1",
     "\nВоины 3 рас -1",
     "\nВоины 4 рас -2",
     "\nМедаль отваги +1",
     "\nМедаль мужества +1",
     "\nМедаль доблести +1",
     "\nМедаль почета +1",
     "\nСимвол неудачи -2",
     "\nПосещен буй +1",
     "\nПосещен оазис +1",
     "\nПосещен храм +2",
     "\nРасхититель гробниц -1",
     "\nРасхититель обломков -1",
     "\nТрусость в бою %d",
     "\nНет",
     "\nВоины 5 рас -3",
     "\nВся армия одна нежить, мораль не важна.",
     "\nВ армии нежить -1",
     "\nПосещена промоина +1",
     "\nРасхититель кораблей -1",
     "\nКолизей варваров +2",
     "\nТаверна +1",
     "\nЛидерство 1-й ступени +1",
     "\nЛидерство 2-й ступени +2",
     "\nЛидерство 3-й ступени +3",
     "\nБонус мачты на море +1",
     "\nБоевое одеяние Андурана дает максимальную мораль."
};
const char* cMapSize[KB_MAP_SIZE_TEXT_COUNT] = { "Маленькая", "Средняя", "Большая", "Огромная"};
const char* cDifficulty[(DIFFICULTY_COUNT)] =
    { "Легкая", "Обычная", "Высокая", "Эксперт", "Невозможно!"};
const char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] = { "Легкая", "Обычная", "Тяжелая", "Эксперт"};
const char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] =
    { "Лорд Айронфист", "Лорд Слэйер", "Королева Ламанда", "Лорд Аламар"};
const char* cWinText[KB_WIN_TEXT_COUNT] =
    { "Дней:", "Очки:", "Сложность:", "Счет:", "Ранг:"};
const char* cHumanDifficulty[(DIFFICULTY_COUNT)] =
    { "Человек\n", "Человек\nЛегкая игра", "Человек\nОбычная игра", "Человек\nТяжелая игра", "Человек\nЭксперт"};
const char* cHumanInfoDifficulty[(DIFFICULTY_COUNT)] =
    { "Чел.-", "Чел.-Легкая игра", "Чел.-Обычная игра", "Чел.-Тяжелая игра", "Чел.-Эксперт"};
const char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] =
    {  "MIDI", "CD-стерео без вокала", "CD-стерео с вокалом"};
const char* gSpellDesc[(SPELL_COUNT)] = {
     "{Огненный шар}\n\nОгромный огненный шар взрывается над выбранным участком поля боя, поражая все находящиеся поблизости отряды.",
     "{Огненный удар}\n\nУсовершенствованный вариант огненного шара. Огненный удар поражает отряды, находящиеся в радиусе не одного, а двух полей от эпицентра.",
     "{Молния}\n\nМощный электрический разряд поражает выбранный отряд противника.",
     "{Цепь молний}\n\nЭлектрический разряд поражает выбранный отряд противника, затем ближайший к нему отряд с половинной силой, затем следующий отряд c еще вдвое меньшей силой, и так далее до тех пор, пока не уходит в землю. Будьте осторожны: это заклинание может поразить и ваши собственные отряды!",
     "{Телепорт}\n\nМгновенно перемещает выбранный отряд в любую свободную точку на поле боя.",
     "{Лечение}\n\nНейтрализует все враждебные заклинания, примененные против одного из ваших отрядов и восстанавливает по 5 единиц здоровья в расчете на каждый уровень магических способностей героя.",
     "{Общее лечение}\n\nНейтрализует враждебные заклинания, примененные против всех ваших отрядов и восстанавливает по 5 единиц здоровья у каждого существа за каждый уровень магических способностей героя.",
     "{Воскрешение}\n\nДо конца сражения воскрешает воинов в отряде, которому был нанесен урон.",
     "{Истинное воскрешение}\n\nНавсегда воскрешает воинов в отряде, которому был нанесен урон.",
     "{Ускорение}\n\nУвеличивает дальность передвижения любого отряда на 2 единицы.",
     "{Общее ускорение}\n\nУвеличивает дальность передвижения всех ваших отрядов на 2 единицы.",
     "{Замедление}\n\nВдвое уменьшает дальность передвижения выбранного отряда противника.",
     "{Общее замедление}\n\nВдвое снижает дальность перемещения всех отрядов противника.",
     "{Ослепление}\n\nЗатуманивает взоры воинов выбранного отряда и тем самым не позволяет им перемещаться по полю боя.",
     "{Благословение}\n\nУвеличивает до максимума урон, наносимый выбранным отрядом.",
     "{Общее благословение}\n\nУвеличивает до максимума урон, наносимый всеми вашими отрядами.",
     "{Каменная кожа}\n\nВолшебным образом повышает защищенность выбранного отряда.",
     "{Стальная кожа}\n\nПовышает защищенность выбранного отряда. Усовершенствованный вариант заклинания Каменная кожа.",
     "{Проклятие}\n\nУменьшает до минимума урон, причиняемый выбранным отрядом противника.",
     "{Общее проклятие}\n\nУменьшает до минимума урон, причиняемый всеми отрядами противника.",
     "{Святое слово}\n\nНаносит урон всей нежити на поле боя.",
     "{Святой глас}\n\nНаносит урон всей нежити на поле боя. Усовершенствованный вариант заклинания Святое слово.",
     "{Антимагия}\n\nЗащищает выбранный отряд от враждебных заклинаний.",
     "{Снятие чар}\n\nСнимает все чары с выбранного отряда.",
     "{Общее снятие чар}\n\nСнимает все чары со всех отрядов.",
     "{Волшебная стрела}\n\nВолшебная стрела поражает выбранный отряд противника.",
     "{Берсерк}\n\nЗаставляет выбранный отряд противника нападать на ближайший к нему отряд.",
     "{Армагеддон}\n\nУжасный катаклизм обрушивается на поле боя, нанося жестокий урон всем участникам сражения.",
     "{Буря стихий}\n\nСилы стихий обрушиваются на поле боя, нанося урон всем участникам сражения.",
     "{Звездопад}\n\nЗвездопад поражает выбранный участок поля боя, нанося урон всем находящимся поблизости участникам сражения.",
     "{Паралич}\n\nОтряд, против которого направлено это заклинание, поражает паралич, и он теряет способность передвигаться или отвечать на удары.",
     "{Гипноз}\n\nВыбранный отряд противника переходит под контроль вашего героя на один ход, если его суммарное здоровье не превышает магических способностей героя, умноженных на 25.",
     "{Хладный луч}\n\nВысасывает тепло жизни из выбранного отряда противника.",
     "{Кольцо стужи}\n\nВысасывает тепло жизни из всех отрядов вокруг эпицентра заклинания, за исключением находящегося в самом эпицентре.",
     "{Разрушительный луч}\n\nПонижает защиту выбранного отряда противника на 3 единицы.",
     "{Дрожь смерти}\n\nНаносит урон всем отрядам живых воинов в сражении, но не действует на нежить.",
     "{Волна смерти}\n\nНаносит урон всем отрядам живых воинов в сражении, но не действует на нежить. Усовершенствованный вариант заклинания Дрожь смерти.",
     "{Убийца драконов}\n\nЗначительно увеличивает урон, наносимый выбранным отрядом в бою против драконов.",
     "{Жажда крови}\n\nУвеличивает урон, наносимый выбранным отрядом.",
     "{Поднять мертвых}\n\nНавсегда \"воскрешает\" из раненных или уничтоженных отрядов нежити.",
     "{Фантом}\n\nЗаклинание создает призрачный отряд, который является двойником существующего отряда. Призрачный отряд наносит противнику такой же урон, как и настоящий, но исчезает, если ему был нанесен хотя бы минимальный урон.",
     "{Щит}\n\nВдвое уменьшает урон, получаемый выбранным отрядом от стрелковых атак противника.",
     "{Общий щит}\n\nВдвое уменьшает урон, получаемый всеми отрядами от стрелковых атак противника.",
     "{Земной элементал}\n\nЗаклинание вызывает отряд элементалов земли, которые присоединяются к вашей армии.",
     "{Воздушный элементал}\n\nЗаклинание вызывает отряд элементалов воздуха, которые присоединяются к вашей армии.",
     "{Огненный элементал}\n\nЗаклинание вызывает отряд элементалов огня, которые присоединяются к вашей армии.",
     "{Водный элементал}\n\nЗаклинание вызывает отряд элеманталов воздуха, которей присоединяются к вашей армии.",
     "{Землетрясение}\n\nНаносит ущерб крепостным стенам.",
     "{Показать шахты}\n\nДелает видимыми все шахты на игровой карте.",
     "{Показать ресурсы}\n\nПоказывает все ресурсы на игровой карте.",
     "{Показать артефакты}\n\nДелает видимыми все артефакты на игровой карте.",
     "{Показать города}\n\nДелает видимыми все города и замки на игровой карте.",
     "{Показать героев}\n\nДелает видимыми всех героев на игровой карте.",
     "{Показать все}\n\nДелает видимыми все объекты на игровой карте.",
     "{Опознать героев}\n\nПозволяет получить подробную информацию о героях противника.",
     "{Призвать корабль}\n\nПеремещает ваш ближайший незанятый корабль в ближайшую к вам точку побережья. Вашим считается корабль, который вы только что построили, либо тот, на котором вы плавали последним.",
     "{Портал}\n\nПереносит героя в расположенную поблизости точку на карте.",
     "{Врата города}\n\nПереносит героя в ближайший принадлежащий игроку город или замок.",
     "{Портал города}\n\nПереносит героя в принадлежащий игроку город или замок по его выбору.",
     "{Виденье}\n\nЭто заклинание позволяет предсказать вероятный исход встречи с нейтральной армией.",
     "{Запустение}\n\nНаводняет принадлежащую игроку шахту призраками, после чего она перестает производить ресурсы. (Не доставайся же ты никому!)",
     "{Стража земли}\n\nОтряд земных элементалов охраняет шахту от нападения армий противника.",
     "{Стража воздуха}\n\nОтряд воздушных элементалов охраняет шахту от нападения армий противника.",
     "{Стража огня}\n\nОтряд огненных элементалов охраняет шахту от нападения армий противника.",
     "{Стража воды}\n\nОтряд водных элементалов охраняет шахту от нападения армий противника."
};
const char* gSpellNames[(SPELL_COUNT)] = {
     "Огненный шар",
     "Огненный взрыв",
     "Молния",
     "Цепь молний",
     "Телепорт",
     "Лечение",
     "Общее лечение",
     "Воскрешение",
     "Истинное воскрешение",
     "Ускорение",
     "Общее ускорение",
     "Замедление",
     "Общее замедление",
     "Ослепление",
     "Благословение",
     "Общее благословение",
     "Каменная кожа",
     "Стальная кожа",
     "Проклятие",
     "Общее проклятие",
     "Святое слово",
     "Святой глас",
     "Антимагия",
     "Снятие чар",
     "Общее снятие чар",
     "Волшебная стрела",
     "Берсерк",
     "Армагеддон",
     "Буря стихий",
     "Звездопад",
     "Паралич",
     "Гипноз",
     "Хладный луч",
     "Кольцо стужи",
     "Разрушительный луч",
     "Дрожь смерти",
     "Волна смерти",
     "Убийца драконов",
     "Жажда крови",
     "Поднять мертвых",
     "Фантом",
     "Щит",
     "Общий щит",
     "Земной элементал",
     "Воздушный элементал",
     "Огненный элементал",
     "Водный элементал",
     "Землетрясение",
     "Показать шахты",
     "Показать ресурсы",
     "Показать артефакты",
     "Показать города",
     "Показать героев",
     "Показать все",
     "Опознать героев",
     "Призвать корабль",
     "Портал",
     "Врата города",
     "Портал города",
     "Виденье",
     "Запустение",
     "Страж земли",
     "Страж воздуха",
     "Страж огня",
     "Страж воды"
};
const char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] =
    { "1 ступени", "2 ступени", "3 ступени"};
const char* gSecondarySkills[(HERO_SKILL_COUNT)] = {
     "Следопыт",
     "Стрелок",
     "Логистика",
     "Разведка",
     "Дипломатия",
     "Навигация",
     "Лидерство",
     "Мудрость",
     "Мистицизм",
     "Удача",
     "Баллистика",
     "Орлиный взор",
     "Некромантия",
     "Казначей"
};
const char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
     "Гильдия магов",
     "Гильдия воров",
     "Таверна",
     "Верфь",
     "Колодец",
     "Шатер",
     "Замок",
     "Статуя",
     "Левая башня",
     "Правая башня",
     "Рынок",
      "",
     "Ров",
      "",
     "Док с кораблем",
     "Дом капитана",
      "",
      "",
      ""
};
const char* gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT] = {
     "Ферма",
     "Свалка истории",
     "Хрустальный сад",
     "Водопад",
     "Фруктовый сад",
     "Груда черепов",
     "Прирост воинов 1 ур."
};
const char* gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT] =
    { "Укрепления", "Колизей", "Радуга", "Подземелье", "Библиотека", "Шторм", "Специальная"};
const char* gDwellingNames[(FACTION_COUNT)][DWELLING_TYPE_COUNT] = {
    {"Мазанка",
     "Стрельбище",
     "Кузница",
     "Оружейная",
     "Ристалище",
     "Собор",
     "Полигон",
     "Ковальня",
     "Арсенал",
     "Арена",
     "Храм",
       ""},
    {"Хижина",
     "Халупа",
     "Логово",
     "Дом огров",
     "Мост",
     "Пирамида",
     "Хибара",
       "",
     "Логово огров",
     "Царь-мост",
       "",
       ""},
    {"Древо-дом",
     "Избушка",
     "Стрельбище",
     "Стоунхендж",
     "Загон",
     "Алая башня",
     "Хоромы",
     "Полигон",
     "Менгиры",
       "",
       "",
       ""},
    {"Пещера",
     "Крипта",
     "Гнездо",
     "Лабиринт",
     "Болото",
     "Зеленая башня",
       "",
       "",
     "Большой лабиринт",
       "",
     "Красная башня",
     "Черная башня"},
    {"Нора",
     "Хлев",
     "Литейный цех",
     "Гнездовье",
     "Башня магов",
     "Небесный замок",
       "",
     "Фабрика",
       "",
     "Обитель магов",
     "Небесный чертог",
       ""},
    {"Могильник",
     "Кладбище",
     "Пирамида",
     "Особняк",
     "Мавзолей",
     "Лаборатория",
     "Погост",
     "Великая пирамида",
     "Цитадель",
     "Некрополь",
       "",
       ""}
};
const char* cSecSkillDesc[(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
    { "{Следопыт 1 ступени}\n\nУменьшает замедление при передвижении по пересеченной местности на 25 процентов.",
      "{Следопыт 2 ступени}\n\nУменьшает замедление при передвижении по пересеченной местности на 50 процентов.",
      "{Следопыт 3 ступени}\n\nПолностью нейтрализует замедление при передвижении по пересеченной местности."},
    { "{Стрелок 1 ступени}\n\nУвеличивает на 10 процентов урон, наносимый стреляющими отрядами.",
      "{Стрелок 2 ступени}\n\nУвеличивает на 25 процентов урон, наносимый стреляющими отрядами.",
      "{Стрелок 3 ступени}\n\nУвеличивает на 50 процентов урон, наносимый стреляющими отрядами."},
    { "{Логистика 1 ступени}\n\nУвеличивает запас движения героя на 10 процентов.",
      "{Логистика 2 ступени}\n\nУвеличивает запас движения героя на 20 процентов.",
      "{Логистика 3 ступени}\n\nУвеличивает запас движения героя на 30 процентов."},
    { "{Разведка 1 ступени}\n\nУвеличивает на 1 клетку радиус обзора героя.",
      "{Разведка 2 ступени}\n\nУвеличивает на 2 клетки радиус обзора героя.",
      "{Разведка 3 ступени}\n\nУвеличивает на 3 клетки радиус обзора героя."},
    { "{Дипломатия 1 ступени}\n\nПозволяет вести переговоры с отрядами монстров, более слабыми, чем ваша армия. На таком уровне дипломатии к вам может присоединиться до 1/4 отряда монстров.",
      "{Дипломатия 2 ступени}\n\nПозволяет вести переговоры с отрядами монстров, более слабыми, чем ваша армия. На таком уровне дипломатии к вам может присоединиться до 1/2 отряда монстров.",
      "{Дипломатия 3 ступени}\n\nПозволяет вести переговоры с отрядами монстров, более слабыми, чем ваша армия. На таком уровне дипломатии к вам может присоединиться весь отряд монстров."},
    { "{Навигация 1 ступени}\n\nУвеличивает на 1/3 запас движения героя при передвижении по воде.",
      "{Навигация 2 ступени}\n\nУвеличивает на 2/3 запас движения героя при передвижении по воде.",
      "{Навигация 3 ступени}\n\nУдваивает запас движения героя при передвижении по воде."},
    { "{Лидерство 1 ступени}\n\nУвеличивает на 1 единицу мораль войск вашего героя.",
      "{Лидерство 2 ступени}\n\nУвеличивает на 2 единицы мораль войск вашего героя.",
      "{Лидерство 3 ступени}\n\nУвеличивает на 3 единицы мораль войск вашего героя."},
    { "{Мудрость 1 ступени}\n\nПозволяет вашему герою изучать заклинания третьего уровня.",
      "{Мудрость 2 ступени}\n\nПозволяет вашему герою изучать заклинания четвертого уровня.",
      "{Мудрость 3 ступени}\n\nПозволяет вашему герою изучать заклинания пятого уровня."},
    { "{Мистицизм 1 ступени}\n\nВаш герой восстанавливает по 2 очка магии в день.",
      "{Мистицизм 2 ступени}\n\nВаш герой восстанавливает по 3 очка магии в день.",
      "{Мистицизм 3 ступени}\n\nВаш герой восстанавливает по 4 очка магии в день."},
    { "{Удача 1 ступени}\n\nУвеличивает на 1 удачу вашего героя.",
      "{Удача 2 ступени}\n\nУвеличивает на 2 удачу вашего героя.",
      "{Удача 3 ступени}\n\nУвеличивает на 3 удачу вашего героя."},
    { "{Баллистика 1 ступени}\n\nУвеличивает точность стрельбы  катапульты вашего героя и урон, наносимый крепостным стенам.",
      "{Баллистика 2 ступени}\n\nКатапульта вашего героя делает дополнительный выстрел; при этом увеличивается точность ее стрельбы и урон, наносимый крепостным стенам.",
      "{Баллистика 3 ступени}\n\nКатапульта вашего героя делает дополнительный выстрел; при этом каждый выстрел разрушает любую стену, за исключением укрепленных стен рыцарского замка."},
    { "{Орлиный взор 1 ступени}\n\nДает вашему герою 20-процентный шанс выучить любое заклинание первого или второго уровней, примененное против него в бою.",
      "{Орлиный взор 2 ступени}\n\nДает вашему герою 30-процентный шанс выучить любое заклинание третьего или более низких уровней, примененное против него в бою.",
      "{Орлиный глаз 3 ступени}\n\nДает вашему герою 40-процентный шанс выучить любое заклинание четвертого или более низких уровней, примененное против него в бою."},
    { "{Некромантия 1 ступени}\n\nВоскрешает 10 процентов существ, павших на поле боя, и превращает их в скелеты для вашей армии.",
      "{Некромантия 2 ступени}\n\nВоскрешает 20 процентов существ, павших на поле боя, и превращает их в скелеты для вашей армии.",
      "{Некромантия 3 ступени}\n\nВоскрешает 30 процентов существ, павших на поле боя, и превращает их в скелеты для вашей армии."},
    { "{Казначей 1 ступени}\n\nВаш герой ежедневно собирает со своих владений налоги в размере 100 золотых.",
      "{Казанчей 2 ступени}\n\nВаш герой ежедневно собирает со своих владений налоги в размере 250 золотых.",
      "{Казначей 3 ступени}\n\n Ваш герой ежедневно собирает со своих владений налоги в размере 500 золотых."}
};
const char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
     "Гильдия магов позволяет разучивать новые заклинания и восстанавливает запас очков магии.",
     "Гильдия воров дает информацию о врагах. Также, Гильдия воров дает разведывательную информацию о вражеских городах. Дополнительные гильдии дают дополнительную информацию.",
     "Таверна увеличивает мораль бойцов, защищающих замок.",
     "Верфь позволяет строить корабли.",
     "Колодец увеличивает прирост всех воинов на 2 в неделю.",
     "Шатер дает рабочих, которые могут возвести замок.",
     "Замок улучшает защиту города и увеличивает доход до 1000 золотых в день.",
     "Статуя увеличивает доход города на 250 золотых в день.",
     "Левая башня обеспечивает в бою дополнительную огневую мощь замку.",
     "Правая башня обеспечивает в бою дополнительную огневую мощь замку.",
     "Рынок можно использовать для перевода одного типа ресурсов в другой. Чем больше рынков вы контролируете, тем выгодней цена.",
      "",
     "Ров замедляет атаку вражеских воинов. Любой воин, вошедший в ров, окончит тут свое движение и станет более уязвимым для атаки.",
      "",
     "Верфь позволяет строить корабли.",
     "Дом капитана позволяет капитану городской стражи организовать защиту замка в отсутствии героя.",
      "",
      "",
      ""
};
const char* gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT] = {
     "Укрепления увеличивают прочность стен, увеличивая число раундов, необходимых для полного их разрушения.",
     "Представления, проходимые в Колизее, увеличивают мораль защитников замка на 2 единицы.",
     "Радуга увеличивает удачу защитников замка на 2 единицы.",
     "Подземелье увеличивает доход города на 500 золотых в день.",
     "Библиотека увеличивает число заклинаний, доступных в Гильдии на 1 на каждый ее этаж.",
     "Шторм добавляет +2 единицы к силе заклинаний защитников замка."
};
const char* cDirections[KB_DIRECTION_TEXT_COUNT] = {
     "севернее",
     "северо-восточнее",
     "восточнее",
     "юго-восточнее",
     "южнее",
     "юго-восточнее",
     "западнее",
     "северо-западнее",
     "в центре"
};
const char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
     "Темные пучины океана",
     "Зеленые равнины",
     "Глубокие снега",
     "Топкие болота",
     "Застывшая лава",
     "Бескрайние пески",
     "Грязь",
     "Бесплодная пустошь",
     "Побережье"
};
const char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] = { "Разный", "'Добрый'", "'Злой'"};
const char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = { "Монохром", "Цветной"};
const char* combatSpeedText[KB_COMBAT_SPEED_COUNT] = { "Обычная", "Высокая", "Оч. высокая"};
const char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] = { "Нет", "Только чары", "Полная"};
const char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
      "\n\n\n***Command Line Help***\n",
      "\n",
     "/D0 - отключить цифровой звук\n",
     "/M0 - отключить MIDI музыку\n",
     "/R0 - отключить музыку\n",
     "/I0 - пропустить интро\n",
      "\n",
      "\n",
     "Пример:\n",
      "\n",
      "HEROES2D /R0 /I0\n",
      "\n",
     "Загрузить DOS версию Героев 2.\n",
     "Звук отключен и интро пропущено.\n"
};
const char* cOverviewText[KB_OVERVIEW_TEXT_COUNT] =
    { "Герой/Параметры", "Навыки", "Артефакты", "Города/Замки", "Гарнизон", "Доступно"};
const char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
     "Ошибка передачи данных при выполнении функции %s\n\nКод ошибки: %d\nЗначение ошибки: %s\n\n",
     "Предлагаемые меры устранения ошибки:",
     "\n1) Убедитесь в надежности подсоединения кабелей.",
     "\n2) Перезагрузите компьютер.",
     "\n3) Убедитесь в том, что в 'CONFIG' задан правильный COM порт. (Третья кнопка на экране, где вы выбираете Хозяина или Гостя.)",
     "\n4) Попробуйте уменьшить скорость передачи данных в 'CONFIG' до 19200 или до 9600."
};
const char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] =
    { "%d воинов", "%d воин", "Атака", "Защита", "ЗД", "Урон", "МР", "УЧ", "Выстр."};
const char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
     "{Маленькие карты}\n\nПросмотр только маленьких карт (36 x 36).",
     "{Средние карты}\n\nПросмотр только средних карт (72 x 72).",
     "{Большие карты}\n\nПросмотр только больших карт (108 x 108).",
     "{Очень большие карты}\n\nПросмотр только очень больших карт (144 x 144).",
     "{Все карты}\n\nПросмотр всех карт.",
     "{Ввод имени}\n\nВведите имя файла, под которым выхотите сохранить игру.",
     "{ОК}\n\nПодтверждение выбора.",
     "{Отмена}\n\nОтмена без подтверждения выбора.",
     "{Значок размера}\n\nОбозначает размер карты: маленькая (36 x 36), средняя (72 x 72), большая (108 x 108) или очень большая (144 x 144).",
     "{Значок игроков}\n\nОбозначает количество игроков в данном сценарии. При отсутствии игроков-людей их места занимает компьютер.",
     "{Условия победы}\nПредусмотрено 6 возможных вариантов:\n{Надгробный камень} - Разгромить всех героев противника и захватить его замки.\n{Город} - Захватить определенный замок.\n{Портрет героя} - Разгромить определенного героя.\n{Медаль} - Найти определенный артефакт.\n{Рукопожатие} - Ваш альянс должен разгромить альянс противника.\n{Монеты} - Накопить нужное количество золота.",
     "{Уссловия поражения}\n\nПредусмотрено 4 возможных условия:\n{Надгробный камень} - Потеря всех ваших героев и городов.\n{Город} - Потеря определенного замка.\n{Портрет героя} - Потеря указанного героя.\n{Песочные часы} - Победа не была достигнута до указанного срока.)",
     "{Название}\n\nНазвание карты.",
     "{Описание}\n\nОписание карты.",
     "{Трудность карты}\n\nСтепень сложности игры на этой карте. Трудность карты определяется разработчиком сценария. Более сложные карты характеризуются большим числом сильных противников, меньшим количеством ресурсов или специальными условиями, затрудняющими достижение победы."
};
const char* cPersonality[KB_PERSONALITY_TEXT_COUNT] = { "Воин", "Строитель", "Исследователь", "Человек"};
const char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
    { "Мало", "Мало", "мало"},
    { "Немного", "Немного", "немного"},
    { "Стая", "Стая", "стая"},
    { "Много", "Много", "много"},
    { "Орда", "Орда", "орда"},
    { "Толпа", "Толпа", "толпа"},
    { "Свора", "Свора", "свора"},
    { "Тысячи", "Тысячи...", "тысячи"},
    { "Легион", "Легион", "легион"}
};
const char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
     "Истина где-то рядом.",
     "Темная сторона сильнее.",
     "Конец Света близок.",
     "Прах Лорда Слэйера захоронен в основании арены.",
     "Он невиновен.",
     "Черный дракон сделает Титана в любой день недели.",
     "Он сказал ей, \"Я-да-да-яда-да\"... а она сказала, \"Ля-ля-ля, ля-ля-ля...\"",
     "Тут бывал человек из Нунтукета..."
};
const char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] =
    { "Прямо пойдешь - коня потеряешь.", "Сдается в аренду.", "До следующего знака 50 миль.", "Кто идет за Блинским?"};
const char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
     "Альянс гномов",
     "Гильдия колдуний",
     "Роланд становится сильнее",
     "Перенос войск",
     "Корлагон побежден",
     "Корона всевластия",
     "Гильдия некромантов",
     "Смерть гномам",
     "Союз огров",
     "Союз драконов",
     "Корона всевластия",
     "Перенос войск"
};
const char* cCampaignName[(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    { "Сила оружия",
      "Аннексия",
      "Спасти гномов",
      "Копи Каратора",
      "Переломный момент",
      "Защитник",
      "Вызов брошен!",
      "Корона",
      "Акт отчаяния",
      "Час нашей славы",
       "",
      "Предательство"},
    { "Первая кровь",
      "Войны с варварами",
      "Некроманты!",
      "Смерть гномам",
      "Переломный момент ",
      "Крестьяне!",
      "Владыка драконов",
      "Лорды провинций",
      "Корона",
      "К вящей славе",
      "Апокалипсис",
      "Предательство!"}
};
const char* cCampaignDescription[(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
    { "Прежде чем поднять восстание против брата, Роланд хочет, чтобы вы одержали победу над соседними властителями. Между ними нет единства, поэтому большую часть времени они будут заняты стычками друг с другом. Победа будет вашей, когда вы захватите все города.",
      "Властители сопредельных земель отказываются принести клятву верности Роланду, и поэтому должны быть повержены. Богатства и власти им не занимать, поэтому будьте готовы к нелегкой борьбе. Чтобы победить, захватите все вражеские замки.",
      "Вам следует защитить гномов от армий Арчибальда. Чтобы победить, захватите все города и замки противника. Следите за тем, чтобы враг не захватил все города гномов, иначе победа достанется ему.",
      "В борьбе за ресурсы и сокровища вам противостоят четыре противника, объединившихся в союз. Чтобы победить, захватите все вражеские замки.",
      "Ваши враги заключили против вас союз. Они где-то рядом, поэтому в любой момент будьте готовы к битве. Вы победите, когда завладеете всеми четырьмя замками, находящимися в этой небольшой долине.",
      "Гильдия колдунов славного города Норастона попросила Роланда помочь ей отбиться от союзников Арчибальда. Чтобы победить, вы должны захватить все вражеские замки. Не потеряйте Норастон, иначе вы проиграли. (Один из вражеских замков на острове в океане).",
      "Соберите армию побольше и захватите замок противника не позднее, чем через 8 недель. Вам противостоит всего один противник, но до его замка скакать и скакать. Все войска, которые останутся у вас к концу этого сценария, примут участие в заключительной битве.",
      "Найдите корону, прежде чем это сделают герои Арчибальда. Корона понадобится Роланду для победы в заключительной битве.",
      "Три противника, и среди них сам лорд Корлагон, заключили союз и стоят между вами и великой победой. Роланд обосновался в замке на северо-западе, и если этот замок падет, вы проиграете. Если вы захватите Корлагона сейчас, он не будет драться против вас в последней битве.",
      "Итак, пробил час последнего и решительного боя. И вы, и ваши противники вооружены до зубов, и все кругом объединились против вас. Война будет закончена, когда вы захватите в плен Арчибальда!",
       "",
      "Вы сменили сюзерена, и теперь у вас три замка против одного у противника. Эта миссия будет для вас самой легкой во всей войне... Предатель!"},
    { "Король Арчибальд требует, чтобы вы разгромили трех противников, которые обосновались в этих землях. Они не связаны между собой союзным договором, поэтому по большей части они будут тратить силы на вражду друг с другом. Вы победите, когда все их замки окажутся в ваших руках.",
      "Вам предстоит объединить племена северных варваров, предварительно усмирив их. Как и в предыдущей миссии, противники не состоят в союзе друг с другом, но у них больше ресурсов. Победа будет вашей, когда вы захватите все вражеские замки и перебьете всех героев противника.",
      "Добрые волшебники захватили замок некромантов. Чтобы победить, вы должны отобрать его обратно. Помните, что хотя вы и начинаете с сильной армией, в самом начале у вас нет своего замка. Вы должны заиметь его за 7 дней, иначе все потеряно. (Ближайший замок на юго-востоке).",
      "Гномов следует привести к покорности, прежде чем они смогут помешать планам короля Арчибальда. Под знаменами Роланда много героев, у него несколько замков, поэтому будьте готовы к нападению сразу с нескольких сторон. Вам надо захватить все города противника.",
      "Ваши противники объединились против вас и притаились неподалеку, поэтому будьте начеку. Победа будет вашей, когда вы завладеете всеми четырьмя замками, находящимися в этой небольшой долине.",
      "Вам предстоит подавить крестьянский бунт, во главе которого стоят агенты Роланда. Все ваши соседи объединились против вас, но на вашей стороне лорд Корлагон - опытный и сильный боец. Чтобы победить, вы должны захватить все замки противника.",
      "В этой миссии вам противостоят два противника. Оба хорошо вооружены и полны решимости выставить вас со своего острова. Избегая встречи с ними, захватите Драконий город - тогда победа будет за вами.",
      "Вам приказано разгромить удельных властителей, которые присягнули на верность Роланду. Все вражеские замки объединились и выступают против вас. Вы начинаете игру без замка. Вам надо захватить замок за 7 дней. Победа будет вашей, когда все замки противника падут.",
      "Найдите корону, пока ею не завладели герои Роланда. Корона понадобится Арчибальду для победы в заключительной битве с Роландом.",
      "Соберите армию побольше и захватите замок противника не позднее, чем через 8 недель. Вам противостоит всего один противник, но до его замка скакать и скакать. Все войска, которые останутся у вас к концу этого сценария, будут с вами в заключительной битве.",
      "Итак, пробил час последней битвы. И вы, и ваши противники вооружены до зубов, и все объединились против вас. Война закончится, когда вы захватите в плен Роланда, и смотрите, не потеряйте Арчибальда в пылу битвы!",
      "Вы сменили сюзерена, и теперь у вас три замка против одного у противника. Эта миссия будет для вас самой легкой во всей войне... Предатель!"}
};
const char* cOutOfMemory =
     "\n\n\n\n\n\n\n\n\n\n\n\n\n\n%s\nГероям II требуется минимум \n%dK Расширенной  памяти (XMS) и\n480K общей памяти.\n\n";
const char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = { "Обычное", "Черес-\nстрочное"};
const char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
     "{ОК}\n\nЗакрыть меню.",
     "{Музыка}\n\nВключить или выключить фоновую музыку.",
     "{Эффекты}\n\nВключить или выключить звуковые эффекты.",
     "{Скорость}\n\nВыбрать скорость передвижения героев по карте.",
     "{Качество звука}\n\nВыбрать формат музыки. Как правило, музыка в формате MIDI не отличается качеством, но она предъявляет меньшие требования к производительности системы, чем формат Стерео CD. Формат Стерео CD дает  возможность воспроизводить оперную музыку.",
     "{Показывать путь}\n\nВключить или выключить отображение пути героя на карте.  Если опция включена, первое нажатие по объекту на карте показывает путь к этому объекту, а по второму нажатию левой кнпоки мыши начинается движение. Если эта опция отключена, движение начинается по первому нажатию.",
     "{Скорость врага}\n\nВыбрать скорости перемещения героев, управляемых компьютером. При этом можно выбрать режим, в котором не будет отображаться передвижение противника.",
     "{Интерфейс}\n\nВыбор желаемого типа интерфейса. По умолчанию задан динамический интерфейс, в котором 'злое' графическое оформление используется для трех 'злых' классов героев (варвара, чернокнижника и некроманта).",
     "{Быстрый бой}\n\nПри включении этой опции перед каждым сражением компьютер будет делать запрос о проведении этого сражения в режиме быстрого боя. Сражение протекает автоматически, и компьютер демонстрирует вам только его результат.",
     "{Курсор}\n\nПереключение курсора с черно-белого на цветной и обратно. Цветной курсор выглядит симпатичнее, но иногда он перемещается по экрану не так плавно, как черно-белый."
};
const char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] =
    { "Сизый", "Синий", "Коричневый", "Золотой", "Зеленый", "Оранжевый", "Фиолетовый", "Красный"};
const char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
     "Башня алхимика",
     "Арена",
     "Лачуга волхва",
     "Око волхва",
     "Конюшни",
     "Русалка",
     "Сирены"
};
const char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
     "Земляные холмы",
     "Алтарь Земли",
     "Алтарь Воздуха",
     "Алтарь Огня",
     "Алтарь Воды"
};

i32 gUnusedData4826c0 = 250;

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
b32 gbComputeExtent = false;
b32 gbCurrArmyDrawn = false;
i32 gUnusedData4a49dc = 0;
b32 gbLimitToExtent = false;
b32 gbLoadingMonoIcon = false;
b32 gbSaveBiggestExtent = false;
i32 gUnusedData4a49ec = 0;
i32 giScrollX = 0;
i32 giScrollY = 0;

i32 gObjectClass = 0;
i32 gUnusedData4a49fc = 0;
b32 gStatusTextShown = false;
b32 gbInDialog = false;
b32 gbMinimized = false;
b32 gbInSetupDialog = false;


b32 gGenerateUnseen = false;
b32 gGeneratingMap = false;
i32 gUnusedData4a4a18 = 0;
b32 gbInSmackMgr = false;
i32 gStatusTextClearTime = 0;
HMENU hmnuDflt = NULL;
HMENU hmnuCmbt = NULL;
HMENU hmnuAdv = NULL;
HMENU hmnuTown = NULL;
i32 gUnusedData4a4a34 = 0;
b32 gbFirstTimeThrough = false;
b32 gbInPollSound = false;
b32 bInShutDown = false;
i32 bEarlySetupDone = 0;
b32 gbInMemError = false;


i32 giDebugLevel;
u8 bSaveMusicPosition[KB_MUSIC_TRACK_COUNT];
class mouseManager* gpMouseManager;
char gText[GLOBAL_TEXT_BUFFER_SIZE];
char* EXPANSION_AGGREGATE_NAME;
char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
char* DEFAULT_AGGREGATE_NAME;
fullMap gMaps[EDIT_MAP_COPIES];
i32 gSelectionWidth;
b32 gbTextEntryEscaped;
class heroWindowManager* gpWindowManager;
editManager* gEditManager;
char gMapFileName[EDITOR_MAP_FILE_NAME_SIZE];
char gStatusText[EDITOR_STATUS_TEXT_SIZE];
i32 gSelectionY;
resourceManager* gpResourceManager;
heroWindow* pNormalDialogWindow;
u8 bMusicIsLooping[KB_MUSIC_TRACK_COUNT];
i32 gSelectionHeight;
soundManager* gpSoundManager;
char gLastFilename[GLOBAL_LAST_FILENAME_SIZE];
i32 gStatusTextHoldTime;
class palette* gpBufferPalette;
palette* gPalette;
char cAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
char gcRegAppPath[GLOBAL_AGGREGATE_PATH_SIZE];
i32 giMaxExtentX;
i32 giMaxExtentY;
inputManager* gpInputManager;
char gcCommandLine[GLOBAL_COMMAND_LINE_SIZE];
configStruct gConfig;
i32 giMinExtentX;
i32 giMinExtentY;
executive* gpExec;
i32 giCurWindowsStyleFlags;
char gcRegCDRomPath[GLOBAL_AGGREGATE_PATH_SIZE];
i32 glTimers[GLOBAL_TIMER_COUNT];

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
            && giMainVideoModeColorDepth != EDITOR_PALETTED_COLOR_DEPTH)
            glTimers[GLOBAL_COLOR_CYCLE_TIMER_SLOT] += EDITOR_NON_PALETTED_CYCLE_DELAY;
        CycleColors(0);
    }
    gbInPollSound = false;
}


void ProtectShippedMap(void) {
    i32 i;

    if (gbShowAllMaps)
        return;
    for (i = 0; i < EDITOR_SHIPPED_MAP_COUNT; i++) {
        if (!strcmpi(gMapFileName, gShippedMaps[i][0])) {
            strcpy(gMapFileName, gShippedMaps[i][1]);
            memmove(gEditMapHeader.name + 1, gEditMapHeader.name, sizeof(gEditMapHeader.name) - 1);
            gEditMapHeader.name[sizeof(gEditMapHeader.name) - 1] = 0;
            gEditMapHeader.name[0] = '_';
        }
    }
}

i32 oldmain(void) {
    heroWindow* window;
    i32 result;
    i32 keepRunning;
    char loadName[EDITOR_MAP_FILE_NAME_SIZE];

    if (gpExec->InitSystem())
        ShutDown("Невозможно инициировать систему");
    KBChangeMenu(hmnuDflt);
    smallFont = gpResourceManager->GetFont("smalfont.fnt");
    bigFont = gpResourceManager->GetFont("BIGfont.fnt");
    gPalette = gpResourceManager->GetPalette("kb.pal");
    gpResourceManager->GetBackdrop("editor.icn", gpWindowManager->m_screen, 1);
    gpWindowManager->UpdateScreen();
    gpWindowManager->FadeScreen(FADE_IN, EDITOR_FADE_STEPS, gPalette);
    gpMouseManager->SetPointer("editor.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
    gpMouseManager->SetColorMice(gConfig.gfx[(giCurExe)].colorMouseCursor);
    gpMouseManager->ShowColorPointer();
    window = NULL;
    result = -1;
    keepRunning = 1;
    while (keepRunning) {
        gbInSetupDialog = true;
        window = new heroWindow(EDITOR_SETUP_WINDOW_X, EDITOR_SETUP_WINDOW_Y, "stpemain.bin");
        if (!window)
            MemError();
        gpWindowManager->DoDialog(window, SetupMainHandler, 0);
        delete window;
        result = gpWindowManager->m_dialogResult;
        gbInSetupDialog = false;
        switch (result) {
            case EDITOR_SETUP_LOAD_MAP:
                if (PickMap(EDITOR_SETUP_PICK_LOAD_MODE))
                    keepRunning = 0;
                sprintf(loadName, gMapFileName);
                break;
            case EDITOR_SETUP_NEW_MAP:
                if (SetupNewMap())
                    keepRunning = 0;
                break;
            case EDITOR_SETUP_QUIT:
            case DIALOG_BUTTON_1:
                gpWindowManager->FadeScreen(FADE_OUT, EDITOR_FADE_STEPS, gPalette);
                ShutDown(NULL);
                break;
        }
    }
    gpMouseManager->HideColorPointer();
    gpWindowManager->FadeScreen(FADE_OUT, EDITOR_SLOW_FADE_STEPS, gPalette);
    memset(gpWindowManager->m_screen->m_pixels, EDITOR_BACKGROUND_COLOR, EDITOR_SCREEN_BYTES);
    if (gpExec->AddManager(gEditManager, -1))
        ShutDown("Не могу добавить менеджера!");
    if (result == EDITOR_SETUP_LOAD_MAP) {
        strcpy(gMapFileName, loadName);
        gEditManager->LoadMap(gMapFileName);
        ProtectShippedMap();
    }
    gEditManager->DrawRadar(1);
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
    gpWindowManager->FadeScreen(FADE_IN, EDITOR_SLOW_FADE_STEPS, gPalette);
    gpWindowManager->m_updateFlags = gConfig.editorPaletteCycling;
    gpExec->MainLoop();
    gpExec->RemoveManager(gEditManager);
    gpWindowManager->FadeScreen(FADE_OUT, EDITOR_FADE_STEPS, gPalette);
    gpResourceManager->Dispose(gPalette);
    ShutDown(NULL);
    return 0;
}

void IncrementArgumentA(i32 value) {
    value++;
}

void EditorIdleHook(void) {}

void IncrementArgumentB(i32 value) {
    value++;
}

void DelayTicks(i32 ticks) {
    i32 unused [[maybe_unused]] = 0;

    glTimers[EDITOR_DELAY_TIMER_SLOT] = KBTickCount() + ticks * EDITOR_DELAY_TICK_MILLISECONDS;
    DelayTil(glTimers + EDITOR_DELAY_TIMER_SLOT);
}

void DelayTil(i32* endTime) {
    while (*endTime > KBTickCount()) {
        Process1WindowsMessage();
        PollSound();
    }
}

void DelayMilli(i32l delay) {
    DelayTilMilli(KBTickCount() + delay);
}

void DelayTilMilli(i32l endTime) {
    while (endTime > KBTickCount()) {
        Process1WindowsMessage();
        PollSound();
    }
}

void FileError(const char* filename) {
    char message[EDITOR_FILE_ERROR_TEXT_SIZE];

    sprintf(message, "Ошибка открытия файла %s!", filename);
    ShutDown(message);
}

void ShutDown(const char* message) {
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
        MessageBoxA(hwndApp, buffer, "Непредвиденное прерывание программы", MB_ICONHAND);
    } else {
        sprintf(buffer, "Пока!");
    }
    gbClosingApp = true;
    if (mapExtra)
        delete mapExtra;
    mapExtra = NULL;
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


i32 InterpretCommandLine(void) {
    i32 size;
    i32 i;

    giDebugLevel = 0;
    size = strlen(gcCommandLine);
    for (i = 0; i < size; i++) {
        if (gcCommandLine[i] == '/' && i + 1 < size) {

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

void EarlyShutdown(const char* caption, const char* text) {
    MessageBoxA(hwndApp, text, caption, MB_ICONHAND);
    exit(0);
}

i32 EarlySetup(void) {
    i32 result;
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
    if (result == EDITOR_CD_NO_DRIVE) {
        EarlyShutdown(
            "Ошибка загрузки",
            "Нет доступа к CD-ROM."
        );
        exit(0);
    }
    if (result == EDITOR_CD_NOT_FOUND) {
        EarlyShutdown(
            "Ошибка загрузки",
            "Вы должны иметь диск с Героями II в вашем CD-ROM, чтобы запустить Редактор \nкарт Героев II. Пожалуйста, вставьте диск и попробуйте еще раз."
        );
        exit(0);
    }
    if (result == EDITOR_CD_NO_APP_PATH) {
        EarlyShutdown(
            "Ошибка загрузки",
            "Не могу переключиться в директорию Героев II.  Запустите программу установки."
        );
        exit(0);
    }
    if (result == EDITOR_CD_NO_DATA) {
        EarlyShutdown(
            "Ошибка загрузки",
            "Не могу найти файлы данных Героев II.  Пожалуйста, запустите программу установки."
        );
        exit(0);
    }
    for (i = 0; i < GLOBAL_TIMER_COUNT; i++)
        glTimers[i] = 0;
    hmnuDflt = LoadMenuA(hInstApp, "mnuDflt");
    return 1;
}

void MemError(void) {
    if (gbInMemError)
        return;
    gbInMemError = true;
    ShutDown("Недостаточно памяти.");
}

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


void NormalDialog(
    const char* text,
    i32 dialogType,
    i32 windowX,
    i32 windowY,
    i32,
    i32,
    i32,
    i32,
    i32,
    i32
) {
    i32 resourceFrame [[maybe_unused]];
    i16 showMessage [[maybe_unused]];
    i32 textWidgetId;
    i32 windowHeight;
    b32 showPrimaryBonus [[maybe_unused]];
    tag_message message;
    i32 resourceSlot [[maybe_unused]];
    i32 resourceY [[maybe_unused]];
    i32 iconHeight [[maybe_unused]];
    i32 lineCount;
    i32 dialogContentHeight;
    i32 savedFirstResourceType [[maybe_unused]];
    i32 maxIconHeight [[maybe_unused]];
    i32 savedSecondResourceType [[maybe_unused]];
    i32 windowRows;
    char iconFile[NORMAL_DIALOG_FILENAME_LENGTH];
    i32 windowWidth;
    i32 panelHeight [[maybe_unused]];

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
    if (windowX == -1 || windowWidth + windowX >= EDITOR_DIALOG_SCREEN_MAX_X)
        windowX = EDITOR_DIALOG_DEFAULT_X;
    if (windowY == -1 || windowHeight + windowY >= EDITOR_DIALOG_SCREEN_MAX_Y) {
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
    message.payload.widget.data.value = (WIDGET_COMMAND_CLEAR_FLAGS);
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


void ShowStatusText(char* text) {
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

void ClearStatusText(void) {
    gStatusTextClearTime = EDITOR_STATUS_TEXT_KEPT;
    if (gStatusTextShown) {
        gStatusTextShown = false;
        gEditManager->m_window->DrawWindow(0);
        gpWindowManager->UpdateScreenRegion(
            EDITOR_STATUS_BAR_X,
            EDITOR_STATUS_BAR_Y,
            EDITOR_STATUS_BAR_WIDTH,
            EDITOR_STATUS_BAR_HEIGHT
        );
    }
}

void UpdateAppSpecificMenus(void*) {}

void CleanUpMenus(void) {
    if (hmnuApp) {
        SetMenu(hwndApp, NULL);
        if (hmnuDflt)
            DestroyMenu(hmnuDflt);
    }
    hmnuApp = NULL;
}

void EarlyShutDownSystem(void) {
    if (gEditManager)
        gEditManager->SelectTool(-1);
}

i32 GameUnsaved(void) {
    return 1;
}

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

void EarlyResizeWindow(i32, i32, i32, i32) {}

void UpdateSystemOptionsMenu(void) {}

void SetWinText(heroWindow* window, i32 id) {
    i32 matchedWidgets [[maybe_unused]] = 0;
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
