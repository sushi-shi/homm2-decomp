#ifndef HOMM2_EDITOR_EDITOR_H
#define HOMM2_EDITOR_EDITOR_H


#include <Ints.h>
#include <Ints.h>
#include <EDITOR/fullMap.h>

typedef enum EditorStatusBar {

    EDITOR_STATUS_BAR_X                 = 0,
    EDITOR_STATUS_BAR_Y                 = 0x1d0,
    EDITOR_STATUS_BAR_WIDTH             = 0x1e0,
    EDITOR_STATUS_BAR_HEIGHT            = 0x10,
    EDITOR_STATUS_TEXT_Y                = 0x1d3,
    EDITOR_STATUS_TEXT_HOLD_MILLISECONDS = 3000,
    EDITOR_STATUS_TEXT_SIZE             = 200,

    EDITOR_STATUS_TEXT_KEPT             = 0
} EditorStatusBar;

typedef enum EditorFileConstant {
    EDITOR_MAP_FILE_NAME_SIZE   = 16,

    EDITOR_CLEAR_FLAGS_DEFAULT  = 0x3fff
} EditorFileConstant;

typedef enum EditorMapCopy {

    EDIT_MAP_CURRENT = 0,
    EDIT_MAP_UNDO    = 1,
    EDIT_MAP_COPIES  = 2
} EditorMapCopy;

typedef enum EditorShippedMap {


    EDITOR_SHIPPED_MAP_COUNT     = 36,
    EDITOR_SHIPPED_MAP_NAME_SIZE = 13,
    EDITOR_SHIPPED_MAP_NAMES     = 2
} EditorShippedMap;

typedef enum EditorTableCount {

    EDIT_ZOOM_COUNT = 3,

    LINE_NEIGHBOUR_MASKS = 256,
    LINE_END_MASKS = 16,
    LINE_ROAD_TILES = 32,
    EDITOR_TERRAIN_HELP_COUNT = 14,
    EDIT_PANEL_HELP_COUNT = 16,
    EDITOR_TERRAIN_NAME_COUNT = 9,
    EDITOR_OBJECT_CLASS_COUNT = 16,
    SETUP_NEW_MAP_HELP_COUNT = 3,
    SETUP_MAP_SIZE_HELP_COUNT = 5,
    SETUP_MAIN_HELP_COUNT = 3,
    EVENT_FREQUENCY_COUNT = 11,
    EDITOR_TOWN_NAME_COUNT = 72,
    EDIT_FILE_MENU_HELP_COUNT = 5,
    EDIT_SYSTEM_OPTIONS_HELP_COUNT = 5,
    SPEC_VICTORY_CONDITION_COUNT = 6,
    SPEC_LOSS_CONDITION_COUNT = 4,

    RANDOM_MAP_TERRAIN_COUNT = 8,
    RANDOM_MAP_DENSITY_COUNT = 5,

    EDITOR_DIALOG_WIN_SETUP_COUNT = 0x74
} EditorTableCount;

extern i32 gRandomMapPlayers;
extern double gTerrainPercent[RANDOM_MAP_TERRAIN_COUNT];
extern double gDensityPercent[RANDOM_MAP_DENSITY_COUNT];
extern b32 gScatterTowns;
extern b32 gGenerateUnseen;
extern b32 gGeneratingMap;
extern i32 gObjectClass;
extern i32 gZoomScale[EDIT_ZOOM_COUNT];
extern i32 gZoomCellSize[EDIT_ZOOM_COUNT];
extern i32 gZoomViewCells[EDIT_ZOOM_COUNT];
extern i32 gZoomTileSize[EDIT_ZOOM_COUNT];
extern u8 gLineTiles[LINE_NEIGHBOUR_MASKS];
extern u8 gLineEdgeTiles[LINE_NEIGHBOUR_MASKS];
extern u8 gLineEndTiles[LINE_END_MASKS];
extern u8 gRoadTileJoins[LINE_ROAD_TILES];
extern u8 gRoadTileJoinsAlt[LINE_ROAD_TILES];
extern const char* gTerrainHelp[EDITOR_TERRAIN_HELP_COUNT];
extern const char* gEditPanelHelp[EDIT_PANEL_HELP_COUNT];
extern const char* gEditTerrainNames[EDITOR_TERRAIN_NAME_COUNT];
extern const char* gObjectClassNames[EDITOR_OBJECT_CLASS_COUNT];
extern const char* gSetupNewMapHelp[SETUP_NEW_MAP_HELP_COUNT];
extern const char* gSetupMapSizeHelp[SETUP_MAP_SIZE_HELP_COUNT];
extern const char* gSetupMainHelp[SETUP_MAIN_HELP_COUNT];
extern const char* gEventFrequencyNames[EVENT_FREQUENCY_COUNT];
extern const char* gTownNames[EDITOR_TOWN_NAME_COUNT];
extern const char* gFileMenuHelp[EDIT_FILE_MENU_HELP_COUNT];
extern const char* gSystemOptionsHelp[EDIT_SYSTEM_OPTIONS_HELP_COUNT];
extern const char* gVictoryConditionNames[SPEC_VICTORY_CONDITION_COUNT];
extern const char* gLossConditionNames[SPEC_LOSS_CONDITION_COUNT];

extern fullMap gMaps[EDIT_MAP_COPIES];
#define gMap (gMaps[EDIT_MAP_CURRENT])
#define gUndoMap (gMaps[EDIT_MAP_UNDO])


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
