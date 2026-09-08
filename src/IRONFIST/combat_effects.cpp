#include <IRONFIST/combat_effects.h>
#include <BASE/Misc.h>
#include <SOURCE/army.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist::effects {
namespace {
constexpr i32 GENIE_QUANTITY_DIVISOR = 2;
#define DAMAGE_DOUBLE_MULTIPLIER 2.0f
#define DAMAGE_HALF_DIVISOR 2.0f
#define DAMAGE_ROUNDING_OFFSET 0.5
#define DAMAGE_SHADOW_MARK_MULTIPLIER 1.5f
#define DAMAGE_CHARGE_PATH_MULTIPLIER 0.5f
#define DAMAGE_CHARGE_MULTIPLIER 1.25f
#define DAMAGE_TELEPORT_MULTIPLIER 1.25f

}

i32 ApplyDamage(combatManager& battle, army& target, i32l damage, SpellType spell) {
    damage += target.m_hitPointsLost;
    if (spell != SPELL_NONE) {
        if (gbRemoteOn)
            battle.ModifyDamageForArtifacts(&damage, spell,
                battle.m_heroes[H2EnumIndex(target.m_side)], battle.m_heroes[H2EnumIndex(battle.m_currentSide)]);
        else
            battle.ModifyDamageForArtifacts(&damage, spell,
                battle.m_heroes[H2EnumIndex(battle.m_currentSide)], battle.m_heroes[H2EnumIndex(target.m_side)]);
    }
    i32 killed = damage / target.m_monster.hitPoints;
    target.m_hitPointsLost = damage % target.m_monster.hitPoints;
    if (H2EnumIndex(target.m_monster.flags.all & MONSTER_FLAGS_MIRROR_IMAGE)) {
        killed = target.m_quantity;
        target.m_hitPointsLost = 0;
    }
    if (killed > target.m_quantity)
        killed = target.m_quantity;
    target.m_quantity -= killed;
    target.CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_DAMAGE);
    return killed;
}

