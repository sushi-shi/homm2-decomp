#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <cstdlib>
#include <cstdio>

// Exercise the production scorer with deterministic combat/environment queries.
static float chance = 1.0f;
static bool adjacentDragon = false;
float army::SpellCastWorkChance(SpellType) { return chance; }
i32 army::GetAttackMask(i32, ArmyAttackTarget, i32) { return 255; }
i32 army::OtherArmyAdjacent(CombatSide, i32) { return adjacentDragon; }
army::army() {}
hero::hero() {}
armyGroup::armyGroup() {}
hexcell::hexcell() {}
baseManager::baseManager() {}
combatManager::combatManager() {}
i32 combatManager::Open(i32) { return 0; }
void combatManager::Close() {}
MessageDispatchResult combatManager::Main(tag_message&) { return MessageDispatchResult(0); }
i32 hero::HasArtifact(ArtifactType) { return 0; }
u8 giSpellInfluenceToSpell[KB_SPELL_INFLUENCE_MAP_COUNT] = {};
tag_monsterInfo gMonsterDatabase[KB_CREATURE_TABLE_CAPACITY] = {};
b8 gArmyEffected[COMBAT_SIDE_COUNT][KB_ARMY_EFFECT_COUNT] = {};
SSpellInfo gsSpellInfo[KB_SPELL_TABLE_CAPACITY] = {};
// These belong to unrelated spell paths; fail if a tested spell reaches them.
i32 combatManager::SpaceForElementalExists() { std::abort(); }
i32 combatManager::FindResurrectArmyIndex(CombatSide, SpellType, i32) { std::abort(); }
void combatManager::ModifyDamageForArtifacts(i32l*, SpellType, hero*, hero*) { std::abort(); }
i32 combatManager::GetNextChainLightningTarget(army*, i32) { std::abort(); }
i32 GetAdjacentCellIndexNoArmy(i32, CombatHexDirection) { std::abort(); }

static void check(bool condition, const char* message) {
    if (!condition) {
        std::fprintf(stderr, "%s\n", message);
        std::exit(1);
    }
}

int main() {
    static combatManager combat;
    static hero caster;
    combat.m_currentSide = COMBAT_ATTACKER_SIDE;
    combat.m_heroes[0] = &caster;
    combat.m_spellPower[0] = 3;
    combat.m_armyCount[0] = 1;
    combat.m_armyCount[1] = 2;
    for (auto& cell : combat.m_hexCells) {
        cell.m_occupantSide = CombatSide(-1);
        cell.m_occupantIndex = -1;
    }
    for (int side = 0; side < 2; ++side) {
        for (int i = 0; i < 2; ++i) {
            auto& stack = combat.m_armies[side][i];
            stack.m_side = CombatSide(side);
            stack.m_index = i;
            stack.m_quantity = 10;
            stack.m_monster.fightValue = 100;
            stack.m_monster.speed = 4;
            stack.m_monster.damageMin = 2;
            stack.m_monster.damageMax = 6;
            stack.m_hex = side ? 10 + 13 * i : 2;
        }
    }
    auto& friendly = combat.m_armies[0][0];
    auto& enemy = combat.m_armies[1][0];
    combat.m_hexCells[10].m_occupantSide = COMBAT_DEFENDER_SIDE;
    combat.m_hexCells[10].m_occupantIndex = 0;
    combat.m_hexCells[23].m_occupantSide = COMBAT_DEFENDER_SIDE;
    combat.m_hexCells[23].m_occupantIndex = 1;

    auto raw = [&](army& stack, ArmySpellInfluence effect) {
        return combat.RawEffectSpellInfluence(&stack, effect);
    };
    check(raw(friendly, ARMY_SPELL_INFLUENCE_HASTE) > 0, "friendly Haste has value");
    check(raw(friendly, ARMY_SPELL_INFLUENCE_HASTE) == raw(enemy, ARMY_SPELL_INFLUENCE_HASTE),
          "mirrored movement scores agree");
    check(raw(enemy, ARMY_SPELL_INFLUENCE_SLOW) < 0, "raw Slow is harmful to its target");

    auto score = [&](SpellType spell) {
        i32 value = 0, hex = -1;
        combat.DetermineEffectOfSpell(spell, &value, &hex);
        return value;
    };
    const int slow = score(SPELL_SLOW);
    const int curse = score(SPELL_CURSE);
    check(slow > 0 && curse > 0, "single-target debuffs have positive caster value");
    check(score(SPELL_MASS_SLOW) == 2 * slow, "Mass Slow sums both targets");
    check(score(SPELL_MASS_CURSE) == 2 * curse, "Mass Curse sums both targets");
    enemy.m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_HASTE)] = 3;
    enemy.m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLESS)] = 3;
    check(score(SPELL_SLOW) > slow, "removing Haste increases Slow value");
    check(score(SPELL_CURSE) > curse, "removing Bless increases Curse value");

    enemy.m_monsterType = CREATURE_GREEN_DRAGON;
    check(raw(friendly, ARMY_SPELL_INFLUENCE_DRAGON_SLAYER) == 140,
          "Dragon Slayer counts opposing dragons and preserves a fractional share");
    adjacentDragon = true;
    check(raw(friendly, ARMY_SPELL_INFLUENCE_DRAGON_SLAYER) == 280,
          "adjacent dragons give full Dragon Slayer value");
    adjacentDragon = false;
    enemy.m_monster.flags.all = MONSTER_FLAGS_SHOOTER;
    const int shield = raw(friendly, ARMY_SPELL_INFLUENCE_SHIELD);
    check(shield >= 224 && shield <= 225,
          "Shield counts opposing shooters and scales by stack worth");
    chance = 0;
    check(raw(friendly, ARMY_SPELL_INFLUENCE_SHIELD) == 0, "immunity suppresses value");
    chance = 1;
    combat.m_armyCount[1] = 0;
    check(raw(friendly, ARMY_SPELL_INFLUENCE_SHIELD) == 0, "empty enemy army is safe");
    check(raw(friendly, ARMY_SPELL_INFLUENCE_DRAGON_SLAYER) == 0, "no dragons means no value");
    combat.m_inCastleCombat = true;
    check(raw(friendly, ARMY_SPELL_INFLUENCE_SHIELD) > 0, "siege towers retain Shield value");
}
