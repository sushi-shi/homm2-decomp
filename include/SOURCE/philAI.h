#ifndef HOMM2_SOURCE_PHILAI_H
#define HOMM2_SOURCE_PHILAI_H

#include <Domains.h>
#include <SOURCE/kbTypes.h>
#include <SOURCE/hero.h>
#include <SOURCE/gameTypes.h>

class army;
class hero;
class playerData;
class searchArray;
class town;

enum class AIPurchaseType : i32 {
    PURCHASE_NONE     = -1,
    PURCHASE_BUILDING = 0,
    PURCHASE_HERO     = 1,
    PURCHASE_CREATURE = 2
};
using enum AIPurchaseType;

struct BHC {
    town* pTown;
    AIPurchaseType type;
    union {
        i32 what;
        H2EnumStorage<BuildingSlotType, i32> building;
    };
    i32 num;
};

typedef enum AIPurchaseConstant {
    AI_RANDOM_MINE_TYPE_COUNT      = 8,
    AI_PURCHASE_DEBUG_DELAY        = 1500,
} AIPurchaseConstant;

typedef enum AIBattleConstant {
    AI_BATTLE_NO_PLAYER              = -1,
    AI_BATTLE_BASE_ARTIFACT_LIMIT    = 37,
    AI_BATTLE_ATTACKER_ARTIFACT_BASE = 1400,
    AI_BATTLE_DEFENDER_ARTIFACT_BASE = 1250,
    AI_BATTLE_SPECIAL_ARTIFACT_VALUE = 50000,
} AIBattleConstant;

typedef enum AIGenericSiteConstant {
    AI_GENERIC_SITE_GOLD_THRESHOLD        = 1500,
    AI_GENERIC_SITE_CURSED_ARTIFACT_VALUE = 500,
    AI_GENERIC_SITE_MAX_LUCK              = 3,
    AI_GENERIC_SITE_WEEK_END              = 8
} AIGenericSiteConstant;

#define AI_GENERIC_SITE_SIRENS_ARMY_REMAINDER 0.7

class armyGroup;
class hero;
class mapCell;
class town;
struct BHC;