DamageResult ResolveAttack(combatManager& battle, state::CombatState& state,
                           army& attacker, army& victim, AttackContext context) {
    DamageResult result;
    result.source = state.Identity(attacker);
    result.target = state.Identity(victim);
    result.creature = victim.m_monsterType;
    result.quantityBefore = result.remaining = victim.m_quantity;
    if (attacker.m_quantity <= 0 || victim.m_quantity <= 0)
        return result;
    army* target = &victim;
    const bool rangedAttack = context.ranged;
    const bool retaliation = context.retaliation;
    float damage;
    i32 attack;
    i32 defense;
    i32 difference;
    i32 rearHex;
    i32 index;
    i32 appliedDamage;
    i32 genieDamage;
    hero* commander;

    damage = 0;

    for (index = 0; index < attacker.m_quantity; index++) {
        if (attacker.m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLESS)]) {
            damage += attacker.m_monster.damageMax;
        } else if (attacker.m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_CURSE)]) {
            damage += attacker.m_monster.damageMin;
        } else {
            damage += SRandom(attacker.m_monster.damageMin, attacker.m_monster.damageMax);
        }
    }
    // Dazed or burning creatures fight at half their skill.
    attack = attacker.m_monster.attack;
    if (attacker.m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_DAZE)]
        || attacker.m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BURN)]) {
        attack /= 2;
    }
    defense = target->m_monster.defense;
    if (target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_DAZE)]
        || target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BURN)]) {
        defense /= 2;
    }
    difference = attack - defense;
    if (attacker.m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_DRAGON_SLAYER)]
        && (target->m_monsterType == CREATURE_GREEN_DRAGON
            || target->m_monsterType == CREATURE_RED_DRAGON
            || target->m_monsterType == CREATURE_BLACK_DRAGON
            || target->m_monsterType == CREATURE_BONE_DRAGON)) {
        difference += ARMY_DRAGON_SLAYER_BONUS;
    }
    if (battle.m_drawbridgeBackgroundVisible) {
        rearHex = -1;
        if ((H2EnumIndex((target->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
            rearHex = ArmyFacingRearHexOffset(target->m_facing) + target->m_hex;
        }
        for (index = 0; index < ARMY_MOAT_CELL_COUNT; index++) {
            if (moatCell[index] == target->m_hex || moatCell[index] == rearHex) {
                difference += ARMY_MOAT_ATTACK_BONUS;
            }
        }
    }
    if (difference > ARMY_DAMAGE_STAT_LIMIT) {
        difference = ARMY_DAMAGE_STAT_LIMIT;
    }
    if (difference < -ARMY_DAMAGE_STAT_LIMIT) {
        difference = -ARMY_DAMAGE_STAT_LIMIT;
    }
    damage *= gfBattleStat[difference + ARMY_DAMAGE_STAT_LIMIT];
    if ((attacker.m_monsterType == CREATURE_CRUSADER
         && (H2EnumIndex((target->m_monster.flags.all) & (MONSTER_FLAGS_UNDEAD))))
        || (attacker.m_monsterType == CREATURE_EARTH_ELEMENTAL
            && target->m_monsterType == CREATURE_AIR_ELEMENTAL)
        || (attacker.m_monsterType == CREATURE_AIR_ELEMENTAL
            && target->m_monsterType == CREATURE_EARTH_ELEMENTAL)
        || (attacker.m_monsterType == CREATURE_WATER_ELEMENTAL
            && target->m_monsterType == CREATURE_FIRE_ELEMENTAL)
        || (attacker.m_monsterType == CREATURE_FIRE_ELEMENTAL
            && target->m_monsterType == CREATURE_WATER_ELEMENTAL)) {
        damage *= DAMAGE_DOUBLE_MULTIPLIER;
    }
    if (attacker.m_luckOutcome > 0) {
        damage *= DAMAGE_DOUBLE_MULTIPLIER;
    }
    if (attacker.m_luckOutcome < 0) {
        damage /= DAMAGE_HALF_DIVISOR;
    }
    attacker.m_luckOutcome = 0;
    if (rangedAttack && battle.ShotIsThroughWall(attacker.m_side, attacker.m_hex, target->m_hex)) {
        damage /= DAMAGE_HALF_DIVISOR;
    }
    commander = battle.m_heroes[H2EnumIndex(attacker.m_side)];
    if (commander && rangedAttack) {
        damage *=
            gfSSArcheryMod[H2EnumIndex(commander->m_secondarySkills[H2EnumIndex(HERO_SKILL_ARCHERY)])];
    }
    if ((H2EnumIndex((attacker.m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER))) && !rangedAttack
        && attacker.m_monsterType != CREATURE_TITAN && attacker.m_monsterType != CREATURE_MAGE
        && attacker.m_monsterType != CREATURE_ARCHMAGE
        && attacker.m_monsterType != CREATURE_CYBER_BEHEMOTH) {
        damage /= DAMAGE_HALF_DIVISOR;
    }
    if (rangedAttack && target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SHIELD)]) {
        damage /= DAMAGE_HALF_DIVISOR;
    }
    if (attacker.m_damagePenalty == ARMY_DAMAGE_PENALTY_HALF) {
        damage /= DAMAGE_HALF_DIVISOR;
    }
    attacker.m_damagePenalty = ARMY_DAMAGE_PENALTY_NONE;
    if (target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PETRIFIED)]) {
        damage /= DAMAGE_HALF_DIVISOR;
    }
    if (target->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_SHADOW_MARK)]) {
        damage *= DAMAGE_SHADOW_MARK_MULTIPLIER;
    }
    // A jumper strikes harder mid-jump, once per battle.
    if (state.HasAbility(attacker, CreatureAttribute::Jumper) && !retaliation
        && state.HasAbilityCharge(attacker, ironfist::CreatureAttribute::Jumper)
        && context.approach.jump) {
        state.ConsumeAbility(attacker, ironfist::CreatureAttribute::Jumper);
        result.jump = true;
        damage *= SRandom(125, 150) * 0.01f;
    }
    // A charger's hit softens along its path and lands harder at the end.
    if (state.HasAbility(attacker, CreatureAttribute::Charger) && context.approach.charge && !retaliation) {
        if (context.chargePath) {
            damage *= DAMAGE_CHARGE_PATH_MULTIPLIER;
        } else {
            damage *= DAMAGE_CHARGE_MULTIPLIER;
        }
    }
    // Teleporting into an enemy from afar hits harder.
    if (context.approach.distantTeleport && !retaliation && state.HasAbility(attacker, CreatureAttribute::Teleporter)) {
        damage *= DAMAGE_TELEPORT_MULTIPLIER;
    }
    appliedDamage = static_cast<i32>(damage + DAMAGE_ROUNDING_OFFSET);
    if (attacker.m_monsterType == CREATURE_GENIE
        && SRandom(1, ARMY_GENIE_HALF_ROLL_MAX) == ARMY_GENIE_HALF_ROLL) {
        genieDamage =
            ((target->m_quantity + 1) / GENIE_QUANTITY_DIVISOR) * target->m_monster.hitPoints;
        if (appliedDamage < genieDamage) {
            result.genieHalf = true;
            appliedDamage = genieDamage;
        }
    }
    if (appliedDamage <= 0) {
        appliedDamage = 1;
    }
    if ((H2EnumIndex((target->m_monster.flags.all) & (MONSTER_FLAGS_MIRROR_IMAGE)))) {
        appliedDamage = -1;
    }
    // An astral dodger slips one melee blow per round; -2 marks the dodge.
    if (!rangedAttack && !retaliation
        && state.HasAbility(*target, CreatureAttribute::AstralDodge)
        && state.HasAbilityCharge(*target, ironfist::CreatureAttribute::AstralDodge)) {
        state.ConsumeAbility(*target, ironfist::CreatureAttribute::AstralDodge);
        appliedDamage = -2;
    }
    result.damage = appliedDamage < 0 ? 0 : appliedDamage;
    if (appliedDamage == -2) {
        result.outcome = Outcome::Dodged;
        return result;
    }
    if (appliedDamage == -1) {
        result.outcome = Outcome::MirrorDestroyed;
        result.killed = ApplyDamage(battle, *target, 0, SPELL_NONE);
        result.remaining = target->m_quantity;
        return result;
    }
    const i32 shieldPoints = state.ShieldHP(*target);
    appliedDamage = state.AbsorbDamage(*target, appliedDamage);
    result.absorbed = result.damage - appliedDamage;
    if (shieldPoints > 0 && state.ShieldHP(*target) == 0)
        target->CancelIndividualSpell(ARMY_SPELL_INFLUENCE_FORCE_SHIELD);
    result.killed = ApplyDamage(battle, *target, appliedDamage, SPELL_NONE);
    result.remaining = target->m_quantity;
    return result;
}

