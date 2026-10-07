

#include <Ints.h>
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

typedef enum EditorStartupConstant {

    EDITOR_DELAY_TICK_MILLISECONDS = 15,
    EDITOR_DELAY_TIMER_SLOT     = 1,
    EDITOR_MOUSE_UPDATE_INTERVAL = 13,
    EDITOR_COLOR_CYCLE_INTERVAL = 200,
    EDITOR_NON_PALETTED_CYCLE_DELAY = 300,

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

H2EnumStorage<TerrainType, u8>
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
const char* gTilesetFiles[H2EnumIndex(TILESET_COUNT)] = {
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
u32l gTownEligibleBuildMask[H2EnumIndex(FACTION_COUNT)] = {
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

i32 gRandomMapPlayers = NEW_MAP_DEFAULT_PLAYERS;
double gTerrainPercent[RANDOM_MAP_TERRAIN_COUNT] = {30.0, 30.0, 20.0, 0.0, 0.0, 0.0, 20.0, 0.0};
double gDensityPercent[RANDOM_MAP_DENSITY_COUNT] = {50.0, 50.0, 50.0, 50.0, 50.0};
b32 gScatterTerrain = true;
struct SMenuEnableStatus gsMenuEnableStatus[MENU_ENABLE_STATUS_COUNT] = {
    {APP_MENU_NONE, 0, 0, 0},
    {H2EnumIndex(KBWIN_MENU_SIZE_640_480), 1, 1, 0},
    {H2EnumIndex(KBWIN_MENU_SIZE_800_600), 1, 1, 0},
    {H2EnumIndex(KBWIN_MENU_SIZE_1024_768), 1, 1, 0},
    {H2EnumIndex(KBWIN_MENU_SIZE_1280_1024), 1, 1, 0},
    {H2EnumIndex(KBWIN_MENU_FULLSCREEN), 1, 1, 0},
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
    {H2EnumIndex(KBWIN_MENU_HELP), 1, 1, 0},
    {H2EnumIndex(KBWIN_MENU_ABOUT), 1, 1, 0},
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

const char* gClearHelp[CLEAR_HELP_COUNT] = {
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

const char* gEditPanelHelp[EDIT_PANEL_HELP_COUNT] = {
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

const char* gEditTerrainNames[EDITOR_TERRAIN_NAME_COUNT] = {
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

const char* gObjectClassNames[EDITOR_OBJECT_CLASS_COUNT] = {
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

const char* gSetupNewMapHelp[SETUP_NEW_MAP_HELP_COUNT] = {
    localization::Tr("editor.table.gSetupNewMapHelp.0"),
    localization::Tr("editor.table.gSetupNewMapHelp.1"),
    localization::Tr("editor.table.gSetupNewMapHelp.2")
};

const char* gSetupMapSizeHelp[SETUP_MAP_SIZE_HELP_COUNT] = {
    localization::Tr("editor.table.gSetupMapSizeHelp.0"),
    localization::Tr("editor.table.gSetupMapSizeHelp.1"),
    localization::Tr("editor.table.gSetupMapSizeHelp.2"),
    localization::Tr("editor.table.gSetupMapSizeHelp.3"),
    localization::Tr("editor.table.gSetupMapSizeHelp.4")
};

const char* gSetupMainHelp[SETUP_MAIN_HELP_COUNT] = {
    localization::Tr("editor.table.gSetupMainHelp.0"),
    localization::Tr("editor.table.gSetupMainHelp.1"),
    localization::Tr("editor.table.gSetupMainHelp.2")
};

const char* gEventFrequencyNames[EVENT_FREQUENCY_COUNT] = {
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

const char* gTownNames[EDITOR_TOWN_NAME_COUNT] = {
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

const char* gFileMenuHelp[EDIT_FILE_MENU_HELP_COUNT] = {
    localization::Tr("editor.table.gFileMenuHelp.0"),
    localization::Tr("editor.table.gFileMenuHelp.1"),
    localization::Tr("editor.table.gFileMenuHelp.2"),
    localization::Tr("editor.table.gFileMenuHelp.3"),
    localization::Tr("editor.table.gFileMenuHelp.4")
};

const char* gSystemOptionsHelp[EDIT_SYSTEM_OPTIONS_HELP_COUNT] = {
    localization::Tr("editor.table.gSystemOptionsHelp.0"),
    localization::Tr("editor.table.gSystemOptionsHelp.1"),
    localization::Tr("editor.table.gSystemOptionsHelp.2"),
    localization::Tr("editor.table.gSystemOptionsHelp.3"),
    localization::Tr("editor.table.gSystemOptionsHelp.4")
};

const char* gVictoryConditionNames[SPEC_VICTORY_CONDITION_COUNT] = {
    localization::Tr("editor.table.gVictoryConditionNames.0"),
    localization::Tr("editor.table.gVictoryConditionNames.1"),
    localization::Tr("editor.table.gVictoryConditionNames.2"),
    localization::Tr("editor.table.gVictoryConditionNames.3"),
    localization::Tr("editor.table.gVictoryConditionNames.4"),
    localization::Tr("editor.table.gVictoryConditionNames.5")
};

const char* gLossConditionNames[SPEC_LOSS_CONDITION_COUNT] = {
    localization::Tr("editor.table.gLossConditionNames.0"),
    localization::Tr("editor.table.gLossConditionNames.1"),
    localization::Tr("editor.table.gLossConditionNames.2"),
    localization::Tr("editor.table.gLossConditionNames.3")
};

SWinSetup gWinSetup[EDITOR_DIALOG_WIN_SETUP_COUNT] = {
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

const char* gArtifactNames[H2EnumIndex(ARTIFACT_COUNT)] = {
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
    "ERROR : Artifact 82"  ,
    "ERROR : Artifact 83"  ,
    "ERROR : Artifact 84"  ,
    "ERROR : Artifact 85"  ,
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
const char* gArtifactDesc[H2EnumIndex(ARTIFACT_COUNT)] = {
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
    "{ERROR}\n\nArtifact 82."  ,
    "{ERROR}\n\nArtifact 83."  ,
    "{ERROR}\n\nArtifact 84."  ,
    "{ERROR}\n\nArtifact 85."  ,
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
const char* gArtifactEvent[H2EnumIndex(ARTIFACT_COUNT)] = {
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
    ""  ,
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
    ""  ,
    "ERROR : Artifact event 82."  ,
    "ERROR : Artifact event 83."  ,
    "ERROR : Artifact event 84."  ,
    "ERROR : Artifact event 85."  ,
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
const char* gStatNames[HERO_PRIMARY_STAT_COUNT] = {
    localization::Tr("table.gStatNames.0"),
    localization::Tr("table.gStatNames.1"),
    localization::Tr("table.gStatNames.2"),
    localization::Tr("table.gStatNames.3")
};
const char* gStatDesc[HERO_PRIMARY_STAT_COUNT] = {
    localization::Tr("table.gStatDesc.0"),
    localization::Tr("table.gStatDesc.1"),
    localization::Tr("table.gStatDesc.2"),
    localization::Tr("table.gStatDesc.3")
};
const char* gAlignmentNames[KB_ALIGNMENT_NAME_COUNT] = {
    localization::Tr("table.gAlignmentNames.0"),
    localization::Tr("table.gAlignmentNames.1"),
    localization::Tr("table.gAlignmentNames.2"),
    localization::Tr("table.gAlignmentNames.3"),
    localization::Tr("table.gAlignmentNames.4"),
    localization::Tr("table.gAlignmentNames.5"),
    localization::Tr("table.gAlignmentNames.6"),
    localization::Tr("table.gAlignmentNames.7")
};
const char* gArmyShortNames[H2EnumIndex(CREATURE_COUNT)] = {
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
const char* gArmyNames[H2EnumIndex(CREATURE_COUNT)] = {
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
const char* gArmyNamesPlural[H2EnumIndex(CREATURE_COUNT)] = {
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
const char* gTerrainNames[H2EnumIndex(TERRAIN_COUNT)] = {
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
const char* gResourceNames[H2EnumIndex(RES_COUNT)] = {
    localization::Tr("table.gResourceNames.0"),
    localization::Tr("table.gResourceNames.1"),
    localization::Tr("table.gResourceNames.2"),
    localization::Tr("table.gResourceNames.3"),
    localization::Tr("table.gResourceNames.4"),
    localization::Tr("table.gResourceNames.5"),
    localization::Tr("table.gResourceNames.6")
};


const char* gMineNames[H2EnumIndex(RES_COUNT)] = {
    localization::Tr("table.gMineNames.0"),
    localization::Tr("table.gMineNames.1"),
    localization::Tr("table.gMineNames.2"),
    localization::Tr("table.gMineNames.3"),
    localization::Tr("table.gMineNames.4"),
    localization::Tr("table.gMineNames.5"),
    localization::Tr("table.gMineNames.6")
};
const char* gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT] = {
    ""  ,
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
    ""  ,
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
    ""  ,
    localization::Tr("table.gQuickViewText.51"),
    localization::Tr("table.gQuickViewText.52"),
    localization::Tr("table.gQuickViewText.53"),
    localization::Tr("table.gQuickViewText.54"),
    localization::Tr("table.gQuickViewText.55"),
    localization::Tr("table.gQuickViewText.56"),
    ""  ,
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
    "%s"  ,
    "%s"  ,
    localization::Tr("table.gQuickViewText.123")
};
const char* gEventText[KB_EVENT_TEXT_TABLE_COUNT] = {


    localization::Tr("table.gEventText.0"),

    localization::Tr("table.gEventText.1"),

    localization::Tr("table.gEventText.2"),


    localization::Tr("table.gEventText.3"),
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",

    localization::Tr("table.gEventText.12"),


    localization::Tr("table.gEventText.13"),

    localization::Tr("table.gEventText.14"),

    localization::Tr("table.gEventText.15"),

    localization::Tr("table.gEventText.16"),


    localization::Tr("table.gEventText.17"),


    localization::Tr("table.gEventText.18"),

    localization::Tr("table.gEventText.19"),

    localization::Tr("table.gEventText.20"),


    localization::Tr("table.gEventText.21"),

    localization::Tr("table.gEventText.22"),


    localization::Tr("table.gEventText.23"),

    localization::Tr("table.gEventText.24"),

    localization::Tr("table.gEventText.25"),


    localization::Tr("table.gEventText.26"),

    localization::Tr("table.gEventText.27"),

    localization::Tr("table.gEventText.28"),


    localization::Tr("table.gEventText.29"),

    localization::Tr("table.gEventText.30"),

    localization::Tr("table.gEventText.31"),


    localization::Tr("table.gEventText.32"),

    localization::Tr("table.gEventText.33"),

    localization::Tr("table.gEventText.34"),


    localization::Tr("table.gEventText.35"),

    localization::Tr("table.gEventText.36"),

    localization::Tr("table.gEventText.37"),


    localization::Tr("table.gEventText.38"),

    localization::Tr("table.gEventText.39"),

    localization::Tr("table.gEventText.40"),


    localization::Tr("table.gEventText.41"),

    localization::Tr("table.gEventText.42"),

    localization::Tr("table.gEventText.43"),


    localization::Tr("table.gEventText.44"),

    localization::Tr("table.gEventText.45"),

    localization::Tr("table.gEventText.46"),


    localization::Tr("table.gEventText.47"),

    localization::Tr("table.gEventText.48"),

    localization::Tr("table.gEventText.49"),


    localization::Tr("table.gEventText.50"),

    localization::Tr("table.gEventText.51"),

    localization::Tr("table.gEventText.52"),
    "",
    "",
    "",
    "",
    "",

    localization::Tr("table.gEventText.58"),


    localization::Tr("table.gEventText.59"),


    localization::Tr("table.gEventText.60"),

    localization::Tr("table.gEventText.61"),


    localization::Tr("table.gEventText.62"),


    localization::Tr("table.gEventText.63"),


    localization::Tr("table.gEventText.64"),


    localization::Tr("table.gEventText.65"),


    localization::Tr("table.gEventText.66"),

    localization::Tr("table.gEventText.67"),


    localization::Tr("table.gEventText.68"),

    localization::Tr("table.gEventText.69"),
    "",
    "",

    localization::Tr("table.gEventText.72"),

    localization::Tr("table.gEventText.73"),


    localization::Tr("table.gEventText.74"),
    "",
    "",
    "",
    "",
    "",
    "",


    localization::Tr("table.gEventText.81"),


    localization::Tr("table.gEventText.82"),


    localization::Tr("table.gEventText.83"),


    localization::Tr("table.gEventText.84"),

    localization::Tr("table.gEventText.85"),


    localization::Tr("table.gEventText.86"),


    localization::Tr("table.gEventText.87"),
    "",
    "",
    "",
    "",
    "",


    localization::Tr("table.gEventText.93"),


    localization::Tr("table.gEventText.94")
};
const char* gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT] = {

    localization::Tr("table.gCPanelHelp.0"),

    localization::Tr("table.gCPanelHelp.1"),

    localization::Tr("table.gCPanelHelp.2"),

    localization::Tr("table.gCPanelHelp.3"),

    localization::Tr("table.gCPanelHelp.4")
};
const char* gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT] = {

    localization::Tr("table.gCSPanelHelp.0"),

    localization::Tr("table.gCSPanelHelp.1"),


    localization::Tr("table.gCSPanelHelp.2"),


    localization::Tr("table.gCSPanelHelp.3"),


    localization::Tr("table.gCSPanelHelp.4"),


    localization::Tr("table.gCSPanelHelp.5"),

    localization::Tr("table.gCSPanelHelp.6")
};
const char* gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT] = {

    localization::Tr("table.gAPanelHelp.0"),

    localization::Tr("table.gAPanelHelp.1"),

    localization::Tr("table.gAPanelHelp.2"),

    (localization::Tr("table.gAPanelHelp.3")),

    localization::Tr("table.gAPanelHelp.4")
};
const char* gInitMenuHelp[KB_INIT_MENU_HELP_COUNT] = {

    localization::Tr("table.gInitMenuHelp.0"),

    localization::Tr("table.gInitMenuHelp.1"),

    localization::Tr("table.gInitMenuHelp.2"),

    localization::Tr("table.gInitMenuHelp.3"),

    localization::Tr("table.gInitMenuHelp.4")
};
const char* gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT] = {

    localization::Tr("table.gAdvMenuHelp.0"),

    localization::Tr("table.gAdvMenuHelp.1"),

    localization::Tr("table.gAdvMenuHelp.2"),

    localization::Tr("table.gAdvMenuHelp.3"),

    localization::Tr("table.gAdvMenuHelp.4"),

    localization::Tr("table.gAdvMenuHelp.5"),

    localization::Tr("table.gAdvMenuHelp.6"),

    localization::Tr("table.gAdvMenuHelp.7")
};
const char* gLuckText[KB_LUCK_TEXT_COUNT] = {

    localization::Tr("table.gLuckText.0"),

    localization::Tr("table.gLuckText.1"),

    localization::Tr("table.gLuckText.2"),

    localization::Tr("table.gLuckText.3"),

    localization::Tr("table.gLuckText.4"),

    localization::Tr("table.gLuckText.5"),

    localization::Tr("table.gLuckText.6")
};
const char* gMoraleText[KB_MORALE_TEXT_COUNT] = {

    localization::Tr("table.gMoraleText.0"),

    localization::Tr("table.gMoraleText.1"),

    localization::Tr("table.gMoraleText.2"),

    localization::Tr("table.gMoraleText.3"),

    localization::Tr("table.gMoraleText.4"),

    localization::Tr("table.gMoraleText.5"),

    localization::Tr("table.gMoraleText.6")
};
const char* onOffText[KB_ON_OFF_TEXT_COUNT] = {

    localization::Tr("table.onOffText.0"),

    localization::Tr("table.onOffText.1"),

    localization::Tr("table.onOffText.2"),

    localization::Tr("table.onOffText.3"),

    localization::Tr("table.onOffText.4"),

    localization::Tr("table.onOffText.5"),

    localization::Tr("table.onOffText.6"),

    localization::Tr("table.onOffText.7"),

    localization::Tr("table.onOffText.8"),

    localization::Tr("table.onOffText.9"),

    localization::Tr("table.onOffText.10")
};
const char* walkSpeedText[KB_WALK_SPEED_TEXT_COUNT] = {

    localization::Tr("table.walkSpeedText.0"),

    localization::Tr("table.walkSpeedText.1"),

    localization::Tr("table.walkSpeedText.2"),

    localization::Tr("table.walkSpeedText.3"),

    localization::Tr("table.walkSpeedText.4")
};
const char* gColors[H2EnumIndex(FACTION_COUNT)] = {
    localization::Tr("table.gColors.0"),
    localization::Tr("table.gColors.1"),
    localization::Tr("table.gColors.2"),
    localization::Tr("table.gColors.3"),
    localization::Tr("table.gColors.4"),
    localization::Tr("table.gColors.5")
};
const char* gColorAbbreviations[PLAYER_COLOR_COUNT] = {
    localization::Tr("color.abbreviated.blue"),
    localization::Tr("color.abbreviated.green"),
    localization::Tr("color.abbreviated.red"),
    localization::Tr("color.abbreviated.yellow"),
    localization::Tr("color.abbreviated.orange"),
    localization::Tr("color.abbreviated.purple")
};
const char* gMonthNames[KB_MONTH_NAME_COUNT] = {

    localization::Tr("table.gMonthNames.0"),

    localization::Tr("table.gMonthNames.1"),

    localization::Tr("table.gMonthNames.2"),

    localization::Tr("table.gMonthNames.3"),

    localization::Tr("table.gMonthNames.4"),

    localization::Tr("table.gMonthNames.5"),

    localization::Tr("table.gMonthNames.6"),

    localization::Tr("table.gMonthNames.7"),

    localization::Tr("table.gMonthNames.8"),

    localization::Tr("table.gMonthNames.9")
};
const char* gWeekNames[KB_WEEK_NAME_COUNT] = {

    localization::Tr("table.gWeekNames.0"),

    localization::Tr("table.gWeekNames.1"),

    localization::Tr("table.gWeekNames.2"),

    localization::Tr("table.gWeekNames.3"),

    localization::Tr("table.gWeekNames.4"),

    localization::Tr("table.gWeekNames.5"),

    localization::Tr("table.gWeekNames.6"),

    localization::Tr("table.gWeekNames.7"),

    localization::Tr("table.gWeekNames.8"),

    localization::Tr("table.gWeekNames.9"),

    localization::Tr("table.gWeekNames.10"),

    localization::Tr("table.gWeekNames.11"),

    localization::Tr("table.gWeekNames.12"),

    localization::Tr("table.gWeekNames.13"),

    localization::Tr("table.gWeekNames.14")
};
const char* cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT] = {

    localization::Tr("table.cHeroScreen.0"),

    localization::Tr("table.cHeroScreen.1"),

    localization::Tr("table.cHeroScreen.2"),

    localization::Tr("table.cHeroScreen.3"),

    localization::Tr("table.cHeroScreen.4"),

    localization::Tr("table.cHeroScreen.5"),

    localization::Tr("table.cHeroScreen.6"),

    localization::Tr("table.cHeroScreen.7"),

    localization::Tr("table.cHeroScreen.8"),

    localization::Tr("table.cHeroScreen.9"),

    localization::Tr("table.cHeroScreen.10"),

    localization::Tr("table.cHeroScreen.11"),

    localization::Tr("table.cHeroScreen.12"),

    localization::Tr("table.cHeroScreen.13"),

    localization::Tr("table.cHeroScreen.14"),

    localization::Tr("table.cHeroScreen.15"),

    localization::Tr("table.cHeroScreen.16"),

    localization::Tr("table.cHeroScreen.17"),

    localization::Tr("table.cHeroScreen.18"),

    localization::Tr("table.cHeroScreen.19"),

    localization::Tr("table.cHeroScreen.20"),

    localization::Tr("table.cHeroScreen.21"),

    localization::Tr("table.cHeroScreen.22"),

    localization::Tr("table.cHeroScreen.23"),

    localization::Tr("table.cHeroScreen.24")
};
const char* cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT] = {

    localization::Tr("table.cCastleInfo.0"),

    localization::Tr("table.cCastleInfo.1"),

    localization::Tr("table.cCastleInfo.2"),

    localization::Tr("table.cCastleInfo.3"),

    localization::Tr("table.cCastleInfo.4"),

    localization::Tr("table.cCastleInfo.5"),

    localization::Tr("table.cCastleInfo.6"),

    localization::Tr("table.cCastleInfo.7"),

    localization::Tr("table.cCastleInfo.8"),

    localization::Tr("table.cCastleInfo.9"),

    localization::Tr("table.cCastleInfo.10"),

    localization::Tr("town.recruit.new_hero"),

    localization::Tr("table.cCastleInfo.12"),

    localization::Tr("table.cCastleInfo.13"),

    localization::Tr("table.cCastleInfo.14"),

    localization::Tr("table.cCastleInfo.15")
};
const char* cLuckInfo[KB_LUCK_INFO_TEXT_COUNT] = {


    localization::Tr("table.cLuckInfo.0"),


    localization::Tr("table.cLuckInfo.1"),


    localization::Tr("table.cLuckInfo.2"),

    localization::Tr("table.cLuckInfo.3"),

    localization::Tr("table.cLuckInfo.4"),

    localization::Tr("table.cLuckInfo.5"),

    localization::Tr("table.cLuckInfo.6"),

    localization::Tr("table.cLuckInfo.7"),

    localization::Tr("table.cLuckInfo.8"),

    localization::Tr("table.cLuckInfo.9"),

    localization::Tr("table.cLuckInfo.10"),

    localization::Tr("table.cLuckInfo.11"),

    localization::Tr("table.cLuckInfo.12"),

    localization::Tr("table.cLuckInfo.13"),

    localization::Tr("table.cLuckInfo.14"),

    localization::Tr("table.cLuckInfo.15"),

    localization::Tr("table.cLuckInfo.16"),

    localization::Tr("table.cLuckInfo.17"),

    localization::Tr("table.cLuckInfo.18"),

    localization::Tr("table.cLuckInfo.19"),

    localization::Tr("table.cLuckInfo.20")
};
const char* IQnames[KB_IQ_NAME_COUNT] = {

    localization::Tr("table.IQnames.0"),

    localization::Tr("table.IQnames.1"),

    localization::Tr("table.IQnames.2"),

    localization::Tr("table.IQnames.3"),

    localization::Tr("table.IQnames.4")
};
const char* cSpellHelp[KB_SPELL_HELP_TEXT_COUNT] = {

    localization::Tr("table.cSpellHelp.0"),

    localization::Tr("table.cSpellHelp.1"),

    localization::Tr("table.cSpellHelp.2"),

    localization::Tr("table.cSpellHelp.3"),

    localization::Tr("table.cSpellHelp.4"),

    localization::Tr("table.cSpellHelp.5"),

    localization::Tr("table.cSpellHelp.6"),

    localization::Tr("table.cSpellHelp.7"),

    (localization::Tr("table.cSpellHelp.8"))
};
const char* speedText[KB_SPEED_TEXT_COUNT] = {
      "",
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
const char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT] = {
     localization::Tr("table.cArmyDetail.0"),
     localization::Tr("table.cArmyDetail.1"),
      localization::Tr("table.cArmyDetail.2"),
      localization::Tr("table.cArmyDetail.3"),
      localization::Tr("table.cArmyDetail.4"),
      localization::Tr("table.cArmyDetail.5"),
      localization::Tr("table.cArmyDetail.6"),
      localization::Tr("table.cArmyDetail.7"),
      localization::Tr("table.cArmyDetail.8")
};
const char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT] = {
     localization::Tr("table.cWellDetail.0"),
     localization::Tr("table.cWellDetail.1"),
      localization::Tr("table.cWellDetail.2"),
      localization::Tr("table.cWellDetail.3"),
      localization::Tr("table.cWellDetail.4"),
      localization::Tr("table.cWellDetail.5"),
      localization::Tr("table.cWellDetail.6"),
     localization::Tr("table.cWellDetail.7"),
     localization::Tr("table.cWellDetail.8")
};
const char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT] = {

    (localization::Tr("table.cKingdomOverview.0")),
     localization::Tr("table.cKingdomOverview.1"),
     localization::Tr("table.cKingdomOverview.2")
};
const char* cNewTurn[KB_NEW_TURN_TEXT_COUNT] = {
     localization::Tr("table.cNewTurn.0"),
     localization::Tr("table.cNewTurn.1"),
     localization::Tr("table.cNewTurn.2"),
     localization::Tr("table.cNewTurn.3"),
     localization::Tr("table.cNewTurn.4"),
     localization::Tr("table.cNewTurn.5"),
     localization::Tr("table.cNewTurn.6")
};
const char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT] = {
     localization::Tr("table.cViewGeneralLabels.0"),
     localization::Tr("table.cViewGeneralLabels.1"),
     localization::Tr("table.cViewGeneralLabels.2"),
     localization::Tr("table.cViewGeneralLabels.3"),
      localization::Tr("table.cViewGeneralLabels.4"),
      localization::Tr("table.cViewGeneralLabels.5"),
     localization::Tr("table.cViewGeneralLabels.6")
};
const char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT] = {
     localization::Tr("table.cViewGeneralHelp.0"),
     localization::Tr("table.cViewGeneralHelp.1"),
     localization::Tr("table.cViewGeneralHelp.2"),
     localization::Tr("table.cViewGeneralHelp.3"),
     localization::Tr("table.cViewGeneralHelp.4"),
     localization::Tr("table.cViewGeneralHelp.5"),
     localization::Tr("table.cViewGeneralHelp.6")
};
const char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT] = {
     localization::Tr("table.cViewGeneralLongHelp.0"),
     localization::Tr("table.cViewGeneralLongHelp.1"),
     localization::Tr("table.cViewGeneralLongHelp.2"),
     localization::Tr("table.cViewGeneralLongHelp.3")
};
const char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT] = {
      "",
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
const char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT] =
    { localization::Tr("table.cHeroLevel.0"),   localization::Tr("table.cHeroLevel.1"),   localization::Tr("table.cHeroLevel.2")};
const char* cCombatHelp[KB_COMBAT_HELP_COUNT] = {
     localization::Tr("table.cCombatHelp.0"),
     localization::Tr("table.cCombatHelp.1"),
     localization::Tr("table.cCombatHelp.2"),
     localization::Tr("table.cCombatHelp.3"),
      ""
};
const char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT] = {
     localization::Tr("table.cLongCombatHelp.0"),
     localization::Tr("table.cLongCombatHelp.1"),
     localization::Tr("table.cLongCombatHelp.2"),
     localization::Tr("table.cLongCombatHelp.3"),
     localization::Tr("table.cLongCombatHelp.4")
};
const char* cTownCommand[KB_TOWN_COMMAND_COUNT] = {
     localization::Tr("table.cTownCommand.0"),
      localization::Tr("table.cTownCommand.1"),
     localization::Tr("table.cTownCommand.2"),
     localization::Tr("table.cTownCommand.3"),
     localization::Tr("table.cTownCommand.4"),
     localization::Tr("table.cTownCommand.5"),
     localization::Tr("table.cTownCommand.6"),
     localization::Tr("table.cTownCommand.7"),
     localization::Tr("table.cTownCommand.8"),
      "",
     localization::Tr("table.cTownCommand.10"),
     localization::Tr("table.cTownCommand.11"),
      "%s",
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
const char* gHeroDefaultNames[GAME_HERO_COUNT] = {
     localization::Tr("table.gHeroDefaultNames.0"), localization::Tr("table.gHeroDefaultNames.1"), localization::Tr("table.gHeroDefaultNames.2"), localization::Tr("table.gHeroDefaultNames.3"), localization::Tr("table.gHeroDefaultNames.4"), localization::Tr("table.gHeroDefaultNames.5"), localization::Tr("table.gHeroDefaultNames.6"),
     localization::Tr("table.gHeroDefaultNames.7"), localization::Tr("table.gHeroDefaultNames.8"), localization::Tr("table.gHeroDefaultNames.9"), localization::Tr("table.gHeroDefaultNames.10"), localization::Tr("table.gHeroDefaultNames.11"), localization::Tr("table.gHeroDefaultNames.12"), localization::Tr("table.gHeroDefaultNames.13"),
     localization::Tr("table.gHeroDefaultNames.14"), localization::Tr("table.gHeroDefaultNames.15"), localization::Tr("table.gHeroDefaultNames.16"), localization::Tr("table.gHeroDefaultNames.17"), localization::Tr("table.gHeroDefaultNames.18"), localization::Tr("table.gHeroDefaultNames.19"), localization::Tr("table.gHeroDefaultNames.20"),
     localization::Tr("table.gHeroDefaultNames.21"), localization::Tr("table.gHeroDefaultNames.22"), localization::Tr("table.gHeroDefaultNames.23"), localization::Tr("table.gHeroDefaultNames.24"), localization::Tr("table.gHeroDefaultNames.25"), localization::Tr("table.gHeroDefaultNames.26"), localization::Tr("table.gHeroDefaultNames.27"),
     localization::Tr("table.gHeroDefaultNames.28"), localization::Tr("table.gHeroDefaultNames.29"), localization::Tr("table.gHeroDefaultNames.30"), localization::Tr("table.gHeroDefaultNames.31"), localization::Tr("table.gHeroDefaultNames.32"), localization::Tr("table.gHeroDefaultNames.33"), localization::Tr("table.gHeroDefaultNames.34"),
     localization::Tr("table.gHeroDefaultNames.35"), localization::Tr("table.gHeroDefaultNames.36"), localization::Tr("table.gHeroDefaultNames.37"), localization::Tr("table.gHeroDefaultNames.38"), localization::Tr("table.gHeroDefaultNames.39"), localization::Tr("table.gHeroDefaultNames.40"), localization::Tr("table.gHeroDefaultNames.41"),
     localization::Tr("table.gHeroDefaultNames.42"), localization::Tr("table.gHeroDefaultNames.43"), localization::Tr("table.gHeroDefaultNames.44"), localization::Tr("table.gHeroDefaultNames.45"), localization::Tr("table.gHeroDefaultNames.46"), localization::Tr("table.gHeroDefaultNames.47"), localization::Tr("table.gHeroDefaultNames.48"),
     localization::Tr("table.gHeroDefaultNames.49"), localization::Tr("table.gHeroDefaultNames.50"), localization::Tr("table.gHeroDefaultNames.51"), localization::Tr("table.gHeroDefaultNames.52"), localization::Tr("table.gHeroDefaultNames.53")
};
const char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT] = {
     localization::Tr("table.gNewGameHelp.0"),
     localization::Tr("table.gNewGameHelp.1"),
     localization::Tr("table.gNewGameHelp.2"),
     localization::Tr("table.gNewGameHelp.3"),
     localization::Tr("table.gNewGameHelp.4"),
     localization::Tr("table.gNewGameHelp.5"),
     localization::Tr("table.gNewGameHelp.6"),
     localization::Tr("table.gNewGameHelp.7")
};
const char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT] = {
     localization::Tr("table.gSetupBaudHelp.0"),
     localization::Tr("table.gSetupBaudHelp.1"),
     localization::Tr("table.gSetupBaudHelp.2"),
     localization::Tr("table.gSetupBaudHelp.3"),
     localization::Tr("table.gSetupBaudHelp.4")
};
const char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT] = {
     localization::Tr("table.gSetupComPortHelp.0"),
     localization::Tr("table.gSetupComPortHelp.1"),
     localization::Tr("table.gSetupComPortHelp.2"),
     localization::Tr("table.gSetupComPortHelp.3"),
     localization::Tr("table.gSetupComPortHelp.4")
};
const char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT] = {
     localization::Tr("table.gSetupDCBaudHelp.0"),
     localization::Tr("table.gSetupDCBaudHelp.1"),
     localization::Tr("table.gSetupDCBaudHelp.2"),
     localization::Tr("table.gSetupDCBaudHelp.3"),
     localization::Tr("table.gSetupDCBaudHelp.4")
};
const char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT] = {
     localization::Tr("table.gSetupDCComPortHelp.0"),
     localization::Tr("table.gSetupDCComPortHelp.1"),
     localization::Tr("table.gSetupDCComPortHelp.2"),
     localization::Tr("table.gSetupDCComPortHelp.3"),
     localization::Tr("table.gSetupDCComPortHelp.4")
};
const char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT] = {
     localization::Tr("table.gSetupHotSeatGameHelp.0"),
     localization::Tr("table.gSetupHotSeatGameHelp.1"),
     localization::Tr("table.gSetupHotSeatGameHelp.2"),
     localization::Tr("table.gSetupHotSeatGameHelp.3"),
     localization::Tr("table.gSetupHotSeatGameHelp.4"),
     localization::Tr("table.gSetupHotSeatGameHelp.5")
};
const char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT] = {
     localization::Tr("table.gSetupModemGameHelp.0"),

    (localization::Tr("table.gSetupModemGameHelp.1")),
     localization::Tr("table.gSetupModemGameHelp.2"),
     localization::Tr("table.gSetupModemGameHelp.3")
};
const char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT] = {
     localization::Tr("table.gSetupDCGameHelp.0"),

    (localization::Tr("table.gSetupDCGameHelp.1")),
     localization::Tr("table.gSetupDCGameHelp.2"),
     localization::Tr("table.gSetupDCGameHelp.3")
};
const char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT] = {
     localization::Tr("table.gSetupMultiPlayerGameHelp.0"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.1"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.2"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.3"),
     localization::Tr("table.gSetupMultiPlayerGameHelp.4")
};
const char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT] = {
     localization::Tr("table.gSetupNetworkGameHelp.0"),
     localization::Tr("table.gSetupNetworkGameHelp.1"),
     localization::Tr("table.gSetupNetworkGameHelp.2")
};
const char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT] = {
     localization::Tr("table.gSetupNetworkGame2Help.0"),
     localization::Tr("table.gSetupNetworkGame2Help.1"),
     localization::Tr("table.gSetupNetworkGame2Help.2"),
     localization::Tr("table.gSetupNetworkGame2Help.3")
};
const char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT] = {
     localization::Tr("table.gSetupGameHelp.0"),
     localization::Tr("table.gSetupGameHelp.1"),
     localization::Tr("table.gSetupGameHelp.2"),
     localization::Tr("table.gSetupGameHelp.3")
};
const char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT] = {
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
const char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT] = {
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
const char* cMapSize[KB_MAP_SIZE_TEXT_COUNT] = { localization::Tr("table.cMapSize.0"), localization::Tr("table.cMapSize.1"), localization::Tr("table.cMapSize.2"), localization::Tr("table.cMapSize.3")};
const char* cDifficulty[H2EnumIndex(DIFFICULTY_COUNT)] =
    { localization::Tr("table.cDifficulty.0"), localization::Tr("table.cDifficulty.1"), localization::Tr("table.cDifficulty.2"), localization::Tr("table.cDifficulty.3"), localization::Tr("table.cDifficulty.4")};
const char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT] = { localization::Tr("table.cStartDifficulty.0"), localization::Tr("table.cStartDifficulty.1"), localization::Tr("table.cStartDifficulty.2"), localization::Tr("table.cStartDifficulty.3")};
const char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT] =
    { localization::Tr("table.cCampaignLeaders.0"), localization::Tr("table.cCampaignLeaders.1"), localization::Tr("table.cCampaignLeaders.2"), localization::Tr("table.cCampaignLeaders.3")};
