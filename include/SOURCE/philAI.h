#ifndef HOMM2_SOURCE_PHILAI_H
#define HOMM2_SOURCE_PHILAI_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 75 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"
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
    int GoodAdjacent(int *);
    void CheckReload(void);
    void CheckBerserk(void);
    void DimensionDoorTo(int, int);
    int DoAnywhereDDoorTownGate(int);
    int DoDimensionDoor(class hero *);
    void SetupRelativeHeroStrengths(void);
    void DoAI(int);
    void GetGameAIVars(void);
    void GetTurnAIVars(int);
    void GetBestBHC(int, struct BHC &);
    class hero * DetermineHeroToMove(int);
    int DetermineTargetPosition(int &, int &, int, int &);
    void ProbableOutcomeOfBattle(class armyGroup *, class hero *, class armyGroup *, class hero *, class armyGroup *, int, int, int, float &, int &, int &, int &, int &, int &);
    float GetOddsOfWinning(int);
    void ValueOfBuyingBuilding(class town *, int, int &, float &);
    void GetBestBuilding(class town *, struct BHC &, float &);
    void ValueOfBuyingCreature(class town *, int, int &, int, float &);
    void GetBestCreature(class town *, struct BHC &, float &);
    int CreaturesToBuy(class town *, int);
    int CreaturesToBuy(int, int);
    int MaxBuyableCreatures(int);
    void ValueOfBuyingHero(class town *, class hero *, int &, float &);
    void GetBestHero(class town *, struct BHC &, float &);
    void LikelihoodOfEnemyAttacking(class town *, class hero *, float &, float &, int &, int &, int &, float &);
    int MeanRVOfUnexploredTerritory(int);
    void GetGameAttentionValue(int);
    void GetTurnAttentionValue(int);
    int RVConversion(int * const);
    float TurnsToBuy(int * const);
    int RVOfPosition(int, int, int, int, int, int, int, int, int, int);
    int StrategicValueOfPosition(int, int, int, int, int *, int);
    int ValueOfTown(class town *);
    void TurnCostResource(int);
    float TurnValueOfObelisk(int);
    float FutureDeflator(int * const);
    int FightValueOfStack(class armyGroup *, class hero *, int, int, int, int);
    void EvaluateOneTimeCreaturePurchase(int, int, int, int &, int &, int &);
    int QuickCombat(class armyGroup *, class hero *, class armyGroup *, class hero *, int, int, float &, float &);
    void HeroInteractionAtHero(class hero *, class hero *, int, int *);
    void HeroInteractionAtTown(class hero *, class town *, int, int *);
    void RedistributeTroops(class armyGroup *, class armyGroup *, int, int, int, int, int);
    int ChooseGoldOrExperience(int, int);
    void ChooseEvaluateBattle(class armyGroup *, class hero *, class armyGroup *, class hero *, int, int, int, int &, int &);
    int ChooseToFightForArtifact(int, int, int);
    int NetValueOfArtifact(int, int, int, int);
    int ChooseToPayRansomOnHero(int);
    void BuildBuilding(class town *, int);
    void BuildHero(class town *, int);
    void BuildCreature(class town *, int, int);
    int CanBuyBHC(struct BHC &);
    int CombatMonsterEvent(class hero *, int, int *, class mapCell *);
    int FightEvent(class hero *, class mapCell *, int);
    int DamageGroup(class armyGroup *, class hero *, class hero *, float);
    void IncrementHourGlass(void);
    void TownEvent(class mapCell *, class hero *, int, int);
    int ComputeUpgradeValue(int, int);
    int ComputeValueOfSS(class hero *, int, int);
    int ComputeValueOfFreeSS(class hero *, int);
    int ManaRefreshValue(class hero *, int);
    int ValueOfEventAtPosition(int, int, int, int *);
    int EvaluateGenericSite(class mapCell *);
    int EvaluateBarrier(class mapCell *);
    int EvaluatePassword(class mapCell *);
    int EvaluateRecruitSite(class mapCell *);
    int EvaluateJail(class mapCell *);
    int EvaluateArtifactEvent(int, int);
    int EvaluateMineEvent(int, int, int, int *);
    int EvaluateMonsterEvent(int, int, int *);
    int EvaluateHeroEvent(int, int, int, int, int *);
    int EvaluateTownEvent(int, int, int, int, int *);
};
#endif // HOMM2_SOURCE_PHILAI_H
