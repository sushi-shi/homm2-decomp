#include <BASE/heroWindowManager.h>
#include <IRONFIST/state.h>
#include <IRONFIST/combat_movement.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

#include <cassert>
#include <array>
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

static void CheckMovement(combatManager& board, army& actor, army& enemy) {
    using namespace ironfist::movement;
    auto& extensions = ironfist::state::Get().combat;
    auto clearBoard = [&] {
        std::memset(board.m_hexCells, 0, sizeof(board.m_hexCells));
        for (auto& cell : board.m_hexCells) {
            cell.m_occupantSide = COMBAT_SIDE_NONE;
            cell.m_occupantIndex = -1;
        }
        board.m_inCastleCombat = false;
        board.m_drawbridgeBackgroundVisible = false;
        board.SetupAdjacencyArray();
    };
    clearBoard();
    actor.m_hex = 14;
    actor.m_facing = ARMY_FACING_RIGHT;
    actor.m_monster.speed = actor.m_speed = 4;
    board.m_hexCells[14].m_occupantSide = actor.m_side;
    board.m_hexCells[14].m_occupantIndex = actor.m_index;
    board.m_hexCells[15].m_blocked = board.m_hexCells[16].m_blocked = true;
    extensions.GrantAbility(actor, ironfist::CreatureAttribute::Jumper);
    std::array<hexcell, COMBAT_HEX_COUNT> before;
    std::memcpy(before.data(), board.m_hexCells, sizeof(board.m_hexCells));
    Traversal jumper(board, actor, extensions);
    auto plan = jumper.Find(14, Target{17}, 4);
    assert(plan && plan.steps.size() == 1 && plan.steps[0].kind == StepKind::Jump);
    assert(!jumper.Find(14, Target{15}, 4)); // Crossing an obstacle does not permit landing on it.
    gbHumanPlayer[0] = false;
    assert(jumper.Find(14, Target{17}, 4).steps[0].to == 17);
    gbHumanPlayer[0] = true;
    assert(jumper.Find(14, Target{18}, 4));
    assert(plan.destination == 17 && plan.steps[0].cost == 3);
    assert(std::memcmp(before.data(), board.m_hexCells, sizeof(board.m_hexCells)) == 0);
    assert(actor.m_hex == 14 && actor.m_monster.speed == 4);
    assert(plan.WithinBudget(2).steps.empty());
    gbNoShowCombat = true;
    assert(Execute(actor, plan));
    assert(actor.m_hex == 17 && board.m_hexCells[14].m_occupantSide == COMBAT_SIDE_NONE);
    assert(board.m_hexCells[17].m_occupantIndex == actor.m_index);

    clearBoard();
    actor.m_hex = 8;
    board.m_hexCells[9].m_blocked = true;
    board.m_inCastleCombat = true;
    assert(!Traversal(board, actor, extensions).Find(8, Target{10}, 2));
    board.m_inCastleCombat = false;
    assert(Traversal(board, actor, extensions).Find(8, Target{10}, 2));
    clearBoard();
    board.m_inCastleCombat = true;
    board.m_drawbridgeState = COMBAT_DRAWBRIDGE_RAISED;
    board.m_hexCells[H2EnumIndex(COMBAT_CASTLE_HEX_GATE)].m_blocked = true;
    assert(!Traversal(board, actor, extensions).CanLand(H2EnumIndex(COMBAT_CASTLE_HEX_GATE)));
    actor.m_side = COMBAT_DEFENDER_SIDE;
    assert(Traversal(board, actor, extensions).CanLand(H2EnumIndex(COMBAT_CASTLE_HEX_GATE)));
    actor.m_side = COMBAT_ATTACKER_SIDE;
    clearBoard();
    actor.m_monster.attributes |= MONSTER_ATTRIBUTE_WIDE;
    board.m_hexCells[17].m_blocked = true;
    assert(!Traversal(board, actor, extensions).CanLand(16));
    actor.m_monster.attributes &= ~MONSTER_ATTRIBUTE_WIDE;
    actor.m_hex = 14;
    board.m_drawbridgeBackgroundVisible = true;
    assert(Traversal(board, actor, extensions).StopsMovement(moatCell[0]));

    clearBoard();
    extensions.ResetStack(actor);
    extensions.GrantAbility(actor, ironfist::CreatureAttribute::Charger);
    actor.m_hex = 14;
    enemy.m_hex = 18;
    board.m_hexCells[18].m_occupantSide = enemy.m_side;
    board.m_hexCells[18].m_occupantIndex = enemy.m_index;
    Traversal charger(board, actor, extensions);
    auto target = charger.Destination(18);
    auto charge = charger.Find(14, target, 4);
    assert(charge && charge.steps.size() == 1 && charge.steps[0].kind == StepKind::Charge);
    assert(charge.steps[0].cost == 4 && charge.WithinBudget(3).steps.empty());
    assert(charger.StraightDirection(14, 18) == COMBAT_DIRECTION_EAST);
    board.m_hexCells[16].m_blocked = true;
    auto detour = charger.Find(14, target, 4);
    for (const auto& step : detour.steps)
        assert(step.kind != StepKind::Charge);
    clearBoard();
    InitStack(actor, COMBAT_ATTACKER_SIDE, 0);
    InitStack(enemy, COMBAT_DEFENDER_SIDE, 0);
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
    CheckMovement(combat, target, enemy);
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
