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
    void DimensionDoorTo(i32, i32);
    i32 DoAnywhereDDoorTownGate(i32);
    i32 DoDimensionDoor(class hero* pHero);
    void SetupRelativeHeroStrengths(void);
    void DoAI(i32);
    void GetGameAIVars(void);
    void GetTurnAIVars(i32);
    void GetBestBHC(i32, struct BHC& best);
    class hero* DetermineHeroToMove(i32);
    i32 DetermineTargetPosition(
        i32&,
        i32&,
        i32,
        H2_ENUM_PARAM(MapDirection, i32)&
     direction);
    void ProbableOutcomeOfBattle(
        class armyGroup* attacker,
        class hero* attackerHero,
        class armyGroup* defender,
        class hero* defenderHero,
        class armyGroup* townArmy,
        i32,
        i32,
        i32,
        float&,
        i32&,
        i32&,
        i32&,
        i32&,
        i32&
    );
    float GetOddsOfWinning(i32);
    void ValueOfBuyingBuilding(class town* townPtr, BuildingSlotType building, i32&, float&);
    void GetBestBuilding(class town* t, struct BHC& bhc, float&);
    void ValueOfBuyingCreature(class town* townPtr, CreatureType creature, i32&, i32, float&);
    void GetBestCreature(class town* townPtr, struct BHC& best, float&);
    i32 CreaturesToBuy(class town* t, i32);
    i32 CreaturesToBuy(H2_ENUM_PARAM(CreatureType, i32) creatureType, i32 availableCount);
    i32 MaxBuyableCreatures(CreatureType creatureType);
    void ValueOfBuyingHero(class town* townPtr, class hero* heroPtr, i32&, float&);
    void GetBestHero(class town* townPtr, struct BHC& best, float&);
    void
    LikelihoodOfEnemyAttacking(class town*, class hero*, float&, float&, i32&, i32&, i32&, float&);
    i32 MeanRVOfUnexploredTerritory(i32);
    void GetGameAttentionValue(i32);
    void GetTurnAttentionValue(i32);
    i32 RVConversion(i32* const);
    float TurnsToBuy(i32* const);
    i32 RVOfPosition(i32, i32, i32, i32, i32, i32, i32, i32, i32, i32);
    i32 StrategicValueOfPosition(i32, i32, i32, i32, i32*, i32);
    i32 ValueOfTown(class town* t);
    void TurnCostResource(i32);
    float TurnValueOfObelisk(i32);
    float FutureDeflator(i32* const);
    i32 FightValueOfStack(
        class armyGroup* group,
        class hero* heroPtr,
        i32 useHero,
        i32 useTown = 0,
        i32 townId = 0,
        i32 useEnemyMods = 0
    );
    void EvaluateOneTimeCreaturePurchase(CreatureType creature, i32, i32, i32&, i32&, i32&);
    i32 QuickCombat(
        class armyGroup* attacker,
        class hero* attackerHero,
        class armyGroup* defender,
        class hero* defenderHero,
        i32,
        i32,
        float&,
        float&
    );
    void HeroInteractionAtHero(class hero* firstHero, class hero* secondHero, i32, i32*);
    void HeroInteractionAtTown(class hero* heroPtr, class town* townPtr, i32, i32*);
    void RedistributeTroops(class armyGroup* sourceArmy, class armyGroup* destinationArmy, i32, i32, i32, i32, i32);
    i32 ChooseGoldOrExperience(i32, i32);
    void ChooseEvaluateBattle(
        class armyGroup* ag1,
        class hero* h1,
        class armyGroup* ag2,
        class hero* h2,
        i32,
        i32,
        i32,
        i32&,
        i32&
    );
    i32 ChooseToFightForArtifact(ArtifactType artifact, H2_ENUM_PARAM(CreatureType, i32) monster, i32);
    i32 NetValueOfArtifact(i32, i32, i32, i32);
    i32 ChooseToPayRansomOnHero(i32);
    void BuildBuilding(class town* t, H2_ENUM_PARAM(BuildingSlotType, i32) building);
    void BuildHero(class town* townPtr, i32);
    void BuildCreature(class town* townPtr, i32, i32);
    i32 CanBuyBHC(struct BHC& bhc);
    i32 CombatMonsterEvent(
        class hero* h, H2_ENUM_PARAM(CreatureType, i32) monType, i32*, class mapCell*
     cell);
    i32 FightEvent(class hero* h, class mapCell* cell, i32);
    i32 DamageGroup(class armyGroup* ag, class hero* loser, class hero*, float);
    void IncrementHourGlass(void);
    void TownEvent(class mapCell* cell, class hero* h, i32, i32);
    i32 ComputeUpgradeValue(CreatureType baseCreatureType, CreatureType upgradedCreatureType);
    i32 ComputeValueOfSS(
        class hero* h,
        H2_ENUM_PARAM(HeroSecondarySkill, i32) skill,
        H2_ENUM_PARAM(HeroSkillLevel, i32)
     level);
    i32 ComputeValueOfFreeSS(class hero* h, H2_ENUM_PARAM(HeroSecondarySkill, i32) skill);
    i32 ManaRefreshValue(class hero* h, i32);
    i32 ValueOfEventAtPosition(i32, i32, i32, i32*);
    i32 EvaluateGenericSite(class mapCell* cell);
    i32 EvaluateBarrier(class mapCell* cell);
    i32 EvaluatePassword(class mapCell* cell);
    i32 EvaluateRecruitSite(class mapCell* cell);
    i32 EvaluateJail(class mapCell*);
    i32 EvaluateArtifactEvent(ArtifactType artifact, i32);
    i32 EvaluateMineEvent(i32, i32, i32, i32*);
    i32 EvaluateMonsterEvent(CreatureType monsterType, i32, i32*);
    i32 EvaluateHeroEvent(i32, i32, i32, i32, i32*);
    i32 EvaluateTownEvent(i32, i32, i32, i32, i32*);
};
#endif
