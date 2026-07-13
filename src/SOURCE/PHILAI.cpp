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
#include <SOURCE/X_GLOBAL.h>
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
    return &gpGame->m_castleRecs[id];
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
                        if (gpCurAIHero->field_0x7 == giCurTurn)
                            continue;
                }
                if (gpAdvManager->GetCell(kn, nb)->triggerType == AI_OBJECT_HERO) {
                    heroId = gpAdvManager->GetCell(kn, nb)->w4hi;
                    if (GetHeroSlot(heroId)->m_owner == giCurPlayer)
                        if (gpCurAIHero->field_0x4 == giCurTurn)
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
                    currentHero15->field_0x7 = -99;
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
                stepLimit36 = static_cast<int>(stepLimit36 * (1.7 - gpGame->m_aiSpeed * 0.1));
                minimumValue9 =
                    static_cast<int>(minimumValue9 * ((gpGame->m_aiSpeed - 1) * 0.06 + 0.8));

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
            1.0 / (gpGame->m_playerCount + gpGame->m_humanPlayerCount);
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
    if (gpGame->m_aiSpeed == 0) {
        gfAttackHumanBonus = 0.6f;
        gfAttackComputerBonus = 1.3f;
    } else if (gpGame->m_aiSpeed == 1) {
        gfAttackHumanBonus = 1.0f;
        gfAttackComputerBonus = 1.0f;
    } else {
        gfAttackHumanBonus = static_cast<float>(gpGame->m_aiSpeed * 0.07 + 1.0);
        gfAttackComputerBonus = static_cast<float>(1.1 - gpGame->m_aiSpeed * 0.12);
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
    if (gpGame->m_playerCount - gpGame->m_humanPlayerCount == 2)
        giMaxHeroesForThisPlayer++;
    if (gpGame->m_playerCount - gpGame->m_humanPlayerCount == 3)
        giMaxHeroesForThisPlayer++;
    if (ownedTownCount19 >= 4)
        giMaxHeroesForThisPlayer++;
    if (ownedTownCount19 >= 9)
        giMaxHeroesForThisPlayer++;
    if (static_cast<unsigned char>(gpGame->field_0x2af) == AI_OBJECT_MONSTER)
        giMaxHeroesForThisPlayer--;
    if (static_cast<unsigned char>(gpGame->field_0x2af) >= AI_EXPANSION_OBJECT_THRESHOLD)
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
DATA(0x00525650) short gaiHeroLiveChance[54];
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
DATA(0x00527c38) float gfHeroInteractionBonus[54];
DATA(0x00527d10) int gbBerserk;
DATA(0x00527d14) int giCurAIHeroMorale;
DATA(0x00527d18) signed char giBuildBoatStuffTurn[6];
DATA(0x00527d20) int iPlacesVisited[30][2];
DATA(0x00527e10) int gbReduceByReload;
DATA(0x00527e14) int gbTroopReload;
DATA(0x00527e18) int giCurAIHeroLuck;
DATA(0x00527e1c) int gbActualShipyardFound;
