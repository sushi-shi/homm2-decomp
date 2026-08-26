#include <IRONFIST/townconsts.h>

#include <string>
#include <vector>

#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist {

struct BuildingCost {
    i32 wood;
    i32 mercury;
    i32 ore;
    i32 sulfur;
    i32 crystal;
    i32 gems;
    i32 gold;
};

static CreatureType dwellingTypes[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT] = {};

static std::vector<std::string> firstLevelGrowerNames = {
    "Farm",
    "Garbage Heap",
    "Crystal Garden",
    "Waterfall",
    "Orchard",
    "Skull Pile",
    "1st Lvl Growth",
    "",
    "",
    "",
    "",
    "",
    "Kobold Hatchery"
  };

static std::vector<std::string> specialBuildingNames = {
    "Fortifications",
    "Coliseum",
    "Rainbow",
    "Dungeon",
    "Library",
    "Storm",
    "Special",
    "",
    "",
    "",
    "",
    "",
    "Radiation Tower"
  };

static std::vector<std::string> specialBuildingDescriptions = {
    "The Fortifications increase the toughness of the walls, increasing the number of turns it takes to knock them down.",
    "The Coliseum provides inspiring spectacles to defending troops, raising their morale by two during combat.",
    "The Rainbow increases the luck of the defending units by two.",
    "The Dungeon increases the income of the town by 500 / day.",
    "The Library increases the number of spells in the Guild by one for each level of the guild.",
    "The Storm adds +2 to the power of spells of a defending spell caster.",
    "",
    "",
    "",
    "",
    "",
    "",
    "Lowers luck of enemies"
  };

static std::vector<std::string> cyborgDwellings = {
    "Sewer",
    "Meeting Hall",
    "Arena",
    "Hunting Grounds",
    "Dojo",
    "Nexus",
    "Cyborg Level 2 Upgrade",
    "Cyborg Level 3 Upgrade",
    "Cyborg Level 4 Upgrade",
    "Cyborg Level 5 Upgrade",
    "Cyborg Level 6 Upgrade",
    // Only Warlocks use a second tier-six upgrade. Keep the shared table's
    // final slot valid for Cyborg lookups even though the building is disabled.
    "Cyborg Level 6 Upgrade"
  };

static BuildingCost specialBuildingCosts[KB_FACTION_TABLE_CAPACITY] = {
  {5, 0, 15, 0, 0, 0, 1500},
  {10, 0, 10, 0, 0, 0, 2000},
  {0, 0, 0, 0, 10, 0, 1500},
  {5, 0, 10, 0, 0, 0, 3000},
  {5, 5, 5, 5, 5, 5, 1500},
  {0, 10, 0, 10, 0, 0, 1000},
  {0},
  {0},
  {0},
  {0},
  {0},
  {0},
  // Ironfist gives the Cyborg special building the Knight cost profile.
  {5, 0, 15, 0, 0, 0, 1500}
};

static BuildingCost dwellingCosts[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT] = {};

