// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\PHILAI.OBJ   from: (directly linked into exe)
// functions: 82   data: 51
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/PHILAI.h>
#include <_types.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/KB.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <SOURCE/philAI.h>
#include <SOURCE/hero.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/advManager.h>
#include <SOURCE/town.h>
#include <SOURCE/game.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>

// Globals -> _globals.h; tag_tilePoint/monsterRV -> _types.h; called free functions
// from owner headers (Misc.h/KB.h/kbwin.h/NOOPT.h); PHILAI's own -> SOURCE/PHILAI.h.

// __FILE__ for the NWC BaseAlloc/BaseFree memory tracking (reloc-masked path string).
#define PHFILE ((char *)"I:\\Projects\\Heroes\\Prog\\SOURCE\\PHILAI.CPP")

// AI view/record structs (BHC/pdView/.../gameView) -> SOURCE/PHILAI.h.

VA(0x0043781b, 0x1b5)
void ResetHeroRVs(int, int, int) {}

VA(0x004379d0, 0x180)
void CheckDoMain(int a1, int doMain) {
    if (KBTickCount() > iLastFrameRateTimer + 0xf || KBTickCount() > glTimers[0]) {
        Process1WindowsMessage();
        PollSound();
        if (KBTickCount() > glTimers[0]) {
            if (doMain == 0) {
                int idx = bShowIt;
                int savedX = *(int *)((char *)gpAdvManager + 0x1de);
                int savedY = *(int *)((char *)gpAdvManager + 0x1e2);
                gbDrawSavedCursor = 1;
                if (const_00128d38 == 0 && gbRemoteOn == 0)
                    bShowIt = 1;
                else
                    bShowIt = 0;
                if (bShowIt == 0)
                    bSpecialHideCursor = 1;
                if (gpAdvManager->ComboDraw(*(int *)((char *)gpAdvManager + 0x1de),
                                            *(int *)((char *)gpAdvManager + 0x1e2), 0))
                    gpAdvManager->UpdateScreen(0, 0);
                else
                    gpAdvManager->UpdBottomView(0, 1, 1);
                bShowIt = idx;
                gbDrawSavedCursor = 0;
                bSpecialHideCursor = 0;
                *(int *)((char *)gpAdvManager + 0x1de) = savedX;
                *(int *)((char *)gpAdvManager + 0x1e2) = savedY;
            }
            glTimers[0] = KBTickCount() + 0x78;
        }
        iLastFrameRateTimer = KBTickCount();
    }
}

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
void philAI::CheckForCreatureUpgrades(void) {}

VA(0x00437fdf, 0x4be)
void philAI::CheckBuyStuff(void) {}

VA(0x0043849d, 0x2e8)
int philAI::GoodAdjacent(int *) { return 0; }

VA(0x00438785, 0x4b8)
void philAI::CheckReload(void) {}

VA(0x00438c3d, 0x302)
void philAI::CheckBerserk(void) {}

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
int philAI::DoAnywhereDDoorTownGate(int) { return 0; }

// @early-stop
// Complete & byte-exact except ONE `movsbl normalDirTable[dir].y`: cl emits disp 0x2
// (reloc normalDirTable) where retail shows disp 0x0 + a distinct reloc — the delinker
// split normalDirTable+2 into its own symbol. Same effective address; delinker artifact.
VA(0x004393a9, 0x158)
int philAI::DoDimensionDoor(hero *pHero) {
    int node;           // i
    int nb;             // y
    int kn;             // x
    mapCell *jb;        // cell
    int idx;            // dist
    int bestX, bestY;
    if (*(int *)((char *)pHero + 0x35) < 0x4b)
        return 0;
    bestX = -1;
    kn = *(int *)((char *)pHero + 0x19);
    nb = *(int *)((char *)pHero + 0x1d);
    for (node = *(int *)((char *)gpSearchArray + 0x8) - 1; node >= 1; node--) {
        kn += normalDirTable[*(unsigned char *)((char *)gpSearchArray + 0x2418 + node)].x;
        nb += normalDirTable[*(unsigned char *)((char *)gpSearchArray + 0x2418 + node)].y;
        if (abs(kn - *(int *)((char *)pHero + 0x19)) <= 7 &&
            abs(nb - *(int *)((char *)pHero + 0x1d)) <= 7) {
            jb = gpAdvManager->GetCell(kn, nb);
            if (!(*(unsigned char *)((char *)jb + 0x9) & 0x80) &&
                !(*(unsigned char *)((char *)jb + 0x8) & 0x8)) {
                bestX = kn;
                bestY = nb;
                idx = *(int *)((char *)gpSearchArray + 0x8) - node;
            }
        }
    }
    if (bestX == -1 || idx <= 5)
        return 0;
    DimensionDoorTo(bestX, bestY);
    return 1;
}

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
void philAI::DoAI(int) {}