DamageResult ResolveBurn(combatManager& battle, state::CombatState& state, army& target) {
    DamageResult result;
    result.target = state.Identity(target);
    result.creature = target.m_monsterType;
    result.quantityBefore = result.remaining = target.m_quantity;
    if (target.m_quantity <= 0)
        return result;
    result.damage = COMBAT_BURN_BASE_DAMAGE + SRandom(0, COMBAT_BURN_RANDOM_DAMAGE);
    result.killed = ApplyDamage(battle, target, result.damage, SPELL_FIRE_BOMB);
    // Death occupancy is committed even if this event is never presented.
    result.remaining = target.m_quantity;
    if (target.m_quantity <= 0) {
        target.ProcessDeath(0);
        for (auto& side : battle.m_armies)
            for (auto& stack : side)
                if (stack.m_quantity <= 0 && stack.m_monsterType != CREATURE_NONE
                    && H2EnumIndex(stack.m_monster.flags.all & MONSTER_FLAGS_DEAD)) {
                    for (auto& cell : battle.m_hexCells)
                        if (cell.m_occupantSide == stack.m_side && cell.m_occupantIndex == stack.m_index) {
                            cell.m_occupantSide = COMBAT_SIDE_NONE;
                            cell.m_occupantIndex = -1;
                        }
                }
    }
    return result;
}

bool ResolveShadowMark(army& target) {
    if (target.m_quantity <= 0 || !target.SpellCastWorks(SPELL_SHADOW_MARK))
        return false;
    target.SetSpellInfluence(ARMY_SPELL_INFLUENCE_SHADOW_MARK, 1);
    return true;
}

} // namespace ironfist::effects
