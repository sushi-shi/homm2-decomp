#include <IRONFIST/register.h>

#include <algorithm>
#include <cctype>
#include <string>

#include <IRONFIST/artifacts.h>
#include <IRONFIST/dialog.h>
#include <IRONFIST/lua.h>
#include <IRONFIST/lua_utils.h>

#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

/*
 * The constant NAMES and VALUES here are Ironfist's scripting interface;
 * existing Ironfist map scripts depend on both. Values are spelled with the
 * port's recovered enums when the recovered name means the same thing, and as
 * numeric literals where Ironfist's reverse engineering named a slot
 * differently or extended a retail domain.
 */

namespace {
// Convert names like "Witch's Broach of Magic" to "WITCHS_BROACH_OF_MAGIC"
std::string LuaConstify(const std::string& str) {
    std::string upperStr(str.size(), '\0');
    std::transform(str.begin(), str.end(), upperStr.begin(), [](char c) {
        return static_cast<char>(::toupper(static_cast<u8>(c)));
    });
    std::replace(upperStr.begin(), upperStr.end(), ' ', '_');
    std::replace(upperStr.begin(), upperStr.end(), '-', '_');

    const auto posIdx = upperStr.find("'S");
    if (posIdx != std::string::npos) {
        upperStr.replace(posIdx, 2, "S");
    }

    return upperStr;
}
}

/******************************* GUI *****************************************/

static void set_dialog_consts(lua_State* L) {
    lua_setconst(L, "DIALOG_OKAY", DIALOG_OKAY);
    lua_setconst(L, "DIALOG_YES_NO", DIALOG_YES_NO);
    lua_setconst(L, "DIALOG_CANCEL", DIALOG_CANCEL);
    lua_setconst(L, "DIALOG_RIGHT_CLICK", DIALOG_RIGHT_CLICK);
    lua_setconst(L, "DIALOG_LEARN_CHOICE", DIALOG_LEARN_CHOICE);

    lua_setconst(L, "DIALOG_OR", DIALOG_OR);
    lua_setconst(L, "DIALOG_EMPTY", DIALOG_EMPTY);
}

static void set_barriercolor_consts(lua_State* L) {
    lua_setconst(L, "AQUA_BARRIER", 0);
}

static void set_messageboxgroups_consts(lua_State* L) {
    lua_setconst(L, "IMAGE_EMPTY", IMAGE_EMPTY);
    lua_setconst(L, "IMAGE_WOOD", IMAGE_WOOD);
    lua_setconst(L, "IMAGE_MERCURY", IMAGE_MERCURY);
    lua_setconst(L, "IMAGE_ORE", IMAGE_ORE);
    lua_setconst(L, "IMAGE_SULFUR", IMAGE_SULFUR);
    lua_setconst(L, "IMAGE_CRYSTALS", IMAGE_CRYSTALS);
    lua_setconst(L, "IMAGE_GEMS", IMAGE_GEMS);
    lua_setconst(L, "IMAGE_GOLD", IMAGE_GOLD);
    lua_setconst(L, "IMAGE_GROUP_ARTIFACTS", IMAGE_GROUP_ARTIFACTS);
    lua_setconst(L, "IMAGE_GROUP_SPELLS", IMAGE_GROUP_SPELLS);
    lua_setconst(L, "IMAGE_GROUP_PLAYERS", IMAGE_GROUP_PLAYERS);
    lua_setconst(L, "IMAGE_LUCK", IMAGE_LUCK);
    lua_setconst(L, "IMAGE_BADLUCK", IMAGE_BADLUCK);
    lua_setconst(L, "IMAGE_GOOD_MORALE", IMAGE_GOOD_MORALE);
    lua_setconst(L, "IMAGE_BAD_MORALE", IMAGE_BAD_MORALE);
    lua_setconst(L, "IMAGE_EXP", IMAGE_EXP);
    lua_setconst(L, "IMAGE_GROUP_HERO", IMAGE_GROUP_HERO);
    lua_setconst(L, "IMAGE_GROUP_SECONDARY_SKILLS", IMAGE_GROUP_SECONDARY_SKILLS);
    lua_setconst(L, "IMAGE_GROUP_UNIT", IMAGE_GROUP_UNIT);
    lua_setconst(L, "IMAGE_GROUP_PRIMARY_SKILLS", IMAGE_GROUP_PRIMARY_SKILLS);
}

static void set_gui_consts(lua_State* L) {
    set_dialog_consts(L);
    set_messageboxgroups_consts(L);
}

/*****************************************************************************/

