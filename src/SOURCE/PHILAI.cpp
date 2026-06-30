// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\PHILAI.OBJ   from: (directly linked into exe)
// functions: 82   data: 51
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/philAI.h>
#include <SOURCE/hero.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/advManager.h>

// ---- globals referenced by this TU (storage types fixed by mangled names) ----
class game;
class playerData;
extern game *gpGame;
extern hero *gpCurAIHero;
extern playerData *gpCurPlayer;
extern advManager *gpAdvManager;
extern int iDummy;
extern int iAlphaMale;
extern float gafAITurnCostResource[7];
// gafAITurnCostResource[3]/[5]/[6] reach the linker as their own constant symbols
// (zero addend) — declared distinctly so the fmuls displacement is 0 like retail.
extern float const_00127c14;          // gafAITurnCostResource[3]
extern float const_00127c1c;          // gafAITurnCostResource[5]
extern float const_00127c20;          // gafAITurnCostResource[6]
// RVConversion weights for ore/mercury/crystal are their own .rdata float consts;
// referencing them as symbols (not inline literals) keeps /Od term order in-source.
extern float gRVWeightOre;            // p[2]
extern float gRVWeightMerc;           // p[1]
extern float gRVWeightCrystal;        // p[4]
extern int gArtifactBaseRV[100];
extern signed char giBuildShipyard[6];
extern signed char giBuildBoat[6];
extern signed char giBuildBoatStuffTurn[6];
extern signed char gDwellingType[20][12];
void GetMonsterCost(int, int *);

// ---- AI member views: the recovered classes carry no field layout, so model the
// exact offsets the /Od code touches as packed views and cast to them. ----
#pragma pack(push, 1)
struct monsterRV { int rv; char pad[22]; };          // 26-byte record (pack 1)
extern monsterRV gMonsterInfo[100];
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
    char _0[3];
    signed char race;            // 0x03
    char _4[0x17 - 0x04];
    signed char visitingHero;    // 0x17
    char _18[0x1e - 0x18];
    signed short garrison[5];    // 0x1e
    signed char getVisitingHero() { return visitingHero; }
};
struct mapCellView {             // mapCell
    char _0[4];
    unsigned short _bits : 3;
    unsigned short tentColor : 13; // bits 3..15 of word @ 0x04
};
#pragma pack(pop)

VA(0x0043781b, 0x1b5)
// void ResetHeroRVs(int, int, int);

VA(0x004379d0, 0x180)
// void CheckDoMain(int, int);

VA(0x00437b50, 0x10)
void ShowStatus(void) {}

VA(0x00437b60, 0x55)
philAI::philAI(void) {
    int i;
    for (i = 0; i < 6; i++) {
        giBuildShipyard[i] = -1;
        giBuildBoat[i] = -1;
        giBuildBoatStuffTurn[i] = 0;
    }
}

VA(0x00437bb5, 0xac)
void philAI::DoAllHeroInteractions(void) {
    int i;
    for (i = 0; i < ((pdView *)gpCurPlayer)->numCastles; i++) {
        townView *pTown = (townView *)((char *)gpGame +
            ((pdView *)gpCurPlayer)->getCastleId(i) * 100 + 0xb53);
        if (pTown->visitingHero != -1)
            HeroInteractionAtTown(
                (hero *)((char *)gpGame + pTown->getVisitingHero() * 250 + 0x27c4),
                (town *)pTown, 0, &iDummy);
    }
}

VA(0x00437c61, 0x37e)
// void philAI::CheckForCreatureUpgrades(void);

VA(0x00437fdf, 0x4be)
// void philAI::CheckBuyStuff(void);

VA(0x0043849d, 0x2e8)
// int philAI::GoodAdjacent(int *);

VA(0x00438785, 0x4b8)
// void philAI::CheckReload(void);

VA(0x00438c3d, 0x302)
// void philAI::CheckBerserk(void);

VA(0x00438f3f, 0x71)
void philAI::DimensionDoorTo(int x, int y) {
    gpAdvManager->TeleportTo(gpCurAIHero, x, y, 0, 0);
    if (*(int *)((char *)gpCurAIHero + 0x35) < 0xe1)
        *(int *)((char *)gpCurAIHero + 0x35) = 0;
    else
        *(int *)((char *)gpCurAIHero + 0x35) -= 0xe1;
    gpCurAIHero->UseSpell(0x38);
}

