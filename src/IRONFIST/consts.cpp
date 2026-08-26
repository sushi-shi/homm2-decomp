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

namespace ironfist::script {

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
    SetConstant(L, "DIALOG_OKAY", DIALOG_OKAY);
    SetConstant(L, "DIALOG_YES_NO", DIALOG_YES_NO);
    SetConstant(L, "DIALOG_CANCEL", DIALOG_CANCEL);
    SetConstant(L, "DIALOG_RIGHT_CLICK", DIALOG_RIGHT_CLICK);
    SetConstant(L, "DIALOG_LEARN_CHOICE", DIALOG_LEARN_CHOICE);

    SetConstant(L, "DIALOG_OR", DIALOG_OR);
    SetConstant(L, "DIALOG_EMPTY", DIALOG_EMPTY);
}

static void set_barriercolor_consts(lua_State* L) {
    SetConstant(L, "AQUA_BARRIER", 0);
}

static void set_messageboxgroups_consts(lua_State* L) {
    SetConstant(L, "IMAGE_EMPTY", IMAGE_EMPTY);
    SetConstant(L, "IMAGE_WOOD", IMAGE_WOOD);
    SetConstant(L, "IMAGE_MERCURY", IMAGE_MERCURY);
    SetConstant(L, "IMAGE_ORE", IMAGE_ORE);
    SetConstant(L, "IMAGE_SULFUR", IMAGE_SULFUR);
    SetConstant(L, "IMAGE_CRYSTALS", IMAGE_CRYSTALS);
    SetConstant(L, "IMAGE_GEMS", IMAGE_GEMS);
    SetConstant(L, "IMAGE_GOLD", IMAGE_GOLD);
    SetConstant(L, "IMAGE_GROUP_ARTIFACTS", IMAGE_GROUP_ARTIFACTS);
    SetConstant(L, "IMAGE_GROUP_SPELLS", IMAGE_GROUP_SPELLS);
    SetConstant(L, "IMAGE_GROUP_PLAYERS", IMAGE_GROUP_PLAYERS);
    SetConstant(L, "IMAGE_LUCK", IMAGE_LUCK);
    SetConstant(L, "IMAGE_BADLUCK", IMAGE_BADLUCK);
    SetConstant(L, "IMAGE_GOOD_MORALE", IMAGE_GOOD_MORALE);
    SetConstant(L, "IMAGE_BAD_MORALE", IMAGE_BAD_MORALE);
    SetConstant(L, "IMAGE_EXP", IMAGE_EXP);
    SetConstant(L, "IMAGE_GROUP_HERO", IMAGE_GROUP_HERO);
    SetConstant(L, "IMAGE_GROUP_SECONDARY_SKILLS", IMAGE_GROUP_SECONDARY_SKILLS);
    SetConstant(L, "IMAGE_GROUP_UNIT", IMAGE_GROUP_UNIT);
    SetConstant(L, "IMAGE_GROUP_PRIMARY_SKILLS", IMAGE_GROUP_PRIMARY_SKILLS);
}

static void set_gui_consts(lua_State* L) {
    set_dialog_consts(L);
    set_messageboxgroups_consts(L);
}

/*****************************************************************************/

