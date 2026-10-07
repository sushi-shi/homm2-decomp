#ifndef HOMM2_EDITOR_EDITOR_H
#define HOMM2_EDITOR_EDITOR_H

// The scenario editor's program unit (src/EDITOR/EDITOR.cpp): start-up and
// shut-down, the main classes, the delay and dialog helpers, the status bar
// and the application-menu hooks kbwin calls. It began as a copy of the
// game's KB.cpp and keeps its names for what both programs define.

#include <va.h>
#include <Ints.h>
#include <BASE/message.h>
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
    // The eraser panel's help: its brushes, then the object classes it erases.
    CLEAR_HELP_COUNT = 20,
    // The random map generator's terrain and density settings.
    RANDOM_MAP_TERRAIN_COUNT = 8,
    RANDOM_MAP_DENSITY_COUNT = 5,
    // gWinSetup: the editor dialogs' captions.
    EDITOR_DIALOG_WIN_SETUP_COUNT = 0x74,
    // The map's time event and rumour capacities, and its player colours.
    EDITOR_TIME_EVENT_CAPACITY = 50,
    EDITOR_RUMOUR_CAPACITY = 30,
    EDITOR_PLAYER_COLOR_COUNT = 6
H2_ENUM_END(EditorTableCount)

H2_ENUM_BEGIN(RandomMapDensity)
    // gDensityPercent's rows (RANDOM_MAP_DENSITY_COUNT).
    RANDOM_MAP_DENSITY_MOUNTAINS = 0,
    RANDOM_MAP_DENSITY_TREES     = 1,
    RANDOM_MAP_DENSITY_OBJECTS   = 2,
    RANDOM_MAP_DENSITY_TREASURE  = 3,
    RANDOM_MAP_DENSITY_MONSTERS  = 4
H2_ENUM_END(RandomMapDensity)

H2_ENUM_BEGIN(EditorWinText)
    // The dialogs whose texts SetWinText fills from gWinSetup.
    EDITOR_WIN_TEXT_SYSTEM_OPTIONS    = 3,
    EDITOR_WIN_TEXT_EVENT             = 4,
    EDITOR_WIN_TEXT_HERO              = 5,
    EDITOR_WIN_TEXT_MONSTER           = 8,
    EDITOR_WIN_TEXT_SPHINX            = 10,
    // The rumour dialog, which the sign editor reuses.
    EDITOR_WIN_TEXT_RUMOUR            = 11,
    EDITOR_WIN_TEXT_SPECIFICATIONS    = 13,
    EDITOR_WIN_TEXT_TOWN              = 15,
    EDITOR_WIN_TEXT_ULTIMATE_ARTIFACT = 16
H2_ENUM_END(EditorWinText)

extern i32 gRandomMapPlayers;
extern double gTerrainPercent[RANDOM_MAP_TERRAIN_COUNT];
extern double gDensityPercent[RANDOM_MAP_DENSITY_COUNT];
// The random map generator spreads each terrain's patches over the whole
// map, or (unset) gathers them toward its centre.
extern b32 gScatterTerrain;
extern b32 gGenerateUnseen;
extern b32 gGeneratingMap;
extern i32 gObjectClass;
#define gNextObjectLink gNextObjectLinkValue // spelling fixes .bss order
extern i32 gNextObjectLink;
// The map's land cells, as RemoveSmallRegions last counted them.
#define gLandCellCount gLandCellCountInfoCore // spelling fixes .bss order
extern i32 gLandCellCount;
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
// The eraser panel's help: its brushes, then the object classes it erases.
extern H2_CONST char* gClearHelp[CLEAR_HELP_COUNT];

// The editor manager (InitMainClasses).
class editManager;
#define gEditManager gpEditManager // spelling fixes .bss order
extern editManager* gEditManager;
// The drag selection the map view outlines (EDIT_NO_CELL when there is none).
extern i32 gSelectionX;
#define gSelectionY gSelectionYBlock // spelling fixes .bss order
extern i32 gSelectionY;
#define gSelectionWidth gSelectionWidthBufferShared // spelling fixes .bss order
extern i32 gSelectionWidth;
#define gSelectionHeight gSelectionHeightRuntimeTable // spelling fixes .bss order
extern i32 gSelectionHeight;

// The object dialog the detail tool has open, and the map cell it edits.
// The compiled spellings keep EDITOR's .bss in its name-hash order.
#define gEditDialog gEditDlg // spelling fixes .bss order
#define gEditCell gpCell     // spelling fixes .bss order
extern class heroWindow* gEditDialog;
extern class mapCell* gEditCell;

#define gMaps gMapsStorage // spelling fixes .bss order
extern fullMap gMaps[EDIT_MAP_COPIES];
#define gMap (gMaps[EDIT_MAP_CURRENT])
#define gUndoMap (gMaps[EDIT_MAP_UNDO])

// The map file being edited (an 8.3 name).
#define gMapFileName gMapFileNameInfo // spelling fixes .bss order
extern char gMapFileName[EDITOR_MAP_FILE_NAME_SIZE];
extern char gShippedMaps[EDITOR_SHIPPED_MAP_COUNT][EDITOR_SHIPPED_MAP_NAMES]
                        [EDITOR_SHIPPED_MAP_NAME_SIZE];
extern i32 gClearFlags;
#define gStatusText gStatusTextStore // spelling fixes .bss order
extern char gStatusText[EDITOR_STATUS_TEXT_SIZE];
extern b32 gStatusTextShown;
#define gStatusTextHoldTime gStatusTextHoldTimeFieldMemory // spelling fixes .bss order
extern i32 gStatusTextHoldTime;
extern i32 gStatusTextClearTime;
// The map's time events and rumours: their map-extra record indices, in
// list order (counted by gEditMapHeader's timeEventCount and rumourCount).
#define gTimeEventExtras gTimeEventExtrasBacking // spelling fixes .bss order
extern u16 gTimeEventExtras[EDITOR_TIME_EVENT_CAPACITY];
#define gRumourExtras gRumourExtrasSlotContent // spelling fixes .bss order
extern u16 gRumourExtras[EDITOR_RUMOUR_CAPACITY];
// The player colours' short names (the specification dialog's side lists).
extern H2_CONST char* gColorAbbreviations[EDITOR_PLAYER_COLOR_COUNT];

class heroWindow;
struct tag_message;

// The game's KB.cpp functions the editor keeps its own copies of (the game
// declares them in KBDeclarations.h and NOOPT.h).
extern "C" void PollSound(void);
i32 oldmain(void);
void DelayTil(i32* endTime);
void DelayMilli(i32l delay);
void DelayTilMilli(i32l endTime);
void FileError(H2_CONST char* filename);
void ShutDown(H2_CONST char* message);
i32 InterpretCommandLine(void);
void EarlyShutdown(H2_CONST char* caption, H2_CONST char* text);
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

// The editor's own: an edited copy of a shipped map gets a new name, the
// status line's texts, and three hooks nothing calls.
void ProtectShippedMap(void);
void IncrementArgumentA(i32 value);
void EditorIdleHook(void);
void IncrementArgumentB(i32 value);
void DelayTicks(i32 ticks);
void ShowStatusText(char* text);
void ClearStatusText(void);

#endif
