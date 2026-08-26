#include <IRONFIST/hooks.h>

#include <algorithm>
#include <cstring>
#include <string>

#include <IRONFIST/artifacts.h>
#include <IRONFIST/callback.h>
#include <IRONFIST/campaigns.h>
#include <IRONFIST/creatures.h>
#include <IRONFIST/deepbinding.h>
#include <IRONFIST/expansions.h>
#include <IRONFIST/prefs.h>
#include <IRONFIST/scripting.h>
#include <IRONFIST/townconsts.h>

#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hexcell.h>
#include <BASE/Misc.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/playerData.h>

// Ironfist kept these as fields it appended to the game object; the port
// keeps the retail game layout intact and holds them here instead.
static bool gFirstDayEventDone = false;
static bool gMapEndCallbackFired = false;

// Ironfist ran this as IronfistInit before the retail main; creature data
// and town constants join it as their layers are ported.
void Ironfist_Startup(void) {
    LoadCreatures();
    LoadArtifacts();
    InitializeTownConstants();
    InitializeCampaigns();
}

void Ironfist_ResetGameState(void) {
    gFirstDayEventDone = false;
    gMapEndCallbackFired = false;
    gIronfistExtra.adventure = adventureExtra();
}

void Ironfist_NewMap(char* filename) {
    Ironfist_ResetGameState();
    std::string mapName(filename);
    ScriptingInit(mapName);
}

void Ironfist_NewMapReady(void) {
    // Upstream resets this after the retail NewMap body, so a map's top-level
    // script cannot change it. Keep that ordering while the other reset state
    // remains available to the script loaded by Ironfist_NewMap.
    gIronfistExtra.adventure.allowAIArmySharing = true;
}

void Ironfist_AdvManagerReady(void) {
    // A loaded game can't have NEWGAME as a save filename, so this tells us
    // it's a new game rather than a loaded one.
    if (!strcmp(gpGame->m_saveName, "NEWGAME") && !gFirstDayEventDone) {
        ScriptCallback("OnMapStart");
        ScriptCallback(
            "OnNewDay", static_cast<i32>(gpGame->m_month), static_cast<i32>(gpGame->m_week), static_cast<i32>(gpGame->m_day)
        );
        gFirstDayEventDone = true;
    }
}

void Ironfist_NewDay(void) {
    ScriptCallback("OnNewDay", static_cast<i32>(gpGame->m_month), static_cast<i32>(gpGame->m_week), static_cast<i32>(gpGame->m_day));
}

void Ironfist_CheckEndGame(void) {
    if (gbGameOver && !gMapEndCallbackFired) {
        gMapEndCallbackFired = true;
        if (giEndSequence) {
            ScriptCallback("OnMapVictory");
        } else {
            ScriptCallback("OnMapLoss");
        }
    }
}

void Ironfist_HeroMoved(void) {
    hero* hro = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    ScriptCallback("OnHeroMove", hro->m_x, hro->m_y);
}

b32 Ironfist_LocationVisit(mapCell* cell, i32 x, i32 y) {
    i32 locationType = H2EnumIndex(cell->m_triggerType & H2EnumIndex(MAP_TRIGGER_TYPE_MASK));
    std::optional<bool> shouldSkip =
        ScriptCallbackResult<bool>("OnLocationVisit", locationType, x, y);
    return shouldSkip.value_or(false);
}

void Ironfist_CastleConquered(i32 castleIdx, i32 playerIdx) {
    ScriptCallback("OnCastleConquered", castleIdx, playerIdx);
}

void Ironfist_ArtifactStat(hero* h, i32 artifact, b32 take) {
    if (!take)
        ScriptCallback("OnArtifactGive", deepbind<hero*>(h), artifact);
    else
        ScriptCallback("OnArtifactTake", deepbind<hero*>(h), artifact);
}

void Ironfist_TownOpened(town* t) {
    ScriptCallback("OnTownOpen", deepbind<town*>(t));
}

void Ironfist_UnitRecruit(i32 creatureType) {
    ScriptCallback("OnUnitRecruit", creatureType);
}

i32 Ironfist_CalcMobility(hero* h, i32 points) {
    auto res = ScriptCallbackResult<i32>("OnCalcMobility", deepbind<hero*>(h), points);
    if (res.has_value())
        points = std::max(1, res.value());
    return points;
}

i32 Ironfist_CalcLuck(hero* h, army* stack, town* castle, i32 luck) {
    auto res = ScriptCallbackResult<i32>(
        "OnCalcLuck", deepbind<hero*>(h), deepbind<army*>(stack), deepbind<town*>(castle),
        static_cast<i32>(luck)
    );
    if (res.has_value())
        luck = res.value();
    return std::max(-3, std::min(luck, 3));
}

i32 Ironfist_CalcMorale(hero* h, town* twn, i32 morale) {
    auto res = ScriptCallbackResult<i32>(
        "OnCalcMorale", deepbind<hero*>(h), deepbind<town*>(twn), static_cast<i32>(morale)
    );
    if (res.has_value())
        morale = res.value();
    return std::max(-3, std::min(morale, 3));
}

