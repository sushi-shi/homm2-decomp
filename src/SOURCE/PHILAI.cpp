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
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <SOURCE/philAI.h>
#include <SOURCE/hero.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/advManager.h>
#include <SOURCE/town.h>
#include <SOURCE/game.h>
#include <SOURCE/GAME.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/CURSOR.h>
#include <EDITOR/mapcell.h>
#include <BASE/mouseManager.h>

// Globals -> _globals.h; tag_tilePoint/monsterRV -> _types.h; called free functions
// from owner headers (Misc.h/KB.h/kbwin.h/NOOPT.h); PHILAI's own -> SOURCE/PHILAI.h.

// __FILE__ for the NWC BaseAlloc/BaseFree memory tracking (reloc-masked path string).
#define PHFILE ((char *)"I:\\Projects\\Heroes\\Prog\\SOURCE\\PHILAI.CPP")

// AI view/record structs (BHC/pdView/.../gameView) -> SOURCE/PHILAI.h.

// @early-stop
// Bytes 0x000-0x1b5 are instruction-identical with relocations masked, and all 17
// relocation targets agree. Retail's three NOPs begin after the CodeView function range.
VA(0x0043781b, 0x1b5)
void ResetHeroRVs(int resetAll, int x, int y) {
    int idx;
    int node;

    if (x != -1) {
        if (y == -1)
            return;
        for (node = 0; node < MAP_WIDTH; node++) {
            for (idx = 0; idx < MAP_HEIGHT; idx++) {
                if (resetAll != 0) {
                    if (abs(x - node) + abs(y - idx) < AI_NEARBY_RADIUS)
                        *reinterpret_cast<short *>(reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
                            node * 2 + idx * MAP_WIDTH * 2) = AI_RV_UNSET;
                } else {
                    *reinterpret_cast<short *>(reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
                        node * 2 + idx * MAP_WIDTH * 2) = AI_RV_UNSET;
                    *reinterpret_cast<short *>(reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
                        node * 2 + idx * MAP_WIDTH * 2) = AI_RV_UNSET;
                }
            }
        }
        *reinterpret_cast<short *>(reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
            x * 2 + (MAP_WIDTH | 0) * y * 2) = AI_RV_UNSET;
        for (node = 0; node < AI_HERO_COUNT; node++) {
            if (resetAll == 0 ||
                abs(y - gpGame->m_heroRecs[node].m_x) +
                    abs(x - gpGame->m_heroRecs[node].m_x) < AI_NEARBY_RADIUS)
                gaiHeroLiveChance[node] = AI_RV_UNSET;
        }
    }
}

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
    for (i = 0; i < reinterpret_cast<pdView *>(gpCurPlayer)->numCastles; i++) {
        townView *pTown = reinterpret_cast<townView *>(reinterpret_cast<char *>(gpGame) +
            reinterpret_cast<pdView *>(gpCurPlayer)->getCastleId(i) * 100 + 0xb53);
        if (pTown->visitingHero != -1)
            HeroInteractionAtTown(
                reinterpret_cast<hero *>(reinterpret_cast<char *>(gpGame) +
                    pTown->getVisitingHero() * 250 + 0x27c4),
                reinterpret_cast<town *>(pTown), 0, &iDummy);
    }
}

// @early-stop
// reloc-masked asm is instruction-identical; the sole differing relocation is retail's
// delinked string symbol at the address of gDwellingType versus our typed array symbol.
VA(0x00437c61, 0x37e)
void philAI::CheckForCreatureUpgrades(void) {
    int upgradeType = -1;
    int node;
    int armyIndex;
    int creatureIndex;
    int dwelling;
    int canUpgrade;
    townView *townPtr;
    armyView *armyPtr;
    int goldCost;
    int resourceType;
    int resourceCost;
    int mergeIndex;

    for (node = 0; node < reinterpret_cast<pdView *>(gpCurPlayer)->numCastles; node++) {
        townPtr = reinterpret_cast<townView *>(
            reinterpret_cast<char *>(gpGame) +
            reinterpret_cast<pdView *>(gpCurPlayer)->getCastleId(node) * 100 + 0xb53);
        for (armyIndex = 0; armyIndex < 2; armyIndex++) {
            if (armyIndex == 0)
                armyPtr = reinterpret_cast<armyView *>(reinterpret_cast<char *>(townPtr) + 8);
            else {
                if (townPtr->visitingHero == -1)
                    continue;
                else
                    armyPtr = reinterpret_cast<armyView *>(
                        reinterpret_cast<char *>(gpGame) + townPtr->visitingHero * 250 + 0x2829);
            }
            for (creatureIndex = 0; creatureIndex < 5; creatureIndex++) {
                if (armyPtr->creatureTypes[creatureIndex] == -1)
                    continue;
                canUpgrade = 0;
                for (dwelling = 20; dwelling <= 24; dwelling++) {
                    if (gDwellingType[townPtr->race][dwelling] ==
                            armyPtr->creatureTypes[creatureIndex] &&
                        (townPtr->buildings & (1 << (dwelling + 5)))) {
                        canUpgrade = 1;
                        upgradeType = armyPtr->creatureTypes[creatureIndex] + 1;
                    }
                }
                if ((armyPtr->creatureTypes[creatureIndex] == 35 ||
                     armyPtr->creatureTypes[creatureIndex] == 36) &&
                    (townPtr->buildings & 0x40000000)) {
                    canUpgrade = 1;
                    upgradeType = 37;
                }
                if (canUpgrade) {
                    goldCost = (gMonsterDatabase[upgradeType].cost -
                                gMonsterDatabase[armyPtr->creatureTypes[creatureIndex]].cost) *
                               armyPtr->quantities[creatureIndex] * 2;
                    if (upgradeType == 37) {
                        resourceType = RES_SULFUR;
                        resourceCost = armyPtr->quantities[creatureIndex] * 2;
                    } else if (upgradeType == 46) {
                        resourceType = RES_GEMS;
                        resourceCost = armyPtr->quantities[creatureIndex] * 2;
                    } else {
                        resourceType = -1;
                        resourceCost = 0;
                    }
                    if (goldCost <= reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] &&
                        (resourceType == -1 ||
                         resourceCost <= reinterpret_cast<pdView *>(gpCurPlayer)->resources[resourceType])) {
                        reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] -= goldCost;
                        if (resourceType != -1)
                            reinterpret_cast<pdView *>(gpCurPlayer)->resources[resourceType] -= resourceCost;
                        armyPtr->creatureTypes[creatureIndex] = static_cast<signed char>(upgradeType);
                        for (mergeIndex = 0; mergeIndex < 5; mergeIndex++) {
                            if (mergeIndex != creatureIndex &&
                                armyPtr->creatureTypes[mergeIndex] ==
                                    armyPtr->creatureTypes[creatureIndex] &&
                                armyPtr->quantities[mergeIndex] > 0) {
                                armyPtr->quantities[mergeIndex] += armyPtr->quantities[creatureIndex];
                                armyPtr->quantities[creatureIndex] = 0;
                                armyPtr->creatureTypes[creatureIndex] = -1;
                            }
                        }
                    }
                }
            }
        }
    }
}

inline townSlot *GetCastleSlot(int id) {
    return reinterpret_cast<townSlot *>(&gpGame->m_castleRecs[id]);
}

// @early-stop
// Instruction stream is identical with relocations masked and all 80 relocation
// targets agree; residual rows are delinker names for the two log strings.
VA(0x00437fdf, 0x4be)
void philAI::CheckBuyStuff(void) {
    int done = 0;
    int jb = 0;
    BHC best;
    townView *idx;

    gpGame->CheckHeroConsistency();
    if (reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] >= 200) {
        if (reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] < 2500 &&
            reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes == 0)
            return;
        LogInt("CheckBuy Start", reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD],
               -999, -999, -999, -999, -999, -999);
        idx = 0;
        if (giBuildShipyard[giCurPlayer] >= 0) {
            idx = reinterpret_cast<townView *>(reinterpret_cast<char *>(gpGame) +
                giBuildShipyard[giCurPlayer] * 100 + 0xb53);
        } else if (giBuildBoat[giCurPlayer] >= 0) {
            idx = reinterpret_cast<townView *>(reinterpret_cast<char *>(gpGame) +
                giBuildBoat[giCurPlayer] * 100 + 0xb53);
        }
        if (giBuildShipyard[giCurPlayer] >= 0) {
            idx = reinterpret_cast<townView *>(GetCastleSlot(giBuildShipyard[giCurPlayer]));
        } else if (giBuildBoat[giCurPlayer] >= 0) {
            idx = reinterpret_cast<townView *>(GetCastleSlot(giBuildBoat[giCurPlayer]));
        }
        if (idx != 0 && idx->owner != giCurPlayer) {
            giBuildShipyard[giCurPlayer] = -1;
            giBuildBoat[giCurPlayer] = giBuildShipyard[giCurPlayer];
            idx = 0;
        }
        if (giBuildShipyard[giCurPlayer] >= 0) {
            if (CanBuy(reinterpret_cast<town *>(idx), 3) &&
                CanBuild(reinterpret_cast<town *>(idx), 3)) {
                BuildBuilding(reinterpret_cast<town *>(idx), 3);
                giBuildShipyard[giCurPlayer] = -1;
            } else {
                reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] -= 2000;
                reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_WOOD] -= 20;
            }
        }
        if (giBuildBoat[giCurPlayer] >= 0) {
            if ((idx->buildings & 8) &&
                reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] >= 1000 &&
                reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_WOOD] >= 10) {
                if (gpGame->GetBoatsBuilt() < 48 &&
                    gpAdvManager->GetCell(idx->dockX, idx->dockY)->triggerType == 0 &&
                    gpGame->CreateBoat(idx->dockX, idx->dockY, 0) != -1) {
                    reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] -= 1000;
                    reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_WOOD] -= 10;
                }
                giBuildBoat[giCurPlayer] = -1;
            } else {
                reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] -= 1000;
                reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_WOOD] -= 10;
            }
        }
        CheckForCreatureUpgrades();
        DoAllHeroInteractions();
        while (!done) {
            GetBestBHC(giCurPlayer, best);
            if (best.type >= 0 && CanBuyBHC(best)) {
                switch (best.type) {
                case AI_PURCHASE_BUILDING:
                    BuildBuilding(best.pTown, best.what);
                    break;
                case AI_PURCHASE_HERO:
                    BuildHero(best.pTown, best.what);
                    break;
                case AI_PURCHASE_CREATURE:
                    BuildCreature(best.pTown, best.what, best.num);
                    break;
                }
                jb = 1;
            } else
                done = 1;
        }
        if (giBuildShipyard[giCurPlayer] >= 0) {
            reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] += 2000;
            reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_WOOD] += 20;
        }
        if (giBuildBoat[giCurPlayer] >= 0) {
            reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] += 1000;
            reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_WOOD] += 10;
        }
        DoAllHeroInteractions();
        LogInt("CheckBuy End  ", reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD],
               -999, -999, -999, -999, -999, -999);
    }
}

inline hero *GetHeroSlot(int id) {
    return &gpGame->m_heroRecs[id];
}

// @early-stop
// At the retained source-hash max, raw disassembly differs only at the two
// owned-current-turn exits: retail uses direct JE branches while /Od emits
// equivalent JNE + continuation JMP blocks. All frame roles agree; 42/42
// relocations agree after the delinker's normalDirTable+2 string-symbol alias.
VA(0x0043849d, 0x2e8)
int philAI::GoodAdjacent(int *direction) {
    int ra;
    float py;
    int p;
    int val;
    int bestValue;
    float node;
    int nb;
    int kn;
    int jb;
    int idx;
    int heroId;

    idx = -1;
    py = fReduceFactor;
    node = fBerserkFactor;
    fReduceFactor = 1.0f;
    fBerserkFactor = 1.0f;
    bestValue = 100;
    if ((gpAdvManager->GetCell(gpCurAIHero->m_x,
                               gpCurAIHero->m_y)->triggerType & 0x7f) == AI_OBJECT_MONSTER)
        return 0;
    for (jb = 0; jb < 8; jb++) {
        if (gpAdvManager->ValidMoveWithEvent(gpCurAIHero, jb)) {
            kn = normalDirTable[jb].x + gpCurAIHero->m_x;
            nb = normalDirTable[jb].y + gpCurAIHero->m_y;
            if ((gpAdvManager->GetCell(kn, nb)->triggerType & 0x80) &&
                !(mapExtra[kn + (MAP_WIDTH | 0) * nb] & 0x80) &&
                ((gpAdvManager->GetCell(kn, nb)->triggerType & 0x7f) != AI_OBJECT_MONSTER) &&
                ((gpAdvManager->GetCell(kn, nb)->triggerType & 0x7f) != AI_OBJECT_ARTIFACT)) {
                if (gpAdvManager->GetCell(kn, nb)->triggerType == AI_OBJECT_TOWN) {
                    ra = gpAdvManager->GetCell(kn, nb)->w4hi;
                    if (reinterpret_cast<townView *>(GetCastleSlot(ra))->owner == giCurPlayer)
                        if (gpCurAIHero->m_lastTownInteractionTurn == giCurTurn)
                            continue;
                }
                if (gpAdvManager->GetCell(kn, nb)->triggerType == AI_OBJECT_HERO) {
                    heroId = gpAdvManager->GetCell(kn, nb)->w4hi;
                    if (GetHeroSlot(heroId)->m_owner == giCurPlayer)
                        if (gpCurAIHero->m_lastHeroInteractionTurn == giCurTurn)
                            continue;
                }
                val = ValueOfEventAtPosition(kn, nb, 2, &p);
                if (p > 80)
                    if (bestValue < val) {
                        bestValue = val;
                        idx = jb;
                    }
            }
        }
    }
    fReduceFactor = py;
    fBerserkFactor = node;
    if (idx != -1) {
        *direction = idx;
        return 1;
    }
    return 0;
}

VA(0x00438785, 0x4b8)
void philAI::CheckReload(void) {
    int p;
    int node;
    mapCell *nb;
    int kn;
    int jb;
    float idx;
    float friendly;

    gbTroopReload = 0;
    fReduceFactor = 1.0f;
    friendly = 0.0f;
    idx = 0.0f;
    kn = FightValueOfStack(&gpCurAIHero->m_army, gpCurAIHero, 0, 0, 0, 0);
    if (kn < AI_MINIMUM_FIGHT_VALUE)
        kn = AI_MINIMUM_FIGHT_VALUE;
    gpSearchArray->SeedPosition(gpCurAIHero->m_x, gpCurAIHero->m_y,
        gpCurAIHero->m_direction, gpCurAIHero->m_mobility << 2,
        gpCurAIHero->m_eventFlags & 0x80, 0, gpCurAIHero->m_remainingMobility,
        gpCurAIHero->m_secondarySkills[0], -1, -1, 0, 0);
    for (node = 0; node < MAP_WIDTH; node++) {
        for (p = 0; p < MAP_HEIGHT; p++) {
            if (gpSearchArray->GetRow(MAP_WIDTH, p)[node].visited) {
                nb = gpAdvManager->GetCell(node, p);
                switch (nb->triggerType) {
                case AI_OBJECT_TOWN:
                    jb = FightValueOfStack(
                    reinterpret_cast<armyGroup *>(
                        reinterpret_cast<char *>(GetCastleSlot(nb->w4hi)) + 8),
                    0, 0, 0, 0, 0);
                    if (gpGame->m_castleOwners[nb->w4hi] == gpCurAIHero->m_owner) {
                        if (kn * 2 < jb) {
                            friendly += (gpCurAIHero->m_mobility + 10) *
                                (static_cast<float>(jb) / (kn * 2) - 1.0f) /
                                (gpSearchArray->GetRow(MAP_WIDTH, p)[node].distance + 10);
                        }
                    } else if ((kn >> 1) < jb) {
                        idx += (gpCurAIHero->m_mobility + 30) *
                            (static_cast<float>(jb) / (kn >> 1) - 1.0f) /
                            (gpSearchArray->GetRow(MAP_WIDTH, p)[node].distance + 30);
                    }
                    break;
                case AI_OBJECT_HERO:
                    if (gpGame->m_availableHeroes[nb->w4hi] != gpCurAIHero->m_owner) {
                        jb = FightValueOfStack(&GetHeroSlot(nb->w4hi)->m_army,
                                               0, 0, 0, 0, 0);
                        if ((kn >> 1) < jb) {
                            idx += (gpCurAIHero->m_mobility + 30) *
                                (static_cast<float>(jb) / (kn >> 1) - 1.0f) /
                                (gpSearchArray->GetRow(MAP_WIDTH, p)[node].distance + 30);
                        }
                    }
                }
            }
        }
    }
    if (friendly > 1.0f && idx > 1.0f) {
        fReduceFactor = AI_RELOAD_NUMERATOR /
                        (friendly + idx + AI_RELOAD_BASE);
        gbTroopReload = 1;
    }
}

// @early-stop
// Objdiff reports 100%; the only disassembly rows are delinker names for
// compiler-emitted floating constants.
VA(0x00438c3d, 0x302)
void philAI::CheckBerserk(void) {
    int row17;
    int column1;
    mapCell *kn;
    int jb;
    int idx;
    int best = -1;
    hero *heroPtr;

    gbBerserk = 0;
    fBerserkFactor = 1.0f;
    jb = FightValueOfStack(&gpCurAIHero->m_army, gpCurAIHero, 1, 0, 0, 0);
    if (reinterpret_cast<pdView *>(gpCurPlayer)->difficulty == 0)
        jb = static_cast<int>(jb * AI_EASY_STRENGTH_FACTOR);
    if (jb < AI_MINIMUM_FIGHT_VALUE)
        jb = AI_MINIMUM_FIGHT_VALUE;
    if (jb < AI_BERSERK_THRESHOLD)
        return;
    {
        for (column1 = 0; column1 < MAP_WIDTH; column1++) {
            for (row17 = 0; row17 < MAP_HEIGHT; row17++) {
                kn = gpAdvManager->GetCell(column1, row17);
                switch (kn->triggerType) {
                case AI_OBJECT_TOWN:
                    if (gpGame->m_castleOwners[kn->w4hi] != gpCurAIHero->m_owner) {
                        if (gpGame->m_castleOwners[kn->w4hi] != -1) {
                        idx = FightValueOfStack(
                            reinterpret_cast<armyGroup *>(
                                reinterpret_cast<char *>(GetCastleSlot(kn->w4hi)) + 8),
                            0, 1, 1, kn->w4hi, 0);
                        if (idx > jb)
                            return;
                        if (idx > best)
                            best = idx;
                        }
                    }
                    break;
                case AI_OBJECT_HERO:
                    if (gpGame->m_availableHeroes[kn->w4hi] != gpCurAIHero->m_owner) {
                    heroPtr = GetHeroSlot(kn->w4hi);
                    idx = FightValueOfStack(&heroPtr->m_army, 0, 1,
                        heroPtr->m_locationType == AI_OBJECT_TOWN,
                        heroPtr->m_occupiedTown, 0);
                    if (idx * 2 > jb)
                        return;
                    if (idx * 2 > best)
                        best = idx * 2;
                    }
                    break;
                }
            }
        }
        if (best <= 0)
            return;
        fBerserkFactor = best * AI_BERSERK_FACTOR / jb;
        gbBerserk = 1;
    }
}

VA(0x00438f3f, 0x71)
void philAI::DimensionDoorTo(int x, int y) {
    gpAdvManager->TeleportTo(gpCurAIHero, x, y, 0, 0);
    if (*(int *)((char *)gpCurAIHero + 0x35) < 0xe1)
        *(int *)((char *)gpCurAIHero + 0x35) = 0;
    else
        *(int *)((char *)gpCurAIHero + 0x35) -= 0xe1;
    gpCurAIHero->UseSpell(0x38);
}

// @early-stop
// Instruction stream is identical with relocations masked and all 34 relocation
// targets agree; objdiff's residual is delinker-local identity only.
VA(0x00438fb0, 0x3f9)
int philAI::DoAnywhereDDoorTownGate(int targetValue) {
    int bestY;
    int candidateY;
    int bestX;
    int y;
    int candidateX;
    int adjacentIndex;
    int x;
    int monsterY;
    int monsterX;
    int value;
    int bestValue;
    mapCell *cell;

    if (gpCurAIHero->m_eventFlags & 0x80)
        return 0;
    {
        bestX = -1;
        if (targetValue > 25)
            bestValue = targetValue + 1000;
        else
            bestValue = Random(0, 125) - 50;
        if (gpCurAIHero->HasSpell(AI_SPELL_DIMENSION_DOOR)) {
          if (gpCurAIHero->m_spellPoints >= AI_DIMENSION_DOOR_SPELL_POINTS) {
            for (x = 0; MAP_WIDTH > x; x++) {
                for (y = 0; y < MAP_HEIGHT; y++) {
                    if (gpSearchArray->GetRow(y, MAP_WIDTH)[x].visited)
                        continue;
                    if (abs(x - gpCurAIHero->m_x) <= 7) {
                    if (abs(y - gpCurAIHero->m_y) > 7)
                        continue;
                    if (gpCurAIHero->m_x == x && gpCurAIHero->m_y == y)
                        continue;
                    cell = gpAdvManager->GetCell(x, y);
                    if (giGroundToTerrain[cell->tile] == 0)
                        continue;
                    if (!((cell->triggerType & 0x80) ||
                          (targetValue < 25 && Random(0, 10) < 2))) {
                    } else {
                    value = RVOfPosition(x, y, 0, -1, -1, 0, -1, -1, 2,
                                         gpCurAIHero->m_remainingMobility);
                    value += Random(0, 30);
                    if (value > bestValue) {
                        for (adjacentIndex = 0; adjacentIndex < 3; adjacentIndex++) {
                            if (adjacentIndex == 0) {
                                candidateX = x - 1;
                                candidateY = y;
                            } else if (adjacentIndex == 1) {
                                candidateX = x;
                                candidateY = y + 1;
                            } else {
                                candidateX = x + 1;
                                candidateY = y;
                            }
                            if (gpSearchArray->GetRow(candidateY, MAP_WIDTH)[candidateX].visited)
                                continue;
                            if (gpCurAIHero->m_x == candidateX &&
                                gpCurAIHero->m_y == candidateY)
                                continue;
                            if (candidateX < 0 || candidateX > MAP_WIDTH - 1 ||
                                candidateY < 0 || candidateY > MAP_HEIGHT - 1)
                                continue;
                            if (gpAdvManager->FindAdjacentMonster(candidateX, candidateY,
                                    &monsterX, &monsterY, -1, -1))
                                continue;
                            cell = gpAdvManager->GetCell(candidateX, candidateY);
                            if (giGroundToTerrain[cell->tile] == 0)
                                continue;
                            if (cell->field8 & 8)
                                continue;
                            if (cell->triggerType & 0x80)
                                continue;
                            if (cell->objIndex != 0xff && !(cell->field8 & 0x80))
                                continue;
                            bestX = candidateX;
                            bestY = candidateY;
                            bestValue = value;
                        }
                    }
                    }
                    }
                }
            }
            if (bestX != -1) {
                DimensionDoorTo(bestX, bestY);
                return 1;
            }
          }
        }
    }
    return 0;
}

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

