#ifndef HOMM2_ADVMGR_H
#define HOMM2_ADVMGR_H
#include <va.h>
// Declarations of the free functions DEFINED in ADVMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class mapCell;
struct tag_message;

typedef enum AdventureScreenConstant {
    ADVMGR_SCREEN_WIDTH = 640,
    ADVMGR_SCREEN_HEIGHT = 480,
    ADVMGR_SCROLL_BORDER = 16,
    ADVMGR_VIEW_EDGE_MARGIN = 8,
    ADVMGR_RADAR_LEFT = 480,
    ADVMGR_RADAR_RIGHT = 624,
    ADVMGR_RADAR_TOP = 16,
    ADVMGR_RADAR_BOTTOM = 160,
    ADVMGR_PUZZLE_TILE_SIZE = 32,
    ADVMGR_PUZZLE_X_TRIM = 12
} AdventureScreenConstant;

typedef enum AdventureButtonConstant {
    ADVMGR_BUTTON_MESSAGE = 0x200,
    ADVMGR_BUTTON_ENABLE = 5,
    ADVMGR_BUTTON_DISABLE = 6,
    ADVMGR_BUTTON_FIRST = 1,
    ADVMGR_BUTTON_LAST = 6,
    ADVMGR_BUTTON_TARGET = 2,
    ADVMGR_BUTTON_SLOT_1 = 1,
    ADVMGR_BUTTON_SLOT_2 = 2,
    ADVMGR_BUTTON_SLOT_3 = 3,
    ADVMGR_BUTTON_SLOT_4 = 4,
    ADVMGR_BUTTON_SLOT_5 = 5,
    ADVMGR_BUTTON_SLOT_6 = 6,
    ADVMGR_BUTTON_BROADCAST_ARG = 1,
    ADVMGR_BUTTON_BROADCAST_FLAGS = 0x4008
} AdventureButtonConstant;

typedef enum AdventureTriggerConstant {
    ADVMGR_TRIGGER_TYPE_MASK = 0x7f,
    ADVMGR_SPECIAL_TRIGGER = 0x7a,
    ADVMGR_SPECIAL_TRIGGER_MASK = 0x3f,
    ADVMGR_TRIGGER_EVENT_5 = 5,
    ADVMGR_TRIGGER_EVENT_6 = 6
} AdventureTriggerConstant;

typedef enum AdventureFrameConstant {
    ADVMGR_MOBILITY_SCALE = 22,
    ADVMGR_MOBILITY_DIVISOR = 1500,
    ADVMGR_MANA_DIVISOR = 5,
    ADVMGR_MANA_MIN_VISIBLE = 3,
    ADVMGR_MOBILITY_TOP_THRESHOLD = 30,
    ADVMGR_MOBILITY_HIGH_THRESHOLD = 26,
    ADVMGR_MOBILITY_MID_THRESHOLD = 23,
    ADVMGR_MANA_TOP_THRESHOLD = 33,
    ADVMGR_MANA_HIGH_THRESHOLD = 28,
    ADVMGR_MANA_MID_THRESHOLD = 23,
    ADVMGR_FRAME_TOP = 25,
    ADVMGR_FRAME_HIGH = 24,
    ADVMGR_FRAME_MID = 23
} AdventureFrameConstant;

typedef enum AdventureStateConstant {
    ADVMGR_INVALID_CELL = -1,
    ADVMGR_INVALID_HERO = -1,
    ADVMGR_LOOPING_SOUND_LIMIT = 4,
    ADVMGR_OBJECT_ICON_COUNT = 64,
    ADVMGR_HERO_ICON_COUNT = 8,
    ADVMGR_PLAYER_COLOR_COUNT = 6,
    ADVMGR_LOOPING_SAMPLE_COUNT = 28,
    ADVMGR_CURSOR_SAMPLE_COUNT = 9,
    ADVMGR_SOUND_CELL_COUNT = 4,
    ADVMGR_CURSOR_SAMPLE_VOLUME = 64,
    ADVMGR_CURSOR_SAMPLE_CHANNEL = 2,
    ADVMGR_QUICK_VIEW_NONE = -99,
    ADVMGR_DEFAULT_POINTER_FRAME = -999,
    ADVMGR_UNUSED_OBJECT_ICON_1 = 21,
    ADVMGR_UNUSED_OBJECT_ICON_2 = 38
} AdventureStateConstant;

typedef enum AdventureOpenConstant {
    ADVMGR_SCROLL_Y = 195,
    ADVMGR_SCROLL_LEFT_X = 540,
    ADVMGR_SCROLL_RIGHT_X = 612,
    ADVMGR_SCROLL_WIDTH = 8,
    ADVMGR_SCROLL_HEIGHT = 17,
    ADVMGR_SCROLL_LEFT_FRAME = 26,
    ADVMGR_SCROLL_RIGHT_FRAME = 27,
    ADVMGR_SCROLL_HOTKEY = 16,
    ADVMGR_TIMER_DELAY = 120,
    ADVMGR_MANAGER_MESSAGE = 0x400
} AdventureOpenConstant;