static void InitDwellingCosts() {
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 200 };
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3] = { 0, 0, 5, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4] = {10, 0, 10, 0, 0, 0, 2000};
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5] = {20, 0, 0, 0, 0, 0, 3000};
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6] = {20, 0, 0, 0, 20, 0, 5000};
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2_UPGRADE] = {5, 0, 0, 0, 0, 0, 1500};
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3_UPGRADE] = {0, 0, 5, 0, 0, 0, 1500};
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4_UPGRADE] = {5, 0, 5, 0, 0, 0, 2000};
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5_UPGRADE] = {10, 0, 0, 0, 0, 0, 3000};
  dwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6_UPGRADE] = {10, 0, 0, 0, 10, 0, 5000};

  dwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 300 };
  dwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2] = { 5, 0, 0, 0, 0, 0, 800 };
  dwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_3] = {0, 0, 0, 0, 0, 0, 1000};
  dwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4] = {10, 0, 10, 0, 0, 0, 2000};
  dwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5] = {0, 0, 20, 0, 0, 0, 4000};
  dwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_6] = {0, 0, 20, 0, 20, 0, 6000};
  dwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2_UPGRADE] = {5, 0, 0, 0, 0, 0, 1200};
  dwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4_UPGRADE] = {5, 0, 5, 0, 0, 0, 3000};
  dwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5_UPGRADE] = {0, 0, 10, 0, 0, 0, 2000};

  dwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_1] = { 5, 0, 0, 0, 0, 0, 500 };
  dwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2] = { 5, 0, 0, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3] = { 0, 0, 0, 0, 0, 0, 1500 };
  dwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4] = { 0, 0, 10, 0, 0, 0, 2500 };
  dwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_5] = {10, 0, 0, 0, 0, 10, 3000 };
  dwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_6] = { 0, 20, 30, 0, 0, 0, 10000 };
  dwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2_UPGRADE] = { 5, 0, 0, 0, 0, 0, 1500 };
  dwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3_UPGRADE] = { 5, 0, 0, 0, 0, 0, 1500 };
  dwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4_UPGRADE] = { 0, 5, 0, 0, 0, 0, 1500 };

  dwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 500 };
  dwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_2] = { 0, 0, 10, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_3] = { 0, 0, 0, 0, 0, 0, 2000 };
  dwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4] = { 0, 0, 0, 0, 0, 10, 3000 };
  dwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_5] = { 0, 0, 0, 10, 0, 0, 4000 };
  dwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6] = { 0, 0, 30, 20, 0, 0, 15000 };
  dwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4_UPGRADE] = { 0, 0, 0, 0, 0, 5, 2000 };
  dwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE] = { 0, 0, 5, 10, 0, 0, 5000 };
  dwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE2] = { 0, 0, 5, 10, 0, 0, 5000 };

  dwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 400 };
  dwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 800 };
  dwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_3] = { 5, 0, 5, 0, 0, 0, 1500 };
  dwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_4] = { 5, 0, 0, 0, 0, 0, 3000 };
  dwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_5] = { 5, 5, 5, 5, 5, 5, 3500 };
  dwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_6] = { 5, 0, 5, 0, 0, 20, 12500 };
  dwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_3_UPGRADE] = { 0, 5, 0, 0, 0, 0, 1500 };
  dwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_5_UPGRADE] = { 5, 0, 5, 0, 0, 0, 4000 };
  dwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_6_UPGRADE] = { 5, 0, 5, 0, 0, 20, 12500 };

  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 400 };
  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3] = { 0, 0, 10, 0, 0, 0, 1500 };
  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4] = { 10, 0, 0, 0, 0, 0, 3000 };
  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5] = { 10, 0, 0, 10, 0, 0, 4000 };
  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_6] = { 10, 5, 10, 5, 5, 5, 10000 };
  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2_UPGRADE] = { 0, 0, 0, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3_UPGRADE] = { 0, 0, 5, 0, 0, 0, 1500 };
  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4_UPGRADE] = { 5, 0, 0, 0, 10, 10, 4000 };
  dwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5_UPGRADE] = { 0, 0, 5, 0, 5, 0, 3000 };

  // Cyborg base dwellings use the Knight cost profile. The shared upgrade
  // slots remain populated for table completeness, but their build-mask bits
  // are disabled.
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 200 };
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_3] = { 0, 0, 5, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_4] = {10, 0, 10, 0, 0, 0, 2000};
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_5] = {20, 0, 0, 0, 0, 0, 3000};
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_6] = {20, 0, 0, 0, 20, 0, 5000};
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_2_UPGRADE] = { 0, 0, 0, 0, 0, 0, 200 };
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_3_UPGRADE] = { 0, 0, 0, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_4_UPGRADE] = { 0, 0, 5, 0, 0, 0, 1000 };
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_5_UPGRADE] = {10, 0, 10, 0, 0, 0, 2000};
  dwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_6_UPGRADE] = {20, 0, 0, 0, 0, 0, 3000};
}