static void set_spell_consts(lua_State* L) {
    SetConstant(L, "SPELL_FIREBALL", H2EnumIndex(SPELL_FIREBALL));
    SetConstant(L, "SPELL_FIREBLAST", H2EnumIndex(SPELL_FIREBLAST));
    SetConstant(L, "SPELL_LIGHTNING_BOLT", H2EnumIndex(SPELL_LIGHTNING_BOLT));
    SetConstant(L, "SPELL_CHAIN_LIGHTNING", H2EnumIndex(SPELL_CHAIN_LIGHTNING));
    SetConstant(L, "SPELL_TELEPORT", H2EnumIndex(SPELL_TELEPORT));
    SetConstant(L, "SPELL_CURE", H2EnumIndex(SPELL_CURE));
    SetConstant(L, "SPELL_MASS_CURE", H2EnumIndex(SPELL_MASS_CURE));
    SetConstant(L, "SPELL_RESURRECT", H2EnumIndex(SPELL_RESURRECT));
    SetConstant(L, "SPELL_RESURRECT_TRUE", H2EnumIndex(SPELL_TRUE_RESURRECT));
    SetConstant(L, "SPELL_HASTE", H2EnumIndex(SPELL_HASTE));
    SetConstant(L, "SPELL_MASS_HASTE", H2EnumIndex(SPELL_MASS_HASTE));
    SetConstant(L, "SPELL_SLOW", H2EnumIndex(SPELL_SLOW));
    SetConstant(L, "SPELL_MASS_SLOW", H2EnumIndex(SPELL_MASS_SLOW));
    SetConstant(L, "SPELL_BLIND", H2EnumIndex(SPELL_BLIND));
    SetConstant(L, "SPELL_BLESS", H2EnumIndex(SPELL_BLESS));
    SetConstant(L, "SPELL_MASS_BLESS", H2EnumIndex(SPELL_MASS_BLESS));
    SetConstant(L, "SPELL_STONESKIN", H2EnumIndex(SPELL_STONE_SKIN));
    SetConstant(L, "SPELL_STEELSKIN", H2EnumIndex(SPELL_STEEL_SKIN));
    SetConstant(L, "SPELL_CURSE", H2EnumIndex(SPELL_CURSE));
    SetConstant(L, "SPELL_MASS_CURSE", H2EnumIndex(SPELL_MASS_CURSE));
    SetConstant(L, "SPELL_HOLY_WORD", H2EnumIndex(SPELL_HOLY_WORD));
    SetConstant(L, "SPELL_HOLY_SHOUT", H2EnumIndex(SPELL_HOLY_SHOUT));
    SetConstant(L, "SPELL_ANTI_MAGIC", H2EnumIndex(SPELL_ANTI_MAGIC));
    SetConstant(L, "SPELL_DISPEL_MAGIC", H2EnumIndex(SPELL_DISPEL));
    SetConstant(L, "SPELL_MASS_DISPEL", H2EnumIndex(SPELL_MASS_DISPEL));
    SetConstant(L, "SPELL_MAGIC_ARROW", H2EnumIndex(SPELL_MAGIC_ARROW));
    SetConstant(L, "SPELL_BERZERKER", H2EnumIndex(SPELL_BERSERKER));
    SetConstant(L, "SPELL_ARMAGEDDON", H2EnumIndex(SPELL_ARMAGEDDON));
    SetConstant(L, "SPELL_ELEMENTAL_STORM", H2EnumIndex(SPELL_ELEMENTAL_STORM));
    SetConstant(L, "SPELL_METEOR_SHOWER", H2EnumIndex(SPELL_METEOR_SHOWER));
    SetConstant(L, "SPELL_PARALYZE", H2EnumIndex(SPELL_PARALYZE));
    SetConstant(L, "SPELL_HYPNOTIZE", H2EnumIndex(SPELL_HYPNOTIZE));
    SetConstant(L, "SPELL_COLD_RAY", H2EnumIndex(SPELL_COLD_RAY));
    SetConstant(L, "SPELL_COLD_RING", H2EnumIndex(SPELL_COLD_RING));
    SetConstant(L, "SPELL_DISRUPTING_RAY", H2EnumIndex(SPELL_DISRUPTING_RAY));
    SetConstant(L, "SPELL_DEATH_RIPPLE", H2EnumIndex(SPELL_DEATH_RIPPLE));
    SetConstant(L, "SPELL_DEATH_WAVE", H2EnumIndex(SPELL_DEATH_WAVE));
    SetConstant(L, "SPELL_DRAGON_SLAYER", H2EnumIndex(SPELL_DRAGON_SLAYER));
    SetConstant(L, "SPELL_BLOOD_LUST", H2EnumIndex(SPELL_BLOOD_LUST));
    SetConstant(L, "SPELL_ANIMATE_DEAD", H2EnumIndex(SPELL_ANIMATE_DEAD));
    SetConstant(L, "SPELL_MIRROR_IMAGE", H2EnumIndex(SPELL_MIRROR_IMAGE));
    SetConstant(L, "SPELL_SHIELD", H2EnumIndex(SPELL_SHIELD));
    SetConstant(L, "SPELL_MASS_SHIELD", H2EnumIndex(SPELL_MASS_SHIELD));
    SetConstant(L, "SPELL_SUMMON_EARTH_ELEMENTAL", H2EnumIndex(SPELL_SUMMON_EARTH_ELEMENTAL));
    SetConstant(L, "SPELL_SUMMON_AIR_ELEMENTAL", H2EnumIndex(SPELL_SUMMON_AIR_ELEMENTAL));
    SetConstant(L, "SPELL_SUMMON_FIRE_ELEMENTAL", H2EnumIndex(SPELL_SUMMON_FIRE_ELEMENTAL));
    SetConstant(L, "SPELL_SUMMON_WATER_ELEMENTAL", H2EnumIndex(SPELL_SUMMON_WATER_ELEMENTAL));
    SetConstant(L, "SPELL_EARTHQUAKE", H2EnumIndex(SPELL_EARTHQUAKE));
    SetConstant(L, "SPELL_VIEW_MINES", H2EnumIndex(SPELL_VIEW_MINES));
    SetConstant(L, "SPELL_VIEW_RESOURCES", H2EnumIndex(SPELL_VIEW_RESOURCES));
    SetConstant(L, "SPELL_VIEW_ARTIFACTS", H2EnumIndex(SPELL_VIEW_ARTIFACTS));
    SetConstant(L, "SPELL_VIEW_TOWNS", H2EnumIndex(SPELL_VIEW_TOWNS));
    SetConstant(L, "SPELL_VIEW_HEROES", H2EnumIndex(SPELL_VIEW_HEROES));
    SetConstant(L, "SPELL_VIEW_ALL", H2EnumIndex(SPELL_VIEW_ALL));
    SetConstant(L, "SPELL_IDENTIFY", H2EnumIndex(SPELL_IDENTIFY_HERO));
    SetConstant(L, "SPELL_SUMMON_BOAT", H2EnumIndex(SPELL_SUMMON_BOAT));
    SetConstant(L, "SPELL_DIMENSION_DOOR", H2EnumIndex(SPELL_DIMENSION_DOOR));
    SetConstant(L, "SPELL_TOWN_GATE", H2EnumIndex(SPELL_TOWN_GATE));
    SetConstant(L, "SPELL_TOWN_PORTAL", H2EnumIndex(SPELL_TOWN_PORTAL));
    SetConstant(L, "SPELL_VISIONS", H2EnumIndex(SPELL_VISIONS));
    SetConstant(L, "SPELL_HAUNT", H2EnumIndex(SPELL_HAUNT));
    SetConstant(L, "SPELL_SET_EARTH_GUARDIAN", H2EnumIndex(SPELL_SET_EARTH_GUARDIAN));
    SetConstant(L, "SPELL_SET_AIR_GUARDIAN", H2EnumIndex(SPELL_SET_AIR_GUARDIAN));
    SetConstant(L, "SPELL_SET_FIRE_GUARDIAN", H2EnumIndex(SPELL_SET_FIRE_GUARDIAN));
    SetConstant(L, "SPELL_SET_WATER_GUARDIAN", H2EnumIndex(SPELL_SET_WATER_GUARDIAN));
    // Ironfist-only spells retain their published Lua IDs.
    SetConstant(L, "SPELL_MARKSMAN_PIERCE", 67);
    SetConstant(L, "SPELL_PLASMA_CONE", 68);
    SetConstant(L, "SPELL_FORCE_SHIELD", 69);
    SetConstant(L, "SPELL_MASS_FORCE_SHIELD", 70);
    SetConstant(L, "SPELL_FIRE_BOMB", 71);
    SetConstant(L, "SPELL_IMPLOSION_GRENADE", 72);
}

