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

static CreatureType gDwellingTypes[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT] = {};

static std::vector<std::string> gFirstLevelGrowerNames = {
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

static std::vector<std::string> gSpecialBuildingNameStorage = {
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

static std::vector<std::string> gSpecialBuildingDescriptions = {
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

static std::vector<std::string> gCyborgDwellings = {
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

static BuildingCost gSpecialBuildingCostDefinitions[KB_FACTION_TABLE_CAPACITY] = {
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

static BuildingCost gDwellingCostDefinitions[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT] = {};

static void InitDwellingCosts() {
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 200 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3] = { 0, 0, 5, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4] = {10, 0, 10, 0, 0, 0, 2000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5] = {20, 0, 0, 0, 0, 0, 3000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6] = {20, 0, 0, 0, 20, 0, 5000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2_UPGRADE] = {5, 0, 0, 0, 0, 0, 1500};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3_UPGRADE] = {0, 0, 5, 0, 0, 0, 1500};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4_UPGRADE] = {5, 0, 5, 0, 0, 0, 2000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5_UPGRADE] = {10, 0, 0, 0, 0, 0, 3000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6_UPGRADE] = {10, 0, 0, 0, 10, 0, 5000};

  gDwellingCostDefinitions[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 300 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2] = { 5, 0, 0, 0, 0, 0, 800 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_3] = {0, 0, 0, 0, 0, 0, 1000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4] = {10, 0, 10, 0, 0, 0, 2000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5] = {0, 0, 20, 0, 0, 0, 4000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_6] = {0, 0, 20, 0, 20, 0, 6000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2_UPGRADE] = {5, 0, 0, 0, 0, 0, 1200};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4_UPGRADE] = {5, 0, 5, 0, 0, 0, 3000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5_UPGRADE] = {0, 0, 10, 0, 0, 0, 2000};

  gDwellingCostDefinitions[H2EnumIndex(FACTION_SORCERESS)][DWELLING_1] = { 5, 0, 0, 0, 0, 0, 500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2] = { 5, 0, 0, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3] = { 0, 0, 0, 0, 0, 0, 1500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4] = { 0, 0, 10, 0, 0, 0, 2500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_SORCERESS)][DWELLING_5] = {10, 0, 0, 0, 0, 10, 3000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_SORCERESS)][DWELLING_6] = { 0, 20, 30, 0, 0, 0, 10000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2_UPGRADE] = { 5, 0, 0, 0, 0, 0, 1500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3_UPGRADE] = { 5, 0, 0, 0, 0, 0, 1500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4_UPGRADE] = { 0, 5, 0, 0, 0, 0, 1500 };

  gDwellingCostDefinitions[H2EnumIndex(FACTION_WARLOCK)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WARLOCK)][DWELLING_2] = { 0, 0, 10, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WARLOCK)][DWELLING_3] = { 0, 0, 0, 0, 0, 0, 2000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4] = { 0, 0, 0, 0, 0, 10, 3000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WARLOCK)][DWELLING_5] = { 0, 0, 0, 10, 0, 0, 4000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6] = { 0, 0, 30, 20, 0, 0, 15000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4_UPGRADE] = { 0, 0, 0, 0, 0, 5, 2000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE] = { 0, 0, 5, 10, 0, 0, 5000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE2] = { 0, 0, 5, 10, 0, 0, 5000 };

  gDwellingCostDefinitions[H2EnumIndex(FACTION_WIZARD)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 400 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WIZARD)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 800 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WIZARD)][DWELLING_3] = { 5, 0, 5, 0, 0, 0, 1500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WIZARD)][DWELLING_4] = { 5, 0, 0, 0, 0, 0, 3000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WIZARD)][DWELLING_5] = { 5, 5, 5, 5, 5, 5, 3500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WIZARD)][DWELLING_6] = { 5, 0, 5, 0, 0, 20, 12500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WIZARD)][DWELLING_3_UPGRADE] = { 0, 5, 0, 0, 0, 0, 1500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WIZARD)][DWELLING_5_UPGRADE] = { 5, 0, 5, 0, 0, 0, 4000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_WIZARD)][DWELLING_6_UPGRADE] = { 5, 0, 5, 0, 0, 20, 12500 };

  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 400 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3] = { 0, 0, 10, 0, 0, 0, 1500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4] = { 10, 0, 0, 0, 0, 0, 3000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5] = { 10, 0, 0, 10, 0, 0, 4000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_6] = { 10, 5, 10, 5, 5, 5, 10000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2_UPGRADE] = { 0, 0, 0, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3_UPGRADE] = { 0, 0, 5, 0, 0, 0, 1500 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4_UPGRADE] = { 5, 0, 0, 0, 10, 10, 4000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5_UPGRADE] = { 0, 0, 5, 0, 5, 0, 3000 };

  // Cyborg base dwellings use the Knight cost profile. The shared upgrade
  // slots remain populated for table completeness, but their build-mask bits
  // are disabled.
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 200 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_3] = { 0, 0, 5, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_4] = {10, 0, 10, 0, 0, 0, 2000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_5] = {20, 0, 0, 0, 0, 0, 3000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_6] = {20, 0, 0, 0, 20, 0, 5000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_2_UPGRADE] = { 0, 0, 0, 0, 0, 0, 200 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_3_UPGRADE] = { 0, 0, 0, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_4_UPGRADE] = { 0, 0, 5, 0, 0, 0, 1000 };
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_5_UPGRADE] = {10, 0, 10, 0, 0, 0, 2000};
  gDwellingCostDefinitions[H2EnumIndex(FACTION_CYBORG)][DWELLING_6_UPGRADE] = {20, 0, 0, 0, 0, 0, 3000};
}

