#include <BASE/heroWindowManager.h>
#include <IRONFIST/state.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

#include <cassert>
#include <cstring>

static void InitStack(army& stack, CombatSide side, i32 index) {
    ironfist::state::Get().combat.ResetStack(stack);
    stack.m_monsterType = CREATURE_PEASANT;
    stack.m_spellCount = 0;
    std::memset(stack.m_spellInfluence, 0, sizeof(stack.m_spellInfluence));
    stack.m_monster = {};
    stack.m_monster.hitPoints = 100;
    stack.m_side = side;
    stack.m_index = index;
    stack.m_quantity = stack.m_initialQuantity = 1;
}

int main() {
    gpCombatManager = new combatManager;
    gpWindowManager = new heroWindowManager;
    auto& combat = *gpCombatManager;
    auto& extensions = ironfist::state::Get().combat;
    extensions.BeginBattle(combat);
    combat.m_currentSide = COMBAT_ATTACKER_SIDE;
    combat.m_heroes[0] = combat.m_heroes[1] = nullptr;
    combat.m_armyCount[0] = 2;
    combat.m_armyCount[1] = 1;
    gMonsterDatabase[H2EnumIndex(CREATURE_PEASANT)].hitPoints = 100;
    auto& target = combat.m_armies[0][0];
    auto& ally = combat.m_armies[0][1];
    auto& enemy = combat.m_armies[1][0];
    InitStack(target, COMBAT_ATTACKER_SIDE, 0);
    InitStack(ally, COMBAT_ATTACKER_SIDE, 1);
    InitStack(enemy, COMBAT_DEFENDER_SIDE, 0);
    combat.m_hexCells[20].m_occupantSide = COMBAT_ATTACKER_SIDE;
    combat.m_hexCells[20].m_occupantIndex = 0;
    constexpr auto shield = ARMY_SPELL_INFLUENCE_FORCE_SHIELD;
    assert(target.SetSpellInfluence(shield, 3) == 1);
    assert(extensions.ShieldHP(target) == 100 && target.m_spellCount == 1);
    assert(extensions.AbsorbDamage(target, 75) == 0);
    assert(combat.ValidSpellTarget(SPELL_FORCE_SHIELD, 20));
    assert(combat.ValidSpellTarget(SPELL_MASS_FORCE_SHIELD, 20));
    assert(target.SetSpellInfluence(shield, 7) == 1);
    assert(extensions.ShieldHP(target) == 100 && target.m_spellCount == 1);
    assert(target.m_spellInfluence[H2EnumIndex(shield)] == 7);
    assert(!combat.ValidSpellTarget(SPELL_FORCE_SHIELD, 20));
    assert(!combat.ValidSpellTarget(SPELL_MASS_FORCE_SHIELD, 20));
    assert(target.SetSpellInfluence(shield, 2) == 0);
    assert(target.m_spellCount == 1);
    target.DecrementSpellRounds();
    assert(extensions.ShieldHP(target) == 100 && target.m_spellInfluence[H2EnumIndex(shield)] == 7);

    // Exercise the real mass-cast path on both depleted and unshielded allies.
    hero caster;
    caster.m_isCaptain = false;
    std::strcpy(caster.m_name, "Caster");
    for (auto& artifact : caster.m_artifacts)
        artifact = ARTIFACT_NONE;
    combat.m_heroes[0] = &caster;
    gbNoShowCombat = true;
    assert(extensions.AbsorbDamage(target, 75) == 0);
    combat.CastMassSpell(SPELL_MASS_FORCE_SHIELD, 3);
    assert(extensions.ShieldHP(target) == 100 && target.m_spellCount == 1);
    assert(extensions.ShieldHP(ally) == 100 && ally.m_spellCount == 1);
    assert(extensions.ShieldHP(enemy) == 0 && enemy.m_spellCount == 0);
    target.CancelIndividualSpell(shield);
    assert(extensions.ShieldHP(target) == 0 && target.m_spellCount == 0);
    assert(target.SetSpellInfluence(shield, 3) == 1);
    assert(extensions.ShieldHP(target) == 100 && target.m_spellCount == 1);

    // Ordinary influences still extend duration without applying a bonus twice.
    target.m_monster.attack = 5;
    assert(target.SetSpellInfluence(ARMY_SPELL_INFLUENCE_BLOODLUST, 3) == 1);
    const auto attack = target.m_monster.attack;
    assert(target.SetSpellInfluence(ARMY_SPELL_INFLUENCE_BLOODLUST, 5) == 0);
    assert(target.m_monster.attack == attack && target.m_spellCount == 2);
    const auto oldTarget = extensions.Identity(target);
    extensions.GrantAbility(target, ironfist::CreatureAttribute::Jumper);
    extensions.StartAnimation(target, ironfist::CreatureAttribute::Jumper);
    target.InitClean();
    assert(extensions.Resolve(oldTarget) == nullptr);
    InitStack(target, COMBAT_ATTACKER_SIDE, 0);
    assert(extensions.Resolve(oldTarget) == nullptr);
    assert(extensions.ShieldHP(target) == 0);
    assert(!extensions.HasAbilityCharge(target, ironfist::CreatureAttribute::Jumper));
    assert(!extensions.IsAnimating(target, ironfist::CreatureAttribute::Jumper));
    const auto newTarget = extensions.Identity(target);
    assert(extensions.Resolve(newTarget) == &target);
    extensions.EndBattle();
    assert(extensions.Resolve(newTarget) == nullptr);
    extensions.BeginBattle(combat);
    InitStack(target, COMBAT_ATTACKER_SIDE, 0);
    assert(extensions.Resolve(newTarget) == nullptr);
    extensions.EndBattle();
    delete gpWindowManager;
    delete gpCombatManager;
}