VA(0x0043a2de, 0x4b)
void philAI::GetGameAIVars(void) {
    int i;
    for (i = 0; i < *(signed char *)((char *)gpGame + 0x48e); i++)
        GetGameAttentionValue(i);
}

VA(0x0043a329, 0xe2b)
void philAI::GetTurnAIVars(int) {}

VA(0x0043b154, 0x5f4)
void philAI::GetBestBHC(int, struct BHC &) {}

// @early-stop
// Complete & correct; two residuals are /Od codegen-shape picks (verified via scratch cl,
// not source-steerable): (1) the hero-slot 2D access gpGame[0x4a0+player*283+i] — cl emits
// the full player*283 then `+i`; retail strength-reduces to (i-player)+player*284 (identical
// address). (2) the fight-value max `cmp` loads the fresh value where retail loads the
// accumulator (the same operand-memory pick parked on SetupRelativeHeroStrengths).
VA(0x0043b748, 0x11d)
hero *philAI::DetermineHeroToMove(int player) {
    int val;            // per-hero value
    int jb;             // best value
    int idx;            // best hero index
    int i;
    jb = 0;
    idx = -1;
    if (((playerData *)gpCurPlayer)->HasMobileHero()) {
        for (i = 0; i < ((pdView *)gpCurPlayer)->numHeroes; i++) {
            val = *(int *)((char *)gpGame +
                ((gameView *)gpGame)->heroSlots[player][i] * 250 + 0x27f9);
            if (val > jb) {
                jb = val;
                idx = i;
            }
        }
    }
    if (idx >= 0)
        return (hero *)((char *)gpGame +
            ((gameView *)gpGame)->heroSlots[player][idx] * 250 + 0x27c4);
    *(signed char *)((char *)gpGame + 0x49e + player * 283) = -1;
    return 0;
}

VA(0x0043b865, 0xe7d)
int philAI::DetermineTargetPosition(int &, int &, int, int &) { return 0; }

VA(0x0043c6e2, 0x791)
void philAI::ProbableOutcomeOfBattle(class armyGroup *, class hero *, class armyGroup *, class hero *, class armyGroup *, int, int, int, float &, int &, int &, int &, int &, int &) {}

VA(0x0043ce73, 0x1e)
float philAI::GetOddsOfWinning(int) {
    return 1.0f;
}

VA(0x0043ce91, 0x826)
void philAI::ValueOfBuyingBuilding(class town *, int, int &, float &) {}

