#ifndef HOMM2_SOURCE_PHILAI_H
#define HOMM2_SOURCE_PHILAI_H

#include <Ints.h>
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
        i32& attackerLoss,
        i32& defenderLoss,
        i32& attackerRemaining,
        i32& defenderRemaining,
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
    LikelihoodOfEnemyAttacking(class town*, class hero*, float& chanceA, float& chanceB, i32& nAttack, i32& nValue, i32& nWeeks, float& fOut);
    i32 MeanRVOfUnexploredTerritory(i32);
    void GetGameAttentionValue(i32 player);
    void GetTurnAttentionValue(i32 player);
    i32 RVConversion(i32* const resources);
    float TurnsToBuy(i32* const resources);
    i32 RVOfPosition(i32 x, i32 y, i32 hasEvent, i32 eventX, i32 eventY, i32 hasStrategicEvent, i32 strategicX, i32 strategicY, i32 eventMode, i32 extraDistance);
    i32 StrategicValueOfPosition(i32 targetX, i32 targetY, i32 immediate, i32 checkEnemies, i32* liveChance, i32 extraDistance);
    i32 ValueOfTown(class town* townPointer);
    void TurnCostResource(i32 player);
    float TurnValueOfObelisk(i32 player);
    float FutureDeflator(i32* const resources);
    i32 FightValueOfStack(
        class armyGroup* group,
        class hero* heroPointer,
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
    void HeroInteractionAtTown(class hero* heroPointer, class town* townPointer, i32 doInteraction, i32* value);
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
        i32& outFlag,
        i32& outValue
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
    i32 DamageGroup(class armyGroup* armyGroupPointer, class hero* loser, class hero*, float damage);
    void IncrementHourGlass(void);
    void TownEvent(class mapCell* cell, class hero* heroPointer, i32 x, i32 y);
    i32 ComputeUpgradeValue(CreatureType baseCreatureType, CreatureType upgradedCreatureType);
    i32 ComputeValueOfSS(
        class hero* heroPointer,
        HeroSecondarySkill skill,
        HeroSkillLevel level
    );
    i32 ComputeValueOfFreeSS(class hero* heroPointer, HeroSecondarySkill skill);
    i32 ManaRefreshValue(class hero* heroPointer, i32 level);
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