VA(0x00438fb0, 0x3f9)
// int philAI::DoAnywhereDDoorTownGate(int);

VA(0x004393a9, 0x158)
// int philAI::DoDimensionDoor(class hero *);

// @early-stop
// Complete & byte-exact except ONE `cmp` in the best-FV update: this cl loads fv(-0x8)
// into the reg (cmp [bestFV],fv / jge) where retail loads bestFV(-0x10) (cmp [fv],bestFV
// / jle) — identical `if (fv > bestFV)` source, a cl operand-memory-selection difference
// (verified: every `<`/`>`/`<=`/else/continue form loads fv; not /QIfdiv-related).
VA(0x00439501, 0xb7)
void philAI::SetupRelativeHeroStrengths(void) {
    hero *alpha;
    int fv;
    int i;
    int bestFV;
    bestFV = -1;
    iAlphaMale = -1;
    for (i = 0; i < ((pdView *)gpCurPlayer)->numHeroes; i++) {
        alpha = (hero *)((char *)gpGame +
            ((pdView *)gpCurPlayer)->getHeroId(i) * 250 + 0x27c4);
        fv = FightValueOfStack((armyGroup *)((char *)alpha + 0x65), alpha, 1, 0, -1, 0);
        if (fv > bestFV) {
            bestFV = fv;
            iAlphaMale = ((heroView *)alpha)->heroId;
        }
    }
}

VA(0x004395b8, 0x79)
void ValidateHero(hero *pHero) {
    int i;
    for (i = 0; i < 5; i++) {
        if (((heroView *)pHero)->army[i] != -1) {
            if (((heroView *)pHero)->army[i] < 0 || ((heroView *)pHero)->army[i] >= 0x42) {
                int j = 1;
                j++;
            }
        }
    }
}

VA(0x00439631, 0xcad)
// void philAI::DoAI(int);

VA(0x0043a2de, 0x4b)
void philAI::GetGameAIVars(void) {
    int i;
    for (i = 0; i < *(signed char *)((char *)gpGame + 0x48e); i++)
        GetGameAttentionValue(i);
}

VA(0x0043a329, 0xe2b)
// void philAI::GetTurnAIVars(int);

VA(0x0043b154, 0x5f4)
// void philAI::GetBestBHC(int, struct BHC &);

VA(0x0043b748, 0x11d)
// class hero * philAI::DetermineHeroToMove(int);

VA(0x0043b865, 0xe7d)
// int philAI::DetermineTargetPosition(int &, int &, int, int &);

VA(0x0043c6e2, 0x791)
// void philAI::ProbableOutcomeOfBattle(class armyGroup *, class hero *, class armyGroup *, class hero *, class armyGroup *, int, int, int, float &, int &, int &, int &, int &, int &);

VA(0x0043ce73, 0x1e)
float philAI::GetOddsOfWinning(int) {
    return 1.0f;
}

VA(0x0043ce91, 0x826)
// void philAI::ValueOfBuyingBuilding(class town *, int, int &, float &);

VA(0x0043d6b7, 0x19b)
// void philAI::GetBestBuilding(class town *, struct BHC &, float &);

VA(0x0043d852, 0x306)
// void philAI::ValueOfBuyingCreature(class town *, int, int &, int, float &);

VA(0x0043db58, 0x35b)
// void philAI::GetBestCreature(class town *, struct BHC &, float &);

VA(0x0043deb3, 0x48)
int philAI::CreaturesToBuy(town *t, int level) {
    int nGarrison = ((townView *)t)->garrison[level];
    return CreaturesToBuy((int)gDwellingType[((townView *)t)->race][level], nGarrison);
}

// @early-stop
// Complete & byte-exact except ONE clamp `cmp`: this cl loads the param b(0xc) into the
// reg (cmp [n],b / jle) where retail loads the local n(-0x4) (cmp [b],n / jge) — identical
// `if (b < n) n = b;` source, a cl operand-memory-selection difference (verified across
// `<`/`>` and cdecl/thiscall probes; not /QIfdiv-related).
VA(0x0043defb, 0x5f)
int philAI::CreaturesToBuy(int a, int b) {
    int n = MaxBuyableCreatures(a);
    if (n > 1)
        n >>= 1;
    if (b < n)
        n = b;
    if (n > 1)
        return n;
    else
        return 0;
}