// @early-stop
// Complete & correct except ONE store/compare pair: cl emits `fcoms kn; fstps score`
// where retail emits `fsts score; fcomps kn` (identical result) — an /Od FPU store-vs-
// compare scheduling pick around the conditional debug-log reuse of `score`.
VA(0x0043d6b7, 0x19b)
void philAI::GetBestBuilding(town *t, BHC &bhc, float &fOut) {
    float score;        // -0x18
    int node;           // i
    float nb;           // best randomized BC -0x10
    float kn;           // best raw BC -0xc
    int jb;             // best building idx -0x8
    float idx;          // raw BC -0x4
    int cost;
    nb = -99.0f;
    kn = -99.0f;
    jb = -1;
    for (node = 0; node < 0x20; node++) {
        if (!(((townView *)t)->buildings & (1 << node)) ||
            (node == 0 && ((townView *)t)->buildState < 5)) {
            if (CanBuild(t, node)) {
                ValueOfBuyingBuilding(t, node, cost, idx);
                if (*(int *)((char *)gpCurPlayer + 0xf) == 1)
                    cost = (int)(cost * 1.3);
                score = (Random(1, 5) + 0x5f) * idx / 100.0f;
                if (score > kn) {
                    jb = node;
                    nb = idx;
                    kn = score;
                }
                if (giDebugLevel >= 5) {
                    sprintf(gText, "Town:%2d  Building: % 18s   Raw BC = %8.2f,  RandBC = %8.2f.",
                            ((townView *)t)->id, GetBuildingName(((townView *)t)->race, node), idx, score);
                    LogStr(gText);
                }
            }
        }
    }
    bhc.pTown = t;
    bhc.type = 0;
    bhc.what = jb;
    fOut = nb;
}

VA(0x0043d852, 0x306)
void philAI::ValueOfBuyingCreature(class town *, int, int &, int, float &) {}

VA(0x0043db58, 0x35b)
void philAI::GetBestCreature(class town *, struct BHC &, float &) {}

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
void philAI::ValueOfBuyingHero(class town *, class hero *, int &, float &) {}

VA(0x0043e2a8, 0x1b1)
void philAI::GetBestHero(class town *, struct BHC &, float &) {}

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
void philAI::GetGameAttentionValue(int) {}

VA(0x0043e6b0, 0xf2)
void philAI::GetTurnAttentionValue(int player) {
    taView *ptr = (taView *)((char *)gpGame + player * 283 + 0x54f);
    ptr->f0 = 0.4f;
    ptr->f8 = 0.3f;
    ptr->f4 = 0.3f;
    ptr->fc = ptr->f0;
    ptr->f14 = ptr->f8;
    ptr->f10 = ptr->f4;
    float factor;
    if (giCurTurn < 5)
        factor = 1.6f;
    else if (giCurTurn < 10)
        factor = 1.4f;
    else if (giCurTurn < 20)
        factor = 1.2f;
    else if (giCurTurn < 30)
        factor = 1.0f;
    else
        factor = 0.8f;
    ptr->f14 = ptr->f14 * factor;
}

VA(0x0043e7a2, 0xa6)
int philAI::RVConversion(int *const p) {
    // explicit left-grouping: a flat `+` chain is reassociated by /Od to a fixed
    // canonical order; nesting forces strict left-to-right (retail) evaluation.
    return (int)((((((((float)p[RES_ORE]     * gafAITurnCostResource[RES_ORE])
               + (float)p[RES_GEMS]    * gafAITurnCostResource[RES_GEMS])
               + (float)p[RES_MERCURY] * gafAITurnCostResource[RES_MERCURY])
               + (float)p[RES_GOLD]    * gafAITurnCostResource[RES_GOLD])
               + (float)p[RES_WOOD]    * gafAITurnCostResource[RES_WOOD])
               + (float)p[RES_SULFUR]  * gafAITurnCostResource[RES_SULFUR])
               + (float)p[RES_CRYSTAL] * gafAITurnCostResource[RES_CRYSTAL]);
}

VA(0x0043e848, 0xd0)
float philAI::TurnsToBuy(int *const p) {
    float maxT = 0;
    int i;
    float turns;
    for (i = 0; i < 7; i++) {
        if (((pdView *)gpCurPlayer)->resources[i] < p[i]) {
            if (*(int *)((char *)gpCurPlayer + 0xe7 + i * 4) > 0)
                turns = (float)((p[i] - ((pdView *)gpCurPlayer)->resources[i]) /
                                *(int *)((char *)gpCurPlayer + 0xe7 + i * 4) + 1);
            else
                turns = 99.0f;
            maxT = turns > maxT ? turns : maxT;
        }
    }
    return maxT;
}