// @early-stop
// Raw disassembly has identical logic and frame roles. Residual rows are one
// TU-cumulative comparison load order, leading/trailing /Ob1 continuations, and
// equivalent conditional/tail-thunk encodings at reconstructed loop/exit edges;
// all 221 relocation targets agree, with remaining names delinker artifacts.
VA(0x00439631, 0xcad)
void philAI::DoAI(int player) {
    int hiddenPointers7 = 0;
    int hourglassShown4 = 0;
    int boughtAfterCapture5 = 0;
    int heroDone5;
    int stepLimit36;
    int minimumValue9;
    hero* currentHero15;
    int pathIndex3[2];
    int steps14;
    int stopAfterStep0[2];
    mapCell* eventCell9;
    int eventX18;
    int eventY16;
    int moveFlagA3[9];
    int moveFlagB12;
    townView* townPtr9;
    int targetValue11;
    int townId3;
    unsigned int direction26;
    unsigned int specialDirection6;
    int adjacentDirection3;
    int savedShow10;

    LogInt("DO AI 1", player, -999, -999, -999, -999, -999, -999);
    PollSound();
    if (gbGameOver != 0)
        goto aiCleanup;
    if (giLimitPlayer != 0) {
        if (giLimitPlayer != player)
            goto aiCleanup;
    }
            LogInt("DO AI", player, -999, -999, -999, -999, -999, -999);
            InitAIMapVars();
            GetTurnAIVars(player);
            if (gpGame->m_day == 1 || gpGame->m_day == 1) {
                for (pathIndex3[0] = 0;
                     pathIndex3[0] < reinterpret_cast<pdView*>(gpCurPlayer)->numHeroes;
                     pathIndex3[0]++) {
                    currentHero15 = GetHeroSlot(
                        reinterpret_cast<pdView*>(gpCurPlayer)->heroIds[pathIndex3[0]]);
                    currentHero15->m_lastTownInteractionTurn = -99;
                }
            }
            ShowStatus();
            SetupRelativeHeroStrengths();
            for (pathIndex3[0] = 0; pathIndex3[0] < 7; pathIndex3[0]++) {
                sprintf(
                    gText,
                    "RES - %15s  %d  %d",
                    gResourceNames[pathIndex3[0]],
                    reinterpret_cast<pdView*>(gpCurPlayer)->resources[pathIndex3[0]],
                    reinterpret_cast<pdView*>(gpCurPlayer)->income[pathIndex3[0]]
                );
                LogStr(gText);
            }
            CheckBuyStuff();
            IncrementHourGlass();

            while ((currentHero15 = DetermineHeroToMove(player)) != 0) {
            ValidateHero(currentHero15);
            gpCurAIHero = currentHero15;
            if (gpCurAIHero->m_boatId != 0xff && gpCurAIHero->m_unknown2b == 0) {
                gpCurAIHero->m_remainingMobility = 0;
                continue;
            }

            giCurAIHeroMorale = gpCurAIHero->m_army.GetMorale(gpCurAIHero, 0, 0);
                giCurAIHeroLuck = gpGame->GetLuck(gpCurAIHero, 0, 0);
                gpCurAIHero->m_army.GetMorale(gpCurAIHero, 0, 0);
                giHumanTownConquered = -1;
                iCurPlaceToVisit = 0;
                if (gbGameOver != 0)
                    goto aiCleanup;

                LogStr("\n\n\n\n");
                LogStr("===================================");
                LogInt("Player with HeroTOMOVE", player, -999, -999, -999, -999, -999, -999);
                LogStr(reinterpret_cast<char*>(gpCurAIHero) + 10);
                LogStr("\n");
                CheckReload();
                CheckBerserk();
                giShowComputerRoute = 0;
                if (const_00128d38 == 0 && gbRemoteOn == 0
                    && MapExtraPosAndAdjacentsSet(
                        gpCurAIHero->m_x,
                        gpCurAIHero->m_y,
                        giCurWatchPlayerBit
                    )) {
                    bShowIt = 1;
                    gpAdvManager->SetHeroContext(
                        static_cast<unsigned char>(gpCurAIHero->m_id), 0);
                } else {
                    bShowIt = 0;
                    gpAdvManager->SetHeroContext(
                        static_cast<unsigned char>(gpCurAIHero->m_id), 0);
                }

                heroDone5 = 0;
                ResetHeroRVs(0, 0, 0);
                stepLimit36 = (gpCurAIHero->m_eventFlags & 0x80) ? 15 : 5;
                minimumValue9 = gpCurAIHero->m_mobility + 800;
                stepLimit36 = static_cast<int>(stepLimit36 * (1.7 - gpGame->m_difficulty * 0.1));
                minimumValue9 =
                    static_cast<int>(minimumValue9 * ((gpGame->m_difficulty - 1) * 0.06 + 0.8));

                while (!heroDone5 && gpCurAIHero->m_remainingMobility >= 75) {
                    if (!(gbGameOver == 0)) {
                        break;
                    } else {
                    if (gpCurAIHero->m_remainingMobility == gpCurAIHero->m_mobility
                        && reinterpret_cast<pdView*>(gpCurPlayer)->routeLength > 15
                        && reinterpret_cast<pdView*>(gpCurPlayer)->routeX == gpCurAIHero->m_x
                        && reinterpret_cast<pdView*>(gpCurPlayer)->routeY
                               == gpCurAIHero->m_y) {
                        gpAdvManager->ProcessSearch(
                            gpCurAIHero->m_x,
                            gpCurAIHero->m_y
                        );
                    }

                    do {
                        do {
                            targetValue11 = DetermineTargetPosition(
                                gpCurAIHero->m_destinationX,
                                gpCurAIHero->m_destinationY,
                                minimumValue9,
                                reinterpret_cast<int&>(specialDirection6)
                            );
                            townId3 =
                                gpGame->GetTownId(gpCurAIHero->m_x, gpCurAIHero->m_y);
                            if (townId3 != -1) {
                                townPtr9 = reinterpret_cast<townView*>(GetCastleSlot(townId3));
                                if (townPtr9->threat != 0
                                    && (gpAdvManager
                                            ->GetCell(
                                                gpCurAIHero->m_destinationX,
                                                gpCurAIHero->m_destinationY
                                            )
                                            ->triggerType
                                        & 0x7f)
                                           != AI_OBJECT_BOAT) {
                                    gpCurAIHero->m_remainingMobility = 0;
                                }
                            }
                            if (specialDirection6 != 0xffffffff) {
                                direction26 = specialDirection6;
                                steps14 = 99;
                                goto aiMoveDirection;
                            }
                        } while (targetValue11 < 1000 && specialDirection6 == 0xffffffff &&
                                 DoAnywhereDDoorTownGate(targetValue11));

                        if (targetValue11 < 75 && gpGame->m_day == 7
                            && gpCurAIHero->m_locationType == AI_OBJECT_TOWN) {
                            gpCurAIHero->m_remainingMobility = 0;
                        }
                        for (pathIndex3[0] = 0;
                             pathIndex3[0] < iCurPlaceToVisit;
                             pathIndex3[0]++) {
                            if (iPlacesVisited[pathIndex3[0]][0] == gpCurAIHero->m_destinationX
                                && iPlacesVisited[pathIndex3[0]][1] == gpCurAIHero->m_destinationY
                                && gpAdvManager
                                           ->GetCell(
                                               gpCurAIHero->m_destinationX,
                                               gpCurAIHero->m_destinationY
                                           )
                                           ->triggerType
                                       != AI_OBJECT_TOWN) {
                                gpCurAIHero->m_remainingMobility = 0;
                            }
                        }
                        if (iCurPlaceToVisit < 30) {
                            iPlacesVisited[iCurPlaceToVisit][0] = gpCurAIHero->m_x;
                            iPlacesVisited[iCurPlaceToVisit][1] = gpCurAIHero->m_y;
                            iCurPlaceToVisit++;
                        }
                        giShowComputerRoute = 1;
                        if (gpCurAIHero->m_remainingMobility == gpCurAIHero->m_mobility) {
                            hourglassShown4 = 0;
                            IncrementHourGlass();
                        }
                        if (gpCurAIHero->m_destinationX == -1 || gpCurAIHero->m_destinationY == -1)
                            goto aiMovementExit;
                        eventCell9 = 0;
                        gpAdvManager->SetHeroContext(
                            static_cast<unsigned char>(gpCurAIHero->m_id), 0);
                        gpSearchArray->BuildPath(
                            gpCurAIHero->m_x,
                            gpCurAIHero->m_y,
                            gpCurAIHero->m_destinationX,
                            gpCurAIHero->m_destinationY,
                            gpCurAIHero->m_remainingMobility
                        );
                        if (gpSearchArray->m_pathLength == 0) {
                            heroDone5 = 1;
                            goto aiEventCheck;
                        }
                        if (gpSearchArray->m_pathLength <= 0)
                            goto aiMovementDone;
                        gpAdvManager->UpdateScreen(0, 0);
                    } while (targetValue11 > 1000 &&
                             gpCurAIHero->HasSpell(AI_SPELL_DIMENSION_DOOR) &&
                             gpCurAIHero->m_spellPoints >= AI_DIMENSION_DOOR_SPELL_POINTS &&
                             DoDimensionDoor(gpCurAIHero));

                    steps14 = 0;
                    pathIndex3[0] = gpSearchArray->m_pathLength - 1;
                    moveFlagA3[0] = 0;
                    moveFlagB12 = 0;
                    stopAfterStep0[0] = 1;
                    while (pathIndex3[0] >= 0 && steps14 < stepLimit36) {
                        stopAfterStep0[0] =
                            (steps14 + 1 == stepLimit36 || pathIndex3[0] == 0) ? 1 : 0;
                        if (pathIndex3[0] > 0) {
                            moveFlagA3[0] = 0;
                            moveFlagB12 = 0;
                            if (GoodAdjacent(&adjacentDirection3)) {
                                gpSearchArray->m_storage.aiPath.directions[pathIndex3[0]] =
                                    static_cast<signed char>(adjacentDirection3);
                                stopAfterStep0[0] = 1;
                            }
                        }
                        direction26 =
                            static_cast<unsigned char>(gpSearchArray->m_storage.aiPath.directions[pathIndex3[0]]);
aiMoveDirection:
                        if (gpAdvManager->GetMoveShowIt(gpCurAIHero, direction26)) {
                            savedShow10 = bShowIt;
                            bShowIt = 1;
                            gpMouseManager->HideColorPointer();
                            hiddenPointers7++;
                            bShowIt = savedShow10;
                        }
                        eventCell9 = gpAdvManager->MoveHero(
                            direction26,
                            stopAfterStep0[0],
                            &eventX18,
                            &eventY16,
                            &moveFlagA3[0],
                            1,
                            &moveFlagB12,
                            0
                        );
                        steps14++;
                        if (eventCell9 != 0 || moveFlagA3[0] != 0 || moveFlagB12 != 0) {
                            break;
                        }
                        pathIndex3[0]--;
                    }

                    if (gpCurAIHero->m_owner != giCurPlayer) {
                        if (!boughtAfterCapture5) {
                            boughtAfterCapture5 = 1;
                            CheckBuyStuff();
                        } else {
                        }
                        break;
                    }
                    if (gpCurAIHero->m_remainingMobility <= (gpCurAIHero->m_mobility >> 1)
                        && !hourglassShown4) {
                        hourglassShown4 = 1;
                        IncrementHourGlass();
                    }
                    if (pathIndex3[0] < 0
                        && reinterpret_cast<pdView*>(gpCurPlayer)->routeLength > 15
                        && reinterpret_cast<pdView*>(gpCurPlayer)->routeX == gpCurAIHero->m_x
                        && reinterpret_cast<pdView*>(gpCurPlayer)->routeY
                               == gpCurAIHero->m_y) {
                        if (gpCurAIHero->m_remainingMobility == gpCurAIHero->m_mobility) {
                            gpAdvManager->ProcessSearch(-1, -1);
                        } else {
                            gpCurAIHero->m_remainingMobility = 0;
                        }
                    }
                    if (pathIndex3[0] < 0
                        && (((gpCurAIHero->m_x != gpCurAIHero->m_destinationX
                              || gpCurAIHero->m_destinationY != gpCurAIHero->m_y)
                             && eventCell9 == 0)
                            || gpCurAIHero->m_remainingMobility < 75
                            || (moveFlagA3[0] != 0 && eventCell9 == 0))) {
                        heroDone5 = 1;
                    }

                    savedShow10 = bShowIt;
                    bShowIt = 1;
                    while (hiddenPointers7 != 0) {
                        gpMouseManager->ShowColorPointer();
                        hiddenPointers7--;
                    }
                    bShowIt = savedShow10;
                    gpAdvManager->UpdateRadar(1, 0);
                    goto aiEventCheck;
aiMovementDone:
                    heroDone5 = 1;
aiEventCheck:
                    if (eventCell9 != 0) {
                        gpAdvManager->DoAIEvent(eventCell9, gpCurAIHero, eventX18, eventY16);
                        if (reinterpret_cast<playerData*>(gpCurPlayer)->m_currentHero == -1) {
                            break;
                        }
                        ValidateHero(gpCurAIHero);
                        ResetHeroRVs(1, gpCurAIHero->m_destinationX, gpCurAIHero->m_destinationY);
                    }
                    goto aiMovementContinue;
aiMovementExit:
                    heroDone5 = 1;
aiMovementContinue:
                    ;
                    }
                }

                gpCurAIHero->m_remainingMobility = 0;
                gpAdvManager->DeactivateCurrHero();
                if (gpCurAIHero->m_locationType == AI_OBJECT_TOWN) {
                    CheckBuyStuff();
                }
    }

aiCleanup:
    gpGame->CheckHeroConsistency();
    gpCurAIHero = 0;
    CloseAIMapVars();
    while (gpMouseManager->m_hideCount != 0) {
        gpMouseManager->ShowColorPointer();
    }
}

VA(0x0043a2de, 0x4b)
void philAI::GetGameAIVars(void) {
    int i;
    for (i = 0; i < *(signed char *)((char *)gpGame + 0x48e); i++)
        GetGameAttentionValue(i);
}

