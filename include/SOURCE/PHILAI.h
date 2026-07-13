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

typedef enum AIResourceValue {
    AI_RV_UNSET = -32001,
    AI_NEARBY_RADIUS = 10,
    AI_HERO_COUNT = 54
} AIResourceValue;

typedef enum AIObjectType {
    AI_OBJECT_TOWN = 0xa3,
    AI_OBJECT_HERO = 0xaa,
    AI_OBJECT_MONSTER = 0x24,
    AI_OBJECT_ARTIFACT = 0x27,
    AI_OBJECT_BOAT = 0x2a,
    AI_EXPANSION_OBJECT_THRESHOLD = 108
} AIObjectType;

typedef enum AISpellType {
    AI_SPELL_DIMENSION_DOOR = 0x38
} AISpellType;

typedef enum AIPurchaseType {
    AI_PURCHASE_BUILDING = 0,
    AI_PURCHASE_HERO = 1,
    AI_PURCHASE_CREATURE = 2
} AIPurchaseType;

#define AI_MINIMUM_FIGHT_VALUE 100
#define AI_BERSERK_THRESHOLD 30000
#define AI_RELOAD_BASE 2.0f
#define AI_RELOAD_NUMERATOR 3.0f
#define AI_EASY_STRENGTH_FACTOR 1.15
#define AI_BERSERK_FACTOR 0.75
#define AI_HERO_VALUE_SCALE 0.00004
#define AI_HERO_VALUE_BASE 0.4
#define AI_ARTIFACT_POOL_DIVISOR 33.0
#define AI_TOWN_DEFENSE_THRESHOLD 0.7
#define AI_PURCHASE_RANDOM_DIVISOR 100.0
#define AI_CREATURE_BALANCE_BASE 0.66
#define AI_HERO_PURCHASE_BONUS 500.0f
#define AI_HERO_PURCHASE_FACTOR 1.3
#define AI_MINIMUM_PURCHASE_VALUE 0.02
#define AI_PLAYER_HERO_IDS_BASE 0x4a0
#define AI_DIMENSION_DOOR_SPELL_POINTS 30

// ---- AI member views: the recovered classes carry no field layout, so model the
// exact offsets the /Od code touches as packed views and cast to them. ----
#pragma pack(push, 1)
struct pdView {                  // playerData
    char _0;
    signed char numHeroes;       // 0x01
    char _2[2];
    signed char heroIds[8];      // 0x04
    char _c[2];
    unsigned char minimumHeroes; // 0x0e
    int difficulty;              // 0x0f
    char _13[0x40 - 0x13];
    signed char routeLength;       // 0x40
    signed char routeX;            // 0x41
    signed char routeY;            // 0x42
    char _43;
    signed char numCastles;      // 0x44
    char _45[2];
    signed char castleIds[8];    // 0x47
    char _4f[0x8f - 0x4f];
    int resources[7];            // 0x8f (gold = [6] @ 0xa7)
    char _ab;
    signed char barrierTents;    // 0xac
    char _ad[0xc3 - 0xad];
    float baseUpgradeFactor;     // 0xc3
    char _c7[0xe7 - 0xc7];
    int income[7];               // 0xe7
    int obeliskValue;            // 0x103
    int totalObeliskValue;       // 0x107
    int unexploredValue;         // 0x10b
    float upgradeFactor;         // 0x10f
    float artifactValue;         // 0x113
    float artifactPoolShare;     // 0x117
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
    signed char owner;           // 0x01
    signed char threat;           // 0x02
    signed char race;            // 0x03
    unsigned char castleX;       // 0x04
    unsigned char castleY;       // 0x05
    unsigned char dockX;         // 0x06
    unsigned char dockY;         // 0x07
    char _8[0x17 - 0x08];
    signed char visitingHero;    // 0x17
    int buildings;               // 0x18
    signed char buildState;      // 0x1c
    char _1d;
    signed short garrison[5];    // 0x1e
    char _28[0x55 - 0x28];
    unsigned short turnsOwned;   // 0x55
    signed char getVisitingHero() { return visitingHero; }
};
struct armyView {                // armyGroup
    signed char creatureTypes[5]; // 0x00
    signed short quantities[5];   // 0x05
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