VA(0x0043e918, 0x62d)
int philAI::RVOfPosition(int, int, int, int, int, int, int, int, int, int) { return 0; }

VA(0x0043ef45, 0xaf9)
int philAI::StrategicValueOfPosition(int, int, int, int, int *, int) { return 0; }

// @early-stop
// Complete & correct except the two castle-match `==` compares: cl unconditionally loads
// the byte operand (town castleX/Y) before the word operand (game field); retail evaluates
// left-to-right (word first). Verified via scratch cl: byte-first is hard-wired, not
// source-steerable. Same equality result.
VA(0x0043fa3e, 0x14e)
int philAI::ValueOfTown(town *t) {
    int sum = 0;
    int idx;            // i
    for (idx = 0; idx <= 0x18; idx++) {
        if (((townView *)t)->buildings & (1 << idx))
            sum += GetBuildingBaseResourceValue(((townView *)t)->race, idx, ((townView *)t)->buildState);
    }
    sum = (int)(gafAITurnCostResource[RES_GOLD] * 1250.0f * 1.5 + sum);
    sum += 0x2ee;
    if (((gameTV *)gpGame)->f2cb == 1 &&
        ((townView *)t)->castleX == ((gameTV *)gpGame)->f2cc &&
        ((gameTV *)gpGame)->f2d7 == ((townView *)t)->castleY)
        sum += 0xc350;
    if (((gameTV *)gpGame)->f2c6 == 1 &&
        ((townView *)t)->castleX == ((gameTV *)gpGame)->f2c9 &&
        ((gameTV *)gpGame)->f2d5 == ((townView *)t)->castleY)
        sum += 0xc350;
    return sum;
}

// @early-stop
// Complete & correct except the final `/ gResourceBaseValue[i]`: cl evaluates the simple
// int divisor before the float numerator -> fdivrp (guard al=0xd); retail evaluates the
// numerator first -> fdivp (al=0xf). Identical quotient; an /Od operand-eval-order pick.
VA(0x0043fb8c, 0x180)
void philAI::TurnCostResource(int player) {
    int nb;             // i
    taView *kn;         // ptr
    float jb[7];        // per-resource ratio
    float idx;          // average turn cost
    int total;
    int cost[7];
    kn = (taView *)((char *)gpGame + player * 283 + 0x54f);
    total = 0;
    for (nb = 0; nb < 7; nb++) {
        cost[nb] = (int)(((double)(kn->field34[nb] * 5) * 0.7 +
                          (double)*(int *)((char *)gpGame + player * 283 + 0x52b + nb * 4)) *
                         (double)gResourceBaseValue[nb]);
        total += cost[nb];
    }
    idx = (float)(total / 7);
    for (nb = 0; nb < 7; nb++) {
        jb[nb] = (float)cost[nb] / idx;
        gafAITurnCostResource[nb] =
            (float)((jb[nb] / 2.0f + 0.5) / gResourceBaseValue[nb]);
    }
}

VA(0x0043fd0c, 0x175)
float philAI::TurnValueOfObelisk(int player) {
    int jb;             // artifact RV
    int idx;            // turns
    taView *ta;         // ptr
    ta = (taView *)((char *)gpGame + player * 283 + 0x54f);
    jb = gArtifactBaseRV[*(signed char *)((char *)gpGame + 0x6397)];
    if (*(unsigned char *)((char *)gpGame + 0x2c6) == 3)
        jb <<= 1;
    idx = jb / 0x6e;
    if (*(signed char *)((char *)gpGame + 0x6397) == -1)
        return 0.0f;
    ta->field50 = idx * 48 / *(signed char *)((char *)gpGame + 0xb52);
    if (*(int *)((char *)gpCurPlayer + 0xf) == 2)
        ta->field50 = (int)(ta->field50 * 1.4);
    ta->field50 = (int)((1.5 -
        abs(0x30 - gpGame->SetupPuzzlePieces(giCurPlayer, 1)) / 48.0f) * ta->field50);
    ta->field50 = (int)((ta->f14 + 0.66) * ta->field50);
    return (float)ta->field50;
}