// @early-stop
// At this source hash's retained max, raw disassembly differs only at +0xce7 and
// +0xda3: the same signed turn comparisons load the commutative operands in the
// opposite order. Frame slots agree and all 171 relocation targets agree.
VA(0x0043a329, 0xe2b)
void philAI::GetTurnAIVars(int player) {
    int ownedTownCount19;
    int firstWeekIndex0;
    int playerIndex0[2];
    int townId3;
    townView *townPtr9;
    int artifactTotal8;
    int outcomeE14;
    int generalIndex4;
    int y4;
    int outcomeD1;
    int innerIndex27;
    int x8;
    int xCenter19;
    int mineValue4;
    int yCenter3;
    pdView *playerPtr27;
    int lastFightValue8;
    float townCountValue5;
    hero *heroPtr5;
    float fightValueFloat15;
    int earlyTurn21;
    int lateTurn40;
    float winChance11;
    int outcomeA9;
    int outcomeB6;
    mapCell *cell12;
    int outcomeC4;

    giCurTurn = (gpGame->m_month - 1) * 28 + (gpGame->m_week - 1) * 7 + gpGame->m_day;
    GetTurnAttentionValue(player);
    TurnCostResource(player);
    iCurHourGlassPhase = 0;
    iSandAnim = 0;
    reinterpret_cast<pdView *>(gpCurPlayer)->obeliskValue =
        static_cast<int>(TurnValueOfObelisk(player));
    reinterpret_cast<pdView *>(gpCurPlayer)->totalObeliskValue =
        reinterpret_cast<pdView *>(gpCurPlayer)->obeliskValue * gpGame->m_obeliskCount / 48;
    reinterpret_cast<pdView *>(gpCurPlayer)->unexploredValue =
        MeanRVOfUnexploredTerritory(player);
    bHeroBuiltThisTurn = 0;
    if (giCurTurn - giBuildBoatStuffTurn[player] > 8) {
        giBuildShipyard[player] = -1;
        giBuildBoat[player] = -1;
    }

    fightValueFloat15 = 0.0f;
    lastFightValue8 = 0;
    if (giCurTurn <= 8) {
        fFirstWeekTownFV = 0.0f;
        for (firstWeekIndex0 = 0;
             firstWeekIndex0 < reinterpret_cast<pdView *>(gpCurPlayer)->numCastles;
             firstWeekIndex0++) {
            townPtr9 = reinterpret_cast<townView *>(GetCastleSlot(
                reinterpret_cast<pdView *>(gpCurPlayer)->castleIds[firstWeekIndex0]));
            for (innerIndex27 = 0; innerIndex27 < AI_HERO_COUNT; innerIndex27++) {
                heroPtr5 = &gpGame->m_heroRecs[innerIndex27];
                if (heroPtr5->m_owner >= 0 && heroPtr5->m_owner <= 6)
                    if (heroPtr5->m_owner == giCurPlayer) {
                    } else
                        if (abs(heroPtr5->m_y - townPtr9->castleY) +
                                abs(heroPtr5->m_x - townPtr9->castleX) < 16) {
                            fFirstWeekTownFV = 0.3f;
                            goto firstWeekDone;
                        }
            }
        }
    }

firstWeekDone:
    lastFightValue8 = 0;
    for (generalIndex4 = 0; generalIndex4 < reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes; generalIndex4++) {
        heroPtr5 = GetHeroSlot(
            reinterpret_cast<pdView *>(gpCurPlayer)->heroIds[generalIndex4]);
        fightValueFloat15 = static_cast<float>(
            FightValueOfStack(&heroPtr5->m_army, heroPtr5, 0, 0, 0, 0));
        lastFightValue8 = static_cast<int>(lastFightValue8 + fightValueFloat15);
        heroPtr5->m_aiFightValue = static_cast<float>(fightValueFloat15 * AI_HERO_VALUE_SCALE +
                                                     AI_HERO_VALUE_BASE);
    }
    for (generalIndex4 = 0; generalIndex4 < reinterpret_cast<pdView *>(gpCurPlayer)->numCastles; generalIndex4++) {
        townPtr9 = reinterpret_cast<townView *>(GetCastleSlot(
            reinterpret_cast<pdView *>(gpCurPlayer)->castleIds[generalIndex4]));
        fightValueFloat15 = static_cast<float>(FightValueOfStack(
            reinterpret_cast<armyGroup *>(reinterpret_cast<char *>(townPtr9) + 8),
            0, 0, 0, 0, 0));
        lastFightValue8 = static_cast<int>(lastFightValue8 + fightValueFloat15);
    }
    reinterpret_cast<pdView *>(gpCurPlayer)->upgradeFactor =
        static_cast<float>(reinterpret_cast<pdView *>(gpCurPlayer)->income[RES_GOLD] +
                           reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD]) /
            (lastFightValue8 + 1000) +
        reinterpret_cast<pdView *>(gpCurPlayer)->baseUpgradeFactor;

    artifactTotal8 = 0;
    for (generalIndex4 = 4; generalIndex4 < 37; generalIndex4++)
        artifactTotal8 += gArtifactBaseRV[generalIndex4];
    for (generalIndex4 = 0; generalIndex4 < gpGame->m_playerCount; generalIndex4++) {
        reinterpret_cast<pdView *>(&gpGame->m_players[generalIndex4])->artifactPoolShare =
            1.0 / (gpGame->m_playerCount + gpGame->m_deadPlayerCount);
    }
    reinterpret_cast<pdView *>(gpCurPlayer)->artifactValue =
        static_cast<float>(artifactTotal8 / AI_ARTIFACT_POOL_DIVISOR);

    for (generalIndex4 = 0; generalIndex4 < 72; generalIndex4++)
        reinterpret_cast<townView *>(&gpGame->m_castleRecs[generalIndex4])->threat = 0;
    for (generalIndex4 = 0; generalIndex4 < reinterpret_cast<pdView *>(gpCurPlayer)->numCastles; generalIndex4++)
        reinterpret_cast<townView *>(
            &gpGame->m_castleRecs[reinterpret_cast<pdView *>(gpCurPlayer)->castleIds[generalIndex4]])->threat = 0;

    memset(gaiEnemyHeroReachable, 0, MAP_WIDTH * MAP_HEIGHT);
    for (playerIndex0[0] = 0; playerIndex0[0] < gpGame->m_playerCount; playerIndex0[0]++) {
        if (playerIndex0[0] == giCurPlayer)
            continue;
        if (OnMySide(playerIndex0[0]))
            continue;
        for (innerIndex27 = 0;
             innerIndex27 < reinterpret_cast<pdView *>(
                                &gpGame->m_players[playerIndex0[0]])->numHeroes;
             innerIndex27++) {
            heroPtr5 = GetHeroSlot(*(reinterpret_cast<signed char *>(gpGame) +
                playerIndex0[0] * sizeof(playerRec) + innerIndex27 +
                AI_PLAYER_HERO_IDS_BASE));
            gpSearchArray->SeedPosition(heroPtr5->m_x, heroPtr5->m_y,
                heroPtr5->m_direction, heroPtr5->m_remainingMobility + 100,
                heroPtr5->m_eventFlags & 0x80, 1, heroPtr5->m_remainingMobility + 100,
                heroPtr5->m_secondarySkills[0], -1, -1, 0, 0);
            for (x8 = 0; x8 < MAP_WIDTH; x8++) {
                for (y4 = 0; (y4 | 0) < MAP_WIDTH; y4++) {
                    if (gpSearchArray->GetRow(MAP_WIDTH, y4)[x8].visited) {
                        gaiEnemyHeroReachable[MAP_WIDTH * y4 + x8] = 1;
                        if (!(y4 != 0)) {
                        } else {
                            cell12 = gpAdvManager->GetCell(x8, y4 - 1);
                            if (cell12->triggerType == AI_OBJECT_TOWN ||
                                (cell12->triggerType & 0x7f) == AI_OBJECT_BOAT) {
                                townId3 = gpGame->GetTownId(x8, y4 - 1);
                                if (townId3 == -1) {
                                } else {
                                    townPtr9 = reinterpret_cast<townView *>(GetCastleSlot(townId3));
                                    if (townPtr9->owner == giCurPlayer &&
                                        (townPtr9->buildings & 0x40000000)) {
                                        ProbableOutcomeOfBattle(&heroPtr5->m_army, heroPtr5,
                                            &GetHeroSlot(townPtr9->visitingHero)->m_army,
                                            GetHeroSlot(townPtr9->visitingHero),
                                            reinterpret_cast<armyGroup *>(
                                                reinterpret_cast<char *>(townPtr9) + 8),
                                            1, townPtr9->id, townPtr9->owner, winChance11,
                                            outcomeA9, outcomeB6, outcomeC4, outcomeD1, outcomeE14);
                                        if (winChance11 > AI_TOWN_DEFENSE_THRESHOLD)
                                            townPtr9->threat = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    memset(gaiTurnValueOfMine, 7, MAP_WIDTH * MAP_HEIGHT);
    for (playerIndex0[0] = 0; playerIndex0[0] < gpGame->m_playerCount; playerIndex0[0]++) {
        if (playerIndex0[0] != giCurPlayer) {
            playerPtr27 = reinterpret_cast<pdView *>(
                &gpGame->m_players[playerIndex0[0]]);
            for (innerIndex27 = 0; innerIndex27 < playerPtr27->numHeroes; innerIndex27++) {
                xCenter19 = GetHeroSlot(playerPtr27->heroIds[innerIndex27])->m_x;
                yCenter3 = GetHeroSlot(playerPtr27->heroIds[innerIndex27])->m_y;
                for (x8 = xCenter19 - 10; x8 <= xCenter19 + 10; x8++) {
                    for (y4 = yCenter3 - 10; y4 <= yCenter3 + 10; y4++) {
                        if (x8 >= 0 && !(MAP_WIDTH <= x8) && y4 >= 0 && y4 < MAP_HEIGHT) {
                            mineValue4 = abs(abs(y4 - yCenter3) + abs(x8 - xCenter19) - 4) >> 2;
                            if (mineValue4 < gaiTurnValueOfMine[MAP_WIDTH * y4 + x8])
                                gaiTurnValueOfMine[MAP_WIDTH * y4 + x8] =
                                    static_cast<signed char>(mineValue4);
                        }
                    }
                }
            }
        }
    }

    for (generalIndex4 = 0; generalIndex4 < AI_HERO_COUNT; generalIndex4++)
        gfHeroInteractionBonus[generalIndex4] = 1.0f;
    if (gpGame->m_difficulty == 0) {
        gfAttackHumanBonus = 0.6f;
        gfAttackComputerBonus = 1.3f;
    } else if (gpGame->m_difficulty == 1) {
        gfAttackHumanBonus = 1.0f;
        gfAttackComputerBonus = 1.0f;
    } else {
        gfAttackHumanBonus = static_cast<float>(gpGame->m_difficulty * 0.07 + 1.0);
        gfAttackComputerBonus = static_cast<float>(1.1 - gpGame->m_difficulty * 0.12);
    }
    if (gbIAmGreatest)
        gfAttackComputerBonus = 0.1f;

    townCountValue5 = 0.1f;
    for (generalIndex4 = 0; generalIndex4 < reinterpret_cast<pdView *>(gpCurPlayer)->numCastles; generalIndex4++) {
        if (reinterpret_cast<townView *>(GetCastleSlot(
                reinterpret_cast<pdView *>(gpCurPlayer)->castleIds[generalIndex4]))
                ->buildings & 0x40000000)
            townCountValue5 += 1.0;
        else
            townCountValue5 += 0.5;
    }
    ownedTownCount19 = static_cast<int>(townCountValue5);
    giMaxHeroesForThisPlayer = 3;
    if (gpGame->m_playerCount - gpGame->m_deadPlayerCount == 2)
        giMaxHeroesForThisPlayer++;
    if (gpGame->m_playerCount - gpGame->m_deadPlayerCount == 3)
        giMaxHeroesForThisPlayer++;
    if (ownedTownCount19 >= 4)
        giMaxHeroesForThisPlayer++;
    if (ownedTownCount19 >= 9)
        giMaxHeroesForThisPlayer++;
    if (gpGame->m_mapHeader.width == AI_MAP_SIZE_SMALL)
        giMaxHeroesForThisPlayer--;
    if (gpGame->m_mapHeader.width >= AI_MAP_SIZE_LARGE)
        giMaxHeroesForThisPlayer++;

    earlyTurn21 = 16;
    lateTurn40 = 22;
    if (reinterpret_cast<pdView *>(gpCurPlayer)->difficulty == 2) {
        earlyTurn21 = 8;
        lateTurn40 = 15;
    }
    if (earlyTurn21 > giCurTurn && ownedTownCount19 == 1 &&
        reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes == 1) {
        heroPtr5 = GetHeroSlot(reinterpret_cast<pdView *>(gpCurPlayer)->heroIds[0]);
        townPtr9 = reinterpret_cast<townView *>(GetCastleSlot(
            reinterpret_cast<pdView *>(gpCurPlayer)->castleIds[0]));
        if (abs(townPtr9->castleX - heroPtr5->m_x) +
                abs(townPtr9->castleY - heroPtr5->m_y) < 18)
            giMaxHeroesForThisPlayer = 1;
    }
    if (lateTurn40 > giCurTurn && giMaxHeroesForThisPlayer > 2) {
        giMaxHeroesForThisPlayer = 2;
    }
    if (reinterpret_cast<pdView *>(gpCurPlayer)->difficulty == 2)
        ownedTownCount19++;
    if (ownedTownCount19 + 1 < giMaxHeroesForThisPlayer)
        giMaxHeroesForThisPlayer = ownedTownCount19 + 1;
    if (giMaxHeroesForThisPlayer < reinterpret_cast<pdView *>(gpCurPlayer)->minimumHeroes)
        giMaxHeroesForThisPlayer = reinterpret_cast<pdView *>(gpCurPlayer)->minimumHeroes;
}

// @early-stop
// Instruction stream is identical with relocations masked; all residual rows are
// delinker names for string literals and compiler-emitted floating constants.
VA(0x0043b154, 0x5f4)
void philAI::GetBestBHC(int player, BHC &best) {
    int townStrength[72];
    int desiredStrength[72];
    BHC candidate;
    float value = 1.0f;
    float bestValue = -99.0f;
    int totalStrength = 0;
    int totalWeight = 0;
    int averageStrength;
    int i;
    townView *townPtr;

    for (i = 0; i < reinterpret_cast<pdView *>(gpCurPlayer)->numCastles; i++) {
        townPtr = reinterpret_cast<townView *>(&gpGame->m_castleRecs[
            reinterpret_cast<pdView *>(gpCurPlayer)->castleIds[i]]);
        townStrength[i] = FightValueOfStack(
            reinterpret_cast<armyGroup *>(reinterpret_cast<char *>(townPtr) + 8),
            0, 0, 0, 0, 0) + 400;
        totalStrength += townStrength[i];
        if (townPtr->buildings & 0x40000000)
            totalWeight += 10;
        else
            totalWeight += 7;
    }
    if (totalWeight < 1)
        totalWeight = 1;
    averageStrength = totalStrength / totalWeight;
    for (i = 0; i < reinterpret_cast<pdView *>(gpCurPlayer)->numCastles; i++) {
        townPtr = reinterpret_cast<townView *>(
            &gpGame->m_castleRecs[reinterpret_cast<pdView *>(gpCurPlayer)->castleIds[i]]);
        desiredStrength[i] = ((townPtr->buildings & 0x40000000) ? 10 : 7) *
                                 averageStrength +
                             400;
    }

    for (i = 0; i < reinterpret_cast<pdView *>(gpCurPlayer)->numCastles; i++) {
        townPtr = reinterpret_cast<townView *>(
            &gpGame->m_castleRecs[reinterpret_cast<pdView *>(gpCurPlayer)->castleIds[i]]);
        LogInt("Turns Owned", townPtr->turnsOwned, -999, -999, -999, -999, -999, -999);
        if (giCurTurn > 4 && townPtr->turnsOwned < 2)
            continue;
        {
            CheckDoMain(0, 0);
            GetBestBuilding(reinterpret_cast<town *>(townPtr), candidate, value);
            value = static_cast<float>((100 - Random(0, 10)) /
                                       AI_PURCHASE_RANDOM_DIVISOR * value);
            if (value > bestValue) {
                bestValue = value;
                best = candidate;
            }

            CheckDoMain(0, 0);
            GetBestCreature(reinterpret_cast<town *>(townPtr), candidate, value);
            value = static_cast<float>(
                (static_cast<float>(desiredStrength[i]) / townStrength[i] / 3.0f +
                 AI_CREATURE_BALANCE_BASE) *
                value);
            value = static_cast<float>((100 - Random(0, 10)) /
                                       AI_PURCHASE_RANDOM_DIVISOR * value);
            if (value > bestValue) {
                bestValue = value;
                best = candidate;
            }

            CheckDoMain(0, 0);
            if (reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes < giMaxHeroesForThisPlayer &&
                (townPtr->buildings & 0x40000000)) {
                GetBestHero(reinterpret_cast<town *>(townPtr), candidate, value);
                value = static_cast<float>((100 - Random(0, 10)) /
                                           AI_PURCHASE_RANDOM_DIVISOR * value);
                if (!bHeroBuiltThisTurn && giCurTurn > 5 && value > 0.0f) {
                    if (!((reinterpret_cast<pdView *>(gpCurPlayer)->income[RES_GOLD] < 1250 ||
                           giMaxHeroesForThisPlayer - 2 <=
                               reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes) &&
                          reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes > 1)) {
                        value += AI_HERO_PURCHASE_BONUS;
                    } else if (reinterpret_cast<pdView *>(gpCurPlayer)->income[RES_GOLD] >= 1500 &&
                               reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes <
                                   giMaxHeroesForThisPlayer - 1) {
                        value = static_cast<float>(value * AI_HERO_PURCHASE_FACTOR);
                    }
                } else if (reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes == 0) {
                    value += AI_HERO_PURCHASE_BONUS;
                }
                if (value > bestValue) {
                    bestValue = value;
                    best = candidate;
                }
            }
        }
    }
    LogInt("BestBHC ", best.type, static_cast<int>(bestValue * 100.0f), best.what,
           0, 0, -999, -999);
    if (bestValue < AI_MINIMUM_PURCHASE_VALUE)
        best.type = -1;
}

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

// @early-stop
// The 0x90 frame and all 179 relocation sites align; normalDirTable+2 is the
// sole delinker alias, for the proven .y field. Relocation-masked comparison
// leaves only: the IsEmbarked /Ob1 continuation moved from +0x454..+0x469 to
// the shared tail at +0xe6f..+0xe74 (the exact five-byte span difference), and
// commutative Manhattan addend order at +0x472..+0x493 and +0xbdb..+0xc2a.
// Positive/negative accessor contexts and both source addend orders were tested.
VA(0x0043b865, 0xe7d)
int philAI::DetermineTargetPosition(int &targetX, int &targetY, int mobility,
                                    int &direction) {
    int boatTravelDistanceCounter;
    int bestValue;
    int scanMinXValue;
    int adjacentDirection;
    int searchPassIndex;
    int columnCounter;
    int targetScoreLocal;
    int scanMinY;
    int travelDistanceRange;
    int pathRowCounter;
    int remainingMobilityPath;
    int x;
    int y;
    int heroIndexSearch;
    mapCell *candidateCell;
    int targetBestXRange;
    int scanMaxXLocal;
    int doubledMobilityScore;
    int scanMaxYCounter;
    int candidate;
    int targetBestYLocal;
    int mapTerrain;
    townSlot *shipyardPtrSearch;
    int scanSpacingStep;

    if (GoodAdjacent(&adjacentDirection)) {
        targetX = normalDirTable[adjacentDirection].x + gpCurAIHero->m_x;
        targetY = normalDirTable[adjacentDirection].y + gpCurAIHero->m_y;
        direction = adjacentDirection;
        return 1000;
    }

    direction = -1;
    targetBestXRange = -1;
    targetBestYLocal = -1;
    bestValue = -999999;
    giBestShipyardId = -1;
    gbPossibleShipyardFound = 0;
    gbActualShipyardFound = 0;
    gbActualBoatFound = 0;

    candidateCell = gpAdvManager->GetCell(gpCurAIHero->m_x, gpCurAIHero->m_y);
    mapTerrain = giGroundToTerrain[candidateCell->tile];
    if (gpCurAIHero->m_secondarySkills[0] <= HERO_SKILL_LEVEL_BASIC) {
        if (mapTerrain == 2 || mapTerrain == 3)
            mobility = static_cast<int>(mobility * 1.15);
        if (mapTerrain == 5)
            mobility = static_cast<int>(mobility * 1.35);
        if (mapTerrain == 7)
            mobility = static_cast<int>(mobility * 1.25);
    } else if (gpGame->m_mapHeader.width == AI_MAP_SIZE_SMALL) {
        mobility = static_cast<int>(mobility * 0.9);
    } else if (gpGame->m_mapHeader.width == AI_MAP_SIZE_LARGE) {
        mobility = static_cast<int>(mobility * 1.05);
    } else if (gpGame->m_mapHeader.width == AI_MAP_SIZE_XLARGE) {
        mobility = static_cast<int>(mobility * 1.1);
    }
    if (reinterpret_cast<pdView *>(gpCurPlayer)->difficulty == 2)
        mobility = static_cast<int>(mobility * 1.15);

    scanSpacingStep = mobility / 100 / 4;
    if (scanSpacingStep < 6)
        scanSpacingStep = 6;
    doubledMobilityScore = mobility * 2;
    gpSearchArray->SeedPosition(gpCurAIHero->m_x, gpCurAIHero->m_y,
        gpCurAIHero->m_direction, doubledMobilityScore,
        gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED,
        1, gpCurAIHero->m_remainingMobility,
        gpCurAIHero->m_secondarySkills[0], -1, -1, 0, 0);
    gpSearchArray->GetNode(gpCurAIHero->m_x | 0, gpCurAIHero->m_y).visited = 0;

    columnCounter = -1;
    scanMinXValue = gpCurAIHero->m_x - 25;
    if (scanMinXValue < 0)
        scanMinXValue = 0;
    scanMinY = gpCurAIHero->m_y - 25;
    if (scanMinY < 0)
        scanMinY = 0;
    scanMaxXLocal = gpCurAIHero->m_x + 25;
    if (scanMaxXLocal > MAP_WIDTH - 1)
        scanMaxXLocal = MAP_WIDTH;
    scanMaxYCounter = gpCurAIHero->m_y + 25;
    if (scanMaxYCounter > MAP_HEIGHT - 1)
        scanMaxYCounter = MAP_HEIGHT;

    for (searchPassIndex = 0; searchPassIndex < 2; searchPassIndex++) {
        if (gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED) {
            if (searchPassIndex == 0)
                continue;
        }
            for (x = scanMinXValue; (x | 0) < scanMaxXLocal; x++) {
                pathRowCounter = -1;
                columnCounter++;
                if ((scanSpacingStep | 0) <= columnCounter)
                    columnCounter = 0;
                for (y = scanMinY; (y | 0) < scanMaxYCounter; y++) {
                    pathRowCounter++;
                    if ((scanSpacingStep | 0) <= pathRowCounter)
                        pathRowCounter = 0;
                    if (gpSearchArray->GetNode(x, y).visited) {
                    if (gpCurAIHero->IsEmbarked()) {
                    } else {
                        travelDistanceRange = abs(x - gpCurAIHero->m_x) +
                                abs(y - gpCurAIHero->m_y);
                        if ((searchPassIndex == 0 && travelDistanceRange > 5 &&
                             (gpCurAIHero->m_destinationX != x ||
                              gpCurAIHero->m_destinationY != y) &&
                             ((travelDistanceRange | 0) >= 16 ||
                              (gpAdvManager->GetCell(x, y)->triggerType != AI_OBJECT_HERO &&
                               (gpAdvManager->GetCell(x, y)->triggerType != AI_OBJECT_TOWN ||
                                reinterpret_cast<townView *>(GetCastleSlot(
                                    gpAdvManager->GetCell(x, y)->w4hi))->owner == giCurPlayer)))) ||
                            (searchPassIndex == 1 &&
                             (travelDistanceRange <= 5 ||
                              (gpCurAIHero->m_destinationX == x &&
                               gpCurAIHero->m_destinationY == y) ||
                              ((travelDistanceRange | 0) < 16 &&
                               (gpAdvManager->GetCell(x, y)->triggerType == AI_OBJECT_HERO ||
                                (gpAdvManager->GetCell(x, y)->triggerType == AI_OBJECT_TOWN &&
                                 reinterpret_cast<townView *>(GetCastleSlot(
                                     gpAdvManager->GetCell(x, y)->w4hi))->owner != giCurPlayer))))))
                            continue;
                    }

                    candidateCell = gpAdvManager->GetCell(x, y);
                    if (mobility < gpSearchArray->GetNode(x, y).distance) {
                        if (doubledMobilityScore < gpSearchArray->GetNode(x, y).distance) {
                            candidate = 0;
                        } else if (candidateCell->triggerType == AI_OBJECT_TOWN ||
                                   candidateCell->triggerType == AI_OBJECT_HERO ||
                                   (candidateCell->triggerType == AI_OBJECT_BOAT &&
                                    !(gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED))) {
                            candidate = 1;
                        } else {
                            candidate = 0;
                        }
                    } else if ((candidateCell->triggerType & 0x80) ||
                               (candidateCell->triggerType == AI_OBJECT_BOAT_TRIGGER &&
                                (gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED)) ||
                               (x % scanSpacingStep == 0 && y % scanSpacingStep == 0 &&
                                 (((gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
                                   giGroundToTerrain[candidateCell->tile] == 0) ||
                                  (!(gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
                                   giGroundToTerrain[candidateCell->tile] != 0))) ||
                               (reinterpret_cast<pdView *>(gpCurPlayer)->routeX == x &&
                                reinterpret_cast<pdView *>(gpCurPlayer)->routeY == y)) {
                        candidate = 1;
                    } else {
                        candidate = 0;
                    }

                    if (candidate && gpCurAIHero->m_boatId != 0xff) {
                        boatTravelDistanceCounter =
                            abs(y - static_cast<unsigned char>(gpCurAIHero->m_unknown2a)) +
                            abs(x - static_cast<unsigned char>(gpCurAIHero->m_boatId));
                        if (gpCurAIHero->m_unknown2b < boatTravelDistanceCounter)
                            candidate = 0;
                    }
                    if (candidate) {
                        for (heroIndexSearch = 0;
                             heroIndexSearch < reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes;
                             heroIndexSearch++) {
                            if (candidateCell->triggerType != AI_OBJECT_TOWN &&
                                candidateCell->triggerType != AI_OBJECT_HERO &&
                                reinterpret_cast<pdView *>(gpCurPlayer)->heroIds[heroIndexSearch] !=
                                    static_cast<unsigned char>(gpCurAIHero->m_id) &&
                                reinterpret_cast<gameHeroPositionView *>(gpGame)->
                                        heroRecs[reinterpret_cast<pdView *>(gpCurPlayer)->
                                                     heroIds[heroIndexSearch]].x == x &&
                                reinterpret_cast<gameHeroPositionView *>(gpGame)->
                                        heroRecs[reinterpret_cast<pdView *>(gpCurPlayer)->
                                                     heroIds[heroIndexSearch]].y == y) {
                                targetScoreLocal = -2000;
                                goto candidate_scored;
                            }
                        }
                        CheckDoMain(0, 0);
                        remainingMobilityPath = gpCurAIHero->m_mobility -
                                gpSearchArray->GetNode(x, y).distance;
                        if (remainingMobilityPath < 0)
                            remainingMobilityPath = 0;
                        targetScoreLocal = RVOfPosition(x, y,
                            gpSearchArray->GetNode(x, y).rvFlag1,
                            reinterpret_cast<unsigned char *>(
                                &gpSearchArray->GetNode(x, y))[5],
                            reinterpret_cast<unsigned char *>(
                                &gpSearchArray->GetNode(x, y))[6],
                            gpSearchArray->GetNode(x, y).rvFlag2,
                            reinterpret_cast<unsigned char *>(
                                &gpSearchArray->GetNode(x, y))[7],
                            reinterpret_cast<unsigned char *>(
                                &gpSearchArray->GetNode(x, y))[8], 2,
                            remainingMobilityPath);
                        targetScoreLocal = (Random(1, 50) + 75) * targetScoreLocal;
                        targetScoreLocal /= 100;
                    } else {
                        targetScoreLocal = -100;
                    }
                    if (targetX == x && targetY == y) {
                        targetScoreLocal = static_cast<int>(targetScoreLocal * 1.1f);
                        targetScoreLocal += 20;
                    }

candidate_scored:
                    if ((bestValue | 0) < targetScoreLocal) {
                        targetBestXRange = x;
                        targetBestYLocal = y;
                        bestValue = targetScoreLocal;
                    } else if ((bestValue | 0) == targetScoreLocal && targetScoreLocal == 0 &&
                               !(gpAdvManager->GetCell(x, y)->triggerType & 0x80)) {
                        if ((gpAdvManager->GetCell(targetBestXRange, targetBestYLocal)->triggerType & 0x80) ||
                            ((abs(targetBestXRange - gpCurAIHero->m_x) +
                              abs(targetBestYLocal - gpCurAIHero->m_y)) | 0) <
                            abs(x - gpCurAIHero->m_x) + abs(y - gpCurAIHero->m_y)) {
                            targetBestXRange = x;
                            targetBestYLocal = y;
                        }
                    }
                    }
                }
            }
            if (searchPassIndex == 0 && bestValue > 150)
                break;
    }

    if (bestValue < 75 && (gbPossibleShipyardFound || gbActualShipyardFound) &&
        !gbActualBoatFound && giCurTurn > 3) {
        if ((gbActualShipyardFound || giBuildShipyard[giCurPlayer] < 0 ||
             giBuildShipyard[giCurPlayer] == giBestShipyardId) &&
            ((!gbActualShipyardFound ? 20 : 0) + 10 <=
             reinterpret_cast<pdView *>(gpCurPlayer)->income[0] * 6 +
             reinterpret_cast<pdView *>(gpCurPlayer)->resources[0])) {
            if (!gbActualShipyardFound)
                giBuildShipyard[giCurPlayer] = static_cast<signed char>(giBestShipyardId);
            giBuildBoat[giCurPlayer] = static_cast<signed char>(giBestShipyardId);
            giBuildBoatStuffTurn[giCurPlayer] = static_cast<signed char>(giCurTurn);
            shipyardPtrSearch = GetCastleSlot(giBestShipyardId);
            bestValue = 123;
            targetBestXRange = reinterpret_cast<townView *>(shipyardPtrSearch)->castleX;
            targetBestYLocal = reinterpret_cast<townView *>(shipyardPtrSearch)->castleY;
            if (gpCurAIHero->m_x == targetBestXRange && gpCurAIHero->m_y == targetBestYLocal)
                gpCurAIHero->m_remainingMobility = 0;
        }
        CheckBuyStuff();
    }

    targetX = targetBestXRange;
    targetY = targetBestYLocal;
    if (gpCurAIHero->m_boatId != 0xff && bestValue <= 0) {
        targetX = static_cast<unsigned char>(gpCurAIHero->m_boatId);
        targetY = static_cast<unsigned char>(gpCurAIHero->m_unknown2a);
    }
    LogInt("Hero  Best RV target XY current XY", gpCurAIHero->m_owner, bestValue,
           targetX, targetY, gpCurAIHero->m_x, gpCurAIHero->m_y, -999);
    LogStr("----");
    return bestValue;
}

// @early-stop
// Relocation-masked raw bytes differ only at +0x4ee/+0x4f1 and +0x62a/+0x62d:
// MSVC loads the two commutative hero-artifact address operands into eax/ecx in
// the opposite order. The effective addresses agree, as do all 84 relocations.
VA(0x0043c6e2, 0x791)
void philAI::ProbableOutcomeOfBattle(armyGroup *attacker, hero *attackerHero,
                                     armyGroup *defender, hero *defenderHero,
                                     armyGroup *townArmy, int useTown,
                                     int townId, int enemyPlayer,
                                     float &winChance, int &attackerLoss,
                                     int &defenderLoss, int &attackerRemaining,
                                     int &defenderRemaining, int &outcomeValue)
{
    int unusedValue29;
    int attackerArtifacts7 = 0;
    int defenderArtifacts18 = 0;
    float defenderFightValue5;
    float attackerFightValue26;
    float defenderPower19;
    float attackerStrength0;
    int experienceValue1;
    int artifactIndex15;
    float defenderRawValue4;
    float attackerRawValue7;
    float defenderStrength4;
    float difficultyFactor5;
    float exponent1;
    float attackerPower17;
    float attackBonus13;

    attackerFightValue26 = static_cast<float>(
        FightValueOfStack(attacker, attackerHero, 1, 0, 0, useTown));
    defenderFightValue5 = static_cast<float>(
        FightValueOfStack(defender, defenderHero, 1, useTown, townId, 0));
    if (townArmy != 0)
        defenderFightValue5 += static_cast<float>(
            FightValueOfStack(townArmy, 0, 1, 0, 0, 0));

    attackerRawValue7 = static_cast<float>(
        FightValueOfStack(attacker, attackerHero, 0, 0, 0, 0));
    defenderRawValue4 = static_cast<float>(
        FightValueOfStack(defender, defenderHero, 0, 0, 0, 0));
    if (townArmy != 0)
        defenderRawValue4 += static_cast<float>(
            FightValueOfStack(townArmy, 0, 0, 0, 0, 0));

    if (useTown != 0)
        defenderFightValue5 = static_cast<float>(
            defenderFightValue5 * AI_BATTLE_TOWN_DEFENDER_FACTOR);

    defenderStrength4 = defenderFightValue5;
    if (enemyPlayer == AI_BATTLE_NO_PLAYER) {
        attackerStrength0 = static_cast<float>(
            (gpGame->m_difficulty * AI_BATTLE_DIFFICULTY_STEP +
             AI_BATTLE_BASE_STRENGTH_FACTOR) * attackerFightValue26);
        if (reinterpret_cast<pdView *>(gpCurPlayer)->difficulty ==
            AI_BATTLE_EASY_DIFFICULTY)
            attackerStrength0 = static_cast<float>(
                attackerStrength0 * AI_BATTLE_EASY_STRENGTH_FACTOR);
    } else {
        attackerStrength0 = attackerFightValue26;
        if (gbHumanPlayer[enemyPlayer] != 0) {
            defenderStrength4 = static_cast<float>(
                defenderStrength4 * AI_BATTLE_HUMAN_DEFENDER_FACTOR);
        } else if (reinterpret_cast<pdView *>(gpCurPlayer)->difficulty ==
                   AI_BATTLE_EASY_DIFFICULTY) {
            attackerStrength0 = static_cast<float>(
                attackerStrength0 * AI_BATTLE_EASY_STRENGTH_FACTOR);
        }
    }

    if (attackerStrength0 < AI_BATTLE_MINIMUM_STRENGTH)
        attackerStrength0 = AI_BATTLE_MINIMUM_STRENGTH;
    if (defenderStrength4 < AI_BATTLE_MINIMUM_STRENGTH)
        defenderStrength4 = AI_BATTLE_MINIMUM_STRENGTH;

    exponent1 = AI_BATTLE_NORMAL_POWER;
    if (AI_BATTLE_LARGE_STRENGTH < attackerStrength0 ||
        AI_BATTLE_LARGE_STRENGTH < defenderStrength4)
        exponent1 = AI_BATTLE_LARGE_POWER;

    attackerPower17 = static_cast<float>(
        pow(static_cast<double>(attackerStrength0), static_cast<double>(exponent1)));
    defenderPower19 = static_cast<float>(
        pow(static_cast<double>(defenderStrength4), static_cast<double>(exponent1)));
    winChance = attackerPower17 / (defenderPower19 + attackerPower17);

    if (winChance < AI_BATTLE_ZERO_CHANCE) {
        winChance = 0.0f;
    } else if (winChance < AI_BATTLE_LOW_CHANCE) {
        winChance = static_cast<float>(winChance - AI_BATTLE_LOW_PENALTY);
    } else if (winChance < AI_BATTLE_MEDIUM_CHANCE) {
        winChance = static_cast<float>(winChance - AI_BATTLE_MEDIUM_PENALTY);
    } else if (winChance < AI_BATTLE_HIGH_CHANCE) {
        winChance = static_cast<float>(winChance - AI_BATTLE_HIGH_PENALTY);
    } else if (winChance < AI_BATTLE_TOP_CHANCE) {
        winChance = static_cast<float>(winChance - AI_BATTLE_TOP_PENALTY);
    }

    attackerLoss = static_cast<int>((1.0 - winChance) * attackerRawValue7);
    defenderLoss = static_cast<int>((+defenderRawValue4) * winChance);
    attackerRemaining = static_cast<int>(
        (1.0f - winChance) * attackerRawValue7 + attackerLoss * winChance);
    defenderRemaining = static_cast<int>(
        (1.0f - winChance) * defenderLoss + (+defenderRawValue4) * winChance);

    difficultyFactor5 = static_cast<float>(
        AI_BATTLE_LOSS_FACTOR_BASE -
        reinterpret_cast<pdView *>(gpCurPlayer)->baseUpgradeFactor);
    outcomeValue = static_cast<int>(
        -attackerRemaining * difficultyFactor5 * difficultyFactor5);
    if (enemyPlayer >= 0) {
        difficultyFactor5 = static_cast<float>(
            reinterpret_cast<pdView *>(gpCurPlayer)->baseUpgradeFactor +
            AI_BATTLE_PLAYER_FACTOR_BASE);
        if (gbHumanPlayer[enemyPlayer] != 0)
            outcomeValue = static_cast<int>(
                defenderRemaining * gfAttackHumanBonus * difficultyFactor5 *
                    difficultyFactor5 + outcomeValue);
        else
            outcomeValue = static_cast<int>(
                defenderRemaining * gfAttackComputerBonus * difficultyFactor5 *
                    difficultyFactor5 + outcomeValue);
    }
    outcomeValue = static_cast<int>(
        outcomeValue * reinterpret_cast<pdView *>(gpCurPlayer)->upgradeFactor);

    if (attackerHero != 0) {
        for (artifactIndex15 = 0;
             artifactIndex15 < AI_BATTLE_ARTIFACT_SLOT_COUNT;
             artifactIndex15++) {
            if (attackerHero->m_artifacts[artifactIndex15] >= 0 &&
                attackerHero->m_artifacts[artifactIndex15] <
                    AI_BATTLE_BASE_ARTIFACT_LIMIT)
                attackerArtifacts7 +=
                    gArtifactBaseRV[attackerHero->m_artifacts[artifactIndex15]];
        }
        if (reinterpret_cast<gameTV *>(gpGame)->f2c6 ==
                AI_BATTLE_SPECIAL_ARTIFACT_STATE &&
            static_cast<unsigned char>(attackerHero->m_id) ==
                reinterpret_cast<gameTV *>(gpGame)->f2c9)
            attackerArtifacts7 += AI_BATTLE_SPECIAL_ARTIFACT_VALUE;

        outcomeValue = static_cast<int>(
            outcomeValue - (AI_BATTLE_FULL_CHANCE - winChance) *
                (attackerArtifacts7 + AI_BATTLE_ATTACKER_ARTIFACT_BASE));
        experienceValue1 = gpGame->ExperienceValueOfStack(defender, defenderHero);
        outcomeValue = static_cast<int>(
            experienceValue1 * attackerHero->m_aiFightValue * winChance *
                AI_BATTLE_HERO_EXPERIENCE_FACTOR + outcomeValue);
    }

    if (defenderHero != 0) {
        for (artifactIndex15 = 0;
             artifactIndex15 < AI_BATTLE_ARTIFACT_SLOT_COUNT;
             artifactIndex15++) {
            if (defenderHero->m_artifacts[artifactIndex15] >= 0 &&
                defenderHero->m_artifacts[artifactIndex15] <
                    AI_BATTLE_BASE_ARTIFACT_LIMIT)
                defenderArtifacts18 +=
                    gArtifactBaseRV[defenderHero->m_artifacts[artifactIndex15]];
        }
        if (reinterpret_cast<gameTV *>(gpGame)->f2cb ==
                AI_BATTLE_SPECIAL_ARTIFACT_STATE &&
            static_cast<unsigned char>(defenderHero->m_id) ==
                reinterpret_cast<gameTV *>(gpGame)->f2cc)
            defenderArtifacts18 += AI_BATTLE_SPECIAL_ARTIFACT_VALUE;

        if (gbHumanPlayer[defenderHero->m_owner] != 0)
            attackBonus13 = gfAttackHumanBonus;
        else
            attackBonus13 = gfAttackComputerBonus;
        outcomeValue = static_cast<int>(
            winChance * ((defenderArtifacts18 + AI_BATTLE_DEFENDER_ARTIFACT_BASE) *
                         attackBonus13) + outcomeValue);
    }

    if (giDebugLevel >= AI_BATTLE_DEBUG_LEVEL) {
        LogInt("POBA", static_cast<int>(attackerStrength0),
               static_cast<int>(defenderStrength4),
               static_cast<int>(reinterpret_cast<pdView *>(gpCurPlayer)->baseUpgradeFactor *
                                AI_BATTLE_PERCENT_SCALE),
               0, attackerArtifacts7, defenderArtifacts18,
               static_cast<int>(reinterpret_cast<pdView *>(gpCurPlayer)->upgradeFactor));
        LogInt("POB", static_cast<int>(attackerFightValue26),
               static_cast<int>(defenderFightValue5),
               static_cast<int>(winChance * AI_BATTLE_PERCENT_SCALE),
               defenderLoss, attackerRemaining, defenderRemaining, outcomeValue);
    }
}

VA(0x0043ce73, 0x1e)
float philAI::GetOddsOfWinning(int) {
    return 1.0f;
}

// @early-stop
// Relocation-masked bytes are exact across all 0x826 bytes. All 75 relocation
// sites and types align; retail delinks constant-pool/addended data references
// and jump-table locals under different symbol identities.
VA(0x0043ce91, 0x826)
void philAI::ValueOfBuyingBuilding(town *townPtr, int building, int &resourceValue,
                                   float &benefitCost) {
    int currentTownRace;
    int dwellingTotal;
    int highestDwellingId;
    int currentOccupiedSlots;
    int mageGuildLevelLocal;
    float adjustedValue;
    int creatureLocated;
    int indexBuilding;
    int buildingLevel;
    int currentCreatureType;
    int costsByResource[7];
    float estimatedAttackChance;
    float enemyStrengthLocal;
    int currentAttackTurns;
    int projectedAttackValue;
    int estimatedAttackWeeks;
    float dangerRating;
    currentTownRace = reinterpret_cast<townView *>(townPtr)->race;
    dwellingTotal = 0;
    highestDwellingId = -1;
    for (indexBuilding = AI_BUILDING_FIRST_DWELLING;
         indexBuilding < AI_BUILDING_LAST_DWELLING;
         indexBuilding++) {
        if (reinterpret_cast<townView *>(townPtr)->buildings & (1 << indexBuilding)) {
            dwellingTotal++;
            highestDwellingId = indexBuilding;
        }
    }
    currentOccupiedSlots = 0;
    for (indexBuilding = 0; indexBuilding < AI_TOWN_ARMY_SLOTS; indexBuilding++) {
        if (reinterpret_cast<townView *>(townPtr)->army.quantities[indexBuilding] > 0)
            currentOccupiedSlots++;
    }
    if (building == AI_BUILDING_MAGE_GUILD)
        mageGuildLevelLocal = reinterpret_cast<townView *>(townPtr)->buildState + 1;
    else
        mageGuildLevelLocal = 1;
    adjustedValue = static_cast<float>(GetBuildingBaseResourceValue(
        currentTownRace, building, mageGuildLevelLocal));
    if (building >= AI_BUILDING_FIRST_UPGRADE &&
        building <= AI_BUILDING_LAST_UPGRADE) {
        adjustedValue -= static_cast<float>(GetBuildingBaseResourceValue(
            currentTownRace, building - 5, 1));
    }
    if (building == AI_BUILDING_LAST_DWELLING) {
        adjustedValue -= static_cast<float>(GetBuildingBaseResourceValue(
            currentTownRace, AI_BUILDING_LAST_DWELLING - 1, 1));
    }
    if (building == AI_BUILDING_MAGE_GUILD &&
        reinterpret_cast<townView *>(townPtr)->buildState > 1) {
        adjustedValue -= static_cast<float>(GetBuildingBaseResourceValue(
            currentTownRace, building,
            reinterpret_cast<townView *>(townPtr)->buildState));
    }

    switch (building) {
    case AI_BUILDING_SPECIAL_SIX:
        if (*(reinterpret_cast<signed char *>(townPtr) + 0x37) != 0)
            adjustedValue = -99.0f;
        break;
    case AI_BUILDING_MAGE_GUILD:
        if (reinterpret_cast<townView *>(townPtr)->race == 0 ||
            reinterpret_cast<townView *>(townPtr)->race == 1) {
            if ((reinterpret_cast<townView *>(townPtr)->buildState >= 4 && giCurTurn < 40) ||
                (reinterpret_cast<townView *>(townPtr)->buildState >= 3 && giCurTurn < 30) ||
                (reinterpret_cast<townView *>(townPtr)->buildState >= 2 && giCurTurn < 20))
                adjustedValue = static_cast<float>(adjustedValue * 0.4);
            else
                adjustedValue = static_cast<float>(adjustedValue * 0.65);
        } else if (reinterpret_cast<townView *>(townPtr)->buildState == 0) {
            adjustedValue = static_cast<float>(adjustedValue * 1.2);
        } else if ((reinterpret_cast<townView *>(townPtr)->buildState >= 4 && giCurTurn < 35) ||
                   (reinterpret_cast<townView *>(townPtr)->buildState >= 3 && giCurTurn < 25) ||
                   (reinterpret_cast<townView *>(townPtr)->buildState >= 2 && giCurTurn < 15)) {
            adjustedValue = static_cast<float>(adjustedValue * 0.55);
        }
        break;
    case AI_BUILDING_SPECIAL_ONE:
        if (reinterpret_cast<townView *>(townPtr)->race == 5 &&
            (reinterpret_cast<townView *>(townPtr)->buildings &
             (1 << AI_BUILDING_THIRD_DWELLING)))
            adjustedValue = 1500.0f;
        else if (giCurTurn < 21)
            adjustedValue = 0.0f;
        break;
    case AI_BUILDING_SPECIAL_SEVEN:
        if (giCurTurn < 3 &&
            !(reinterpret_cast<townView *>(townPtr)->buildings &
              (1 << AI_BUILDING_THIRD_DWELLING)))
            adjustedValue = 0.0f;
        break;
    case 10:
        if (giCurTurn < 21)
            adjustedValue = 0.0f;
        break;
    case 11:
        if (gpGame->m_day < 6)
            adjustedValue = 0.0f;
        break;
    case 8:
    case 9:
    case 12:
    case 15:
    deferEarlyBuilding:
        if (giCurTurn < 16)
            adjustedValue = 0.0f;
        break;
    case 13:
        if (reinterpret_cast<townView *>(townPtr)->race == 0 ||
            reinterpret_cast<townView *>(townPtr)->race == 2 ||
            reinterpret_cast<townView *>(townPtr)->race == 1 ||
            reinterpret_cast<townView *>(townPtr)->race == 5) {
            goto deferEarlyBuilding;
        }
        break;
    case 14:
        break;
    case AI_BUILDING_SPECIAL_THREE:
        adjustedValue = 0.0f;
        break;
    case AI_BUILDING_SPECIAL_FOUR:
        adjustedValue = static_cast<float>(
            (reinterpret_cast<pdView *>(gpCurPlayer)->buildingFactor + 0.66) * adjustedValue);
        adjustedValue = static_cast<float>(
            (reinterpret_cast<pdView *>(gpCurPlayer)->baseUpgradeFactor * 2.0f + 0.33) * adjustedValue);
        adjustedValue = static_cast<float>((dwellingTotal * 0.33 + 0.66) * adjustedValue);
        if ((reinterpret_cast<townView *>(townPtr)->race != 0 ||
             !(reinterpret_cast<townView *>(townPtr)->buildings &
               (1 << AI_BUILDING_SECOND_DWELLING))) &&
            gpGame->m_day < 6)
            adjustedValue = 0.0f;
        break;
    case AI_BUILDING_SPECIAL_TWO:
        if (reinterpret_cast<townView *>(townPtr)->race == 5)
            break;
        if ((reinterpret_cast<townView *>(townPtr)->race == 0 &&
             (reinterpret_cast<townView *>(townPtr)->buildings &
              (1 << AI_BUILDING_THIRD_DWELLING))) ||
            (reinterpret_cast<townView *>(townPtr)->race == 2 &&
             (reinterpret_cast<townView *>(townPtr)->buildings &
              (1 << AI_BUILDING_THIRD_DWELLING)))) {
            adjustedValue = 1000.0f;
        } else {
            goto deferEarlyBuilding;
        }
        break;
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
        if (currentOccupiedSlots == AI_TOWN_ARMY_SLOTS) {
            creatureLocated = 0;
            for (indexBuilding = 0; indexBuilding < AI_TOWN_ARMY_SLOTS; indexBuilding++) {
                if (gDwellingType[reinterpret_cast<townView *>(townPtr)->race]
                                 [building - AI_BUILDING_FIRST_DWELLING] ==
                    reinterpret_cast<townView *>(townPtr)->army.creatureTypes[indexBuilding]) {
                    creatureLocated = 1;
                }
            }
            if (!creatureLocated)
                break;
        }
        adjustedValue = static_cast<float>(
            (reinterpret_cast<pdView *>(gpCurPlayer)->buildingFactor + 0.66) * adjustedValue);
        adjustedValue = static_cast<float>(
            (reinterpret_cast<pdView *>(gpCurPlayer)->baseUpgradeFactor * 2.0f + 0.33) * adjustedValue);
        adjustedValue = static_cast<float>(
            (1.0 - gpCurPlayer->BuildingsOwned(currentTownRace, building, 0) * 0.05) * adjustedValue);
        if (building - AI_BUILDING_FIRST_DWELLING < highestDwellingId)
            adjustedValue = static_cast<float>((1.66 - dwellingTotal * 0.33) * adjustedValue);
        if (reinterpret_cast<townView *>(townPtr)->buildings & 0x10)
            adjustedValue = static_cast<float>(adjustedValue * 1.1);
        for (buildingLevel = 0; buildingLevel < AI_DWELLING_LEVELS; buildingLevel++) {
            currentCreatureType =
                gDwellingType[reinterpret_cast<townView *>(townPtr)->race][buildingLevel];
            if ((reinterpret_cast<townView *>(townPtr)->buildings &
                 (1 << (buildingLevel + AI_BUILDING_FIRST_DWELLING))) &&
                *reinterpret_cast<short *>(reinterpret_cast<char *>(townPtr) +
                                           0x1e + buildingLevel * 2) > 0 &&
                gMonsterInfo[gDwellingType[reinterpret_cast<townView *>(townPtr)->race]
                                          [building - AI_BUILDING_FIRST_DWELLING]].level <
                    gMonsterInfo[currentCreatureType].level * 1.2) {
                adjustedValue = 0.0f;
                break;
            }
        }
        break;
    }

    LikelihoodOfEnemyAttacking(townPtr, 0, estimatedAttackChance, enemyStrengthLocal,
                                currentAttackTurns, projectedAttackValue,
                                estimatedAttackWeeks, dangerRating);
    adjustedValue = static_cast<float>((1.0 - dangerRating * 3.0) * adjustedValue);
    if (*(reinterpret_cast<signed char *>(townPtr) + 0x39) != giCurPlayer)
        adjustedValue = static_cast<float>(adjustedValue * 0.85);
    if (adjustedValue < 0.0f)
        adjustedValue = 0.0f;
    GetBuildingCost(currentTownRace, building, costsByResource,
                    building == AI_BUILDING_MAGE_GUILD ?
                        reinterpret_cast<townView *>(townPtr)->buildState : 0);
    adjustedValue = FutureDeflator(costsByResource) * adjustedValue;
    resourceValue = static_cast<int>(adjustedValue);
    benefitCost = adjustedValue / RVConversion(costsByResource);
}

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

// @early-stop
// At +0x2f9..+0x30d, cl loads MAP_HEIGHT first and emits `cmp [y], eax; jge`
// (19 bytes); retail loads y first and emits `cmp [MAP_HEIGHT], eax; jle`
// (20 bytes). Both are the same signed `y < MAP_HEIGHT` loop condition. A focused
// /Od /Ob1 probe proved all ordinary for-condition spellings canonicalize to the
// current form; the one if/else spelling that flips the operands adds two non-retail
// routing jumps. The other three diff rows are constant-pool/delinker identities,
// and all 99 relocation sites align (`homm2 relocs`: only-base=0).
VA(0x0043ef45, 0xaf9)
int philAI::StrategicValueOfPosition(int targetX, int targetY, int immediate,
                                     int checkEnemies, int *liveChance,
                                     int extraDistance) {
    int distance15;
    searchArray *search5;
    int eventValue13;
    int eventFlags8;
    int x13;
    int enemyMobility19;
    int seedMobility2;
    int yCounter;
    int friendlyTown1;
    mapCell *cell9;
    int unusedThreatValue9;
    int danger26;
    int heroIndex2;
    int heroTerrain7;
    int shortDistance13;
    int penalty;
    int allocatedSearch2;
    int unusedStrategicValue17;
    int targetTerrain29;
    int score4;

    if (!extraDistance && !immediate &&
        *reinterpret_cast<short *>(reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2) != AI_RV_UNSET) {
        *liveChance = *reinterpret_cast<short *>(
            reinterpret_cast<char *>(gaiLiveChanceOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2);
        return *reinterpret_cast<short *>(
            reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2);
    }

    score4 = 0;
    allocatedSearch2 = 0;
    friendlyTown1 = 0;
    cell9 = gpAdvManager->GetCell(targetX, targetY);
    if (cell9->triggerType == AI_OBJECT_TOWN &&
        reinterpret_cast<townView *>(GetCastleSlot(cell9->w4hi))->owner == giCurPlayer &&
        (reinterpret_cast<townView *>(GetCastleSlot(cell9->w4hi))->buildings & 0x40000000)) {
        friendlyTown1 = 1;
    }
    *liveChance = 100;
    if (checkEnemies && !gaiEnemyHeroReachable[targetY * MAP_WIDTH + targetX]) {
        if (immediate)
            return 0;
        else
            checkEnemies = 0;
    }

    if (bSVSearchArrayInUse) {
        allocatedSearch2 = 1;
        search5 = new searchArray;
        if (!search5)
            MemError();
        search5->Init();
    } else {
        bSVSearchArrayInUse = 1;
        search5 = &SVSearchArray;
    }

    eventFlags8 = gpCurAIHero->m_eventFlags & 0x80;
    if (eventFlags8 && gpAdvManager->GetCell(targetX, targetY)->triggerType == 0x1c)
        eventFlags8 = 0;
    shortDistance13 = 700;
    if (immediate || checkEnemies)
        seedMobility2 = 1700;
    else
        seedMobility2 = shortDistance13;
    search5->SeedPosition(targetX, targetY, 2, seedMobility2, eventFlags8, 0,
                         59999, gpCurAIHero->m_secondarySkills[0], -1, -1, 0, 0);
    search5->GetNode(targetX, targetY | 0).visited = 0;

    for (x13 = 0; MAP_WIDTH > x13; x13++) {
        for (yCounter = 0; MAP_HEIGHT > (yCounter | 0); yCounter++) {
            if (search5->GetNode(x13, yCounter).visited) {
            cell9 = gpAdvManager->GetCell(x13, yCounter);
            if ((!immediate && (cell9->triggerType & 0x80) &&
                 search5->GetNode(x13, yCounter).distance <= shortDistance13) ||
                (checkEnemies && cell9->triggerType == AI_OBJECT_HERO)) {
                CheckDoMain(0, 0);
                eventValue13 = static_cast<int>((ValueOfEventAtPosition(x13, yCounter, 0, &iDummy) * 25) /
                    (search5->GetNode(x13, yCounter).distance + 50.0));
                if (!friendlyTown1 || cell9->triggerType != AI_OBJECT_HERO || eventValue13 >= 0)
                    score4 += eventValue13;
            }

            if (cell9->triggerType == AI_OBJECT_HERO) {
                if (gaiHeroLiveChance[cell9->w4hi] == AI_RV_UNSET)
                    ValueOfEventAtPosition(x13, yCounter, 0, &iDummy);
                if (gaiHeroLiveChance[cell9->w4hi] != AI_RV_UNSET &&
                    gaiHeroLiveChance[cell9->w4hi] < 100) {
                    if (!friendlyTown1) {
                        enemyMobility19 = GetHeroSlot(cell9->w4hi)->m_mobility;
                        if (gbHumanPlayer[gpGame->m_availableHeroes[cell9->w4hi]]) {
                            if (search5->GetNode(x13, yCounter).distance + extraDistance <=
                                enemyMobility19) {
                                if (search5->GetNode(x13, yCounter).distance + extraDistance <= 500) {
                                    danger26 = 100 - gaiHeroLiveChance[cell9->w4hi];
                                } else {
                                    danger26 = ((enemyMobility19 -
                                        (search5->GetNode(x13, yCounter).distance + extraDistance)) + 250) *
                                        (100 - gaiHeroLiveChance[cell9->w4hi]) / enemyMobility19;
                                }
                            } else {
                                danger26 = static_cast<int>((100 - gaiHeroLiveChance[cell9->w4hi]) * 0.9);
                            }
                        } else {
                            danger26 = ((enemyMobility19 + 500) -
                                (search5->GetNode(x13, yCounter).distance + extraDistance)) *
                                (100 - gaiHeroLiveChance[cell9->w4hi]) /
                                (enemyMobility19 + 500);
                        }
                        *liveChance = (100 - danger26) * *liveChance / 100;
                    }
                }
            }

            if (static_cast<unsigned char>(gpCurAIHero->m_id) != iAlphaMale &&
                (targetX != x13 || targetY != yCounter) &&
                search5->GetNode(x13, yCounter).distance < 1024 &&
                gpAdvManager->GetCell(x13, yCounter)->triggerType == AI_OBJECT_HERO &&
                gpAdvManager->GetCell(x13, yCounter)->w4hi !=
                    static_cast<unsigned char>(gpCurAIHero->m_id) &&
                gpGame->m_availableHeroes[gpAdvManager->GetCell(x13, yCounter)->w4hi] ==
                    gpCurAIHero->m_owner) {
                penalty = (1024 - search5->GetNode(x13, yCounter).distance) * 600 >> 10;
                if (penalty > 0)
                    score4 -= penalty;
            }
            }
        }
    }

    targetTerrain29 = giGroundToTerrain[gpAdvManager->GetCell(targetX, targetY)->tile];
    for (heroIndex2 = 0;
         heroIndex2 < reinterpret_cast<pdView *>(gpCurPlayer)->numHeroes;
         heroIndex2++) {
        if (reinterpret_cast<pdView *>(gpCurPlayer)->heroIds[heroIndex2] !=
            static_cast<unsigned char>(gpCurAIHero->m_id)) {
            distance15 = abs(gpGame->m_heroRecs[
                           reinterpret_cast<pdView *>(gpCurPlayer)->heroIds[heroIndex2]].m_x -
                           targetX) +
                       abs(gpGame->m_heroRecs[
                           reinterpret_cast<pdView *>(gpCurPlayer)->heroIds[heroIndex2]].m_y -
                           targetY);
            if (distance15 < 9) {
                heroTerrain7 = giGroundToTerrain[
                    gpAdvManager->GetCell(
                        gpGame->m_heroRecs[
                            reinterpret_cast<pdView *>(gpCurPlayer)->heroIds[heroIndex2]].m_x,
                        gpGame->m_heroRecs[
                            reinterpret_cast<pdView *>(gpCurPlayer)->heroIds[heroIndex2]].m_y)->tile];
                if (targetTerrain29 != 0 || heroTerrain7 <= 0) {
                    if (targetTerrain29 <= 0 || heroTerrain7 != 0) {
                        score4 -= (9 - distance15) * 600 / 9;
                    }
                }
            }
        }
    }

    if (allocatedSearch2)
        delete search5;
    else
        bSVSearchArrayInUse = 0;

    if (*liveChance < 100)
        score4 -= (100 - *liveChance) * 2500 / 100;
    score4 = static_cast<int>(score4 * 1.25f);
    if (score4 > 32000)
        score4 = 32000;
    if (!immediate && !extraDistance) {
        *reinterpret_cast<short *>(reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2) = static_cast<short>(score4);
        *reinterpret_cast<short *>(reinterpret_cast<char *>(gaiLiveChanceOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2) = static_cast<short>(*liveChance);
    }
    return score4;
}

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
    if (gpGame->m_mapHeader.victoryCondition == 3)
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

// @early-stop
// Exact 0xbf8 code span with zero relocation-masked byte differences. All 72
// relocation sites align; gMonsterInfo interior-field names are delinker aliases.
VA(0x0043fed2, 0xbf8)
int philAI::FightValueOfStack(armyGroup *group, hero *heroPtr, int useHero,
                              int useTown, int townId, int useEnemyMods) {
    int armyValue;
    int spellValueMap;
    float unusedModifierRange;
    float enemyRangedModifier27;
    int scoreIndex15;
    int armySlotRecord;
    float enemyFlyingModifier36;
    int stackValueMap;
    float quantityModifierTarget;
    float enemyMeleeModifierIndex;
    int townArcherValueValue;
    town *townRecord19;
    int numArchers16;
    int archerLevel4;
    int statPowerTarget;
    int extraArchersTarget;
    int morale;
    int heroLuckIndex;
    float durationModifier7;
    int spellScoreTotal;
    int spellCount;
    int spellScores[46];
    float spellPowerModifier;
    int numSpellCastsValue;
    int bestSpellScore8;
    int bestSpellIndexTotal;

    armyValue = 0;
    spellValueMap = 0;
    townArcherValueValue = 0;
    enemyRangedModifier27 = 0.9f;
    enemyMeleeModifierIndex = 1.1f;
    enemyFlyingModifier36 = 0.75f;
    unusedModifierRange = 0.1f;
    townRecord19 = 0;

    if (useEnemyMods) {
        if (heroPtr->HasArtifact(0x15) || heroPtr->HasSpell(0x2f) ||
            heroPtr->m_secondarySkills[10]) {
            enemyMeleeModifierIndex = 1.05f;
            enemyFlyingModifier36 = 0.95f;
        }
        if (heroPtr->m_secondarySkills[1] || heroPtr->HasArtifact(0x3f))
            enemyRangedModifier27 = 1.05f;
    }
    if (useTown) {
        townRecord19 = reinterpret_cast<town *>(GetCastleSlot(townId));
        if (!(reinterpret_cast<townView *>(townRecord19)->buildings &
              AI_BUILDING_CASTLE_MASK))
            useTown = 0;
    }

    for (armySlotRecord = 0; armySlotRecord < AI_TOWN_ARMY_SLOTS; armySlotRecord++) {
        if (group->m_creatureTypes[armySlotRecord] != -1) {
            stackValueMap = gMonsterDatabase[group->m_creatureTypes[armySlotRecord]].fightValue *
                         group->m_quantities[armySlotRecord];
            if (useHero) {
                if (group->m_quantities[armySlotRecord] > 500)
                    quantityModifierTarget = 4.4f;
                else if (group->m_quantities[armySlotRecord] > 300)
                    quantityModifierTarget = 3.9f;
                else if (group->m_quantities[armySlotRecord] > 180)
                    quantityModifierTarget = 3.08f;
                else if (group->m_quantities[armySlotRecord] > 140)
                    quantityModifierTarget = 2.42f;
                else if (group->m_quantities[armySlotRecord] > 100)
                    quantityModifierTarget = 2.1f;
                else if (group->m_quantities[armySlotRecord] > 75)
                    quantityModifierTarget = 1.63f;
                else if (group->m_quantities[armySlotRecord] > 50)
                    quantityModifierTarget = 1.4f;
                else if (group->m_quantities[armySlotRecord] > 35)
                    quantityModifierTarget = 1.0f;
                else if (group->m_quantities[armySlotRecord] > 23)
                    quantityModifierTarget = 0.75f;
                else if (group->m_quantities[armySlotRecord] > 16)
                    quantityModifierTarget = 0.5f;
                else if (group->m_quantities[armySlotRecord] > 11)
                    quantityModifierTarget = 0.32f;
                else if (group->m_quantities[armySlotRecord] > 8)
                    quantityModifierTarget = 0.21f;
                else if (group->m_quantities[armySlotRecord] > 5)
                    quantityModifierTarget = 0.0f;
                else if (group->m_quantities[armySlotRecord] > 3)
                    quantityModifierTarget = -0.1f;
                else if (group->m_quantities[armySlotRecord] > 2)
                    quantityModifierTarget = -0.3f;
                else if (group->m_quantities[armySlotRecord] > 2)
                    quantityModifierTarget = -0.4f;
                else
                    quantityModifierTarget = -0.58f;

                if ((gMonsterInfo[group->m_creatureTypes[armySlotRecord]].attributes &
                     MONSTER_ATTRIBUTE_RANGED) ||
                    group->m_creatureTypes[armySlotRecord] == 0x34 ||
                    group->m_creatureTypes[armySlotRecord] == 0x35 ||
                    group->m_creatureTypes[armySlotRecord] == 0x14 ||
                    group->m_creatureTypes[armySlotRecord] == 0x39 ||
                    group->m_creatureTypes[armySlotRecord] == 0x22 ||
                    group->m_creatureTypes[armySlotRecord] == 0x11) {
                    quantityModifierTarget = static_cast<float>(quantityModifierTarget * 0.7);
                } else if (group->m_creatureTypes[armySlotRecord] == 0xe) {
                    quantityModifierTarget = static_cast<float>(quantityModifierTarget * 0.85);
                } else if (group->m_creatureTypes[armySlotRecord] == 0x1f ||
                           group->m_creatureTypes[armySlotRecord] == 0x3b) {
                    quantityModifierTarget = static_cast<float>(quantityModifierTarget * 1.2);
                }
                stackValueMap = static_cast<int>((quantityModifierTarget + 1.0f) * stackValueMap);

                if (useTown) {
                    if (gMonsterInfo[group->m_creatureTypes[armySlotRecord]].attributes &
                        MONSTER_ATTRIBUTE_RANGED)
                        stackValueMap = static_cast<int>(stackValueMap * 1.18);
                    if (gMonsterInfo[group->m_creatureTypes[armySlotRecord]].attributes &
                        MONSTER_ATTRIBUTE_FLYING)
                        stackValueMap = static_cast<int>(stackValueMap * 0.95);
                }
                if ((gMonsterInfo[group->m_creatureTypes[armySlotRecord]].attributes &
                     MONSTER_ATTRIBUTE_RANGED) && heroPtr &&
                    heroPtr->m_secondarySkills[1]) {
                    stackValueMap = static_cast<int>(stackValueMap *
                        gfSSAIArcheryMod[heroPtr->m_secondarySkills[1]]);
                }
                if (useEnemyMods) {
                    if (gMonsterInfo[group->m_creatureTypes[armySlotRecord]].attributes &
                        MONSTER_ATTRIBUTE_RANGED)
                        stackValueMap = static_cast<int>(stackValueMap * enemyRangedModifier27);
                    if (gMonsterInfo[group->m_creatureTypes[armySlotRecord]].attributes &
                        MONSTER_ATTRIBUTE_FLYING)
                        stackValueMap = static_cast<int>(stackValueMap * enemyMeleeModifierIndex);
                    else
                        stackValueMap = static_cast<int>(stackValueMap * enemyFlyingModifier36);
                }
            }
            armyValue += stackValueMap;
        }
    }

    if (useTown) {
        extraArchersTarget = 0;
        townRecord19->CalcNumLevelArchers(&numArchers16, &archerLevel4);
        if (reinterpret_cast<townView *>(townRecord19)->buildings &
            AI_BUILDING_LEFT_TURRET_MASK)
            extraArchersTarget += numArchers16 >> 1;
        if (reinterpret_cast<townView *>(townRecord19)->buildings &
            AI_BUILDING_RIGHT_TURRET_MASK)
            extraArchersTarget += numArchers16 >> 1;
        numArchers16 += extraArchersTarget;
        townArcherValueValue = static_cast<int>((archerLevel4 * 0.1 + 1.0) *
                                           (numArchers16 * 400));
    }

    if (useHero && heroPtr) {
        statPowerTarget = heroPtr->Stats(0) + heroPtr->Stats(1) + 20;
        if (statPowerTarget < 0)
            statPowerTarget = 0;
        if (statPowerTarget > 40)
            statPowerTarget = 40;
        armyValue = static_cast<int>(armyValue * gfStatPower[statPowerTarget]);
        townArcherValueValue = static_cast<int>(townArcherValueValue * gfStatPower[statPowerTarget]);

        morale = heroPtr->m_army.GetMorale(heroPtr, heroPtr->GetOccupiedTown(), 0);
        if (morale > 0)
            armyValue = (morale + 48) * armyValue / 48;
        else if (morale < 0)
            armyValue = (morale + 24) * armyValue / 24;

        heroLuckIndex = gpGame->GetLuck(heroPtr, 0, useTown ? townRecord19 : 0);
        if (heroLuckIndex)
            armyValue = (heroLuckIndex + 16) * armyValue / 16;

        if (heroPtr->m_spellPoints >= 3) {
            spellCount = 0;
            if (heroPtr->Stats(2) <= 10) {
                durationModifier7 = gfPhilAIDurationMod[heroPtr->Stats(2)];
                spellPowerModifier = gfPhilAISpellPowerMod[heroPtr->Stats(2)];
            } else {
                durationModifier7 = gfPhilAIDurationMod[10];
                spellPowerModifier = gfPhilAISpellPowerMod[10];
            }
            for (armySlotRecord = 0; armySlotRecord < 46; armySlotRecord++) {
                if (heroPtr->HasSpell(armySlotRecord)) {
                    spellScoreTotal = gsSpellInfo[armySlotRecord].aiValue;
                    if (gsSpellInfo[armySlotRecord].m_e & 8)
                        spellScoreTotal = static_cast<int>(spellScoreTotal * durationModifier7);
                    else if (gsSpellInfo[armySlotRecord].m_e & 1)
                        spellScoreTotal = static_cast<int>(spellScoreTotal * spellPowerModifier);
                    numSpellCastsValue = heroPtr->m_spellPoints / GetManaCost(armySlotRecord, heroPtr);
                    if (numSpellCastsValue > 10)
                        numSpellCastsValue = 10;
                    spellScoreTotal = static_cast<int>(spellScoreTotal * gfSpellCastNumMod[numSpellCastsValue]);
                    if (spellScoreTotal <= 0)
                        spellScoreTotal = 1;
                    spellScores[spellCount] = spellScoreTotal;
                    spellCount++;
                }
            }
            for (armySlotRecord = 0; armySlotRecord < 7; armySlotRecord++) {
                if ((armySlotRecord | 0) >= spellCount) {
                    break;
                }
                bestSpellScore8 = 0;
                for (scoreIndex15 = 0; (spellCount | 0) > scoreIndex15; scoreIndex15++) {
                    if (spellScores[scoreIndex15] > bestSpellScore8) {
                        bestSpellScore8 = spellScores[scoreIndex15];
                        bestSpellIndexTotal = scoreIndex15;
                    }
                }
                spellValueMap = static_cast<int>(spellValueMap +
                    bestSpellScore8 * gfSpellTypeNumMod[armySlotRecord]);
                spellScores[bestSpellIndexTotal] = 0;
            }
        }
    }

    if (armyValue * 2 < spellValueMap)
        spellValueMap = static_cast<int>(armyValue * 1.25);
    else if (spellValueMap > armyValue * 1.5) {
        spellValueMap = armyValue;
    } else if ((armyValue | 0) < spellValueMap)
        spellValueMap = static_cast<int>(armyValue * 0.75);
    if (armyValue * 2 < townArcherValueValue)
        townArcherValueValue = static_cast<int>(armyValue * 1.5);
    else if (townArcherValueValue > armyValue * 1.5) {
        townArcherValueValue = static_cast<int>(armyValue * 0.9);
    } else if (armyValue < townArcherValueValue)
        townArcherValueValue = static_cast<int>(armyValue * 1.25);
    if (giDebugLevel == 9)
        LogInt("Fight Value", armyValue, spellValueMap, townArcherValueValue,
               0, 0, -999, -999);
    armyValue += spellValueMap;
    armyValue += townArcherValueValue;
    return armyValue;
}

// @early-stop
// Relocation-masked bytes are exact across all 0x1e7 bytes, and all 14
// relocation sites and targets agree.
VA(0x00440aca, 0x1e7)
void philAI::EvaluateOneTimeCreaturePurchase(int creature, int availableCount,
                                             int useAvailableCount,
                                             int &purchaseCount, int &purchaseValue,
                                             int &replacementSlot)
{
    int replacementStackValue7;
    int leastStackValue3;
    int purchaseFightValue15;
    int armyIndex3;

    purchaseCount = 0;
    purchaseValue = 0;
    replacementSlot = AI_CREATURE_PURCHASE_NO_SLOT;
    leastStackValue3 = AI_CREATURE_PURCHASE_VALUE_LIMIT;
    if (useAvailableCount != 0)
        purchaseCount = availableCount;
    else
        purchaseCount = MaxBuyableCreatures(creature);
    if (purchaseCount > availableCount)
        purchaseCount = availableCount;

    if (purchaseCount == 0)
        return;
    {
        purchaseFightValue15 =
            gMonsterDatabase[creature].fightValue * purchaseCount;
        if (gpCurAIHero->m_army.CanJoin(creature) == 0) {
            for (armyIndex3 = 0;
                 armyIndex3 < AI_CREATURE_PURCHASE_ARMY_SLOT_COUNT;
                 armyIndex3++) {
                if (gpCurAIHero->m_army.m_creatureTypes[armyIndex3] == creature) {
                    replacementSlot = AI_CREATURE_PURCHASE_NO_SLOT;
                    armyIndex3 = AI_CREATURE_PURCHASE_ARMY_SLOT_COUNT;
                } else {
                    replacementStackValue7 =
                        gpCurAIHero->m_army.m_quantities[armyIndex3] *
                        gMonsterDatabase[armyIndex3].fightValue;
                    if (replacementStackValue7 < leastStackValue3) {
                        leastStackValue3 = replacementStackValue7;
                        replacementSlot = armyIndex3;
                    }
                }
            }
        }
        if (replacementSlot != AI_CREATURE_PURCHASE_NO_SLOT)
            purchaseFightValue15 -= leastStackValue3;

        purchaseValue = static_cast<int>(
            purchaseFightValue15 *
            reinterpret_cast<pdView *>(
                &gpGame->m_players[gpCurAIHero->m_owner])->upgradeFactor);
        if (useAvailableCount == 0) {
            GetMonsterCost(creature, costTemp);
            purchaseValue -= RVConversion(costTemp) * purchaseCount;
        }
        if (purchaseValue < 0) {
            purchaseValue = 0;
            purchaseCount = 0;
        }
    }
}

// @early-stop
// Relocation-masked bytes are exact across all 0x768 bytes, and all 85
// relocation sites and targets agree.
VA(0x00440cb1, 0x768)
int philAI::QuickCombat(armyGroup *attacker, hero *attackerHero,
                        armyGroup *defender, hero *defenderHero,
                        int townBattle, int townId,
                        float &attackerDamage, float &defenderDamage)
{
    int attackerTroopCount5;
    hero *victoriousHero4;
    armyGroup *selectedGroup36;
    int unusedOutcomeA6;
    int attackerLoss5;
    int attackerRemaining7;
    int defenderLoss8;
    int attackerWon2;
    int defenderRemaining19;
    int armyIndex0;
    int unusedOutcomeB5;
    int attackerExperience37;
    int defenderExperience4;
    float adjustedDifference1;
    int defenderTroopCount1;
    int outcomeValue29;
    hero *defeatedHero5;
    float winChance37;
    int necromancyCount6;
    float casualtyFraction17;
    int defenderOwner1;
    float winnerChance0;
    float rollDifference8;

    attackerExperience37 =
        gpGame->ExperienceValueOfStack(attacker, attackerHero);
    if (townBattle != 0)
        attackerExperience37 += AI_QUICK_COMBAT_TOWN_EXPERIENCE;
    defenderExperience4 =
        gpGame->ExperienceValueOfStack(defender, defenderHero);
    attackerExperience37 = static_cast<int>(
        (gpGame->m_difficulty * AI_QUICK_COMBAT_EXPERIENCE_DIFFICULTY_STEP +
         1.0) * attackerExperience37);
    defenderExperience4 = static_cast<int>(
        (gpGame->m_difficulty * AI_QUICK_COMBAT_EXPERIENCE_DIFFICULTY_STEP +
         1.0) * defenderExperience4);

    attackerWon2 = 0;
    selectedGroup36 = 0;
    if (defenderHero != 0)
        defenderOwner1 = defenderHero->m_owner;
    else
        defenderOwner1 = AI_BATTLE_NO_PLAYER;
    ProbableOutcomeOfBattle(attacker, attackerHero, defender, defenderHero, 0,
                            townBattle, townId, defenderOwner1, winChance37,
                            attackerLoss5, defenderLoss8, attackerRemaining7,
                            defenderRemaining19, outcomeValue29);

    float randomRoll8 = static_cast<float>(
        Random(0, AI_QUICK_COMBAT_RANDOM_LIMIT) /
        static_cast<double>(AI_QUICK_COMBAT_RANDOM_LIMIT));
    if (randomRoll8 < winChance37) {
        attackerWon2 = 1;
        winnerChance0 = winChance37;
        selectedGroup36 = attacker;
    } else {
        winnerChance0 = AI_BATTLE_FULL_CHANCE - winChance37;
        selectedGroup36 = defender;
    }

    if (winChance37 < randomRoll8)
        rollDifference8 = randomRoll8 - winChance37;
    else
        rollDifference8 = winChance37 - randomRoll8;
    adjustedDifference1 = rollDifference8;
    if (attackerWon2 != 0 &&
        winChance37 > AI_QUICK_COMBAT_WIN_BONUS_THRESHOLD)
        adjustedDifference1 = static_cast<float>(
            (winChance37 + AI_QUICK_COMBAT_WIN_BONUS) * adjustedDifference1);
    if (adjustedDifference1 > AI_BATTLE_FULL_CHANCE)
        adjustedDifference1 = AI_BATTLE_FULL_CHANCE;

    casualtyFraction17 = static_cast<float>(
        (1.0 - adjustedDifference1) * (1.0 - adjustedDifference1));
    if (winnerChance0 > AI_QUICK_COMBAT_HIGH_WIN_CHANCE &&
        casualtyFraction17 > AI_QUICK_COMBAT_MINIMUM_CASUALTY)
        casualtyFraction17 *= casualtyFraction17;
    if (winnerChance0 > AI_QUICK_COMBAT_CASUALTY_CAP_CHANCE &&
        casualtyFraction17 >
            (AI_BATTLE_FULL_CHANCE - winnerChance0) /
                AI_QUICK_COMBAT_CASUALTY_DIVISOR)
        casualtyFraction17 =
            (AI_BATTLE_FULL_CHANCE - winnerChance0) /
            AI_QUICK_COMBAT_CASUALTY_DIVISOR;
    if (casualtyFraction17 > AI_QUICK_COMBAT_DAMAGE_PENALTY_LOW &&
        casualtyFraction17 < AI_QUICK_COMBAT_DAMAGE_PENALTY_HIGH)
        casualtyFraction17 = static_cast<float>(
            casualtyFraction17 - AI_QUICK_COMBAT_DAMAGE_PENALTY);

    if (attackerWon2 != 0) {
        if (attackerHero != 0) {
            gpAdvManager->GiveExperience(attackerHero, defenderExperience4, 1);
            attackerHero->ApplyBattleWinTemps();
        }
        attackerHero->CheckLevel();
        defenderDamage = AI_BATTLE_FULL_CHANCE;
        attackerDamage = casualtyFraction17;
    } else {
        if (attackerHero != 0) {
            attackerHero->m_remainingMobility = 0;
            attackerHero->ApplyBattleLossTemps();
        }
        if (defenderHero != 0)
            attackerHero->ApplyBattleWinTemps();
        defenderDamage = casualtyFraction17;
        attackerDamage = AI_BATTLE_FULL_CHANCE;
        if (attackerDamage >= AI_QUICK_COMBAT_DEFEAT_THRESHOLD &&
            defenderHero != 0) {
            gpAdvManager->GiveExperience(defenderHero, defenderExperience4, 1);
            defenderHero->CheckLevel();
        }
    }

    attackerTroopCount5 = 0;
    defenderTroopCount1 = 0;
    for (armyIndex0 = 0;
         armyIndex0 < AI_CREATURE_PURCHASE_ARMY_SLOT_COUNT;
         armyIndex0++) {
        if (attackerHero->m_army.m_creatureTypes[armyIndex0] !=
            AI_CREATURE_PURCHASE_NO_SLOT)
            attackerTroopCount5 +=
                attackerHero->m_army.m_quantities[armyIndex0];
        if (defenderHero != 0 &&
            defenderHero->m_army.m_creatureTypes[armyIndex0] !=
                AI_CREATURE_PURCHASE_NO_SLOT)
            defenderTroopCount1 +=
                defenderHero->m_army.m_quantities[armyIndex0];
    }

    gbRetreatWin = 0;
    if ((attackerWon2 == 0 || defenderHero != 0) &&
        Random(0, AI_QUICK_COMBAT_RANDOM_LIMIT) <
            AI_QUICK_COMBAT_RETREAT_CHANCE)
        gbRetreatWin = 1;
    if (gbRetreatWin == 0) {
        if (attackerDamage > AI_QUICK_COMBAT_DEFEAT_THRESHOLD)
            gpAdvManager->TransferArtifacts(attackerHero, defenderHero);
        else if (defenderDamage > AI_QUICK_COMBAT_DEFEAT_THRESHOLD)
            gpAdvManager->TransferArtifacts(defenderHero, attackerHero);
    }

    DamageGroup(attacker, attackerHero, defenderHero, attackerDamage);
    DamageGroup(defender, defenderHero, attackerHero, defenderDamage);

    if (attackerWon2 != 0 &&
        attackerHero->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
        necromancyCount6 = static_cast<int>(
            static_cast<float>(defenderTroopCount1) *
            static_cast<float>(
                attackerHero->GetSSLevel(HERO_SKILL_NECROMANCY)) *
            AI_QUICK_COMBAT_NECROMANCY_FACTOR);
        if (necromancyCount6 <= 0)
            necromancyCount6 = 1;
        attackerHero->m_army.Add(AI_CREATURE_SKELETON,
                                 necromancyCount6,
                                 AI_CREATURE_PURCHASE_NO_SLOT);
    } else if (defenderHero != 0 &&
               defenderDamage <= AI_QUICK_COMBAT_NECROMANCY_THRESHOLD &&
               defenderHero->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
        necromancyCount6 = static_cast<int>(
            static_cast<float>(attackerTroopCount5) *
            static_cast<float>(
                defenderHero->GetSSLevel(HERO_SKILL_NECROMANCY)) *
            AI_QUICK_COMBAT_NECROMANCY_FACTOR);
        if (necromancyCount6 <= 0)
            necromancyCount6 = 1;
        defenderHero->m_army.Add(AI_CREATURE_SKELETON,
                                 necromancyCount6,
                                 AI_CREATURE_PURCHASE_NO_SLOT);
    }

    defeatedHero5 = 0;
    victoriousHero4 = 0;
    if (attackerWon2 != 0) {
        defeatedHero5 = defenderHero;
        victoriousHero4 = attackerHero;
    } else if (defenderDamage <= AI_QUICK_COMBAT_NECROMANCY_THRESHOLD) {
        defeatedHero5 = attackerHero;
        victoriousHero4 = defenderHero;
    }
    if (defeatedHero5 != 0 &&
        defeatedHero5->m_secondarySkills[HERO_SKILL_EAGLE_EYE] != 0 &&
        victoriousHero4 != 0) {
        for (armyIndex0 = 0; armyIndex0 < AI_QUICK_COMBAT_SPELL_COUNT;
             armyIndex0++) {
            if (defeatedHero5->HasSpell(armyIndex0) != 0 &&
                victoriousHero4->HasSpell(armyIndex0) == 0 &&
                gsSpellInfo[armyIndex0].level <=
                    victoriousHero4->m_secondarySkills[HERO_SKILL_EAGLE_EYE] + 1 &&
                (gsSpellInfo[armyIndex0].m_e &
                 AI_QUICK_COMBAT_LEARNABLE_SPELL)) {
                victoriousHero4->m_spells[armyIndex0] = 1;
                break;
            }
        }
    }

    attackerHero->ApplyBattleWinTemps();
    if (defenderHero != 0)
        defenderHero->ApplyBattleWinTemps();
    if (attackerWon2 != 0 && townBattle != 0)
        gpGame->ClaimTown(townId, giCurPlayer, 0);
    gbRetreatWin = 0;
    return attackerWon2;
}

VA(0x00441419, 0x422)
void philAI::HeroInteractionAtHero(hero *firstHero, hero *secondHero,
                                   int evaluateOnly, int *value) {
    hero *dominantHero19;
    float armyShare0, transferFraction35, dominantShare19, desiredShare6,
        shareDelta1;
    int interactionValue8;
    int artifactType15;
    int dominantFightValue4;
    int heroIndex9;
    int heroValues27[AI_HERO_INTERACTION_HERO_COUNT];
    hero *recipientHero36;
    int statIndex8;
    int recipientFightValue10;
    hero *currentHero9;
    hero *savedHero9;

    *value = 0;
    if (evaluateOnly != 0 &&
        static_cast<unsigned char>(firstHero->m_id) != iAlphaMale &&
        static_cast<unsigned char>(secondHero->m_id) != iAlphaMale)
        return;
    {
        if (evaluateOnly == 0) {
            firstHero->m_lastHeroInteractionTurn =
                static_cast<short>(giCurTurn);
            firstHero->m_lastInteractionHeroId = secondHero->m_id;
            secondHero->m_lastHeroInteractionTurn =
                static_cast<short>(giCurTurn);
            secondHero->m_lastInteractionHeroId = firstHero->m_id;
        }

        for (heroIndex9 = 0;
             heroIndex9 < AI_HERO_INTERACTION_HERO_COUNT;
             heroIndex9++) {
            if (heroIndex9 == 0)
                currentHero9 = firstHero;
            else
                currentHero9 = secondHero;
            heroValues27[heroIndex9] = 0;
            for (statIndex8 = 0;
                 statIndex8 < AI_HERO_INTERACTION_PRIMARY_STAT_COUNT;
                 statIndex8++) {
                if (statIndex8 < 2 ||
                    currentHero9->HasArtifact(AI_ARTIFACT_MAGIC_BOOK))
                    heroValues27[heroIndex9] +=
                        currentHero9->Stats(statIndex8) *
                        AI_HERO_INTERACTION_PRIMARY_STAT_VALUE;
            }
            for (statIndex8 = 0;
                 statIndex8 < AI_BATTLE_ARTIFACT_SLOT_COUNT;
                 statIndex8++) {
                if (statIndex8 == HERO_SKILL_ESTATES)
                    continue;
                if (currentHero9->m_secondarySkills[statIndex8] !=
                    HERO_SKILL_LEVEL_NONE) {
                    heroValues27[heroIndex9] +=
                        gSecondarySkillRV[statIndex8]
                            [currentHero9->m_secondarySkills[statIndex8]];
                }
            }
        }

        dominantShare19 = static_cast<float>(heroValues27[0]) /
            static_cast<float>(heroValues27[0] + heroValues27[1]);
        if (heroValues27[1] < heroValues27[0]) {
            dominantHero19 = firstHero;
            recipientHero36 = secondHero;
        } else {
            dominantHero19 = secondHero;
            recipientHero36 = firstHero;
            dominantShare19 = static_cast<float>(1.0 - dominantShare19);
        }
        if (evaluateOnly != 0 &&
            static_cast<unsigned char>(recipientHero36->m_id) == iAlphaMale &&
            dominantShare19 < AI_HERO_INTERACTION_ALPHA_SHARE) {
            // Preserve retail's assignment order: the saved pointer is not read.
            savedHero9 = recipientHero36;
            recipientHero36 = dominantHero19;
            dominantHero19 = recipientHero36;
        }

        dominantFightValue4 = FightValueOfStack(
            &dominantHero19->m_army, 0, 0, 0, 0, 0);
        recipientFightValue10 = FightValueOfStack(
            &recipientHero36->m_army, 0, 0, 0, 0, 0);
        desiredShare6 = AI_HERO_INTERACTION_TARGET_SHARE;
        armyShare0 = static_cast<float>(dominantFightValue4) /
            static_cast<float>(dominantFightValue4 + recipientFightValue10);
        if (armyShare0 <= desiredShare6)
            shareDelta1 = desiredShare6 - armyShare0;
        else
            shareDelta1 = armyShare0 - desiredShare6;
        transferFraction35 = shareDelta1;
        if (transferFraction35 < AI_HERO_INTERACTION_MINIMUM_TRANSFER)
            return;
        {
            interactionValue8 = static_cast<int>(
                reinterpret_cast<pdView *>(gpCurPlayer)->upgradeFactor *
                (static_cast<float>(dominantFightValue4 +
                                    recipientFightValue10) *
                 transferFraction35));
            if (evaluateOnly != 0) {
                for (statIndex8 = 0;
                     statIndex8 < AI_BATTLE_ARTIFACT_SLOT_COUNT;
                     statIndex8++) {
                    artifactType15 = recipientHero36->m_artifacts[statIndex8];
                    if (artifactType15 != -1 &&
                        artifactType15 != AI_ARTIFACT_MAGIC_BOOK)
                        interactionValue8 += gArtifactBaseRV[artifactType15];
                }
            } else {
                gpAdvManager->TransferArtifacts(recipientHero36,
                                                dominantHero19);
                if (!(armyShare0 <= desiredShare6)) {
                    RedistributeTroops(
                        &dominantHero19->m_army,
                        &recipientHero36->m_army, 1, 1,
                        dominantFightValue4, recipientFightValue10,
                        static_cast<int>(
                            static_cast<float>(dominantFightValue4 +
                                               recipientFightValue10) *
                            transferFraction35));
                } else {
                    RedistributeTroops(
                        &recipientHero36->m_army,
                        &dominantHero19->m_army, 1, 1,
                        recipientFightValue10, dominantFightValue4,
                        static_cast<int>(
                            static_cast<float>(dominantFightValue4 +
                                               recipientFightValue10) *
                            transferFraction35));
                }
                SetupRelativeHeroStrengths();
            }
            *value = static_cast<int>(interactionValue8 *
                                      AI_HERO_INTERACTION_VALUE_FACTOR);
        }
    }
}

// @early-stop
// Exact 0x882 span and 0x78 frame; all 71 relocation sites and targets agree. The
// 25 relocation-masked residual bytes are only commutative load order at
// +0x45..+0x64, +0x139..+0x158, and +0x37d..+0x380; equivalent x87 comparison
// polarity at +0x554..+0x576 and +0x5b9..+0x5c3; and local continuation routing
// at +0x593..+0x594, +0x5ac, and +0x780.
VA(0x0044183b, 0x882)
void philAI::HeroInteractionAtTown(hero *heroPtr, town *townPtr, int doInteraction,
                                  int *value) {
    int heroStrength;
    int transferCount6;
    int estimatedTransfer16;
    int spellLevel14;
    int townWins2;
    int secondStrength0;
    int primarySkills3;
    int spellIndex;
    float transferShare9;
    armyGroup *firstArmy0;
    armyGroup *secondArmy8;
    float townShare5;
    int firstStrength3;
    int townStrength6;
    int otherIndex9;
    int index7;
    int canMerge2;
    int transferValue;
    float desiredShare0;
    float transferCurve;
    float shareDifference7;
    int spellMultiplier1;
    float transferFactor;

    *value = 0;
    if (doInteraction != 0) {
        if ((reinterpret_cast<townView *>(townPtr)->buildings & AI_BUILDING_SHIPYARD_MASK) &&
            reinterpret_cast<townView *>(townPtr)->id != giBestShipyardId) {
            index7 = abs(reinterpret_cast<townView *>(townPtr)->castleY - heroPtr->m_y) +
                     abs(reinterpret_cast<townView *>(townPtr)->castleX - heroPtr->m_x);
            if (gbActualShipyardFound) {
                if (giBestShipyardDist > index7) {
                    giBestShipyardDist = index7;
                    giBestShipyardId = reinterpret_cast<townView *>(townPtr)->id;
                }
            } else {
                giBestShipyardDist = index7;
                giBestShipyardId = reinterpret_cast<townView *>(townPtr)->id;
            }
            gbPossibleShipyardFound = 1;
            gbActualShipyardFound = 1;
        } else if ((reinterpret_cast<townView *>(townPtr)->buildings & AI_BUILDING_CASTLE_MASK) &&
                   giGroundToTerrain[gpAdvManager->GetCell(
                       reinterpret_cast<townView *>(townPtr)->castleX - 1,
                       reinterpret_cast<townView *>(townPtr)->castleY + 1)->tile] == 0 &&
                   !gbActualShipyardFound &&
                   reinterpret_cast<townView *>(townPtr)->id != giBestShipyardId) {
            index7 = abs(reinterpret_cast<townView *>(townPtr)->castleY - heroPtr->m_y) +
                     abs(reinterpret_cast<townView *>(townPtr)->castleX - heroPtr->m_x);
            if (gbPossibleShipyardFound) {
                if (giBestShipyardDist > index7) {
                    giBestShipyardDist = index7;
                    giBestShipyardId = reinterpret_cast<townView *>(townPtr)->id;
                }
            } else {
                giBestShipyardDist = index7;
                giBestShipyardId = reinterpret_cast<townView *>(townPtr)->id;
            }
            gbPossibleShipyardFound = 1;
        }
    } else {
        heroPtr->m_lastTownInteractionTurn = static_cast<short>(giCurTurn);
        heroPtr->m_visitedTownId = static_cast<unsigned char>(
            reinterpret_cast<townView *>(townPtr)->id);
        if (!heroPtr->HasArtifact(AI_ARTIFACT_MAGIC_BOOK) &&
            (reinterpret_cast<townView *>(townPtr)->buildings & AI_BUILDING_MAGE_GUILD_MASK)) {
            if (reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] >=
                AI_MAGIC_BOOK_COST) {
                GiveArtifact(heroPtr, AI_ARTIFACT_MAGIC_BOOK, 1, -1);
                reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] -=
                    AI_MAGIC_BOOK_COST;
            } else {
                heroPtr->m_remainingMobility = 0;
            }
        }
        if ((reinterpret_cast<townView *>(townPtr)->buildings & AI_BUILDING_MAGE_GUILD_MASK) &&
            heroPtr->HasArtifact(AI_ARTIFACT_MAGIC_BOOK) &&
            heroPtr->m_spellPoints < heroPtr->Stats(3) * AI_MANA_PER_KNOWLEDGE) {
            heroPtr->m_remainingMobility = 0;
        }
    }

    if ((reinterpret_cast<townView *>(townPtr)->buildings & AI_BUILDING_MAGE_GUILD_MASK) &&
        (doInteraction != 0 || heroPtr->HasArtifact(AI_ARTIFACT_MAGIC_BOOK))) {
        *value += ManaRefreshValue(heroPtr, 1);
        for (spellLevel14 = 1;
             spellLevel14 <= heroPtr->m_secondarySkills[HERO_SKILL_WISDOM] + 2;
             spellLevel14++) {
            for (spellIndex = 0;
                spellIndex < reinterpret_cast<townView *>(townPtr)
                                  ->mageGuildSpellCounts[spellLevel14 - 1];
                 spellIndex++) {
                if (!heroPtr->HasSpell(reinterpret_cast<townView *>(townPtr)
                                           ->mageGuildSpells[(spellLevel14 - 1) * 4 + spellIndex])) {
                    if (gsSpellInfo[reinterpret_cast<townView *>(townPtr)
                                        ->mageGuildSpells[(spellLevel14 - 1) * 4 + spellIndex]].m_e & 1)
                        spellMultiplier1 = heroPtr->Stats(3);
                    else
                        spellMultiplier1 = 1;
                    *value += spellMultiplier1 *
                              gsSpellInfo[reinterpret_cast<townView *>(townPtr)
                                              ->mageGuildSpells[(spellLevel14 - 1) * 4 +
                                                  spellIndex]].aiValue;
                }
            }
        }
    }

    heroStrength = FightValueOfStack(&heroPtr->m_army, 0, 0, 0, 0, 0);
    townStrength6 = FightValueOfStack(
        reinterpret_cast<armyGroup *>(reinterpret_cast<char *>(townPtr) + 8),
        0, 0, 0, 0, 0);
    townShare5 = static_cast<float>(townStrength6) /
                 (townStrength6 + (heroStrength | 0));
    primarySkills3 = 0;
    primarySkills3 = heroPtr->Stats(0) + heroPtr->Stats(1);
    if (primarySkills3 > 10)
        primarySkills3 = 10;
    if (reinterpret_cast<townView *>(townPtr)->buildings & AI_BUILDING_CASTLE_MASK)
        desiredShare0 = static_cast<float>(0.55 - primarySkills3 * 0.02);
    else
        desiredShare0 = static_cast<float>(0.33 - primarySkills3 * 0.01);
    if (giCurTurn <= AI_EARLY_TOWN_SHARE_TURN)
        desiredShare0 = fFirstWeekTownFV;
    else if (giCurTurn <= AI_SECOND_WEEK_END_TURN)
        desiredShare0 = static_cast<float>(desiredShare0 * 0.5);
    else if (giCurTurn <= AI_THIRD_WEEK_END_TURN)
        desiredShare0 = static_cast<float>(desiredShare0 * 0.75);
    if (static_cast<unsigned char>(heroPtr->m_id) == iAlphaMale)
        desiredShare0 = static_cast<float>(desiredShare0 * 0.5);
    if (reinterpret_cast<gameTV *>(gpGame)->f2c6 == 1 &&
        reinterpret_cast<townView *>(townPtr)->castleX ==
            reinterpret_cast<gameTV *>(gpGame)->f2c9 &&
        (reinterpret_cast<gameTV *>(gpGame)->f2d5 | 0) ==
            reinterpret_cast<townView *>(townPtr)->castleY) {
        desiredShare0 = 0.8f;
    }
    if (desiredShare0 < townShare5)
        shareDifference7 = townShare5 - desiredShare0;
    else
        shareDifference7 = desiredShare0 - townShare5;
    transferShare9 = shareDifference7;
    if (desiredShare0 * AI_TOWN_SHARE_DIFFERENCE_FACTOR <= transferShare9) {
      if (transferShare9 < AI_MINIMUM_TOWN_SHARE_DIFFERENCE) {
      } else {
        townWins2 = 0;
        if (desiredShare0 < townShare5)
            townWins2 = 1;
        if (doInteraction != 0) {
            if ((heroStrength | 0) < townStrength6)
                transferFactor = AI_WEAKER_ARMY_TRANSFER_FACTOR;
            else
                transferFactor = AI_STRONGER_ARMY_TRANSFER_FACTOR;
            transferCurve = static_cast<float>(
                transferShare9 + AI_TOWN_TRANSFER_CURVE_OFFSET -
                AI_TOWN_TRANSFER_CURVE_CENTER);
            estimatedTransfer16 = static_cast<int>(
                (townStrength6 + (heroStrength | 0)) *
                ((transferCurve * transferCurve - AI_TOWN_TRANSFER_CURVE_OFFSET) *
                 reinterpret_cast<pdView *>(gpCurPlayer)->upgradeFactor) *
                transferFactor);
            if (estimatedTransfer16 < 0)
                estimatedTransfer16 = 0;
            canMerge2 = 0;
            if (townWins2) {
                for (index7 = 0; index7 < AI_TOWN_ARMY_SLOTS; index7++) {
                    if (heroPtr->m_army.m_quantities[index7] <= 0)
                        canMerge2 = 1;
                }
            } else {
                for (index7 = 0; index7 < AI_TOWN_ARMY_SLOTS; index7++) {
                    if (reinterpret_cast<townView *>(townPtr)->army.quantities[index7] <= 0)
                        canMerge2 = 1;
                }
            }
            if (!canMerge2) {
                for (index7 = 0; index7 < AI_TOWN_ARMY_SLOTS; index7++) {
                    for (otherIndex9 = 0; otherIndex9 < AI_TOWN_ARMY_SLOTS; otherIndex9++) {
                        if ((heroPtr->m_army.m_creatureTypes[otherIndex9] | 0) ==
                            reinterpret_cast<townView *>(townPtr)->army.creatureTypes[index7]) {
                            canMerge2 = 1;
                            break;
                        }
                    }
                }
            }
            if (!canMerge2)
                estimatedTransfer16 = 0;
            *value += estimatedTransfer16;
            if (reinterpret_cast<townView *>(townPtr)->threat != 0 &&
                reinterpret_cast<townView *>(townPtr)->visitingHero == -1)
                *value += AI_UNGUARDED_TOWN_VALUE;
        } else {
            townPtr->GiveSpells(0);
            if (townWins2)
                transferShare9 = static_cast<float>(transferShare9 + AI_TOWN_TRANSFER_BONUS);
            transferCount6 = static_cast<int>(
                (townStrength6 + (heroStrength | 0)) * transferShare9);
            if (townWins2)
                firstArmy0 = reinterpret_cast<armyGroup *>(reinterpret_cast<char *>(townPtr) + 8);
            else
                firstArmy0 = &heroPtr->m_army;
            if (townWins2)
                secondArmy8 = &heroPtr->m_army;
            else
                secondArmy8 = reinterpret_cast<armyGroup *>(reinterpret_cast<char *>(townPtr) + 8);
            if (townWins2) {
                firstStrength3 = townStrength6;
                secondStrength0 = heroStrength;
            } else {
                firstStrength3 = heroStrength;
                secondStrength0 = townStrength6;
            }
            RedistributeTroops(firstArmy0, secondArmy8, !townWins2, townWins2,
                               firstStrength3, secondStrength0, transferCount6);
            if (reinterpret_cast<townView *>(townPtr)->id == giHumanTownConquered &&
                heroPtr->m_remainingMobility <= AI_CONQUERED_HERO_MOBILITY_LIMIT)
                heroPtr->m_remainingMobility = 0;
        }
      }
    }
}

// @early-stop
// Exact 0x4ba span and 0x48 frame; all 19 relocation sites and targets agree.
// The only masked residual is TU-cumulative comparison load order at +0x3b9,
// +0x3bc, and +0x3be (transfer count vs total creature count).
VA(0x004420bd, 0x4ba)
void philAI::RedistributeTroops(armyGroup *sourceArmy, armyGroup *destinationArmy,
                                int preserveOne, int preferFast, int sourceStrength,
                                int destinationStrength, int transferBudget) {
    int totalCreatures0;
    int keepGoing5;
    int sourceIndex15;
    int destinationIndex6;
    int selectedIndex5;
    int bestValue37;
    int stackValue7;
    int selectedSpeed19;
    int transferCount16;

    keepGoing5 = 1;
    gbTroopReload = 0;
    while (keepGoing5) {
        if (preserveOne != 0) {
            totalCreatures0 = 0;
            for (sourceIndex15 = 0; sourceIndex15 < AI_TOWN_ARMY_SLOTS;
                 sourceIndex15++) {
                if (sourceArmy->m_creatureTypes[sourceIndex15] != AI_TROOP_EMPTY_SLOT)
                    totalCreatures0 += sourceArmy->m_quantities[sourceIndex15];
            }
            if (totalCreatures0 <= 1)
                return;
        }

        selectedIndex5 = AI_TROOP_EMPTY_SLOT;
        for (sourceIndex15 = 0; sourceIndex15 < AI_TOWN_ARMY_SLOTS;
             sourceIndex15++) {
            if (selectedIndex5 == AI_TROOP_EMPTY_SLOT) {
                for (destinationIndex6 = 0;
                     destinationIndex6 < AI_TOWN_ARMY_SLOTS;
                     destinationIndex6++) {
                    if (sourceArmy->m_creatureTypes[sourceIndex15] != AI_TROOP_EMPTY_SLOT &&
                        destinationArmy->m_creatureTypes[destinationIndex6] ==
                            sourceArmy->m_creatureTypes[sourceIndex15]) {
                        selectedIndex5 = sourceIndex15;
                        break;
                    }
                }
            }
        }

        if (selectedIndex5 == AI_TROOP_EMPTY_SLOT) {
            bestValue37 = AI_TROOP_REDISTRIBUTION_WORST_VALUE;
            if (preferFast != 0)
                selectedSpeed19 = AI_TROOP_REDISTRIBUTION_MIN_SPEED;
            else
                selectedSpeed19 = AI_TROOP_REDISTRIBUTION_MAX_SPEED;

            for (sourceIndex15 = 0; sourceIndex15 < AI_TOWN_ARMY_SLOTS;
                 sourceIndex15++) {
                if (sourceArmy->m_creatureTypes[sourceIndex15] != AI_TROOP_EMPTY_SLOT) {
                    stackValue7 =
                        gMonsterDatabase[sourceArmy->m_creatureTypes[sourceIndex15]].fightValue *
                        sourceArmy->m_quantities[sourceIndex15];
                    if ((preferFast != 0 &&
                         gMonsterDatabase[sourceArmy->m_creatureTypes[sourceIndex15]].speed >
                             selectedSpeed19) ||
                        (preferFast == 0 &&
                         gMonsterDatabase[sourceArmy->m_creatureTypes[sourceIndex15]].speed <
                             selectedSpeed19)) {
                        selectedSpeed19 =
                            gMonsterDatabase[sourceArmy->m_creatureTypes[sourceIndex15]].speed;
                        bestValue37 = stackValue7;
                        selectedIndex5 = sourceIndex15;
                    } else if (
                        gMonsterDatabase[sourceArmy->m_creatureTypes[sourceIndex15]].speed ==
                            selectedSpeed19 &&
                        stackValue7 > bestValue37) {
                        bestValue37 = stackValue7;
                        selectedIndex5 = sourceIndex15;
                    }
                }
            }
        }

        if (selectedIndex5 == AI_TROOP_EMPTY_SLOT) {
            keepGoing5 = 0;
        } else if (destinationArmy->CanJoin(
                       sourceArmy->m_creatureTypes[selectedIndex5])) {
            transferCount16 = static_cast<int>(
                static_cast<double>(transferBudget) /
                    gMonsterDatabase[sourceArmy->m_creatureTypes[selectedIndex5]].fightValue +
                AI_TROOP_REDISTRIBUTION_ROUNDING);
            if (transferCount16 > 0) {
                if (sourceArmy->m_quantities[selectedIndex5] < transferCount16) {
                    transferCount16 = sourceArmy->m_quantities[selectedIndex5];
                } else {
                    keepGoing5 = 0;
                    if ((sourceArmy->m_quantities[selectedIndex5] *
                                 AI_TROOP_REDISTRIBUTION_STACK_SHARE <=
                             transferCount16 ||
                         sourceArmy->m_quantities[selectedIndex5] - 1 <= transferCount16) &&
                        gMonsterDatabase[sourceArmy->m_creatureTypes[selectedIndex5]].fightValue *
                                (sourceArmy->m_quantities[selectedIndex5] - transferCount16) <
                            (sourceStrength - transferBudget) *
                                AI_TROOP_REDISTRIBUTION_REMAINDER_FACTOR) {
                        transferCount16 = sourceArmy->m_quantities[selectedIndex5];
                    }
                }

                if (preserveOne != 0 && totalCreatures0 <= transferCount16) {
                    transferCount16 = totalCreatures0 - 1;
                    keepGoing5 = 0;
                }

                if (gMonsterDatabase[sourceArmy->m_creatureTypes[selectedIndex5]].fightValue *
                            transferCount16 * AI_TROOP_REDISTRIBUTION_BUDGET_FACTOR >
                        transferBudget) {
                    keepGoing5 = 0;
                } else {
                    transferBudget -=
                        gMonsterDatabase[sourceArmy->m_creatureTypes[selectedIndex5]].fightValue *
                        transferCount16;
                }

                destinationArmy->Add(sourceArmy->m_creatureTypes[selectedIndex5],
                                     transferCount16, AI_TROOP_EMPTY_SLOT);
                sourceArmy->m_quantities[selectedIndex5] -= transferCount16;
                if (sourceArmy->m_quantities[selectedIndex5] == 0)
                    sourceArmy->m_creatureTypes[selectedIndex5] = AI_TROOP_EMPTY_SLOT;
            } else {
                keepGoing5 = 0;
            }
        } else {
            keepGoing5 = 0;
        }
    }
}

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
void philAI::BuildHero(town *townPtr, int availableHeroIndex) {
    int townX37;
    int townY9;
    hero *newHero6;

    sprintf(gText, "Player %d built hero in town %d.", giCurPlayer,
            townPtr->m_id);
    LogStr(gText);
    if (giDebugLevel >= AI_PURCHASE_DEBUG_LEVEL) {
        AiPrint(gText);
        DelayMilli(AI_PURCHASE_DEBUG_DELAY);
    }

    gpCurPlayer->m_resources[RES_GOLD] -= gHeroGoldCost;
    gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] =
        gpCurPlayer->m_availableHeroIds[availableHeroIndex];
    gpCurPlayer->m_heroCount++;

    townX37 = townPtr->m_x;
    townY9 = townPtr->m_y;
    newHero6 = &gpGame->m_heroRecs[
        gpCurPlayer->AvailableHeroId(availableHeroIndex)];
    gpGame->SetRandomHeroArmies(static_cast<unsigned char>(newHero6->m_id), 1);
    newHero6->m_lastHeroInteractionTurn = AI_HERO_BUILD_COORDINATE_UNSET;
    newHero6->m_lastTownInteractionTurn = AI_HERO_BUILD_COORDINATE_UNSET;
    newHero6->m_owner = static_cast<char>(giCurPlayer);
    newHero6->m_x = townX37;
    newHero6->m_y = townY9;
    newHero6->m_eventFlags = 0;
    newHero6->m_direction = AI_HERO_BUILD_DIRECTION;
    newHero6->m_remainingMobility = newHero6->CalcMobility();
    newHero6->m_mobility = newHero6->m_remainingMobility;

    newHero6->m_locationType =
        gpGame->m_worldMap.GetCell(townX37, townY9)->triggerType;
    newHero6->m_occupiedTown =
        gpGame->m_worldMap.GetCell(townX37, townY9)->w4hi;
    gpGame->m_worldMap.GetCell(townX37, townY9)->triggerType = AI_OBJECT_HERO;
    gpGame->m_worldMap.GetCell(townX37, townY9)->w4hi =
        gpCurPlayer->m_availableHeroIds[availableHeroIndex];
    gpGame->m_availableHeroes[static_cast<unsigned char>(newHero6->m_id)] =
        townPtr->m_owner;

    CheckValidAvailableHeroes();
    SendMapChange(AI_HERO_BUILD_MAP_CHANGE, newHero6->m_id,
                  static_cast<unsigned char>(newHero6->m_x),
                  static_cast<unsigned char>(newHero6->m_y),
                  AI_HERO_BUILD_MAP_CHANGE_VALUE, 0, 0);
    townPtr->m_occupyingHeroId = newHero6->m_id;
    townPtr->GiveSpells(0);

    gpCurPlayer->m_availableHeroIds[availableHeroIndex] = static_cast<signed char>(
        gpGame->GetNewHeroId(giCurPlayer, AI_TROOP_EMPTY_SLOT, 1));
    gpGame->m_availableHeroes[
        gpCurPlayer->m_availableHeroIds[availableHeroIndex]] = AI_HERO_AVAILABLE_FLAG;
    bHeroBuiltThisTurn = 1;
    HeroInteractionAtTown(newHero6, townPtr, 0, &iDummy);
    ShowStatus();
}

VA(0x00442be0, 0x2cd)
void philAI::BuildCreature(town *townPtr, int dwelling, int purchaseCount) {
    int canJoin6;
    int weakestSlot8;
    int armyIndex4;
    int creatureType13;
    float stackValue9;
    int monsterCosts10[AI_PURCHASE_RESOURCE_COUNT];
    float weakestValue5;

    sprintf(gText, "Player %d built %d %s in town %d.", giCurPlayer,
            purchaseCount,
            GetMonsterName(gDwellingType[townPtr->m_type][dwelling]),
            townPtr->m_id);
    LogStr(gText);
    if (giDebugLevel >= AI_PURCHASE_DEBUG_LEVEL) {
        AiPrint(gText);
        DelayMilli(AI_PURCHASE_DEBUG_DELAY);
    }

    creatureType13 = gDwellingType[townPtr->m_type][dwelling];
    canJoin6 = 0;
    for (armyIndex4 = 0; armyIndex4 < AI_TOWN_ARMY_SLOTS; armyIndex4++) {
        if (townPtr->m_army.m_creatureTypes[armyIndex4] == AI_TROOP_EMPTY_SLOT ||
            townPtr->m_army.m_creatureTypes[armyIndex4] == creatureType13) {
            canJoin6 = 1;
        }
    }

    weakestValue5 = AI_CREATURE_SELECTION_WORST_VALUE;
    weakestSlot8 = AI_TROOP_EMPTY_SLOT;
    if (canJoin6 == 0) {
        for (armyIndex4 = 0; armyIndex4 < AI_TOWN_ARMY_SLOTS; armyIndex4++) {
            stackValue9 = static_cast<float>(
                gMonsterDatabase[townPtr->m_army.m_creatureTypes[armyIndex4]].fightValue *
                townPtr->m_army.m_quantities[armyIndex4]);
            if (gMonsterDatabase[townPtr->m_army.m_creatureTypes[armyIndex4]].race !=
                townPtr->m_type) {
                stackValue9 = static_cast<float>(
                    stackValue9 * AI_CREATURE_OFF_RACE_FACTOR);
            }
            stackValue9 = static_cast<float>(
                (gMonsterDatabase[townPtr->m_army.m_creatureTypes[armyIndex4]].fightValue +
                 AI_CREATURE_SELECTION_BASE_VALUE) /
                AI_CREATURE_SELECTION_BASE_VALUE * stackValue9);
            if (stackValue9 < weakestValue5) {
                weakestValue5 = stackValue9;
                weakestSlot8 = armyIndex4;
            }
        }
        if (weakestSlot8 == AI_TROOP_EMPTY_SLOT)
            weakestSlot8 = 0;

        gpCurPlayer->m_resources[RES_GOLD] +=
            gMonsterDatabase[townPtr->m_army.m_creatureTypes[weakestSlot8]].cost *
            townPtr->m_army.m_quantities[weakestSlot8];
        townPtr->m_army.m_creatureTypes[weakestSlot8] = AI_TROOP_EMPTY_SLOT;
        townPtr->m_army.m_quantities[weakestSlot8] = 0;
    }

    GetMonsterCost(creatureType13, monsterCosts10);
    for (armyIndex4 = 0; armyIndex4 < AI_PURCHASE_RESOURCE_COUNT; armyIndex4++) {
        gpCurPlayer->m_resources[armyIndex4] -=
            monsterCosts10[armyIndex4] * purchaseCount;
    }
    townPtr->m_garrison[dwelling] -= purchaseCount;
    townPtr->m_army.Add(creatureType13, purchaseCount, AI_TROOP_EMPTY_SLOT);
    ShowStatus();
}

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

// @early-stop
// All 0x6d7 bytes match with the 102 relocation fields masked. The objdiff
// residual is delinker naming for switch-local labels, floating constants, and
// gafAITurnCostResource[RES_GOLD]; every external call/global resolves equally.
VA(0x0044316b, 0x6d7)
int philAI::FightEvent(hero *h, mapCell *cell, int evaluateOnly) {
    int eventType16;
    int monsterType6;
    int monsterCount28;
    int stackIndex7;
    int rewardValue7;
    int battleWon29;
    int battleValue19;
    float attackerLoss5;
    float defenderLoss4;
    int combatResult18;
    int unusedValue8;

    eventType16 = cell->triggerType & 0x7f;
    if (cell->w4hi == AI_FIGHT_EVENT_EMPTY)
        return 0;

    int shipwreckCounts15[AI_FIGHT_EVENT_LEVEL_COUNT] = {
        AI_FIGHT_EVENT_SHIPWRECK_COUNT_1,
        AI_FIGHT_EVENT_SHIPWRECK_COUNT_2,
        AI_FIGHT_EVENT_SHIPWRECK_COUNT_3,
        AI_FIGHT_EVENT_SHIPWRECK_COUNT_4
    };
    int derelictCounts28[AI_FIGHT_EVENT_LEVEL_COUNT] = {
        AI_FIGHT_EVENT_DERELICT_COUNT_1,
        AI_FIGHT_EVENT_DERELICT_COUNT_2,
        AI_FIGHT_EVENT_DERELICT_COUNT_3,
        AI_FIGHT_EVENT_DERELICT_COUNT_4
    };
    int graveyardCounts37[AI_FIGHT_EVENT_LEVEL_COUNT] = {
        AI_FIGHT_EVENT_GRAVEYARD_COUNT_1,
        AI_FIGHT_EVENT_GRAVEYARD_COUNT_2,
        AI_FIGHT_EVENT_GRAVEYARD_COUNT_3,
        AI_FIGHT_EVENT_GRAVEYARD_COUNT_4
    };

    switch (eventType16) {
    case AI_OBJECT_SHIPWRECK:
        monsterType6 = AI_CREATURE_GHOST;
        monsterCount28 = shipwreckCounts15[cell->w4hi - AI_FIGHT_EVENT_LEVEL_OFFSET];
        break;
    case AI_OBJECT_GRAVEYARD:
        monsterType6 = AI_CREATURE_ZOMBIE;
        monsterCount28 = graveyardCounts37[cell->w4hi - AI_FIGHT_EVENT_LEVEL_OFFSET];
        break;
    default:
        monsterType6 = AI_CREATURE_SKELETON;
        monsterCount28 = derelictCounts28[cell->w4hi - AI_FIGHT_EVENT_LEVEL_OFFSET];
        break;
    }

    for (stackIndex7 = 0; stackIndex7 < AI_FIGHT_EVENT_STACKS; stackIndex7++) {
        gpMonGroup->m_creatureTypes[stackIndex7] = static_cast<signed char>(monsterType6);
        gpMonGroup->m_quantities[stackIndex7] = static_cast<short>(monsterCount28);
    }

    rewardValue7 = 0;
    switch (eventType16) {
    case AI_OBJECT_GRAVEYARD:
        switch (cell->w4hi) {
        case AI_FIGHT_EVENT_LEVEL_1:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_500);
            break;
        case AI_FIGHT_EVENT_LEVEL_2:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_1000);
            break;
        case AI_FIGHT_EVENT_LEVEL_3:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_3000);
            break;
        case AI_FIGHT_EVENT_LEVEL_4:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_1000 +
                reinterpret_cast<pdView *>(gpCurPlayer)->artifactValue);
            break;
        }
        break;
    case AI_OBJECT_SHIPWRECK:
        switch (cell->w4hi) {
        case AI_FIGHT_EVENT_LEVEL_1:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_1000);
            break;
        case AI_FIGHT_EVENT_LEVEL_2:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_2000);
            break;
        case AI_FIGHT_EVENT_LEVEL_3:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_5000);
            break;
        case AI_FIGHT_EVENT_LEVEL_4:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_2000 +
                reinterpret_cast<pdView *>(gpCurPlayer)->artifactValue);
            break;
        }
        break;
    case AI_OBJECT_DERELICT_SHIP:
        switch (cell->w4hi) {
        case AI_FIGHT_EVENT_LEVEL_1:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_500);
            break;
        case AI_FIGHT_EVENT_LEVEL_2:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_1000);
            break;
        case AI_FIGHT_EVENT_LEVEL_3:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_2000);
            break;
        case AI_FIGHT_EVENT_LEVEL_4:
            rewardValue7 = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_5000);
            break;
        }
        break;
    }

    ChooseEvaluateBattle(&h->m_army, h, gpMonGroup, 0, 0, 0, rewardValue7,
                         battleWon29, battleValue19);
    if (evaluateOnly != 0)
        return battleValue19;
    if (battleWon29 != 0 &&
        (combatResult18 = QuickCombat(&h->m_army, h, gpMonGroup, 0, 0, 0,
                                      attackerLoss5, defenderLoss4)) != 0) {
        switch (eventType16) {
        case AI_OBJECT_SHIPWRECK:
            switch (cell->w4hi) {
            case AI_FIGHT_EVENT_LEVEL_1:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_1000);
                break;
            case AI_FIGHT_EVENT_LEVEL_2:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_2000);
                break;
            case AI_FIGHT_EVENT_LEVEL_3:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_5000);
                break;
            case AI_FIGHT_EVENT_LEVEL_4:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_2000);
                gpAdvManager->GiveRandomArtifact(h);
                break;
            }
            break;
        case AI_OBJECT_GRAVEYARD:
            switch (cell->w4hi) {
            case AI_FIGHT_EVENT_LEVEL_1:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_500);
                break;
            case AI_FIGHT_EVENT_LEVEL_2:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_1000);
                break;
            case AI_FIGHT_EVENT_LEVEL_3:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_3000);
                break;
            case AI_FIGHT_EVENT_LEVEL_4:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_1000);
                gpAdvManager->GiveRandomArtifact(h);
                break;
            }
            break;
        case AI_OBJECT_DERELICT_SHIP:
            switch (cell->w4hi) {
            case AI_FIGHT_EVENT_LEVEL_1:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_500);
                break;
            case AI_FIGHT_EVENT_LEVEL_2:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_1000);
                break;
            case AI_FIGHT_EVENT_LEVEL_3:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_2000);
                break;
            case AI_FIGHT_EVENT_LEVEL_4:
                gpAdvManager->GiveResource(h, RES_GOLD, AI_FIGHT_EVENT_REWARD_5000);
                break;
            }
            break;
        }
        cell->w4hi = AI_FIGHT_EVENT_EMPTY;
    }
    return 0;
}

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
void philAI::TownEvent(mapCell *cell, hero *h, int x, int y) {
    float attackerLoss6;
    float defenderLoss5;
    int currentPlayer26;
    int quickCombatResult9;
    town *townPtr2;
    int combatResult7;
    hero *defendingHero0;

    townPtr2 = reinterpret_cast<town *>(GetCastleSlot(cell->w4hi));
    currentPlayer26 = giCurPlayer;
    gpAdvManager->DemobilizeCurrHero();

    if (townPtr2->m_owner != giCurPlayer) {
        if (townPtr2->HasGarrison()) {
            if (townPtr2->m_owner < 0 ||
                gbHumanPlayer[townPtr2->m_owner] == 0) {
                quickCombatResult9 = QuickCombat(
                    &h->m_army, h, &townPtr2->m_army, 0,
                    AI_TOWN_EVENT_USE_GARRISON, townPtr2->m_id,
                    defenderLoss5, attackerLoss6);
            } else {
                defendingHero0 = townPtr2->m_occupyingHeroId == AI_TOWN_EVENT_NO_HERO
                    ? 0 : GetHeroSlot(townPtr2->m_occupyingHeroId);

                combatResult7 = gpAdvManager->DoCombat(
                    x, y, h, &h->m_army, townPtr2, defendingHero0,
                    &townPtr2->m_army, x, y, AI_BATTLE_NO_PLAYER,
                    AI_TOWN_EVENT_USE_GARRISON);
                if (combatResult7 == AI_TOWN_EVENT_ATTACKER_WON) {
                    gpGame->ClaimTown(townPtr2->m_id, giCurPlayer, 0);
                    giHumanTownConquered = townPtr2->m_id;
                }
            }
        } else {
            gpGame->ClaimTown(townPtr2->m_id, giCurPlayer, 0);
        }
    }

    if (townPtr2->m_owner == giCurPlayer && h->m_x == x && h->m_y == y) {
        townPtr2->m_occupyingHeroId = gpCurPlayer->CurrentHero();
        h->m_locationType = AI_OBJECT_TOWN;
        h->m_occupiedTown = townPtr2->m_id;
        HeroInteractionAtTown(h, townPtr2, 0, &iDummy);
    }

    gpAdvManager->MobilizeCurrHero(0);
    townPtr2->GiveSpells(0);
}

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

