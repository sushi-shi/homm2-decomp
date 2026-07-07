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
DATA(0x004f57d0) extern int iCurHourGlassPhase;

// ---- globals (declarations, RVA order) ----
DATA(0x004f20e0) extern float fFirstWeekTownFV;
DATA(0x004f20e4) extern int iVepCacheHits;
DATA(0x004f20e8) extern int iTotalVepHits;
DATA(0x004f20ec) extern int giShowComputerRoute;
DATA(0x004f2104) extern long glLastStartTick;
DATA(0x004f2108) extern long glCurTicks;
DATA(0x004f210c) extern long glTotalTicks;
DATA(0x004f2114) extern float gfAttackHumanBonus;
DATA(0x004f2118) extern float gfAttackComputerBonus;
DATA(0x004f22bc) extern int bSVSearchArrayInUse;
DATA(0x004f2340) extern int bEvaluatingTravelGates;
DATA(0x00525620) extern int gbReduceByBerserk;
DATA(0x00525624) extern float fBerserkFactor;
DATA(0x00525634) extern int giMaxHeroesForThisPlayer;
DATA(0x00525640) extern float fReduceFactor;
DATA(0x00525644) extern unsigned char giCurPlayerBit;
DATA(0x00525648) extern int giBestShipyardDist;
DATA(0x00525650) extern short *gaiHeroLiveChance;
DATA(0x005256bc) extern int giHumanTownConquered;
DATA(0x005256c8) extern int *costTemp;
DATA(0x005256ec) extern int gbPossibleShipyardFound;
DATA(0x00527c24) extern int iCurPlaceToVisit;
DATA(0x00527c28) extern int giBestShipyardId;
DATA(0x00527c2c) extern int gbActualBoatFound;
DATA(0x00527c30) extern unsigned char giCurWatchPlayerBit;
DATA(0x00527c38) extern float *gfHeroInteractionBonus;
DATA(0x00527d10) extern int gbBerserk;
DATA(0x00527d14) extern int giCurAIHeroMorale;
DATA(0x00527d20) extern int (*iPlacesVisited)[2];
DATA(0x00527e14) extern int gbTroopReload;
DATA(0x00527e18) extern int giCurAIHeroLuck;
DATA(0x00527e1c) extern int gbActualShipyardFound;

#endif // HOMM2_ADVMGR_H