VA(0x0043fe81, 0x51)
float philAI::FutureDeflator(int *const p) {
    float t = TurnsToBuy(p);
    float v = 1.0f - t * 0.1f;
    if (v < 0.0)
        v = 0;
    return v;
}

VA(0x0043fed2, 0xbf8)
int philAI::FightValueOfStack(class armyGroup *, class hero *, int, int, int, int) { return 0; }

VA(0x00440aca, 0x1e7)
void philAI::EvaluateOneTimeCreaturePurchase(int, int, int, int &, int &, int &) {}

VA(0x00440cb1, 0x768)
int philAI::QuickCombat(class armyGroup *, class hero *, class armyGroup *, class hero *, int, int, float &, float &) { return 0; }

VA(0x00441419, 0x422)
void philAI::HeroInteractionAtHero(class hero *, class hero *, int, int *) {}

VA(0x0044183b, 0x882)
void philAI::HeroInteractionAtTown(class hero *, class town *, int, int *) {}

VA(0x004420bd, 0x4ba)
void philAI::RedistributeTroops(class armyGroup *, class armyGroup *, int, int, int, int, int) {}

VA(0x00442577, 0x39)
int philAI::ChooseGoldOrExperience(int, int) {
    return ((pdView *)gpCurPlayer)->resources[6] > 4000 ? 1 : 0;
}

VA(0x004425b0, 0xc7)
void philAI::ChooseEvaluateBattle(armyGroup *ag1, hero *h1, armyGroup *ag2, hero *h2,
                                  int a, int b, int c, int &outFlag, int &outValue) {
    int val;            // score
    int p;              // unused
    int node, nb, kn, jb; // ProbableOutcomeOfBattle int& outputs
    float idx;          // fv
    int race;
    if (h2 != 0)
        race = *(signed char *)((char *)h2 + 0x3);
    else
        race = -1;
    ProbableOutcomeOfBattle(ag1, h1, ag2, h2, 0, a, b, race, idx, jb, kn, nb, node, val);
    val = (int)(c * idx + val);
    if (val <= 0) {
        outValue = 0;
        outFlag = 0;
    } else {
        outValue = val;
        outFlag = 1;
    }
}

VA(0x00442677, 0xfa)
int philAI::ChooseToFightForArtifact(int a, int b, int c) {
    int ra;             // i (-0x20)
    int py;             // ProbableOutcomeOfBattle out o4 (-0x1c)
    int p;              // out o3 (-0x18)
    int node;           // out o2 (-0x14)
    int nb;             // out o1 (-0x10)
    float kn;           // fv (-0xc)
    int jb;             // artifact RV (-0x8)
    int idx;            // result (-0x4)
    int o5;             // out (-0x24)
    jb = gArtifactBaseRV[a];
    for (ra = 0; ra < 5; ra++) {
        ((char *)gpMonGroup)[ra] = (char)b;
        if (ra == 0)
            ((short *)((char *)gpMonGroup + 0x5))[ra] = 1;
        else
            ((short *)((char *)gpMonGroup + 0x5))[ra] = 0;
    }
    ProbableOutcomeOfBattle((armyGroup *)((char *)gpCurAIHero + 0x65), gpCurAIHero, gpMonGroup,
                            0, 0, 0, 0, -1, kn, nb, node, p, py, o5);
    idx = (int)(jb * kn + o5);
    if (idx > 0)
        return 1;
    else
        return 0;
}

VA(0x00442771, 0x53)
int philAI::NetValueOfArtifact(int a1, int a2, int a3, int a4) {
    return (int)((float)gArtifactBaseRV[a1]
               - ((float)a4 * gafAITurnCostResource[a3] + (float)a2 * gafAITurnCostResource[RES_GOLD]));
}

VA(0x004427c4, 0x1d)
int philAI::ChooseToPayRansomOnHero(int) { return 1; }

