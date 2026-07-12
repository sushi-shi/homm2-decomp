#ifndef HOMM2_ADVMGR_H
#define HOMM2_ADVMGR_H
#include <va.h>
// Declarations of the free functions DEFINED in ADVMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class mapCell;
struct tag_message;

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