VA(0x0043df5a, 0x9c)
int philAI::MaxBuyableCreatures(int level) {
    int cost7[7];
    int res;
    int i;
    GetMonsterCost(level, cost7);
    for (i = 0; i < 7; i++) {
        if (cost7[i] == 0)
            res = 0x270f;
        else if (((pdView *)gpCurPlayer)->resources[i] > 0)
            res = ((pdView *)gpCurPlayer)->resources[i] / cost7[i];
        else
            res = 0;
    }
    return res;
}

VA(0x0043dff6, 0x2b2)
// void philAI::ValueOfBuyingHero(class town *, class hero *, int &, float &);

VA(0x0043e2a8, 0x1b1)
// void philAI::GetBestHero(class town *, struct BHC &, float &);

VA(0x0043e459, 0x65)
void philAI::LikelihoodOfEnemyAttacking(town *, hero *, float &chanceA, float &chanceB,
                                        int &nAttack, int &nValue, int &nWeeks, float &fOut) {
    chanceA = 0.15f;
    chanceB = 0.6f;
    nAttack = 3000;
    nValue = (int)((float)nAttack * chanceA);
    nWeeks = 6;
    fOut = chanceB * chanceA;
}

VA(0x0043e4be, 0x1a)
int philAI::MeanRVOfUnexploredTerritory(int) { return 0; }

VA(0x0043e4d8, 0x1d8)
// void philAI::GetGameAttentionValue(int);

VA(0x0043e6b0, 0xf2)
// void philAI::GetTurnAttentionValue(int);

VA(0x0043e7a2, 0xa6)
int philAI::RVConversion(int *const p) {
    // explicit left-grouping: a flat `+` chain is reassociated by /Od to a fixed
    // canonical order; nesting forces strict left-to-right (retail) evaluation.
    return (int)((((((((float)p[2] * gRVWeightOre)
               + (float)p[5] * const_00127c1c)
               + (float)p[1] * gRVWeightMerc)
               + (float)p[6] * const_00127c20)
               + (float)p[0] * gafAITurnCostResource[0])
               + (float)p[3] * const_00127c14)
               + (float)p[4] * gRVWeightCrystal);
}

VA(0x0043e848, 0xd0)
// float philAI::TurnsToBuy(int * const);

VA(0x0043e918, 0x62d)
// int philAI::RVOfPosition(int, int, int, int, int, int, int, int, int, int);

VA(0x0043ef45, 0xaf9)
// int philAI::StrategicValueOfPosition(int, int, int, int, int *, int);

VA(0x0043fa3e, 0x14e)
// int philAI::ValueOfTown(class town *);

VA(0x0043fb8c, 0x180)
// void philAI::TurnCostResource(int);

VA(0x0043fd0c, 0x175)
// float philAI::TurnValueOfObelisk(int);

VA(0x0043fe81, 0x51)
float philAI::FutureDeflator(int *const p) {
    float t = TurnsToBuy(p);
    float v = 1.0f - t * 0.1f;
    if (v < 0.0)
        v = 0;
    return v;
}

VA(0x0043fed2, 0xbf8)
// int philAI::FightValueOfStack(class armyGroup *, class hero *, int, int, int, int);

VA(0x00440aca, 0x1e7)
// void philAI::EvaluateOneTimeCreaturePurchase(int, int, int, int &, int &, int &);

VA(0x00440cb1, 0x768)
// int philAI::QuickCombat(class armyGroup *, class hero *, class armyGroup *, class hero *, int, int, float &, float &);

VA(0x00441419, 0x422)
// void philAI::HeroInteractionAtHero(class hero *, class hero *, int, int *);

VA(0x0044183b, 0x882)
// void philAI::HeroInteractionAtTown(class hero *, class town *, int, int *);

VA(0x004420bd, 0x4ba)
// void philAI::RedistributeTroops(class armyGroup *, class armyGroup *, int, int, int, int, int);

VA(0x00442577, 0x39)
int philAI::ChooseGoldOrExperience(int, int) {
    return ((pdView *)gpCurPlayer)->resources[6] > 4000 ? 1 : 0;
}