VA(0x004427e1, 0xd7)
void philAI::BuildBuilding(town *t, int building) {
    int cost[7];
    int i;
    sprintf(gText, "Player %d built %s in town %d.\n", giCurPlayer,
            GetBuildingName(((townView *)t)->race, building), ((townView *)t)->id);
    LogStr(gText);
    if (giDebugLevel >= 3) {
        AiPrint(gText);
        DelayMilli(0x5dc);
    }
    GetBuildingCost(((townView *)t)->race, building, cost, ((townView *)t)->buildState);
    for (i = 0; i < 7; i++)
        ((pdView *)gpCurPlayer)->resources[i] -= cost[i];
    t->BuildBuilding(building);
    ShowStatus();
}

VA(0x004428b8, 0x328)
void philAI::BuildHero(class town *, int) {}

VA(0x00442be0, 0x2cd)
void philAI::BuildCreature(class town *, int, int) {}

VA(0x00442ead, 0x15a)
int philAI::CanBuyBHC(BHC &bhc) {
    int jb;             // dwelling monster type
    int idx;            // i
    int cost[7];
    switch (bhc.type) {
    case 0:
        if (CanBuy(bhc.pTown, bhc.what))
            return 1;
        break;
    case 1:
        if (((pdView *)gpCurPlayer)->resources[6] >= gHeroGoldCost &&
            ((townView *)bhc.pTown)->visitingHero == -1 &&
            bHeroBuiltThisTurn == 0)
            return 1;
        break;
    case 2:
        jb = gDwellingType[((townView *)bhc.pTown)->race][bhc.what];
        if (((townView *)bhc.pTown)->garrison[bhc.what] < bhc.num)
            return 0;
        GetMonsterCost(jb, cost);
        for (idx = 0; idx < 7; idx++)
            if (cost[idx] * bhc.num > ((pdView *)gpCurPlayer)->resources[idx])
                return 0;
        return 1;
    }
    return 0;
}

VA(0x00443007, 0x164)
int philAI::CombatMonsterEvent(hero *h, int monType, int *pCount, mapCell *cell) {
    int kn;             // i
    int jb;             // combat result
    float f2;
    float idx;          // f1
    int total;
    memset(gpMonGroup, -1, 5);
    memset((char *)gpMonGroup + 5, 0, 0xa);
    if (*pCount / 5 > 0) {
        for (kn = 0; kn < 5; kn++) {
            ((char *)gpMonGroup)[kn] = (char)monType;
            ((short *)((char *)gpMonGroup + 0x5))[kn] = (short)(*pCount / 5);
        }
    }
    for (kn = *pCount % 5 - 1; kn >= 0; kn--) {
        ((char *)gpMonGroup)[kn] = (char)monType;
        ((short *)((char *)gpMonGroup + 0x5))[kn]++;
    }
    jb = gpPhilAI->QuickCombat((armyGroup *)((char *)h + 0x65), h, gpMonGroup, 0, 0, 0, f2, idx);
    total = 0;
    for (kn = 0; kn < 5; kn++)
        total += ((short *)((char *)gpMonGroup + 0x5))[kn];
    *pCount = total;
    if (jb != 0)
        return 1;
    return 0;
}

VA(0x0044316b, 0x6d7)
int philAI::FightEvent(class hero *, class mapCell *, int) { return 0; }

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
void philAI::IncrementHourGlass(void) {
    int nb = ((pdView *)gpCurPlayer)->numHeroes;
    if (nb < 4 && ((pdView *)gpCurPlayer)->resources[6] >= 0x9c4 && bHeroBuiltThisTurn == 0)
        nb++;
    iCurHourGlassPhase++;
    if (nb == 1) {
        iCurHourGlassPhase++;
        iCurHourGlassPhase++;
    }
    if (nb == 2 && iCurHourGlassPhase != 1)
        iCurHourGlassPhase++;
    if (nb == 3 && (iCurHourGlassPhase == 3 || iCurHourGlassPhase == 6))
        iCurHourGlassPhase++;
    if (iCurHourGlassPhase > 9)
        iCurHourGlassPhase = 9;
}