static void set_artifact_consts(lua_State* L) {
    for (i32 i = 0; i < KB_ARTIFACT_TABLE_CAPACITY; ++i) {
        if (IsArtifactValid(i)) {
            const std::string constName = "ARTIFACT_" + LuaConstify(GetArtifactName(i));
            SetConstant(L, constName.c_str(), i);
        }
    }
}

static void set_town_consts(lua_State* L) {
    // Ironfist building codes are bit indexes into the town buildingsBuilt
    // mask; the port's TOWN_BUILDING_* constants are the masks themselves.
    SetConstant(L, "BUILDING_MAGE_GUILD", 0x0);
    SetConstant(L, "BUILDING_THIEVES_GUILD", 0x1);
    SetConstant(L, "BUILDING_TAVERN", 0x2);
    SetConstant(L, "BUILDING_DOCK", 0x3);
    SetConstant(L, "BUILDING_WELL", 0x4);
    SetConstant(L, "BUILDING_TENT", 0x5);
    SetConstant(L, "BUILDING_CASTLE", 0x6);
    SetConstant(L, "BUILDING_STATUE", 0x7);
    SetConstant(L, "BUILDING_LEFT_TURRET", 0x8);
    SetConstant(L, "BUILDING_RIGHT_TURRET", 0x9);
    SetConstant(L, "BUILDING_MARKET", 0xa);
    SetConstant(L, "BUILDING_SPECIAL_GROWTH", 0xb);
    SetConstant(L, "BUILDING_MOAT", 0xc);
    SetConstant(L, "BUILDING_SPECIAL", 0xd);
    SetConstant(L, "BUILDING_BOAT", 0xe);
    SetConstant(L, "BUILDING_CAPTAIN", 0xf);
    SetConstant(L, "BUILDING_DWELLING_1", 0x13);
    SetConstant(L, "BUILDING_DWELLING_2", 0x14);
    SetConstant(L, "BUILDING_DWELLING_3", 0x15);
    SetConstant(L, "BUILDING_DWELLING_4", 0x16);
    SetConstant(L, "BUILDING_DWELLING_5", 0x17);
    SetConstant(L, "BUILDING_DWELLING_6", 0x18);
    SetConstant(L, "BUILDING_UPGRADE_1", 0x19);
    SetConstant(L, "BUILDING_UPGRADE_2", 0x1a);
    SetConstant(L, "BUILDING_UPGRADE_3", 0x1b);
    SetConstant(L, "BUILDING_UPGRADE_4", 0x1c);
    SetConstant(L, "BUILDING_UPGRADE_5", 0x1d);
    SetConstant(L, "BUILDING_UPGRADE_5B", 0x1e);
}

static void set_faction_consts(lua_State* L) {
    SetConstant(L, "FACTION_KNIGHT", H2EnumIndex(FACTION_KNIGHT));
    SetConstant(L, "FACTION_BARBARIAN", H2EnumIndex(FACTION_BARBARIAN));
    SetConstant(L, "FACTION_SORCERESS", H2EnumIndex(FACTION_SORCERESS));
    SetConstant(L, "FACTION_WARLOCK", H2EnumIndex(FACTION_WARLOCK));
    SetConstant(L, "FACTION_WIZARD", H2EnumIndex(FACTION_WIZARD));
    SetConstant(L, "FACTION_NECROMANCER", H2EnumIndex(FACTION_NECROMANCER));
    SetConstant(L, "FACTION_MULTIPLE", H2EnumIndex(FACTION_NEUTRAL));
    SetConstant(L, "FACTION_RANDOM", H2EnumIndex(FACTION_RANDOM));

    // Ironfist-only faction retains its published Lua ID.
    SetConstant(L, "FACTION_CYBORG", H2EnumIndex(FACTION_CYBORG));
}

static void set_skill_consts(lua_State* L) {
    SetConstant(L, "PRIMARY_SKILL_ATTACK", H2EnumIndex(HERO_PRIMARY_ATTACK));
    SetConstant(L, "PRIMARY_SKILL_DEFENSE", H2EnumIndex(HERO_PRIMARY_DEFENSE));
    SetConstant(L, "PRIMARY_SKILL_SPELLPOWER", H2EnumIndex(HERO_PRIMARY_SPELL_POWER));
    SetConstant(L, "PRIMARY_SKILL_KNOWLEDGE", H2EnumIndex(HERO_PRIMARY_KNOWLEDGE));
    SetConstant(L, "SECONDARY_SKILL_PATHFINDING", H2EnumIndex(HERO_SKILL_PATHFINDING));
    SetConstant(L, "SECONDARY_SKILL_ARCHERY", H2EnumIndex(HERO_SKILL_ARCHERY));
    SetConstant(L, "SECONDARY_SKILL_LOGISTICS", H2EnumIndex(HERO_SKILL_LOGISTICS));
    SetConstant(L, "SECONDARY_SKILL_SCOUTING", H2EnumIndex(HERO_SKILL_SCOUTING));
    SetConstant(L, "SECONDARY_SKILL_DIPLOMACY", H2EnumIndex(HERO_SKILL_DIPLOMACY));
    SetConstant(L, "SECONDARY_SKILL_NAVIGATION", H2EnumIndex(HERO_SKILL_NAVIGATION));
    SetConstant(L, "SECONDARY_SKILL_LEADERSHIP", H2EnumIndex(HERO_SKILL_LEADERSHIP));
    SetConstant(L, "SECONDARY_SKILL_WISDOM", H2EnumIndex(HERO_SKILL_WISDOM));
    SetConstant(L, "SECONDARY_SKILL_MYSTICISM", H2EnumIndex(HERO_SKILL_MYSTICISM));
    SetConstant(L, "SECONDARY_SKILL_LUCK", H2EnumIndex(HERO_SKILL_LUCK));
    SetConstant(L, "SECONDARY_SKILL_BALLISTICS", H2EnumIndex(HERO_SKILL_BALLISTICS));
    SetConstant(L, "SECONDARY_SKILL_EAGLE_EYE", H2EnumIndex(HERO_SKILL_EAGLE_EYE));
    SetConstant(L, "SECONDARY_SKILL_NECROMANCY", H2EnumIndex(HERO_SKILL_NECROMANCY));
    SetConstant(L, "SECONDARY_SKILL_ESTATES", H2EnumIndex(HERO_SKILL_ESTATES));
}

