#include <IRONFIST/combat_effects.h>
#include <IRONFIST/state.h>
#include <IRONFIST/creatures.h>
#include <BASE/Utf8.h>
#include <SOURCE/army.h>
#include <SOURCE/advManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/Localization.h>
#include <SOURCE/X_GLOBAL.h>
#include <cstring>

namespace ironfist::effects {
namespace {
class RenderExtents {
    i32 m_left = giMinExtentX, m_top = giMinExtentY;
    i32 m_right = giMaxExtentX, m_bottom = giMaxExtentY;
public:
    ~RenderExtents() {
        giMinExtentX = m_left;
        giMinExtentY = m_top;
        giMaxExtentX = m_right;
        giMaxExtentY = m_bottom;
    }
};
}

void PrepareAttackPresentation(state::CombatState& state, const DamageResult& result) {
    if (auto* target = state.Resolve(result.target)) {
        target->m_damagePending = true;
        target->m_deathPending = result.remaining <= 0;
        if (result.killed > 0) {
            target->m_killPending = true;
            target->m_lastTargetHex = result.quantityBefore;
        }
    }
    if (result.jump)
        if (auto* source = state.Resolve(result.source))
            state.StartAnimation(*source, CreatureAttribute::CREATURE_ATTRIBUTE_JUMPER);
    if (result.outcome == Outcome::DAMAGE_DODGED)
        if (auto* target = state.Resolve(result.target)) {
            target->m_damagePending = true;
            state.StartAnimation(*target, CreatureAttribute::CREATURE_ATTRIBUTE_ASTRAL_DODGE);
        }
}

void PresentShadowMark(army& target) {
    if (!gbNoShowCombat)
        target.SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_SHADOW_MARK)].combatEffect, 0, 0);
}

void PresentBurn(combatManager& battle, const DamageResult& result) {
    if (gbNoShowCombat)
        return;
    auto* target = state::Get().combat.Resolve(result.target);
    if (!target)
        return;
    const RenderExtents extents;
    PrepareAttackPresentation(state::Get().combat, result);
    target->m_animationSequence = ARMY_ANIMATION_WINCE;
    target->m_animationFrame = 0;
    target->m_deathPending = target->m_quantity <= 0;
    target->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_FIRE_BOMB)].combatEffect, 0, 0);
    target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
    target->m_deathPending = false;

    char message[GLOBAL_TEXT_BUFFER_SIZE];
    utf8::Format(message, sizeof(message), localization::Tr("combat.burning.damage"), result.damage);
    if (result.killed > 0) {
        const char* name = result.killed > 1
            ? GetCreaturePluralName(H2EnumIndex(result.creature)) : GetCreatureName(H2EnumIndex(result.creature));
        const auto used = std::strlen(message);
        utf8::Format(message + used, sizeof(message) - used,
            localization::TrPlural("combat.burning.killed", result.killed), result.killed, name);
    }
    battle.CombatMessage(message, 1, 1, 0);
}

} // namespace ironfist::effects