VA(0x00443980, 0x227)
void philAI::TownEvent(class mapCell *, class hero *, int, int) {}

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
int philAI::ComputeValueOfSS(class hero *, int, int) { return 0; }

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
int philAI::ValueOfEventAtPosition(int, int, int, int *) { return 0; }

VA(0x00445a89, 0x299)
int philAI::EvaluateGenericSite(class mapCell *) { return 0; }

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

// @early-stop
// Complete & correct except the `switch` jump table: cl references it as a local ($L)
// label (jmpl disp 0); the delinker folds the in-.text table into the function symbol
// (jmpl disp 0xa9 + per-entry relocs to the function). Same target; delinker artifact.
VA(0x00445dde, 0xf5)
int philAI::EvaluateRecruitSite(mapCell *cell) {
    int val;            // value (returned)
    int node;           // color
    int nb;             // monster type
    int kn;             // out param
    int jb;             // unused
    int idx;            // out param
    short lvl;          // monster level
    node = ((mapCellView *)cell)->tentColor;
    node &= 7;
    lvl = ((mapCellView *)cell)->tentColor;
    lvl >>= 3;
    nb = 0;
    val = 0;
    switch (node) {
    case 0: nb = 0x3b; break;
    case 1: nb = 0x3e; break;
    case 2: nb = 0x3f; break;
    case 3: nb = 0x40; break;
    case 4: nb = 0x41; break;
    }
    EvaluateOneTimeCreaturePurchase(nb, lvl, 0, idx, val, kn);
    gbReduceByReload = 0;
    return val;
}

VA(0x00445ed3, 0x1d)
int philAI::EvaluateJail(mapCell *) {
    return 10000;
}

VA(0x00445ef0, 0xf6)
void InitAIMapVars(void) {
    CloseAIMapVars();
    SVSearchArray.Init();
    gaiLiveChanceOfPos = (short *)BaseAlloc(MAP_WIDTH * MAP_HEIGHT * 2, PHFILE, *(short *)"\x86\x1b" + 8);
    gaiHeroStrategicRVOfPos = (short *)BaseAlloc(MAP_WIDTH * MAP_HEIGHT * 2, PHFILE, *(short *)"\x86\x1b" + 9);
    gaiHeroEventStratRVOfPos = (short *)BaseAlloc(MAP_WIDTH * MAP_HEIGHT * 2, PHFILE, *(short *)"\x86\x1b" + 10);
    gaiTurnValueOfMine = (signed char *)BaseAlloc(MAP_WIDTH * MAP_HEIGHT, PHFILE, *(short *)"\x86\x1b" + 11);
    gaiEnemyHeroReachable = (signed char *)BaseAlloc(MAP_WIDTH * MAP_HEIGHT, PHFILE, *(short *)"\x86\x1b" + 12);
}

VA(0x00445fe6, 0x112)
void CloseAIMapVars(void) {
    if (gaiLiveChanceOfPos != 0)
        BaseFree(gaiLiveChanceOfPos, PHFILE, *(short *)"\x96\x1b" + 1);
    if (gaiHeroStrategicRVOfPos != 0)
        BaseFree(gaiHeroStrategicRVOfPos, PHFILE, *(short *)"\x96\x1b" + 2);
    if (gaiHeroEventStratRVOfPos != 0)
        BaseFree(gaiHeroEventStratRVOfPos, PHFILE, *(short *)"\x96\x1b" + 3);
    if (gaiTurnValueOfMine != 0)
        BaseFree(gaiTurnValueOfMine, PHFILE, *(short *)"\x96\x1b" + 4);
    if (gaiEnemyHeroReachable != 0)
        BaseFree(gaiEnemyHeroReachable, PHFILE, *(short *)"\x96\x1b" + 5);
    gaiLiveChanceOfPos = 0;
    gaiHeroStrategicRVOfPos = 0;
    gaiHeroEventStratRVOfPos = 0;
    gaiTurnValueOfMine = 0;
    gaiEnemyHeroReachable = 0;
    SVSearchArray.Close();
}