static void set_creature_consts(lua_State* L) {
    SetConstant(L, "CREATURE_PEASANT", H2EnumIndex(CREATURE_PEASANT));
    SetConstant(L, "CREATURE_ARCHER", H2EnumIndex(CREATURE_ARCHER));
    SetConstant(L, "CREATURE_RANGER", H2EnumIndex(CREATURE_RANGER));
    SetConstant(L, "CREATURE_PIKEMAN", H2EnumIndex(CREATURE_PIKEMAN));
    SetConstant(L, "CREATURE_VETERAN_PIKEMAN", H2EnumIndex(CREATURE_VETERAN_PIKEMAN));
    SetConstant(L, "CREATURE_SWORDSMAN", H2EnumIndex(CREATURE_SWORDSMAN));
    SetConstant(L, "CREATURE_MASTER_SWORDSMAN", H2EnumIndex(CREATURE_MASTER_SWORDSMAN));
    SetConstant(L, "CREATURE_CAVALRY", H2EnumIndex(CREATURE_CAVALRY));
    SetConstant(L, "CREATURE_CHAMPION", H2EnumIndex(CREATURE_CHAMPION));
    SetConstant(L, "CREATURE_PALADIN", H2EnumIndex(CREATURE_PALADIN));
    SetConstant(L, "CREATURE_CRUSADER", H2EnumIndex(CREATURE_CRUSADER));
    SetConstant(L, "CREATURE_GOBLIN", H2EnumIndex(CREATURE_GOBLIN));
    SetConstant(L, "CREATURE_ORC", H2EnumIndex(CREATURE_ORC));
    SetConstant(L, "CREATURE_ORC_CHIEF", H2EnumIndex(CREATURE_ORC_CHIEF));
    SetConstant(L, "CREATURE_WOLF", H2EnumIndex(CREATURE_WOLF));
    SetConstant(L, "CREATURE_OGRE", H2EnumIndex(CREATURE_OGRE));
    SetConstant(L, "CREATURE_OGRE_LORD", H2EnumIndex(CREATURE_OGRE_LORD));
    SetConstant(L, "CREATURE_TROLL", H2EnumIndex(CREATURE_TROLL));
    SetConstant(L, "CREATURE_WAR_TROLL", H2EnumIndex(CREATURE_WAR_TROLL));
    SetConstant(L, "CREATURE_CYCLOPS", H2EnumIndex(CREATURE_CYCLOPS));
    SetConstant(L, "CREATURE_SPRITE", H2EnumIndex(CREATURE_SPRITE));
    SetConstant(L, "CREATURE_DWARF", H2EnumIndex(CREATURE_DWARF));
    SetConstant(L, "CREATURE_BATTLE_DWARF", H2EnumIndex(CREATURE_BATTLE_DWARF));
    SetConstant(L, "CREATURE_ELF", H2EnumIndex(CREATURE_ELF));
    SetConstant(L, "CREATURE_GRAND_ELF", H2EnumIndex(CREATURE_GRAND_ELF));
    SetConstant(L, "CREATURE_DRUID", H2EnumIndex(CREATURE_DRUID));
    SetConstant(L, "CREATURE_GREATER_DRUID", H2EnumIndex(CREATURE_GREATER_DRUID));
    SetConstant(L, "CREATURE_UNICORN", H2EnumIndex(CREATURE_UNICORN));
    SetConstant(L, "CREATURE_PHOENIX", H2EnumIndex(CREATURE_PHOENIX));
    SetConstant(L, "CREATURE_CENTAUR", H2EnumIndex(CREATURE_CENTAUR));
    SetConstant(L, "CREATURE_GARGOYLE", H2EnumIndex(CREATURE_GARGOYLE));
    SetConstant(L, "CREATURE_GRIFFIN", H2EnumIndex(CREATURE_GRIFFIN));
    SetConstant(L, "CREATURE_MINOTAUR", H2EnumIndex(CREATURE_MINOTAUR));
    SetConstant(L, "CREATURE_MINOTAUR_KING", H2EnumIndex(CREATURE_MINOTAUR_KING));
    SetConstant(L, "CREATURE_HYDRA", H2EnumIndex(CREATURE_HYDRA));
    SetConstant(L, "CREATURE_GREEN_DRAGON", H2EnumIndex(CREATURE_GREEN_DRAGON));
    SetConstant(L, "CREATURE_RED_DRAGON", H2EnumIndex(CREATURE_RED_DRAGON));
    SetConstant(L, "CREATURE_BLACK_DRAGON", H2EnumIndex(CREATURE_BLACK_DRAGON));
    SetConstant(L, "CREATURE_HALFLING", H2EnumIndex(CREATURE_HALFLING));
    SetConstant(L, "CREATURE_BOAR", H2EnumIndex(CREATURE_BOAR));
    SetConstant(L, "CREATURE_IRON_GOLEM", H2EnumIndex(CREATURE_IRON_GOLEM));
    SetConstant(L, "CREATURE_STEEL_GOLEM", H2EnumIndex(CREATURE_STEEL_GOLEM));
    SetConstant(L, "CREATURE_ROC", H2EnumIndex(CREATURE_ROC));
    SetConstant(L, "CREATURE_MAGE", H2EnumIndex(CREATURE_MAGE));
    SetConstant(L, "CREATURE_ARCHMAGE", H2EnumIndex(CREATURE_ARCHMAGE));
    SetConstant(L, "CREATURE_GIANT", H2EnumIndex(CREATURE_GIANT));
    SetConstant(L, "CREATURE_TITAN", H2EnumIndex(CREATURE_TITAN));
    SetConstant(L, "CREATURE_SKELETON", H2EnumIndex(CREATURE_SKELETON));
    SetConstant(L, "CREATURE_ZOMBIE", H2EnumIndex(CREATURE_ZOMBIE));
    SetConstant(L, "CREATURE_MUTANT_ZOMBIE", H2EnumIndex(CREATURE_MUTANT_ZOMBIE));
    SetConstant(L, "CREATURE_MUMMY", H2EnumIndex(CREATURE_MUMMY));
    SetConstant(L, "CREATURE_ROYAL_MUMMY", H2EnumIndex(CREATURE_ROYAL_MUMMY));
    SetConstant(L, "CREATURE_VAMPIRE", H2EnumIndex(CREATURE_VAMPIRE));
    SetConstant(L, "CREATURE_VAMPIRE_LORD", H2EnumIndex(CREATURE_VAMPIRE_LORD));
    SetConstant(L, "CREATURE_LICH", H2EnumIndex(CREATURE_LICH));
    SetConstant(L, "CREATURE_POWER_LICH", H2EnumIndex(CREATURE_POWER_LICH));
    SetConstant(L, "CREATURE_BONE_DRAGON", H2EnumIndex(CREATURE_BONE_DRAGON));
    SetConstant(L, "CREATURE_ROGUE", H2EnumIndex(CREATURE_ROGUE));
    SetConstant(L, "CREATURE_NOMAD", H2EnumIndex(CREATURE_NOMAD));
    SetConstant(L, "CREATURE_GHOST", H2EnumIndex(CREATURE_GHOST));
    SetConstant(L, "CREATURE_GENIE", H2EnumIndex(CREATURE_GENIE));
    SetConstant(L, "CREATURE_MEDUSA", H2EnumIndex(CREATURE_MEDUSA));
    SetConstant(L, "CREATURE_EARTH_ELEMENTAL", H2EnumIndex(CREATURE_EARTH_ELEMENTAL));
    SetConstant(L, "CREATURE_AIR_ELEMENTAL", H2EnumIndex(CREATURE_AIR_ELEMENTAL));
    SetConstant(L, "CREATURE_FIRE_ELEMENTAL", H2EnumIndex(CREATURE_FIRE_ELEMENTAL));
    SetConstant(L, "CREATURE_WATER_ELEMENTAL", H2EnumIndex(CREATURE_WATER_ELEMENTAL));
    // Ironfist-only creatures retain their published Lua IDs.
    SetConstant(L, "CREATURE_KOBOLD", 72);
    SetConstant(L, "CREATURE_BLOODSUCKER", 73);
    SetConstant(L, "CREATURE_HARPY", 74);
    SetConstant(L, "CREATURE_BLACK_KNIGHT", 75);
    SetConstant(L, "CREATURE_CATOBLEBA", 76);
    SetConstant(L, "CREATURE_TREANT", 77);
    SetConstant(L, "CREATURE_CYBER_KOBOLD_SPEARMAN", 78);
    SetConstant(L, "CREATURE_CYBER_PLASMA_BERSERKER", 79);
    SetConstant(L, "CREATURE_CYBER_PLASMA_LANCER", 80);
    SetConstant(L, "CREATURE_CYBER_INDIGO_PANTHER", 81);
    SetConstant(L, "CREATURE_CYBER_SHADOW_ASSASSIN", 82);
    SetConstant(L, "CREATURE_CYBER_BEHEMOTH", 83);
}