// @early-stop: all 0x271 bytes match after masking 34 aligned COFF relocations.
// Objdiff's residual is symbol identity: table/field addends and compiler constant
// pools resolve to the retail addresses, while jump-table labels delink as this function.
VA(0x00443c54, 0x271)
int philAI::ComputeValueOfSS(hero *h, int skill, int level) {
    int fightValue7;
    int value28;
    int armyIndex4;
    int totalArmyValue2;
    float rangedShare2;
    int rangedArmyValue28;
    int stackValue7;

    value28 = gSSValues[skill][level - AI_SECONDARY_SKILL_LEVEL_OFFSET];
    fightValue7 = FightValueOfStack(&h->m_army, h, 1, 0, 0, 0);
    if (skill != HERO_SKILL_ESTATES) {
        value28 = static_cast<int>(
            (static_cast<float>(fightValue7) /
                 reinterpret_cast<pdView *>(gpCurPlayer)->upgradeFactor /
                 AI_SECONDARY_SKILL_FIGHT_SCALE +
             AI_SECONDARY_SKILL_BASE_FACTOR) * value28);
    }

    switch (skill) {
    case HERO_SKILL_NAVIGATION:
        if (h->m_eventFlags & HERO_EVENT_EMBARKED)
            value28 = static_cast<int>(
                value28 * AI_SECONDARY_SKILL_NAVIGATION_FACTOR);
        break;
    case HERO_SKILL_ARCHERY:
        rangedArmyValue28 = 0;
        totalArmyValue2 = rangedArmyValue28;
        for (armyIndex4 = 0;
             armyIndex4 < AI_SECONDARY_SKILL_ARMY_SLOTS;
             armyIndex4++) {
            if (h->m_army.m_creatureTypes[armyIndex4] != AI_TROOP_EMPTY_SLOT) {
                stackValue7 =
                    gMonsterDatabase[h->m_army.m_creatureTypes[armyIndex4]].fightValue *
                    h->m_army.m_quantities[armyIndex4];
                totalArmyValue2 += stackValue7;
                if (gMonsterDatabase[h->m_army.m_creatureTypes[armyIndex4]]
                        .flags.all & AI_SECONDARY_SKILL_RANGED_ATTRIBUTE) {
                    rangedArmyValue28 += stackValue7;
                }
            }
        }
        rangedShare2 = static_cast<float>(rangedArmyValue28) /
                       static_cast<float>(totalArmyValue2);
        value28 = static_cast<int>(
            (rangedShare2 / AI_SECONDARY_SKILL_ARCHERY_SHARE +
             AI_SECONDARY_SKILL_ARCHERY_BASE) * value28);
        break;
    case HERO_SKILL_WISDOM:
    case HERO_SKILL_MYSTICISM:
        if (!h->HasArtifact(AI_ARTIFACT_MAGIC_BOOK) ||
            h->Stats(HERO_PRIMARY_KNOWLEDGE) <
                AI_SECONDARY_SKILL_MINIMUM_KNOWLEDGE) {
            value28 = static_cast<int>(value28 * AI_SECONDARY_SKILL_BASE_FACTOR);
        }
        break;
    }
    return value28;
}

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