typedef enum AdventureInterfaceConstant {
    ADVMGR_INTERFACE_AUTO = 0,
    ADVMGR_INTERFACE_GOOD = 1,
    ADVMGR_INTERFACE_EVIL = 2,
    ADVMGR_INTERFACE_TRANSLATION_COUNT = 37,
    ADVMGR_INTERFACE_MESSAGE = 0x200,
    ADVMGR_INTERFACE_REPLACE_RESOURCE = 0x3c
} AdventureInterfaceConstant;

typedef enum AdventureCommand {
    ADVMGR_COMMAND_NONE = -1,
    ADVMGR_COMMAND_MOVE_TO = 1,
    ADVMGR_COMMAND_HERO_VIEW = 2,
    ADVMGR_COMMAND_TOWN_VIEW = 3,
    ADVMGR_COMMAND_SELECT_HERO = 4,
    ADVMGR_COMMAND_SELECT_TOWN = 5,
    ADVMGR_COMMAND_OCCUPIED_TOWN_VIEW = 6,
    ADVMGR_COMMAND_CONTINUE_ROUTE = 7
} AdventureCommand;

typedef enum AdventureInputEvent {
    ADVMGR_INPUT_MOUSE_DOWN = 1,
    ADVMGR_INPUT_MOUSE_UP = 8,
    ADVMGR_INPUT_KEY_DOWN = 0x20,
    ADVMGR_INPUT_COMMAND = 0x200
} AdventureInputEvent;

typedef enum AdventureSearchConstant {
    ADVMGR_ARTIFACT_CAPACITY = 14,
    ADVMGR_CLEAR_GROUND_TILESET = 47,
    ADVMGR_DIG_HOLE_TILESET = 56,
    ADVMGR_DIG_HOLE_FRAME = 140,
    ADVMGR_ULTIMATE_ARTIFACT_NONE = -1,
    ADVMGR_SPHERE_OF_NEGATION = 98,
    ADVMGR_ULTIMATE_ARTIFACT_MUSIC = 41
} AdventureSearchConstant;

typedef enum AdventurePanelCommand {
    ADVMGR_PANEL_NEXT_HERO = 1,
    ADVMGR_PANEL_CONTINUE_ROUTE = 2,
    ADVMGR_PANEL_OVERVIEW = 3,
    ADVMGR_PANEL_END_TURN = 4,
    ADVMGR_PANEL_ADVENTURE_OPTIONS = 5,
    ADVMGR_PANEL_CONTROL_OPTIONS = 6,
    ADVMGR_PANEL_SYSTEM_OPTIONS = 7,
    ADVMGR_PANEL_CAST_SPELL = 8,
    ADVMGR_PANEL_HERO_PAGE_PREVIOUS = 20,
    ADVMGR_PANEL_HERO_PAGE_NEXT = 21,
    ADVMGR_PANEL_TOWN_PAGE_PREVIOUS = 23,
    ADVMGR_PANEL_TOWN_PAGE_NEXT = 24,
    ADVMGR_OVERVIEW_HERO = 1,
    ADVMGR_OVERVIEW_TOWN = 2,
    ADVMGR_DIALOG_OK = 0x7806,
    ADVMGR_BOTTOM_VIEW_FIRST_MESSAGE = 2000,
    ADVMGR_BOTTOM_VIEW_LAST_MESSAGE = 2200,
    ADVMGR_BOTTOM_VIEW_DURATION = 3000,
    ADVMGR_LOCATORS_PER_PAGE = 5,
    ADVMGR_LOCATOR_PAGE_STEP = 4
} AdventurePanelCommand;

int SaveGame(void);
int DimensionDoorHandler(struct tag_message &);
int TownPortalHandler(struct tag_message &);
void ComputeAdvNetControl(void);
int MapExtraPosAndAdjacentsSet(int, int, unsigned char);
int APanelHandler(struct tag_message &);
int CPanelHandler(struct tag_message &);
void UpdateSystemOptions(int);
int SystemOptionsHandler(struct tag_message &);
int GetMobilityFrame(int);
int GetManaFrame(int);
unsigned char StopOnTrigger(class mapCell *);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern int iCurHourGlassPhase;

// ---- globals (declarations, RVA order) ----
extern float fFirstWeekTownFV;
extern int iVepCacheHits;
extern int iTotalVepHits;
extern int giShowComputerRoute;
extern long glLastStartTick;
extern long glCurTicks;
extern long glTotalTicks;
extern float gfAttackHumanBonus;
extern float gfAttackComputerBonus;
extern int bSVSearchArrayInUse;
extern int bEvaluatingTravelGates;
extern int gbReduceByBerserk;
extern float fBerserkFactor;
extern int giMaxHeroesForThisPlayer;
extern float fReduceFactor;
extern unsigned char giCurPlayerBit;
extern int giBestShipyardDist;
extern short gaiHeroLiveChance[54];
extern int giHumanTownConquered;
extern int *costTemp;
extern int gbPossibleShipyardFound;
extern int iCurPlaceToVisit;
extern int giBestShipyardId;
extern int gbActualBoatFound;
extern unsigned char giCurWatchPlayerBit;
extern float gfHeroInteractionBonus[54];
extern int gbBerserk;
extern int giCurAIHeroMorale;
extern int iPlacesVisited[30][2];
extern int gbTroopReload;
extern int giCurAIHeroLuck;
extern int gbActualShipyardFound;

#endif // HOMM2_ADVMGR_H