static void set_resources_consts(lua_State* L) {
    SetConstant(L, "RESOURCE_WOOD", H2EnumIndex(RES_WOOD));
    SetConstant(L, "RESOURCE_MERCURY", H2EnumIndex(RES_MERCURY));
    SetConstant(L, "RESOURCE_ORE", H2EnumIndex(RES_ORE));
    SetConstant(L, "RESOURCE_SULFUR", H2EnumIndex(RES_SULFUR));
    SetConstant(L, "RESOURCE_CRYSTALS", H2EnumIndex(RES_CRYSTAL));
    SetConstant(L, "RESOURCE_GEMS", H2EnumIndex(RES_GEMS));
    SetConstant(L, "RESOURCE_GOLD", H2EnumIndex(RES_GOLD));
}

static void set_location_consts(lua_State* L) {
    SetConstant(L, "LOCATION_ALCHEMIST_LAB", H2EnumIndex(MAP_OBJECT_ALCHEMIST_LAB));
    SetConstant(L, "LOCATION_SIGN", H2EnumIndex(MAP_OBJECT_SIGN));
    SetConstant(L, "LOCATION_BUOY", H2EnumIndex(MAP_OBJECT_BUOY));
    SetConstant(L, "LOCATION_SKELETON", H2EnumIndex(MAP_OBJECT_SKELETON));
    SetConstant(L, "LOCATION_DAEMON_CAVE", H2EnumIndex(MAP_OBJECT_DAEMON_CAVE));
    SetConstant(L, "LOCATION_TREASURE_CHEST", H2EnumIndex(MAP_OBJECT_TREASURE_CHEST));
    SetConstant(L, "LOCATION_FAERIE_RING", H2EnumIndex(MAP_OBJECT_FAERIE_RING));
    SetConstant(L, "LOCATION_CAMPFIRE", H2EnumIndex(MAP_OBJECT_CAMPFIRE));
    SetConstant(L, "LOCATION_FOUNTAIN", H2EnumIndex(MAP_OBJECT_FOUNTAIN));
    SetConstant(L, "LOCATION_GAZEBO", H2EnumIndex(MAP_OBJECT_GAZEBO));
    SetConstant(L, "LOCATION_ANCIENT_LAMP", H2EnumIndex(MAP_OBJECT_ANCIENT_LAMP));
    SetConstant(L, "LOCATION_GRAVEYARD", H2EnumIndex(MAP_OBJECT_GRAVEYARD));
    SetConstant(L, "LOCATION_ARCHERS_HOUSE", H2EnumIndex(MAP_OBJECT_ARCHER_HOUSE));
    SetConstant(L, "LOCATION_GOBLIN_HUT", H2EnumIndex(MAP_OBJECT_GOBLIN_HUT));
    SetConstant(L, "LOCATION_DWARF_COTTAGE", H2EnumIndex(MAP_OBJECT_DWARF_COTTAGE));
    SetConstant(L, "LOCATION_PEASANT_HUT", H2EnumIndex(MAP_OBJECT_PEASANT_HUT));
    SetConstant(L, "LOCATION_LOG_CABIN", H2EnumIndex(MAP_OBJECT_LOG_CABIN));
    // Ironfist's names for 18 and 19 predate the recovered ones.
    SetConstant(L, "LOCATION_ROAD", H2EnumIndex(MAP_OBJECT_ALCHEMIST_TOWER));
    SetConstant(L, "LOCATION_EVENT", H2EnumIndex(MAP_OBJECT_MAP_EVENT));
    SetConstant(L, "LOCATION_DRAGON_CITY", H2EnumIndex(MAP_OBJECT_DRAGON_CITY));
    SetConstant(L, "LOCATION_LIGHTHOUSE", H2EnumIndex(MAP_OBJECT_LIGHTHOUSE));
    SetConstant(L, "LOCATION_WATERWHEEL", H2EnumIndex(MAP_OBJECT_WATER_WHEEL));
    SetConstant(L, "LOCATION_MINE", H2EnumIndex(MAP_OBJECT_MINE));
    SetConstant(L, "LOCATION_ARMY_CAMP", H2EnumIndex(MAP_OBJECT_MONSTER));
    SetConstant(L, "LOCATION_OBELISK", H2EnumIndex(MAP_OBJECT_OBELISK));
    SetConstant(L, "LOCATION_OASIS", H2EnumIndex(MAP_OBJECT_OASIS));
    SetConstant(L, "LOCATION_RESOURCE", H2EnumIndex(MAP_OBJECT_RESOURCE));
    SetConstant(L, "LOCATION_SAWMILL", H2EnumIndex(MAP_OBJECT_SAWMILL));
    SetConstant(L, "LOCATION_ORACLE", H2EnumIndex(MAP_OBJECT_ORACLE));
    SetConstant(L, "LOCATION_SHRINE_FIRST_ORDER", H2EnumIndex(MAP_OBJECT_SHRINE_FIRST_CIRCLE));
    SetConstant(L, "LOCATION_SHIPWRECK", H2EnumIndex(MAP_OBJECT_SHIPWRECK));
    SetConstant(L, "LOCATION_SEA_CHEST", H2EnumIndex(MAP_OBJECT_SEA_CHEST));
    SetConstant(L, "LOCATION_DESERT_TENT", H2EnumIndex(MAP_OBJECT_DESERT_TENT));
    SetConstant(L, "LOCATION_TOWN", H2EnumIndex(MAP_OBJECT_CASTLE));
    SetConstant(L, "LOCATION_STONE_LITHS", H2EnumIndex(MAP_OBJECT_STONE_LITHS));
    SetConstant(L, "LOCATION_WAGON_CAMP", H2EnumIndex(MAP_OBJECT_WAGON_CAMP));
    SetConstant(L, "LOCATION_WELL", H2EnumIndex(MAP_OBJECT_HUT_OF_MAGI));
    SetConstant(L, "LOCATION_WHIRLPOOL", H2EnumIndex(MAP_OBJECT_WHIRLPOOL));
    SetConstant(L, "LOCATION_WINDMILL", H2EnumIndex(MAP_OBJECT_WINDMILL));
    SetConstant(L, "LOCATION_ARTIFACT", H2EnumIndex(MAP_OBJECT_ARTIFACT));
    SetConstant(L, "LOCATION_HERO", H2EnumIndex(MAP_OBJECT_MERMAID));
    SetConstant(L, "LOCATION_BOAT", H2EnumIndex(MAP_OBJECT_BOAT));
    SetConstant(L, "LOCATION_RANDOM_ARTIFACT", H2EnumIndex(MAP_OBJECT_RANDOM_ARTIFACT));
    SetConstant(L, "LOCATION_RANDOM_RESOURCE", H2EnumIndex(MAP_OBJECT_RANDOM_RESOURCE));
    SetConstant(L, "LOCATION_RANDOM_MONSTER", H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER));
    SetConstant(L, "LOCATION_RANDOM_TOWN", H2EnumIndex(MAP_OBJECT_RANDOM_TOWN));
    SetConstant(L, "LOCATION_RANDOM_CASTLE", H2EnumIndex(MAP_OBJECT_RANDOM_CASTLE));
    SetConstant(L, "LOCATION_RANDOM_MONSTER_WEAK", H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER_WEAK));
    SetConstant(
        L, "LOCATION_RANDOM_MONSTER_MEDIUM", H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER_MEDIUM)
    );
    SetConstant(
        L, "LOCATION_RANDOM_MONSTER_STRONG", H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER_STRONG)
    );
    SetConstant(
        L, "LOCATION_RANDOM_MONSTER_VERY_STRONG",
        H2EnumIndex(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG)
    );
    SetConstant(L, "LOCATION_RANDOM_HERO", H2EnumIndex(MAP_OBJECT_HERO));
    SetConstant(L, "LOCATION_NOTHING_SPECIAL", H2EnumIndex(MAP_OBJECT_NOTHING_SPECIAL));
    SetConstant(L, "LOCATION_WATCH_TOWER", H2EnumIndex(MAP_OBJECT_WATCH_TOWER));
    SetConstant(L, "LOCATION_TREE_HOUSE", H2EnumIndex(MAP_OBJECT_TREE_HOUSE));
    SetConstant(L, "LOCATION_TREE_CITY", H2EnumIndex(MAP_OBJECT_TREE_CITY));
    SetConstant(L, "LOCATION_RUINS", H2EnumIndex(MAP_OBJECT_RUINS));
    SetConstant(L, "LOCATION_FORT", H2EnumIndex(MAP_OBJECT_FORT));
    SetConstant(L, "LOCATION_TRADING_POST", H2EnumIndex(MAP_OBJECT_TRADING_POST));
    SetConstant(L, "LOCATION_ABANDONED_MINE", H2EnumIndex(MAP_OBJECT_ABANDONED_MINE));
    SetConstant(L, "LOCATION_DWARF_CABIN", H2EnumIndex(MAP_OBJECT_SIRENS));
    SetConstant(L, "LOCATION_STANDING_STONES", H2EnumIndex(MAP_OBJECT_STANDING_STONES));
    SetConstant(L, "LOCATION_IDOL", H2EnumIndex(MAP_OBJECT_IDOL));
    SetConstant(L, "LOCATION_TREE_OF_KNOWLEDGE", H2EnumIndex(MAP_OBJECT_TREE_OF_KNOWLEDGE));
    SetConstant(L, "LOCATION_WITCH_DOCTORS_HUT", H2EnumIndex(MAP_OBJECT_WITCH_DOCTOR_HUT));
    SetConstant(L, "LOCATION_TEMPLE", H2EnumIndex(MAP_OBJECT_TEMPLE));
    SetConstant(L, "LOCATION_HILL_FORT", H2EnumIndex(MAP_OBJECT_HILL_FORT));
    SetConstant(L, "LOCATION_HALFLING_HOLE", H2EnumIndex(MAP_OBJECT_HALFLING_HOLE));
    SetConstant(L, "LOCATION_MERCENARY_CAMP", H2EnumIndex(MAP_OBJECT_MERCENARY_CAMP));
    SetConstant(L, "LOCATION_SHRINE_SECOND_ORDER", H2EnumIndex(MAP_OBJECT_SHRINE_SECOND_CIRCLE));
    SetConstant(L, "LOCATION_SHRINE_THIRD_ORDER", H2EnumIndex(MAP_OBJECT_SHRINE_THIRD_CIRCLE));
    SetConstant(L, "LOCATION_PYRAMID", H2EnumIndex(MAP_OBJECT_PYRAMID));
    SetConstant(L, "LOCATION_CITY_OF_DEAD", H2EnumIndex(MAP_OBJECT_CITY_OF_DEAD));
    SetConstant(L, "LOCATION_EXCAVATION", H2EnumIndex(MAP_OBJECT_EXCAVATION));
    SetConstant(L, "LOCATION_SPHINX", H2EnumIndex(MAP_OBJECT_SPHINX));
    SetConstant(L, "LOCATION_WAGON", H2EnumIndex(MAP_OBJECT_WAGON));
    SetConstant(L, "LOCATION_TAR_PIT", H2EnumIndex(MAP_OBJECT_TAR_PIT));
    SetConstant(L, "LOCATION_ARTESIAN_SPRING", H2EnumIndex(MAP_OBJECT_ARTESIAN_SPRING));
    SetConstant(L, "LOCATION_TROLL_BRIDGE", H2EnumIndex(MAP_OBJECT_TROLL_BRIDGE));
    SetConstant(L, "LOCATION_WATERING_HOLE", H2EnumIndex(MAP_OBJECT_WATERING_HOLE));
    SetConstant(L, "LOCATION_WITCH_HUT", H2EnumIndex(MAP_OBJECT_WITCH_HUT));
    SetConstant(L, "LOCATION_XANADU", H2EnumIndex(MAP_OBJECT_XANADU));
    SetConstant(L, "LOCATION_CAVE", H2EnumIndex(MAP_OBJECT_CAVE));
    SetConstant(L, "LOCATION_LEAN_TO", H2EnumIndex(MAP_OBJECT_LEAN_TO));
    SetConstant(L, "LOCATION_MAGELLANS_MAPS", H2EnumIndex(MAP_OBJECT_MAGELLAN_MAPS));
    SetConstant(L, "LOCATION_FLOTSAM", H2EnumIndex(MAP_OBJECT_FLOTSAM));
    SetConstant(L, "LOCATION_DERELICT_SHIP", H2EnumIndex(MAP_OBJECT_DERELICT_SHIP));
    SetConstant(L, "LOCATION_SHIPWRECK_SURVIVOR", H2EnumIndex(MAP_OBJECT_SHIPWRECK_SURVIVOR));
    SetConstant(L, "LOCATION_BOTTLE", H2EnumIndex(MAP_OBJECT_BOTTLE));
    SetConstant(L, "LOCATION_MAGIC_WELL", H2EnumIndex(MAP_OBJECT_MAGIC_WELL));
    SetConstant(L, "LOCATION_MAGIC_GARDEN", H2EnumIndex(MAP_OBJECT_MAGIC_GARDEN));
    SetConstant(L, "LOCATION_OBSERVATION_TOWER", H2EnumIndex(MAP_OBJECT_OBSERVATION_TOWER));
    SetConstant(L, "LOCATION_FREEMANS_FOUNDRY", H2EnumIndex(MAP_OBJECT_FREEMANS_FOUNDRY));
    SetConstant(L, "LOCATION_STREAM", H2EnumIndex(MAP_OBJECT_REEFS));
    SetConstant(L, "LOCATION_TREES", H2EnumIndex(MAP_OBJECT_TREES));
    SetConstant(L, "LOCATION_MOUNTAINS", H2EnumIndex(MAP_OBJECT_MOUNTAINS));
    SetConstant(L, "LOCATION_VOLCANO", H2EnumIndex(MAP_OBJECT_VOLCANO));
    SetConstant(L, "LOCATION_FLOWERS", H2EnumIndex(MAP_OBJECT_FLOWERS));
    SetConstant(L, "LOCATION_ROCK", H2EnumIndex(MAP_OBJECT_ROCK));
    SetConstant(L, "LOCATION_LAKE", H2EnumIndex(MAP_OBJECT_WATER_LAKE));
    SetConstant(L, "LOCATION_MANDRAKE", H2EnumIndex(MAP_OBJECT_MANDRAKE));
    SetConstant(L, "LOCATION_DEAD_TREE", H2EnumIndex(MAP_OBJECT_DEAD_TREE));
    SetConstant(L, "LOCATION_STUMP", H2EnumIndex(MAP_OBJECT_STUMP));
    SetConstant(L, "LOCATION_CRATER", H2EnumIndex(MAP_OBJECT_CRATER));
    SetConstant(L, "LOCATION_CACTUS", H2EnumIndex(MAP_OBJECT_CACTUS));
    SetConstant(L, "LOCATION_MOUND", H2EnumIndex(MAP_OBJECT_MOUND));
    SetConstant(L, "LOCATION_DUNE", H2EnumIndex(MAP_OBJECT_DUNE));
    SetConstant(L, "LOCATION_LAVA_POOL", H2EnumIndex(MAP_OBJECT_LAVA_POOL));
    SetConstant(L, "LOCATION_SHRUB", H2EnumIndex(MAP_OBJECT_SHRUB));
    SetConstant(L, "LOCATION_HOLE", H2EnumIndex(MAP_OBJECT_ARENA));
    SetConstant(L, "LOCATION_OUTCROPPING", H2EnumIndex(MAP_OBJECT_BARROW_MOUNDS));
    SetConstant(
        L, "LOCATION_RANDOM_ARTIFACT_TREASURE", H2EnumIndex(MAP_OBJECT_RANDOM_TREASURE_ARTIFACT)
    );
    SetConstant(
        L, "LOCATION_RANDOM_ARTIFACT_MINOR", H2EnumIndex(MAP_OBJECT_RANDOM_MINOR_ARTIFACT)
    );
    SetConstant(
        L, "LOCATION_RANDOM_ARTIFACT_MAJOR", H2EnumIndex(MAP_OBJECT_RANDOM_MAJOR_ARTIFACT)
    );
    SetConstant(L, "LOCATION_BARRIER", H2EnumIndex(MAP_OBJECT_BARRIER));
    SetConstant(L, "LOCATION_TRAVELLER_TENT", H2EnumIndex(MAP_OBJECT_TRAVELER_TENT));
    SetConstant(L, "LOCATION_EXPANSION_DWELLING", H2EnumIndex(MAP_OBJECT_EXPANSION_DWELLING));
    SetConstant(L, "LOCATION_ALCHEMIST_TOWER", H2EnumIndex(MAP_OBJECT_EXPANSION_OBJECT));
    SetConstant(L, "LOCATION_JAIL", H2EnumIndex(MAP_OBJECT_JAIL));
}