static void InitDwellingTypes() {
  for (i32 i = 0; i < KB_FACTION_TABLE_CAPACITY; ++i) {
    for (i32 j = 0; j < KB_DWELLING_TYPE_COUNT; ++j) {
      dwellingTypes[i][j] = CREATURE_NONE;
    }
  }

  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_1] = CREATURE_PEASANT;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2] = CREATURE_ARCHER;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3] = CREATURE_PIKEMAN;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4] = CREATURE_SWORDSMAN;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5] = CREATURE_CAVALRY;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6] = CREATURE_PALADIN;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2_UPGRADE] = CREATURE_RANGER;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3_UPGRADE] = CREATURE_VETERAN_PIKEMAN;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4_UPGRADE] = CREATURE_MASTER_SWORDSMAN;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5_UPGRADE] = CREATURE_CHAMPION;
  dwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6_UPGRADE] = CREATURE_CRUSADER;

  dwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_1] = CREATURE_GOBLIN;
  dwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2] = CREATURE_ORC;
  dwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_3] = CREATURE_WOLF;
  dwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4] = CREATURE_OGRE;
  dwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5] = CREATURE_TROLL;
  dwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_6] = CREATURE_CYCLOPS;
  dwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2_UPGRADE] = CREATURE_ORC_CHIEF;
  dwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4_UPGRADE] = CREATURE_OGRE_LORD;
  dwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5_UPGRADE] = CREATURE_WAR_TROLL;

  dwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_1] = CREATURE_SPRITE;
  dwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2] = CREATURE_DWARF;
  dwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3] = CREATURE_ELF;
  dwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4] = CREATURE_DRUID;
  dwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_5] = CREATURE_UNICORN;
  dwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_6] = CREATURE_PHOENIX;
  dwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2_UPGRADE] = CREATURE_BATTLE_DWARF;
  dwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3_UPGRADE] = CREATURE_GRAND_ELF;
  dwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4_UPGRADE] = CREATURE_GREATER_DRUID;

  dwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_1] = CREATURE_CENTAUR;
  dwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_2] = CREATURE_GARGOYLE;
  dwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_3] = CREATURE_GRIFFIN;
  dwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4] = CREATURE_MINOTAUR;
  dwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_5] = CREATURE_HYDRA;
  dwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6] = CREATURE_GREEN_DRAGON;
  dwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4_UPGRADE] = CREATURE_MINOTAUR_KING;
  dwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE] = CREATURE_RED_DRAGON;
  dwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE2] = CREATURE_BLACK_DRAGON;

  dwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_1] = CREATURE_HALFLING;
  dwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_2] = CREATURE_BOAR;
  dwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_3] = CREATURE_IRON_GOLEM;
  dwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_4] = CREATURE_ROC;
  dwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_5] = CREATURE_MAGE;
  dwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_6] = CREATURE_GIANT;
  dwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_3_UPGRADE] = CREATURE_STEEL_GOLEM;
  dwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_5_UPGRADE] = CREATURE_ARCHMAGE;
  dwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_6_UPGRADE] = CREATURE_TITAN;

  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_1] = CREATURE_SKELETON;
  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2] = CREATURE_ZOMBIE;
  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3] = CREATURE_MUMMY;
  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4] = CREATURE_VAMPIRE;
  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5] = CREATURE_LICH;
  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_6] = CREATURE_BONE_DRAGON;
  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2_UPGRADE] = CREATURE_MUTANT_ZOMBIE;
  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3_UPGRADE] = CREATURE_ROYAL_MUMMY;
  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4_UPGRADE] = CREATURE_VAMPIRE_LORD;
  dwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5_UPGRADE] = CREATURE_POWER_LICH;

  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_1] = CREATURE_CYBER_KOBOLD_SPEARMAN;
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_2] = CREATURE_CYBER_PLASMA_BERSERKER;
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_3] = CREATURE_CYBER_PLASMA_LANCER;
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_4] = CREATURE_CYBER_INDIGO_PANTHER;
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_5] = CREATURE_CYBER_SHADOW_ASSASSIN;
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_6] = CREATURE_CYBER_BEHEMOTH;
  // Cyborgs have no upgraded creature variants, so disabled upgrade slots
  // resolve to their corresponding base creatures.
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_2_UPGRADE] = CREATURE_CYBER_PLASMA_BERSERKER;
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_3_UPGRADE] = CREATURE_CYBER_PLASMA_LANCER;
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_4_UPGRADE] = CREATURE_CYBER_INDIGO_PANTHER;
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_5_UPGRADE] = CREATURE_CYBER_SHADOW_ASSASSIN;
  dwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_6_UPGRADE] = CREATURE_CYBER_BEHEMOTH;
}

static u32 townEligibleBuildMasks[KB_FACTION_TABLE_CAPACITY] = {
  0x3FF8BF9F,
  0x1BF8BF9F,
  0xFF8BF9F,
  0x69F8BF9F,
  0x35F8BF9F,
  0x1FF8BF9B,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x01F8BF9F  // Cyborg upgrade-dwelling bits are intentionally disabled.
};

static const char* townPrefixNames[KB_FACTION_TABLE_CAPACITY] = {
  "twnk", "twnb", "twns", "twnw", "twnz", "twnn",
  "", "", "", "", "", "",
  "twnc"
};

// The Cybernetics Lab holds fewer spells per level than a mage guild.
i8 CyborgSpellLimits[KB_MAGE_GUILD_MAX_LEVEL] = {2, 2, 1, 1, 1};