i32 Ironfist_CalcManaCost(hero* h, i32 spell, i32 cost) {
    auto res = ScriptCallbackResult<i32>("OnCalcManaCost", deepbind<hero*>(h), spell, static_cast<i32>(cost));
    if (res.has_value())
        cost = res.value();
    return cost;
}

float Ironfist_CalcSpellChance(army* stack, SpellType spell, float chance) {
    auto res = ScriptCallbackResult<double>(
        "OnCalcSpellChance", deepbind<army*>(stack), H2EnumIndex(spell), static_cast<double>(chance)
    );
    if (res.has_value())
        chance = static_cast<float>(res.value());
    return std::max(0.0f, std::min(chance, 1.0f));
}

void Ironfist_MeleeAttack(army* attacker, i32 retaliation) {
    army* target = attacker;
    if (H2EnumIndex(attacker->m_targetSide) >= 0 && attacker->m_targetIndex >= 0) {
        target = &gpCombatManager
                      ->m_armies[H2EnumIndex(attacker->m_targetSide)][attacker->m_targetIndex];
    }
    ScriptCallback(
        "OnBattleMeleeAttack", deepbind<army*>(attacker), deepbind<army*>(target),
        static_cast<bool>(retaliation)
    );
}

void Ironfist_MeleeAttackComplete(army* attacker, army* target) {
    ScriptCallback(
        "OnBattleMeleeAttackComplete", deepbind<army*>(attacker), deepbind<army*>(target)
    );
}

void Ironfist_MonsterInteract(mapCell* cell) {
    if (cell->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER))
        return;
    i32 index = static_cast<i32>(cell - gpGame->m_worldMap.cells);
    ScriptCallback(
        "OnMonsterInteract", index % gpGame->m_worldMap.width, index / gpGame->m_worldMap.width
    );
}

b32 Ironfist_TooltipText(mapCell* cell, i32 x, i32 y, std::string& text) {
    i32 locationType = H2EnumIndex(cell->m_triggerType & H2EnumIndex(MAP_TRIGGER_TYPE_MASK));
    auto res = ScriptCallbackResult<std::string>("GetTooltipText", locationType, x, y);
    if (!res.has_value() || res->empty())
        return false;
    text = *res;
    return true;
}

void Ironfist_AppendLuckInfo(hero* h) {
    auto res = ScriptCallbackResult<std::string>("OnShowLuckInfo", deepbind<hero*>(h));
    if (res.has_value()) {
        strcat(gText, "\n");
        strcat(gText, res->c_str());
    }
}

void Ironfist_AppendMoraleInfo(hero* h) {
    auto res = ScriptCallbackResult<std::string>("OnShowMoraleInfo", deepbind<hero*>(h));
    if (res.has_value()) {
        strcat(gText, "\n");
        strcat(gText, res->c_str());
    }
}

// A hero carrying the Pandora Box opens the battle with a random tier-one
// stack summoned beside them.
static void HandlePandoraBox(i32 side) {
    static const i32 squaresAroundCaster[2][3] = { { 14, 27, 40 }, { 11, 24, 37 } };
    if (gpCombatManager->m_heroes[side]
        && gpCombatManager->m_heroes[side]->HasArtifact(
               static_cast<ArtifactType>(ARTIFACT_PANDORA_BOX)
           )) {
        // The HoMM II code appears to lack a definition of creature tier;
        // Ironfist hardcodes the tier-one creatures.
        static const i32 creatChoices[] = {
            H2EnumIndex(CREATURE_PEASANT),  H2EnumIndex(CREATURE_SPRITE),
            H2EnumIndex(CREATURE_HALFLING), H2EnumIndex(CREATURE_GOBLIN),
            H2EnumIndex(CREATURE_SKELETON), H2EnumIndex(CREATURE_CENTAUR),
            H2EnumIndex(CREATURE_ROGUE),    CREATURE_BLOODSUCKER_ID
        };
        const i32 creat = creatChoices[SRandom(0, 7)];

        i32 hex = -1;
        const i32 poss = 3;
        const i32 tryFirst = SRandom(0, poss - 1);
        for (i32 i = 0; i < poss; i++) {
            i32 square = squaresAroundCaster[side][(i + tryFirst) % poss];
            if (H2EnumIndex((gMonsterDatabase[creat].flags.all) & (MONSTER_FLAGS_WIDE))) {
                i32 dir = side == 0 ? 1 : -1;
                if (gpCombatManager->m_hexCells[square + dir].m_occupantSide.value() != -1)
                    continue;
            }
            if (gpCombatManager->m_hexCells[square].m_occupantSide.value() == -1)
                hex = square;
        }

        if (hex == -1)
            return;

        const i32 amt = gpGame->GetRandomNumTroops(static_cast<CreatureType>(creat));
        gpCombatManager->AddArmy(
            static_cast<CombatSide>(side), static_cast<CreatureType>(creat), amt, hex,
            static_cast<MonsterFlags>(0x8000), 0
        );

        hexcell* cell = &gpCombatManager->m_hexCells[hex];
        gpCombatManager
            ->m_armies[H2EnumIndex(cell->m_occupantSide)][cell->m_occupantIndex]
            .m_temporaryResurrectionQuantity = amt;
    }
}

