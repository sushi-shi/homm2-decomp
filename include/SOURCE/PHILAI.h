#ifndef HOMM2_PHILAI_TU_H
#define HOMM2_PHILAI_TU_H
#include <va.h>
// AI view/record structs + free functions DEFINED in PHILAI.cpp — single home.
// (tag_tilePoint / monsterRV are shared, in _types.h.)
// forward declarations (was <_all.h>):
class army;
class hero;
class playerData;
class searchArray;
class town;
#include <_types.h>

struct BHC {
    town *pTown;     // 0x00
    int type;        // 0x04  (0=building, 1=hero, 2=creature)
    int what;        // 0x08
    int num;         // 0x0c
};

// ---- AI member views: the recovered classes carry no field layout, so model the
// exact offsets the /Od code touches as packed views and cast to them. ----
#pragma pack(push, 1)
struct pdView {                  // playerData
    char _0;
    signed char numHeroes;       // 0x01
    char _2[2];
    signed char heroIds[8];      // 0x04
    char _c[0x44 - 0x0c];
    signed char numCastles;      // 0x44
    char _45[2];
    signed char castleIds[8];    // 0x47
    char _4f[0x8f - 0x4f];
    int resources[7];            // 0x8f (gold = [6] @ 0xa7)
    char _ab;
    signed char barrierTents;    // 0xac
    char _ad[0x10f - 0xad];
    float upgradeFactor;         // 0x10f
    signed char getHeroId(int i) { return heroIds[i]; }
    signed char getCastleId(int i) { return castleIds[i]; }
};
struct heroView {                // hero
    signed short curMana;        // 0x00
    unsigned char heroId;        // 0x02
    char _3[0x65 - 0x03];
    signed char army[0x74 - 0x65]; // 0x65 (embedded armyGroup creature types)
    signed char skills[0x90 - 0x74]; // 0x74
    int level;                   // 0x90
};
struct townView {                // town
    signed char id;              // 0x00
    char _1[2];
    signed char race;            // 0x03
    unsigned char castleX;       // 0x04
    unsigned char castleY;       // 0x05
    char _6[0x17 - 0x06];
    signed char visitingHero;    // 0x17
    int buildings;               // 0x18
    signed char buildState;      // 0x1c
    char _1d;
    signed short garrison[5];    // 0x1e
    signed char getVisitingHero() { return visitingHero; }
};
struct taView {                  // per-player AI "turn attention" record (gpGame+0x49e + player*283)
    float f0, f4, f8, fc, f10, f14; // 0x00..0x14
    char _18[0x34 - 0x18];
    int field34[7];              // 0x34..0x4f
    int field50;                 // 0x50
};
struct mapCellView {             // mapCell
    char _0[4];
    unsigned short _bits : 3;
    unsigned short tentColor : 13; // bits 3..15 of word @ 0x04
};
struct gameTV {                  // game fields used by ValueOfTown (modelled so /Od evals L->R)
    char _0[0x2c6];
    unsigned char f2c6;          // 0x2c6
    char _2c7[2];
    unsigned short f2c9;         // 0x2c9
    unsigned char f2cb;          // 0x2cb
    unsigned short f2cc;         // 0x2cc
    char _2ce[0x2d5 - 0x2ce];
    unsigned short f2d5;         // 0x2d5
    unsigned short f2d7;         // 0x2d7
};
#pragma pack(pop)

// gpGame view for the per-player AI hero-slot 2D array at 0x4a0 (row stride 283),
// modelled as a struct member so /Od keeps 0x4a0 as the load displacement and folds
// the row multiply with the column index.
struct gameView {
    char _0[0x4a0];
    signed char heroSlots[40][283];  // 0x4a0
};

// Declarations of the free functions DEFINED in PHILAI.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
void ResetHeroRVs(int, int, int);
void CheckDoMain(int a1, int doMain);
void ShowStatus(void);
void ValidateHero(hero *pHero);
void InitAIMapVars(void);
void CloseAIMapVars(void);
int OnMySide(int);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern int bHeroBuiltThisTurn;
extern float gafAITurnCostResource[7];
extern signed char *gaiEnemyHeroReachable;
extern short *gaiHeroEventStratRVOfPos;
extern short *gaiHeroStrategicRVOfPos;
extern short *gaiLiveChanceOfPos;
extern signed char *gaiTurnValueOfMine;
extern int gbReduceByReload;
extern signed char giBuildBoat[6];
extern signed char giBuildBoatStuffTurn[6];
extern signed char giBuildShipyard[6];
extern int giCurPlayer;
extern unsigned char giCurPlayerBit;
extern int giCurTurn;
extern hero *gpCurAIHero;
extern playerData *gpCurPlayer;
extern unsigned char giCurWatchPlayerBit;
extern int iAlphaMale;
extern int iDummy;
extern int iLastFrameRateTimer;
extern searchArray SVSearchArray;

// ---- globals (declarations, RVA order) ----
extern int gbGameOver;
extern signed char *giMonType;
extern int iViewArmyNumTroops;
extern signed char *gbNGHeroType;
extern short giUABaseX;
extern short giUABaseY;
extern int giEndSequence;
extern int gbDismissArmy;
extern signed char *gbNGHuman;
extern int iViewArmyFrame;
extern int gbAllowUpgrade;
extern int iViewArmyType;
extern class hero *viewSpellsHero;
extern int gbUpgradeArmy;
extern short *RandMineQty;
extern char *gcCurMapName;
extern signed char *gbNGDifficulty;
extern int iViewArmyUpgradeToType;
extern int viewArmyBaseX;
extern int viewArmyBaseY;
extern signed char *gbNGColor;
extern short giUARadius;
extern signed char *gbNGPlayerPos;
extern int viewArmyFacingWIPXMod;

#endif // HOMM2_PHILAI_TU_H
