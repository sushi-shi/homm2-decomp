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
    ADVMGR_LOOPING_SOUND_LIMIT = 4
} AdventureStateConstant;

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
extern short *gaiHeroLiveChance;
extern int giHumanTownConquered;
extern int *costTemp;
extern int gbPossibleShipyardFound;
extern int iCurPlaceToVisit;
extern int giBestShipyardId;
extern int gbActualBoatFound;
extern unsigned char giCurWatchPlayerBit;
extern float *gfHeroInteractionBonus;
extern int gbBerserk;
extern int giCurAIHeroMorale;
extern int iPlacesVisited[30][2];
extern int gbTroopReload;
extern int giCurAIHeroLuck;
extern int gbActualShipyardFound;

#endif // HOMM2_ADVMGR_H