static void set_spell_consts(lua_State* L) {
    lua_setconst(L, "SPELL_FIREBALL", H2EnumIndex(SPELL_FIREBALL));
    lua_setconst(L, "SPELL_FIREBLAST", H2EnumIndex(SPELL_FIREBLAST));
    lua_setconst(L, "SPELL_LIGHTNING_BOLT", H2EnumIndex(SPELL_LIGHTNING_BOLT));
    lua_setconst(L, "SPELL_CHAIN_LIGHTNING", H2EnumIndex(SPELL_CHAIN_LIGHTNING));
    lua_setconst(L, "SPELL_TELEPORT", H2EnumIndex(SPELL_TELEPORT));
    lua_setconst(L, "SPELL_CURE", H2EnumIndex(SPELL_CURE));
    lua_setconst(L, "SPELL_MASS_CURE", H2EnumIndex(SPELL_MASS_CURE));
    lua_setconst(L, "SPELL_RESURRECT", H2EnumIndex(SPELL_RESURRECT));
    lua_setconst(L, "SPELL_RESURRECT_TRUE", H2EnumIndex(SPELL_TRUE_RESURRECT));
    lua_setconst(L, "SPELL_HASTE", H2EnumIndex(SPELL_HASTE));
    lua_setconst(L, "SPELL_MASS_HASTE", H2EnumIndex(SPELL_MASS_HASTE));
    lua_setconst(L, "SPELL_SLOW", H2EnumIndex(SPELL_SLOW));
    lua_setconst(L, "SPELL_MASS_SLOW", H2EnumIndex(SPELL_MASS_SLOW));
    lua_setconst(L, "SPELL_BLIND", H2EnumIndex(SPELL_BLIND));
    lua_setconst(L, "SPELL_BLESS", H2EnumIndex(SPELL_BLESS));
    lua_setconst(L, "SPELL_MASS_BLESS", H2EnumIndex(SPELL_MASS_BLESS));
    lua_setconst(L, "SPELL_STONESKIN", H2EnumIndex(SPELL_STONE_SKIN));
    lua_setconst(L, "SPELL_STEELSKIN", H2EnumIndex(SPELL_STEEL_SKIN));
    lua_setconst(L, "SPELL_CURSE", H2EnumIndex(SPELL_CURSE));
    lua_setconst(L, "SPELL_MASS_CURSE", H2EnumIndex(SPELL_MASS_CURSE));
    lua_setconst(L, "SPELL_HOLY_WORD", H2EnumIndex(SPELL_HOLY_WORD));
    lua_setconst(L, "SPELL_HOLY_SHOUT", H2EnumIndex(SPELL_HOLY_SHOUT));
    lua_setconst(L, "SPELL_ANTI_MAGIC", H2EnumIndex(SPELL_ANTI_MAGIC));
    lua_setconst(L, "SPELL_DISPEL_MAGIC", H2EnumIndex(SPELL_DISPEL));
    lua_setconst(L, "SPELL_MASS_DISPEL", H2EnumIndex(SPELL_MASS_DISPEL));
    lua_setconst(L, "SPELL_MAGIC_ARROW", H2EnumIndex(SPELL_MAGIC_ARROW));
    lua_setconst(L, "SPELL_BERZERKER", H2EnumIndex(SPELL_BERSERKER));
    lua_setconst(L, "SPELL_ARMAGEDDON", H2EnumIndex(SPELL_ARMAGEDDON));
    lua_setconst(L, "SPELL_ELEMENTAL_STORM", H2EnumIndex(SPELL_ELEMENTAL_STORM));
    lua_setconst(L, "SPELL_METEOR_SHOWER", H2EnumIndex(SPELL_METEOR_SHOWER));
    lua_setconst(L, "SPELL_PARALYZE", H2EnumIndex(SPELL_PARALYZE));
    lua_setconst(L, "SPELL_HYPNOTIZE", H2EnumIndex(SPELL_HYPNOTIZE));
    lua_setconst(L, "SPELL_COLD_RAY", H2EnumIndex(SPELL_COLD_RAY));
    lua_setconst(L, "SPELL_COLD_RING", H2EnumIndex(SPELL_COLD_RING));
    lua_setconst(L, "SPELL_DISRUPTING_RAY", H2EnumIndex(SPELL_DISRUPTING_RAY));
    lua_setconst(L, "SPELL_DEATH_RIPPLE", H2EnumIndex(SPELL_DEATH_RIPPLE));
    lua_setconst(L, "SPELL_DEATH_WAVE", H2EnumIndex(SPELL_DEATH_WAVE));
    lua_setconst(L, "SPELL_DRAGON_SLAYER", H2EnumIndex(SPELL_DRAGON_SLAYER));
    lua_setconst(L, "SPELL_BLOOD_LUST", H2EnumIndex(SPELL_BLOOD_LUST));
    lua_setconst(L, "SPELL_ANIMATE_DEAD", H2EnumIndex(SPELL_ANIMATE_DEAD));
    lua_setconst(L, "SPELL_MIRROR_IMAGE", H2EnumIndex(SPELL_MIRROR_IMAGE));
    lua_setconst(L, "SPELL_SHIELD", H2EnumIndex(SPELL_SHIELD));
    lua_setconst(L, "SPELL_MASS_SHIELD", H2EnumIndex(SPELL_MASS_SHIELD));
    lua_setconst(L, "SPELL_SUMMON_EARTH_ELEMENTAL", H2EnumIndex(SPELL_SUMMON_EARTH_ELEMENTAL));
    lua_setconst(L, "SPELL_SUMMON_AIR_ELEMENTAL", H2EnumIndex(SPELL_SUMMON_AIR_ELEMENTAL));
    lua_setconst(L, "SPELL_SUMMON_FIRE_ELEMENTAL", H2EnumIndex(SPELL_SUMMON_FIRE_ELEMENTAL));
    lua_setconst(L, "SPELL_SUMMON_WATER_ELEMENTAL", H2EnumIndex(SPELL_SUMMON_WATER_ELEMENTAL));
    lua_setconst(L, "SPELL_EARTHQUAKE", H2EnumIndex(SPELL_EARTHQUAKE));
    lua_setconst(L, "SPELL_VIEW_MINES", H2EnumIndex(SPELL_VIEW_MINES));
    lua_setconst(L, "SPELL_VIEW_RESOURCES", H2EnumIndex(SPELL_VIEW_RESOURCES));
    lua_setconst(L, "SPELL_VIEW_ARTIFACTS", H2EnumIndex(SPELL_VIEW_ARTIFACTS));
    lua_setconst(L, "SPELL_VIEW_TOWNS", H2EnumIndex(SPELL_VIEW_TOWNS));
    lua_setconst(L, "SPELL_VIEW_HEROES", H2EnumIndex(SPELL_VIEW_HEROES));
    lua_setconst(L, "SPELL_VIEW_ALL", H2EnumIndex(SPELL_VIEW_ALL));
    lua_setconst(L, "SPELL_IDENTIFY", H2EnumIndex(SPELL_IDENTIFY_HERO));
    lua_setconst(L, "SPELL_SUMMON_BOAT", H2EnumIndex(SPELL_SUMMON_BOAT));
    lua_setconst(L, "SPELL_DIMENSION_DOOR", H2EnumIndex(SPELL_DIMENSION_DOOR));
    lua_setconst(L, "SPELL_TOWN_GATE", H2EnumIndex(SPELL_TOWN_GATE));
    lua_setconst(L, "SPELL_TOWN_PORTAL", H2EnumIndex(SPELL_TOWN_PORTAL));
    lua_setconst(L, "SPELL_VISIONS", H2EnumIndex(SPELL_VISIONS));
    lua_setconst(L, "SPELL_HAUNT", H2EnumIndex(SPELL_HAUNT));
    lua_setconst(L, "SPELL_SET_EARTH_GUARDIAN", H2EnumIndex(SPELL_SET_EARTH_GUARDIAN));
    lua_setconst(L, "SPELL_SET_AIR_GUARDIAN", H2EnumIndex(SPELL_SET_AIR_GUARDIAN));
    lua_setconst(L, "SPELL_SET_FIRE_GUARDIAN", H2EnumIndex(SPELL_SET_FIRE_GUARDIAN));
    lua_setconst(L, "SPELL_SET_WATER_GUARDIAN", H2EnumIndex(SPELL_SET_WATER_GUARDIAN));
    // Ironfist-only spells retain their published Lua IDs.
    lua_setconst(L, "SPELL_MARKSMAN_PIERCE", 67);
    lua_setconst(L, "SPELL_PLASMA_CONE", 68);
    lua_setconst(L, "SPELL_FORCE_SHIELD", 69);
    lua_setconst(L, "SPELL_MASS_FORCE_SHIELD", 70);
    lua_setconst(L, "SPELL_FIRE_BOMB", 71);
    lua_setconst(L, "SPELL_IMPLOSION_GRENADE", 72);
}