// @early-stop: exact 0x1ac5 size and relocation layout. The only relocation-masked
// byte difference is +0x34d: SIB 0x01 versus retail 0x08. Both encode the same
// scale-1 address, EAX + ECX + 0x634c; field, pointer-first, integer-first, and
// byte-neutral operand spellings all retain the assembler's equivalent encoding.
VA(0x00443fc4, 0x1ac5)
int philAI::ValueOfEventAtPosition(int x, int y, int immediate, int *liveChance) {
    mapCell *cell_k;
    int cellState_m;
    int creatureFlag_k;
    int purchaseCost_j;
    int creaturePurchaseState[9];
    int resource;
    int resourceState_a[3];
    int purchaseValue_n;
    int index_k;
    int amount_j;
    int battleWon_p;
    int rewardState_a[2];
    int eventState_l;
    int value_h;
    int battleValue_b;
    int combatState_k[4];
    int creature;
    int exitY_p;
    int exitX_p;
    int routeLiveChance_e[4];
    int currentValue_h;
    mapCell *otherCell_j;
    int exitValue_i;
    int bestExitValue_l;
    int bestExitY_c;
    int bestExitX_c;
    float battleStatMod_n;
    float spellPowerMod_m;
    float shrinePowerMod_p;
    int resources_e[7];

    if (!immediate &&
        *reinterpret_cast<short *>(reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
                                   (MAP_WIDTH | 0) * y * sizeof(short) + x * sizeof(short)) !=
            AI_RV_UNSET)
        return *reinterpret_cast<short *>(
            reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
            (MAP_WIDTH | 0) * y * sizeof(short) + x * sizeof(short));

    gbReduceByReload = 1;
    gbReduceByBerserk = 1;
    *liveChance = 100;
    value_h = 0;
    cell_k = gpAdvManager->GetCell(x, y);

    if (reinterpret_cast<pdView *>(gpCurPlayer)->routeLength > 15 &&
        reinterpret_cast<pdView *>(gpCurPlayer)->routeX == x &&
        reinterpret_cast<pdView *>(gpCurPlayer)->routeY == y) {
        value_h = (reinterpret_cast<pdView *>(gpCurPlayer)->routeLength - 15) *
                gUltArtifactAvgValue / 100;
    } else if (cell_k->triggerType & 0x80) {
        switch (cell_k->triggerType & 0x7f) {
    case AI_OBJECT_MONSTER:
        value_h = EvaluateMonsterEvent(cell_k->objIndex, cell_k->w4hi, liveChance);
        break;
    case AI_OBJECT_RESOURCE:
        resource = cell_k->objIndex >> 1;
        switch (resource) {
        case RES_GOLD:
            value_h = static_cast<int>(
                gafAITurnCostResource[resource] *
                (cell_k->w4hi * AI_GOLD_RESOURCE_MULTIPLIER));
            break;
        default:
            value_h = static_cast<int>(gafAITurnCostResource[resource] * cell_k->w4hi);
            break;
        }
        break;
    case AI_OBJECT_TREASURE_CHEST:
        value_h = static_cast<int>(gafAITurnCostResource[RES_GOLD] * 1000.0f);
        break;
    case AI_OBJECT_HERO_EVENT:
        value_h = EvaluateHeroEvent(cell_k->w4hi, x, y, immediate, liveChance);
        break;
    case AI_OBJECT_TOWN_EVENT:
        value_h = EvaluateTownEvent(cell_k->w4hi, x, y, immediate, liveChance);
        break;
    case AI_OBJECT_CAMPFIRE:
        value_h = static_cast<int>(
            ((((((gafAITurnCostResource[RES_GEMS] +
                  gafAITurnCostResource[RES_CRYSTAL]) +
                 gafAITurnCostResource[RES_SULFUR]) +
                gafAITurnCostResource[RES_ORE]) +
               gafAITurnCostResource[RES_MERCURY]) +
              gafAITurnCostResource[RES_WOOD]) /
                 AI_CAMPFIRE_AVERAGE_DIVISOR *
             AI_CAMPFIRE_RESOURCE_AMOUNT) +
            gafAITurnCostResource[RES_GOLD] * AI_CAMPFIRE_GOLD_AMOUNT);
        break;
    case AI_OBJECT_ARTIFACT:
        value_h = EvaluateArtifactEvent(cell_k->objIndex >> 1, cell_k->w4hi);
        break;
    case AI_OBJECT_ALCHEMIST_LAB:
    case AI_OBJECT_MINE:
    case AI_OBJECT_SAWMILL:
        value_h = EvaluateMineEvent(cell_k->w4hi, x, y, liveChance);
        break;
    case AI_OBJECT_OBELISK:
        value_h = (giCurPlayerBit &
                   reinterpret_cast<gameObeliskView *>(gpGame)->
                       obeliskFlags[cell_k->w4hi]) ? 0 :
                reinterpret_cast<pdView *>(gpCurPlayer)->obeliskValue;
        break;
    case AI_OBJECT_OASIS:
        if (!(gpCurAIHero->m_eventFlags & 8))
            value_h = static_cast<int>(gpCurAIHero->m_aiFightValue * AI_OASIS_VALUE_FACTOR);
        break;
    case AI_OBJECT_BUOY:
        if (!(gpCurAIHero->m_eventFlags & 2) && giCurAIHeroMorale < 3)
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_MORALE_LUCK_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_TEMPLE:
        if (!(gpCurAIHero->m_eventFlags & 0x100) && giCurAIHeroMorale < 3)
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_MORALE_LUCK_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_FAERIE_RING:
        if (!(gpCurAIHero->m_eventFlags & 0x10) && giCurAIHeroLuck < 3)
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_MORALE_LUCK_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_IDOL:
        if (!(gpCurAIHero->m_eventFlags & 0x2000) && giCurAIHeroLuck < 3)
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_MORALE_LUCK_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_FOUNTAIN:
        if (!(gpCurAIHero->m_eventFlags & 4) && giCurAIHeroLuck < 3)
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_MORALE_LUCK_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_WATERING_HOLE:
        if (!(gpCurAIHero->m_eventFlags & 0x200))
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_WATERING_HOLE_VALUE_FACTOR);
        break;
    case AI_OBJECT_SHRINE_FIRST:
    case AI_OBJECT_SHRINE_SECOND:
    case AI_OBJECT_SHRINE_THIRD:
        if (gpCurAIHero->Stats(3) > 0 &&
            gpCurAIHero->HasArtifact(AI_ARTIFACT_MAGIC_BOOK) &&
            !gpCurAIHero->HasSpell(cell_k->w4hi - 1)) {
            if (gsSpellInfo[cell_k->w4hi - 1].level <=
                gpCurAIHero->m_secondarySkills[7] + 2) {
                value_h = gsSpellInfo[cell_k->w4hi - 1].aiValue;
                if (gsSpellInfo[cell_k->w4hi - 1].m_e & 1) {
                    if (gpCurAIHero->Stats(3) > 40)
                        shrinePowerMod_p = gfStatPower[40];
                    else
                        shrinePowerMod_p = gfStatPower[gpCurAIHero->Stats(3)];
                    value_h = static_cast<int>(value_h * shrinePowerMod_p);
                }
            }
        } else
            value_h = 0;
        break;
    case AI_OBJECT_GAZEBO:
        if (gpCurAIHero->m_gazeboVisits & (1U << cell_k->w4hi))
            value_h = 0;
        else
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_GAZEBO_VALUE_FACTOR);
        break;
    case AI_OBJECT_TREE_OF_KNOWLEDGE:
        value_h = 0;
        if (!(gpCurAIHero->m_treeKnowledgeVisits & (1U << (cell_k->w4hi & 31)))) {
            switch (cell_k->w4hi >> 6) {
            case 1:
                value_h = static_cast<int>(
                    gpCurAIHero->m_aiFightValue * AI_TREE_KNOWLEDGE_VALUE_FACTOR);
                break;
            case 2:
                if (reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] >=
                    static_cast<int>(AI_TREE_KNOWLEDGE_GOLD_COST)) {
                    value_h = static_cast<int>(
                        gpCurAIHero->m_aiFightValue * AI_TREE_KNOWLEDGE_VALUE_FACTOR);
                    value_h = static_cast<int>(
                        value_h - gafAITurnCostResource[RES_GOLD] *
                                    AI_TREE_KNOWLEDGE_GOLD_COST);
                }
                break;
            case 3:
                if (reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GEMS] >=
                    static_cast<int>(AI_TREE_KNOWLEDGE_GEM_COST)) {
                    value_h = static_cast<int>(
                        gpCurAIHero->m_aiFightValue * AI_TREE_KNOWLEDGE_VALUE_FACTOR);
                    value_h = static_cast<int>(
                        value_h - gafAITurnCostResource[RES_GEMS] *
                                    AI_TREE_KNOWLEDGE_GEM_COST);
                }
                break;
            }
            if (value_h < 0)
                value_h = 0;
        }
        break;
    case AI_OBJECT_WINDMILL:
        if (cell_k->w4hi == 99) {
            value_h = 0;
        } else {
            memset(resources_e, 0, sizeof(resources_e));
            resources_e[cell_k->w4hi] = 2;
            value_h = RVConversion(resources_e);
        }
        break;
    case AI_OBJECT_MAGIC_GARDEN:
        if (!cell_k->w4hi)
            value_h = 0;
        else if (cell_k->w4hi - 1 == RES_GOLD)
            value_h = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_MAGIC_GARDEN_GOLD_AMOUNT);
        else
            value_h = static_cast<int>(gafAITurnCostResource[cell_k->w4hi] * 5.0f);
        break;
    case AI_OBJECT_FLOTSAM:
        value_h = static_cast<int>(
            gafAITurnCostResource[RES_GOLD] * AI_FLOTSAM_GOLD_AMOUNT);
        value_h = static_cast<int>(
            gafAITurnCostResource[RES_WOOD] * AI_FLOTSAM_WOOD_AMOUNT);
        break;
    case AI_OBJECT_SEA_CHEST:
        if (cell_k->w4hi & 0x100)
            value_h = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] *
                    AI_SEA_CHEST_ARTIFACT_GOLD_AMOUNT +
                gArtifactBaseRV[cell_k->w4hi & 0xff]);
        else if (cell_k->w4hi == 1)
            value_h = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_SEA_CHEST_LARGE_GOLD_AMOUNT);
        else
            value_h = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_SEA_CHEST_SMALL_GOLD_AMOUNT);
        if (value_h <= 0)
            value_h = AI_SEA_CHEST_MINIMUM_VALUE;
        break;
    case AI_OBJECT_WAGON:
    case AI_OBJECT_LEAN_TO:
        if (!cell_k->w4hi)
            value_h = 0;
        else if (cell_k->w4hi & 0x80) {
            value_h = gArtifactBaseRV[cell_k->w4hi & 0x7f];
        } else {
            resource = (cell_k->w4hi & 0xf) - 1;
            amount_j = (cell_k->w4hi & 0xf0) >> 4;
            value_h = static_cast<int>(gafAITurnCostResource[resource] * amount_j);
        }
        break;
    case AI_OBJECT_GENIE_LAMP:
        creature = AI_CREATURE_GENIE;
        creatureFlag_k = 0;
        goto creature_purchase;
    case AI_OBJECT_TREE_CITY:
        creature = AI_CREATURE_SPRITE;
        creatureFlag_k = 0;
        goto creature_purchase;
    case AI_OBJECT_WATCH_TOWER:
        creature = AI_CREATURE_ORC;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_TREE_HOUSE:
        creature = AI_CREATURE_SPRITE;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_EXCAVATION:
        creature = AI_CREATURE_SKELETON;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_HALFLING_HOLE:
        creature = AI_CREATURE_HALFLING;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_RUINS:
        creature = AI_CREATURE_MEDUSA;
        creatureFlag_k = 0;
        goto creature_purchase;
    case AI_OBJECT_TROLL_BRIDGE:
        if (cell_k->w4hi & 0x100) {
            value_h = 0;
        } else {
            creature = AI_CREATURE_TROLL;
            creatureFlag_k = 0;
            goto creature_purchase;
        }
        break;
    case AI_OBJECT_DRAGON_CITY:
        if (cell_k->w4hi & 0x100) {
            value_h = 0;
        } else {
            creature = AI_CREATURE_RED_DRAGON;
            creatureFlag_k = 0;
            goto creature_purchase;
        }
        break;
    case AI_OBJECT_CITY_OF_DEAD:
        if (cell_k->w4hi & 0x100) {
            value_h = 0;
        } else {
            creature = AI_CREATURE_POWER_LICH;
            creatureFlag_k = 0;
            goto creature_purchase;
        }
        break;
    case AI_OBJECT_CAVE:
        creature = AI_CREATURE_CENTAUR;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_ARCHER_HOUSE:
        creature = AI_CREATURE_ARCHER;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_GOBLIN_HUT:
        creature = AI_CREATURE_GOBLIN;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_PEASANT_HUT:
        creature = AI_CREATURE_PEASANT;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_DWARF_COTTAGE:
    case AI_OBJECT_SIRENS:
        creature = AI_CREATURE_DWARF;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_DESERT_TENT:
        creature = AI_CREATURE_NOMAD;
        creatureFlag_k = 0;
        goto creature_purchase;
    case AI_OBJECT_WAGON_CAMP:
        creature = AI_CREATURE_ROGUE;
        creatureFlag_k = 0;