static void set_map_cell_consts(lua_State* L) {
    SetConstant(L, "MAP_CELL_NO_FLIP", 0);
    SetConstant(L, "MAP_CELL_FLIP_VERTICALLY", 0x1);
    SetConstant(L, "MAP_CELL_FLIP_HORIZONTALLY", 0x2);
    SetConstant(L, "MAP_CELL_FLIP_DIAGONALLY", 0x3);
}

static void set_tooltip_consts(lua_State* L) {
    SetNilConstant(L, "TOOLTIP_DEFAULT");
}

static void set_map_consts(lua_State* L) {
    set_location_consts(L);
    set_map_cell_consts(L);
    set_tooltip_consts(L);
}

static void set_campaign_choice_consts(lua_State* L) {
    SetConstant(L, "CAMPAIGN_CHOICE_RESOURCE", H2EnumIndex(CAMPAIGN_CHOICE_RESOURCE));
    SetConstant(L, "CAMPAIGN_CHOICE_ARTIFACT", H2EnumIndex(CAMPAIGN_CHOICE_ARTIFACT));
    SetConstant(L, "CAMPAIGN_CHOICE_SPELL", H2EnumIndex(CAMPAIGN_CHOICE_SPELL));
    SetConstant(
        L, "CAMPAIGN_CHOICE_SECONDARY_SKILL", H2EnumIndex(CAMPAIGN_CHOICE_SECONDARY_SKILL)
    );
    SetConstant(L, "CAMPAIGN_CHOICE_ARMY", H2EnumIndex(CAMPAIGN_CHOICE_CREATURES));
    SetConstant(L, "CAMPAIGN_CHOICE_PUZZLE_PIECES", H2EnumIndex(CAMPAIGN_CHOICE_PUZZLE_PIECES));
    SetConstant(L, "CAMPAIGN_CHOICE_EXPERIENCE", H2EnumIndex(CAMPAIGN_CHOICE_EXPERIENCE));
    SetConstant(L, "CAMPAIGN_CHOICE_NOT_AVAILABLE", H2EnumIndex(CAMPAIGN_CHOICE_NONE));
    SetConstant(L, "CAMPAIGN_CHOICE_ALIGNMENT", H2EnumIndex(CAMPAIGN_CHOICE_ALIGNMENT));
    SetConstant(L, "CAMPAIGN_CHOICE_PRIMARY_SKILL", H2EnumIndex(CAMPAIGN_CHOICE_PRIMARY_SKILL));
    SetConstant(L, "CAMPAIGN_CHOICE_SPELL_SCROLL", H2EnumIndex(CAMPAIGN_CHOICE_SPELL_SCROLL));
}

/*****************************************************************************/

void RegisterConstants(lua_State* L) {
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

} // namespace ironfist::script