const char* cWinText[KB_WIN_TEXT_COUNT] =
    { localization::Tr("table.cWinText.0"), localization::Tr("table.cWinText.1"), localization::Tr("table.cWinText.2"), localization::Tr("table.cWinText.3"), localization::Tr("table.cWinText.4")};
const char* cHumanDifficulty[H2EnumIndex(DIFFICULTY_COUNT)] =
    { localization::Tr("table.cHumanDifficulty.0"), localization::Tr("table.cHumanDifficulty.1"), localization::Tr("table.cHumanDifficulty.2"), localization::Tr("table.cHumanDifficulty.3"), localization::Tr("table.cHumanDifficulty.4")};
const char* cHumanInfoDifficulty[H2EnumIndex(DIFFICULTY_COUNT)] =
    { localization::Tr("table.cHumanInfoDifficulty.0"), localization::Tr("table.cHumanInfoDifficulty.1"), localization::Tr("table.cHumanInfoDifficulty.2"), localization::Tr("table.cHumanInfoDifficulty.3"), localization::Tr("table.cHumanInfoDifficulty.4")};
const char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT] =
    {  "MIDI", localization::Tr("table.musicQualityText.1"), localization::Tr("table.musicQualityText.2")};
const char* gSpellDesc[H2EnumIndex(SPELL_COUNT)] = {
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
const char* gSpellNames[H2EnumIndex(SPELL_COUNT)] = {
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
const char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT] =
    { localization::Tr("table.gSecondarySkillLevels.0"), localization::Tr("table.gSecondarySkillLevels.1"), localization::Tr("table.gSecondarySkillLevels.2")};
const char* gSecondarySkills[H2EnumIndex(HERO_SKILL_COUNT)] = {
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
const char* gNeutralBuildingNames[KB_NEUTRAL_BUILDING_TEXT_COUNT] = {
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
      "",
     localization::Tr("table.gNeutralBuildingNames.12"),
      "",
     localization::Tr("table.gNeutralBuildingNames.14"),
     localization::Tr("table.gNeutralBuildingNames.15"),
      "",
      "",
      ""
};
const char* gWellExtraNames[KB_WELL_EXTRA_NAME_COUNT] = {
     localization::Tr("table.gWellExtraNames.0"),
     localization::Tr("table.gWellExtraNames.1"),
     localization::Tr("table.gWellExtraNames.2"),
     localization::Tr("table.gWellExtraNames.3"),
     localization::Tr("table.gWellExtraNames.4"),
     localization::Tr("table.gWellExtraNames.5"),
     localization::Tr("table.gWellExtraNames.6")
};
const char* gSpecialBuildingNames[KB_SPECIAL_BUILDING_NAME_COUNT] =
    { localization::Tr("table.gSpecialBuildingNames.0"), localization::Tr("table.gSpecialBuildingNames.1"), localization::Tr("table.gSpecialBuildingNames.2"), localization::Tr("table.gSpecialBuildingNames.3"), localization::Tr("table.gSpecialBuildingNames.4"), localization::Tr("table.gSpecialBuildingNames.5"), localization::Tr("table.gSpecialBuildingNames.6")};
const char* gDwellingNames[H2EnumIndex(FACTION_COUNT)][DWELLING_TYPE_COUNT] = {
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
       ""},
    {localization::Tr("table.gDwellingNames.1.0"),
     localization::Tr("table.gDwellingNames.1.1"),
     localization::Tr("table.gDwellingNames.1.2"),
     localization::Tr("table.gDwellingNames.1.3"),
     localization::Tr("table.gDwellingNames.1.4"),
     localization::Tr("table.gDwellingNames.1.5"),
     localization::Tr("table.gDwellingNames.1.6"),
       "",
     localization::Tr("table.gDwellingNames.1.8"),
     localization::Tr("table.gDwellingNames.1.9"),
       "",
       ""},
    {localization::Tr("table.gDwellingNames.2.0"),
     localization::Tr("table.gDwellingNames.2.1"),
     localization::Tr("table.gDwellingNames.2.2"),
     localization::Tr("table.gDwellingNames.2.3"),
     localization::Tr("table.gDwellingNames.2.4"),
     localization::Tr("table.gDwellingNames.2.5"),
     localization::Tr("table.gDwellingNames.2.6"),
     localization::Tr("table.gDwellingNames.2.7"),
     localization::Tr("table.gDwellingNames.2.8"),
       "",
       "",
       ""},
    {localization::Tr("table.gDwellingNames.3.0"),
     localization::Tr("table.gDwellingNames.3.1"),
     localization::Tr("table.gDwellingNames.3.2"),
     localization::Tr("table.gDwellingNames.3.3"),
     localization::Tr("table.gDwellingNames.3.4"),
     localization::Tr("table.gDwellingNames.3.5"),
       "",
       "",
     localization::Tr("table.gDwellingNames.3.8"),
       "",
     localization::Tr("table.gDwellingNames.3.10"),
     localization::Tr("table.gDwellingNames.3.11")},
    {localization::Tr("table.gDwellingNames.4.0"),
     localization::Tr("table.gDwellingNames.4.1"),
     localization::Tr("table.gDwellingNames.4.2"),
     localization::Tr("table.gDwellingNames.4.3"),
     localization::Tr("table.gDwellingNames.4.4"),
     localization::Tr("table.gDwellingNames.4.5"),
       "",
     localization::Tr("table.gDwellingNames.4.7"),
       "",
     localization::Tr("table.gDwellingNames.4.9"),
     localization::Tr("table.gDwellingNames.4.10"),
       ""},
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
       "",
       ""}
};
const char* cSecSkillDesc[H2EnumIndex(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT] = {
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
const char* cBuildingInfoNeutral[KB_NEUTRAL_BUILDING_INFO_COUNT] = {
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
      "",
     localization::Tr("table.cBuildingInfoNeutral.12"),
      "",
     localization::Tr("table.cBuildingInfoNeutral.14"),
     localization::Tr("table.cBuildingInfoNeutral.15"),
      "",
      "",
      ""
};
const char* gBuildingInfoSpecial[KB_SPECIAL_BUILDING_INFO_COUNT] = {
     localization::Tr("table.gBuildingInfoSpecial.0"),
     localization::Tr("table.gBuildingInfoSpecial.1"),
     localization::Tr("table.gBuildingInfoSpecial.2"),
     localization::Tr("table.gBuildingInfoSpecial.3"),
     localization::Tr("table.gBuildingInfoSpecial.4"),
     localization::Tr("table.gBuildingInfoSpecial.5")
};
const char* cDirections[KB_DIRECTION_TEXT_COUNT] = {
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
const char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT] = {
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
const char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT] = { localization::Tr("table.gInterfaceTypeText.0"), localization::Tr("table.gInterfaceTypeText.1"), localization::Tr("table.gInterfaceTypeText.2")};
const char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT] = { localization::Tr("table.cBWMouseText.0"), localization::Tr("table.cBWMouseText.1")};
const char* combatSpeedText[KB_COMBAT_SPEED_COUNT] = { localization::Tr("table.combatSpeedText.0"), localization::Tr("table.combatSpeedText.1"), localization::Tr("table.combatSpeedText.2")};
const char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT] = { localization::Tr("table.combatMiniInfoText.0"), localization::Tr("table.combatMiniInfoText.1"), localization::Tr("table.combatMiniInfoText.2")};
const char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT] = {
      "\n\n\n***Command Line Help***\n",
      "\n",
     localization::Tr("system.command_line.disable_digital_sound"),
     localization::Tr("system.command_line.disable_midi"),
     localization::Tr("system.command_line.disable_music"),
     localization::Tr("system.command_line.skip_intro"),
      "\n",
      "\n",
     localization::Tr("system.command_line.example"),
      "\n",
      "HEROES2D /R0 /I0\n",
      "\n",
     localization::Tr("system.command_line.dos_example"),
     localization::Tr("system.command_line.disabled_example")
};
const char* cOverviewText[KB_OVERVIEW_TEXT_COUNT] =
    { localization::Tr("table.cOverviewText.0"), localization::Tr("table.cOverviewText.1"), localization::Tr("table.cOverviewText.2"), localization::Tr("table.cOverviewText.3"), localization::Tr("table.cOverviewText.4"), localization::Tr("table.cOverviewText.5")};