creature_purchase:
        EvaluateOneTimeCreaturePurchase(creature, cell_k->w4hi, creatureFlag_k,
                                        purchaseCost_j, value_h, purchaseValue_n);
        gbReduceByReload = 0;
        break;
    case AI_OBJECT_SHIPWRECK_SURVIVOR:
        value_h = gArtifactBaseRV[cell_k->w4hi];
        if (value_h < 125)
            value_h = 125;
        break;
    case AI_OBJECT_SKELETON:
        value_h = cell_k->w4hi == 1 ? 0 : gArtifactBaseRV[cell_k->w4hi];
        break;
    case AI_OBJECT_GRAVEYARD:
    case AI_OBJECT_SHIPWRECK:
    case AI_OBJECT_DERELICT_SHIP:
        value_h = FightEvent(gpCurAIHero, cell_k, 1);
        break;
    case AI_OBJECT_PYRAMID:
        if (!cell_k->w4hi) {
            value_h = 0;
        } else {
            index_k = cell_k->w4hi - 1;
            if (gsSpellInfo[index_k].m_e & 1) {
                battleStatMod_n = gpCurAIHero->Stats(2) > AI_MAX_BATTLE_STAT ?
                    gfBattleStat[AI_MAX_BATTLE_STAT] :
                    gfBattleStat[gpCurAIHero->Stats(2)];
                spellPowerMod_m = battleStatMod_n;
            } else {
                spellPowerMod_m = 1.0f;
            }
            battleValue_b = static_cast<int>(
                gsSpellInfo[index_k].aiValue *
                reinterpret_cast<pdView *>(gpCurPlayer)->upgradeFactor * spellPowerMod_m);
            for (index_k = 0; index_k < AI_TOWN_ARMY_SLOTS; index_k++) {
                reinterpret_cast<armyView *>(gpMonGroup)->creatureTypes[index_k] = 0x33;
                reinterpret_cast<armyView *>(gpMonGroup)->quantities[index_k] = 10;
            }
            ChooseEvaluateBattle(&gpCurAIHero->m_army, gpCurAIHero, gpMonGroup, 0,
                                 0, 0, battleValue_b, battleWon_p, value_h);
            if (!battleWon_p)
                value_h = -50;
        }
        break;
    case AI_OBJECT_DAEMON_CAVE:
        if (cell_k->w4hi == 1)
            value_h = 0;
        else {
            value_h = static_cast<int>(
                gafAITurnCostResource[RES_GOLD] * AI_DAEMON_GOLD_PENALTY +
                (gpCurAIHero->m_aiFightValue * AI_DAEMON_FIGHT_VALUE_FACTOR +
                 ((gpCurAIHero->m_aiFightValue *
                       AI_DAEMON_SECONDARY_FIGHT_VALUE_FACTOR +
                   reinterpret_cast<pdView *>(gpCurPlayer)->artifactValue) +
                  (gafAITurnCostResource[RES_GOLD] * AI_DAEMON_GOLD_VALUE_FACTOR +
                   gpCurAIHero->m_aiFightValue * AI_DAEMON_FIGHT_VALUE_FACTOR))));
            if (cell_k->w4hi == 5 &&
                reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] <
                    AI_DAEMON_CAVE_GOLD_REQUIRED)
                value_h = -100;
        }
        break;
    case AI_OBJECT_ABANDONED_MINE:
        battleValue_b = static_cast<int>(
            static_cast<float>(gaiTurnValueOfMine[(MAP_WIDTH | 0) * y + x]) *
            gMineCharacteristics[RES_GOLD] * gafAITurnCostResource[RES_GOLD]);
        for (index_k = 0; index_k < AI_TOWN_ARMY_SLOTS; index_k++) {
            reinterpret_cast<armyView *>(gpMonGroup)->creatureTypes[index_k] =
                *reinterpret_cast<signed char *>(
                reinterpret_cast<char *>(gpGame) + 0x5cb9 + cell_k->w4hi * 7);
            reinterpret_cast<armyView *>(gpMonGroup)->quantities[index_k] = static_cast<short>(
                *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(gpGame) +
                                                    0x5cba + cell_k->w4hi * 7) / 5);
        }
        ChooseEvaluateBattle(&gpCurAIHero->m_army, gpCurAIHero, gpMonGroup, 0,
                             0, 0, battleValue_b, battleWon_p, value_h);
        if (!battleWon_p)
            value_h = -50;
        break;
    case AI_OBJECT_STONE_LITHS:
    case AI_OBJECT_WHIRLPOOL:
        if (!bEvaluatingTravelGates) {
            value_h = 0;
            break;
        }
        bEvaluatingTravelGates = 0;
        bestExitValue_l = AI_TRAVEL_GATE_INITIAL_VALUE;
        for (exitY_p = 0; exitY_p < MAP_HEIGHT; exitY_p++) {
            for (exitX_p = 0; exitX_p < MAP_WIDTH; exitX_p++) {
                otherCell_j = gpAdvManager->GetCell(exitX_p, exitY_p);
                if ((cell_k->triggerType | 0) == otherCell_j->triggerType &&
                    (cell_k->objIndex | 0) == otherCell_j->objIndex &&
                    (abs(x - exitX_p) | 0) + abs(y - exitY_p) >
                        AI_TRAVEL_GATE_EXIT_RADIUS) {
                    exitValue_i = StrategicValueOfPosition(
                        exitX_p, exitY_p, 0, 0, routeLiveChance_e, AI_TRAVEL_GATE_EXIT_DEPTH);
                    exitValue_i = static_cast<int>(exitValue_i * AI_TRAVEL_GATE_EXIT_SCALE);
                    if ((exitValue_i | 0) > bestExitValue_l) {
                        bestExitValue_l = exitValue_i;
                        bestExitX_c = exitX_p;
                        bestExitY_c = exitY_p;
                    }
                }
            }
        }
        currentValue_h = StrategicValueOfPosition(
            gpCurAIHero->m_x, gpCurAIHero->m_y, 0, 0, routeLiveChance_e,
            AI_TRAVEL_GATE_CURRENT_DEPTH);
        if (bestExitValue_l > currentValue_h + AI_TRAVEL_GATE_PENALTY)
            value_h = bestExitValue_l - currentValue_h - AI_TRAVEL_GATE_PENALTY;
        else if (!immediate)
            value_h = 0;
        else
            value_h = -AI_TRAVEL_GATE_PENALTY;
        bEvaluatingTravelGates = 1;
        gbReduceByReload = 0;
        break;
    case AI_OBJECT_FORT:
        if (gpCurAIHero->m_fortVisits & (1U << cell_k->w4hi))
            value_h = 0;
        else
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_WITCH_DOCTORS_HUT:
        if (gpCurAIHero->m_witchDoctorVisits & (1U << cell_k->w4hi))
            value_h = 0;
        else
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_STANDING_STONES:
        if (gpCurAIHero->m_standingStoneVisits & (1U << cell_k->w4hi))
            value_h = 0;
        else
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_MERCENARY_CAMP:
        if (gpCurAIHero->m_mercenaryCampVisits & (1U << cell_k->w4hi))
            value_h = 0;
        else
            value_h = static_cast<int>(
                gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_XANADU:
        if (gpCurAIHero->m_xanaduVisits & (1U << cell_k->w4hi))
            value_h = 0;
        else {
            if (gpCurAIHero->m_level + gpCurAIHero->m_secondarySkills[4] * 2 >= 10)
                value_h = static_cast<int>(
                    gpCurAIHero->m_aiFightValue * AI_XANADU_VALUE_FACTOR);
            else
                value_h = 0;
        }
        break;
    case AI_OBJECT_LIGHTHOUSE:
        if (*reinterpret_cast<signed char *>(reinterpret_cast<char *>(gpGame) +
                                             0x5cb7 + cell_k->w4hi * 7) ==
                gpCurAIHero->m_owner ||
            OnMySide(*reinterpret_cast<signed char *>(reinterpret_cast<char *>(gpGame) +
                                                       0x5cb7 + cell_k->w4hi * 7)))
            value_h = 0;
        else
            value_h = 1000;
        break;
    case AI_OBJECT_WATER_WHEEL:
        value_h = static_cast<int>(
            cell_k->w4hi * AI_WATER_WHEEL_GOLD_AMOUNT *
            gafAITurnCostResource[RES_GOLD]);
        break;
    case AI_OBJECT_BOAT_EVENT:
        gbActualBoatFound = 1;
        value_h = 90;
        break;
    case AI_OBJECT_BOTTLE:
        value_h = 105;
        break;
    case AI_OBJECT_HILL_FORT:
        value_h = (ComputeUpgradeValue(AI_CREATURE_DWARF, AI_CREATURE_BATTLE_DWARF) | 0) +
                (((ComputeUpgradeValue(AI_CREATURE_ORC, AI_CREATURE_ORC_CHIEF) | 0) +
                  (ComputeUpgradeValue(AI_CREATURE_OGRE, AI_CREATURE_OGRE_LORD) | 0)) | 0);
        break;
    case AI_OBJECT_FREEMANS_FOUNDRY:
        value_h = (ComputeUpgradeValue(
                       AI_CREATURE_SWORDSMAN, AI_CREATURE_MASTER_SWORDSMAN) | 0) +
                (((ComputeUpgradeValue(AI_CREATURE_PIKEMAN,
                                       AI_CREATURE_VETERAN_PIKEMAN)) +
                  (ComputeUpgradeValue(AI_CREATURE_IRON_GOLEM,
                                       AI_CREATURE_STEEL_GOLEM) | 0)) | 0);
        break;
    case AI_OBJECT_MAGIC_WELL:
        value_h = ManaRefreshValue(gpCurAIHero, 1);
        break;
    case AI_OBJECT_ARTESIAN_SPRING:
        if (!cell_k->w4hi)
            value_h = 0;
        else
            value_h = ManaRefreshValue(gpCurAIHero, 2);
        break;
    case AI_OBJECT_WITCHS_HUT:
        value_h = ComputeValueOfFreeSS(gpCurAIHero, cell_k->w4hi);
        break;
    case AI_OBJECT_SIGN:
    case 0x1e:
    case AI_OBJECT_TRADING_POST:
    case AI_OBJECT_SPHINX:
    case 0x51:
    case AI_OBJECT_MAGELLANS_MAPS:
    case AI_OBJECT_OBSERVATION_TOWER:
        value_h = 0;
        break;
    case AI_OBJECT_EXPANSION_OBJECT:
        value_h = EvaluateGenericSite(cell_k);
        break;
    case AI_OBJECT_BARRIER:
        value_h = EvaluateBarrier(cell_k);
        break;
    case AI_OBJECT_TRAVELLER_TENT:
        value_h = EvaluatePassword(cell_k);
        break;
    case AI_OBJECT_EXPANSION_DWELLING:
        value_h = EvaluateRecruitSite(cell_k);
        break;
    case AI_OBJECT_JAIL:
        value_h = EvaluateJail(cell_k);
        break;
    default:
        sprintf(gText, "AI encountered object type %d and doesn't know what to do",
                cell_k->triggerType & 0x7f);
        NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        value_h = 0;
        break;
        }
    } else if (!(giCurPlayerBit & mapExtra[(MAP_WIDTH | 0) * y + x])) {
        value_h = 5;
    }

    if (gbTroopReload && gbReduceByReload)
        value_h = static_cast<int>(value_h * fReduceFactor);
    if (gbBerserk && gbReduceByBerserk)
        value_h = static_cast<int>(value_h * fBerserkFactor);
    if (!immediate) {
        if (value_h > 0 && (mapExtra[(MAP_WIDTH | 0) * y + x] & 0x80) &&
            (cell_k->triggerType & 0x7f) != AI_OBJECT_MONSTER)
            value_h = 0;
        if (value_h < 0 && (cell_k->triggerType & 0x7f) != AI_OBJECT_HERO_EVENT)
            value_h = 0;
        else if (value_h > 32000)
            value_h = 32000;
        else if (value_h < -32000)
            value_h = -32000;
        *reinterpret_cast<short *>(reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
                                   (MAP_WIDTH | 0) * y * sizeof(short) + x * sizeof(short)) =
            static_cast<short>(value_h);
    }
    return value_h;
}

