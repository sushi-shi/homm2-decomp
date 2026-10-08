#ifndef HOMM2_EDITOR_EDITOR_H
#define HOMM2_EDITOR_EDITOR_H


#include <Domains.h>
#include <BASE/message.h>
#include <EDITOR/fullMap.h>
#include <SOURCE/gameTypes.h>

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

    ROAD_NEIGHBOUR_MASKS = 256,
    STREAM_NEIGHBOUR_MASKS = 16,
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

    CLEAR_HELP_COUNT = 20,

    RANDOM_MAP_TERRAIN_COUNT = 8,
    RANDOM_MAP_DENSITY_COUNT = 5,

    EDITOR_DIALOG_WIN_SETUP_COUNT = 0x74,

    EDITOR_TIME_EVENT_CAPACITY = 50,
    EDITOR_RUMOUR_CAPACITY = 30
} EditorTableCount;

typedef enum RandomMapDensity {

    RANDOM_MAP_DENSITY_MOUNTAINS = 0,
    RANDOM_MAP_DENSITY_TREES     = 1,
    RANDOM_MAP_DENSITY_OBJECTS   = 2,
    RANDOM_MAP_DENSITY_TREASURE  = 3,
    RANDOM_MAP_DENSITY_MONSTERS  = 4
} RandomMapDensity;

typedef enum EditorWinText {

    EDITOR_WIN_TEXT_SYSTEM_OPTIONS    = 3,
    EDITOR_WIN_TEXT_EVENT             = 4,
    EDITOR_WIN_TEXT_HERO              = 5,
    EDITOR_WIN_TEXT_MONSTER           = 8,
    EDITOR_WIN_TEXT_SPHINX            = 10,

    EDITOR_WIN_TEXT_RUMOUR            = 11,
    EDITOR_WIN_TEXT_SPECIFICATIONS    = 13,
    EDITOR_WIN_TEXT_TOWN              = 15,
    EDITOR_WIN_TEXT_ULTIMATE_ARTIFACT = 16
} EditorWinText;

extern i32 gRandomMapPlayers;
extern double gTerrainPercent[RANDOM_MAP_TERRAIN_COUNT];
extern double gDensityPercent[RANDOM_MAP_DENSITY_COUNT];


extern b32 gScatterTerrain;
extern b32 gGenerateUnseen;
extern b32 gGeneratingUnseen;
extern i32 gObjectClass;
extern i32 gNextObjectLink;

extern i32 gLandCellCount;
extern i32 gZoomScale[EDIT_ZOOM_COUNT];
extern i32 gZoomCellSize[EDIT_ZOOM_COUNT];
extern i32 gZoomViewCells[EDIT_ZOOM_COUNT];
extern i32 gZoomTileSize[EDIT_ZOOM_COUNT];
extern u8 gRoadSideTiles[ROAD_NEIGHBOUR_MASKS];
extern u8 gRoadTiles[ROAD_NEIGHBOUR_MASKS];
extern u8 gStreamTiles[STREAM_NEIGHBOUR_MASKS];
extern u8 gRoadTileOnLine[LINE_ROAD_TILES];
extern u8 gRoadTileIsRoad[LINE_ROAD_TILES];
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

extern const char* gObjectPanelHelp[CLEAR_HELP_COUNT];


class editManager;
extern editManager* gEditManager;

extern i32 gSelectionX;
extern i32 gSelectionY;
extern i32 gSelectionWidth;
extern i32 gSelectionHeight;


extern class heroWindow* gEditDialog;
extern class mapCell* gEditCell;

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


extern u16 gTimeEventExtras[EDITOR_TIME_EVENT_CAPACITY];
extern u16 gRumourExtras[EDITOR_RUMOUR_CAPACITY];

extern const char* gColorAbbreviations[PLAYER_COLOR_COUNT];

class heroWindow;
struct tag_message;


extern "C" void PollSound(void);
i32 oldmain(void);
void DelayTil(i32* endTime);
void DelayMilli(i32l delay);
void DelayTilMilli(i32l endTime);
void FileError(const char* filename);
void ShutDown(const char* message);
i32 InterpretCommandLine(void);
void EarlyShutdown(const char* caption, const char* text);
i32 EarlySetup(void);
void MemError(void);
void InitMainClasses(void);
void DeleteMainClasses(void);
MessageDispatchResult EventWindowHandler(struct tag_message& message);
void QuickViewWait(void);
void UpdateAppSpecificMenus(void* hMenu);
void CleanUpMenus(void);
void EarlyShutDownSystem(void);
i32 GameUnsaved(void);
i32 HandleAppSpecificMenuCommands(i32 command);
void EarlyResizeWindow(i32 x, i32 y, i32 width, i32 height);
void UpdateSystemOptionsMenu(void);
void SetWinText(heroWindow* window, i32 id);


void ProtectShippedMap(void);
void IncrementArgumentA(i32 value);
void EditorIdleHook(void);
void IncrementArgumentB(i32 value);
void DelayTicks(i32 ticks);
void ShowStatusText(const char* text);
void ClearStatusText(void);

#endif