static void set_artifact_consts(lua_State* L) {
    for (i32 i = 0; i < KB_ARTIFACT_TABLE_CAPACITY; ++i) {
        if (IsArtifactValid(i)) {
            const std::string constName = "ARTIFACT_" + LuaConstify(GetArtifactName(i));
            lua_setconst(L, constName.c_str(), i);
        }
    }
}

static void set_town_consts(lua_State* L) {
    // Ironfist building codes are bit indexes into the town buildingsBuilt
    // mask; the port's TOWN_BUILDING_* constants are the masks themselves.
    lua_setconst(L, "BUILDING_MAGE_GUILD", 0x0);
    lua_setconst(L, "BUILDING_THIEVES_GUILD", 0x1);
    lua_setconst(L, "BUILDING_TAVERN", 0x2);
    lua_setconst(L, "BUILDING_DOCK", 0x3);
    lua_setconst(L, "BUILDING_WELL", 0x4);
    lua_setconst(L, "BUILDING_TENT", 0x5);
    lua_setconst(L, "BUILDING_CASTLE", 0x6);
    lua_setconst(L, "BUILDING_STATUE", 0x7);
    lua_setconst(L, "BUILDING_LEFT_TURRET", 0x8);
    lua_setconst(L, "BUILDING_RIGHT_TURRET", 0x9);
    lua_setconst(L, "BUILDING_MARKET", 0xa);
    lua_setconst(L, "BUILDING_SPECIAL_GROWTH", 0xb);
    lua_setconst(L, "BUILDING_MOAT", 0xc);
    lua_setconst(L, "BUILDING_SPECIAL", 0xd);
    lua_setconst(L, "BUILDING_BOAT", 0xe);
    lua_setconst(L, "BUILDING_CAPTAIN", 0xf);
    lua_setconst(L, "BUILDING_DWELLING_1", 0x13);
    lua_setconst(L, "BUILDING_DWELLING_2", 0x14);
    lua_setconst(L, "BUILDING_DWELLING_3", 0x15);
    lua_setconst(L, "BUILDING_DWELLING_4", 0x16);
    lua_setconst(L, "BUILDING_DWELLING_5", 0x17);
    lua_setconst(L, "BUILDING_DWELLING_6", 0x18);
    lua_setconst(L, "BUILDING_UPGRADE_1", 0x19);
    lua_setconst(L, "BUILDING_UPGRADE_2", 0x1a);
    lua_setconst(L, "BUILDING_UPGRADE_3", 0x1b);
    lua_setconst(L, "BUILDING_UPGRADE_4", 0x1c);
    lua_setconst(L, "BUILDING_UPGRADE_5", 0x1d);
    lua_setconst(L, "BUILDING_UPGRADE_5B", 0x1e);
}

static void set_faction_consts(lua_State* L) {
    lua_setconst(L, "FACTION_KNIGHT", H2EnumIndex(FACTION_KNIGHT));
    lua_setconst(L, "FACTION_BARBARIAN", H2EnumIndex(FACTION_BARBARIAN));
    lua_setconst(L, "FACTION_SORCERESS", H2EnumIndex(FACTION_SORCERESS));
    lua_setconst(L, "FACTION_WARLOCK", H2EnumIndex(FACTION_WARLOCK));
    lua_setconst(L, "FACTION_WIZARD", H2EnumIndex(FACTION_WIZARD));
    lua_setconst(L, "FACTION_NECROMANCER", H2EnumIndex(FACTION_NECROMANCER));
    lua_setconst(L, "FACTION_MULTIPLE", H2EnumIndex(FACTION_NEUTRAL));
    lua_setconst(L, "FACTION_RANDOM", H2EnumIndex(FACTION_RANDOM));

    // Ironfist-only faction retains its published Lua ID.
    lua_setconst(L, "FACTION_CYBORG", H2EnumIndex(FACTION_CYBORG));
}

static void set_skill_consts(lua_State* L) {
    lua_setconst(L, "PRIMARY_SKILL_ATTACK", H2EnumIndex(HERO_PRIMARY_ATTACK));
    lua_setconst(L, "PRIMARY_SKILL_DEFENSE", H2EnumIndex(HERO_PRIMARY_DEFENSE));
    lua_setconst(L, "PRIMARY_SKILL_SPELLPOWER", H2EnumIndex(HERO_PRIMARY_SPELL_POWER));
    lua_setconst(L, "PRIMARY_SKILL_KNOWLEDGE", H2EnumIndex(HERO_PRIMARY_KNOWLEDGE));
    lua_setconst(L, "SECONDARY_SKILL_PATHFINDING", H2EnumIndex(HERO_SKILL_PATHFINDING));
    lua_setconst(L, "SECONDARY_SKILL_ARCHERY", H2EnumIndex(HERO_SKILL_ARCHERY));
    lua_setconst(L, "SECONDARY_SKILL_LOGISTICS", H2EnumIndex(HERO_SKILL_LOGISTICS));
    lua_setconst(L, "SECONDARY_SKILL_SCOUTING", H2EnumIndex(HERO_SKILL_SCOUTING));
    lua_setconst(L, "SECONDARY_SKILL_DIPLOMACY", H2EnumIndex(HERO_SKILL_DIPLOMACY));
    lua_setconst(L, "SECONDARY_SKILL_NAVIGATION", H2EnumIndex(HERO_SKILL_NAVIGATION));
    lua_setconst(L, "SECONDARY_SKILL_LEADERSHIP", H2EnumIndex(HERO_SKILL_LEADERSHIP));
    lua_setconst(L, "SECONDARY_SKILL_WISDOM", H2EnumIndex(HERO_SKILL_WISDOM));
    lua_setconst(L, "SECONDARY_SKILL_MYSTICISM", H2EnumIndex(HERO_SKILL_MYSTICISM));
    lua_setconst(L, "SECONDARY_SKILL_LUCK", H2EnumIndex(HERO_SKILL_LUCK));
    lua_setconst(L, "SECONDARY_SKILL_BALLISTICS", H2EnumIndex(HERO_SKILL_BALLISTICS));
    lua_setconst(L, "SECONDARY_SKILL_EAGLE_EYE", H2EnumIndex(HERO_SKILL_EAGLE_EYE));
    lua_setconst(L, "SECONDARY_SKILL_NECROMANCY", H2EnumIndex(HERO_SKILL_NECROMANCY));
    lua_setconst(L, "SECONDARY_SKILL_ESTATES", H2EnumIndex(HERO_SKILL_ESTATES));
}

