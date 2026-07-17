// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\PHILAI.OBJ   from: (directly linked into exe)
// functions: 82   data: 51
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/PHILAI.h>
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

// Globals and called free functions come from their owner headers.

// __FILE__ for the NWC BaseAlloc/BaseFree memory tracking (reloc-masked path string).
#define PHFILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\PHILAI.CPP")
#define PHILAI_INIT_MAP_LINE_BASE 0x1b86
#define PHILAI_CLOSE_MAP_LINE_BASE 0x1b96

// @data-layout-note
// Retail initialized storage is 0xf20e0..0xf2552 (0x472 bytes). The 18 public
// definitions, 35 compiler-private literals, and two recovered short line-base
// statics have exact retail owners and payloads; all private references use
// owner-relative addend zero. The line bases are 0x1b86 at 0xf2394 and 0x1b96
// at 0xf2474, with five references each. The separate CRT constructor entry is
// at 0xec004. Do not restore the former five overlapping literal aliases for
// either line base.
//
// Retail .rdata is 0xeb280..0xeb5e0 (0x360 bytes). Candidate .rdata is 0x368
// bytes, so whole-section translation is not valid. DetermineTargetPosition's
// recovered 1.5f owner removed a candidate-only 1.1f allocation and its
// alignment tail, shrinking the post-type-ownership object from 0x370. The
// remaining pool has 140 allocations and 0x334 logical bytes: the canonical
// CONFIG_TYPES include boundary materializes the 2.0f hero-purchase divisor
// and gives the attention identity its eight-byte declared representation.
// Retail has 139 allocations, 0x330 logical bytes, and twelve zero padding
// words; the candidate has thirteen. The corrected pool distinguishes campfire
// 500.0f, buoy 400.0f, watering-hole 300.0f, and the shared 200.0f land-site
// factor.
//
// All 33 zero-fill owners are source DATA definitions. Their retail public
// span is 0x125620..0x127e20 (0x2800 bytes); candidate COMMON ordering differs
// and is not a reason to add padding or aliases. Of 1238 candidate data
// references, 1231 pair at exact function-relative sites. The seven residuals
// are two uncarved initializer references and five already documented
// function-shape sites, not evidence for another data owner.
DATA(0x004f2394) static i16 s_initAIMapLineBase = PHILAI_INIT_MAP_LINE_BASE;
DATA(0x004f2474) static i16 s_closeAIMapLineBase = PHILAI_CLOSE_MAP_LINE_BASE;

DATA(0x005256f0) searchArray SVSearchArray;