void Ironfist_BattleStart(void) {
    ScriptCallback("OnBattleStart");

    for (i32 side = 0; side < 2; side++) {
        HandlePandoraBox(side);
    }
}

void Ironfist_Shutdown(void) {
    UnloadCreatures();
    ScriptingShutdown();
}

static void PropagateVision() {
    for (i32 p1 = 0; p1 < H2EnumIndex(GAME_PLAYER_COUNT); p1++) {
        for (i32 p2 = 0; p2 < H2EnumIndex(GAME_PLAYER_COUNT); p2++) {
            if (!gIronfistExtra.adventure.sharePlayerVision[p1][p2]) {
                continue;
            }
            for (i32 cell = 0; cell < MAP_WIDTH * MAP_HEIGHT; cell++) {
                if (mapExtra[cell] & (1 << p1)) {
                    mapExtra[cell] |= static_cast<u8>(1 << p2);
                }
            }
        }
    }
}

void Ironfist_ShareVision(i32 sourcePlayer, i32 destPlayer) {
    gIronfistExtra.adventure.sharePlayerVision[sourcePlayer][destPlayer] = true;
    PropagateVision();
}

void Ironfist_CancelShareVision(i32 sourcePlayer, i32 destPlayer) {
    gIronfistExtra.adventure.sharePlayerVision[sourcePlayer][destPlayer] = false;
}

void Ironfist_SetVisibilityShared(i32 x, i32 y, i32 player, i32 radius) {
    // Not transitive, like Ironfist; the guard stops the hook from recursing
    // through the game function it re-enters.
    static bool propagating = false;
    if (propagating)
        return;
    propagating = true;
    for (i32 i = 0; i < H2EnumIndex(GAME_PLAYER_COUNT); i++) {
        if (gIronfistExtra.adventure.sharePlayerVision[player][i]) {
            gpGame->SetVisibility(x, y, i, radius);
        }
    }
    propagating = false;
}

void Ironfist_AllWaterVisibleShared(i32 player) {
    static bool propagating = false;
    if (propagating)
        return;
    propagating = true;
    for (i32 i = 0; i < H2EnumIndex(GAME_PLAYER_COUNT); i++) {
        if (gIronfistExtra.adventure.sharePlayerVision[player][i]) {
            gpGame->MakeAllWaterVisible(i);
        }
    }
    propagating = false;
}

void Ironfist_DisallowBuilding(i32 townIdx, i32 building) {
    if (building >= 0 && building < 32 && townIdx >= 0
        && townIdx < H2EnumIndex(GAME_TOWN_COUNT)) {
        gIronfistExtra.adventure.disallowedBuildings[townIdx][building] = true;
    }
}

b32 Ironfist_BuildingDisallowed(town* t, i32 building) {
    if (building < 0 || building >= 32) {
        return false;
    }
    return gIronfistExtra.adventure.disallowedBuildings[t->m_id][building];
}

void Ironfist_SetAIArmySharing(b32 allow) {
    gIronfistExtra.adventure.allowAIArmySharing = allow;
}

b32 Ironfist_AIArmySharingAllowed(void) {
    return gIronfistExtra.adventure.allowAIArmySharing;
}

void Ironfist_ForceChase(i32 sourceHeroId, i32 destHeroId, b32 force) {
    gIronfistExtra.adventure.forcedComputerPlayerChases[sourceHeroId][destHeroId] = force;
}

// With the Well game modification disabled, undo the +2 weekly growth the
// Well granted during PerWeek (+1 for unowned towns, which grow at half
// rate). Ironfist building codes: the Well is bit 4, the dwellings and their
// upgrades are bits 0x13-0x1e.
void Ironfist_UndoWellGrowth(void) {
    if (!IsWellDisabled()) {
        return;
    }

    for (i32 i = 0; i < H2EnumIndex(GAME_TOWN_COUNT); ++i) {
        town& townObj = gpGame->m_castleRecs[i];
        for (i32 d = 0x13; d <= 0x1e; ++d) {
            if (!(townObj.m_buildings & (1u << 4)) || !(townObj.m_buildings & (1u << d))) {
                continue;
            }

            const i32 dwellingIdx = d - 0x13;
            if (townObj.m_owner >= 0) {
                townObj.m_garrison[dwellingIdx] -= 2;
            } else {
                townObj.m_garrison[dwellingIdx] -= 1;
            }
        }
    }
}

// Because heroes no longer regain movement on hire, every hero in the pool
// regains movement between turns instead.
void Ironfist_HeroPoolRegainMobility(void) {
    for (i32 i = 0; i < H2EnumIndex(GAME_HERO_COUNT); i++) {
        hero* h = &gpGame->m_heroRecs[i];
        h->m_mobility = h->CalcMobility();
        h->m_remainingMobility = h->m_mobility;
    }
}

b32 Ironfist_ForcedChaseValue(i32 heroId, i32* value) {
    hero* source = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    if (gIronfistExtra.adventure.forcedComputerPlayerChases[source->m_id][heroId]) {
        *value = 32000;
        return true;
    }
    return false;
}
