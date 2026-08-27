#ifndef HOMM2_PHILAI_TU_H
#define HOMM2_PHILAI_TU_H

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>

class army;
class hero;
class playerData;
class searchArray;
class town;

enum {
    PURCHASE_NONE     = -1,
    PURCHASE_BUILDING = 0,
    PURCHASE_HERO     = 1,
    PURCHASE_CREATURE = 2
};
typedef i32 AIPurchaseType;
struct BHC {
    town* pTown;
    AIPurchaseType type;
    union {
        i32 what;
        i32 building;
    };
    i32 num;
};

typedef enum AIPurchaseConstant {
    AI_PURCHASE_RESOURCE_COUNT     = (RES_COUNT),
    AI_PLAYER_COUNT                = 6,
    AI_RANDOM_MINE_TYPE_COUNT      = 8,
    AI_PURCHASE_DEBUG_LEVEL        = 3,
    AI_PURCHASE_DEBUG_DELAY        = 1500,
    AI_HERO_BUILD_COORDINATE_UNSET = -99,
    AI_HERO_AVAILABLE_FLAG         = 0x40,
    AI_PURCHASE_VALUE_DEBUG_LEVEL  = 5
} AIPurchaseConstant;

typedef enum AIBattleConstant {
    AI_BATTLE_NO_PLAYER              = -1,
    AI_BATTLE_ARTIFACT_SLOT_COUNT    = 14,
    AI_BATTLE_BASE_ARTIFACT_LIMIT    = 37,
    AI_BATTLE_ATTACKER_ARTIFACT_BASE = 1400,
    AI_BATTLE_DEFENDER_ARTIFACT_BASE = 1250,
    AI_BATTLE_SPECIAL_ARTIFACT_VALUE = 50000,
    AI_BATTLE_DEBUG_LEVEL            = 9
} AIBattleConstant;

typedef enum AIGenericSiteConstant {
    AI_GENERIC_SITE_GOLD_THRESHOLD        = 1500,
    AI_GENERIC_SITE_CURSED_ARTIFACT_VALUE = 500,
    AI_GENERIC_SITE_MAX_LUCK              = 3,
    AI_GENERIC_SITE_ARMY_SLOTS            = 5,
    AI_GENERIC_SITE_WEEK_END              = 8
} AIGenericSiteConstant;

#define AI_GENERIC_SITE_SIRENS_ARMY_REMAINDER 0.7

void ResetHeroRVs(i32, i32, i32);
void CheckDoMain(i32 a1, i32 doMain);
void ShowStatus(void);
void ValidateHero(hero* pHero);
void InitAIMapVars(void);
void CloseAIMapVars(void);
i32 OnMySide(i32);

extern b32 bHeroBuiltThisTurn;
extern float gafAITurnCostResource[AI_PURCHASE_RESOURCE_COUNT];
extern i8* gaiEnemyHeroReachable;
extern i16* gaiHeroEventStratRVOfPos;
extern i16* gaiHeroStrategicRVOfPos;
extern i16* gaiLiveChanceOfPos;
extern i8* gaiTurnValueOfMine;
extern b32 gbReduceByReload;
extern i8 giBuildBoat[AI_PLAYER_COUNT];
extern i8 giBuildBoatStuffTurn[AI_PLAYER_COUNT];
extern i8 giBuildShipyard[AI_PLAYER_COUNT];
extern i32 giCurPlayer;
extern u8 giCurPlayerBit;
extern i32 giCurTurn;
extern hero* gpCurAIHero;
extern playerData* gpCurPlayer;
extern u8 giCurWatchPlayerBit;
extern i32 iAlphaMale;
extern i32 iDummy;
extern i32 iLastFrameRateTimer;
extern searchArray SVSearchArray;

extern b32 gbGameOver;
extern i8 giMonType[];
extern i32 iViewArmyNumTroops;
extern i8 gbNGHeroType[];
extern i16 giUABaseX;
extern i16 giUABaseY;
extern b32 giEndSequence;
extern b32 gbDismissArmy;
extern i8 gbNGHuman[];
extern i32 iViewArmyFrame;
extern b32 gbAllowUpgrade;
extern i32 iViewArmyType;
extern class hero* viewSpellsHero;
extern b32 gbUpgradeArmy;
extern i16 RandMineQty[AI_RANDOM_MINE_TYPE_COUNT];
extern i8 gbNGDifficulty[];
extern i32 iViewArmyUpgradeToType;
extern i32 viewArmyBaseX;
extern i32 viewArmyBaseY;
extern i8 gbNGColor[];
extern i16 giUARadius;
extern i8 gbNGPlayerPos[];
extern i32 viewArmyFacingWIPXMod;

#endif