// @semantic
// Bytes 0x000-0x1b5 are instruction-identical with relocations masked, and all 17
// relocation targets agree. Retail's three NOPs begin after the CodeView function range.
VA(0x0043781b, 0x1b5)
void ResetHeroRVs(i32 resetAll, i32 x, i32 y) {
    i32 idx;
    i32 node;

    if (x != -1) {
        if (y == -1)
            return;
        for (node = 0; node < MAP_WIDTH; node++) {
            for (idx = 0; idx < MAP_HEIGHT; idx++) {
                if (resetAll != 0) {
                    if (abs(x - node) + abs(y - idx) < AI_NEARBY_RADIUS)
                        *reinterpret_cast<i16 *>(reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
                            node * 2 + idx * MAP_WIDTH * 2) = AI_RV_UNSET;
                } else {
                    *reinterpret_cast<i16 *>(reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
                        node * 2 + idx * MAP_WIDTH * 2) = AI_RV_UNSET;
                    *reinterpret_cast<i16 *>(reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
                        node * 2 + idx * MAP_WIDTH * 2) = AI_RV_UNSET;
                }
            }
        }
        *reinterpret_cast<i16 *>(reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
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
void CheckDoMain(i32 a1, i32 doMain) {
    if (KBTickCount() > iLastFrameRateTimer + 0xf || KBTickCount() > glTimers[0]) {
        Process1WindowsMessage();
        PollSound();
        if (KBTickCount() > glTimers[0]) {
            if (doMain == 0) {
                i32 idx = bShowIt;
                i32 savedX = gpAdvManager->m_previousOriginX;
                i32 savedY = gpAdvManager->m_previousOriginY;
                gbDrawSavedCursor = 1;
                if (gConfig.blackoutComputer == 0 && gbRemoteOn == 0)
                    bShowIt = 1;
                else
                    bShowIt = 0;
                if (bShowIt == 0)
                    bSpecialHideCursor = 1;
                if (gpAdvManager->ComboDraw(gpAdvManager->m_previousOriginX,
                                            gpAdvManager->m_previousOriginY, 0))
                    gpAdvManager->UpdateScreen(0, 0);
                else
                    gpAdvManager->UpdBottomView(0, 1, 1);
                bShowIt = idx;
                gbDrawSavedCursor = 0;
                bSpecialHideCursor = 0;
                gpAdvManager->m_previousOriginX = savedX;
                gpAdvManager->m_previousOriginY = savedY;
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
    i32 i;
    for (i = 0; i < 6; i++) {
        giBuildShipyard[i] = -1;
        giBuildBoat[i] = -1;
        giBuildBoatStuffTurn[i] = 0;
    }
}

// @semantic: Complete CFG, frame/slots, and all 6/6 ordered relocations align.
// At normalized instruction 13 ours has one extra continuation jmp before the
// retail loop body. An early-continue spelling lowered the match to 94.90%.
// Revisit after PHILAI TU/header state changes or in the byte-last-mile phase.
VA(0x00437bb5, 0xac)
void philAI::DoAllHeroInteractions(void) {
    i32 i;
    for (i = 0; i < gpCurPlayer->m_townCount; i++) {
        town *pTown = gpGame->GetTown(gpCurPlayer->TownId(i));
        if (pTown->m_occupyingHeroId != -1)
            HeroInteractionAtTown(
                gpGame->GetHero(pTown->m_occupyingHeroId), pTown, 0, &iDummy);
    }
}

// @semantic: Complete 0x37e-byte CFG, frame/slots, and all 11 external
// relocations align. Current code has extra /Ob1 continuation jumps at the
// first hero-count and empty-slot tests. Revisit after PHILAI TU/header state
// changes or in the byte-last-mile phase.
VA(0x00437c61, 0x37e)
void philAI::CheckForCreatureUpgrades(void) {
    i32 upgradeType = -1;
    i32 node;
    i32 armyIndex;
    i32 creatureIndex;
    i32 dwelling;
    i32 canUpgrade;
    town *townPtr;
    armyGroup *armyPtr;
    i32 goldCost;
    i32 resourceType;
    i32 resourceCost;
    i32 mergeIndex;

    for (node = 0; node < gpCurPlayer->m_townCount; node++) {
        townPtr = gpGame->GetTown(gpCurPlayer->TownId(node));
        for (armyIndex = 0; armyIndex < 2; armyIndex++) {
            if (armyIndex == 0)
                armyPtr = &townPtr->m_army;
            else {
                if (townPtr->m_occupyingHeroId == -1)
                    continue;
                else
                    armyPtr = &gpGame->GetHero(townPtr->m_occupyingHeroId)->m_army;
            }
            for (creatureIndex = 0; creatureIndex < 5; creatureIndex++) {
                if (armyPtr->m_creatureTypes[creatureIndex] == -1)
                    continue;
                canUpgrade = 0;
                for (dwelling = 20; dwelling <= 24; dwelling++) {
                    if (gDwellingType[townPtr->m_type]
                                      [dwelling - AI_BUILDING_FIRST_DWELLING] ==
                            armyPtr->m_creatureTypes[creatureIndex] &&
                        (townPtr->m_buildings & (1 << (dwelling + 5)))) {
                        canUpgrade = 1;
                        upgradeType = armyPtr->m_creatureTypes[creatureIndex] + 1;
                    }
                }
                if ((armyPtr->m_creatureTypes[creatureIndex] == 35 ||
                     armyPtr->m_creatureTypes[creatureIndex] == 36) &&
                    (townPtr->m_buildings & 0x40000000)) {
                    canUpgrade = 1;
                    upgradeType = 37;
                }
                if (canUpgrade) {
                    goldCost = (gMonsterDatabase[upgradeType].cost -
                                gMonsterDatabase[armyPtr->m_creatureTypes[creatureIndex]].cost) *
                               armyPtr->m_quantities[creatureIndex] * 2;
                    if (upgradeType == 37) {
                        resourceType = RES_SULFUR;
                        resourceCost = armyPtr->m_quantities[creatureIndex] * 2;
                    } else if (upgradeType == 46) {
                        resourceType = RES_GEMS;
                        resourceCost = armyPtr->m_quantities[creatureIndex] * 2;
                    } else {
                        resourceType = -1;
                        resourceCost = 0;
                    }
                    if (goldCost <= gpCurPlayer->m_resources[RES_GOLD] &&
                        (resourceType == -1 ||
                         resourceCost <= gpCurPlayer->m_resources[resourceType])) {
                        gpCurPlayer->m_resources[RES_GOLD] -= goldCost;
                        if (resourceType != -1)
                            gpCurPlayer->m_resources[resourceType] -= resourceCost;
                        armyPtr->m_creatureTypes[creatureIndex] = static_cast<i8>(upgradeType);
                        for (mergeIndex = 0; mergeIndex < 5; mergeIndex++) {
                            if (mergeIndex != creatureIndex &&
                                armyPtr->m_creatureTypes[mergeIndex] ==
                                    armyPtr->m_creatureTypes[creatureIndex] &&
                                armyPtr->m_quantities[mergeIndex] > 0) {
                                armyPtr->m_quantities[mergeIndex] += armyPtr->m_quantities[creatureIndex];
                                armyPtr->m_quantities[creatureIndex] = 0;
                                armyPtr->m_creatureTypes[creatureIndex] = -1;
                            }
                        }
                    }
                }
            }
        }
    }
}

inline town *GetCastleSlot(i32 id) {
    return &gpGame->m_castleRecs[id];
}

// @semantic
// Instruction stream is identical with relocations masked and all 80 relocation
// targets agree; residual rows are delinker names for the two log strings.
VA(0x00437fdf, 0x4be)
void philAI::CheckBuyStuff(void) {
    i32 done = 0;
    i32 jb = 0;
    BHC best;
    town *idx;

    gpGame->CheckHeroConsistency();
    if (gpCurPlayer->m_resources[RES_GOLD] >= 200) {
        if (gpCurPlayer->m_resources[RES_GOLD] < 2500 &&
            gpCurPlayer->m_heroCount == 0)
            return;
        LogInt("CheckBuy Start", gpCurPlayer->m_resources[RES_GOLD],
               -999, -999, -999, -999, -999, -999);
        idx = 0;
        if (giBuildShipyard[giCurPlayer] >= 0) {
            idx = gpGame->GetTown(giBuildShipyard[giCurPlayer]);
        } else if (giBuildBoat[giCurPlayer] >= 0) {
            idx = gpGame->GetTown(giBuildBoat[giCurPlayer]);
        }
        if (giBuildShipyard[giCurPlayer] >= 0) {
            idx = GetCastleSlot(giBuildShipyard[giCurPlayer]);
        } else if (giBuildBoat[giCurPlayer] >= 0) {
            idx = GetCastleSlot(giBuildBoat[giCurPlayer]);
        }
        if (idx != 0 && idx->m_owner != giCurPlayer) {
            giBuildShipyard[giCurPlayer] = -1;
            giBuildBoat[giCurPlayer] = giBuildShipyard[giCurPlayer];
            idx = 0;
        }
        if (giBuildShipyard[giCurPlayer] >= 0) {
            if (CanBuy(idx, 3) &&
                CanBuild(idx, 3)) {
                BuildBuilding(idx, 3);
                giBuildShipyard[giCurPlayer] = -1;
            } else {
                gpCurPlayer->m_resources[RES_GOLD] -= 2000;
                gpCurPlayer->m_resources[RES_WOOD] -= 20;
            }
        }
        if (giBuildBoat[giCurPlayer] >= 0) {
            if ((idx->m_buildings & 8) &&
                gpCurPlayer->m_resources[RES_GOLD] >= 1000 &&
                gpCurPlayer->m_resources[RES_WOOD] >= 10) {
                if (gpGame->GetBoatsBuilt() < 48 &&
                    gpAdvManager->GetCell(idx->m_boatX, idx->m_boatY)->m_triggerType == 0 &&
                    gpGame->CreateBoat(idx->m_boatX, idx->m_boatY, 0) != -1) {
                    gpCurPlayer->m_resources[RES_GOLD] -= 1000;
                    gpCurPlayer->m_resources[RES_WOOD] -= 10;
                }
                giBuildBoat[giCurPlayer] = -1;
            } else {
                gpCurPlayer->m_resources[RES_GOLD] -= 1000;
                gpCurPlayer->m_resources[RES_WOOD] -= 10;
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
            gpCurPlayer->m_resources[RES_GOLD] += 2000;
            gpCurPlayer->m_resources[RES_WOOD] += 20;
        }
        if (giBuildBoat[giCurPlayer] >= 0) {
            gpCurPlayer->m_resources[RES_GOLD] += 1000;
            gpCurPlayer->m_resources[RES_WOOD] += 10;
        }
        DoAllHeroInteractions();
        LogInt("CheckBuy End  ", gpCurPlayer->m_resources[RES_GOLD],
               -999, -999, -999, -999, -999, -999);
    }
}

inline hero *GetHeroSlot(i32 id) {
    return &gpGame->m_heroRecs[id];
}

// @semantic
// At the retained source-hash max, raw disassembly differs only at the two
// owned-current-turn exits: retail uses direct JE branches while /Od emits
// equivalent JNE + continuation JMP blocks. All frame roles agree; 42/42
// relocations agree after the delinker's normalDirTable+1 string-symbol alias.
VA(0x0043849d, 0x2e8)
i32 philAI::GoodAdjacent(i32 *direction) {
    i32 ra;
    float py;
    i32 p;
    i32 val;
    i32 bestValue;
    float node;
    i32 nb;
    i32 kn;
    i32 jb;
    i32 idx;
    i32 heroId;

    idx = -1;
    py = fReduceFactor;
    node = fBerserkFactor;
    fReduceFactor = 1.0f;
    fBerserkFactor = 1.0f;
    bestValue = 100;
    if ((gpAdvManager->GetCell(gpCurAIHero->m_x,
                               gpCurAIHero->m_y)->m_triggerType & 0x7f) == AI_OBJECT_MONSTER)
        return 0;
    for (jb = 0; jb < 8; jb++) {
        if (gpAdvManager->ValidMoveWithEvent(gpCurAIHero, jb)) {
            kn = normalDirTable[jb].x + gpCurAIHero->m_x;
            nb = normalDirTable[jb].y + gpCurAIHero->m_y;
            if ((gpAdvManager->GetCell(kn, nb)->m_triggerType & 0x80) &&
                !(mapExtra[kn + (MAP_WIDTH | 0) * nb] & 0x80) &&
                ((gpAdvManager->GetCell(kn, nb)->m_triggerType & 0x7f) != AI_OBJECT_MONSTER) &&
                ((gpAdvManager->GetCell(kn, nb)->m_triggerType & 0x7f) != AI_OBJECT_ARTIFACT)) {
                if (gpAdvManager->GetCell(kn, nb)->m_triggerType == AI_OBJECT_TOWN) {
                    ra = gpAdvManager->GetCell(kn, nb)->m_objectMetadata;
                    if (GetCastleSlot(ra)->m_owner == giCurPlayer)
                        if (gpCurAIHero->m_lastTownInteractionTurn == giCurTurn)
                            continue;
                }
                if (gpAdvManager->GetCell(kn, nb)->m_triggerType == AI_OBJECT_HERO) {
                    heroId = gpAdvManager->GetCell(kn, nb)->m_objectMetadata;
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

// @semantic: Complete CFG, frame/slots, and all 61 external relocations align.
// The first residual at normalized instruction 72 is the /Od operand-memory
// choice for both MAP_WIDTH and MAP_HEIGHT loop bounds; later rows are floating
// constant and division-guard identities. Ten guarded TU-state probes did not
// produce an eligible closure. Revisit in the byte-last-mile phase.
VA(0x00438785, 0x4b8)
void philAI::CheckReload(void) {
    i32 p;
    i32 node;
    mapCell *nb;
    i32 kn;
    i32 jb;
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
                switch (nb->m_triggerType) {
                case AI_OBJECT_TOWN:
                    jb = FightValueOfStack(
                        &GetCastleSlot(nb->m_objectMetadata)->m_army,
                        0, 0, 0, 0, 0);
                    if (gpGame->m_castleOwners[nb->m_objectMetadata] == gpCurAIHero->m_owner) {
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
                    if (gpGame->m_availableHeroes[nb->m_objectMetadata] != gpCurAIHero->m_owner) {
                        jb = FightValueOfStack(&GetHeroSlot(nb->m_objectMetadata)->m_army,
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

// @semantic: Complete 0x302-byte CFG, frame/slots, and all 26 external
// relocations align. Current TU state first differs at normalized instruction
// 46: ours loads each loop index before MAP_WIDTH/MAP_HEIGHT and skips on jle;
// retail loads the bound first and skips on jge. Later rows are floating
// constant/division-guard identities. Revisit in the byte-last-mile phase.
VA(0x00438c3d, 0x302)
void philAI::CheckBerserk(void) {
    i32 row17;
    i32 column1;
    mapCell *kn;
    i32 jb;
    i32 idx;
    i32 best = -1;
    hero *heroPtr;

    gbBerserk = 0;
    fBerserkFactor = 1.0f;
    jb = FightValueOfStack(&gpCurAIHero->m_army, gpCurAIHero, 1, 0, 0, 0);
    if (gpCurPlayer->m_aiDifficulty == 0)
        jb = static_cast<i32>(jb * AI_EASY_STRENGTH_FACTOR);
    if (jb < AI_MINIMUM_FIGHT_VALUE)
        jb = AI_MINIMUM_FIGHT_VALUE;
    if (jb < AI_BERSERK_THRESHOLD)
        return;
    {
        for (column1 = 0; column1 < MAP_WIDTH; column1++) {
            for (row17 = 0; row17 < MAP_HEIGHT; row17++) {
                kn = gpAdvManager->GetCell(column1, row17);
                switch (kn->m_triggerType) {
                case AI_OBJECT_TOWN:
                    if (gpGame->m_castleOwners[kn->m_objectMetadata] != gpCurAIHero->m_owner) {
                        if (gpGame->m_castleOwners[kn->m_objectMetadata] != -1) {
                        idx = FightValueOfStack(
                            &GetCastleSlot(kn->m_objectMetadata)->m_army,
                            0, 1, 1, kn->m_objectMetadata, 0);
                        if (idx > jb)
                            return;
                        if (idx > best)
                            best = idx;
                        }
                    }
                    break;
                case AI_OBJECT_HERO:
                    if (gpGame->m_availableHeroes[kn->m_objectMetadata] != gpCurAIHero->m_owner) {
                    heroPtr = GetHeroSlot(kn->m_objectMetadata);
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
void philAI::DimensionDoorTo(i32 x, i32 y) {
    gpAdvManager->TeleportTo(gpCurAIHero, x, y, 0, 0);
    if (gpCurAIHero->m_remainingMobility < 0xe1)
        gpCurAIHero->m_remainingMobility = 0;
    else
        gpCurAIHero->m_remainingMobility -= 0xe1;
    gpCurAIHero->UseSpell(0x38);
}

// @semantic
// The 0x38 frame and all 34 external relocations agree. The remaining instruction
// delta is the equivalent MAP_WIDTH loop comparison operand/polarity.
VA(0x00438fb0, 0x3f9)
i32 philAI::DoAnywhereDDoorTownGate(i32 targetValue) {
    i32 bestY;
    i32 candidateY;
    i32 bestX;
    i32 y;
    i32 candidateX;
    i32 adjacentIndex;
    i32 x;
    i32 monsterY;
    i32 monsterX;
    i32 positionValue;
    i32 value;
    i32 bestValue;
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
                    if (giGroundToTerrain[cell->m_terrainImageIndex] == 0)
                        continue;
                    if (!((cell->m_triggerType & 0x80) ||
                          (targetValue < 25 && Random(0, 10) < 2))) {
                    } else {
                    positionValue = RVOfPosition(
                        x, y, 0, -1, -1, 0, -1, -1, 2,
                        gpCurAIHero->m_remainingMobility);
                    positionValue += Random(0, 30);
                    if (positionValue > bestValue) {
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
                            if (giGroundToTerrain[cell->m_terrainImageIndex] == 0)
                                continue;
                            if (cell->m_flags & 8)
                                continue;
                            if (cell->m_triggerType & 0x80)
                                continue;
                            if (cell->m_objectIndex != 0xff && !(cell->m_flags & 0x80))
                                continue;
                            bestX = candidateX;
                            bestY = candidateY;
                        bestValue = positionValue;
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

// @semantic: Complete 0x158-byte CFG, frame/slots, and all 11/11 ordered
// relocations align. Explicit unsigned path bytes fixed the retail zero-extend
// semantics and raised the live score from 91.28% to 97.60%. The first residual
// now only loads node before gpSearchArray where retail loads them in reverse.
// Revisit after PHILAI TU/header state changes or in the byte-last-mile phase.
VA(0x004393a9, 0x158)
i32 philAI::DoDimensionDoor(hero *pHero) {
    i32 node;           // i
    i32 nb;             // y
    i32 kn;             // x
    mapCell *jb;        // cell
    i32 idx;            // dist
    i32 bestX, bestY;
    if (pHero->m_remainingMobility < 0x4b)
        return 0;
    bestX = -1;
    kn = pHero->m_x;
    nb = pHero->m_y;
    for (node = gpSearchArray->m_pathLength - 1; node >= 1; node--) {
        kn += normalDirTable[static_cast<u8>(
            gpSearchArray->m_storage.aiPath.directions[node])].x;
        nb += normalDirTable[static_cast<u8>(
            gpSearchArray->m_storage.aiPath.directions[node])].y;
        if (abs(kn - pHero->m_x) <= 7 &&
            abs(nb - pHero->m_y) <= 7) {
            jb = gpAdvManager->GetCell(kn, nb);
            if (!(jb->m_triggerType & 0x80) &&
                !(jb->m_flags & 0x8)) {
                bestX = kn;
                bestY = nb;
                idx = gpSearchArray->m_pathLength - node;
            }
        }
    }
    if (bestX == -1 || idx <= 5)
        return 0;
    DimensionDoorTo(bestX, bestY);
    return 1;
}

// @semantic: The complete 0xb7-byte CFG, 0x14 frame, five stack slots, and all
// 6/6 ordered relocations align. At normalized instruction 44 retail loads
// bestFV into eax, compares fv, and skips on jle; ours loads fv, compares bestFV,
// and skips on jge. Reversed operands, a value-preserving | 0 pin, and an empty
// positive arm with the update in else were neutral or added a trampoline.
// Revisit in the byte-last-mile phase or after PHILAI TU/header state changes.
VA(0x00439501, 0xb7)
void philAI::SetupRelativeHeroStrengths(void) {
    hero *alpha;
    i32 fv;
    i32 i;
    i32 bestFV;
    bestFV = -1;
    iAlphaMale = -1;
    for (i = 0; i < gpCurPlayer->m_heroCount; i++) {
        alpha = &gpGame->m_heroRecs[gpCurPlayer->HeroId(i)];
        fv = FightValueOfStack(&alpha->m_army, alpha, 1, 0, -1, 0);
        if (fv > bestFV) {
            bestFV = fv;
            iAlphaMale = alpha->m_id;
        }
    }
}

VA(0x004395b8, 0x79)
void ValidateHero(hero *pHero) {
    i32 i;
    for (i = 0; i < 5; i++) {
        if (pHero->m_army.m_creatureTypes[i] != -1) {
            if (pHero->m_army.m_creatureTypes[i] < 0 ||
                pHero->m_army.m_creatureTypes[i] >= 0x42) {
                i32 j = 1;
                j++;
            }
        }
    }
}

// @semantic
// Raw disassembly has identical logic and frame roles. Residual rows are one
// TU-cumulative comparison load order, leading/trailing /Ob1 continuations, and
// equivalent conditional/tail-thunk encodings at reconstructed loop/exit edges;
// all 221 relocation targets agree, with remaining names delinker artifacts.
VA(0x00439631, 0xcad)
void philAI::DoAI(i32 player) {
    i32 hiddenPointers7 = 0;
    i32 hourglassShown4 = 0;
    i32 boughtAfterCapture5 = 0;
    i32 heroDone5;
    i32 stepLimit36;
    i32 minimumValue9;
    hero* currentHero15;
    i32 pathIndex3[2];
    i32 steps14;
    i32 stopAfterStep0[2];
    mapCell* eventCell9;
    i32 eventX18;
    i32 eventY16;
    i32 moveFlagA3[9];
    i32 moveFlagB12;
    town * townPtr9;
    i32 targetValue11;
    i32 townId3;
    u32 direction26;
    i32 specialDirection6;
    i32 adjacentDirection3;
    i32 savedShow10;

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
                     pathIndex3[0] < gpCurPlayer->m_heroCount;
                     pathIndex3[0]++) {
                    currentHero15 = GetHeroSlot(
                        gpCurPlayer->m_heroIds[pathIndex3[0]]);
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
                    gpCurPlayer->m_resources[pathIndex3[0]],
                    gpCurPlayer->m_income[pathIndex3[0]]
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
                LogStr(gpCurAIHero->m_name);
                LogStr("\n");
                CheckReload();
                CheckBerserk();
                giShowComputerRoute = 0;
                if (gConfig.blackoutComputer == 0 && gbRemoteOn == 0
                    && MapExtraPosAndAdjacentsSet(
                        gpCurAIHero->m_x,
                        gpCurAIHero->m_y,
                        giCurWatchPlayerBit
                    )) {
                    bShowIt = 1;
                    gpAdvManager->SetHeroContext(
                        static_cast<u8>(gpCurAIHero->m_id), 0);
                } else {
                    bShowIt = 0;
                    gpAdvManager->SetHeroContext(
                        static_cast<u8>(gpCurAIHero->m_id), 0);
                }

                heroDone5 = 0;
                ResetHeroRVs(0, 0, 0);
                stepLimit36 = (gpCurAIHero->m_eventFlags & 0x80) ? 15 : 5;
                minimumValue9 = gpCurAIHero->m_mobility + 800;
                stepLimit36 = static_cast<i32>(stepLimit36 * (1.7 - gpGame->m_difficulty * 0.1));
                minimumValue9 =
                    static_cast<i32>(minimumValue9 * ((gpGame->m_difficulty - 1) * 0.06 + 0.8));

                while (!heroDone5 && gpCurAIHero->m_remainingMobility >= 75) {
                    if (!(gbGameOver == 0)) {
                        break;
                    } else {
                    if (gpCurAIHero->m_remainingMobility == gpCurAIHero->m_mobility
                        && gpCurPlayer->m_ultimateArtifactHintChance > 15
                        && gpCurPlayer->m_ultimateArtifactHintX == gpCurAIHero->m_x
                        && gpCurPlayer->m_ultimateArtifactHintY
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
                                specialDirection6
                            );
                            townId3 =
                                gpGame->GetTownId(gpCurAIHero->m_x, gpCurAIHero->m_y);
                            if (townId3 != -1) {
                                townPtr9 = GetCastleSlot(townId3);
                                if (townPtr9->m_threat != 0
                                    && (gpAdvManager
                                            ->GetCell(
                                                gpCurAIHero->m_destinationX,
                                                gpCurAIHero->m_destinationY
                                            )
                                            ->m_triggerType
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
                                           ->m_triggerType
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
                            static_cast<u8>(gpCurAIHero->m_id), 0);
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
                                    static_cast<i8>(adjacentDirection3);
                                stopAfterStep0[0] = 1;
                            }
                        }
                        direction26 =
                            static_cast<u8>(gpSearchArray->m_storage.aiPath.directions[pathIndex3[0]]);
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
                        && gpCurPlayer->m_ultimateArtifactHintChance > 15
                        && gpCurPlayer->m_ultimateArtifactHintX == gpCurAIHero->m_x
                        && gpCurPlayer->m_ultimateArtifactHintY
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
                        if (gpCurPlayer->m_currentHero == -1) {
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
    i32 i;
    for (i = 0; i < gpGame->m_playerCount; i++)
        GetGameAttentionValue(i);
}

// @semantic
// The threat scan and early-turn hero/town pointers retain their retail scopes,
// including the initialized unused fight-value word. The 0xac frame and all 171
// external relocations agree; residuals are local constant identities and
// equivalent loop/comparison operand order.
VA(0x0043a329, 0xe2b)
void philAI::GetTurnAIVars(i32 player) {
    i32 ownedTownCount19;
    i32 firstWeekIndex0;
    i32 playerIndex0;
    town *townPtr9;
    i32 unusedFightValue7;
    i32 artifactTotal8;
    i32 outcomeE14;
    i32 generalIndex4;
    i32 y4;
    i32 outcomeD1;
    i32 innerIndex27;
    i32 x8;
    i32 xCenter19;
    i32 mineValue4;
    i32 yCenter3;
    playerData *playerPtr27;
    i32 lastFightValue8;
    float townCountValue5;
    hero *heroPtr5;
    float fightValueFloat15;
    i32 earlyTurn21;
    i32 lateTurn40;
    float winChance11;
    i32 outcomeA9;
    i32 outcomeB6;
    mapCell *cell12;
    i32 outcomeC4;

    giCurTurn = (gpGame->m_month - 1) * 28 + (gpGame->m_week - 1) * 7 + gpGame->m_day;
    GetTurnAttentionValue(player);
    TurnCostResource(player);
    iCurHourGlassPhase = 0;
    iSandAnim = 0;
    gpCurPlayer->m_obeliskValue =
        static_cast<i32>(TurnValueOfObelisk(player));
    gpCurPlayer->m_totalObeliskValue =
        gpCurPlayer->m_obeliskValue * gpGame->m_obeliskCount / 48;
    gpCurPlayer->m_unexploredValue =
        MeanRVOfUnexploredTerritory(player);
    bHeroBuiltThisTurn = 0;
    if (giCurTurn - giBuildBoatStuffTurn[player] > 8) {
        giBuildShipyard[player] = -1;
        giBuildBoat[player] = -1;
    }

    unusedFightValue7 = 0;
    fightValueFloat15 = 0.0f;
    if (giCurTurn <= 8) {
        fFirstWeekTownFV = 0.0f;
        for (firstWeekIndex0 = 0;
             firstWeekIndex0 < gpCurPlayer->m_townCount;
             firstWeekIndex0++) {
            townPtr9 = GetCastleSlot(
                gpCurPlayer->m_townIds[firstWeekIndex0]);
            for (innerIndex27 = 0; innerIndex27 < AI_HERO_COUNT; innerIndex27++) {
                heroPtr5 = &gpGame->m_heroRecs[innerIndex27];
                if (heroPtr5->m_owner >= 0 && heroPtr5->m_owner <= 6)
                    if (heroPtr5->m_owner == giCurPlayer) {
                    } else
                        if (abs(heroPtr5->m_y - townPtr9->m_y) +
                                abs(heroPtr5->m_x - townPtr9->m_x) < 16) {
                            fFirstWeekTownFV = 0.3f;
                            goto firstWeekDone;
                        }
            }
        }
    }

firstWeekDone:
    lastFightValue8 = 0;
    for (generalIndex4 = 0; generalIndex4 < gpCurPlayer->m_heroCount; generalIndex4++) {
        heroPtr5 = GetHeroSlot(
            gpCurPlayer->m_heroIds[generalIndex4]);
        fightValueFloat15 = static_cast<float>(
            FightValueOfStack(&heroPtr5->m_army, heroPtr5, 0, 0, 0, 0));
        lastFightValue8 = static_cast<i32>(lastFightValue8 + fightValueFloat15);
        heroPtr5->m_aiFightValue = static_cast<float>(fightValueFloat15 * AI_HERO_VALUE_SCALE +
                                                     AI_HERO_VALUE_BASE);
    }
    for (generalIndex4 = 0; generalIndex4 < gpCurPlayer->m_townCount; generalIndex4++) {
        townPtr9 = GetCastleSlot(
            gpCurPlayer->m_townIds[generalIndex4]);
        fightValueFloat15 = static_cast<float>(FightValueOfStack(
            &townPtr9->m_army, 0, 0, 0, 0, 0));
        lastFightValue8 = static_cast<i32>(lastFightValue8 + fightValueFloat15);
    }
    gpCurPlayer->m_upgradeValueWeight =
        static_cast<float>(gpCurPlayer->m_income[RES_GOLD] +
                           gpCurPlayer->m_resources[RES_GOLD]) /
            (lastFightValue8 + 1000) +
        gpCurPlayer->m_attentionWeights.upgradeBase;

    artifactTotal8 = 0;
    for (generalIndex4 = 4; generalIndex4 < 37; generalIndex4++)
        artifactTotal8 += gArtifactBaseRV[generalIndex4];
    for (generalIndex4 = 0; generalIndex4 < gpGame->m_playerCount; generalIndex4++) {
        gpGame->m_players[generalIndex4].m_artifactPoolShare =
            1.0 / (gpGame->m_playerCount + gpGame->m_deadPlayerCount);
    }
    gpCurPlayer->m_artifactValue =
        static_cast<float>(artifactTotal8 / AI_ARTIFACT_POOL_DIVISOR);

    for (generalIndex4 = 0; generalIndex4 < 72; generalIndex4++)
        gpGame->m_castleRecs[generalIndex4].m_threat = 0;
    for (generalIndex4 = 0; generalIndex4 < gpCurPlayer->m_townCount; generalIndex4++)
        gpGame->m_castleRecs[gpCurPlayer->m_townIds[generalIndex4]].m_threat = 0;

    memset(gaiEnemyHeroReachable, 0, MAP_WIDTH * MAP_HEIGHT);
    for (playerIndex0 = 0; playerIndex0 < gpGame->m_playerCount; playerIndex0++) {
        if (playerIndex0 == giCurPlayer)
            continue;
        if (OnMySide(playerIndex0))
            continue;
        for (innerIndex27 = 0;
             innerIndex27 < gpGame->m_players[playerIndex0].m_heroCount;
             innerIndex27++) {
            hero *threatHeroPtr6 = GetHeroSlot(
                gpGame->m_players[playerIndex0].m_heroIds[innerIndex27]);
            gpSearchArray->SeedPosition(threatHeroPtr6->m_x, threatHeroPtr6->m_y,
                threatHeroPtr6->m_direction, threatHeroPtr6->m_remainingMobility + 100,
                threatHeroPtr6->m_eventFlags & 0x80, 1,
                threatHeroPtr6->m_remainingMobility + 100,
                threatHeroPtr6->m_secondarySkills[0], -1, -1, 0, 0);
            for (x8 = 0; x8 < MAP_WIDTH; x8++) {
                for (y4 = 0; (y4 | 0) < MAP_WIDTH; y4++) {
                    if (gpSearchArray->GetRow(MAP_WIDTH, y4)[x8].visited) {
                        gaiEnemyHeroReachable[MAP_WIDTH * y4 + x8] = 1;
                        if (!(y4 != 0)) {
                        } else {
                            cell12 = gpAdvManager->GetCell(x8, y4 - 1);
                            if (cell12->m_triggerType == AI_OBJECT_TOWN ||
                                (cell12->m_triggerType & 0x7f) == AI_OBJECT_BOAT) {
                                i32 threatTownId8 = gpGame->GetTownId(x8, y4 - 1);
                                if (threatTownId8 == -1) {
                                } else {
                                    town *threatTownPtr10 =
                                        GetCastleSlot(threatTownId8);
                                    if (threatTownPtr10->m_owner == giCurPlayer &&
                                        (threatTownPtr10->m_buildings & 0x40000000)) {
                                        ProbableOutcomeOfBattle(&threatHeroPtr6->m_army,
                                            threatHeroPtr6,
                                            &GetHeroSlot(threatTownPtr10->m_occupyingHeroId)->m_army,
                                            GetHeroSlot(threatTownPtr10->m_occupyingHeroId),
                                            &threatTownPtr10->m_army,
                                            1, threatTownPtr10->m_id, threatTownPtr10->m_owner, winChance11,
                                            outcomeA9, outcomeB6, outcomeC4, outcomeD1, outcomeE14);
                                        if (winChance11 > AI_TOWN_DEFENSE_THRESHOLD)
                                            threatTownPtr10->m_threat = 1;
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
    for (playerIndex0 = 0; playerIndex0 < gpGame->m_playerCount; playerIndex0++) {
        if (playerIndex0 != giCurPlayer) {
            playerPtr27 = &gpGame->m_players[playerIndex0];
            for (innerIndex27 = 0; innerIndex27 < playerPtr27->m_heroCount; innerIndex27++) {
                xCenter19 = GetHeroSlot(playerPtr27->m_heroIds[innerIndex27])->m_x;
                yCenter3 = GetHeroSlot(playerPtr27->m_heroIds[innerIndex27])->m_y;
                for (x8 = xCenter19 - 10; x8 <= xCenter19 + 10; x8++) {
                    for (y4 = yCenter3 - 10; y4 <= yCenter3 + 10; y4++) {
                        if (x8 >= 0 && !(MAP_WIDTH <= x8) && y4 >= 0 && y4 < MAP_HEIGHT) {
                            mineValue4 = abs(abs(y4 - yCenter3) + abs(x8 - xCenter19) - 4) >> 2;
                            if (mineValue4 < gaiTurnValueOfMine[MAP_WIDTH * y4 + x8])
                                gaiTurnValueOfMine[MAP_WIDTH * y4 + x8] =
                                    static_cast<i8>(mineValue4);
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
    for (generalIndex4 = 0; generalIndex4 < gpCurPlayer->m_townCount; generalIndex4++) {
        if (GetCastleSlot(
                gpCurPlayer->m_townIds[generalIndex4])
                ->m_buildings & 0x40000000)
            townCountValue5 += 1.0;
        else
            townCountValue5 += 0.5;
    }
    ownedTownCount19 = static_cast<i32>(townCountValue5);
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
    if (gpCurPlayer->m_aiDifficulty == 2) {
        earlyTurn21 = 8;
        lateTurn40 = 15;
    }
    if (earlyTurn21 > giCurTurn && ownedTownCount19 == 1 &&
        gpCurPlayer->m_heroCount == 1) {
        town *earlyTownPtr29;
        hero *earlyHeroPtr6;
        earlyHeroPtr6 = GetHeroSlot(gpCurPlayer->m_heroIds[0]);
        earlyTownPtr29 = GetCastleSlot(
            gpCurPlayer->m_townIds[0]);
        if (abs(earlyTownPtr29->m_x - earlyHeroPtr6->m_x) +
                abs(earlyTownPtr29->m_y - earlyHeroPtr6->m_y) < 18)
            giMaxHeroesForThisPlayer = 1;
    }
    if (lateTurn40 > giCurTurn && giMaxHeroesForThisPlayer > 2) {
        giMaxHeroesForThisPlayer = 2;
    }
    if (gpCurPlayer->m_aiDifficulty == 2)
        ownedTownCount19++;
    if (ownedTownCount19 + 1 < giMaxHeroesForThisPlayer)
        giMaxHeroesForThisPlayer = ownedTownCount19 + 1;
    if (giMaxHeroesForThisPlayer < gpCurPlayer->m_minimumHeroCount)
        giMaxHeroesForThisPlayer = gpCurPlayer->m_minimumHeroCount;
}

// @semantic
// Instruction stream is identical with relocations masked; all residual rows are
// delinker names for string literals and compiler-emitted floating constants.
VA(0x0043b154, 0x5f4)
void philAI::GetBestBHC(i32 player, BHC &best) {
    i32 townStrength[72];
    i32 desiredStrength[72];
    BHC candidate;
    float value = 1.0f;
    float bestValue = -99.0f;
    i32 totalStrength = 0;
    i32 totalWeight = 0;
    i32 averageStrength;
    i32 i;
    town *townPtr;

    for (i = 0; i < gpCurPlayer->m_townCount; i++) {
        townPtr = &gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]];
        townStrength[i] = FightValueOfStack(&townPtr->m_army, 0, 0, 0, 0, 0) + 400;
        totalStrength += townStrength[i];
        if (townPtr->m_buildings & 0x40000000)
            totalWeight += 10;
        else
            totalWeight += 7;
    }
    if (totalWeight < 1)
        totalWeight = 1;
    averageStrength = totalStrength / totalWeight;
    for (i = 0; i < gpCurPlayer->m_townCount; i++) {
        townPtr = &gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]];
        desiredStrength[i] = ((townPtr->m_buildings & 0x40000000) ? 10 : 7) *
                                 averageStrength +
                             400;
    }

    for (i = 0; i < gpCurPlayer->m_townCount; i++) {
        townPtr = &gpGame->m_castleRecs[gpCurPlayer->m_townIds[i]];
        LogInt("Turns Owned", townPtr->m_turnsOwned, -999, -999, -999, -999, -999, -999);
        if (giCurTurn > 4 && townPtr->m_turnsOwned < 2)
            continue;
        {
            CheckDoMain(0, 0);
            GetBestBuilding(townPtr, candidate, value);
            value = static_cast<float>((100 - Random(0, 10)) /
                                       AI_PURCHASE_RANDOM_DIVISOR * value);
            if (value > bestValue) {
                bestValue = value;
                best = candidate;
            }

            CheckDoMain(0, 0);
            GetBestCreature(townPtr, candidate, value);
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
            if (gpCurPlayer->m_heroCount < giMaxHeroesForThisPlayer &&
                (townPtr->m_buildings & 0x40000000)) {
                GetBestHero(townPtr, candidate, value);
                value = static_cast<float>((100 - Random(0, 10)) /
                                           AI_PURCHASE_RANDOM_DIVISOR * value);
                if (!bHeroBuiltThisTurn && giCurTurn > 5 && value > 0.0f) {
                    if (!((gpCurPlayer->m_income[RES_GOLD] < 1250 ||
                           giMaxHeroesForThisPlayer - 2 <=
                               gpCurPlayer->m_heroCount) &&
                          gpCurPlayer->m_heroCount > 1)) {
                        value += AI_HERO_PURCHASE_BONUS;
                    } else if (gpCurPlayer->m_income[RES_GOLD] >= 1500 &&
                               gpCurPlayer->m_heroCount <
                                   giMaxHeroesForThisPlayer - 1) {
                        value = static_cast<float>(value * AI_HERO_PURCHASE_FACTOR);
                    }
                } else if (gpCurPlayer->m_heroCount == 0) {
                    value += AI_HERO_PURCHASE_BONUS;
                }
                if (value > bestValue) {
                    bestValue = value;
                    best = candidate;
                }
            }
        }
    }
    LogInt("BestBHC ", best.type, static_cast<i32>(bestValue * 100.0f), best.what,
           0, 0, -999, -999);
    if (bestValue < AI_MINIMUM_PURCHASE_VALUE)
        best.type = -1;
}

// @semantic
// Complete & correct; two residuals are /Od codegen-shape picks (verified via scratch cl,
// not source-steerable): (1) the hero-slot 2D access gpGame[0x4a0+player*283+i] — cl emits
// the full player*283 then `+i`; retail strength-reduces to (i-player)+player*284 (identical
// address). (2) the fight-value max `cmp` loads the fresh value where retail loads the
// accumulator (the same operand-memory pick parked on SetupRelativeHeroStrengths).
VA(0x0043b748, 0x11d)
hero *philAI::DetermineHeroToMove(i32 player) {
    i32 val;            // per-hero value
    i32 jb;             // best value
    i32 idx;            // best hero index
    i32 i;
    jb = 0;
    idx = -1;
    if (gpCurPlayer->HasMobileHero()) {
        for (i = 0; i < gpCurPlayer->m_heroCount; i++) {
            val = gpGame->GetPlayerHero(player, i)->m_remainingMobility;
            if (val > jb) {
                jb = val;
                idx = i;
            }
        }
    }
    if (idx >= 0)
        return gpGame->GetPlayerHero(player, idx);
    gpGame->m_players[player].m_currentHero = -1;
    return 0;
}

// @semantic
// The 0x90 frame and all 179 relocation sites align; normalDirTable+1 is the
// sole delinker alias, for the proven .y field. Relocation-masked comparison
// leaves only: the IsEmbarked /Ob1 continuation moved from +0x454..+0x469 to
// the shared tail at +0xe6f..+0xe74 (the exact five-byte span difference), and
// commutative Manhattan addend order at +0x472..+0x493 and +0xbdb..+0xc2a.
// Positive/negative accessor contexts and both source addend orders were tested.
VA(0x0043b865, 0xe7d)
i32 philAI::DetermineTargetPosition(i32 &targetX, i32 &targetY, i32 mobility,
                                    i32 &direction) {
    i32 boatTravelDistanceCounter;
    i32 bestValue;
    i32 scanMinXValue;
    i32 adjacentDirection;
    i32 searchPassIndex;
    i32 columnCounter;
    i32 targetScoreLocal;
    i32 scanMinY;
    i32 travelDistanceRange;
    i32 pathRowCounter;
    i32 remainingMobilityPath;
    i32 x;
    i32 y;
    i32 heroIndexSearch;
    mapCell *candidateCell;
    i32 targetBestXRange;
    i32 scanMaxXLocal;
    i32 doubledMobilityScore;
    i32 scanMaxYCounter;
    i32 candidate;
    i32 targetBestYLocal;
    i32 mapTerrain;
    town *shipyardPtrSearch;
    i32 scanSpacingStep;

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
    mapTerrain = giGroundToTerrain[candidateCell->m_terrainImageIndex];
    if (gpCurAIHero->m_secondarySkills[0] <= HERO_SKILL_LEVEL_BASIC) {
        if (mapTerrain == 2 || mapTerrain == 3)
            mobility = static_cast<i32>(mobility * 1.15);
        if (mapTerrain == 5)
            mobility = static_cast<i32>(mobility * 1.35);
        if (mapTerrain == 7)
            mobility = static_cast<i32>(mobility * 1.25);
    } else if (gpGame->m_mapHeader.width == AI_MAP_SIZE_SMALL) {
        mobility = static_cast<i32>(mobility * 0.9);
    } else if (gpGame->m_mapHeader.width == AI_MAP_SIZE_LARGE) {
        mobility = static_cast<i32>(mobility * 1.05);
    } else if (gpGame->m_mapHeader.width == AI_MAP_SIZE_XLARGE) {
        mobility = static_cast<i32>(mobility * 1.1);
    }
    if (gpCurPlayer->m_aiDifficulty == 2)
        mobility = static_cast<i32>(mobility * 1.15);

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
                              (gpAdvManager->GetCell(x, y)->m_triggerType != AI_OBJECT_HERO &&
                               (gpAdvManager->GetCell(x, y)->m_triggerType != AI_OBJECT_TOWN ||
                                GetCastleSlot(
                                    gpAdvManager->GetCell(x, y)->m_objectMetadata)->m_owner == giCurPlayer)))) ||
                            (searchPassIndex == 1 &&
                             (travelDistanceRange <= 5 ||
                              (gpCurAIHero->m_destinationX == x &&
                               gpCurAIHero->m_destinationY == y) ||
                              ((travelDistanceRange | 0) < 16 &&
                               (gpAdvManager->GetCell(x, y)->m_triggerType == AI_OBJECT_HERO ||
                                (gpAdvManager->GetCell(x, y)->m_triggerType == AI_OBJECT_TOWN &&
                                 GetCastleSlot(
                                     gpAdvManager->GetCell(x, y)->m_objectMetadata)->m_owner != giCurPlayer))))))
                            continue;
                    }

                    candidateCell = gpAdvManager->GetCell(x, y);
                    if (mobility < gpSearchArray->GetNode(x, y).distance) {
                        if (doubledMobilityScore < gpSearchArray->GetNode(x, y).distance) {
                            candidate = 0;
                        } else if (candidateCell->m_triggerType == AI_OBJECT_TOWN ||
                                   candidateCell->m_triggerType == AI_OBJECT_HERO ||
                                   (candidateCell->m_triggerType == AI_OBJECT_BOAT &&
                                    !(gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED))) {
                            candidate = 1;
                        } else {
                            candidate = 0;
                        }
                    } else if ((candidateCell->m_triggerType & 0x80) ||
                               (candidateCell->m_triggerType == AI_OBJECT_BOAT_TRIGGER &&
                                (gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED)) ||
                               (x % scanSpacingStep == 0 && y % scanSpacingStep == 0 &&
                                 (((gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
                                   giGroundToTerrain[candidateCell->m_terrainImageIndex] == 0) ||
                                  (!(gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
                                   giGroundToTerrain[candidateCell->m_terrainImageIndex] != 0))) ||
                               (gpCurPlayer->m_ultimateArtifactHintX == x &&
                                gpCurPlayer->m_ultimateArtifactHintY == y)) {
                        candidate = 1;
                    } else {
                        candidate = 0;
                    }

                    if (candidate && gpCurAIHero->m_boatId != 0xff) {
                        boatTravelDistanceCounter =
                            abs(y - static_cast<u8>(gpCurAIHero->m_unknown2a)) +
                            abs(x - static_cast<u8>(gpCurAIHero->m_boatId));
                        if (gpCurAIHero->m_unknown2b < boatTravelDistanceCounter)
                            candidate = 0;
                    }
                    if (candidate) {
                        for (heroIndexSearch = 0;
                             heroIndexSearch < gpCurPlayer->m_heroCount;
                             heroIndexSearch++) {
                            if (candidateCell->m_triggerType != AI_OBJECT_TOWN &&
                                candidateCell->m_triggerType != AI_OBJECT_HERO &&
                                gpCurPlayer->m_heroIds[heroIndexSearch] !=
                                    static_cast<u8>(gpCurAIHero->m_id) &&
                                gpGame->m_heroRecs[
                                    gpCurPlayer->m_heroIds[heroIndexSearch]].m_x == x &&
                                gpGame->m_heroRecs[
                                    gpCurPlayer->m_heroIds[heroIndexSearch]].m_y == y) {
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
                            gpSearchArray->GetNode(x, y).adjacentMonsterX,
                            gpSearchArray->GetNode(x, y).adjacentMonsterY,
                            gpSearchArray->GetNode(x, y).rvFlag2,
                            gpSearchArray->GetNode(x, y).previousFlags,
                            gpSearchArray->GetNode(x, y).terrain, 2,
                            remainingMobilityPath);
                        targetScoreLocal = (Random(1, 50) + 75) * targetScoreLocal;
                        targetScoreLocal /= 100;
                    } else {
                        targetScoreLocal = -100;
                    }
                    if (targetX == x && targetY == y) {
                        targetScoreLocal = static_cast<i32>(
                            targetScoreLocal * AI_EVENT_HUMAN_VALUE_FACTOR);
                        targetScoreLocal += 20;
                    }

candidate_scored:
                    if ((bestValue | 0) < targetScoreLocal) {
                        targetBestXRange = x;
                        targetBestYLocal = y;
                        bestValue = targetScoreLocal;
                    } else if ((bestValue | 0) == targetScoreLocal && targetScoreLocal == 0 &&
                               !(gpAdvManager->GetCell(x, y)->m_triggerType & 0x80)) {
                        if ((gpAdvManager->GetCell(targetBestXRange, targetBestYLocal)->m_triggerType & 0x80) ||
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
             gpCurPlayer->m_income[0] * 6 +
             gpCurPlayer->m_resources[0])) {
            if (!gbActualShipyardFound)
                giBuildShipyard[giCurPlayer] = static_cast<i8>(giBestShipyardId);
            giBuildBoat[giCurPlayer] = static_cast<i8>(giBestShipyardId);
            giBuildBoatStuffTurn[giCurPlayer] = static_cast<i8>(giCurTurn);
            shipyardPtrSearch = GetCastleSlot(giBestShipyardId);
            bestValue = 123;
            targetBestXRange = shipyardPtrSearch->m_x;
            targetBestYLocal = shipyardPtrSearch->m_y;
            if (gpCurAIHero->m_x == targetBestXRange && gpCurAIHero->m_y == targetBestYLocal)
                gpCurAIHero->m_remainingMobility = 0;
        }
        CheckBuyStuff();
    }

    targetX = targetBestXRange;
    targetY = targetBestYLocal;
    if (gpCurAIHero->m_boatId != 0xff && bestValue <= 0) {
        targetX = static_cast<u8>(gpCurAIHero->m_boatId);
        targetY = static_cast<u8>(gpCurAIHero->m_unknown2a);
    }
    LogInt("Hero, Best RV target XY  current XY", gpCurAIHero->m_owner, bestValue,
           targetX, targetY, gpCurAIHero->m_x, gpCurAIHero->m_y, -999);
    LogStr("\n\n****");
    return bestValue;
}

// @semantic: Complete CFG, frame/slots, and all 84 external relocations align.
// Relocation-masked raw bytes differ only at +0x4ee/+0x4f1 and +0x62a/+0x62d:
// MSVC loads the two commutative hero-artifact address operands into eax/ecx in
// the opposite order. The effective addresses agree, as do all 84 relocations.
// Ten guarded TU-state probes peaked at 99.499070 without exact closure; the
// current header state reaches 99.95%. Revisit in the byte-last-mile phase.
VA(0x0043c6e2, 0x791)
void philAI::ProbableOutcomeOfBattle(armyGroup *attacker, hero *attackerHero,
                                     armyGroup *defender, hero *defenderHero,
                                     armyGroup *townArmy, i32 useTown,
                                     i32 townId, i32 enemyPlayer,
                                     float &winChance, i32 &attackerLoss,
                                     i32 &defenderLoss, i32 &attackerRemaining,
                                     i32 &defenderRemaining, i32 &outcomeValue)
{
    i32 unusedValue29;
    i32 attackerArtifacts7 = 0;
    i32 defenderArtifacts18 = 0;
    float defenderFightValue5;
    float attackerFightValue26;
    float defenderPower19;
    float attackerStrength0;
    i32 experienceValue1;
    i32 artifactIndex15;
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
        if (gpCurPlayer->m_aiDifficulty ==
            AI_BATTLE_EASY_DIFFICULTY)
            attackerStrength0 = static_cast<float>(
                attackerStrength0 * AI_BATTLE_EASY_STRENGTH_FACTOR);
    } else {
        attackerStrength0 = attackerFightValue26;
        if (gbHumanPlayer[enemyPlayer] != 0) {
            defenderStrength4 = static_cast<float>(
                defenderStrength4 * AI_BATTLE_HUMAN_DEFENDER_FACTOR);
        } else if (gpCurPlayer->m_aiDifficulty ==
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

    attackerLoss = static_cast<i32>((1.0 - winChance) * attackerRawValue7);
    defenderLoss = static_cast<i32>((+defenderRawValue4) * winChance);
    attackerRemaining = static_cast<i32>(
        (1.0f - winChance) * attackerRawValue7 + attackerLoss * winChance);
    defenderRemaining = static_cast<i32>(
        (1.0f - winChance) * defenderLoss + (+defenderRawValue4) * winChance);

    difficultyFactor5 = static_cast<float>(
        AI_BATTLE_LOSS_FACTOR_BASE -
        gpCurPlayer->m_attentionWeights.upgradeBase);
    outcomeValue = static_cast<i32>(
        -attackerRemaining * difficultyFactor5 * difficultyFactor5);
    if (enemyPlayer >= 0) {
        difficultyFactor5 = static_cast<float>(
            gpCurPlayer->m_attentionWeights.upgradeBase +
            AI_BATTLE_PLAYER_FACTOR_BASE);
        if (gbHumanPlayer[enemyPlayer] != 0)
            outcomeValue = static_cast<i32>(
                defenderRemaining * gfAttackHumanBonus * difficultyFactor5 *
                    difficultyFactor5 + outcomeValue);
        else
            outcomeValue = static_cast<i32>(
                defenderRemaining * gfAttackComputerBonus * difficultyFactor5 *
                    difficultyFactor5 + outcomeValue);
    }
    outcomeValue = static_cast<i32>(
        outcomeValue * gpCurPlayer->m_upgradeValueWeight);

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
        if (gpGame->m_mapHeader.victoryCondition ==
                AI_BATTLE_SPECIAL_ARTIFACT_STATE &&
            static_cast<u8>(attackerHero->m_id) ==
                gpGame->m_mapHeader.victoryConditionValue)
            attackerArtifacts7 += AI_BATTLE_SPECIAL_ARTIFACT_VALUE;

        outcomeValue = static_cast<i32>(
            outcomeValue - (AI_BATTLE_FULL_CHANCE - winChance) *
                (attackerArtifacts7 + AI_BATTLE_ATTACKER_ARTIFACT_BASE));
        experienceValue1 = gpGame->ExperienceValueOfStack(defender, defenderHero);
        outcomeValue = static_cast<i32>(
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
        if (gpGame->m_mapHeader.lossCondition ==
                AI_BATTLE_SPECIAL_ARTIFACT_STATE &&
            static_cast<u8>(defenderHero->m_id) ==
                gpGame->m_mapHeader.lossConditionValue)
            defenderArtifacts18 += AI_BATTLE_SPECIAL_ARTIFACT_VALUE;

        if (gbHumanPlayer[defenderHero->m_owner] != 0)
            attackBonus13 = gfAttackHumanBonus;
        else
            attackBonus13 = gfAttackComputerBonus;
        outcomeValue = static_cast<i32>(
            winChance * ((defenderArtifacts18 + AI_BATTLE_DEFENDER_ARTIFACT_BASE) *
                         attackBonus13) + outcomeValue);
    }

    if (giDebugLevel >= AI_BATTLE_DEBUG_LEVEL) {
        LogInt("POBA", static_cast<i32>(attackerStrength0),
               static_cast<i32>(defenderStrength4),
               static_cast<i32>(gpCurPlayer->m_attentionWeights.upgradeBase *
                                AI_BATTLE_PERCENT_SCALE),
               0, attackerArtifacts7, defenderArtifacts18,
               static_cast<i32>(gpCurPlayer->m_upgradeValueWeight));
        LogInt("POB", static_cast<i32>(attackerFightValue26),
               static_cast<i32>(defenderFightValue5),
               static_cast<i32>(winChance * AI_BATTLE_PERCENT_SCALE),
               defenderLoss, attackerRemaining, defenderRemaining, outcomeValue);
    }
}

VA(0x0043ce73, 0x1e)
float philAI::GetOddsOfWinning(i32) {
    return 1.0f;
}

// @early-stop
// @early-stop-reloc-only: Current PHILAI.cpp/header epoch: relocation-masked bytes are
// exact across all 0x826 bytes and all 75 ordered relocation sites/types align.
// The sole raw byte difference, +0x801, is the proven __adjust_fdiv versus
// iLeftRightSave+0x10 delinker spelling; constant-pool/addended references and
// jump-table locals also have equivalent identities. Revisit only after the
// PHILAI source/TU/header or comparison epoch.
VA(0x0043ce91, 0x826)
void philAI::ValueOfBuyingBuilding(town *townPtr, i32 building, i32 &resourceValue,
                                   float &benefitCost) {
    i32 currentTownRace;
    i32 dwellingTotal;
    i32 highestDwellingId;
    i32 currentOccupiedSlots;
    i32 mageGuildLevelLocal;
    float adjustedValue;
    i32 creatureLocated;
    i32 indexBuilding;
    i32 buildingLevel;
    i32 currentCreatureType;
    i32 costsByResource[7];
    float estimatedAttackChance;
    float enemyStrengthLocal;
    i32 currentAttackTurns;
    i32 projectedAttackValue;
    i32 estimatedAttackWeeks;
    float dangerRating;
    currentTownRace = townPtr->m_type;
    dwellingTotal = 0;
    highestDwellingId = -1;
    for (indexBuilding = AI_BUILDING_FIRST_DWELLING;
         indexBuilding < AI_BUILDING_LAST_DWELLING;
         indexBuilding++) {
        if (townPtr->m_buildings & (1 << indexBuilding)) {
            dwellingTotal++;
            highestDwellingId = indexBuilding;
        }
    }
    currentOccupiedSlots = 0;
    for (indexBuilding = 0; indexBuilding < AI_TOWN_ARMY_SLOTS; indexBuilding++) {
        if (townPtr->m_army.m_quantities[indexBuilding] > 0)
            currentOccupiedSlots++;
    }
    if (building == AI_BUILDING_MAGE_GUILD)
        mageGuildLevelLocal = townPtr->m_buildState + 1;
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
        townPtr->m_buildState > 1) {
        adjustedValue -= static_cast<float>(GetBuildingBaseResourceValue(
            currentTownRace, building,
            townPtr->m_buildState));
    }

    switch (building) {
    case AI_BUILDING_SPECIAL_SIX:
        if (townPtr->m_unknown37 != 0)
            adjustedValue = -99.0f;
        break;
    case AI_BUILDING_MAGE_GUILD:
        if (townPtr->m_type == 0 ||
            townPtr->m_type == 1) {
            if ((townPtr->m_buildState >= 4 && giCurTurn < 40) ||
                (townPtr->m_buildState >= 3 && giCurTurn < 30) ||
                (townPtr->m_buildState >= 2 && giCurTurn < 20))
                adjustedValue = static_cast<float>(adjustedValue * 0.4);
            else
                adjustedValue = static_cast<float>(adjustedValue * 0.65);
        } else if (townPtr->m_buildState == 0) {
            adjustedValue = static_cast<float>(adjustedValue * 1.2);
        } else if ((townPtr->m_buildState >= 4 && giCurTurn < 35) ||
                   (townPtr->m_buildState >= 3 && giCurTurn < 25) ||
                   (townPtr->m_buildState >= 2 && giCurTurn < 15)) {
            adjustedValue = static_cast<float>(adjustedValue * 0.55);
        }
        break;
    case AI_BUILDING_SPECIAL_ONE:
        if (townPtr->m_type == 5 &&
            (townPtr->m_buildings &
             (1 << AI_BUILDING_THIRD_DWELLING)))
            adjustedValue = 1500.0f;
        else if (giCurTurn < 21)
            adjustedValue = 0.0f;
        break;
    case AI_BUILDING_SPECIAL_SEVEN:
        if (giCurTurn < 3 &&
            !(townPtr->m_buildings &
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
        if (townPtr->m_type == 0 ||
            townPtr->m_type == 2 ||
            townPtr->m_type == 1 ||
            townPtr->m_type == 5) {
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
            (gpCurPlayer->m_attentionWeights.buildingValue + 0.66) * adjustedValue);
        adjustedValue = static_cast<float>(
            (gpCurPlayer->m_attentionWeights.upgradeBase * 2.0f + 0.33) * adjustedValue);
        adjustedValue = static_cast<float>((dwellingTotal * 0.33 + 0.66) * adjustedValue);
        if ((townPtr->m_type != 0 ||
             !(townPtr->m_buildings &
               (1 << AI_BUILDING_SECOND_DWELLING))) &&
            gpGame->m_day < 6)
            adjustedValue = 0.0f;
        break;
    case AI_BUILDING_SPECIAL_TWO:
        if (townPtr->m_type == 5)
            break;
        if ((townPtr->m_type == 0 &&
             (townPtr->m_buildings &
              (1 << AI_BUILDING_THIRD_DWELLING))) ||
            (townPtr->m_type == 2 &&
             (townPtr->m_buildings &
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
                if (gDwellingType[townPtr->m_type]
                                 [building - AI_BUILDING_FIRST_DWELLING] ==
                    townPtr->m_army.m_creatureTypes[indexBuilding]) {
                    creatureLocated = 1;
                }
            }
            if (!creatureLocated)
                break;
        }
        adjustedValue = static_cast<float>(
            (gpCurPlayer->m_attentionWeights.buildingValue + 0.66) * adjustedValue);
        adjustedValue = static_cast<float>(
            (gpCurPlayer->m_attentionWeights.upgradeBase * 2.0f + 0.33) * adjustedValue);
        adjustedValue = static_cast<float>(
            (1.0 - gpCurPlayer->BuildingsOwned(currentTownRace, building, 0) * 0.05) * adjustedValue);
        if (building - AI_BUILDING_FIRST_DWELLING < highestDwellingId)
            adjustedValue = static_cast<float>((1.66 - dwellingTotal * 0.33) * adjustedValue);
        if (townPtr->m_buildings & 0x10)
            adjustedValue = static_cast<float>(adjustedValue * 1.1);
        for (buildingLevel = 0; buildingLevel < AI_DWELLING_LEVELS; buildingLevel++) {
            currentCreatureType =
                gDwellingType[townPtr->m_type][buildingLevel];
            if ((townPtr->m_buildings &
                 (1 << (buildingLevel + AI_BUILDING_FIRST_DWELLING))) &&
                townPtr->m_garrison[buildingLevel] > 0 &&
                gMonsterDatabase[gDwellingType[townPtr->m_type]
                                              [building - AI_BUILDING_FIRST_DWELLING]].iconIndex <
                    gMonsterDatabase[currentCreatureType].iconIndex * 1.2) {
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
    if (townPtr->m_originalOwner != giCurPlayer)
        adjustedValue = static_cast<float>(adjustedValue * 0.85);
    if (adjustedValue < 0.0f)
        adjustedValue = 0.0f;
    GetBuildingCost(currentTownRace, building, costsByResource,
                    building == AI_BUILDING_MAGE_GUILD ?
                        townPtr->m_buildState : 0);
    adjustedValue = FutureDeflator(costsByResource) * adjustedValue;
    resourceValue = static_cast<i32>(adjustedValue);
    benefitCost = adjustedValue / RVConversion(costsByResource);
}

// @early-stop
// @early-stop-reloc-only: Current PHILAI.cpp/header epoch: all 0x19b bytes match after
// masking 17 ordered relocation sites. The sole raw difference is byte +0xd8,
// inside the proven __adjust_fdiv versus iLeftRightSave+0x10 delinker spelling;
// the remaining constant/string rows are identity-only. The earlier FPU
// scheduling residual is absent in this epoch. Revisit only after the PHILAI
// source/TU/header or comparison epoch.
VA(0x0043d6b7, 0x19b)
void philAI::GetBestBuilding(town *t, BHC &bhc, float &fOut) {
    float score;        // -0x18
    i32 node;           // i
    float nb;           // best randomized BC -0x10
    float kn;           // best raw BC -0xc
    i32 jb;             // best building idx -0x8
    float idx;          // raw BC -0x4
    i32 cost;
    nb = -99.0f;
    kn = -99.0f;
    jb = -1;
    for (node = 0; node < 0x20; node++) {
        if (!(t->m_buildings & (1 << node)) ||
            (node == 0 && t->m_buildState < 5)) {
            if (CanBuild(t, node)) {
                ValueOfBuyingBuilding(t, node, cost, idx);
                if (gpCurPlayer->m_aiDifficulty == 1)
                    cost = static_cast<i32>(cost * 1.3);
                score = (Random(1, 5) + 0x5f) * idx / 100.0f;
                if (score > kn) {
                    jb = node;
                    nb = idx;
                    kn = score;
                }
                if (giDebugLevel >= 5) {
                    sprintf(gText, "Town:%2d  Building: % 18s   Raw BC = %8.2f,  RandBC = %8.2f.",
                            t->m_id, GetBuildingName(t->m_type, node), idx, score);
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

// @semantic
// Retail structure is recovered: the 0x84-byte frame, monster/cost evaluation,
// visiting-hero and garrison ranged-stack passes, danger cube, deflator, and final
// ratio all align. All semantic locals occupy their retail slots (-0x4 through
// -0x50); residual rows are local constant/interior gMonsterDatabase identities
// and a zero-length inline continuation jump.
VA(0x0043d852, 0x306)
void philAI::ValueOfBuyingCreature(town *townPtr, i32 creature, i32 &resourceValue,
                                   i32 purchaseCount, float &benefitCost) {
    i32 score;
    i32 weeksBeforeAttack;
    i32 missileStacks;
    i32 creatureCosts[AI_PURCHASE_RESOURCE_COUNT];
    float danger;
    i32 creatureValue;
    float attackChance;
    i32 purchaseCost;
    float enemyStrength;
    float riskFactor;
    hero *visitingHeroCandidate;
    i32 turnsUntilAttack;
    i32 garrisonSlot;
    i32 visitingArmySlot;

    missileStacks = 0;
    GetMonsterCost(creature, creatureCosts);
    purchaseCost = RVConversion(creatureCosts) * purchaseCount;
    creatureValue = static_cast<i32>(
        gMonsterDatabase[creature].fightValue * purchaseCount *
        gpCurPlayer->m_upgradeValueWeight);

    if (townPtr->m_occupyingHeroId != AI_TROOP_EMPTY_SLOT) {
        visitingHeroCandidate = &gpGame->m_heroRecs[townPtr->m_occupyingHeroId];
        creatureValue = static_cast<i32>(
            creatureValue * AI_CREATURE_VISITING_HERO_FACTOR);
        if (gMonsterDatabase[creature].race == visitingHeroCandidate->m_cursorType)
            creatureValue = static_cast<i32>(
                creatureValue * AI_CREATURE_SAME_RACE_FACTOR);
        if (gMonsterDatabase[creature].attributes & AI_CREATURE_PURCHASE_RANGED_ATTRIBUTE) {
            for (visitingArmySlot = 0;
                 visitingArmySlot < AI_CREATURE_PURCHASE_ARMY_SLOT_COUNT;
                 visitingArmySlot++) {
                if (visitingHeroCandidate->m_army.m_creatureTypes[visitingArmySlot] !=
                        AI_TROOP_EMPTY_SLOT &&
                    (gMonsterDatabase[visitingHeroCandidate->m_army.
                                          m_creatureTypes[visitingArmySlot]].attributes &
                     AI_CREATURE_PURCHASE_RANGED_ATTRIBUTE)) {
                    missileStacks++;
                }
            }
            creatureValue = static_cast<i32>(
                (AI_CREATURE_RANGED_BASE_FACTOR -
                 missileStacks * AI_CREATURE_RANGED_STACK_FACTOR) * creatureValue);
        }
        creatureValue = static_cast<i32>(
            (gpGame->m_players[townPtr->m_owner].m_attentionWeights.upgradeBase +
             AI_CREATURE_BALANCE_BASE) * creatureValue);
    }

    if (gMonsterDatabase[creature].attributes & AI_CREATURE_PURCHASE_RANGED_ATTRIBUTE) {
        for (garrisonSlot = 0; garrisonSlot < AI_CREATURE_PURCHASE_ARMY_SLOT_COUNT;
             garrisonSlot++) {
            if (townPtr->m_army.m_creatureTypes[garrisonSlot] != AI_TROOP_EMPTY_SLOT &&
                (gMonsterDatabase[townPtr->m_army.m_creatureTypes[garrisonSlot]].attributes &
                 AI_CREATURE_PURCHASE_RANGED_ATTRIBUTE)) {
                missileStacks++;
            }
        }
        creatureValue = static_cast<i32>(
            (AI_CREATURE_RANGED_BASE_FACTOR -
             missileStacks * AI_CREATURE_RANGED_STACK_FACTOR) * creatureValue);
    }

    LikelihoodOfEnemyAttacking(townPtr, 0, attackChance, enemyStrength,
                                turnsUntilAttack, score, weeksBeforeAttack, danger);
    riskFactor = static_cast<float>(danger + AI_CREATURE_DANGER_BASE);
    creatureValue = static_cast<i32>(
        riskFactor * riskFactor * riskFactor * creatureValue);
    creatureValue = static_cast<i32>(FutureDeflator(creatureCosts) * creatureValue);
    resourceValue = creatureValue;
    benefitCost = static_cast<float>(resourceValue) / purchaseCost;
}

// @semantic
// The instruction stream and all external targets align. Remaining differences are
// four delinked interior names for gMonsterDatabase: all four retail operands resolve
// to 0x4faeb2 (gMonsterDatabase + 2), while our COFF expresses the same address as
// the gMonsterDatabase base relocation plus addend 2. Other residuals are local
// literal/string identities and equivalent x87 `fcom; fstp` versus `fst; fcomp`.
VA(0x0043db58, 0x35b)
void philAI::GetBestCreature(town *townPtr, BHC &best, float &bestValue) {
    i32 bestDwelling;
    i32 purchaseCount;
    float bestRaw;
    float bestRandomized;
    i32 dwelling;
    i32 creature;
    i32 leastArmyValue;
    i32 canJoin;
    i32 armyIndex;
    i32 currentPurchaseCount;
    i32 resourceValue;
    float rawValue;
    float randomizedValue;

    bestDwelling = AI_CREATURE_PURCHASE_NO_SLOT;
    purchaseCount = 0;
    bestRaw = AI_PURCHASE_INITIAL_VALUE;
    bestRandomized = AI_PURCHASE_INITIAL_VALUE;
    for (dwelling = 0; dwelling < AI_CREATURE_PURCHASE_DWELLING_COUNT; dwelling++) {
        creature = gDwellingType[townPtr->m_type][dwelling];
        leastArmyValue = AI_CREATURE_PURCHASE_VALUE_LIMIT;
        if ((townPtr->m_buildings & (1 << (dwelling + AI_BUILDING_FIRST_DWELLING))) &&
            townPtr->m_garrison[dwelling] > 0) {
            canJoin = 0;
            for (armyIndex = 0; armyIndex < AI_CREATURE_PURCHASE_ARMY_SLOT_COUNT; armyIndex++) {
                if (townPtr->m_army.m_creatureTypes[armyIndex] == AI_TROOP_EMPTY_SLOT ||
                    townPtr->m_army.m_creatureTypes[armyIndex] == creature) {
                    canJoin = 1;
                }
            }
            for (armyIndex = 0; armyIndex < AI_CREATURE_PURCHASE_ARMY_SLOT_COUNT; armyIndex++) {
                if (townPtr->m_army.m_creatureTypes[armyIndex] != AI_TROOP_EMPTY_SLOT &&
                    gMonsterDatabase[townPtr->m_army.m_creatureTypes[armyIndex]].randomValue <
                        leastArmyValue) {
                    leastArmyValue =
                        gMonsterDatabase[townPtr->m_army.m_creatureTypes[armyIndex]].randomValue;
                }
            }
            if (leastArmyValue < gMonsterDatabase[creature].randomValue &&
                gMonsterDatabase[creature].randomValue > AI_CREATURE_PURCHASE_EXPENSIVE_VALUE) {
                canJoin = 1;
            }
            if (dwelling == 0 && townPtr->m_occupyingHeroId != AI_TROOP_EMPTY_SLOT &&
                ((townPtr->m_buildings & TOWN_BUILDING_UPGRADED_DWELLING_5) ||
                 (townPtr->m_buildings & TOWN_BUILDING_DWELLING_5) ||
                 (townPtr->m_buildings & TOWN_BUILDING_UPGRADED_DWELLING_6) ||
                 (townPtr->m_buildings & TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6) ||
                 (townPtr->m_buildings & TOWN_BUILDING_DWELLING_6))) {
                canJoin = 0;
            }
            if (canJoin) {
                currentPurchaseCount = CreaturesToBuy(townPtr, dwelling);
                if (currentPurchaseCount > 0) {
                    ValueOfBuyingCreature(townPtr, creature, resourceValue,
                                          currentPurchaseCount, rawValue);
                    if (gpCurPlayer->m_aiDifficulty == 0)
                        resourceValue = static_cast<i32>(resourceValue *
                                                         AI_CREATURE_EASY_COST_FACTOR);
                    if (townPtr->m_threat != 0)
                        resourceValue <<= 1;
                    randomizedValue = static_cast<float>(
                        (Random(1, 10) + AI_CREATURE_RANDOM_BASE) * rawValue /
                        AI_PURCHASE_RANDOM_DIVISOR);
                    if (randomizedValue > bestRandomized) {
                        bestDwelling = dwelling;
                        bestRaw = rawValue;
                        bestRandomized = randomizedValue;
                        purchaseCount = currentPurchaseCount;
                    }
                    if (giDebugLevel >= AI_PURCHASE_VALUE_DEBUG_LEVEL) {
                        sprintf(gText,
                                "Town:%2d  Creature: % 18s   Raw BC = %8.2f,  RandBC = %8.2f.",
                                townPtr->m_id, GetMonsterName(creature), rawValue,
                                randomizedValue);
                        LogStr(gText);
                    }
                }
            }
        }
    }
    best.pTown = townPtr;
    best.type = AI_PURCHASE_CREATURE;
    best.what = bestDwelling;
    best.num = purchaseCount;
    bestValue = bestRaw;
}

VA(0x0043deb3, 0x48)
i32 philAI::CreaturesToBuy(town *t, i32 level) {
    i32 nGarrison = t->m_garrison[level];
    return CreaturesToBuy((i32)gDwellingType[t->m_type][level], nGarrison);
}

// @semantic: Complete 0x5f-byte CFG, frame/slots, and the sole relocation align.
// At normalized instruction 17 ours loads b, compares n, and skips on jle;
// retail loads n, compares b, and skips on jge. Reversed operands and a | 0
// accumulator pin were neutral; scratch `<`/`>` and calling-convention probes
// emitted the same choice. Revisit in the byte-last-mile phase.
VA(0x0043defb, 0x5f)
i32 philAI::CreaturesToBuy(i32 a, i32 b) {
    i32 n = MaxBuyableCreatures(a);
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
i32 philAI::MaxBuyableCreatures(i32 level) {
    i32 cost7[7];
    i32 res;
    i32 i;
    GetMonsterCost(level, cost7);
    for (i = 0; i < 7; i++) {
        if (cost7[i] == 0)
            res = 0x270f;
        else if (gpCurPlayer->m_resources[i] > 0)
            res = gpCurPlayer->m_resources[i] / cost7[i];
        else
            res = 0;
    }
    return res;
}

// @semantic
// Hero experience, artifact filtering, player factors, class/town preference,
// deflation, benefit/cost output, and the magic/non-magic build-state polarity are
// reconstructed. The 0x50 frame and all 27 relocations agree. The typed 0.16 and
// 2.0 constants retain retail's unfolded x87 arithmetic.
VA(0x0043dff6, 0x2b2)
void philAI::ValueOfBuyingHero(town *townPtr, hero *heroPtr, i32 &resourceValue,
                               float &benefitCost) {
    i32 costValue6;
    i32 costs2[AI_PURCHASE_RESOURCE_COUNT];
    i32 artifactIndex10;
    i32 magicHero6;
    i32 purchaseScratch10;
    i32 value27;

    costs2[0] = 0;
    costs2[1] = 0;
    costs2[2] = 0;
    costs2[3] = 0;
    costs2[4] = 0;
    costs2[5] = 0;
    costs2[6] = AI_HERO_PURCHASE_GOLD_COST;
    costValue6 = RVConversion(costs2);
    value27 = heroPtr->m_experience + AI_HERO_PURCHASE_EXPERIENCE_BASE;
    for (artifactIndex10 = 0;
         artifactIndex10 < AI_BATTLE_ARTIFACT_SLOT_COUNT;
         artifactIndex10++) {
        if (heroPtr->m_artifacts[artifactIndex10] >= 0 &&
            heroPtr->m_artifacts[artifactIndex10] < ARTIFACT_EDITOR_ANY_ULTIMATE &&
            heroPtr->m_artifacts[artifactIndex10] != ARTIFACT_MAGIC_BOOK) {
            value27 += gArtifactBaseRV[heroPtr->m_artifacts[artifactIndex10]];
        }
    }
    value27 += heroPtr->m_experience / 2;
    value27 = static_cast<i32>(
        (gpCurPlayer->m_attentionWeights.heroValue + 1.0 -
         gpCurPlayer->m_attentionWeights.upgradeBase) * value27);
    magicHero6 = heroPtr->m_cursorType == FACTION_SORCERESS ||
                 heroPtr->m_cursorType == FACTION_WARLOCK ||
                 heroPtr->m_cursorType == FACTION_WIZARD ||
                 heroPtr->m_cursorType == FACTION_NECROMANCER;
    if (townPtr->m_type == heroPtr->m_cursorType) {
        value27 *= AI_HERO_PURCHASE_SAME_RACE_FACTOR + AI_ATTENTION_IDENTITY;
    } else if ((townPtr->m_buildState >= 2 && magicHero6) ||
               (townPtr->m_buildState < 2 && !magicHero6)) {
        value27 *= AI_HERO_PURCHASE_SAME_RACE_FACTOR /
                       AI_HERO_PURCHASE_CLASS_DIVISOR +
                   AI_ATTENTION_IDENTITY;
    } else {
        value27 *= AI_ATTENTION_IDENTITY -
                   AI_HERO_PURCHASE_SAME_RACE_FACTOR /
                       AI_HERO_PURCHASE_CLASS_DIVISOR;
    }
    value27 = static_cast<i32>(FutureDeflator(costs2) * value27);
    benefitCost = static_cast<float>(value27) / costValue6;
    resourceValue = value27;
}

// @semantic
// Candidate traversal, scoring, best-choice updates, logging, BHC output, frame,
// and the occupied-map-cell penalty align. The eight semantic locals occupy the
// retail -0x4 through -0x20 slots. Residuals are literal identities, an equivalent
// x87 store/compare, and map-index evaluation order.
VA(0x0043e2a8, 0x1b1)
void philAI::GetBestHero(town *townPtr, BHC &best, float &bestValue) {
    float benefitCost;
    i32 bestHeroIndex;
    i32 heroIndex;
    float bestScore;
    float bestRawLocal;
    float randomizedScore;
    i32 resourceValue;
    hero *heroPtr;

    bestHeroIndex = AI_CREATURE_PURCHASE_NO_SLOT;
    bestRawLocal = AI_PURCHASE_INITIAL_VALUE;
    bestScore = AI_PURCHASE_INITIAL_VALUE;
    for (heroIndex = 0; heroIndex < HERO_AVAILABLE_SLOT_COUNT; heroIndex++) {
        heroPtr = &gpGame->m_heroRecs[
            gpCurPlayer->m_availableHeroIds[heroIndex]];
        ValueOfBuyingHero(townPtr, heroPtr, resourceValue, benefitCost);
        randomizedScore = static_cast<float>(
            (Random(1, 10) + AI_HERO_PURCHASE_RANDOM_BASE) * benefitCost /
            AI_PURCHASE_RANDOM_DIVISOR);
        if (randomizedScore > bestScore) {
            bestHeroIndex = heroIndex;
            bestRawLocal = benefitCost;
            bestScore = randomizedScore;
        }
        if (giDebugLevel >= AI_PURCHASE_VALUE_DEBUG_LEVEL) {
            sprintf(gText,
                    "Town:%2d  Hero    : % 15i   Raw BC = %8.2f,  RandBC = %8.2f.",
                    townPtr->m_id, heroIndex, benefitCost, randomizedScore);
            LogStr(gText);
        }
    }
    best.pTown = townPtr;
    best.type = AI_PURCHASE_HERO;
    best.what = bestHeroIndex;
    bestValue = bestRawLocal;
    if (gpGame->m_worldMap.cells[
            gpGame->m_worldMap.width * townPtr->m_y + townPtr->m_x].m_triggerType ==
        AI_OBJECT_HERO) {
        bestValue -= AI_HERO_PURCHASE_CELL_PENALTY;
    }
}

VA(0x0043e459, 0x65)
void philAI::LikelihoodOfEnemyAttacking(town *, hero *, float &chanceA, float &chanceB,
                                        i32 &nAttack, i32 &nValue, i32 &nWeeks, float &fOut) {
    chanceA = 0.15f;
    chanceB = 0.6f;
    nAttack = 3000;
    nValue = (i32)((float)nAttack * chanceA);
    nWeeks = 6;
    fOut = chanceB * chanceA;
}

VA(0x0043e4be, 0x1a)
i32 philAI::MeanRVOfUnexploredTerritory(i32) { return 0; }

// @early-stop
// @early-stop-reloc-only: Current PHILAI.cpp/header epoch: all 0x1d8 code bytes
// match after masking 45 ordered relocation sites. Correcting the final identity
// literal to the modeled float removed the sole opcode-width residual; remaining
// rows are compiler constant and division-guard identities. Revisit only after
// PHILAI source/TU/header or comparison state changes.
VA(0x0043e4d8, 0x1d8)
void philAI::GetGameAttentionValue(i32 player) {
    playerAttentionWeights *attention =
        &gpGame->m_players[player].m_attentionWeights;
    attention->gameWeightA = static_cast<float>(
        Random(0, 100) / AI_ATTENTION_RANDOM_DIVISOR + AI_ATTENTION_RANDOM_BASE);
    attention->gameWeightB = static_cast<float>(
        Random(0, 100) / AI_ATTENTION_RANDOM_DIVISOR + AI_ATTENTION_RANDOM_BASE);
    attention->gameWeightB *=
        (AI_ATTENTION_IDENTITY_FLOAT + AI_ATTENTION_PLAYER_CENTER) /
        AI_ATTENTION_NORMALIZER;
    attention->gameWeightB *=
        (AI_ATTENTION_UPPER_BOUND - AI_ATTENTION_IDENTITY_FLOAT) /
        AI_ATTENTION_NORMALIZER;
    attention->gameWeightA *=
        (AI_ATTENTION_IDENTITY_FLOAT + AI_ATTENTION_PLAYER_CENTER) /
        AI_ATTENTION_NORMALIZER;
    attention->gameWeightB = static_cast<float>(
        ((AI_ATTENTION_PLAYER_CENTER - gpGame->m_playerCount) *
             AI_ATTENTION_WEIGHT_B_PLAYER_FACTOR + 1.0) * attention->gameWeightB);
    attention->gameWeightA = static_cast<float>(
        ((AI_ATTENTION_PLAYER_CENTER - gpGame->m_playerCount) *
             AI_ATTENTION_WEIGHT_A_PLAYER_FACTOR + 1.0) * attention->gameWeightA);
    attention->gameRemainder =
        static_cast<float>((AI_ATTENTION_IDENTITY_FLOAT - attention->gameWeightB) -
                           attention->gameWeightA);
}

VA(0x0043e6b0, 0xf2)
void philAI::GetTurnAttentionValue(i32 player) {
    playerAttentionWeights *ptr = &gpGame->m_players[player].m_attentionWeights;
    ptr->gameWeightA = 0.4f;
    ptr->gameWeightB = 0.3f;
    ptr->gameRemainder = 0.3f;
    ptr->buildingValue = ptr->gameWeightA;
    ptr->heroValue = ptr->gameWeightB;
    ptr->upgradeBase = ptr->gameRemainder;
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
    ptr->heroValue = ptr->heroValue * factor;
}

VA(0x0043e7a2, 0xa6)
i32 philAI::RVConversion(i32 *const p) {
    // explicit left-grouping: a flat `+` chain is reassociated by /Od to a fixed
    // canonical order; nesting forces strict left-to-right (retail) evaluation.
    return (i32)((((((((float)p[RES_ORE]     * gafAITurnCostResource[RES_ORE])
               + (float)p[RES_GEMS]    * gafAITurnCostResource[RES_GEMS])
               + (float)p[RES_MERCURY] * gafAITurnCostResource[RES_MERCURY])
               + (float)p[RES_GOLD]    * gafAITurnCostResource[RES_GOLD])
               + (float)p[RES_WOOD]    * gafAITurnCostResource[RES_WOOD])
               + (float)p[RES_SULFUR]  * gafAITurnCostResource[RES_SULFUR])
               + (float)p[RES_CRYSTAL] * gafAITurnCostResource[RES_CRYSTAL]);
}

// @semantic: All 61 normalized instructions and all 4 ordered relocations align.
// Raw bytes differ only at +0xa4/+0xa7: retail loads maxT then turns for the
// x87 maximum comparison, while ours loads turns then maxT. The equivalent
// `maxT < turns` spelling was neutral; guarded probes reached disposable 100%
// but failed global eligibility. Revisit in the byte-last-mile phase.
VA(0x0043e848, 0xd0)
float philAI::TurnsToBuy(i32 *const p) {
    float maxT = 0;
    i32 i;
    float turns;
    for (i = 0; i < 7; i++) {
        if (gpCurPlayer->m_resources[i] < p[i]) {
            if (gpCurPlayer->m_income[i] > 0)
                turns = static_cast<float>((p[i] - gpCurPlayer->m_resources[i]) /
                                           gpCurPlayer->m_income[i] + 1);
            else
                turns = 99.0f;
            maxT = turns > maxT ? turns : maxT;
        }
    }
    return maxT;
}

// @semantic
// The 0xc8-byte frame and full CFG align: strategic/event deltas, chained live
// chances, adjacent-monster handling, mobility curve, embarked bonus, and debug log
// are present. Normalized residual code is one MAP_WIDTH operand-load choice; the
// other rows are constant-pool and delinked string identities.
VA(0x0043e918, 0x62d)
i32 philAI::RVOfPosition(i32 x, i32 y, i32 hasEvent, i32 eventX, i32 eventY,
                         i32 hasStrategicEvent, i32 strategicX, i32 strategicY,
                         i32 eventMode, i32 extraDistance) {
    i32 strategicEventValue;
    i32 targetLiveChance;
    i32 adjacentEventChance;
    u32 triggerType;
    u32 objectType;
    i32 currentStrategicValue;
    i32 targetStrategicValue;
    i32 strategicDelta;
    i32 totalValue;
    i32 primaryEventChance;
    i32 strategicLiveChance;
    i32 adjacentX;
    i32 adjacentY;
    i32 eventValue;
    i32 adjacentEventValue;
    float distanceFactor;
    char debugText[104];
    char *objectName;

    strategicEventValue = 0;
    targetLiveChance = AI_POSITION_FULL_CHANCE;
    adjacentEventChance = AI_POSITION_FULL_CHANCE;
    triggerType = gpAdvManager->GetCell(x, y)->m_triggerType;
    objectType = triggerType & 0x7f;
    primaryEventChance = AI_POSITION_FULL_CHANCE;
    strategicLiveChance = AI_POSITION_FULL_CHANCE;
    targetLiveChance = AI_POSITION_FULL_CHANCE;

    if (abs(x - gpCurAIHero->m_x) <= AI_POSITION_NEARBY_DELTA &&
        abs(y - gpCurAIHero->m_y) <= AI_POSITION_NEARBY_DELTA) {
        currentStrategicValue = 0;
        strategicDelta = 0;
    } else {
        currentStrategicValue = StrategicValueOfPosition(
            gpCurAIHero->m_x, gpCurAIHero->m_y, 0, 1, &targetLiveChance, 0);
        strategicDelta = StrategicValueOfPosition(
            x, y, 0, 1, &targetLiveChance, extraDistance);
        strategicDelta -= currentStrategicValue;
    }
    if (objectType == AI_OBJECT_BOAT_EVENT && strategicDelta < 0)
        strategicDelta = 0;

    totalValue = 0;
    if (hasEvent)
        totalValue += ValueOfEventAtPosition(eventX, eventY, 1, &strategicLiveChance);
    if (hasStrategicEvent) {
        strategicEventValue = StrategicValueOfPosition(
            strategicX, strategicY, 1, 1, &adjacentEventChance, 0);
        if (strategicEventValue < 0)
            totalValue += strategicEventValue;
    }

    if (gpAdvManager->FindAdjacentMonster(x, y, &adjacentX, &adjacentY, -1, -1)) {
        if (StopOnTrigger(gpAdvManager->GetCell(x, y))) {
            adjacentEventValue =
                ValueOfEventAtPosition(adjacentX, adjacentY, 1, &primaryEventChance);
            if (adjacentEventValue < 0)
                totalValue += adjacentEventValue;
            if (strategicLiveChance == AI_POSITION_FULL_CHANCE)
                strategicLiveChance = primaryEventChance;
            else
                strategicLiveChance =
                    strategicLiveChance * primaryEventChance / AI_POSITION_FULL_CHANCE;
        }
    }

    if ((triggerType & 0x80) ||
        (gpCurPlayer->m_ultimateArtifactHintX == x &&
         gpCurPlayer->m_ultimateArtifactHintY == y)) {
        eventValue = ValueOfEventAtPosition(x, y, eventMode, &primaryEventChance);
    } else {
        eventValue = 0;
    }
    if (primaryEventChance < AI_POSITION_FULL_CHANCE)
        strategicDelta = strategicDelta * primaryEventChance / AI_POSITION_FULL_CHANCE;

    if (targetLiveChance < AI_POSITION_MINIMUM_LIVE_CHANCE)
        return AI_POSITION_FAILED_VALUE;
    if (targetLiveChance < AI_POSITION_FULL_CHANCE) {
        eventValue = targetLiveChance * eventValue / AI_POSITION_FULL_CHANCE;
        strategicDelta = strategicDelta * targetLiveChance / AI_POSITION_FULL_CHANCE;
    }
    if (adjacentEventChance < AI_POSITION_MINIMUM_LIVE_CHANCE)
        return AI_POSITION_FAILED_VALUE;
    if (adjacentEventChance < AI_POSITION_FULL_CHANCE) {
        eventValue = adjacentEventChance * eventValue / AI_POSITION_FULL_CHANCE;
        strategicDelta = strategicDelta * adjacentEventChance / AI_POSITION_FULL_CHANCE;
    }
    if (strategicLiveChance < AI_POSITION_FULL_CHANCE) {
        if (totalValue > 0) {
            totalValue = (strategicDelta + totalValue + eventValue) * strategicLiveChance /
                         AI_POSITION_FULL_CHANCE;
        } else {
            totalValue += (strategicDelta + eventValue) * strategicLiveChance /
                          AI_POSITION_FULL_CHANCE;
        }
    } else {
        totalValue += eventValue;
    }

    distanceFactor = static_cast<float>(gpSearchArray->GetRow(y, MAP_WIDTH)[x].distance) /
                     gpCurAIHero->m_mobility;
    if (gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED) {
        distanceFactor = static_cast<float>(
            distanceFactor * AI_POSITION_EMBARKED_DISTANCE_FACTOR +
            AI_POSITION_EMBARKED_DISTANCE_FACTOR);
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_6) {
        distanceFactor *= AI_POSITION_LAND_FACTOR_FAR;
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_5) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_6);
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_4) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_5);
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_3) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_4);
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_2) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_3);
    } else if (distanceFactor > AI_POSITION_LAND_DISTANCE_1) {
        distanceFactor = static_cast<float>(distanceFactor * AI_POSITION_LAND_FACTOR_2);
    }

    totalValue = static_cast<i32>(totalValue / (distanceFactor + AI_POSITION_DISTANCE_BASE));
    strategicDelta = static_cast<i32>(
        strategicDelta * 2 / (distanceFactor + AI_POSITION_STRATEGIC_DISTANCE_BASE));
    if (strategicLiveChance == AI_POSITION_FULL_CHANCE)
        totalValue += strategicDelta;
    if ((gpCurAIHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
        triggerType == AI_OBJECT_BOAT_TRIGGER) {
        totalValue += AI_POSITION_EMBARKED_BOAT_BONUS;
    }

    if (giDebugLevel > AI_POSITION_DEBUG_LEVEL - 1) {
        i32 debugObjectType = triggerType & 0x7f;
        if (debugObjectType > 0 && debugObjectType < AI_POSITION_OBJECT_NAME_COUNT)
            objectName = gQuickViewText[debugObjectType];
        else
            objectName = " ";
        sprintf(debugText, "FUN U :% 15s", objectName);
        LogInt(debugText, x, y, totalValue, eventValue, strategicDelta,
               targetLiveChance * AI_POSITION_FULL_CHANCE, AI_POSITION_DEBUG_UNUSED);
    }
    return totalValue;
}

// @semantic
// At +0x2f9..+0x30d, cl loads MAP_HEIGHT first and emits `cmp [y], eax; jge`
// (19 bytes); retail loads y first and emits `cmp [MAP_HEIGHT], eax; jle`
// (20 bytes). Both are the same signed `y < MAP_HEIGHT` loop condition. A focused
// /Od /Ob1 probe proved all ordinary for-condition spellings canonicalize to the
// current form; the one if/else spelling that flips the operands adds two non-retail
// routing jumps. The other three diff rows are constant-pool/delinker identities,
// and all 99 relocation sites align (`homm2 relocs`: only-base=0).
VA(0x0043ef45, 0xaf9)
i32 philAI::StrategicValueOfPosition(i32 targetX, i32 targetY, i32 immediate,
                                     i32 checkEnemies, i32 *liveChance,
                                     i32 extraDistance) {
    i32 distance15;
    searchArray *search5;
    i32 eventValue13;
    i32 eventFlags8;
    i32 x13;
    i32 enemyMobility19;
    i32 seedMobility2;
    i32 yCounter;
    i32 friendlyTown1;
    mapCell *cell9;
    i32 unusedThreatValue9;
    i32 danger26;
    i32 heroIndex2;
    i32 heroTerrain7;
    i32 shortDistance13;
    i32 penalty;
    i32 allocatedSearch2;
    i32 unusedStrategicValue17;
    i32 targetTerrain29;
    i32 score4;

    if (!extraDistance && !immediate &&
        *reinterpret_cast<i16 *>(reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2) != AI_RV_UNSET) {
        *liveChance = *reinterpret_cast<i16 *>(
            reinterpret_cast<char *>(gaiLiveChanceOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2);
        return *reinterpret_cast<i16 *>(
            reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2);
    }

    score4 = 0;
    allocatedSearch2 = 0;
    friendlyTown1 = 0;
    cell9 = gpAdvManager->GetCell(targetX, targetY);
    if (cell9->m_triggerType == AI_OBJECT_TOWN &&
        GetCastleSlot(cell9->m_objectMetadata)->m_owner == giCurPlayer &&
        (GetCastleSlot(cell9->m_objectMetadata)->m_buildings & 0x40000000)) {
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
    if (eventFlags8 && gpAdvManager->GetCell(targetX, targetY)->m_triggerType == 0x1c)
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
            if ((!immediate && (cell9->m_triggerType & 0x80) &&
                 search5->GetNode(x13, yCounter).distance <= shortDistance13) ||
                (checkEnemies && cell9->m_triggerType == AI_OBJECT_HERO)) {
                CheckDoMain(0, 0);
                eventValue13 = static_cast<i32>((ValueOfEventAtPosition(x13, yCounter, 0, &iDummy) * 25) /
                    (search5->GetNode(x13, yCounter).distance + 50.0));
                if (!friendlyTown1 || cell9->m_triggerType != AI_OBJECT_HERO || eventValue13 >= 0)
                    score4 += eventValue13;
            }

            if (cell9->m_triggerType == AI_OBJECT_HERO) {
                if (gaiHeroLiveChance[cell9->m_objectMetadata] == AI_RV_UNSET)
                    ValueOfEventAtPosition(x13, yCounter, 0, &iDummy);
                if (gaiHeroLiveChance[cell9->m_objectMetadata] != AI_RV_UNSET &&
                    gaiHeroLiveChance[cell9->m_objectMetadata] < 100) {
                    if (!friendlyTown1) {
                        enemyMobility19 = GetHeroSlot(cell9->m_objectMetadata)->m_mobility;
                        if (gbHumanPlayer[gpGame->m_availableHeroes[cell9->m_objectMetadata]]) {
                            if (search5->GetNode(x13, yCounter).distance + extraDistance <=
                                enemyMobility19) {
                                if (search5->GetNode(x13, yCounter).distance + extraDistance <= 500) {
                                    danger26 = 100 - gaiHeroLiveChance[cell9->m_objectMetadata];
                                } else {
                                    danger26 = ((enemyMobility19 -
                                        (search5->GetNode(x13, yCounter).distance + extraDistance)) + 250) *
                                        (100 - gaiHeroLiveChance[cell9->m_objectMetadata]) / enemyMobility19;
                                }
                            } else {
                                danger26 = static_cast<i32>((100 - gaiHeroLiveChance[cell9->m_objectMetadata]) * 0.9);
                            }
                        } else {
                            danger26 = ((enemyMobility19 + 500) -
                                (search5->GetNode(x13, yCounter).distance + extraDistance)) *
                                (100 - gaiHeroLiveChance[cell9->m_objectMetadata]) /
                                (enemyMobility19 + 500);
                        }
                        *liveChance = (100 - danger26) * *liveChance / 100;
                    }
                }
            }

            if (static_cast<u8>(gpCurAIHero->m_id) != iAlphaMale &&
                (targetX != x13 || targetY != yCounter) &&
                search5->GetNode(x13, yCounter).distance < 1024 &&
                gpAdvManager->GetCell(x13, yCounter)->m_triggerType == AI_OBJECT_HERO &&
                gpAdvManager->GetCell(x13, yCounter)->m_objectMetadata !=
                    static_cast<u8>(gpCurAIHero->m_id) &&
                gpGame->m_availableHeroes[gpAdvManager->GetCell(x13, yCounter)->m_objectMetadata] ==
                    gpCurAIHero->m_owner) {
                penalty = (1024 - search5->GetNode(x13, yCounter).distance) * 600 >> 10;
                if (penalty > 0)
                    score4 -= penalty;
            }
            }
        }
    }

    targetTerrain29 = giGroundToTerrain[gpAdvManager->GetCell(targetX, targetY)->m_terrainImageIndex];
    for (heroIndex2 = 0;
         heroIndex2 < gpCurPlayer->m_heroCount;
         heroIndex2++) {
        if (gpCurPlayer->m_heroIds[heroIndex2] !=
            static_cast<u8>(gpCurAIHero->m_id)) {
            distance15 = abs(gpGame->m_heroRecs[
                           gpCurPlayer->m_heroIds[heroIndex2]].m_x -
                           targetX) +
                       abs(gpGame->m_heroRecs[
                           gpCurPlayer->m_heroIds[heroIndex2]].m_y -
                           targetY);
            if (distance15 < 9) {
                heroTerrain7 = giGroundToTerrain[
                    gpAdvManager->GetCell(
                        gpGame->m_heroRecs[
                            gpCurPlayer->m_heroIds[heroIndex2]].m_x,
                        gpGame->m_heroRecs[
                            gpCurPlayer->m_heroIds[heroIndex2]].m_y)->m_terrainImageIndex];
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
    score4 = static_cast<i32>(score4 * AI_STRATEGIC_POSITION_SCORE_FACTOR);
    if (score4 > 32000)
        score4 = 32000;
    if (!immediate && !extraDistance) {
        *reinterpret_cast<i16 *>(reinterpret_cast<char *>(gaiHeroStrategicRVOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2) = static_cast<i16>(score4);
        *reinterpret_cast<i16 *>(reinterpret_cast<char *>(gaiLiveChanceOfPos) +
            targetX * 2 + targetY * MAP_WIDTH * 2) = static_cast<i16>(*liveChance);
    }
    return score4;
}

// @semantic: Complete 0x14e-byte CFG, frame/slots, and all 11 external
// relocations align. The first code residual is the first castle-coordinate
// equality: ours loads the town byte before the game-header word, while retail
// loads the word first; the second comparison repeats it. Scratch equality
// spellings retained the byte-first choice. Revisit in the byte-last-mile phase.
VA(0x0043fa3e, 0x14e)
i32 philAI::ValueOfTown(town *t) {
    i32 sum = 0;
    i32 idx;            // i
    for (idx = 0; idx <= 0x18; idx++) {
        if (t->m_buildings & (1 << idx))
            sum += GetBuildingBaseResourceValue(t->m_type, idx, t->m_buildState);
    }
    sum = (i32)(gafAITurnCostResource[RES_GOLD] * 1250.0f * 1.5 + sum);
    sum += 0x2ee;
    if (gpGame->m_mapHeader.lossCondition == MAP_LOSS_TOWN &&
        t->m_x == gpGame->m_mapHeader.lossConditionValue &&
        gpGame->m_mapHeader.lossTownY == t->m_y)
        sum += 0xc350;
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_CAPTURE_TOWN &&
        t->m_x == gpGame->m_mapHeader.victoryConditionValue &&
        gpGame->m_mapHeader.victoryTownY == t->m_y)
        sum += 0xc350;
    return sum;
}

// @semantic
// Complete & correct except the final `/ gResourceBaseValue[i]`: cl evaluates the simple
// int divisor before the float numerator -> fdivrp (guard al=0xd); retail evaluates the
// numerator first -> fdivp (al=0xf). Identical quotient; an /Od operand-eval-order pick.
VA(0x0043fb8c, 0x180)
void philAI::TurnCostResource(i32 player) {
    i32 nb;             // i
    playerData *kn;         // ptr
    float jb[7];        // per-resource ratio
    float idx;          // average turn cost
    i32 total;
    i32 cost[7];
    kn = &gpGame->m_players[player];
    total = 0;
    for (nb = 0; nb < 7; nb++) {
        cost[nb] = (i32)(((double)(kn->m_income[nb] * 5) * 0.7 +
                          (double)kn->m_resources[nb]) *
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

// @semantic: Complete 0x175-byte CFG, frame/slots, and all 22 external
// relocations align. Current differences are compiler float constants and
// division-guard identities, with shifted relocation sites in that guarded
// sequence. Revisit in the byte-last-mile phase.
VA(0x0043fd0c, 0x175)
float philAI::TurnValueOfObelisk(i32 player) {
    i32 jb;             // artifact RV
    i32 idx;            // turns
    playerData *ta;         // ptr
    ta = &gpGame->m_players[player];
    jb = gArtifactBaseRV[gpGame->m_ultimateArtifactId];
    if (gpGame->m_mapHeader.victoryCondition == 3)
        jb <<= 1;
    idx = jb / 0x6e;
    if (gpGame->m_ultimateArtifactId == ARTIFACT_NONE)
        return 0.0f;
    ta->m_obeliskValue = idx * 48 / gpGame->m_obeliskCount;
    if (gpCurPlayer->m_aiDifficulty == 2)
        ta->m_obeliskValue = (i32)(ta->m_obeliskValue * 1.4);
    ta->m_obeliskValue = (i32)((1.5 -
        abs(0x30 - gpGame->SetupPuzzlePieces(giCurPlayer, 1)) / 48.0f) * ta->m_obeliskValue);
    ta->m_obeliskValue = static_cast<i32>(
        (ta->m_attentionWeights.heroValue + 0.66) * ta->m_obeliskValue);
    return (float)ta->m_obeliskValue;
}

VA(0x0043fe81, 0x51)
float philAI::FutureDeflator(i32 *const p) {
    float t = TurnsToBuy(p);
    float v = 1.0f - t * AI_FUTURE_DEFLATION_RATE;
    if (v < 0.0)
        v = 0;
    return v;
}

// @early-stop
// Exact 0xbf8 code span with zero relocation-masked byte differences. All 72
// relocation sites align; retail delinks canonical monster-record field addends as local symbols.
VA(0x0043fed2, 0xbf8)
i32 philAI::FightValueOfStack(armyGroup *group, hero *heroPtr, i32 useHero,
                              i32 useTown, i32 townId, i32 useEnemyMods) {
    i32 armyValue;
    i32 spellValueMap;
    float unusedModifierRange;
    float enemyRangedModifier27;
    i32 scoreIndex15;
    i32 armySlotRecord;
    float enemyFlyingModifier36;
    i32 stackValueMap;
    float quantityModifierTarget;
    float enemyMeleeModifierIndex;
    i32 townArcherValueValue;
    town *townRecord19;
    i32 numArchers16;
    i32 archerLevel4;
    i32 statPowerTarget;
    i32 extraArchersTarget;
    i32 morale;
    i32 heroLuckIndex;
    float durationModifier7;
    i32 spellScoreTotal;
    i32 spellCount;
    i32 spellScores[46];
    float spellPowerModifier;
    i32 numSpellCastsValue;
    i32 bestSpellScore8;
    i32 bestSpellIndexTotal;

    armyValue = 0;
    spellValueMap = 0;
    townArcherValueValue = 0;
    enemyRangedModifier27 = 0.9f;
    enemyMeleeModifierIndex = 1.1f;
    enemyFlyingModifier36 = 0.75f;
    unusedModifierRange = 0.1f;
    townRecord19 = 0;

    if (useEnemyMods) {
        if (heroPtr->HasArtifact(ARTIFACT_BALLISTA) || heroPtr->HasSpell(0x2f) ||
            heroPtr->m_secondarySkills[10]) {
            enemyMeleeModifierIndex = 1.05f;
            enemyFlyingModifier36 = 0.95f;
        }
        if (heroPtr->m_secondarySkills[1] || heroPtr->HasArtifact(ARTIFACT_GOLDEN_BOW))
            enemyRangedModifier27 = 1.05f;
    }
    if (useTown) {
        townRecord19 = GetCastleSlot(townId);
        if (!(townRecord19->m_buildings &
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

                if ((gMonsterDatabase[group->m_creatureTypes[armySlotRecord]].attributes &
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
                stackValueMap = static_cast<i32>((quantityModifierTarget + 1.0f) * stackValueMap);

                if (useTown) {
                    if (gMonsterDatabase[group->m_creatureTypes[armySlotRecord]].attributes &
                        MONSTER_ATTRIBUTE_RANGED)
                        stackValueMap = static_cast<i32>(stackValueMap * 1.18);
                    if (gMonsterDatabase[group->m_creatureTypes[armySlotRecord]].attributes &
                        MONSTER_ATTRIBUTE_FLYING)
                        stackValueMap = static_cast<i32>(stackValueMap * 0.95);
                }
                if ((gMonsterDatabase[group->m_creatureTypes[armySlotRecord]].attributes &
                     MONSTER_ATTRIBUTE_RANGED) && heroPtr &&
                    heroPtr->m_secondarySkills[1]) {
                    stackValueMap = static_cast<i32>(stackValueMap *
                        gfSSAIArcheryMod[heroPtr->m_secondarySkills[1]]);
                }
                if (useEnemyMods) {
                    if (gMonsterDatabase[group->m_creatureTypes[armySlotRecord]].attributes &
                        MONSTER_ATTRIBUTE_RANGED)
                        stackValueMap = static_cast<i32>(stackValueMap * enemyRangedModifier27);
                    if (gMonsterDatabase[group->m_creatureTypes[armySlotRecord]].attributes &
                        MONSTER_ATTRIBUTE_FLYING)
                        stackValueMap = static_cast<i32>(stackValueMap * enemyMeleeModifierIndex);
                    else
                        stackValueMap = static_cast<i32>(stackValueMap * enemyFlyingModifier36);
                }
            }
            armyValue += stackValueMap;
        }
    }

    if (useTown) {
        extraArchersTarget = 0;
        townRecord19->CalcNumLevelArchers(&numArchers16, &archerLevel4);
        if (townRecord19->m_buildings &
            AI_BUILDING_LEFT_TURRET_MASK)
            extraArchersTarget += numArchers16 >> 1;
        if (townRecord19->m_buildings &
            AI_BUILDING_RIGHT_TURRET_MASK)
            extraArchersTarget += numArchers16 >> 1;
        numArchers16 += extraArchersTarget;
        townArcherValueValue = static_cast<i32>((archerLevel4 * 0.1 + 1.0) *
                                           (numArchers16 * 400));
    }

    if (useHero && heroPtr) {
        statPowerTarget = heroPtr->Stats(0) + heroPtr->Stats(1) + 20;
        if (statPowerTarget < 0)
            statPowerTarget = 0;
        if (statPowerTarget > 40)
            statPowerTarget = 40;
        armyValue = static_cast<i32>(armyValue * gfStatPower[statPowerTarget]);
        townArcherValueValue = static_cast<i32>(townArcherValueValue * gfStatPower[statPowerTarget]);

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
                    if (gsSpellInfo[armySlotRecord].attributes & 8)
                        spellScoreTotal = static_cast<i32>(spellScoreTotal * durationModifier7);
                    else if (gsSpellInfo[armySlotRecord].attributes & 1)
                        spellScoreTotal = static_cast<i32>(spellScoreTotal * spellPowerModifier);
                    numSpellCastsValue = heroPtr->m_spellPoints / GetManaCost(armySlotRecord, heroPtr);
                    if (numSpellCastsValue > 10)
                        numSpellCastsValue = 10;
                    spellScoreTotal = static_cast<i32>(spellScoreTotal * gfSpellCastNumMod[numSpellCastsValue]);
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
                spellValueMap = static_cast<i32>(spellValueMap +
                    bestSpellScore8 * gfSpellTypeNumMod[armySlotRecord]);
                spellScores[bestSpellIndexTotal] = 0;
            }
        }
    }

    if (armyValue * 2 < spellValueMap)
        spellValueMap = static_cast<i32>(armyValue * 1.25);
    else if (spellValueMap > armyValue * 1.5) {
        spellValueMap = armyValue;
    } else if ((armyValue | 0) < spellValueMap)
        spellValueMap = static_cast<i32>(armyValue * 0.75);
    if (armyValue * 2 < townArcherValueValue)
        townArcherValueValue = static_cast<i32>(armyValue * 1.5);
    else if (townArcherValueValue > armyValue * 1.5) {
        townArcherValueValue = static_cast<i32>(armyValue * 0.9);
    } else if ((armyValue | 0) < townArcherValueValue)
        townArcherValueValue = static_cast<i32>(armyValue * 1.25);
    if (giDebugLevel == 9)
        LogInt("FV3", armyValue, spellValueMap, townArcherValueValue,
               0, 0, -999, -999);
    armyValue += spellValueMap;
    armyValue += townArcherValueValue;
    return armyValue;
}

// @semantic: The complete 0x1e7-byte CFG, 0x18 frame, six stack slots, and all
// 14/14 ordered relocations align. At normalized instruction 82 retail loads
// leastStackValue3 into eax, compares replacementStackValue7, and skips on jge;
// ours loads replacementStackValue7, compares leastStackValue3, and skips on
// jle. Reversed operands, a value-preserving | 0 pin, and an empty positive arm
// with the update in else were neutral or added a trampoline. Revisit in the
// byte-last-mile phase or after PHILAI TU/header state changes.
VA(0x00440aca, 0x1e7)
void philAI::EvaluateOneTimeCreaturePurchase(i32 creature, i32 availableCount,
                                             i32 useAvailableCount,
                                             i32 &purchaseCount, i32 &purchaseValue,
                                             i32 &replacementSlot)
{
    i32 replacementStackValue7;
    i32 leastStackValue3;
    i32 purchaseFightValue15;
    i32 armyIndex3;

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

        purchaseValue = static_cast<i32>(
            purchaseFightValue15 *
            gpGame->m_players[gpCurAIHero->m_owner].m_upgradeValueWeight);
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
// @early-stop-reloc-only: Current PHILAI.cpp/header epoch: all 0x768 code bytes
// match after masking 85 ordered relocation sites. The fuzzy residual consists
// only of compiler float-constant and division-guard symbol identities; frame,
// slots, opcodes, operands, and CFG are exact. Revisit only after the PHILAI
// source/TU/header or comparison epoch changes.
VA(0x00440cb1, 0x768)
i32 philAI::QuickCombat(armyGroup *attacker, hero *attackerHero,
                        armyGroup *defender, hero *defenderHero,
                        i32 townBattle, i32 townId,
                        float &attackerDamage, float &defenderDamage)
{
    i32 attackerTroopCount5;
    hero *victoriousHero4;
    armyGroup *selectedGroup36;
    i32 unusedOutcomeA6;
    i32 attackerLoss5;
    i32 attackerRemaining7;
    i32 defenderLoss8;
    i32 attackerWon2;
    i32 defenderRemaining19;
    i32 armyIndex0;
    i32 unusedOutcomeB5;
    i32 attackerExperience37;
    i32 defenderExperience4;
    float adjustedDifference1;
    i32 defenderTroopCount1;
    i32 outcomeValue29;
    hero *defeatedHero5;
    float winChance37;
    i32 necromancyCount6;
    float casualtyFraction17;
    i32 defenderOwner1;
    float winnerChance0;
    float rollDifference8;

    attackerExperience37 =
        gpGame->ExperienceValueOfStack(attacker, attackerHero);
    if (townBattle != 0)
        attackerExperience37 += AI_QUICK_COMBAT_TOWN_EXPERIENCE;
    defenderExperience4 =
        gpGame->ExperienceValueOfStack(defender, defenderHero);
    attackerExperience37 = static_cast<i32>(
        (gpGame->m_difficulty * AI_QUICK_COMBAT_EXPERIENCE_DIFFICULTY_STEP +
         1.0) * attackerExperience37);
    defenderExperience4 = static_cast<i32>(
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
        necromancyCount6 = static_cast<i32>(
            static_cast<float>(defenderTroopCount1) *
            static_cast<float>(
                attackerHero->GetSSLevel(HERO_SKILL_NECROMANCY)) *
            AI_QUICK_COMBAT_NECROMANCY_FACTOR);
        if (necromancyCount6 <= 0)
            necromancyCount6 = 1;
        attackerHero->m_army.Add(CREATURE_SKELETON,
                                 necromancyCount6,
                                 AI_CREATURE_PURCHASE_NO_SLOT);
    } else if (defenderHero != 0 &&
               defenderDamage <= AI_QUICK_COMBAT_NECROMANCY_THRESHOLD &&
               defenderHero->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
        necromancyCount6 = static_cast<i32>(
            static_cast<float>(attackerTroopCount5) *
            static_cast<float>(
                defenderHero->GetSSLevel(HERO_SKILL_NECROMANCY)) *
            AI_QUICK_COMBAT_NECROMANCY_FACTOR);
        if (necromancyCount6 <= 0)
            necromancyCount6 = 1;
        defenderHero->m_army.Add(CREATURE_SKELETON,
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
                (gsSpellInfo[armyIndex0].attributes &
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

// @semantic: Complete 0x422-byte CFG, frame/slots, and all 29 external
// relocations align. Current TU state differs in floating symbol identities and
// four commutative address-load byte pairs at +0x276/+0x279, +0x2df/+0x2e2,
// +0x384/+0x387, and +0x3c0/+0x3c3. Ten guarded perturbations did not yield an
// eligible closure. Revisit in the byte-last-mile phase.
VA(0x00441419, 0x422)
void philAI::HeroInteractionAtHero(hero *firstHero, hero *secondHero,
                                   i32 evaluateOnly, i32 *value) {
    hero *dominantHero19;
    float armyShare0, transferFraction35, dominantShare19, desiredShare6,
        shareDelta1;
    i32 interactionValue8;
    i32 artifactType15;
    i32 dominantFightValue4;
    i32 heroIndex9;
    i32 heroValues27[AI_HERO_INTERACTION_HERO_COUNT];
    hero *recipientHero36;
    i32 statIndex8;
    i32 recipientFightValue10;
    hero *currentHero9;
    hero *savedHero9;

    *value = 0;
    if (evaluateOnly != 0 &&
        static_cast<u8>(firstHero->m_id) != iAlphaMale &&
        static_cast<u8>(secondHero->m_id) != iAlphaMale)
        return;
    {
        if (evaluateOnly == 0) {
            firstHero->m_lastHeroInteractionTurn =
                static_cast<i16>(giCurTurn);
            firstHero->m_lastInteractionHeroId = secondHero->m_id;
            secondHero->m_lastHeroInteractionTurn =
                static_cast<i16>(giCurTurn);
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
                    currentHero9->HasArtifact(ARTIFACT_MAGIC_BOOK))
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
                        gSSValues[statIndex8]
                            [currentHero9->m_secondarySkills[statIndex8] -
                             AI_SECONDARY_SKILL_LEVEL_OFFSET];
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
            static_cast<u8>(recipientHero36->m_id) == iAlphaMale &&
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
            interactionValue8 = static_cast<i32>(
                gpCurPlayer->m_upgradeValueWeight *
                (static_cast<float>(dominantFightValue4 +
                                    recipientFightValue10) *
                 transferFraction35));
            if (evaluateOnly != 0) {
                for (statIndex8 = 0;
                     statIndex8 < AI_BATTLE_ARTIFACT_SLOT_COUNT;
                     statIndex8++) {
                    artifactType15 = recipientHero36->m_artifacts[statIndex8];
                    if (artifactType15 != -1 &&
                        artifactType15 != ARTIFACT_MAGIC_BOOK)
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
                        static_cast<i32>(
                            static_cast<float>(dominantFightValue4 +
                                               recipientFightValue10) *
                            transferFraction35));
                } else {
                    RedistributeTroops(
                        &recipientHero36->m_army,
                        &dominantHero19->m_army, 1, 1,
                        recipientFightValue10, dominantFightValue4,
                        static_cast<i32>(
                            static_cast<float>(dominantFightValue4 +
                                               recipientFightValue10) *
                            transferFraction35));
                }
                SetupRelativeHeroStrengths();
            }
            *value = static_cast<i32>(interactionValue8 *
                                      AI_HERO_INTERACTION_VALUE_FACTOR);
        }
    }
}

// @semantic
// Exact 0x882 span and 0x78 frame; all 71 relocation sites and targets agree. The
// 25 relocation-masked residual bytes are only commutative load order at
// +0x45..+0x64, +0x139..+0x158, and +0x37d..+0x380; equivalent x87 comparison
// polarity at +0x554..+0x576 and +0x5b9..+0x5c3; and local continuation routing
// at +0x593..+0x594, +0x5ac, and +0x780.
VA(0x0044183b, 0x882)
void philAI::HeroInteractionAtTown(hero *heroPtr, town *townPtr, i32 doInteraction,
                                  i32 *value) {
    i32 heroStrength;
    i32 transferCount6;
    i32 estimatedTransfer16;
    i32 spellLevel14;
    i32 townWins2;
    i32 secondStrength0;
    i32 primarySkills3;
    i32 spellIndex;
    float transferShare9;
    armyGroup *firstArmy0;
    armyGroup *secondArmy8;
    float townShare5;
    i32 firstStrength3;
    i32 townStrength6;
    i32 otherIndex9;
    i32 index7;
    i32 canMerge2;
    i32 transferValue;
    float desiredShare0;
    float transferCurve;
    float shareDifference7;
    i32 spellMultiplier1;
    float transferFactor;

    *value = 0;
    if (doInteraction != 0) {
        if ((townPtr->m_buildings & AI_BUILDING_SHIPYARD_MASK) &&
            townPtr->m_id != giBestShipyardId) {
            index7 = abs(townPtr->m_y - heroPtr->m_y) +
                     abs(townPtr->m_x - heroPtr->m_x);
            if (gbActualShipyardFound) {
                if (giBestShipyardDist > index7) {
                    giBestShipyardDist = index7;
                    giBestShipyardId = townPtr->m_id;
                }
            } else {
                giBestShipyardDist = index7;
                giBestShipyardId = townPtr->m_id;
            }
            gbPossibleShipyardFound = 1;
            gbActualShipyardFound = 1;
        } else if ((townPtr->m_buildings & AI_BUILDING_CASTLE_MASK) &&
                   giGroundToTerrain[gpAdvManager->GetCell(
                       townPtr->m_x - 1,
                       townPtr->m_y + 1)->m_terrainImageIndex] == 0 &&
                   !gbActualShipyardFound &&
                   townPtr->m_id != giBestShipyardId) {
            index7 = abs(townPtr->m_y - heroPtr->m_y) +
                     abs(townPtr->m_x - heroPtr->m_x);
            if (gbPossibleShipyardFound) {
                if (giBestShipyardDist > index7) {
                    giBestShipyardDist = index7;
                    giBestShipyardId = townPtr->m_id;
                }
            } else {
                giBestShipyardDist = index7;
                giBestShipyardId = townPtr->m_id;
            }
            gbPossibleShipyardFound = 1;
        }
    } else {
        heroPtr->m_lastTownInteractionTurn = static_cast<i16>(giCurTurn);
        heroPtr->m_visitedTownId = static_cast<u8>(
            townPtr->m_id);
        if (!heroPtr->HasArtifact(ARTIFACT_MAGIC_BOOK) &&
            (townPtr->m_buildings & AI_BUILDING_MAGE_GUILD_MASK)) {
            if (gpCurPlayer->m_resources[RES_GOLD] >=
                AI_MAGIC_BOOK_COST) {
                GiveArtifact(heroPtr, ARTIFACT_MAGIC_BOOK, 1, -1);
                gpCurPlayer->m_resources[RES_GOLD] -=
                    AI_MAGIC_BOOK_COST;
            } else {
                heroPtr->m_remainingMobility = 0;
            }
        }
        if ((townPtr->m_buildings & AI_BUILDING_MAGE_GUILD_MASK) &&
            heroPtr->HasArtifact(ARTIFACT_MAGIC_BOOK) &&
            heroPtr->m_spellPoints < heroPtr->Stats(3) * AI_MANA_PER_KNOWLEDGE) {
            heroPtr->m_remainingMobility = 0;
        }
    }

    if ((townPtr->m_buildings & AI_BUILDING_MAGE_GUILD_MASK) &&
        (doInteraction != 0 || heroPtr->HasArtifact(ARTIFACT_MAGIC_BOOK))) {
        *value += ManaRefreshValue(heroPtr, 1);
        for (spellLevel14 = 1;
             spellLevel14 <= heroPtr->m_secondarySkills[HERO_SKILL_WISDOM] + 2;
             spellLevel14++) {
            for (spellIndex = 0;
                spellIndex < townPtr
                                  ->m_spellCounts[spellLevel14 - 1];
                 spellIndex++) {
                if (!heroPtr->HasSpell(
                        townPtr->m_spells[spellLevel14 - 1][spellIndex])) {
                    if (gsSpellInfo[
                            townPtr->m_spells[spellLevel14 - 1][spellIndex]].attributes & 1)
                        spellMultiplier1 = heroPtr->Stats(3);
                    else
                        spellMultiplier1 = 1;
                    *value += spellMultiplier1 *
                              gsSpellInfo[townPtr->m_spells[spellLevel14 - 1]
                                                           [spellIndex]].aiValue;
                }
            }
        }
    }

    heroStrength = FightValueOfStack(&heroPtr->m_army, 0, 0, 0, 0, 0);
    townStrength6 = FightValueOfStack(&townPtr->m_army, 0, 0, 0, 0, 0);
    townShare5 = static_cast<float>(townStrength6) /
                 (townStrength6 + (heroStrength | 0));
    primarySkills3 = 0;
    primarySkills3 = heroPtr->Stats(0) + heroPtr->Stats(1);
    if (primarySkills3 > 10)
        primarySkills3 = 10;
    if (townPtr->m_buildings & AI_BUILDING_CASTLE_MASK)
        desiredShare0 = static_cast<float>(0.55 - primarySkills3 * 0.02);
    else
        desiredShare0 = static_cast<float>(AI_TOWN_PRIMARY_SKILL_SHARE_BASE -
                                           primarySkills3 * 0.01);
    if (giCurTurn <= AI_EARLY_TOWN_SHARE_TURN)
        desiredShare0 = fFirstWeekTownFV;
    else if (giCurTurn <= AI_SECOND_WEEK_END_TURN)
        desiredShare0 = static_cast<float>(desiredShare0 * 0.5);
    else if (giCurTurn <= AI_THIRD_WEEK_END_TURN)
        desiredShare0 = static_cast<float>(desiredShare0 * 0.75);
    if (static_cast<u8>(heroPtr->m_id) == iAlphaMale)
        desiredShare0 = static_cast<float>(desiredShare0 * 0.5);
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_CAPTURE_TOWN &&
        townPtr->m_x ==
            gpGame->m_mapHeader.victoryConditionValue &&
        (gpGame->m_mapHeader.victoryTownY | 0) ==
            townPtr->m_y) {
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
            estimatedTransfer16 = static_cast<i32>(
                (townStrength6 + (heroStrength | 0)) *
                ((transferCurve * transferCurve - AI_TOWN_TRANSFER_CURVE_OFFSET) *
                 gpCurPlayer->m_upgradeValueWeight) *
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
                    if (townPtr->m_army.m_quantities[index7] <= 0)
                        canMerge2 = 1;
                }
            }
            if (!canMerge2) {
                for (index7 = 0; index7 < AI_TOWN_ARMY_SLOTS; index7++) {
                    for (otherIndex9 = 0; otherIndex9 < AI_TOWN_ARMY_SLOTS; otherIndex9++) {
                        if ((heroPtr->m_army.m_creatureTypes[otherIndex9] | 0) ==
                            townPtr->m_army.m_creatureTypes[index7]) {
                            canMerge2 = 1;
                            break;
                        }
                    }
                }
            }
            if (!canMerge2)
                estimatedTransfer16 = 0;
            *value += estimatedTransfer16;
            if (townPtr->m_threat != 0 &&
                townPtr->m_occupyingHeroId == -1)
                *value += AI_UNGUARDED_TOWN_VALUE;
        } else {
            townPtr->GiveSpells(0);
            if (townWins2)
                transferShare9 = static_cast<float>(transferShare9 + AI_TOWN_TRANSFER_BONUS);
            transferCount6 = static_cast<i32>(
                (townStrength6 + (heroStrength | 0)) * transferShare9);
            if (townWins2)
                firstArmy0 = &townPtr->m_army;
            else
                firstArmy0 = &heroPtr->m_army;
            if (townWins2)
                secondArmy8 = &heroPtr->m_army;
            else
                secondArmy8 = &townPtr->m_army;
            if (townWins2) {
                firstStrength3 = townStrength6;
                secondStrength0 = heroStrength;
            } else {
                firstStrength3 = heroStrength;
                secondStrength0 = townStrength6;
            }
            RedistributeTroops(firstArmy0, secondArmy8, !townWins2, townWins2,
                               firstStrength3, secondStrength0, transferCount6);
            if (townPtr->m_id == giHumanTownConquered &&
                heroPtr->m_remainingMobility <= AI_CONQUERED_HERO_MOBILITY_LIMIT)
                heroPtr->m_remainingMobility = 0;
        }
      }
    }
}

// @semantic
// Exact 0x4ba span and 0x48 frame; all 19 relocation sites and targets agree.
// The only masked residual is TU-cumulative comparison load order at +0x3b9,
// +0x3bc, and +0x3be (transfer count vs total creature count).
VA(0x004420bd, 0x4ba)
void philAI::RedistributeTroops(armyGroup *sourceArmy, armyGroup *destinationArmy,
                                i32 preserveOne, i32 preferFast, i32 sourceStrength,
                                i32 destinationStrength, i32 transferBudget) {
    i32 totalCreatures0;
    i32 keepGoing5;
    i32 sourceIndex15;
    i32 destinationIndex6;
    i32 selectedIndex5;
    i32 bestValue37;
    i32 stackValue7;
    i32 selectedSpeed19;
    i32 transferCount16;

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
            transferCount16 = static_cast<i32>(
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
i32 philAI::ChooseGoldOrExperience(i32, i32) {
    return gpCurPlayer->m_resources[6] > 4000 ? 1 : 0;
}

VA(0x004425b0, 0xc7)
void philAI::ChooseEvaluateBattle(armyGroup *ag1, hero *h1, armyGroup *ag2, hero *h2,
                                  i32 a, i32 b, i32 c, i32 &outFlag, i32 &outValue) {
    i32 val;            // score
    i32 p;              // unused
    i32 node, nb, kn, jb; // ProbableOutcomeOfBattle int& outputs
    float idx;          // fv
    i32 race;
    if (h2 != 0)
        race = h2->m_owner;
    else
        race = -1;
    ProbableOutcomeOfBattle(ag1, h1, ag2, h2, 0, a, b, race, idx, jb, kn, nb, node, val);
    val = static_cast<i32>(c * idx + val);
    if (val <= 0) {
        outValue = 0;
        outFlag = 0;
    } else {
        outValue = val;
        outFlag = 1;
    }
}

VA(0x00442677, 0xfa)
i32 philAI::ChooseToFightForArtifact(i32 a, i32 b, i32 c) {
    i32 ra;             // i (-0x20)
    i32 py;             // ProbableOutcomeOfBattle out o4 (-0x1c)
    i32 p;              // out o3 (-0x18)
    i32 node;           // out o2 (-0x14)
    i32 nb;             // out o1 (-0x10)
    float kn;           // fv (-0xc)
    i32 jb;             // artifact RV (-0x8)
    i32 idx;            // result (-0x4)
    i32 o5;             // out (-0x24)
    jb = gArtifactBaseRV[a];
    for (ra = 0; ra < AI_TOWN_ARMY_SLOTS; ra++) {
        gpMonGroup->m_creatureTypes[ra] = static_cast<i8>(b);
        if (ra == 0)
            gpMonGroup->m_quantities[ra] = 1;
        else
            gpMonGroup->m_quantities[ra] = 0;
    }
    ProbableOutcomeOfBattle(&gpCurAIHero->m_army, gpCurAIHero, gpMonGroup,
                            0, 0, 0, 0, -1, kn, nb, node, p, py, o5);
    idx = static_cast<i32>(jb * kn + o5);
    if (idx > 0)
        return 1;
    else
        return 0;
}

VA(0x00442771, 0x53)
i32 philAI::NetValueOfArtifact(i32 a1, i32 a2, i32 a3, i32 a4) {
    return static_cast<i32>(static_cast<float>(gArtifactBaseRV[a1])
               - (static_cast<float>(a4) * gafAITurnCostResource[a3] +
                  static_cast<float>(a2) * gafAITurnCostResource[RES_GOLD]));
}

VA(0x004427c4, 0x1d)
i32 philAI::ChooseToPayRansomOnHero(i32) { return 1; }

VA(0x004427e1, 0xd7)
void philAI::BuildBuilding(town *t, i32 building) {
    i32 cost[7];
    i32 i;
    sprintf(gText, "Player %d built %s in town %d.\n", giCurPlayer,
            GetBuildingName(t->m_type, building), t->m_id);
    LogStr(gText);
    if (giDebugLevel >= 3) {
        AiPrint(gText);
        DelayMilli(0x5dc);
    }
    GetBuildingCost(t->m_type, building, cost, t->m_buildState);
    for (i = 0; i < 7; i++)
        gpCurPlayer->m_resources[i] -= cost[i];
    t->BuildBuilding(building);
    ShowStatus();
}

VA(0x004428b8, 0x328)
void philAI::BuildHero(town *townPtr, i32 availableHeroIndex) {
    i32 townX37;
    i32 townY9;
    hero *newHero6;

    sprintf(gText, "Player %d built hero in town %d.\n", giCurPlayer,
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
    gpGame->SetRandomHeroArmies(static_cast<u8>(newHero6->m_id), 1);
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
        gpGame->m_worldMap.GetCell(townX37, townY9)->m_triggerType;
    newHero6->m_occupiedTown =
        gpGame->m_worldMap.GetCell(townX37, townY9)->m_objectMetadata;
    gpGame->m_worldMap.GetCell(townX37, townY9)->m_triggerType = AI_OBJECT_HERO;
    gpGame->m_worldMap.GetCell(townX37, townY9)->m_objectMetadata =
        gpCurPlayer->m_availableHeroIds[availableHeroIndex];
    gpGame->m_availableHeroes[static_cast<u8>(newHero6->m_id)] =
        townPtr->m_owner;

    CheckValidAvailableHeroes();
    SendMapChange(AI_HERO_BUILD_MAP_CHANGE, newHero6->m_id,
                  static_cast<u8>(newHero6->m_x),
                  static_cast<u8>(newHero6->m_y),
                  AI_HERO_BUILD_MAP_CHANGE_VALUE, 0, 0);
    townPtr->m_occupyingHeroId = newHero6->m_id;
    townPtr->GiveSpells(0);

    gpCurPlayer->m_availableHeroIds[availableHeroIndex] = static_cast<i8>(
        gpGame->GetNewHeroId(giCurPlayer, AI_TROOP_EMPTY_SLOT, 1));
    gpGame->m_availableHeroes[
        gpCurPlayer->m_availableHeroIds[availableHeroIndex]] = AI_HERO_AVAILABLE_FLAG;
    bHeroBuiltThisTurn = 1;
    HeroInteractionAtTown(newHero6, townPtr, 0, &iDummy);
    ShowStatus();
}

// @semantic: The fixed-width typedef migration invalidated the former relocation-only
// proof. Semantics, frame, CFG, and all 29 relocation sites still agree; the first
// residual is an FPU operand-load swap at +0x1c4/+0x1c6 and +0x1c7/+0x1c9. Revisit
// after a PHILAI source/TU/header or comparison-epoch change.
VA(0x00442be0, 0x2cd)
void philAI::BuildCreature(town *townPtr, i32 dwelling, i32 purchaseCount) {
    i32 canJoin6;
    i32 weakestSlot8;
    i32 armyIndex4;
    i32 creatureType13;
    float stackValue9;
    i32 monsterCosts10[AI_PURCHASE_RESOURCE_COUNT];
    float weakestValue5;

    sprintf(gText, "Player %d built %d %s in town %d.\n", giCurPlayer,
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
i32 philAI::CanBuyBHC(BHC &bhc) {
    i32 jb;             // dwelling monster type
    i32 idx;            // i
    i32 cost[7];
    switch (bhc.type) {
    case 0:
        if (CanBuy(bhc.pTown, bhc.what))
            return 1;
        break;
    case 1:
        if (gpCurPlayer->m_resources[6] >= gHeroGoldCost &&
            bhc.pTown->m_occupyingHeroId == -1 &&
            bHeroBuiltThisTurn == 0)
            return 1;
        break;
    case 2:
        jb = gDwellingType[bhc.pTown->m_type][bhc.what];
        if (bhc.pTown->m_garrison[bhc.what] < bhc.num)
            return 0;
        GetMonsterCost(jb, cost);
        for (idx = 0; idx < 7; idx++)
            if (cost[idx] * bhc.num > gpCurPlayer->m_resources[idx])
                return 0;
        return 1;
    }
    return 0;
}

VA(0x00443007, 0x164)
i32 philAI::CombatMonsterEvent(hero *h, i32 monType, i32 *pCount, mapCell *cell) {
    i32 kn;             // i
    i32 jb;             // combat result
    float f2;
    float idx;          // f1
    i32 total;
    memset(gpMonGroup->m_creatureTypes, -1, sizeof(gpMonGroup->m_creatureTypes));
    memset(gpMonGroup->m_quantities, 0, sizeof(gpMonGroup->m_quantities));
    if (*pCount / AI_TOWN_ARMY_SLOTS > 0) {
        for (kn = 0; kn < AI_TOWN_ARMY_SLOTS; kn++) {
            gpMonGroup->m_creatureTypes[kn] = static_cast<i8>(monType);
            gpMonGroup->m_quantities[kn] = static_cast<i16>(*pCount / AI_TOWN_ARMY_SLOTS);
        }
    }
    for (kn = *pCount % AI_TOWN_ARMY_SLOTS - 1; kn >= 0; kn--) {
        gpMonGroup->m_creatureTypes[kn] = static_cast<i8>(monType);
        gpMonGroup->m_quantities[kn]++;
    }
    jb = gpPhilAI->QuickCombat(&h->m_army, h, gpMonGroup, 0, 0, 0, f2, idx);
    total = 0;
    for (kn = 0; kn < AI_TOWN_ARMY_SLOTS; kn++)
        total += gpMonGroup->m_quantities[kn];
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
i32 philAI::FightEvent(hero *h, mapCell *cell, i32 evaluateOnly) {
    i32 eventType16;
    i32 monsterType6;
    i32 monsterCount28;
    i32 stackIndex7;
    i32 rewardValue7;
    i32 battleWon29;
    i32 battleValue19;
    float attackerLoss5;
    float defenderLoss4;
    i32 combatResult18;
    i32 unusedValue8;

    eventType16 = cell->m_triggerType & 0x7f;
    if (cell->m_objectMetadata == AI_FIGHT_EVENT_EMPTY)
        return 0;

    i32 shipwreckCounts15[AI_FIGHT_EVENT_LEVEL_COUNT] = {
        AI_FIGHT_EVENT_SHIPWRECK_COUNT_1,
        AI_FIGHT_EVENT_SHIPWRECK_COUNT_2,
        AI_FIGHT_EVENT_SHIPWRECK_COUNT_3,
        AI_FIGHT_EVENT_SHIPWRECK_COUNT_4
    };
    i32 derelictCounts28[AI_FIGHT_EVENT_LEVEL_COUNT] = {
        AI_FIGHT_EVENT_DERELICT_COUNT_1,
        AI_FIGHT_EVENT_DERELICT_COUNT_2,
        AI_FIGHT_EVENT_DERELICT_COUNT_3,
        AI_FIGHT_EVENT_DERELICT_COUNT_4
    };
    i32 graveyardCounts37[AI_FIGHT_EVENT_LEVEL_COUNT] = {
        AI_FIGHT_EVENT_GRAVEYARD_COUNT_1,
        AI_FIGHT_EVENT_GRAVEYARD_COUNT_2,
        AI_FIGHT_EVENT_GRAVEYARD_COUNT_3,
        AI_FIGHT_EVENT_GRAVEYARD_COUNT_4
    };

    switch (eventType16) {
    case AI_OBJECT_SHIPWRECK:
        monsterType6 = CREATURE_GHOST;
        monsterCount28 = shipwreckCounts15[cell->m_objectMetadata - AI_FIGHT_EVENT_LEVEL_OFFSET];
        break;
    case AI_OBJECT_GRAVEYARD:
        monsterType6 = CREATURE_ZOMBIE;
        monsterCount28 = graveyardCounts37[cell->m_objectMetadata - AI_FIGHT_EVENT_LEVEL_OFFSET];
        break;
    default:
        monsterType6 = CREATURE_SKELETON;
        monsterCount28 = derelictCounts28[cell->m_objectMetadata - AI_FIGHT_EVENT_LEVEL_OFFSET];
        break;
    }

    for (stackIndex7 = 0; stackIndex7 < AI_FIGHT_EVENT_STACKS; stackIndex7++) {
        gpMonGroup->m_creatureTypes[stackIndex7] = static_cast<i8>(monsterType6);
        gpMonGroup->m_quantities[stackIndex7] = static_cast<i16>(monsterCount28);
    }

    rewardValue7 = 0;
    switch (eventType16) {
    case AI_OBJECT_GRAVEYARD:
        switch (cell->m_objectMetadata) {
        case AI_FIGHT_EVENT_LEVEL_1:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_500);
            break;
        case AI_FIGHT_EVENT_LEVEL_2:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_1000);
            break;
        case AI_FIGHT_EVENT_LEVEL_3:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_3000);
            break;
        case AI_FIGHT_EVENT_LEVEL_4:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_1000 +
                gpCurPlayer->m_artifactValue);
            break;
        }
        break;
    case AI_OBJECT_SHIPWRECK:
        switch (cell->m_objectMetadata) {
        case AI_FIGHT_EVENT_LEVEL_1:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_1000);
            break;
        case AI_FIGHT_EVENT_LEVEL_2:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_2000);
            break;
        case AI_FIGHT_EVENT_LEVEL_3:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_5000);
            break;
        case AI_FIGHT_EVENT_LEVEL_4:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_2000 +
                gpCurPlayer->m_artifactValue);
            break;
        }
        break;
    case AI_OBJECT_DERELICT_SHIP:
        switch (cell->m_objectMetadata) {
        case AI_FIGHT_EVENT_LEVEL_1:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_500);
            break;
        case AI_FIGHT_EVENT_LEVEL_2:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_1000);
            break;
        case AI_FIGHT_EVENT_LEVEL_3:
            rewardValue7 = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_FIGHT_EVENT_REWARD_2000);
            break;
        case AI_FIGHT_EVENT_LEVEL_4:
            rewardValue7 = static_cast<i32>(
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
            switch (cell->m_objectMetadata) {
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
            switch (cell->m_objectMetadata) {
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
            switch (cell->m_objectMetadata) {
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
        cell->m_objectMetadata = AI_FIGHT_EVENT_EMPTY;
    }
    return 0;
}

VA(0x00443842, 0x73)
i32 philAI::DamageGroup(armyGroup *ag, hero *loser, hero *, float dmg) {
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
    i32 nb = gpCurPlayer->m_heroCount;
    if (nb < 4 && gpCurPlayer->m_resources[6] >= 0x9c4 && bHeroBuiltThisTurn == 0)
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
void philAI::TownEvent(mapCell *cell, hero *h, i32 x, i32 y) {
    float attackerLoss6;
    float defenderLoss5;
    i32 currentPlayer26;
    i32 quickCombatResult9;
    town *townPtr2;
    i32 combatResult7;
    hero *defendingHero0;

    townPtr2 = GetCastleSlot(cell->m_objectMetadata);
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
i32 philAI::ComputeUpgradeValue(i32 a1, i32 a2) {
    i32 cnt = gpCurAIHero->CreatureTypeCount(a1);
    if (cnt == 0)
        return 0;
    i32 result = static_cast<i32>(
        static_cast<float>((gMonsterDatabase[a2].fightValue -
                            gMonsterDatabase[a1].fightValue) * cnt) *
        gpCurPlayer->m_upgradeValueWeight);
    if (gpCurAIHero->CreatureTypeCount(a2) != 0)
        result = static_cast<i32>(result * 0.6);
    return result;
}

// @early-stop
// @early-stop-reloc-only: All 0x271 bytes match after masking 34 aligned COFF relocations.
// Objdiff's residual is symbol identity: table/field addends and compiler constant
// pools resolve to the retail addresses, while jump-table labels delink as this function.
VA(0x00443c54, 0x271)
i32 philAI::ComputeValueOfSS(hero *h, i32 skill, i32 level) {
    i32 fightValue7;
    i32 value28;
    i32 armyIndex4;
    i32 totalArmyValue2;
    float rangedShare2;
    i32 rangedArmyValue28;
    i32 stackValue7;

    value28 = gSSValues[skill][level - AI_SECONDARY_SKILL_LEVEL_OFFSET];
    fightValue7 = FightValueOfStack(&h->m_army, h, 1, 0, 0, 0);
    if (skill != HERO_SKILL_ESTATES) {
        value28 = static_cast<i32>(
            (static_cast<float>(fightValue7) /
                 gpCurPlayer->m_upgradeValueWeight /
                 AI_SECONDARY_SKILL_FIGHT_SCALE +
             AI_SECONDARY_SKILL_BASE_FACTOR) * value28);
    }

    switch (skill) {
    case HERO_SKILL_NAVIGATION:
        if (h->m_eventFlags & HERO_EVENT_EMBARKED)
            value28 = static_cast<i32>(
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
                        .attributes & MONSTER_ATTRIBUTE_RANGED) {
                    rangedArmyValue28 += stackValue7;
                }
            }
        }
        rangedShare2 = static_cast<float>(rangedArmyValue28) /
                       static_cast<float>(totalArmyValue2);
        value28 = static_cast<i32>(
            (rangedShare2 / AI_SECONDARY_SKILL_ARCHERY_SHARE +
             AI_SECONDARY_SKILL_ARCHERY_BASE) * value28);
        break;
    case HERO_SKILL_WISDOM:
    case HERO_SKILL_MYSTICISM:
        if (!h->HasArtifact(ARTIFACT_MAGIC_BOOK) ||
            h->Stats(HERO_PRIMARY_KNOWLEDGE) <
                AI_SECONDARY_SKILL_MINIMUM_KNOWLEDGE) {
            value28 = static_cast<i32>(value28 * AI_SECONDARY_SKILL_BASE_FACTOR);
        }
        break;
    }
    return value28;
}

VA(0x00443ec5, 0x59)
i32 philAI::ComputeValueOfFreeSS(hero *h, i32 ss) {
    if (h->m_secondarySkills[ss] != 0 || h->m_secondarySkillCount >= 8)
        return 0;
    else
        return ComputeValueOfSS(h, ss, 1);
}

// @early-stop
// @early-stop-reloc-only: Current PHILAI.cpp/header epoch: all 0xa6 code bytes
// match after masking 4 ordered relocation sites. The fuzzy residual is only
// compiler float-constant/division-guard identity. Revisit only after the
// PHILAI source/TU/header or comparison epoch changes.
VA(0x00443f1e, 0xa6)
i32 philAI::ManaRefreshValue(hero *h, i32 level) {
    i32 v = 0;
    i32 sp = h->Stats(3) * level * 10;
    i32 deficit = sp - h->m_spellPoints;
    if (deficit <= 0)
        return 0;
    float fr = (float)deficit / sp;
    if (deficit > 0)
        v = (i32)((float)(deficit * 5) * fr);
    return v;
}

// @semantic: The complete event switch, frame/slots, case order, and 356 ordered
// relocation sites are reconstructed. The next-public retail span absorbs
// delinked private code, so symbol-only disassembly truncates our body after the
// first switch arm; explicit-range review is required. One canonical
// gafAITurnCostResource reference remains delinked under a different identity.
// Revisit when private-function boundaries are normalized.
VA(0x00443fc4, 0x1ac5)
i32 philAI::ValueOfEventAtPosition(i32 x, i32 y, i32 immediate, i32 *liveChance) {
    mapCell *cell_k;
    i32 cellState_m;
    i32 creatureFlag_k;
    i32 purchaseCost_j;
    i32 creaturePurchaseState[9];
    i32 resource;
    i32 resourceState_a[3];
    i32 purchaseValue_n;
    i32 index_k;
    i32 amount_j;
    i32 battleWon_p;
    i32 rewardState_a[2];
    i32 eventState_l;
    i32 value_h;
    i32 battleValue_b;
    i32 combatState_k[4];
    i32 creature;
    i32 exitY_p;
    i32 exitX_p;
    i32 routeLiveChance_e[4];
    i32 currentValue_h;
    mapCell *otherCell_j;
    i32 exitValue_i;
    i32 bestExitValue_l;
    i32 bestExitY_c;
    i32 bestExitX_c;
    float battleStatMod_n;
    float spellPowerMod_m;
    float shrinePowerMod_p;
    i32 resources_e[7];

    if (!immediate &&
        *reinterpret_cast<i16 *>(reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
                                   (MAP_WIDTH | 0) * y * sizeof(i16) + x * sizeof(i16)) !=
            AI_RV_UNSET)
        return *reinterpret_cast<i16 *>(
            reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
            (MAP_WIDTH | 0) * y * sizeof(i16) + x * sizeof(i16));

    gbReduceByReload = 1;
    gbReduceByBerserk = 1;
    *liveChance = 100;
    value_h = 0;
    cell_k = gpAdvManager->GetCell(x, y);

    if (gpCurPlayer->m_ultimateArtifactHintChance > 15 &&
        gpCurPlayer->m_ultimateArtifactHintX == x &&
        gpCurPlayer->m_ultimateArtifactHintY == y) {
        value_h = (gpCurPlayer->m_ultimateArtifactHintChance - 15) *
                gUltArtifactAvgValue / 100;
    } else if (cell_k->m_triggerType & 0x80) {
        switch (cell_k->m_triggerType & 0x7f) {
    case AI_OBJECT_MONSTER:
        value_h = EvaluateMonsterEvent(cell_k->m_objectIndex, cell_k->m_objectMetadata, liveChance);
        break;
    case AI_OBJECT_RESOURCE:
        resource = cell_k->m_objectIndex >> 1;
        switch (resource) {
        case RES_GOLD:
            value_h = static_cast<i32>(
                gafAITurnCostResource[resource] *
                (cell_k->m_objectMetadata * AI_GOLD_RESOURCE_MULTIPLIER));
            break;
        default:
            value_h = static_cast<i32>(gafAITurnCostResource[resource] * cell_k->m_objectMetadata);
            break;
        }
        break;
    case AI_OBJECT_TREASURE_CHEST:
        value_h = static_cast<i32>(gafAITurnCostResource[RES_GOLD] *
                                   AI_TREASURE_CHEST_GOLD_AMOUNT);
        break;
    case AI_OBJECT_HERO_EVENT:
        value_h = EvaluateHeroEvent(cell_k->m_objectMetadata, x, y, immediate, liveChance);
        break;
    case AI_OBJECT_TOWN_EVENT:
        value_h = EvaluateTownEvent(cell_k->m_objectMetadata, x, y, immediate, liveChance);
        break;
    case AI_OBJECT_CAMPFIRE:
        value_h = static_cast<i32>(
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
        value_h = EvaluateArtifactEvent(cell_k->m_objectIndex >> 1, cell_k->m_objectMetadata);
        break;
    case AI_OBJECT_ALCHEMIST_LAB:
    case AI_OBJECT_MINE:
    case AI_OBJECT_SAWMILL:
        value_h = EvaluateMineEvent(cell_k->m_objectMetadata, x, y, liveChance);
        break;
    case AI_OBJECT_OBELISK:
        value_h = (giCurPlayerBit & gpGame->m_obeliskVisitors[cell_k->m_objectMetadata - 1]) ? 0 :
                gpCurPlayer->m_obeliskValue;
        break;
    case AI_OBJECT_OASIS:
        if (!(gpCurAIHero->m_eventFlags & 8))
            value_h = static_cast<i32>(gpCurAIHero->m_aiFightValue * AI_OASIS_VALUE_FACTOR);
        break;
    case AI_OBJECT_BUOY:
        if (!(gpCurAIHero->m_eventFlags & 2) && giCurAIHeroMorale < 3)
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_BUOY_VALUE_FACTOR);
        break;
    case AI_OBJECT_TEMPLE:
        if (!(gpCurAIHero->m_eventFlags & 0x100) && giCurAIHeroMorale < 3)
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_MORALE_LUCK_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_FAERIE_RING:
        if (!(gpCurAIHero->m_eventFlags & 0x10) && giCurAIHeroLuck < 3)
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_MORALE_LUCK_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_IDOL:
        if (!(gpCurAIHero->m_eventFlags & 0x2000) && giCurAIHeroLuck < 3)
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_MORALE_LUCK_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_FOUNTAIN:
        if (!(gpCurAIHero->m_eventFlags & 4) && giCurAIHeroLuck < 3)
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_MORALE_LUCK_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_WATERING_HOLE:
        if (!(gpCurAIHero->m_eventFlags & 0x200))
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_WATERING_HOLE_VALUE_FACTOR);
        break;
    case AI_OBJECT_SHRINE_FIRST:
    case AI_OBJECT_SHRINE_SECOND:
    case AI_OBJECT_SHRINE_THIRD:
        if (gpCurAIHero->Stats(3) > 0 &&
            gpCurAIHero->HasArtifact(ARTIFACT_MAGIC_BOOK) &&
            !gpCurAIHero->HasSpell(cell_k->m_objectMetadata - 1)) {
            if (gsSpellInfo[cell_k->m_objectMetadata - 1].level <=
                gpCurAIHero->m_secondarySkills[7] + 2) {
                value_h = gsSpellInfo[cell_k->m_objectMetadata - 1].aiValue;
                if (gsSpellInfo[cell_k->m_objectMetadata - 1].attributes & 1) {
                    if (gpCurAIHero->Stats(3) > 40)
                        shrinePowerMod_p = gfStatPower[40];
                    else
                        shrinePowerMod_p = gfStatPower[gpCurAIHero->Stats(3)];
                    value_h = static_cast<i32>(value_h * shrinePowerMod_p);
                }
            }
        } else
            value_h = 0;
        break;
    case AI_OBJECT_GAZEBO:
        if (gpCurAIHero->m_gazeboVisits & (1U << cell_k->m_objectMetadata))
            value_h = 0;
        else
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_GAZEBO_VALUE_FACTOR);
        break;
    case AI_OBJECT_TREE_OF_KNOWLEDGE:
        value_h = 0;
        if (!(gpCurAIHero->m_treeKnowledgeVisits & (1U << (cell_k->m_objectMetadata & 31)))) {
            switch (cell_k->m_objectMetadata >> 6) {
            case 1:
                value_h = static_cast<i32>(
                    gpCurAIHero->m_aiFightValue * AI_TREE_KNOWLEDGE_VALUE_FACTOR);
                break;
            case 2:
                if (gpCurPlayer->m_resources[RES_GOLD] >=
                    static_cast<i32>(AI_TREE_KNOWLEDGE_GOLD_COST)) {
                    value_h = static_cast<i32>(
                        gpCurAIHero->m_aiFightValue * AI_TREE_KNOWLEDGE_VALUE_FACTOR);
                    value_h = static_cast<i32>(
                        value_h - gafAITurnCostResource[RES_GOLD] *
                                    AI_TREE_KNOWLEDGE_GOLD_COST);
                }
                break;
            case 3:
                if (gpCurPlayer->m_resources[RES_GEMS] >=
                    static_cast<i32>(AI_TREE_KNOWLEDGE_GEM_COST)) {
                    value_h = static_cast<i32>(
                        gpCurAIHero->m_aiFightValue * AI_TREE_KNOWLEDGE_VALUE_FACTOR);
                    value_h = static_cast<i32>(
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
        if (cell_k->m_objectMetadata == 99) {
            value_h = 0;
        } else {
            memset(resources_e, 0, sizeof(resources_e));
            resources_e[cell_k->m_objectMetadata] = 2;
            value_h = RVConversion(resources_e);
        }
        break;
    case AI_OBJECT_MAGIC_GARDEN:
        if (!cell_k->m_objectMetadata)
            value_h = 0;
        else if (cell_k->m_objectMetadata - 1 == RES_GOLD)
            value_h = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_MAGIC_GARDEN_GOLD_AMOUNT);
        else
            value_h = static_cast<i32>(gafAITurnCostResource[cell_k->m_objectMetadata] * 5.0f);
        break;
    case AI_OBJECT_FLOTSAM:
        value_h = static_cast<i32>(
            gafAITurnCostResource[RES_GOLD] * AI_FLOTSAM_GOLD_AMOUNT);
        value_h = static_cast<i32>(
            gafAITurnCostResource[RES_WOOD] * AI_FLOTSAM_WOOD_AMOUNT);
        break;
    case AI_OBJECT_SEA_CHEST:
        if (cell_k->m_objectMetadata & 0x100)
            value_h = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] *
                    AI_SEA_CHEST_ARTIFACT_GOLD_AMOUNT +
                gArtifactBaseRV[cell_k->m_objectMetadata & 0xff]);
        else if (cell_k->m_objectMetadata == 1)
            value_h = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_SEA_CHEST_LARGE_GOLD_AMOUNT);
        else
            value_h = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_SEA_CHEST_SMALL_GOLD_AMOUNT);
        if (value_h <= 0)
            value_h = AI_SEA_CHEST_MINIMUM_VALUE;
        break;
    case AI_OBJECT_WAGON:
    case AI_OBJECT_LEAN_TO:
        if (!cell_k->m_objectMetadata)
            value_h = 0;
        else if (cell_k->m_objectMetadata & 0x80) {
            value_h = gArtifactBaseRV[cell_k->m_objectMetadata & 0x7f];
        } else {
            resource = (cell_k->m_objectMetadata & 0xf) - 1;
            amount_j = (cell_k->m_objectMetadata & 0xf0) >> 4;
            value_h = static_cast<i32>(gafAITurnCostResource[resource] * amount_j);
        }
        break;
    case AI_OBJECT_GENIE_LAMP:
        creature = CREATURE_GENIE;
        creatureFlag_k = 0;
        goto creature_purchase;
    case AI_OBJECT_TREE_CITY:
        creature = CREATURE_SPRITE;
        creatureFlag_k = 0;
        goto creature_purchase;
    case AI_OBJECT_WATCH_TOWER:
        creature = CREATURE_ORC;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_TREE_HOUSE:
        creature = CREATURE_SPRITE;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_EXCAVATION:
        creature = CREATURE_SKELETON;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_HALFLING_HOLE:
        creature = CREATURE_HALFLING;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_RUINS:
        creature = CREATURE_MEDUSA;
        creatureFlag_k = 0;
        goto creature_purchase;
    case AI_OBJECT_TROLL_BRIDGE:
        if (cell_k->m_objectMetadata & 0x100) {
            value_h = 0;
        } else {
            creature = CREATURE_TROLL;
            creatureFlag_k = 0;
            goto creature_purchase;
        }
        break;
    case AI_OBJECT_DRAGON_CITY:
        if (cell_k->m_objectMetadata & 0x100) {
            value_h = 0;
        } else {
            creature = CREATURE_RED_DRAGON;
            creatureFlag_k = 0;
            goto creature_purchase;
        }
        break;
    case AI_OBJECT_CITY_OF_DEAD:
        if (cell_k->m_objectMetadata & 0x100) {
            value_h = 0;
        } else {
            creature = CREATURE_POWER_LICH;
            creatureFlag_k = 0;
            goto creature_purchase;
        }
        break;
    case AI_OBJECT_CAVE:
        creature = CREATURE_CENTAUR;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_ARCHER_HOUSE:
        creature = CREATURE_ARCHER;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_GOBLIN_HUT:
        creature = CREATURE_GOBLIN;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_PEASANT_HUT:
        creature = CREATURE_PEASANT;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_DWARF_COTTAGE:
    case AI_OBJECT_SIRENS:
        creature = CREATURE_DWARF;
        creatureFlag_k = 1;
        goto creature_purchase;
    case AI_OBJECT_DESERT_TENT:
        creature = CREATURE_NOMAD;
        creatureFlag_k = 0;
        goto creature_purchase;
    case AI_OBJECT_WAGON_CAMP:
        creature = CREATURE_ROGUE;
        creatureFlag_k = 0;
creature_purchase:
        EvaluateOneTimeCreaturePurchase(creature, cell_k->m_objectMetadata, creatureFlag_k,
                                        purchaseCost_j, value_h, purchaseValue_n);
        gbReduceByReload = 0;
        break;
    case AI_OBJECT_SHIPWRECK_SURVIVOR:
        value_h = gArtifactBaseRV[
            cell_k->m_objectMetadata - AI_SHIPWRECK_SURVIVOR_ARTIFACT_METADATA_OFFSET];
        if (value_h < 125)
            value_h = 125;
        break;
    case AI_OBJECT_SKELETON:
        value_h = cell_k->m_objectMetadata == 1 ? 0 : gArtifactBaseRV[cell_k->m_objectMetadata];
        break;
    case AI_OBJECT_GRAVEYARD:
    case AI_OBJECT_SHIPWRECK:
    case AI_OBJECT_DERELICT_SHIP:
        value_h = FightEvent(gpCurAIHero, cell_k, 1);
        break;
    case AI_OBJECT_PYRAMID:
        if (!cell_k->m_objectMetadata) {
            value_h = 0;
        } else {
            index_k = cell_k->m_objectMetadata - 1;
            if (gsSpellInfo[index_k].attributes & 1) {
                battleStatMod_n = gpCurAIHero->Stats(2) > AI_MAX_BATTLE_STAT ?
                    gfBattleStat[AI_MAX_BATTLE_STAT] :
                    gfBattleStat[gpCurAIHero->Stats(2)];
                spellPowerMod_m = battleStatMod_n;
            } else {
                spellPowerMod_m = 1.0f;
            }
            battleValue_b = static_cast<i32>(
                gsSpellInfo[index_k].aiValue *
                gpCurPlayer->m_upgradeValueWeight * spellPowerMod_m);
            for (index_k = 0; index_k < AI_TOWN_ARMY_SLOTS; index_k++) {
                gpMonGroup->m_creatureTypes[index_k] = 0x33;
                gpMonGroup->m_quantities[index_k] = 10;
            }
            ChooseEvaluateBattle(&gpCurAIHero->m_army, gpCurAIHero, gpMonGroup, 0,
                                 0, 0, battleValue_b, battleWon_p, value_h);
            if (!battleWon_p)
                value_h = -50;
        }
        break;
    case AI_OBJECT_DAEMON_CAVE:
        if (cell_k->m_objectMetadata == 1)
            value_h = 0;
        else {
            value_h = static_cast<i32>(
                gafAITurnCostResource[RES_GOLD] * AI_DAEMON_GOLD_PENALTY +
                (gpCurAIHero->m_aiFightValue * AI_DAEMON_FIGHT_VALUE_FACTOR +
                 ((gpCurAIHero->m_aiFightValue *
                       AI_DAEMON_SECONDARY_FIGHT_VALUE_FACTOR +
                   gpCurPlayer->m_artifactValue) +
                  (gafAITurnCostResource[RES_GOLD] * AI_DAEMON_GOLD_VALUE_FACTOR +
                   gpCurAIHero->m_aiFightValue * AI_DAEMON_FIGHT_VALUE_FACTOR))));
            if (cell_k->m_objectMetadata == 5 &&
                gpCurPlayer->m_resources[RES_GOLD] <
                    AI_DAEMON_CAVE_GOLD_REQUIRED)
                value_h = -100;
        }
        break;
    case AI_OBJECT_ABANDONED_MINE:
        battleValue_b = static_cast<i32>(
            static_cast<float>(gaiTurnValueOfMine[(MAP_WIDTH | 0) * y + x]) *
            gMineCharacteristics[RES_GOLD] * gafAITurnCostResource[RES_GOLD]);
        for (index_k = 0; index_k < AI_TOWN_ARMY_SLOTS; index_k++) {
            gpMonGroup->m_creatureTypes[index_k] =
                gpGame->m_mines[cell_k->m_objectMetadata].guardianType;
            gpMonGroup->m_quantities[index_k] = static_cast<i16>(
                gpGame->m_mines[cell_k->m_objectMetadata].guardianCount / 5);
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
                if ((cell_k->m_triggerType | 0) == otherCell_j->m_triggerType &&
                    (cell_k->m_objectIndex | 0) == otherCell_j->m_objectIndex &&
                    (abs(x - exitX_p) | 0) + abs(y - exitY_p) >
                        AI_TRAVEL_GATE_EXIT_RADIUS) {
                    exitValue_i = StrategicValueOfPosition(
                        exitX_p, exitY_p, 0, 0, routeLiveChance_e, AI_TRAVEL_GATE_EXIT_DEPTH);
                    exitValue_i = static_cast<i32>(exitValue_i * AI_TRAVEL_GATE_EXIT_SCALE);
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
        if (gpCurAIHero->m_fortVisits & (1U << cell_k->m_objectMetadata))
            value_h = 0;
        else
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_WITCH_DOCTORS_HUT:
        if (gpCurAIHero->m_witchDoctorVisits & (1U << cell_k->m_objectMetadata))
            value_h = 0;
        else
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_STANDING_STONES:
        if (gpCurAIHero->m_standingStoneVisits & (1U << cell_k->m_objectMetadata))
            value_h = 0;
        else
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_MERCENARY_CAMP:
        if (gpCurAIHero->m_mercenaryCampVisits & (1U << cell_k->m_objectMetadata))
            value_h = 0;
        else
            value_h = static_cast<i32>(
                gpCurAIHero->m_aiFightValue * AI_TRAINING_SITE_VALUE_FACTOR);
        break;
    case AI_OBJECT_XANADU:
        if (gpCurAIHero->m_xanaduVisits & (1U << cell_k->m_objectMetadata))
            value_h = 0;
        else {
            if (gpCurAIHero->m_level + gpCurAIHero->m_secondarySkills[4] * 2 >= 10)
                value_h = static_cast<i32>(
                    gpCurAIHero->m_aiFightValue * AI_XANADU_VALUE_FACTOR);
            else
                value_h = 0;
        }
        break;
    case AI_OBJECT_LIGHTHOUSE:
        if (gpGame->m_mines[cell_k->m_objectMetadata].owner == gpCurAIHero->m_owner ||
            OnMySide(gpGame->m_mines[cell_k->m_objectMetadata].owner))
            value_h = 0;
        else
            value_h = 1000;
        break;
    case AI_OBJECT_WATER_WHEEL:
        value_h = static_cast<i32>(
            cell_k->m_objectMetadata * AI_WATER_WHEEL_GOLD_AMOUNT *
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
        value_h = (ComputeUpgradeValue(CREATURE_DWARF, CREATURE_BATTLE_DWARF) | 0) +
                (((ComputeUpgradeValue(CREATURE_ORC, CREATURE_ORC_CHIEF) | 0) +
                  (ComputeUpgradeValue(CREATURE_OGRE, CREATURE_OGRE_LORD) | 0)) | 0);
        break;
    case AI_OBJECT_FREEMANS_FOUNDRY:
        value_h = (ComputeUpgradeValue(
                       CREATURE_SWORDSMAN, CREATURE_MASTER_SWORDSMAN) | 0) +
                (((ComputeUpgradeValue(CREATURE_PIKEMAN,
                                       CREATURE_VETERAN_PIKEMAN)) +
                  (ComputeUpgradeValue(CREATURE_IRON_GOLEM,
                                       CREATURE_STEEL_GOLEM) | 0)) | 0);
        break;
    case AI_OBJECT_MAGIC_WELL:
        value_h = ManaRefreshValue(gpCurAIHero, 1);
        break;
    case AI_OBJECT_ARTESIAN_SPRING:
        if (!cell_k->m_objectMetadata)
            value_h = 0;
        else
            value_h = ManaRefreshValue(gpCurAIHero, 2);
        break;
    case AI_OBJECT_WITCHS_HUT:
        value_h = ComputeValueOfFreeSS(gpCurAIHero, cell_k->m_objectMetadata);
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
        sprintf(gText, "AI encountered object type %d and doesn't know how to deal with it.   Tell Phil",
                cell_k->m_triggerType & 0x7f);
        NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        value_h = 0;
        break;
        }
    } else if (!(giCurPlayerBit & mapExtra[(MAP_WIDTH | 0) * y + x])) {
        value_h = 5;
    }

    if (gbTroopReload && gbReduceByReload)
        value_h = static_cast<i32>(value_h * fReduceFactor);
    if (gbBerserk && gbReduceByBerserk)
        value_h = static_cast<i32>(value_h * fBerserkFactor);
    if (!immediate) {
        if (value_h > 0 && (mapExtra[(MAP_WIDTH | 0) * y + x] & 0x80) &&
            (cell_k->m_triggerType & 0x7f) != AI_OBJECT_MONSTER)
            value_h = 0;
        if (value_h < 0 && (cell_k->m_triggerType & 0x7f) != AI_OBJECT_HERO_EVENT)
            value_h = 0;
        else if (value_h > 32000)
            value_h = 32000;
        else if (value_h < -32000)
            value_h = -32000;
        *reinterpret_cast<i16 *>(reinterpret_cast<char *>(gaiHeroEventStratRVOfPos) +
                                   (MAP_WIDTH | 0) * y * sizeof(i16) + x * sizeof(i16)) =
            static_cast<i16>(value_h);
    }
    return value_h;
}

// @early-stop
// All 0x299 bytes match after masking 33 aligned COFF relocations.
// The residual is constant/field symbol identity plus jump-table labels delinked
// as this function; all external calls and globals agree.
VA(0x00445a89, 0x299)
i32 philAI::EvaluateGenericSite(mapCell *cell) {
    i32 unusedValue29;
    i32 artifactIndex1;
    i32 siteLevel5;
    i32 siteType0;
    i32 value1;
    i32 cursedArtifactCount2;
    i32 creatureType2;
    i32 removedQuantity3;
    i32 quantity6;
    i32 armyValue7;

    cursedArtifactCount2 = 0;
    siteType0 = cell->m_tentColor;
    siteType0 &= AI_GENERIC_SITE_TYPE_MASK;
    siteLevel5 = cell->m_tentColor;
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
        if (gpCurPlayer->m_resources[RES_GOLD] >
            AI_GENERIC_SITE_GOLD_THRESHOLD) {
            value1 = cursedArtifactCount2 * AI_GENERIC_SITE_CURSED_ARTIFACT_VALUE;
        }
        break;
    case AI_GENERIC_SITE_SHIPWRECK:
        if (!(gpCurAIHero->m_eventFlags & AI_GENERIC_SITE_SHIPWRECK_FLAG)) {
            value1 = static_cast<i32>(gpCurAIHero->m_aiFightValue *
                                      AI_GENERIC_SITE_SHIPWRECK_VALUE);
        }
        break;
    case AI_GENERIC_SITE_FAERIE_RING:
        if (!(gpCurAIHero->m_eventFlags & AI_GENERIC_SITE_FAERIE_RING_FLAG) &&
            giCurAIHeroLuck < AI_GENERIC_SITE_MAX_LUCK) {
            value1 = static_cast<i32>(gpCurAIHero->m_aiFightValue *
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
                    removedQuantity3 = static_cast<i32>(
                        quantity6 * AI_GENERIC_SITE_GRAVEYARD_REMAINING);
                    armyValue7 += gMonsterDatabase[creatureType2].hitPoints *
                                  (quantity6 - removedQuantity3);
                }
            }
            value1 = static_cast<i32>(armyValue7 * gpCurAIHero->m_aiFightValue);
        }
        break;
    case AI_GENERIC_SITE_CREATURE_UPGRADE:
        value1 = ComputeUpgradeValue(AI_GENERIC_SITE_UPGRADE_FROM,
                                     AI_GENERIC_SITE_UPGRADE_TO);
        if (!(gpCurAIHero->m_eventFlags & AI_GENERIC_SITE_CREATURE_UPGRADE_FLAG)) {
            value1 = static_cast<i32>(
                value1 +
                (AI_GENERIC_SITE_WEEK_END - gpGame->m_day) *
                    AI_GENERIC_SITE_DAY_VALUE * gpCurAIHero->m_aiFightValue);
        }
        break;
    }
    return value1;
}

VA(0x00445d22, 0x5e)
i32 philAI::EvaluateBarrier(mapCell *cell) {
    i32 color = cell->m_tentColor;
    color &= 7;
    if (gpCurPlayer->m_barrierTents & (1 << color))
        return 5000;
    else
        return 0;
}

VA(0x00445d80, 0x5e)
i32 philAI::EvaluatePassword(mapCell *cell) {
    i32 color = cell->m_tentColor;
    color &= 7;
    if (!(gpCurPlayer->m_barrierTents & (1 << color)))
        return 2500;
    else
        return 0;
}

// @early-stop
// Complete & correct except the `switch` jump table: cl references it as a local ($L)
// label (jmpl disp 0); the delinker folds the in-.text table into the function symbol
// (jmpl disp 0xa9 + per-entry relocs to the function). Same target; delinker artifact.
VA(0x00445dde, 0xf5)
i32 philAI::EvaluateRecruitSite(mapCell *cell) {
    i32 val;            // value (returned)
    i32 node;           // color
    i32 nb;             // monster type
    i32 kn;             // out param
    i32 jb;             // unused
    i32 idx;            // out param
    i16 lvl;          // monster level
    node = cell->m_tentColor;
    node &= 7;
    lvl = cell->m_tentColor;
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
i32 philAI::EvaluateJail(mapCell *) {
    return 10000;
}

VA(0x00445ef0, 0xf6)
void InitAIMapVars(void) {
    // This scalar-lvalue spelling makes VC4.2 /Od emit the retail width-then-height
    // relocation order at all five commutative products.
    CloseAIMapVars();
    SVSearchArray.Init();
    gaiLiveChanceOfPos = static_cast<i16 *>(
        BaseAlloc(0[&MAP_WIDTH] * MAP_HEIGHT * 2, PHFILE,
                  s_initAIMapLineBase + 8));
    gaiHeroStrategicRVOfPos = static_cast<i16 *>(
        BaseAlloc(0[&MAP_WIDTH] * MAP_HEIGHT * 2, PHFILE,
                  s_initAIMapLineBase + 9));
    gaiHeroEventStratRVOfPos = static_cast<i16 *>(
        BaseAlloc(0[&MAP_WIDTH] * MAP_HEIGHT * 2, PHFILE,
                  s_initAIMapLineBase + 10));
    gaiTurnValueOfMine = static_cast<i8 *>(
        BaseAlloc(0[&MAP_WIDTH] * MAP_HEIGHT, PHFILE,
                  s_initAIMapLineBase + 11));
    gaiEnemyHeroReachable = static_cast<i8 *>(
        BaseAlloc(0[&MAP_WIDTH] * MAP_HEIGHT, PHFILE,
                  s_initAIMapLineBase + 12));
}

VA(0x00445fe6, 0x112)
void CloseAIMapVars(void) {
    if (gaiLiveChanceOfPos != 0)
        BaseFree(gaiLiveChanceOfPos, PHFILE, s_closeAIMapLineBase + 1);
    if (gaiHeroStrategicRVOfPos != 0)
        BaseFree(gaiHeroStrategicRVOfPos, PHFILE, s_closeAIMapLineBase + 2);
    if (gaiHeroEventStratRVOfPos != 0)
        BaseFree(gaiHeroEventStratRVOfPos, PHFILE, s_closeAIMapLineBase + 3);
    if (gaiTurnValueOfMine != 0)
        BaseFree(gaiTurnValueOfMine, PHFILE, s_closeAIMapLineBase + 4);
    if (gaiEnemyHeroReachable != 0)
        BaseFree(gaiEnemyHeroReachable, PHFILE, s_closeAIMapLineBase + 5);
    gaiLiveChanceOfPos = 0;
    gaiHeroStrategicRVOfPos = 0;
    gaiHeroEventStratRVOfPos = 0;
    gaiTurnValueOfMine = 0;
    gaiEnemyHeroReachable = 0;
    SVSearchArray.Close();
}

// @semantic: Complete 0x26a-byte CFG, fastcall frame, and all 25 ordered
// relocations align. At normalized instruction 7 retail loads player into eax
// then compares giCurPlayer; ours loads giCurPlayer then compares player. Direct
// operand reversal, negated inequality, and a value-preserving `player | 0` pin
// were neutral. Revisit after PHILAI TU/header state changes or in last-mile.
VA(0x004460f8, 0x26a)
i32 OnMySide(i32 player) {
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
            ((gpGame->m_players[giCurPlayer].m_color <
                  gpGame->m_mapHeader.victorySideThreshold &&
              gpGame->m_players[player].m_color <
                  gpGame->m_mapHeader.victorySideThreshold) ||
             (gpGame->m_players[giCurPlayer].m_color >=
                  gpGame->m_mapHeader.victorySideThreshold &&
              gpGame->m_players[player].m_color >=
                  gpGame->m_mapHeader.victorySideThreshold))))) ||
         (gbInCampaign &&
          gpGame->m_campaignType == AI_SIDE_CAMPAIGN_TYPE_ZERO &&
          gpGame->m_campaignScenario + AI_SIDE_CAMPAIGN_SCENARIO_OFFSET ==
              AI_SIDE_CAMPAIGN_SCENARIO_NINE &&
          gpGame->m_players[player].m_color != AI_SIDE_FIRST_COLOR &&
          gpGame->m_players[player].m_color != AI_SIDE_FOURTH_COLOR))) {
        return 1;
    } else
        return 0;
}

// @early-stop
// Exact 0x48 frame and 30/30 external relocations. Excluding the 0x1c-byte jump
// table, every non-jump opcode and operand matches; retail has one extra five-byte
// inline continuation after HasSpell. The size delta is exactly that one jump.
VA(0x00446362, 0x2bc)
i32 philAI::EvaluateArtifactEvent(i32 artifact, i32 eventData) {
    i32 battleScratch46;
    i32 guardedValue2;
    i32 result5;
    i32 defaultValue37;
    i32 outcomeValue17;
    i32 defenderRemaining6;
    i32 stackIndex29;
    i32 attackerRemaining3;
    i32 defenderLoss4;
    i32 battleScratch13;
    i32 attackerLoss6;
    i32 battleScratch2;
    float winChance9;
    i32 artifactValue15;

    if (gpCurAIHero->NumArtifacts() == AI_BATTLE_ARTIFACT_SLOT_COUNT)
        return 0;

    result5 = 0;
    artifactValue15 = gArtifactBaseRV[artifact];

    if (artifact == ARTIFACT_SPELL_SCROLL) {
        if (gpCurAIHero->HasSpell(eventData))
            return artifactValue15;
        return artifactValue15 + gsSpellInfo[eventData].aiValue;
    }

    defaultValue37 = artifactValue15;
    if (eventData & AI_ARTIFACT_EVENT_GUARD_FLAG) {
        for (stackIndex29 = 0; stackIndex29 < ARMY_GROUP_SLOT_COUNT; stackIndex29++) {
            gpMonGroup->m_creatureTypes[stackIndex29] = static_cast<i8>(eventData);
            if (gpMonGroup->m_creatureTypes[stackIndex29] == CREATURE_ROGUE)
                gpMonGroup->m_quantities[stackIndex29] =
                    AI_ARTIFACT_EVENT_GUARD_ROGUE_COUNT;
            else if (stackIndex29 == 0)
                gpMonGroup->m_quantities[stackIndex29] = 1;
            else
                gpMonGroup->m_quantities[stackIndex29] = 0;
        }
        ProbableOutcomeOfBattle(&gpCurAIHero->m_army, gpCurAIHero, gpMonGroup, 0,
                                0, 0, 0, -1, winChance9, attackerLoss6,
                                defenderLoss4, attackerRemaining3, defenderRemaining6,
                                outcomeValue17);
        guardedValue2 = static_cast<i32>(
            gArtifactBaseRV[artifact] * winChance9 + outcomeValue17);
        if (guardedValue2 < 0)
            guardedValue2 = 0;
        result5 = guardedValue2;
    } else {
        switch (eventData & AI_ARTIFACT_EVENT_MODE_MASK) {
        case AI_ARTIFACT_EVENT_VALUE:
            result5 = defaultValue37;
            break;
        case AI_ARTIFACT_EVENT_REQUIRES_WISDOM:
            if (gpCurAIHero->m_secondarySkills[HERO_SKILL_WISDOM] != 0)
                result5 = defaultValue37;
            else
                result5 = 0;
            break;
        case AI_ARTIFACT_EVENT_REQUIRES_LEADERSHIP:
            if (gpCurAIHero->m_secondarySkills[HERO_SKILL_LEADERSHIP] != 0)
                result5 = defaultValue37;
            else
                result5 = 0;
            break;
        case AI_ARTIFACT_EVENT_NO_VALUE:
            break;
        case AI_ARTIFACT_EVENT_PAY_GOLD:
            result5 = NetValueOfArtifact(
                artifact, AI_ARTIFACT_EVENT_GOLD_COST, 0, 0);
            break;
        case AI_ARTIFACT_EVENT_PAY_RESOURCE_THREE:
            result5 = NetValueOfArtifact(
                artifact, AI_ARTIFACT_EVENT_RESOURCE_THREE_COST,
                (eventData & AI_ARTIFACT_EVENT_RESOURCE_MASK) >>
                    AI_ARTIFACT_EVENT_RESOURCE_SHIFT,
                AI_ARTIFACT_EVENT_RESOURCE_THREE_AMOUNT);
            break;
        case AI_ARTIFACT_EVENT_PAY_RESOURCE_FIVE:
            result5 = NetValueOfArtifact(
                artifact, AI_ARTIFACT_EVENT_RESOURCE_FIVE_COST,
                (eventData & AI_ARTIFACT_EVENT_RESOURCE_MASK) >>
                    AI_ARTIFACT_EVENT_RESOURCE_SHIFT,
                AI_ARTIFACT_EVENT_RESOURCE_FIVE_AMOUNT);
            break;
        }
    }
    return result5;
}

// @semantic: Current live score is 92.91% after the corrected PHILAI float/double
// declarations moved TU-cumulative codegen; this unchanged source hash retains
// a prior exact maximum. All 244 instructions, the 0x30 frame and exact slots,
// the 778-byte extent, and 38/38 relocation targets are complete. Ten guarded
// probes again reached disposable 100% but failed global eligibility. Forcing
// MAP_WIDTH-first multiplication regressed FPU scheduling and was discarded.
// Revisit only after PHILAI TU/header state stabilizes for final maxing.
VA(0x0044661e, 0x30a)
i32 philAI::EvaluateMineEvent(i32 mineIndex, i32 x, i32 y, i32 *liveChance) {
    float winChance;
    i32 attackerLoss2;
    i32 guardianCount5;
    i32 defenderLoss;
    i32 attackerRemaining;
    i32 defenderRemaining0;
    i32 stackIndex1;
    i32 result1 = 0;
    i32 outcomeValue4;
    i32 mineValue0;
    float attackBonus0;
    if (gpGame->m_mineOwners[mineIndex] == gpCurAIHero->m_owner ||
        OnMySide(gpGame->m_mineOwners[mineIndex]))
        return result1;

    if (gpGame->m_mines[mineIndex].guardianType != ARMY_GROUP_EMPTY_SLOT) {
        guardianCount5 = gpGame->m_mines[mineIndex].guardianCount;
        memset(gpMonGroup->m_creatureTypes, ARMY_GROUP_EMPTY_SLOT,
               ARMY_GROUP_SLOT_COUNT);
        memset(gpMonGroup->m_quantities, 0,
               ARMY_GROUP_SLOT_COUNT * sizeof(gpMonGroup->m_quantities[0]));

        if (guardianCount5 / ARMY_GROUP_SLOT_COUNT > 0) {
            for (stackIndex1 = 0; stackIndex1 < ARMY_GROUP_SLOT_COUNT; stackIndex1++) {
                gpMonGroup->m_creatureTypes[stackIndex1] =
                    gpGame->m_mines[mineIndex].guardianType;
                gpMonGroup->m_quantities[stackIndex1] = static_cast<i16>(
                    guardianCount5 / ARMY_GROUP_SLOT_COUNT);
            }
        }
        for (stackIndex1 = guardianCount5 % ARMY_GROUP_SLOT_COUNT - 1;
             stackIndex1 >= 0; stackIndex1--) {
            gpMonGroup->m_creatureTypes[stackIndex1] =
                gpGame->m_mines[mineIndex].guardianType;
            gpMonGroup->m_quantities[stackIndex1]++;
        }

        ProbableOutcomeOfBattle(
            &gpCurAIHero->m_army, gpCurAIHero, gpMonGroup, 0, 0, 0, 0, -1,
            winChance, attackerLoss2, defenderLoss, attackerRemaining,
            defenderRemaining0, outcomeValue4);
        *liveChance = static_cast<i32>(winChance);
        result1 = outcomeValue4;
    }

    if (gbIAmGreatest && gpGame->m_mineOwners[mineIndex] >= 0 &&
        !gbHumanPlayer[gpGame->m_mineOwners[mineIndex]])
        return result1;
    else {
        mineValue0 = static_cast<i32>(
            static_cast<float>(
                gMineCharacteristics[gpGame->m_mines[mineIndex].resourceType]) *
            gafAITurnCostResource[gpGame->m_mines[mineIndex].resourceType] *
            gaiTurnValueOfMine[x + MAP_WIDTH * y]);
        if (gpGame->m_mineOwners[mineIndex] >= 0) {
            if (gbHumanPlayer[gpGame->m_mineOwners[mineIndex]])
                attackBonus0 = gfAttackHumanBonus;
            else
                attackBonus0 = gfAttackComputerBonus;
            mineValue0 = static_cast<i32>(mineValue0 * attackBonus0);
        }
        result1 += mineValue0;
    }
    return result1;
}

// @semantic: The 0x33e frame/CFG and all 40 ordered relocation semantics agree.
// The unmasked ModRM bytes at +0x277 and +0x286 exchange equivalent register roles;
// the remaining residuals are compiler float-constant and division-guard identities.
// This is not a relocation-only early stop. Revisit after PHILAI compiler state changes.
VA(0x00446928, 0x33e)
i32 philAI::EvaluateMonsterEvent(i32 monsterType, i32 eventData, i32 *liveChance) {
    i32 result5;
    i32 outcomeValue0;
    i32 defenderRemaining6;
    i32 stackIndex29;
    i32 attackerRemaining3;
    i32 defenderLoss4;
    i32 willJoin15;
    i32 attackerLoss6;
    i32 monsterCount4;
    i32 unusedPurchaseValue7;
    float winChance9;
    float strengthRatio26;
    i32 purchaseCost9;

    monsterCount4 = eventData & 0xfff;
    willJoin15 = eventData & 0x1000;
    result5 = 0;
    memset(gpMonGroup->m_creatureTypes, ARMY_GROUP_EMPTY_SLOT,
           ARMY_GROUP_SLOT_COUNT);
    memset(gpMonGroup->m_quantities, 0,
           ARMY_GROUP_SLOT_COUNT * sizeof(gpMonGroup->m_quantities[0]));
    if (monsterCount4 / ARMY_GROUP_SLOT_COUNT > 0) {
        for (stackIndex29 = 0; stackIndex29 < ARMY_GROUP_SLOT_COUNT; stackIndex29++) {
            gpMonGroup->m_creatureTypes[stackIndex29] = static_cast<i8>(monsterType);
            gpMonGroup->m_quantities[stackIndex29] = static_cast<i16>(
                monsterCount4 / ARMY_GROUP_SLOT_COUNT);
        }
    }
    for (stackIndex29 = monsterCount4 % ARMY_GROUP_SLOT_COUNT - 1;
         stackIndex29 >= 0; stackIndex29--) {
        gpMonGroup->m_creatureTypes[stackIndex29] = static_cast<i8>(monsterType);
        gpMonGroup->m_quantities[stackIndex29]++;
    }

    ProbableOutcomeOfBattle(&gpCurAIHero->m_army, gpCurAIHero, gpMonGroup, 0,
                            0, 0, 0, -1, winChance9, attackerLoss6,
                            defenderLoss4, attackerRemaining3, defenderRemaining6,
                            outcomeValue0);
    EvaluateOneTimeCreaturePurchase(monsterType, monsterCount4, 1,
                                    purchaseCost9, attackerLoss6, unusedPurchaseValue7);
    strengthRatio26 = static_cast<float>(gpPhilAI->FightValueOfStack(
                          &gpCurAIHero->m_army, gpCurAIHero, 0, 0, 0, 0)) /
                      static_cast<float>(gMonsterDatabase[monsterType].fightValue * monsterCount4);

    if (willJoin15 && strengthRatio26 > AI_MONSTER_JOIN_RATIO &&
        !gpCurAIHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) &&
        gpCurAIHero->m_army.CanJoin(monsterType) &&
        monsterType != CREATURE_GHOST &&
        monsterType != CREATURE_EARTH_ELEMENTAL &&
        monsterType != CREATURE_AIR_ELEMENTAL &&
        monsterType != CREATURE_FIRE_ELEMENTAL &&
        monsterType != CREATURE_WATER_ELEMENTAL) {
        *liveChance = 100;
        *liveChance = static_cast<i32>(winChance9 * AI_MONSTER_JOIN_CHANCE_SCALE +
                                       AI_MONSTER_JOIN_CHANCE_BASE);
        if (gpCurAIHero->m_army.CanJoin(monsterType))
            result5 = attackerLoss6;
        else
            result5 = 0;
        result5 = static_cast<i32>(result5 * AI_MONSTER_JOIN_PURCHASE_WEIGHT +
                                   outcomeValue0 * AI_MONSTER_JOIN_OUTCOME_WEIGHT);
    } else if (strengthRatio26 > AI_MONSTER_OVERWHELMING_RATIO) {
        if (gpCurAIHero->GetSSLevel(HERO_SKILL_NECROMANCY) != 0)
            result5 = 120;
        else
            result5 = 0;
        result5 += gMonsterDatabase[monsterType].hitPoints * monsterCount4;
    } else {
        *liveChance = static_cast<i32>(winChance9 * AI_MONSTER_FIGHT_CHANCE_SCALE);
        result5 = outcomeValue0;
    }

    if (result5 < 0)
        gbReduceByReload = 0;
    return result5;
}

// @semantic
// Battle outputs, town-defense context, live-chance bands, and the unconditional
// berserk reset match retail. The frame is 0x54 and all 73 external relocations
// agree; residuals are /Od branch trampolines, constant identities, and equivalent
// comparison/x87 expression order.
VA(0x00446c66, 0x5ab)
i32 philAI::EvaluateHeroEvent(i32 heroId, i32 x, i32 y, i32 mode, i32 *liveChance) {
    float attackBonus6;
    town *townPtr29;
    i32 townId8;
    hero *enemyHero6;
    i32 townValue8;
    armyGroup *townArmy26;
    i32 result5;
    i32 battleScratch2;
    i32 defenderRemaining6;
    i32 attackerRemaining3;
    i32 defenderLoss4;
    i32 attackerLoss6;
    float winChance9;

    if (gpGame->m_availableHeroes[heroId] == gpCurAIHero->m_owner) {
        if (mode == AI_EVENT_MODE_IGNORE)
            result5 = 0;
        else if (mode == AI_EVENT_MODE_AVOID)
            result5 = AI_EVENT_SEVERE_PENALTY;
        else
            result5 = AI_EVENT_FRIENDLY_PENALTY;

        if (giCurTurn - gpCurAIHero->m_lastHeroInteractionTurn >=
                AI_EVENT_INTERACTION_AGE ||
            gpGame->m_heroRecs[heroId].m_id !=
                gpCurAIHero->m_lastInteractionHeroId) {
            HeroInteractionAtHero(gpCurAIHero, &gpGame->m_heroRecs[heroId], 1,
                                  &result5);
        }
        gaiHeroLiveChance[heroId] = 100;
        return result5;
    }

    if (OnMySide(gpGame->m_availableHeroes[heroId])) {
        if (mode == AI_EVENT_MODE_IGNORE)
            result5 = 0;
        else if (mode == AI_EVENT_MODE_AVOID)
            result5 = AI_EVENT_SEVERE_PENALTY;
        else
            result5 = AI_EVENT_ALLIED_PENALTY;
        return result5;
    }

    if (gbIAmGreatest && !gbHumanPlayer[gpGame->m_availableHeroes[heroId]]) {
        result5 = 0;
        *liveChance = 100;
        return result5;
    }

    result5 = 0;
    townValue8 = 0;
    townPtr29 = 0;
    townArmy26 = 0;
    enemyHero6 = &gpGame->m_heroRecs[heroId];
    if (enemyHero6->m_locationType == AI_OBJECT_TOWN) {
        townPtr29 = &gpGame->m_castleRecs[enemyHero6->m_occupiedTown];
        townArmy26 = &townPtr29->m_army;
        townValue8 = ValueOfTown(townPtr29);
        townId8 = townPtr29->m_id;
        if (townPtr29->m_owner >= 0) {
            if (gbHumanPlayer[townPtr29->m_owner])
                attackBonus6 = gfAttackHumanBonus;
            else
                attackBonus6 = gfAttackComputerBonus;
            townValue8 = static_cast<i32>(
                ((AI_EVENT_EARLY_TURN_DIFFICULTY_STEP - gpGame->m_playerCount) *
                     AI_EVENT_TOWN_PLAYER_FACTOR * attackBonus6 +
                 AI_EVENT_VALUE_BASE_FACTOR) * townValue8);
        }
    } else {
        townId8 = TOWN_ID_NONE;
    }

    ProbableOutcomeOfBattle(&gpCurAIHero->m_army, gpCurAIHero,
                            &enemyHero6->m_army, enemyHero6, townArmy26,
                            townArmy26 != 0, townId8, enemyHero6->m_owner,
                            winChance9, attackerLoss6, defenderLoss4,
                            attackerRemaining3, defenderRemaining6, result5);
    *liveChance = static_cast<i32>(winChance9 * AI_EVENT_CERTAIN_CHANCE);
    if (townValue8 > 0)
        result5 = static_cast<i32>(townValue8 * winChance9 + result5);

    if (gbHumanPlayer[enemyHero6->m_owner] &&
        result5 > AI_EVENT_HUMAN_VALUE_THRESHOLD) {
        if (gpCurPlayer->m_aiDifficulty == 0)
            result5 *= 2;
        else
            result5 = static_cast<i32>(result5 * AI_EVENT_HUMAN_VALUE_FACTOR);
    }

    if (winChance9 > AI_EVENT_CERTAIN_ODDS)
        gaiHeroLiveChance[heroId] = 100;
    else if (winChance9 > AI_EVENT_HIGH_ODDS)
        gaiHeroLiveChance[heroId] = static_cast<i16>(
            winChance9 * AI_EVENT_HIGH_CHANCE_SCALE);
    else if (winChance9 > AI_EVENT_GOOD_ODDS)
        gaiHeroLiveChance[heroId] = static_cast<i16>(
            winChance9 * AI_EVENT_GOOD_CHANCE_SCALE);
    else if (winChance9 > AI_EVENT_POOR_ODDS)
        gaiHeroLiveChance[heroId] = static_cast<i16>(
            winChance9 * AI_EVENT_POOR_CHANCE_SCALE);
    else if (winChance9 > AI_EVENT_BAD_ODDS)
        gaiHeroLiveChance[heroId] = static_cast<i16>(
            winChance9 * AI_EVENT_BAD_CHANCE_SCALE);
    else
        gaiHeroLiveChance[heroId] = static_cast<i16>(
            winChance9 * AI_EVENT_CERTAIN_CHANCE);
    if (gaiHeroLiveChance[heroId] > 100)
        gaiHeroLiveChance[heroId] = 100;

    if (mode == AI_EVENT_MODE_IGNORE && winChance9 < AI_EVENT_GOOD_ODDS)
        result5 = static_cast<i32>((3.0f - winChance9 * 2.0f) * result5);
    if (mode == AI_EVENT_MODE_IGNORE && winChance9 < AI_EVENT_BAD_ODDS)
        result5 = static_cast<i32>((2.0f - winChance9 * 2.0f) * result5);

    if (result5 < 0)
        gbReduceByReload = 0;
    gbReduceByBerserk = 0;
    if (result5 > 0 &&
        AI_EVENT_EARLY_TURN_BASE -
                gpGame->m_difficulty * AI_EVENT_EARLY_TURN_DIFFICULTY_STEP >
            giCurTurn &&
        !(mapExtra[x + MAP_WIDTH * y] & giCurPlayerBit)) {
        result5 = 0;
    }
    return result5;
}

// @early-stop
// The real evaluator ends at target .text+0xff03 (`ret 0x14`). The assigned target
// span then contains 12 INT3 bytes and .text+0xff0f..0xff79: four xCampaign static
// initialization/termination thunks with _atexit, constructor, and destructor calls.
// Our compiler emits the same thunks as separately named _$E symbols, so objdiff
// excludes them from this function. Over the real body all 48 external relocation
// occurrences agree; normalized instructions differ only by one equivalent compare
// load order, three zero-distance inline continuations, and local constant identities.
VA(0x00447211, 0x4fa)
i32 philAI::EvaluateTownEvent(i32 townId, i32 x, i32 y, i32 mode, i32 *liveChance) {
    float ra;       // attack bonus, -0x28
    i32 py;         // town value, -0x24
    town *p;        // town, -0x20
    i32 node = 0;   // result, -0x1c
    i32 result;     // battle outcome, -0x18
    i32 nb;         // defender remaining, -0x14
    i32 kn;         // attacker remaining, -0x10
    i32 jb;         // defender loss, -0x0c
    i32 idx;        // attacker loss, -0x08
    float val;      // win chance, -0x04

    p = gpGame->GetTown(townId);

    if (gpGame->m_townOwners[townId] == gpCurAIHero->m_owner) {
        if (p->m_occupyingHeroId != TOWN_ID_NONE) {
            if (mode == AI_EVENT_MODE_IGNORE)
                node = 0;
            else
                node = AI_EVENT_FRIENDLY_PENALTY;
        } else {
            node = 0;
            if (giCurTurn - gpCurAIHero->m_lastTownInteractionTurn >=
                    AI_EVENT_INTERACTION_AGE ||
                p->m_id != gpCurAIHero->m_visitedTownId) {
                HeroInteractionAtTown(gpCurAIHero, p, 1, &node);
                node = static_cast<i32>(
                    node * gfHeroInteractionBonus[gpCurAIHero->m_id]);
            }
        }
        gbReduceByReload = 0;
    } else if (OnMySide(gpGame->m_townOwners[townId])) {
        if (mode == AI_EVENT_MODE_IGNORE)
            node = 0;
        else
            node = AI_EVENT_ALLIED_PENALTY;
    } else if (gbIAmGreatest && gpGame->m_townOwners[townId] >= 0 &&
               !gbHumanPlayer[gpGame->m_townOwners[townId]]) {
        node = 0;
    } else if (gpGame->m_difficulty < AI_TOWN_EARLY_DIFFICULTY_LIMIT &&
               AI_TOWN_EARLY_TURN_BASE -
                       gpGame->m_difficulty * AI_TOWN_EARLY_TURN_DIFFICULTY_STEP >
                   giCurTurn &&
               !(mapExtra[x + MAP_WIDTH * y] & giCurPlayerBit)) {
        node = 0;
    } else {
        py = ValueOfTown(p);
        if (gpGame->m_castleRecs[townId].m_occupyingHeroId != TOWN_ID_NONE) {
            ProbableOutcomeOfBattle(&gpCurAIHero->m_army, gpCurAIHero,
                                    &gpGame->m_heroRecs[p->m_occupyingHeroId].m_army,
                                    &gpGame->m_heroRecs[p->m_occupyingHeroId],
                                    &p->m_army, 1, townId,
                                    p->m_owner, val, idx, jb, kn, nb, result);
        } else if (p->HasGarrison()) {
            ProbableOutcomeOfBattle(&gpCurAIHero->m_army, gpCurAIHero,
                                    &p->m_army, 0, 0, 1, townId,
                                    p->m_owner, val, idx, jb, kn, nb, result);
        } else {
            val = 1.0f;
            result = 0;
        }
        *liveChance = static_cast<i32>(AI_EVENT_CERTAIN_CHANCE * val);

        if (p->m_owner >= 0) {
            if (gbHumanPlayer[p->m_owner])
                ra = gfAttackHumanBonus;
            else
                ra = gfAttackComputerBonus;
            py = static_cast<i32>(py *
                (((AI_EVENT_EARLY_TURN_DIFFICULTY_STEP - gpGame->m_playerCount) *
                      AI_EVENT_TOWN_PLAYER_FACTOR +
                  AI_EVENT_VALUE_BASE_FACTOR) * ra));
        }
        node = static_cast<i32>(py * val + result);
        if (gpGame->m_townOwners[townId] != TOWN_OWNER_NONE)
            gbReduceByBerserk = 0;
    }

    if (p->m_owner != TOWN_OWNER_NONE &&
        gbHumanPlayer[p->m_owner] &&
        node > AI_EVENT_HUMAN_VALUE_THRESHOLD) {
        if (gpCurPlayer->m_aiDifficulty == 0)
            node = static_cast<i32>(node * AI_TOWN_PRIMARY_HUMAN_VALUE_FACTOR);
        else
            node = static_cast<i32>(node * AI_TOWN_OTHER_HUMAN_VALUE_FACTOR);
    }
    return node;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f20e0) float fFirstWeekTownFV = 0.0f;
DATA(0x004f20e4) i32 iVepCacheHits = 0;
DATA(0x004f20e8) i32 iTotalVepHits = 0;
DATA(0x004f20ec) i32 giShowComputerRoute = 0;
DATA(0x004f20f0) i16 *gaiLiveChanceOfPos = 0;
DATA(0x004f20f4) i16 *gaiHeroStrategicRVOfPos = 0;
DATA(0x004f20f8) i16 *gaiHeroEventStratRVOfPos = 0;
DATA(0x004f20fc) i8 *gaiTurnValueOfMine = 0;
DATA(0x004f2100) i8 *gaiEnemyHeroReachable = 0;
DATA(0x004f2104) i32l glLastStartTick = 0;
DATA(0x004f2108) i32l glCurTicks = 0;
DATA(0x004f210c) i32l glTotalTicks = 0;
DATA(0x004f2110) hero *gpCurAIHero = 0;
DATA(0x004f2114) float gfAttackHumanBonus = 2.0f;
DATA(0x004f2118) float gfAttackComputerBonus = 0.8f;
DATA(0x004f211c) i32 iLastFrameRateTimer = 0;
DATA(0x004f22bc) i32 bSVSearchArrayInUse = 0;
DATA(0x004f2340) i32 bEvaluatingTravelGates = 1;
DATA(0x00525620) i32 gbReduceByBerserk;
DATA(0x00525624) float fBerserkFactor;
DATA(0x00525628) i32 giCurPlayer;
DATA(0x0052562c) i8 giBuildShipyard[6];
DATA(0x00525634) i32 giMaxHeroesForThisPlayer;
DATA(0x00525638) i8 giBuildBoat[6];
DATA(0x00525640) float fReduceFactor;
DATA(0x00525644) u8 giCurPlayerBit;
DATA(0x00525648) i32 giBestShipyardDist;
DATA(0x0052564c) i32 bHeroBuiltThisTurn;
DATA(0x00525650) i16 gaiHeroLiveChance[54];
DATA(0x005256bc) i32 giHumanTownConquered;
DATA(0x005256c0) i32 giCurTurn;
DATA(0x005256c8) i32 costTemp[7];
DATA(0x005256e4) i32 iAlphaMale;
DATA(0x005256e8) i32 iDummy;
DATA(0x005256ec) i32 gbPossibleShipyardFound;
DATA(0x00527c08) float gafAITurnCostResource[7];
DATA(0x00527c24) i32 iCurPlaceToVisit;
DATA(0x00527c28) i32 giBestShipyardId;
DATA(0x00527c2c) i32 gbActualBoatFound;
DATA(0x00527c30) u8 giCurWatchPlayerBit;
DATA(0x00527c34) playerData *gpCurPlayer;
DATA(0x00527c38) float gfHeroInteractionBonus[54];
DATA(0x00527d10) i32 gbBerserk;
DATA(0x00527d14) i32 giCurAIHeroMorale;
DATA(0x00527d18) i8 giBuildBoatStuffTurn[6];
DATA(0x00527d20) i32 iPlacesVisited[30][2];
DATA(0x00527e10) i32 gbReduceByReload;
DATA(0x00527e14) i32 gbTroopReload;
DATA(0x00527e18) i32 giCurAIHeroLuck;
DATA(0x00527e1c) i32 gbActualShipyardFound;