static void set_creature_consts(lua_State* L) {
    lua_setconst(L, "CREATURE_PEASANT", H2EnumIndex(CREATURE_PEASANT));
    lua_setconst(L, "CREATURE_ARCHER", H2EnumIndex(CREATURE_ARCHER));
    lua_setconst(L, "CREATURE_RANGER", H2EnumIndex(CREATURE_RANGER));
    lua_setconst(L, "CREATURE_PIKEMAN", H2EnumIndex(CREATURE_PIKEMAN));
    lua_setconst(L, "CREATURE_VETERAN_PIKEMAN", H2EnumIndex(CREATURE_VETERAN_PIKEMAN));
    lua_setconst(L, "CREATURE_SWORDSMAN", H2EnumIndex(CREATURE_SWORDSMAN));
    lua_setconst(L, "CREATURE_MASTER_SWORDSMAN", H2EnumIndex(CREATURE_MASTER_SWORDSMAN));
    lua_setconst(L, "CREATURE_CAVALRY", H2EnumIndex(CREATURE_CAVALRY));
    lua_setconst(L, "CREATURE_CHAMPION", H2EnumIndex(CREATURE_CHAMPION));
    lua_setconst(L, "CREATURE_PALADIN", H2EnumIndex(CREATURE_PALADIN));
    lua_setconst(L, "CREATURE_CRUSADER", H2EnumIndex(CREATURE_CRUSADER));
    lua_setconst(L, "CREATURE_GOBLIN", H2EnumIndex(CREATURE_GOBLIN));
    lua_setconst(L, "CREATURE_ORC", H2EnumIndex(CREATURE_ORC));
    lua_setconst(L, "CREATURE_ORC_CHIEF", H2EnumIndex(CREATURE_ORC_CHIEF));
    lua_setconst(L, "CREATURE_WOLF", H2EnumIndex(CREATURE_WOLF));
    lua_setconst(L, "CREATURE_OGRE", H2EnumIndex(CREATURE_OGRE));
    lua_setconst(L, "CREATURE_OGRE_LORD", H2EnumIndex(CREATURE_OGRE_LORD));
    lua_setconst(L, "CREATURE_TROLL", H2EnumIndex(CREATURE_TROLL));
    lua_setconst(L, "CREATURE_WAR_TROLL", H2EnumIndex(CREATURE_WAR_TROLL));
    lua_setconst(L, "CREATURE_CYCLOPS", H2EnumIndex(CREATURE_CYCLOPS));
    lua_setconst(L, "CREATURE_SPRITE", H2EnumIndex(CREATURE_SPRITE));
    lua_setconst(L, "CREATURE_DWARF", H2EnumIndex(CREATURE_DWARF));
    lua_setconst(L, "CREATURE_BATTLE_DWARF", H2EnumIndex(CREATURE_BATTLE_DWARF));
    lua_setconst(L, "CREATURE_ELF", H2EnumIndex(CREATURE_ELF));
    lua_setconst(L, "CREATURE_GRAND_ELF", H2EnumIndex(CREATURE_GRAND_ELF));
    lua_setconst(L, "CREATURE_DRUID", H2EnumIndex(CREATURE_DRUID));
    lua_setconst(L, "CREATURE_GREATER_DRUID", H2EnumIndex(CREATURE_GREATER_DRUID));
    lua_setconst(L, "CREATURE_UNICORN", H2EnumIndex(CREATURE_UNICORN));
    lua_setconst(L, "CREATURE_PHOENIX", H2EnumIndex(CREATURE_PHOENIX));
    lua_setconst(L, "CREATURE_CENTAUR", H2EnumIndex(CREATURE_CENTAUR));
    lua_setconst(L, "CREATURE_GARGOYLE", H2EnumIndex(CREATURE_GARGOYLE));
    lua_setconst(L, "CREATURE_GRIFFIN", H2EnumIndex(CREATURE_GRIFFIN));
    lua_setconst(L, "CREATURE_MINOTAUR", H2EnumIndex(CREATURE_MINOTAUR));
    lua_setconst(L, "CREATURE_MINOTAUR_KING", H2EnumIndex(CREATURE_MINOTAUR_KING));
    lua_setconst(L, "CREATURE_HYDRA", H2EnumIndex(CREATURE_HYDRA));
    lua_setconst(L, "CREATURE_GREEN_DRAGON", H2EnumIndex(CREATURE_GREEN_DRAGON));
    lua_setconst(L, "CREATURE_RED_DRAGON", H2EnumIndex(CREATURE_RED_DRAGON));
    lua_setconst(L, "CREATURE_BLACK_DRAGON", H2EnumIndex(CREATURE_BLACK_DRAGON));
    lua_setconst(L, "CREATURE_HALFLING", H2EnumIndex(CREATURE_HALFLING));
    lua_setconst(L, "CREATURE_BOAR", H2EnumIndex(CREATURE_BOAR));
    lua_setconst(L, "CREATURE_IRON_GOLEM", H2EnumIndex(CREATURE_IRON_GOLEM));
    lua_setconst(L, "CREATURE_STEEL_GOLEM", H2EnumIndex(CREATURE_STEEL_GOLEM));
    lua_setconst(L, "CREATURE_ROC", H2EnumIndex(CREATURE_ROC));
    lua_setconst(L, "CREATURE_MAGE", H2EnumIndex(CREATURE_MAGE));
    lua_setconst(L, "CREATURE_ARCHMAGE", H2EnumIndex(CREATURE_ARCHMAGE));
    lua_setconst(L, "CREATURE_GIANT", H2EnumIndex(CREATURE_GIANT));
    lua_setconst(L, "CREATURE_TITAN", H2EnumIndex(CREATURE_TITAN));
    lua_setconst(L, "CREATURE_SKELETON", H2EnumIndex(CREATURE_SKELETON));
    lua_setconst(L, "CREATURE_ZOMBIE", H2EnumIndex(CREATURE_ZOMBIE));
    lua_setconst(L, "CREATURE_MUTANT_ZOMBIE", H2EnumIndex(CREATURE_MUTANT_ZOMBIE));
    lua_setconst(L, "CREATURE_MUMMY", H2EnumIndex(CREATURE_MUMMY));
    lua_setconst(L, "CREATURE_ROYAL_MUMMY", H2EnumIndex(CREATURE_ROYAL_MUMMY));
    lua_setconst(L, "CREATURE_VAMPIRE", H2EnumIndex(CREATURE_VAMPIRE));
    lua_setconst(L, "CREATURE_VAMPIRE_LORD", H2EnumIndex(CREATURE_VAMPIRE_LORD));
    lua_setconst(L, "CREATURE_LICH", H2EnumIndex(CREATURE_LICH));
    lua_setconst(L, "CREATURE_POWER_LICH", H2EnumIndex(CREATURE_POWER_LICH));
    lua_setconst(L, "CREATURE_BONE_DRAGON", H2EnumIndex(CREATURE_BONE_DRAGON));
    lua_setconst(L, "CREATURE_ROGUE", H2EnumIndex(CREATURE_ROGUE));
    lua_setconst(L, "CREATURE_NOMAD", H2EnumIndex(CREATURE_NOMAD));
    lua_setconst(L, "CREATURE_GHOST", H2EnumIndex(CREATURE_GHOST));
    lua_setconst(L, "CREATURE_GENIE", H2EnumIndex(CREATURE_GENIE));
    lua_setconst(L, "CREATURE_MEDUSA", H2EnumIndex(CREATURE_MEDUSA));
    lua_setconst(L, "CREATURE_EARTH_ELEMENTAL", H2EnumIndex(CREATURE_EARTH_ELEMENTAL));
    lua_setconst(L, "CREATURE_AIR_ELEMENTAL", H2EnumIndex(CREATURE_AIR_ELEMENTAL));
    lua_setconst(L, "CREATURE_FIRE_ELEMENTAL", H2EnumIndex(CREATURE_FIRE_ELEMENTAL));
    lua_setconst(L, "CREATURE_WATER_ELEMENTAL", H2EnumIndex(CREATURE_WATER_ELEMENTAL));
    // Ironfist-only creatures retain their published Lua IDs.
    lua_setconst(L, "CREATURE_KOBOLD", 72);
    lua_setconst(L, "CREATURE_BLOODSUCKER", 73);
    lua_setconst(L, "CREATURE_HARPY", 74);
    lua_setconst(L, "CREATURE_BLACK_KNIGHT", 75);
    lua_setconst(L, "CREATURE_CATOBLEBA", 76);
    lua_setconst(L, "CREATURE_TREANT", 77);
    lua_setconst(L, "CREATURE_CYBER_KOBOLD_SPEARMAN", 78);
    lua_setconst(L, "CREATURE_CYBER_PLASMA_BERSERKER", 79);
    lua_setconst(L, "CREATURE_CYBER_PLASMA_LANCER", 80);
    lua_setconst(L, "CREATURE_CYBER_INDIGO_PANTHER", 81);
    lua_setconst(L, "CREATURE_CYBER_SHADOW_ASSASSIN", 82);
    lua_setconst(L, "CREATURE_CYBER_BEHEMOTH", 83);
}