const char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT] = {
     localization::Tr("table.cWinComError.0"),
     localization::Tr("table.cWinComError.1"),
     localization::Tr("table.cWinComError.2"),
     localization::Tr("table.cWinComError.3"),
     localization::Tr("table.cWinComError.4"),
     localization::Tr("table.cWinComError.5")
};
const char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT] =
    { localization::Tr("table.cMiniViewText.0"), localization::Tr("table.cMiniViewText.1"), localization::Tr("table.cMiniViewText.2"), localization::Tr("table.cMiniViewText.3"), localization::Tr("table.cMiniViewText.4"), localization::Tr("table.cMiniViewText.5"), localization::Tr("table.cMiniViewText.6"), localization::Tr("table.cMiniViewText.7"), localization::Tr("table.cMiniViewText.8")};
const char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT] = {
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
const char* cPersonality[KB_PERSONALITY_TEXT_COUNT] = { localization::Tr("table.cPersonality.0"), localization::Tr("table.cPersonality.1"), localization::Tr("table.cPersonality.2"), localization::Tr("table.cPersonality.3")};
const char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT] = {
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
const char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT] = {
     localization::Tr("table.cRandomTavernText.0"),
     localization::Tr("table.cRandomTavernText.1"),
     localization::Tr("table.cRandomTavernText.2"),
     localization::Tr("table.cRandomTavernText.3"),
     localization::Tr("table.cRandomTavernText.4"),
     localization::Tr("table.cRandomTavernText.5"),
     localization::Tr("table.cRandomTavernText.6"),
     localization::Tr("table.cRandomTavernText.7")
};
const char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT] =
    { localization::Tr("table.cRandomSignText.0"), localization::Tr("table.cRandomSignText.1"), localization::Tr("table.cRandomSignText.2"), localization::Tr("table.cRandomSignText.3")};