VA(0x004425b0, 0xc7)
// void philAI::ChooseEvaluateBattle(class armyGroup *, class hero *, class armyGroup *, class hero *, int, int, int, int &, int &);

VA(0x00442677, 0xfa)
// int philAI::ChooseToFightForArtifact(int, int, int);

VA(0x00442771, 0x53)
int philAI::NetValueOfArtifact(int a1, int a2, int a3, int a4) {
    return (int)((float)gArtifactBaseRV[a1]
               - ((float)a4 * gafAITurnCostResource[a3] + (float)a2 * const_00127c20));
}

VA(0x004427c4, 0x1d)
int philAI::ChooseToPayRansomOnHero(int) { return 1; }

VA(0x004427e1, 0xd7)
// void philAI::BuildBuilding(class town *, int);

VA(0x004428b8, 0x328)
// void philAI::BuildHero(class town *, int);

VA(0x00442be0, 0x2cd)
// void philAI::BuildCreature(class town *, int, int);

VA(0x00442ead, 0x15a)
// int philAI::CanBuyBHC(struct BHC &);

VA(0x00443007, 0x164)
// int philAI::CombatMonsterEvent(class hero *, int, int *, class mapCell *);

VA(0x0044316b, 0x6d7)
// int philAI::FightEvent(class hero *, class mapCell *, int);

VA(0x00443842, 0x73)
int philAI::DamageGroup(armyGroup *ag, hero *loser, hero *, float dmg) {
    if (dmg < 1.0) {
        ag->DamageGroup(dmg);
        return 0;
    } else {
        if (loser != 0)
            gpAdvManager->HeroLoses(loser);
        else
            ag->DamageGroup(dmg);
        return 1;
    }
}

VA(0x004438b5, 0xcb)
// void philAI::IncrementHourGlass(void);

VA(0x00443980, 0x227)
// void philAI::TownEvent(class mapCell *, class hero *, int, int);

VA(0x00443ba7, 0xad)
int philAI::ComputeUpgradeValue(int a1, int a2) {
    int cnt = gpCurAIHero->CreatureTypeCount(a1);
    if (cnt == 0)
        return 0;
    int result = (int)((float)((gMonsterInfo[a2].rv - gMonsterInfo[a1].rv) * cnt)
                       * ((pdView *)gpCurPlayer)->upgradeFactor);
    if (gpCurAIHero->CreatureTypeCount(a2) != 0)
        result = (int)(result * 0.6);
    return result;
}

VA(0x00443c54, 0x271)
// int philAI::ComputeValueOfSS(class hero *, int, int);

VA(0x00443ec5, 0x59)
int philAI::ComputeValueOfFreeSS(hero *h, int ss) {
    if (((heroView *)h)->skills[ss] != 0 || ((heroView *)h)->level >= 8)
        return 0;
    else
        return ComputeValueOfSS(h, ss, 1);
}

VA(0x00443f1e, 0xa6)
int philAI::ManaRefreshValue(hero *h, int level) {
    int v = 0;
    int sp = h->Stats(3) * level * 10;
    int deficit = sp - ((heroView *)h)->curMana;
    if (deficit <= 0)
        return 0;
    float fr = (float)deficit / sp;
    if (deficit > 0)
        v = (int)((float)(deficit * 5) * fr);
    return v;
}

VA(0x00443fc4, 0x1ac5)
// int philAI::ValueOfEventAtPosition(int, int, int, int *);

VA(0x00445a89, 0x299)
// int philAI::EvaluateGenericSite(class mapCell *);

VA(0x00445d22, 0x5e)
int philAI::EvaluateBarrier(mapCell *cell) {
    int color = ((mapCellView *)cell)->tentColor;
    color &= 7;
    if (((pdView *)gpCurPlayer)->barrierTents & (1 << color))
        return 5000;
    else
        return 0;
}

VA(0x00445d80, 0x5e)
int philAI::EvaluatePassword(mapCell *cell) {
    int color = ((mapCellView *)cell)->tentColor;
    color &= 7;
    if (!(((pdView *)gpCurPlayer)->barrierTents & (1 << color)))
        return 2500;
    else
        return 0;
}

VA(0x00445dde, 0xf5)
// int philAI::EvaluateRecruitSite(class mapCell *);

