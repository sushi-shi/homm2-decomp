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

H2_ENUM_BEGIN(EditorTableCount)
    // The map view's zoom levels.
    EDIT_ZOOM_COUNT = 3,
    // The road and stream tools' neighbour-mask tables.
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
    // The random map generator's terrain and density settings.
    RANDOM_MAP_TERRAIN_COUNT = 8,
    RANDOM_MAP_DENSITY_COUNT = 5,
    // gWinSetup: the editor dialogs' captions.
    EDITOR_DIALOG_WIN_SETUP_COUNT = 0x74
H2_ENUM_END(EditorTableCount)

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
extern H2_CONST char* gTerrainHelp[EDITOR_TERRAIN_HELP_COUNT];
extern H2_CONST char* gEditPanelHelp[EDIT_PANEL_HELP_COUNT];
extern H2_CONST char* gEditTerrainNames[EDITOR_TERRAIN_NAME_COUNT];
extern H2_CONST char* gObjectClassNames[EDITOR_OBJECT_CLASS_COUNT];
extern H2_CONST char* gSetupNewMapHelp[SETUP_NEW_MAP_HELP_COUNT];
extern H2_CONST char* gSetupMapSizeHelp[SETUP_MAP_SIZE_HELP_COUNT];
extern H2_CONST char* gSetupMainHelp[SETUP_MAIN_HELP_COUNT];
extern H2_CONST char* gEventFrequencyNames[EVENT_FREQUENCY_COUNT];
extern H2_CONST char* gTownNames[EDITOR_TOWN_NAME_COUNT];
extern H2_CONST char* gFileMenuHelp[EDIT_FILE_MENU_HELP_COUNT];
extern H2_CONST char* gSystemOptionsHelp[EDIT_SYSTEM_OPTIONS_HELP_COUNT];
extern H2_CONST char* gVictoryConditionNames[SPEC_VICTORY_CONDITION_COUNT];
extern H2_CONST char* gLossConditionNames[SPEC_LOSS_CONDITION_COUNT];

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
// The object dialog an event editor has open, and the map cell it edits.
extern class heroWindow* gEditDialog;
extern class mapCell* gEditCell;

void ProtectShippedMap(void);
void IncrementArgumentA(i32 value);
void EditorIdleHook(void);
void IncrementArgumentB(i32 value);
void DelayTicks(i32 ticks);
void ShowStatusText(char* text);
void ClearStatusText(void);

#endif