VA(0x004460f8, 0x26a)
int OnMySide(int) { return 0; }

VA(0x00446362, 0x2bc)
int philAI::EvaluateArtifactEvent(int, int) { return 0; }

VA(0x0044661e, 0x30a)
int philAI::EvaluateMineEvent(int, int, int, int *) { return 0; }

VA(0x00446928, 0x33e)
int philAI::EvaluateMonsterEvent(int, int, int *) { return 0; }

VA(0x00446c66, 0x5ab)
int philAI::EvaluateHeroEvent(int, int, int, int, int *) { return 0; }

VA(0x00447211, 0x4fa)
int philAI::EvaluateTownEvent(int, int, int, int, int *) { return 0; }

// ---- globals (definitions, RVA order) ----
DATA(0x004f20e0) float fFirstWeekTownFV;
DATA(0x004f20e4) int iVepCacheHits;
DATA(0x004f20e8) int iTotalVepHits;
DATA(0x004f20ec) int giShowComputerRoute;
DATA(0x004f20f0) short *gaiLiveChanceOfPos;
DATA(0x004f20f4) short *gaiHeroStrategicRVOfPos;
DATA(0x004f20f8) short *gaiHeroEventStratRVOfPos;
DATA(0x004f20fc) signed char *gaiTurnValueOfMine;
DATA(0x004f2100) signed char *gaiEnemyHeroReachable;
DATA(0x004f2104) long glLastStartTick;
DATA(0x004f2108) long glCurTicks;
DATA(0x004f210c) long glTotalTicks;
DATA(0x004f2110) hero *gpCurAIHero;
DATA(0x004f2114) float gfAttackHumanBonus;
DATA(0x004f2118) float gfAttackComputerBonus;
DATA(0x004f211c) int iLastFrameRateTimer;
DATA(0x004f22bc) int bSVSearchArrayInUse;
DATA(0x004f2340) int bEvaluatingTravelGates;
DATA(0x00525620) int gbReduceByBerserk;
DATA(0x00525624) float fBerserkFactor;
DATA(0x00525628) int giCurPlayer;
DATA(0x0052562c) signed char giBuildShipyard[6];
DATA(0x00525634) int giMaxHeroesForThisPlayer;
DATA(0x00525638) signed char giBuildBoat[6];
DATA(0x00525640) float fReduceFactor;
DATA(0x00525644) unsigned char giCurPlayerBit;
DATA(0x00525648) int giBestShipyardDist;
DATA(0x0052564c) int bHeroBuiltThisTurn;
DATA(0x00525650) short *gaiHeroLiveChance;
DATA(0x005256bc) int giHumanTownConquered;
DATA(0x005256c0) int giCurTurn;
DATA(0x005256c8) int *costTemp;
DATA(0x005256e4) int iAlphaMale;
DATA(0x005256e8) int iDummy;
DATA(0x005256ec) int gbPossibleShipyardFound;
DATA(0x005256f0) searchArray SVSearchArray;
DATA(0x00527c08) float gafAITurnCostResource[7];
DATA(0x00527c24) int iCurPlaceToVisit;
DATA(0x00527c28) int giBestShipyardId;
DATA(0x00527c2c) int gbActualBoatFound;
DATA(0x00527c30) unsigned char giCurWatchPlayerBit;
DATA(0x00527c34) playerData *gpCurPlayer;
DATA(0x00527c38) float *gfHeroInteractionBonus;
DATA(0x00527d10) int gbBerserk;
DATA(0x00527d14) int giCurAIHeroMorale;
DATA(0x00527d18) signed char giBuildBoatStuffTurn[6];
DATA(0x00527d20) int iPlacesVisited[30][2];
DATA(0x00527e10) int gbReduceByReload;
DATA(0x00527e14) int gbTroopReload;
DATA(0x00527e18) int giCurAIHeroLuck;
DATA(0x00527e1c) int gbActualShipyardFound;