class philAI {
public:
    philAI(void);
    void DoAllHeroInteractions(void);
    void CheckForCreatureUpgrades(void);
    void CheckBuyStuff(void);
    i32 GoodAdjacent(MapDirection* direction);
    void CheckReload(void);
    void CheckBerserk(void);
    void DimensionDoorTo(i32 x, i32 y);
    i32 DoAnywhereDDoorTownGate(i32 targetValue);
    i32 DoDimensionDoor(class hero* pHero);
    void SetupRelativeHeroStrengths(void);
    void DoAI(i32 player);
    void GetGameAIVars(void);
    void GetTurnAIVars(i32 player);
    void GetBestBHC(i32, struct BHC& best);
    class hero* DetermineHeroToMove(i32 player);
    i32 DetermineTargetPosition(
        i32& targetX,
        i32& targetY,
        i32 mobility,
        MapDirection& direction
    );
    void ProbableOutcomeOfBattle(
        class armyGroup* attacker,
        class hero* attackerHero,
        class armyGroup* defender,
        class hero* defenderHero,
        class armyGroup* townArmy,
        i32 useTown,
        i32 townId,
        i32 enemyPlayer,
        float& winChance,
        i32& attackerLossValue,
        i32& defenderLossValue,
        i32& expectedAttackerLossValue,
        i32& expectedDefenderLossValue,
        i32& outcomeValue
    );
    float GetOddsOfWinning(i32);
    void ValueOfBuyingBuilding(class town* townPointer, BuildingSlotType building, i32& resourceValue, float& benefitCost);
    void GetBestBuilding(class town* townPointer, struct BHC& purchase, float& benefitCost);
    void ValueOfBuyingCreature(class town* townPointer, CreatureType creature, i32& resourceValue, i32 purchaseCount, float& benefitCost);
    void GetBestCreature(class town* townPointer, struct BHC& best, float& bestValue);
    i32 CreaturesToBuy(class town* townPointer, i32 level);
    i32 CreaturesToBuy(CreatureType creatureType, i32 availableCount);
    i32 MaxBuyableCreatures(CreatureType creatureType);
    void ValueOfBuyingHero(class town* townPointer, class hero* heroPointer, i32& resourceValue, float& benefitCost);
    void GetBestHero(class town* townPointer, struct BHC& best, float& bestValue);
    void
    LikelihoodOfEnemyAttacking(class town*, class hero*, float& attackChance, float& lossRisk, i32& attackStrength, i32& weightedAttack, i32& attackWeeks, float& dangerRating);
    i32 MeanRVOfUnexploredTerritory(i32);
    void GetGameAttentionValue(i32 player);
    void GetTurnAttentionValue(i32 player);
    i32 RVConversion(i32* const resources);
    float TurnsToBuy(i32* const resources);
    i32 RVOfPosition(i32 x, i32 y, i32 hasAdjacentMonster, i32 adjacentMonsterX, i32 adjacentMonsterY, i32 beyondTurnMobility, i32 turnEndX, i32 turnEndY, i32 eventMode, i32 extraDistance);
    i32 StrategicValueOfPosition(i32 targetX, i32 targetY, i32 immediate, i32 checkEnemies, i32* liveChance, i32 extraDistance);
    i32 ValueOfTown(class town* townPointer);
    void TurnCostResource(i32 player);
    float TurnValueOfObelisk(i32 player);
    float FutureDeflator(i32* const resources);
    i32 FightValueOfStack(
        class armyGroup* group,
        class hero* heroPointer,
        i32 useAdjustedFightValue,
        i32 useTown = 0,
        i32 townId = 0,
        i32 applySiegeAttackerModifiers = 0
    );
    void EvaluateOneTimeCreaturePurchase(CreatureType creature, i32 availableCount, i32 isFree, i32& purchaseCount, i32& purchaseValue, i32& replacementSlot);
    i32 QuickCombat(
        class armyGroup* attacker,
        class hero* attackerHero,
        class armyGroup* defender,
        class hero* defenderHero,
        i32 townBattle,
        i32 townId,
        float& attackerCasualtyFraction,
        float& defenderCasualtyFraction
    );
    void HeroInteractionAtHero(class hero* firstHero, class hero* secondHero, i32 evaluateOnly, i32* value);
    void HeroInteractionAtTown(class hero* heroPointer, class town* townPointer, i32 evaluateOnly, i32* value);
    void RedistributeTroops(class armyGroup* sourceArmy, class armyGroup* destinationArmy, i32 preserveOne, i32 preferFast, i32 sourceStrength, i32, i32 transferBudget);
    i32 ChooseGoldOrExperience(i32, i32);
    void ChooseEvaluateBattle(
        class armyGroup* attackerArmy,
        class hero* attackerHero,
        class armyGroup* defenderArmy,
        class hero* defenderHero,
        i32 isCastle,
        i32 castleId,
        i32 rewardValue,
        i32& worthFighting,
        i32& battleValue
    );
    i32 ChooseToFightForArtifact(ArtifactType artifact, CreatureType monster, i32);
    i32 NetValueOfArtifact(i32 artifact, i32 goldCost, i32 resourceType, i32 resourceCost);
    i32 ChooseToPayRansomOnHero(i32);
    void BuildBuilding(class town* townPointer, BuildingSlotType building);
    void BuildHero(class town* townPointer, i32 availableHeroIndex);
    void BuildCreature(class town* townPointer, i32 dwelling, i32 purchaseCount);
    i32 CanBuyBHC(struct BHC& purchase);
    i32 CombatMonsterEvent(
        class hero* heroPointer, CreatureType monType, i32* pCount, class mapCell*
    );
    i32 FightEvent(class hero* heroPointer, class mapCell* cell, i32 evaluateOnly);
    i32 DamageGroup(class armyGroup* armyGroupPointer, class hero* loser, class hero*, float casualtyFraction);
    void IncrementHourGlass(void);
    void TownEvent(class mapCell* cell, class hero* heroPointer, i32 x, i32 y);
    i32 ComputeUpgradeValue(CreatureType baseCreatureType, CreatureType upgradedCreatureType);
    i32 ComputeValueOfSS(
        class hero* heroPointer,
        HeroSecondarySkill skill,
        HeroSkillLevel level
    );
    i32 ComputeValueOfFreeSS(class hero* heroPointer, HeroSecondarySkill skill);
    i32 ManaRefreshValue(class hero* heroPointer, i32 manaMultiplier);
    i32 ValueOfEventAtPosition(i32 x, i32 y, i32 eventMode, i32* liveChance);
    i32 EvaluateGenericSite(class mapCell* cell);
    i32 EvaluateBarrier(class mapCell* cell);
    i32 EvaluatePassword(class mapCell* cell);
    i32 EvaluateRecruitSite(class mapCell* cell);
    i32 EvaluateJail(class mapCell*);
    i32 EvaluateArtifactEvent(ArtifactType artifact, i32 eventData);
    i32 EvaluateMineEvent(i32 mineIndex, i32 x, i32 y, i32* liveChance);
    i32 EvaluateMonsterEvent(CreatureType monsterType, i32 eventData, i32* liveChance);
    i32 EvaluateHeroEvent(i32 heroId, i32 x, i32 y, i32 mode, i32* liveChance);
    i32 EvaluateTownEvent(i32 townId, i32 x, i32 y, i32 mode, i32* liveChance);
};

void ResetHeroRVs(i32 nearbyOnly, i32 x, i32 y);
void CheckDoMain(i32 unused, i32 doMain);
void ShowStatus(void);
void ValidateHero(hero* pHero);
void InitAIMapVars(void);
void CloseAIMapVars(void);
i32 OnMySide(i32 player);

extern b32 bHeroBuiltThisTurn;
extern float gafAITurnCostResource[H2EnumIndex(RES_COUNT)];
extern i8* gaiEnemyHeroReachable;
extern i16* gaiHeroEventStratRVOfPos;
extern i16* gaiHeroStrategicRVOfPos;
extern i16* gaiLiveChanceOfPos;
extern i8* gaiTurnValueOfMine;
extern b32 gbReduceByReload;
extern i8 giBuildBoat[GAME_PLAYER_COUNT];
extern i8 giBuildBoatStuffTurn[GAME_PLAYER_COUNT];
extern i8 giBuildShipyard[GAME_PLAYER_COUNT];
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
extern H2EnumStorage<CreatureType, i32> iViewArmyType;
extern class hero* viewSpellsHero;
extern b32 gbUpgradeArmy;
extern i16 RandMineQty[AI_RANDOM_MINE_TYPE_COUNT];
extern i8 gbNGDifficulty[];
extern H2EnumStorage<CreatureType, i32> iViewArmyUpgradeToType;
extern i32 viewArmyBaseX;
extern i32 viewArmyBaseY;
extern i8 gbNGColor[];
extern i16 giUARadius;
extern i8 gbNGPlayerPos[];
extern i32 viewArmyFacingWIPXMod;

#endif
