#ifndef HOMM2_IRONFIST_COMBAT_EFFECTS_H
#define HOMM2_IRONFIST_COMBAT_EFFECTS_H

#include <IRONFIST/combat_state.h>

namespace ironfist::effects {

enum class Outcome { DAMAGE_HIT, DAMAGE_MIRROR_DESTROYED, DAMAGE_DODGED };
struct AttackContext {
    state::AttackApproach approach;
    bool ranged = false;
    bool retaliation = false;
    bool chargePath = false;
};

// Owned event data: no native pointers or borrowed text. Resolution applies the
// health, shield and ability changes before presentation observes this value.
struct DamageResult {
    state::StackIdentity source;
    state::StackIdentity target;
    CreatureType creature = CREATURE_NONE;
    Outcome outcome = Outcome::DAMAGE_HIT;
    i32 damage = 0;
    i32 absorbed = 0;
    i32 killed = 0;
    i32 quantityBefore = 0;
    i32 remaining = 0;
    bool jump = false;
    bool genieHalf = false;
};

DamageResult ResolveAttack(combatManager& battle, state::CombatState& state,
                           army& attacker, army& target, AttackContext context);
DamageResult ResolveBurn(combatManager& battle, state::CombatState& state, army& target);
// Shared health/spell rules; the retail Damage entry point adapts its result to
// the existing animation pipeline, while new resolvers need no renderer.
i32 ApplyDamage(combatManager& battle, army& target, i32l damage, SpellType spell);
bool ResolveShadowMark(army& target);
void PrepareAttackPresentation(state::CombatState& state, const DamageResult& result);
void PresentBurn(combatManager& battle, const DamageResult& result);
void PresentShadowMark(army& target);

} // namespace ironfist::effects
#endif
