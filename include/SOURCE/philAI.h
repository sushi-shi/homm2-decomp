#ifndef HOMM2_SOURCE_PHILAI_H
#define HOMM2_SOURCE_PHILAI_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 75 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class armyGroup;
class hero;
class mapCell;
class town;
struct BHC;

class philAI {
public:
    // --- constructors ---
    philAI(void);
    // --- methods ---
    void DoAllHeroInteractions(void);
    void CheckForCreatureUpgrades(void);
    void CheckBuyStuff(void);
    i32 GoodAdjacent(i32*);
    void CheckReload(void);
    void CheckBerserk(void);
    void DimensionDoorTo(i32, i32);
    i32 DoAnywhereDDoorTownGate(i32);
    i32 DoDimensionDoor(class hero*);
    void SetupRelativeHeroStrengths(void);
    void DoAI(i32);
    void GetGameAIVars(void);
    void GetTurnAIVars(i32);
    void GetBestBHC(i32, struct BHC&);
    class hero* DetermineHeroToMove(i32);
    i32 DetermineTargetPosition(i32&, i32&, i32, i32&);
    void ProbableOutcomeOfBattle(
        class armyGroup*,
        class hero*,
        class armyGroup*,
        class hero*,
        class armyGroup*,
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
    void ValueOfBuyingBuilding(class town*, BuildingSlotType, i32&, float&);
    void GetBestBuilding(class town*, struct BHC&, float&);
    void ValueOfBuyingCreature(class town*, i32, i32&, i32, float&);
    void GetBestCreature(class town*, struct BHC&, float&);
    i32 CreaturesToBuy(class town*, i32);
    i32 CreaturesToBuy(i32, i32);
    i32 MaxBuyableCreatures(i32);
    void ValueOfBuyingHero(class town*, class hero*, i32&, float&);
    void GetBestHero(class town*, struct BHC&, float&);
    void
    LikelihoodOfEnemyAttacking(class town*, class hero*, float&, float&, i32&, i32&, i32&, float&);
    i32 MeanRVOfUnexploredTerritory(i32);
    void GetGameAttentionValue(i32);
    void GetTurnAttentionValue(i32);
    i32 RVConversion(i32* const);
    float TurnsToBuy(i32* const);
    i32 RVOfPosition(i32, i32, i32, i32, i32, i32, i32, i32, i32, i32);
    i32 StrategicValueOfPosition(i32, i32, i32, i32, i32*, i32);
    i32 ValueOfTown(class town*);
    void TurnCostResource(i32);
    float TurnValueOfObelisk(i32);
    float FutureDeflator(i32* const);
    i32 FightValueOfStack(class armyGroup*, class hero*, i32, i32, i32, i32);
    void EvaluateOneTimeCreaturePurchase(i32, i32, i32, i32&, i32&, i32&);
    i32 QuickCombat(
        class armyGroup*,
        class hero*,
        class armyGroup*,
        class hero*,
        i32,
        i32,
        float&,
        float&
    );
    void HeroInteractionAtHero(class hero*, class hero*, i32, i32*);
    void HeroInteractionAtTown(class hero*, class town*, i32, i32*);
    void RedistributeTroops(class armyGroup*, class armyGroup*, i32, i32, i32, i32, i32);
    i32 ChooseGoldOrExperience(i32, i32);
    void ChooseEvaluateBattle(
        class armyGroup*,
        class hero*,
        class armyGroup*,
        class hero*,
        i32,
        i32,
        i32,
        i32&,
        i32&
    );
    i32 ChooseToFightForArtifact(i32, i32, i32);
    i32 NetValueOfArtifact(i32, i32, i32, i32);
    i32 ChooseToPayRansomOnHero(i32);
    void BuildBuilding(class town*, i32);
    void BuildHero(class town*, i32);
    void BuildCreature(class town*, i32, i32);
    i32 CanBuyBHC(struct BHC&);
    i32 CombatMonsterEvent(class hero*, i32, i32*, class mapCell*);
    i32 FightEvent(class hero*, class mapCell*, i32);
    i32 DamageGroup(class armyGroup*, class hero*, class hero*, float);
    void IncrementHourGlass(void);
    void TownEvent(class mapCell*, class hero*, i32, i32);
    i32 ComputeUpgradeValue(i32, i32);
    i32 ComputeValueOfSS(class hero*, i32, i32);
    i32 ComputeValueOfFreeSS(class hero*, i32);
    i32 ManaRefreshValue(class hero*, i32);
    i32 ValueOfEventAtPosition(i32, i32, i32, i32*);
    i32 EvaluateGenericSite(class mapCell*);
    i32 EvaluateBarrier(class mapCell*);
    i32 EvaluatePassword(class mapCell*);
    i32 EvaluateRecruitSite(class mapCell*);
    i32 EvaluateJail(class mapCell*);
    i32 EvaluateArtifactEvent(ArtifactType, i32);
    i32 EvaluateMineEvent(i32, i32, i32, i32*);
    i32 EvaluateMonsterEvent(CreatureType, i32, i32*);
    i32 EvaluateHeroEvent(i32, i32, i32, i32, i32*);
    i32 EvaluateTownEvent(i32, i32, i32, i32, i32*);
};
#endif // HOMM2_SOURCE_PHILAI_H