// @early-stop: all 0x299 bytes match after masking 33 aligned COFF relocations.
// The residual is constant/field symbol identity plus jump-table labels delinked
// as this function; all external calls and globals agree.
VA(0x00445a89, 0x299)
int philAI::EvaluateGenericSite(mapCell *cell) {
    int unusedValue29;
    int artifactIndex1;
    int siteLevel5;
    int siteType0;
    int value1;
    int cursedArtifactCount2;
    int creatureType2;
    int removedQuantity3;
    int quantity6;
    int armyValue7;

    cursedArtifactCount2 = 0;
    siteType0 = reinterpret_cast<mapCellView *>(cell)->tentColor;
    siteType0 &= AI_GENERIC_SITE_TYPE_MASK;
    siteLevel5 = reinterpret_cast<mapCellView *>(cell)->tentColor;
    siteLevel5 >>= AI_GENERIC_SITE_LEVEL_SHIFT;
    value1 = 0;

    switch (siteType0) {
    case AI_GENERIC_SITE_CURSED_ARTIFACTS:
        for (artifactIndex1 = 0;
             artifactIndex1 < AI_BATTLE_ARTIFACT_SLOT_COUNT;
             artifactIndex1++) {
            if (IsCursedItem(gpCurAIHero->m_artifacts[artifactIndex1]))
                cursedArtifactCount2++;
        }
        if (reinterpret_cast<pdView *>(gpCurPlayer)->resources[RES_GOLD] >
            AI_GENERIC_SITE_GOLD_THRESHOLD) {
            value1 = cursedArtifactCount2 * AI_GENERIC_SITE_CURSED_ARTIFACT_VALUE;
        }
        break;
    case AI_GENERIC_SITE_SHIPWRECK:
        if (!(gpCurAIHero->m_eventFlags & AI_GENERIC_SITE_SHIPWRECK_FLAG)) {
            value1 = static_cast<int>(gpCurAIHero->m_aiFightValue *
                                      AI_GENERIC_SITE_SHIPWRECK_VALUE);
        }
        break;
    case AI_GENERIC_SITE_FAERIE_RING:
        if (!(gpCurAIHero->m_eventFlags & AI_GENERIC_SITE_FAERIE_RING_FLAG) &&
            giCurAIHeroLuck < AI_GENERIC_SITE_MAX_LUCK) {
            value1 = static_cast<int>(gpCurAIHero->m_aiFightValue *
                                      AI_GENERIC_SITE_FAERIE_RING_VALUE);
        }
        break;
    case AI_GENERIC_SITE_UNUSED_2:
    case AI_GENERIC_SITE_UNUSED_3:
        break;
    case AI_GENERIC_SITE_GRAVEYARD:
        if (!(gpCurAIHero->m_eventFlags & AI_GENERIC_SITE_GRAVEYARD_FLAG)) {
            armyValue7 = 0;
            for (artifactIndex1 = 0;
                 artifactIndex1 < AI_GENERIC_SITE_ARMY_SLOTS;
                 artifactIndex1++) {
                creatureType2 = gpCurAIHero->m_army.m_creatureTypes[artifactIndex1];
                if (creatureType2 != AI_TROOP_EMPTY_SLOT) {
                    quantity6 = gpCurAIHero->m_army.m_quantities[artifactIndex1];
                    removedQuantity3 = static_cast<int>(
                        quantity6 * AI_GENERIC_SITE_GRAVEYARD_REMAINING);
                    armyValue7 += gMonsterDatabase[creatureType2].hitPoints *
                                  (quantity6 - removedQuantity3);
                }
            }
            value1 = static_cast<int>(armyValue7 * gpCurAIHero->m_aiFightValue);
        }
        break;
    case AI_GENERIC_SITE_CREATURE_UPGRADE:
        value1 = ComputeUpgradeValue(AI_GENERIC_SITE_UPGRADE_FROM,
                                     AI_GENERIC_SITE_UPGRADE_TO);
        if (!(gpCurAIHero->m_eventFlags & AI_GENERIC_SITE_CREATURE_UPGRADE_FLAG)) {
            value1 = static_cast<int>(
                value1 +
                (AI_GENERIC_SITE_WEEK_END - gpGame->m_day) *
                    AI_GENERIC_SITE_DAY_VALUE * gpCurAIHero->m_aiFightValue);
        }
        break;
    }
    return value1;
}

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
int OnMySide(int player) {
    if (player != AI_SIDE_NO_PLAYER &&
        (player == giCurPlayer ||
         (gbInCampaign &&
          gpGame->m_campaignType == AI_SIDE_CAMPAIGN_TYPE_ZERO &&
          gpGame->m_campaignScenario + AI_SIDE_CAMPAIGN_SCENARIO_OFFSET ==
              AI_SIDE_CAMPAIGN_SCENARIO_TEN &&
          player != AI_SIDE_PRIMARY_PLAYER) ||
         (gbInCampaign &&
          gpGame->m_campaignType == AI_SIDE_CAMPAIGN_TYPE_ONE &&
          gpGame->m_campaignScenario + AI_SIDE_CAMPAIGN_SCENARIO_OFFSET ==
              AI_SIDE_CAMPAIGN_SCENARIO_ELEVEN &&
          player != AI_SIDE_PRIMARY_PLAYER) ||
         (gpGame->m_mapHeader.victoryCondition == AI_SIDE_VICTORY_CONDITION &&
          ((gpGame->m_mapHeader.victoryConditionValue == AI_SIDE_VICTORY_SPECIAL_VALUE &&
            player != AI_SIDE_PRIMARY_PLAYER) ||
           (gpGame->m_mapHeader.victoryConditionValue != AI_SIDE_VICTORY_SPECIAL_VALUE &&
            ((gpGame->m_players[giCurPlayer].color <
                  gpGame->m_mapHeader.victorySideThreshold &&
              gpGame->m_players[player].color <
                  gpGame->m_mapHeader.victorySideThreshold) ||
             (gpGame->m_players[giCurPlayer].color >=
                  gpGame->m_mapHeader.victorySideThreshold &&
              gpGame->m_players[player].color >=
                  gpGame->m_mapHeader.victorySideThreshold))))) ||
         (gbInCampaign &&
          gpGame->m_campaignType == AI_SIDE_CAMPAIGN_TYPE_ZERO &&
          gpGame->m_campaignScenario + AI_SIDE_CAMPAIGN_SCENARIO_OFFSET ==
              AI_SIDE_CAMPAIGN_SCENARIO_NINE &&
          gpGame->m_players[player].color != AI_SIDE_FIRST_COLOR &&
          gpGame->m_players[player].color != AI_SIDE_FOURTH_COLOR))) {
        return 1;
    } else
        return 0;
}

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
DATA(0x00525650) short gaiHeroLiveChance[54];
DATA(0x005256bc) int giHumanTownConquered;
DATA(0x005256c0) int giCurTurn;
DATA(0x005256c8) int costTemp[7];
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
DATA(0x00527c38) float gfHeroInteractionBonus[54];
DATA(0x00527d10) int gbBerserk;
DATA(0x00527d14) int giCurAIHeroMorale;
DATA(0x00527d18) signed char giBuildBoatStuffTurn[6];
DATA(0x00527d20) int iPlacesVisited[30][2];
DATA(0x00527e10) int gbReduceByReload;
DATA(0x00527e14) int gbTroopReload;
DATA(0x00527e18) int giCurAIHeroLuck;
DATA(0x00527e1c) int gbActualShipyardFound;