const char* GetDwellingName(i32 faction, i32 dwellingIndex) {
    if (faction >= 0 && faction < KB_FACTION_TABLE_CAPACITY
        && dwellingIndex >= 0 && dwellingIndex < KB_DWELLING_TYPE_COUNT
        && gDwellingNames[faction][dwellingIndex] != NULL) {
        return gDwellingNames[faction][dwellingIndex];
    }
    return "INVALID DWELLING";
}

static char* GetFirstLevelGrowerName(i32 faction) {
    return &firstLevelGrowerNames[faction][0];
}

static char* GetSpecialBuildingName(i32 faction) {
    return &specialBuildingNames[faction][0];
}

static char* GetSpecialBuildingDesc(i32 faction) {
    return &specialBuildingDescriptions[faction][0];
}

static void BuildingCostToIntArray(
    const BuildingCost& cost, i32 (&arr)[H2EnumIndex(RES_COUNT)]
) {
    arr[H2EnumIndex(RES_WOOD)] = cost.wood;
    arr[H2EnumIndex(RES_MERCURY)] = cost.mercury;
    arr[H2EnumIndex(RES_ORE)] = cost.ore;
    arr[H2EnumIndex(RES_SULFUR)] = cost.sulfur;
    arr[H2EnumIndex(RES_CRYSTAL)] = cost.crystal;
    arr[H2EnumIndex(RES_GEMS)] = cost.gems;
    arr[H2EnumIndex(RES_GOLD)] = cost.gold;
}

void InitializeTownConstants() {
    InitDwellingTypes();
    InitDwellingCosts();

    firstLevelGrowerNames[H2EnumIndex(FACTION_CYBORG)] =
        localization::Tr("town.cyborg.growth_building.name");
    specialBuildingNames[H2EnumIndex(FACTION_CYBORG)] =
        localization::Tr("town.cyborg.special_building.name");
    specialBuildingDescriptions[H2EnumIndex(FACTION_CYBORG)] =
        localization::Tr("town.cyborg.special_building.description");
    static const char* cyborgDwellingIds[KB_DWELLING_TYPE_COUNT] = {
        "town.cyborg.dwelling.1", "town.cyborg.dwelling.2", "town.cyborg.dwelling.3",
        "town.cyborg.dwelling.4", "town.cyborg.dwelling.5", "town.cyborg.dwelling.6",
        "town.cyborg.dwelling.upgrade_2", "town.cyborg.dwelling.upgrade_3",
        "town.cyborg.dwelling.upgrade_4", "town.cyborg.dwelling.upgrade_5",
        "town.cyborg.dwelling.upgrade_6", "town.cyborg.dwelling.upgrade_6"
    };
    for (i32 dwelling = 0; dwelling < KB_DWELLING_TYPE_COUNT; ++dwelling) {
        cyborgDwellings[dwelling] = localization::Tr(cyborgDwellingIds[dwelling]);
    }

    // Fill the retail tables, which this branch grows to the thirteen-row
    // faction capacity, from the Ironfist data (their InitBuildingNames and
    // the generalized tables).
    for (i32 f = 0; f < KB_FACTION_TABLE_CAPACITY; ++f) {
        gTownEligibleBuildMask[f] = townEligibleBuildMasks[f];
        gTownPrefixNames[f] = const_cast<char*>(townPrefixNames[f]);
        if (f == H2EnumIndex(FACTION_CYBORG)) {
            gWellExtraNames[f] = GetFirstLevelGrowerName(f);
            gSpecialBuildingNames[f] = GetSpecialBuildingName(f);
            gBuildingInfoSpecial[f] = GetSpecialBuildingDesc(f);
            for (i32 dwelling = 0; dwelling < KB_DWELLING_TYPE_COUNT; ++dwelling) {
                gDwellingNames[f][dwelling] = &cyborgDwellings[dwelling][0];
            }
        }

        for (i32 d = 0; d < KB_DWELLING_TYPE_COUNT; ++d) {
            gDwellingType[f][d] = static_cast<i8>(dwellingTypes[f][d]);
            i32 costs[H2EnumIndex(RES_COUNT)];
            BuildingCostToIntArray(dwellingCosts[f][d], costs);
            for (i32 r = 0; r < H2EnumIndex(RES_COUNT); ++r) {
                gDwellingCosts[f][d][r] = costs[r];
            }
        }

        i32 specialCosts[H2EnumIndex(RES_COUNT)];
        BuildingCostToIntArray(specialBuildingCosts[f], specialCosts);
        for (i32 r = 0; r < H2EnumIndex(RES_COUNT); ++r) {
            gSpecialBuildingCosts[f][r] = specialCosts[r];
        }
    }
}

} // namespace ironfist
