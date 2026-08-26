#include <IRONFIST/hooks.h>

#include <algorithm>
#include <cstring>
#include <optional>
#include <string>

#include <IRONFIST/callback.h>
#include <IRONFIST/deepbinding.h>
#include <IRONFIST/state.h>

#include <BASE/Misc.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/playerData.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist::hooks {

void NewDay() {
    script::Invoke(
        "OnNewDay", static_cast<i32>(gpGame->m_month), static_cast<i32>(gpGame->m_week),
        static_cast<i32>(gpGame->m_day)
    );
}

void CheckEndGame() {
    if (gbGameOver && !state::Get().adventure.mapEndCallbackFired) {
        state::Get().adventure.mapEndCallbackFired = true;
        script::Invoke(giEndSequence ? "OnMapVictory" : "OnMapLoss");
    }
}

void HeroMoved() {
    hero* currentHero = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    script::Invoke("OnHeroMove", currentHero->m_x, currentHero->m_y);
}

b32 LocationVisit(mapCell* cell, i32 x, i32 y) {
    const i32 locationType =
        H2EnumIndex(cell->m_triggerType & H2EnumIndex(MAP_TRIGGER_TYPE_MASK));
    return script::InvokeResult<bool>("OnLocationVisit", locationType, x, y).value_or(false);
}

void CastleConquered(i32 castleIdx, i32 playerIdx) {
    script::Invoke("OnCastleConquered", castleIdx, playerIdx);
}

void ArtifactChanged(hero* h, i32 artifact, b32 take) {
    if (take) {
        script::Invoke("OnArtifactTake", script::Binding<hero*>(h), artifact);
    } else {
        script::Invoke("OnArtifactGive", script::Binding<hero*>(h), artifact);
    }
}

void TownOpened(town* t) {
    script::Invoke("OnTownOpen", script::Binding<town*>(t));
}

void UnitRecruited(i32 creatureType) {
    script::Invoke("OnUnitRecruit", creatureType);
}

i32 ModifyMobility(hero* h, i32 points) {
    const auto result = script::InvokeResult<i32>("OnCalcMobility", script::Binding<hero*>(h), points);
    return result.has_value() ? std::max(1, *result) : points;
}

i32 ModifyLuck(hero* h, army* stack, town* castle, i32 luck) {
    const auto result = script::InvokeResult<i32>(
        "OnCalcLuck", script::Binding<hero*>(h), script::Binding<army*>(stack), script::Binding<town*>(castle), luck
    );
    return std::clamp(result.value_or(luck), -3, 3);
}

i32 ModifyMorale(hero* h, town* currentTown, i32 morale) {
    const auto result = script::InvokeResult<i32>(
        "OnCalcMorale", script::Binding<hero*>(h), script::Binding<town*>(currentTown), morale
    );
    return std::clamp(result.value_or(morale), -3, 3);
}

i32 ModifyManaCost(hero* h, i32 spell, i32 cost) {
    return script::InvokeResult<i32>(
               "OnCalcManaCost", script::Binding<hero*>(h), spell, cost
    ).value_or(cost);
}

float ModifySpellChance(army* stack, SpellType spell, float chance) {
    const auto result = script::InvokeResult<double>(
        "OnCalcSpellChance", script::Binding<army*>(stack), H2EnumIndex(spell),
        static_cast<double>(chance)
    );
    const float modifiedChance = static_cast<float>(result.value_or(chance));
    return std::max(0.0f, std::min(modifiedChance, 1.0f));
}

void MeleeAttackStarted(army* attacker, i32 retaliation) {
    army* target = attacker;
    if (H2EnumIndex(attacker->m_targetSide) >= 0 && attacker->m_targetIndex >= 0) {
        target = &gpCombatManager
                      ->m_armies[H2EnumIndex(attacker->m_targetSide)][attacker->m_targetIndex];
    }
    script::Invoke(
        "OnBattleMeleeAttack", script::Binding<army*>(attacker), script::Binding<army*>(target),
        static_cast<bool>(retaliation)
    );
}

void MeleeAttackCompleted(army* attacker, army* target) {
    script::Invoke(
        "OnBattleMeleeAttackComplete", script::Binding<army*>(attacker), script::Binding<army*>(target)
    );
}

void MonsterInteraction(mapCell* cell) {
    if (cell->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
        return;
    }
    const i32 index = static_cast<i32>(cell - gpGame->m_worldMap.cells);
    script::Invoke(
        "OnMonsterInteract", index % gpGame->m_worldMap.width, index / gpGame->m_worldMap.width
    );
}

b32 OverrideTooltip(mapCell* cell, i32 x, i32 y, std::string& text) {
    const i32 locationType =
        H2EnumIndex(cell->m_triggerType & H2EnumIndex(MAP_TRIGGER_TYPE_MASK));
    const auto result = script::InvokeResult<std::string>("GetTooltipText", locationType, x, y);
    if (!result.has_value() || result->empty()) {
        return false;
    }
    text = *result;
    return true;
}

void AppendLuckInfo(hero* h) {
    const auto result = script::InvokeResult<std::string>("OnShowLuckInfo", script::Binding<hero*>(h));
    if (result.has_value()) {
        std::strcat(gText, "\n");
        std::strcat(gText, result->c_str());
    }
}

void AppendMoraleInfo(hero* h) {
    const auto result = script::InvokeResult<std::string>("OnShowMoraleInfo", script::Binding<hero*>(h));
    if (result.has_value()) {
        std::strcat(gText, "\n");
        std::strcat(gText, result->c_str());
    }
}

void BattleStarted() {
    script::Invoke("OnBattleStart");
}

} // namespace ironfist::hooks