static void InitDwellingTypes() {
  for (i32 i = 0; i < KB_FACTION_TABLE_CAPACITY; ++i) {
    for (i32 j = 0; j < KB_DWELLING_TYPE_COUNT; ++j) {
      gDwellingTypes[i][j] = CREATURE_NONE;
    }
  }

  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_1] = CREATURE_PEASANT;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2] = CREATURE_ARCHER;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3] = CREATURE_PIKEMAN;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4] = CREATURE_SWORDSMAN;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5] = CREATURE_CAVALRY;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6] = CREATURE_PALADIN;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2_UPGRADE] = CREATURE_RANGER;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3_UPGRADE] = CREATURE_VETERAN_PIKEMAN;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4_UPGRADE] = CREATURE_MASTER_SWORDSMAN;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5_UPGRADE] = CREATURE_CHAMPION;
  gDwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6_UPGRADE] = CREATURE_CRUSADER;

  gDwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_1] = CREATURE_GOBLIN;
  gDwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2] = CREATURE_ORC;
  gDwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_3] = CREATURE_WOLF;
  gDwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4] = CREATURE_OGRE;
  gDwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5] = CREATURE_TROLL;
  gDwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_6] = CREATURE_CYCLOPS;
  gDwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2_UPGRADE] = CREATURE_ORC_CHIEF;
  gDwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4_UPGRADE] = CREATURE_OGRE_LORD;
  gDwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5_UPGRADE] = CREATURE_WAR_TROLL;

  gDwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_1] = CREATURE_SPRITE;
  gDwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2] = CREATURE_DWARF;
  gDwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3] = CREATURE_ELF;
  gDwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4] = CREATURE_DRUID;
  gDwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_5] = CREATURE_UNICORN;
  gDwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_6] = CREATURE_PHOENIX;
  gDwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2_UPGRADE] = CREATURE_BATTLE_DWARF;
  gDwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3_UPGRADE] = CREATURE_GRAND_ELF;
  gDwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4_UPGRADE] = CREATURE_GREATER_DRUID;

  gDwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_1] = CREATURE_CENTAUR;
  gDwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_2] = CREATURE_GARGOYLE;
  gDwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_3] = CREATURE_GRIFFIN;
  gDwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4] = CREATURE_MINOTAUR;
  gDwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_5] = CREATURE_HYDRA;
  gDwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6] = CREATURE_GREEN_DRAGON;
  gDwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4_UPGRADE] = CREATURE_MINOTAUR_KING;
  gDwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE] = CREATURE_RED_DRAGON;
  gDwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE2] = CREATURE_BLACK_DRAGON;

  gDwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_1] = CREATURE_HALFLING;
  gDwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_2] = CREATURE_BOAR;
  gDwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_3] = CREATURE_IRON_GOLEM;
  gDwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_4] = CREATURE_ROC;
  gDwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_5] = CREATURE_MAGE;
  gDwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_6] = CREATURE_GIANT;
  gDwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_3_UPGRADE] = CREATURE_STEEL_GOLEM;
  gDwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_5_UPGRADE] = CREATURE_ARCHMAGE;
  gDwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_6_UPGRADE] = CREATURE_TITAN;

  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_1] = CREATURE_SKELETON;
  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2] = CREATURE_ZOMBIE;
  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3] = CREATURE_MUMMY;
  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4] = CREATURE_VAMPIRE;
  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5] = CREATURE_LICH;
  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_6] = CREATURE_BONE_DRAGON;
  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2_UPGRADE] = CREATURE_MUTANT_ZOMBIE;
  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3_UPGRADE] = CREATURE_ROYAL_MUMMY;
  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4_UPGRADE] = CREATURE_VAMPIRE_LORD;
  gDwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5_UPGRADE] = CREATURE_POWER_LICH;

  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_1] = CREATURE_CYBER_KOBOLD_SPEARMAN;
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_2] = CREATURE_CYBER_PLASMA_BERSERKER;
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_3] = CREATURE_CYBER_PLASMA_LANCER;
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_4] = CREATURE_CYBER_INDIGO_PANTHER;
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_5] = CREATURE_CYBER_SHADOW_ASSASSIN;
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_6] = CREATURE_CYBER_BEHEMOTH;
  // Cyborgs have no upgraded creature variants, so disabled upgrade slots
  // resolve to their corresponding base creatures.
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_2_UPGRADE] = CREATURE_CYBER_PLASMA_BERSERKER;
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_3_UPGRADE] = CREATURE_CYBER_PLASMA_LANCER;
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_4_UPGRADE] = CREATURE_CYBER_INDIGO_PANTHER;
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_5_UPGRADE] = CREATURE_CYBER_SHADOW_ASSASSIN;
  gDwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_6_UPGRADE] = CREATURE_CYBER_BEHEMOTH;
}

