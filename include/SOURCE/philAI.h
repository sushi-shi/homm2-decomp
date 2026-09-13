#ifndef HOMM2_SOURCE_PHILAI_H
#define HOMM2_SOURCE_PHILAI_H

#include <va.h>
#include <SOURCE/KB_TYPES.h>
#include <SOURCE/hero.h>

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
    i32 GoodAdjacent(H2_ENUM_PARAM(MapDirection, i32)* direction);
    void CheckReload(void);
    void CheckBerserk(void);
    void DimensionDoorTo(i32 x, i32 y);
    i32 DoAnywhereDDoorTownGate(i32 targetValue);
    i32 DoDimensionDoor(class hero* pHero);
    void SetupRelativeHeroStrengths(void);
    void DoAI(i32 player);
    void GetGameAIVars(void);
    void GetTurnAIVars(i32 player);
    void GetBestBHC(i32 player, struct BHC& best);
    class hero* DetermineHeroToMove(i32 player);
    i32 DetermineTargetPosition(
        i32& targetX,
        i32& targetY,
        i32 mobility,
        H2_ENUM_PARAM(MapDirection, i32)& direction
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
        i32& attackerLoss,
        i32& defenderLoss,
        i32& attackerRemaining,
        i32& defenderRemaining,
        i32& outcomeValue
    );
    float GetOddsOfWinning(i32);
    void ValueOfBuyingBuilding(class town* townPtr, BuildingSlotType building, i32& resourceValue, float& benefitCost);
    void GetBestBuilding(class town* t, struct BHC& bhc, float& fOut);
    void ValueOfBuyingCreature(class town* townPtr, CreatureType creature, i32& resourceValue, i32 purchaseCount, float& benefitCost);
    void GetBestCreature(class town* townPtr, struct BHC& best, float& bestValue);
    i32 CreaturesToBuy(class town* t, i32 level);
    i32 CreaturesToBuy(H2_ENUM_PARAM(CreatureType, i32) creatureType, i32 availableCount);
    i32 MaxBuyableCreatures(CreatureType creatureType);
    void ValueOfBuyingHero(class town* townPtr, class hero* heroPtr, i32& resourceValue, float& benefitCost);
    void GetBestHero(class town* townPtr, struct BHC& best, float& bestValue);
    void
    LikelihoodOfEnemyAttacking(class town*, class hero*, float& chanceA, float& chanceB, i32& nAttack, i32& nValue, i32& nWeeks, float& fOut);
    i32 MeanRVOfUnexploredTerritory(i32);
    void GetGameAttentionValue(i32 player);
    void GetTurnAttentionValue(i32 player);
    i32 RVConversion(i32* const p);
    float TurnsToBuy(i32* const p);
    i32 RVOfPosition(i32 x, i32 y, i32 hasEvent, i32 eventX, i32 eventY, i32 hasStrategicEvent, i32 strategicX, i32 strategicY, i32 eventMode, i32 extraDistance);
    i32 StrategicValueOfPosition(i32 targetX, i32 targetY, i32 immediate, i32 checkEnemies, i32* liveChance, i32 extraDistance);
    i32 ValueOfTown(class town* t);
    void TurnCostResource(i32 player);
    float TurnValueOfObelisk(i32 player);
    float FutureDeflator(i32* const p);
    i32 FightValueOfStack(
        class armyGroup* group,
        class hero* heroPtr,
        i32 useHero,
        i32 useTown = 0,
        i32 townId = 0,
        i32 useEnemyMods = 0
    );
    void EvaluateOneTimeCreaturePurchase(CreatureType creature, i32 availableCount, i32 useAvailableCount, i32& purchaseCount, i32& purchaseValue, i32& replacementSlot);
    i32 QuickCombat(
        class armyGroup* attacker,
        class hero* attackerHero,
        class armyGroup* defender,
        class hero* defenderHero,
        i32 townBattle,
        i32 townId,
        float& attackerDamage,
        float& defenderDamage
    );
    void HeroInteractionAtHero(class hero* firstHero, class hero* secondHero, i32 evaluateOnly, i32* value);
    void HeroInteractionAtTown(class hero* heroPtr, class town* townPtr, i32 doInteraction, i32* value);
    void RedistributeTroops(class armyGroup* sourceArmy, class armyGroup* destinationArmy, i32 preserveOne, i32 preferFast, i32 sourceStrength, i32 destinationStrength, i32 transferBudget);
    i32 ChooseGoldOrExperience(i32, i32);
    void ChooseEvaluateBattle(
        class armyGroup* ag1,
        class hero* h1,
        class armyGroup* ag2,
        class hero* h2,
        i32 a,
        i32 b,
        i32 c,
        i32& outFlag,
        i32& outValue
    );
    i32 ChooseToFightForArtifact(ArtifactType artifact, H2_ENUM_PARAM(CreatureType, i32) monster, i32 quantity);
    i32 NetValueOfArtifact(i32 a1, i32 a2, i32 a3, i32 a4);
    i32 ChooseToPayRansomOnHero(i32);
    void BuildBuilding(class town* t, H2_ENUM_PARAM(BuildingSlotType, i32) building);
    void BuildHero(class town* townPtr, i32 availableHeroIndex);
    void BuildCreature(class town* townPtr, i32 dwelling, i32 purchaseCount);
    i32 CanBuyBHC(struct BHC& bhc);
    i32 CombatMonsterEvent(
        class hero* h, H2_ENUM_PARAM(CreatureType, i32) monType, i32* pCount, class mapCell* cell
    );
    i32 FightEvent(class hero* h, class mapCell* cell, i32 evaluateOnly);
    i32 DamageGroup(class armyGroup* ag, class hero* loser, class hero*, float dmg);
    void IncrementHourGlass(void);
    void TownEvent(class mapCell* cell, class hero* h, i32 x, i32 y);
    i32 ComputeUpgradeValue(CreatureType baseCreatureType, CreatureType upgradedCreatureType);
    i32 ComputeValueOfSS(
        class hero* h,
        H2_ENUM_PARAM(HeroSecondarySkill, i32) skill,
        H2_ENUM_PARAM(HeroSkillLevel, i32) level
    );
    i32 ComputeValueOfFreeSS(class hero* h, H2_ENUM_PARAM(HeroSecondarySkill, i32) skill);
    i32 ManaRefreshValue(class hero* h, i32 level);
    i32 ValueOfEventAtPosition(i32 x, i32 y, i32 immediate, i32* liveChance);
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
#endif