VA(0x00445ed3, 0x1d)
int philAI::EvaluateJail(mapCell *) {
    return 10000;
}

VA(0x00445ef0, 0xf6)
// void InitAIMapVars(void);

VA(0x00445fe6, 0x112)
// void CloseAIMapVars(void);

VA(0x004460f8, 0x26a)
// int OnMySide(int);

VA(0x00446362, 0x2bc)
// int philAI::EvaluateArtifactEvent(int, int);

VA(0x0044661e, 0x30a)
// int philAI::EvaluateMineEvent(int, int, int, int *);

VA(0x00446928, 0x33e)
// int philAI::EvaluateMonsterEvent(int, int, int *);

VA(0x00446c66, 0x5ab)
// int philAI::EvaluateHeroEvent(int, int, int, int, int *);

VA(0x00447211, 0x4fa)
// int philAI::EvaluateTownEvent(int, int, int, int, int *);

// ---- data / globals / vtables ----
DATA(0x004f20e0)  // float fFirstWeekTownFV
DATA(0x004f20e4)  // int iVepCacheHits
DATA(0x004f20e8)  // int iTotalVepHits
DATA(0x004f20ec)  // int giShowComputerRoute
DATA(0x004f20f0)  // short int * gaiLiveChanceOfPos
DATA(0x004f20f4)  // short int * gaiHeroStrategicRVOfPos
DATA(0x004f20f8)  // short int * gaiHeroEventStratRVOfPos
DATA(0x004f20fc)  // signed char * gaiTurnValueOfMine
DATA(0x004f2100)  // signed char * gaiEnemyHeroReachable
DATA(0x004f2104)  // long int glLastStartTick
DATA(0x004f2108)  // long int glCurTicks
DATA(0x004f210c)  // long int glTotalTicks
DATA(0x004f2110)  // class hero * gpCurAIHero
DATA(0x004f2114)  // float gfAttackHumanBonus
DATA(0x004f2118)  // float gfAttackComputerBonus
DATA(0x004f211c)  // int iLastFrameRateTimer
DATA(0x004f22bc)  // int bSVSearchArrayInUse
DATA(0x004f2340)  // int bEvaluatingTravelGates
DATA(0x00525620)  // int gbReduceByBerserk
DATA(0x00525624)  // float fBerserkFactor
DATA(0x00525628)  // int giCurPlayer
DATA(0x0052562c)  // signed char * giBuildShipyard
DATA(0x00525634)  // int giMaxHeroesForThisPlayer
DATA(0x00525638)  // signed char * giBuildBoat
DATA(0x00525640)  // float fReduceFactor
DATA(0x00525644)  // unsigned char giCurPlayerBit
DATA(0x00525648)  // int giBestShipyardDist
DATA(0x0052564c)  // int bHeroBuiltThisTurn
DATA(0x00525650)  // short int * gaiHeroLiveChance
DATA(0x005256bc)  // int giHumanTownConquered
DATA(0x005256c0)  // int giCurTurn
DATA(0x005256c8)  // int * costTemp
DATA(0x005256e4)  // int iAlphaMale
DATA(0x005256e8)  // int iDummy
DATA(0x005256ec)  // int gbPossibleShipyardFound
DATA(0x005256f0)  // class searchArray SVSearchArray
DATA(0x00527c08)  // float * gafAITurnCostResource
DATA(0x00527c24)  // int iCurPlaceToVisit
DATA(0x00527c28)  // int giBestShipyardId
DATA(0x00527c2c)  // int gbActualBoatFound
DATA(0x00527c30)  // unsigned char giCurWatchPlayerBit
DATA(0x00527c34)  // class playerData * gpCurPlayer
DATA(0x00527c38)  // float * gfHeroInteractionBonus
DATA(0x00527d10)  // int gbBerserk
DATA(0x00527d14)  // int giCurAIHeroMorale
DATA(0x00527d18)  // signed char * giBuildBoatStuffTurn
DATA(0x00527d20)  // int (*)[2] iPlacesVisited
DATA(0x00527e10)  // int gbReduceByReload
DATA(0x00527e14)  // int gbTroopReload
DATA(0x00527e18)  // int giCurAIHeroLuck
DATA(0x00527e1c)  // int gbActualShipyardFound