const char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT] = {
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
const char* cCampaignName[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
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
       "",
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
const char* cCampaignDescription[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT] = {
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
       "",
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
const char* cOutOfMemory =
     localization::Tr("system.memory.requirement");
const char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT] = { localization::Tr("table.cSlowVideoLevelText.0"), localization::Tr("table.cSlowVideoLevelText.1")};
const char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT] = {
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
const char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT] =
    { localization::Tr("table.xBarrierColor.0"), localization::Tr("table.xBarrierColor.1"), localization::Tr("table.xBarrierColor.2"), localization::Tr("table.xBarrierColor.3"), localization::Tr("table.xBarrierColor.4"), localization::Tr("table.xBarrierColor.5"), localization::Tr("table.xBarrierColor.6"), localization::Tr("table.xBarrierColor.7")};
const char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT] = {
     localization::Tr("table.xGenericSiteNames.0"),
     localization::Tr("table.xGenericSiteNames.1"),
     localization::Tr("table.xGenericSiteNames.2"),
     localization::Tr("table.xGenericSiteNames.3"),
     localization::Tr("table.xGenericSiteNames.4"),
     localization::Tr("table.xGenericSiteNames.5"),
     localization::Tr("table.xGenericSiteNames.6")
};
const char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT] = {
     localization::Tr("table.xRecruitmentSiteNames.0"),
     localization::Tr("table.xRecruitmentSiteNames.1"),
     localization::Tr("table.xRecruitmentSiteNames.2"),
     localization::Tr("table.xRecruitmentSiteNames.3"),
     localization::Tr("table.xRecruitmentSiteNames.4")
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

char cOverrideMIDIDriver[GLOBAL_DRIVER_NAME_SIZE];
u8 bSaveMusicPosition[MIDI_TRACK_COUNT];
u16 gTimeEventExtras[EDITOR_TIME_EVENT_CAPACITY];
class mouseManager* gpMouseManager;
char gText[GLOBAL_TEXT_BUFFER_SIZE];

i32 gUnusedData4a3e80Cache[3];
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
u16 gRumourExtras[EDITOR_RUMOUR_CAPACITY];
heroWindow* pNormalDialogWindow;
u8 bMusicIsLooping[MIDI_TRACK_COUNT];
heroWindow* gEditDialog;
i32 gSelectionHeight;
soundManager* gpSoundManager;
char gLastFilename[GLOBAL_LAST_FILENAME_SIZE];
i32 gStatusTextHoldTime;

i32 gUnusedData4a42fcBlock[2];
mapCell* gEditCell;
class palette* gpBufferPalette;
palette* gPalette;
char cAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
char gcRegAppPath[GLOBAL_AGGREGATE_PATH_SIZE];
i32 giMaxExtentX;
i32 giMaxExtentY;

i32 gUnusedData4a45d8Instance[2];
inputManager* gpInputManager;
char cOverrideDigitalDriver[GLOBAL_DRIVER_NAME_SIZE];

i32 gNextObjectLink;
char gcCommandLine[GLOBAL_COMMAND_LINE_SIZE];
configStruct gConfig;

i32 gUnusedData4a47d8StateBlock;
i32 giMinExtentX;
i32 giMinExtentY;
executive* gpExec;
i32 gLandCellCount;
i32 giCurWindowsStyleFlags;
char gcRegCDRomPath[GLOBAL_AGGREGATE_PATH_SIZE];
i32 glTimers[GLOBAL_TIMER_COUNT];

i32 gUnusedData4a4978Runtime[5];

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
    gpMouseManager->SetColorMice(gConfig.gfx[H2EnumIndex(giCurExe)].colorMouseCursor);
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
            case SETUP_CHOICE_TWO:
                if (PickMap(FILE_REQUESTER_MAP))
                    keepRunning = false;
                sprintf(loadName, gMapFileName);
                break;
            case SETUP_CHOICE_ONE:
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
    if (result == SETUP_CHOICE_TWO) {
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

    sprintf(message, localization::Tr("system.file.open_error"), filename);
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

void MemError(void) {
    if (gbInMemError)
        return;
    gbInMemError = true;
    ShutDown(localization::Tr("system.memory.out_of_memory"));
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
    i32 firstResourceType [[maybe_unused]],
    i32 firstResourceValue [[maybe_unused]],
    i32 secondResourceType [[maybe_unused]],
    i32 secondResourceValue [[maybe_unused]],
    i32 showOrText [[maybe_unused]],
    i32 timeout [[maybe_unused]]
) {
    i32 resourceFrame [[maybe_unused]];
    i16 showMessage [[maybe_unused]];
    i32 textWidgetId [[maybe_unused]];
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
    message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED) | H2EnumIndex(WIDGET_FLAG_DRAW);
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


void ShowStatusText(const char* text) {
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
        gEditManager->m_window->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
        gpWindowManager->UpdateScreenRegion(
            EDITOR_STATUS_BAR_X,
            EDITOR_STATUS_BAR_Y,
            EDITOR_STATUS_BAR_WIDTH,
            EDITOR_STATUS_BAR_HEIGHT
        );
    }
}

void UpdateAppSpecificMenus(void* hMenu [[maybe_unused]]) {}

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
        gEditManager->SelectTool(EDIT_TOOL_NONE);
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

void EarlyResizeWindow(i32 x [[maybe_unused]], i32 y [[maybe_unused]], i32 width [[maybe_unused]],
                       i32 height [[maybe_unused]]) {}

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