static u32 gTownEligibleBuildMasks[KB_FACTION_TABLE_CAPACITY] = {
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

static const char* gTownPrefixNameDefinitions[KB_FACTION_TABLE_CAPACITY] = {
  "twnk", "twnb", "twns", "twnw", "twnz", "twnn",
  "", "", "", "", "", "",
  "twnc"
};

// The Cybernetics Lab holds fewer spells per level than a mage guild.
i8 gCyborgSpellLimits[KB_MAGE_GUILD_MAX_LEVEL] = {2, 2, 1, 1, 1};

const char* GetDwellingName(i32 faction, i32 dwellingIndex) {
    if (faction >= 0 && faction < KB_FACTION_TABLE_CAPACITY
        && dwellingIndex >= 0 && dwellingIndex < KB_DWELLING_TYPE_COUNT
        && gDwellingNames[faction][dwellingIndex] != NULL) {
        return gDwellingNames[faction][dwellingIndex];
    }
    return "INVALID DWELLING";
}

static char* GetFirstLevelGrowerName(i32 faction) {
    return &gFirstLevelGrowerNames[faction][0];
}

static char* GetSpecialBuildingName(i32 faction) {
    return &gSpecialBuildingNameStorage[faction][0];
}

static char* GetSpecialBuildingDesc(i32 faction) {
    return &gSpecialBuildingDescriptions[faction][0];
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

    gFirstLevelGrowerNames[H2EnumIndex(FACTION_CYBORG)] =
        localization::Tr("town.cyborg.growth_building.name");
    gSpecialBuildingNameStorage[H2EnumIndex(FACTION_CYBORG)] =
        localization::Tr("town.cyborg.special_building.name");
    gSpecialBuildingDescriptions[H2EnumIndex(FACTION_CYBORG)] =
        localization::Tr("town.cyborg.special_building.description");
    static const char* cyborgDwellingIds[KB_DWELLING_TYPE_COUNT] = {
        "town.cyborg.dwelling.1", "town.cyborg.dwelling.2", "town.cyborg.dwelling.3",
        "town.cyborg.dwelling.4", "town.cyborg.dwelling.5", "town.cyborg.dwelling.6",
        "town.cyborg.dwelling.upgrade_2", "town.cyborg.dwelling.upgrade_3",
        "town.cyborg.dwelling.upgrade_4", "town.cyborg.dwelling.upgrade_5",
        "town.cyborg.dwelling.upgrade_6", "town.cyborg.dwelling.upgrade_6"
    };
    for (i32 dwelling = 0; dwelling < KB_DWELLING_TYPE_COUNT; ++dwelling) {
        gCyborgDwellings[dwelling] = localization::Tr(cyborgDwellingIds[dwelling]);
    }

    // Fill the retail tables, which this branch grows to the thirteen-row
    // faction capacity, from the Ironfist data (their InitBuildingNames and
    // the generalized tables).
    for (i32 f = 0; f < KB_FACTION_TABLE_CAPACITY; ++f) {
        gTownEligibleBuildMask[f] = gTownEligibleBuildMasks[f];
        gTownPrefixNames[f] = const_cast<char*>(gTownPrefixNameDefinitions[f]);
        if (f == H2EnumIndex(FACTION_CYBORG)) {
            gWellExtraNames[f] = GetFirstLevelGrowerName(f);
            gSpecialBuildingNames[f] = GetSpecialBuildingName(f);
            gBuildingInfoSpecial[f] = GetSpecialBuildingDesc(f);
            for (i32 dwelling = 0; dwelling < KB_DWELLING_TYPE_COUNT; ++dwelling) {
                gDwellingNames[f][dwelling] = &gCyborgDwellings[dwelling][0];
            }
        }

        for (i32 d = 0; d < KB_DWELLING_TYPE_COUNT; ++d) {
            gDwellingType[f][d] = static_cast<i8>(gDwellingTypes[f][d]);
            i32 costs[H2EnumIndex(RES_COUNT)];
            BuildingCostToIntArray(gDwellingCostDefinitions[f][d], costs);
            for (i32 r = 0; r < H2EnumIndex(RES_COUNT); ++r) {
                gDwellingCosts[f][d][r] = costs[r];
            }
        }

        i32 specialCosts[H2EnumIndex(RES_COUNT)];
        BuildingCostToIntArray(gSpecialBuildingCostDefinitions[f], specialCosts);
        for (i32 r = 0; r < H2EnumIndex(RES_COUNT); ++r) {
            gSpecialBuildingCosts[f][r] = specialCosts[r];
        }
    }
}

} // namespace ironfist