static void set_resources_consts(lua_State* L) {
    lua_setconst(L, "RESOURCE_WOOD", H2EnumIndex(RES_WOOD));
    lua_setconst(L, "RESOURCE_MERCURY", H2EnumIndex(RES_MERCURY));
    lua_setconst(L, "RESOURCE_ORE", H2EnumIndex(RES_ORE));
    lua_setconst(L, "RESOURCE_SULFUR", H2EnumIndex(RES_SULFUR));
    lua_setconst(L, "RESOURCE_CRYSTALS", H2EnumIndex(RES_CRYSTAL));
    lua_setconst(L, "RESOURCE_GEMS", H2EnumIndex(RES_GEMS));
    lua_setconst(L, "RESOURCE_GOLD", H2EnumIndex(RES_GOLD));
}

static void set_location_consts(lua_State* L) {
    lua_setconst(L, "LOCATION_ALCHEMIST_LAB", H2EnumIndex(MAP_OBJECT_ALCHEMIST_LAB));
    lua_setconst(L, "LOCATION_SIGN", H2EnumIndex(MAP_OBJECT_SIGN));
    lua_setconst(L, "LOCATION_BUOY", H2EnumIndex(MAP_OBJECT_BUOY));
    lua_setconst(L, "LOCATION_SKELETON", H2EnumIndex(MAP_OBJECT_SKELETON));
    lua_setconst(L, "LOCATION_DAEMON_CAVE", H2EnumIndex(MAP_OBJECT_DAEMON_CAVE));
    lua_setconst(L, "LOCATION_TREASURE_CHEST", H2EnumIndex(MAP_OBJECT_TREASURE_CHEST));
    lua_setconst(L, "LOCATION_FAERIE_RING", H2EnumIndex(MAP_OBJECT_FAERIE_RING));
    lua_setconst(L, "LOCATION_CAMPFIRE", H2EnumIndex(MAP_OBJECT_CAMPFIRE));
    lua_setconst(L, "LOCATION_FOUNTAIN", H2EnumIndex(MAP_OBJECT_FOUNTAIN));
    lua_setconst(L, "LOCATION_GAZEBO", H2EnumIndex(MAP_OBJECT_GAZEBO));
    lua_setconst(L, "LOCATION_ANCIENT_LAMP", H2EnumIndex(MAP_OBJECT_ANCIENT_LAMP));
    lua_setconst(L, "LOCATION_GRAVEYARD", H2EnumIndex(MAP_OBJECT_GRAVEYARD));
    lua_setconst(L, "LOCATION_ARCHERS_HOUSE", H2EnumIndex(MAP_OBJECT_ARCHER_HOUSE));
    lua_setconst(L, "LOCATION_GOBLIN_HUT", H2EnumIndex(MAP_OBJECT_GOBLIN_HUT));
    lua_setconst(L, "LOCATION_DWARF_COTTAGE", H2EnumIndex(MAP_OBJECT_DWARF_COTTAGE));
    lua_setconst(L, "LOCATION_PEASANT_HUT", H2EnumIndex(MAP_OBJECT_PEASANT_HUT));
    lua_setconst(L, "LOCATION_LOG_CABIN", H2EnumIndex(MAP_OBJECT_LOG_CABIN));
    // Ironfist's names for 18 and 19 predate the recovered ones.
    lua_setconst(L, "LOCATION_ROAD", H2EnumIndex(MAP_OBJECT_ALCHEMIST_TOWER));
    lua_setconst(L, "LOCATION_EVENT", H2EnumIndex(MAP_OBJECT_MAP_EVENT));
    lua_setconst(L, "LOCATION_DRAGON_CITY", H2EnumIndex(MAP_OBJECT_DRAGON_CITY));
    lua_setconst(L, "LOCATION_LIGHTHOUSE", H2EnumIndex(MAP_OBJECT_LIGHTHOUSE));
    lua_setconst(L, "LOCATION_WATERWHEEL", H2EnumIndex(MAP_OBJECT_WATER_WHEEL));
    lua_setconst(L, "LOCATION_MINE", H2EnumIndex(MAP_OBJECT_MINE));
    lua_setconst(L, "LOCATION_ARMY_CAMP", H2EnumIndex(MAP_OBJECT_MONSTER));
    lua_setconst(L, "LOCATION_OBELISK", H2EnumIndex(MAP_OBJECT_OBELISK));
    lua_setconst(L, "LOCATION_OASIS", H2EnumIndex(MAP_OBJECT_OASIS));
    lua_setconst(L, "LOCATION_RESOURCE", H2EnumIndex(MAP_OBJECT_RESOURCE));
    lua_setconst(L, "LOCATION_SAWMILL", H2EnumIndex(MAP_OBJECT_SAWMILL));
    lua_setconst(L, "LOCATION_ORACLE", H2EnumIndex(MAP_OBJECT_ORACLE));
    lua_setconst(L, "LOCATION_SHRINE_FIRST_ORDER", H2EnumIndex(MAP_OBJECT_SHRINE_FIRST_CIRCLE));
    lua_setconst(L, "LOCATION_SHIPWRECK", H2EnumIndex(MAP_OBJECT_SHIPWRECK));
    lua_setconst(L, "LOCATION_SEA_CHEST", H2EnumIndex(MAP_OBJECT_SEA_CHEST));
    lua_setconst(L, "LOCATION_DESERT_TENT", H2EnumIndex(MAP_OBJECT_DESERT_TENT));
    lua_setconst(L, "LOCATION_TOWN", H2EnumIndex(MAP_OBJECT_CASTLE));
    lua_setconst(L, "LOCATION_STONE_LITHS", H2EnumIndex(MAP_OBJECT_STONE_LITHS));
    lua_setconst(L, "LOCATION_WAGON_CAMP", H2EnumIndex(MAP_OBJECT_WAGON_CAMP));
    lua_setconst(L, "LOCATION_WELL", H2EnumIndex(MAP_OBJECT_HUT_OF_MAGI));
    lua_setconst(L, "LOCATION_WHIRLPOOL", H2EnumIndex(MAP_OBJECT_WHIRLPOOL));
    lua_setconst(L, "LOCATION_WINDMILL", H2EnumIndex(MAP_OBJECT_WINDMILL));
    lua_setconst(L, "LOCATION_ARTIFACT", H2EnumIndex(MAP_OBJECT_ARTIFACT));
    lua_setconst(L, "LOCATION_HERO", H2EnumIndex(MAP_OBJECT_MERMAID));
    lua_setconst(L, "LOCATION_BOAT", H2EnumIndex(MAP_OBJECT_BOAT));
    lua_setconst(L, "LOCATION_RANDOM_ARTIFACT", H2EnumIndex(MAP_OBJECT_RANDOM_ARTIFACT));
    lua_setconst(L, "LOCATION_RANDOM_RESOURCE", H2EnumIndex(MAP_OBJECT_RANDOM_RESOURCE));
    lua_setconst(L, "LOCATION_RANDOM_MONSTER", H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER));
    lua_setconst(L, "LOCATION_RANDOM_TOWN", H2EnumIndex(MAP_OBJECT_RANDOM_TOWN));
    lua_setconst(L, "LOCATION_RANDOM_CASTLE", H2EnumIndex(MAP_OBJECT_RANDOM_CASTLE));
    lua_setconst(L, "LOCATION_RANDOM_MONSTER_WEAK", H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER_WEAK));
    lua_setconst(
        L, "LOCATION_RANDOM_MONSTER_MEDIUM", H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER_MEDIUM)
    );
    lua_setconst(
        L, "LOCATION_RANDOM_MONSTER_STRONG", H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER_STRONG)
    );
    lua_setconst(
        L, "LOCATION_RANDOM_MONSTER_VERY_STRONG",
        H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG)
    );
    lua_setconst(L, "LOCATION_RANDOM_HERO", H2EnumIndex(MAP_OBJECT_HERO));
    lua_setconst(L, "LOCATION_NOTHING_SPECIAL", H2EnumIndex(MAP_OBJECT_NOTHING_SPECIAL));
    lua_setconst(L, "LOCATION_WATCH_TOWER", H2EnumIndex(MAP_OBJECT_WATCH_TOWER));
    lua_setconst(L, "LOCATION_TREE_HOUSE", H2EnumIndex(MAP_OBJECT_TREE_HOUSE));
    lua_setconst(L, "LOCATION_TREE_CITY", H2EnumIndex(MAP_OBJECT_TREE_CITY));
    lua_setconst(L, "LOCATION_RUINS", H2EnumIndex(MAP_OBJECT_RUINS));
    lua_setconst(L, "LOCATION_FORT", H2EnumIndex(MAP_OBJECT_FORT));
    lua_setconst(L, "LOCATION_TRADING_POST", H2EnumIndex(MAP_OBJECT_TRADING_POST));
    lua_setconst(L, "LOCATION_ABANDONED_MINE", H2EnumIndex(MAP_OBJECT_ABANDONED_MINE));
    lua_setconst(L, "LOCATION_DWARF_CABIN", H2EnumIndex(MAP_OBJECT_SIRENS));
    lua_setconst(L, "LOCATION_STANDING_STONES", H2EnumIndex(MAP_OBJECT_STANDING_STONES));
    lua_setconst(L, "LOCATION_IDOL", H2EnumIndex(MAP_OBJECT_IDOL));
    lua_setconst(L, "LOCATION_TREE_OF_KNOWLEDGE", H2EnumIndex(MAP_OBJECT_TREE_OF_KNOWLEDGE));
    lua_setconst(L, "LOCATION_WITCH_DOCTORS_HUT", H2EnumIndex(MAP_OBJECT_WITCH_DOCTOR_HUT));
    lua_setconst(L, "LOCATION_TEMPLE", H2EnumIndex(MAP_OBJECT_TEMPLE));
    lua_setconst(L, "LOCATION_HILL_FORT", H2EnumIndex(MAP_OBJECT_HILL_FORT));
    lua_setconst(L, "LOCATION_HALFLING_HOLE", H2EnumIndex(MAP_OBJECT_HALFLING_HOLE));
    lua_setconst(L, "LOCATION_MERCENARY_CAMP", H2EnumIndex(MAP_OBJECT_MERCENARY_CAMP));
    lua_setconst(L, "LOCATION_SHRINE_SECOND_ORDER", H2EnumIndex(MAP_OBJECT_SHRINE_SECOND_CIRCLE));
    lua_setconst(L, "LOCATION_SHRINE_THIRD_ORDER", H2EnumIndex(MAP_OBJECT_SHRINE_THIRD_CIRCLE));
    lua_setconst(L, "LOCATION_PYRAMID", H2EnumIndex(MAP_OBJECT_PYRAMID));
    lua_setconst(L, "LOCATION_CITY_OF_DEAD", H2EnumIndex(MAP_OBJECT_CITY_OF_DEAD));
    lua_setconst(L, "LOCATION_EXCAVATION", H2EnumIndex(MAP_OBJECT_EXCAVATION));
    lua_setconst(L, "LOCATION_SPHINX", H2EnumIndex(MAP_OBJECT_SPHINX));
    lua_setconst(L, "LOCATION_WAGON", H2EnumIndex(MAP_OBJECT_WAGON));
    lua_setconst(L, "LOCATION_TAR_PIT", H2EnumIndex(MAP_OBJECT_TAR_PIT));
    lua_setconst(L, "LOCATION_ARTESIAN_SPRING", H2EnumIndex(MAP_OBJECT_ARTESIAN_SPRING));
    lua_setconst(L, "LOCATION_TROLL_BRIDGE", H2EnumIndex(MAP_OBJECT_TROLL_BRIDGE));
    lua_setconst(L, "LOCATION_WATERING_HOLE", H2EnumIndex(MAP_OBJECT_WATERING_HOLE));
    lua_setconst(L, "LOCATION_WITCH_HUT", H2EnumIndex(MAP_OBJECT_WITCH_HUT));
    lua_setconst(L, "LOCATION_XANADU", H2EnumIndex(MAP_OBJECT_XANADU));
    lua_setconst(L, "LOCATION_CAVE", H2EnumIndex(MAP_OBJECT_CAVE));
    lua_setconst(L, "LOCATION_LEAN_TO", H2EnumIndex(MAP_OBJECT_LEAN_TO));
    lua_setconst(L, "LOCATION_MAGELLANS_MAPS", H2EnumIndex(MAP_OBJECT_MAGELLAN_MAPS));
    lua_setconst(L, "LOCATION_FLOTSAM", H2EnumIndex(MAP_OBJECT_FLOTSAM));
    lua_setconst(L, "LOCATION_DERELICT_SHIP", H2EnumIndex(MAP_OBJECT_DERELICT_SHIP));
    lua_setconst(L, "LOCATION_SHIPWRECK_SURVIVOR", H2EnumIndex(MAP_OBJECT_SHIPWRECK_SURVIVOR));
    lua_setconst(L, "LOCATION_BOTTLE", H2EnumIndex(MAP_OBJECT_BOTTLE));
    lua_setconst(L, "LOCATION_MAGIC_WELL", H2EnumIndex(MAP_OBJECT_MAGIC_WELL));
    lua_setconst(L, "LOCATION_MAGIC_GARDEN", H2EnumIndex(MAP_OBJECT_MAGIC_GARDEN));
    lua_setconst(L, "LOCATION_OBSERVATION_TOWER", H2EnumIndex(MAP_OBJECT_OBSERVATION_TOWER));
    lua_setconst(L, "LOCATION_FREEMANS_FOUNDRY", H2EnumIndex(MAP_OBJECT_FREEMANS_FOUNDRY));
    lua_setconst(L, "LOCATION_STREAM", H2EnumIndex(MAP_OBJECT_REEFS));
    lua_setconst(L, "LOCATION_TREES", H2EnumIndex(MAP_OBJECT_TREES));
    lua_setconst(L, "LOCATION_MOUNTAINS", H2EnumIndex(MAP_OBJECT_MOUNTAINS));
    lua_setconst(L, "LOCATION_VOLCANO", H2EnumIndex(MAP_OBJECT_VOLCANO));
    lua_setconst(L, "LOCATION_FLOWERS", H2EnumIndex(MAP_OBJECT_FLOWERS));
    lua_setconst(L, "LOCATION_ROCK", H2EnumIndex(MAP_OBJECT_ROCK));
    lua_setconst(L, "LOCATION_LAKE", H2EnumIndex(MAP_OBJECT_WATER_LAKE));
    lua_setconst(L, "LOCATION_MANDRAKE", H2EnumIndex(MAP_OBJECT_MANDRAKE));
    lua_setconst(L, "LOCATION_DEAD_TREE", H2EnumIndex(MAP_OBJECT_DEAD_TREE));
    lua_setconst(L, "LOCATION_STUMP", H2EnumIndex(MAP_OBJECT_STUMP));
    lua_setconst(L, "LOCATION_CRATER", H2EnumIndex(MAP_OBJECT_CRATER));
    lua_setconst(L, "LOCATION_CACTUS", H2EnumIndex(MAP_OBJECT_CACTUS));
    lua_setconst(L, "LOCATION_MOUND", H2EnumIndex(MAP_OBJECT_MOUND));
    lua_setconst(L, "LOCATION_DUNE", H2EnumIndex(MAP_OBJECT_DUNE));
    lua_setconst(L, "LOCATION_LAVA_POOL", H2EnumIndex(MAP_OBJECT_LAVA_POOL));
    lua_setconst(L, "LOCATION_SHRUB", H2EnumIndex(MAP_OBJECT_SHRUB));
    lua_setconst(L, "LOCATION_HOLE", H2EnumIndex(MAP_OBJECT_ARENA));
    lua_setconst(L, "LOCATION_OUTCROPPING", H2EnumIndex(MAP_OBJECT_BARROW_MOUNDS));
    lua_setconst(
        L, "LOCATION_RANDOM_ARTIFACT_TREASURE", H2EnumIndex(MAP_OBJECT_RANDOM_TREASURE_ARTIFACT)
    );
    lua_setconst(
        L, "LOCATION_RANDOM_ARTIFACT_MINOR", H2EnumIndex(MAP_OBJECT_RANDOM_MINOR_ARTIFACT)
    );
    lua_setconst(
        L, "LOCATION_RANDOM_ARTIFACT_MAJOR", H2EnumIndex(MAP_OBJECT_RANDOM_MAJOR_ARTIFACT)
    );
    lua_setconst(L, "LOCATION_BARRIER", H2EnumIndex(MAP_OBJECT_BARRIER));
    lua_setconst(L, "LOCATION_TRAVELLER_TENT", H2EnumIndex(MAP_OBJECT_TRAVELER_TENT));
    lua_setconst(L, "LOCATION_EXPANSION_DWELLING", H2EnumIndex(MAP_OBJECT_EXPANSION_DWELLING));
    lua_setconst(L, "LOCATION_ALCHEMIST_TOWER", H2EnumIndex(MAP_OBJECT_EXPANSION_OBJECT));
    lua_setconst(L, "LOCATION_JAIL", H2EnumIndex(MAP_OBJECT_JAIL));
    lua_setconst(L, "LOCATION_SHIPYARD", H2EnumIndex(MAP_OBJECT_SHIPYARD));
}

static void set_map_cell_consts(lua_State* L) {
    lua_setconst(L, "MAP_CELL_NO_FLIP", 0);
    lua_setconst(L, "MAP_CELL_FLIP_VERTICALLY", 0x1);
    lua_setconst(L, "MAP_CELL_FLIP_HORIZONTALLY", 0x2);
    lua_setconst(L, "MAP_CELL_FLIP_DIAGONALLY", 0x3);
}

static void set_tooltip_consts(lua_State* L) {
    lua_setconst_nil(L, "TOOLTIP_DEFAULT");
}

static void set_map_consts(lua_State* L) {
    set_location_consts(L);
    set_map_cell_consts(L);
    set_tooltip_consts(L);
}

static void set_campaign_choice_consts(lua_State* L) {
    lua_setconst(L, "CAMPAIGN_CHOICE_RESOURCE", H2EnumIndex(CAMPAIGN_CHOICE_RESOURCE));
    lua_setconst(L, "CAMPAIGN_CHOICE_ARTIFACT", H2EnumIndex(CAMPAIGN_CHOICE_ARTIFACT));
    lua_setconst(L, "CAMPAIGN_CHOICE_SPELL", H2EnumIndex(CAMPAIGN_CHOICE_SPELL));
    lua_setconst(
        L, "CAMPAIGN_CHOICE_SECONDARY_SKILL", H2EnumIndex(CAMPAIGN_CHOICE_SECONDARY_SKILL)
    );
    lua_setconst(L, "CAMPAIGN_CHOICE_ARMY", H2EnumIndex(CAMPAIGN_CHOICE_CREATURES));
    lua_setconst(L, "CAMPAIGN_CHOICE_PUZZLE_PIECES", H2EnumIndex(CAMPAIGN_CHOICE_PUZZLE_PIECES));
    lua_setconst(L, "CAMPAIGN_CHOICE_EXPERIENCE", H2EnumIndex(CAMPAIGN_CHOICE_EXPERIENCE));
    lua_setconst(L, "CAMPAIGN_CHOICE_NOT_AVAILABLE", H2EnumIndex(CAMPAIGN_CHOICE_NONE));
    lua_setconst(L, "CAMPAIGN_CHOICE_ALIGNMENT", H2EnumIndex(CAMPAIGN_CHOICE_ALIGNMENT));
    lua_setconst(L, "CAMPAIGN_CHOICE_PRIMARY_SKILL", H2EnumIndex(CAMPAIGN_CHOICE_PRIMARY_SKILL));
    lua_setconst(L, "CAMPAIGN_CHOICE_SPELL_SCROLL", H2EnumIndex(CAMPAIGN_CHOICE_SPELL_SCROLL));
}

/*****************************************************************************/

void set_scripting_consts(lua_State* L) {
    set_gui_consts(L);
    set_spell_consts(L);
    set_artifact_consts(L);
    set_town_consts(L);
    set_faction_consts(L);
    set_skill_consts(L);
    set_creature_consts(L);
    set_location_consts(L);
    set_barriercolor_consts(L);
    set_resources_consts(L);
    set_map_consts(L);
    set_campaign_choice_consts(L);
}
