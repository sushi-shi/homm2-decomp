#include <IRONFIST/townconsts.h>

#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist {

u8 DwellingTypes[H2EnumIndex(KB_FACTION_TABLE_CAPACITY)][KB_DWELLING_TYPE_COUNT] = {};

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

i8 BuildingsToDraw[H2EnumIndex(KB_FACTION_TABLE_CAPACITY)][H2EnumIndex(BUILDING_SLOT_COUNT)] = {
  { // Knight buildings
    BUILDING_SPECIAL_GROWTH,
    BUILDING_TENT,
    BUILDING_CASTLE,
    BUILDING_SPECIAL,
    BUILDING_LEFT_TURRET,
    BUILDING_RIGHT_TURRET,
    BUILDING_CAPTAIN,
    BUILDING_MOAT,
    BUILDING_MARKET,
    BUILDING_THIEVES_GUILD,
    BUILDING_DWELLING_2,
    BUILDING_UPGRADE_1,
    BUILDING_DWELLING_5,
    BUILDING_UPGRADE_4,
    BUILDING_TAVERN,
    BUILDING_MAGE_GUILD,
    BUILDING_DWELLING_6,
    BUILDING_UPGRADE_5,
    BUILDING_EXT_1,
    BUILDING_EXT_2,
    BUILDING_EXT_0,
    BUILDING_DOCK,
    BUILDING_BOAT,
    BUILDING_DWELLING_1,
    BUILDING_DWELLING_3,
    BUILDING_UPGRADE_2,
    BUILDING_DWELLING_4,
    BUILDING_UPGRADE_3,
    BUILDING_STATUE,
    BUILDING_WELL,
    BUILDING_INVALID,
    BUILDING_INVALID
  },
  { // Barbarian buildings
    BUILDING_EXT_0,
    BUILDING_EXT_1,
    BUILDING_SPECIAL,
    BUILDING_SPECIAL_GROWTH,
    BUILDING_DWELLING_6,
    BUILDING_THIEVES_GUILD,
    BUILDING_CAPTAIN,
    BUILDING_DWELLING_5,
    BUILDING_UPGRADE_4,
    BUILDING_TENT,
    BUILDING_CASTLE,
    BUILDING_MOAT,
    BUILDING_MARKET,
    BUILDING_DWELLING_2,
    BUILDING_UPGRADE_1,
    BUILDING_DWELLING_3,
    BUILDING_EXT_3,
    BUILDING_DWELLING_1,
    BUILDING_DWELLING_4,
    BUILDING_UPGRADE_3,
    BUILDING_MAGE_GUILD,
    BUILDING_EXT_2,
    BUILDING_TAVERN,
    BUILDING_DOCK,
    BUILDING_BOAT,
    BUILDING_WELL,
    BUILDING_LEFT_TURRET,
    BUILDING_RIGHT_TURRET,
    BUILDING_STATUE,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID
  },
  { // Sorceress buildings
    BUILDING_SPECIAL,
    BUILDING_DWELLING_6,
    BUILDING_DWELLING_5,
    BUILDING_TENT,
    BUILDING_CASTLE,
    BUILDING_LEFT_TURRET,
    BUILDING_RIGHT_TURRET,
    BUILDING_MOAT,
    BUILDING_CAPTAIN,
    BUILDING_DWELLING_3,
    BUILDING_UPGRADE_2,
    BUILDING_DWELLING_1,
    BUILDING_MAGE_GUILD,
    BUILDING_DOCK,
    BUILDING_BOAT,
    BUILDING_DWELLING_4,
    BUILDING_UPGRADE_3,
    BUILDING_WELL,
    BUILDING_MARKET,
    BUILDING_DWELLING_2,
    BUILDING_UPGRADE_1,
    BUILDING_THIEVES_GUILD,
    BUILDING_TAVERN,
    BUILDING_EXT_1,
    BUILDING_SPECIAL_GROWTH,
    BUILDING_STATUE,
    BUILDING_EXT_0,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID
  },
  { // Warlock buildings
    BUILDING_DWELLING_5,
    BUILDING_DWELLING_3,
    BUILDING_TENT,
    BUILDING_CASTLE,
    BUILDING_LEFT_TURRET,
    BUILDING_RIGHT_TURRET,
    BUILDING_CAPTAIN,
    BUILDING_EXT_0,
    BUILDING_MOAT,
    BUILDING_TAVERN,
    BUILDING_THIEVES_GUILD,
    BUILDING_MAGE_GUILD,
    BUILDING_MARKET,
    BUILDING_STATUE,
    BUILDING_DOCK,
    BUILDING_BOAT,
    BUILDING_SPECIAL_GROWTH,
    BUILDING_DWELLING_1,
    BUILDING_SPECIAL,
    BUILDING_DWELLING_4,
    BUILDING_UPGRADE_3,
    BUILDING_DWELLING_2,
    BUILDING_UPGRADE_5,
    BUILDING_UPGRADE_5B,
    BUILDING_DWELLING_6,
    BUILDING_WELL,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID
  },
  { // Wizard buildings
    BUILDING_UPGRADE_5,
    BUILDING_DWELLING_6,
    BUILDING_THIEVES_GUILD,
    BUILDING_CAPTAIN,
    BUILDING_EXT_0,
    BUILDING_DWELLING_5,
    BUILDING_UPGRADE_4,
    BUILDING_TENT,
    BUILDING_CASTLE,
    BUILDING_MOAT,
    BUILDING_DWELLING_3,
    BUILDING_UPGRADE_2,
    BUILDING_DWELLING_1,
    BUILDING_DWELLING_4,
    BUILDING_MAGE_GUILD,
    BUILDING_TAVERN,
    BUILDING_DOCK,
    BUILDING_BOAT,
    BUILDING_WELL,
    BUILDING_DWELLING_2,
    BUILDING_SPECIAL,
    BUILDING_MARKET,
    BUILDING_LEFT_TURRET,
    BUILDING_RIGHT_TURRET,
    BUILDING_STATUE,
    BUILDING_SPECIAL_GROWTH,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID
  },
  { // Necromancer buildings
    BUILDING_SPECIAL,
    BUILDING_TAVERN,
    BUILDING_CASTLE,
    BUILDING_LEFT_TURRET,
    BUILDING_RIGHT_TURRET,
    BUILDING_MOAT,
    BUILDING_CAPTAIN,
    BUILDING_THIEVES_GUILD,
    BUILDING_DWELLING_6,
    BUILDING_DWELLING_1,
    BUILDING_DWELLING_3,
    BUILDING_UPGRADE_2,
    BUILDING_MAGE_GUILD,
    BUILDING_EXT_0,
    BUILDING_DOCK,
    BUILDING_BOAT,
    BUILDING_DWELLING_5,
    BUILDING_UPGRADE_4,
    BUILDING_DWELLING_2,
    BUILDING_UPGRADE_1,
    BUILDING_DWELLING_4,
    BUILDING_UPGRADE_3,
    BUILDING_SPECIAL_GROWTH,
    BUILDING_TENT,
    BUILDING_WELL,
    BUILDING_MARKET,
    BUILDING_STATUE,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID,
    BUILDING_INVALID
  },
  { BUILDING_INVALID },
  { BUILDING_INVALID },
  { BUILDING_INVALID },
  { BUILDING_INVALID },
  { BUILDING_INVALID },
  { BUILDING_INVALID },
  { // Cyborg buildings (include them all for now)
    BUILDING_MAGE_GUILD,
    BUILDING_TENT,
    BUILDING_CASTLE,
    BUILDING_WELL,
    BUILDING_STATUE,
    BUILDING_LEFT_TURRET,
    BUILDING_RIGHT_TURRET,
    BUILDING_SPECIAL_GROWTH,
    BUILDING_MOAT,
    BUILDING_SPECIAL,
    BUILDING_CAPTAIN,
    BUILDING_EXT_1,
    BUILDING_EXT_0,
    BUILDING_EXT_2,
    BUILDING_DOCK,
    BUILDING_BOAT,
    BUILDING_DWELLING_1,
    BUILDING_DWELLING_2,
    BUILDING_MARKET,
    BUILDING_TAVERN,
    BUILDING_DWELLING_3,
    BUILDING_DWELLING_4,
    BUILDING_DWELLING_5,
    BUILDING_DWELLING_6,
    BUILDING_UPGRADE_1,
    BUILDING_UPGRADE_2,
    BUILDING_UPGRADE_3,
    BUILDING_UPGRADE_4,
    BUILDING_UPGRADE_5,
    BUILDING_UPGRADE_5B,
    BUILDING_THIEVES_GUILD,
    BUILDING_EXT_3
  }
};

SBuildingCost SpecialBuildingCosts[H2EnumIndex(KB_FACTION_TABLE_CAPACITY)] = {
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

SBuildingCost DwellingCosts[H2EnumIndex(KB_FACTION_TABLE_CAPACITY)][KB_DWELLING_TYPE_COUNT] = {};

static void InitDwellingCosts() {
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 200 };
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3] = { 0, 0, 5, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4] = {10, 0, 10, 0, 0, 0, 2000};
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5] = {20, 0, 0, 0, 0, 0, 3000};
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6] = {20, 0, 0, 0, 20, 0, 5000};
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2_UPGRADE] = {5, 0, 0, 0, 0, 0, 1500};
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3_UPGRADE] = {0, 0, 5, 0, 0, 0, 1500};
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4_UPGRADE] = {5, 0, 5, 0, 0, 0, 2000};
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5_UPGRADE] = {10, 0, 0, 0, 0, 0, 3000};
  DwellingCosts[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6_UPGRADE] = {10, 0, 0, 0, 10, 0, 5000};

  DwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 300 };
  DwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2] = { 5, 0, 0, 0, 0, 0, 800 };
  DwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_3] = {0, 0, 0, 0, 0, 0, 1000};
  DwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4] = {10, 0, 10, 0, 0, 0, 2000};
  DwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5] = {0, 0, 20, 0, 0, 0, 4000};
  DwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_6] = {0, 0, 20, 0, 20, 0, 6000};
  DwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2_UPGRADE] = {5, 0, 0, 0, 0, 0, 1200};
  DwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4_UPGRADE] = {5, 0, 5, 0, 0, 0, 3000};
  DwellingCosts[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5_UPGRADE] = {0, 0, 10, 0, 0, 0, 2000};

  DwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_1] = { 5, 0, 0, 0, 0, 0, 500 };
  DwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2] = { 5, 0, 0, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3] = { 0, 0, 0, 0, 0, 0, 1500 };
  DwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4] = { 0, 0, 10, 0, 0, 0, 2500 };
  DwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_5] = {10, 0, 0, 0, 0, 10, 3000 };
  DwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_6] = { 0, 20, 30, 0, 0, 0, 10000 };
  DwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2_UPGRADE] = { 5, 0, 0, 0, 0, 0, 1500 };
  DwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3_UPGRADE] = { 5, 0, 0, 0, 0, 0, 1500 };
  DwellingCosts[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4_UPGRADE] = { 0, 5, 0, 0, 0, 0, 1500 };

  DwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 500 };
  DwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_2] = { 0, 0, 10, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_3] = { 0, 0, 0, 0, 0, 0, 2000 };
  DwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4] = { 0, 0, 0, 0, 0, 10, 3000 };
  DwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_5] = { 0, 0, 0, 10, 0, 0, 4000 };
  DwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6] = { 0, 0, 30, 20, 0, 0, 15000 };
  DwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4_UPGRADE] = { 0, 0, 0, 0, 0, 5, 2000 };
  DwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE] = { 0, 0, 5, 10, 0, 0, 5000 };
  DwellingCosts[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE2] = { 0, 0, 5, 10, 0, 0, 5000 };

  DwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 400 };
  DwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 800 };
  DwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_3] = { 5, 0, 5, 0, 0, 0, 1500 };
  DwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_4] = { 5, 0, 0, 0, 0, 0, 3000 };
  DwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_5] = { 5, 5, 5, 5, 5, 5, 3500 };
  DwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_6] = { 5, 0, 5, 0, 0, 20, 12500 };
  DwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_3_UPGRADE] = { 0, 5, 0, 0, 0, 0, 1500 };
  DwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_5_UPGRADE] = { 5, 0, 5, 0, 0, 0, 4000 };
  DwellingCosts[H2EnumIndex(FACTION_WIZARD)][DWELLING_6_UPGRADE] = { 5, 0, 5, 0, 0, 20, 12500 };

  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 400 };
  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3] = { 0, 0, 10, 0, 0, 0, 1500 };
  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4] = { 10, 0, 0, 0, 0, 0, 3000 };
  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5] = { 10, 0, 0, 10, 0, 0, 4000 };
  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_6] = { 10, 5, 10, 5, 5, 5, 10000 };
  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2_UPGRADE] = { 0, 0, 0, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3_UPGRADE] = { 0, 0, 5, 0, 0, 0, 1500 };
  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4_UPGRADE] = { 5, 0, 0, 0, 10, 10, 4000 };
  DwellingCosts[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5_UPGRADE] = { 0, 0, 5, 0, 5, 0, 3000 };

  // Cyborg base dwellings use the Knight cost profile. The shared upgrade
  // slots remain populated for table completeness, but their build-mask bits
  // are disabled.
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_1] = { 0, 0, 0, 0, 0, 0, 200 };
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_2] = { 0, 0, 0, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_3] = { 0, 0, 5, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_4] = {10, 0, 10, 0, 0, 0, 2000};
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_5] = {20, 0, 0, 0, 0, 0, 3000};
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_6] = {20, 0, 0, 0, 20, 0, 5000};
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_2_UPGRADE] = { 0, 0, 0, 0, 0, 0, 200 };
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_3_UPGRADE] = { 0, 0, 0, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_4_UPGRADE] = { 0, 0, 5, 0, 0, 0, 1000 };
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_5_UPGRADE] = {10, 0, 10, 0, 0, 0, 2000};
  DwellingCosts[H2EnumIndex(FACTION_CYBORG)][DWELLING_6_UPGRADE] = {20, 0, 0, 0, 0, 0, 3000};
}

static void InitDwellingTypes() {
  for (i32 i = 0; i < KB_FACTION_TABLE_CAPACITY; ++i) {
    for (i32 j = 0; j < KB_DWELLING_TYPE_COUNT; ++j) {
      DwellingTypes[i][j] = static_cast<u8>(CREATURE_TABLE_UNUSED);
    }
  }

  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_1] = static_cast<u8>(H2EnumIndex(CREATURE_PEASANT));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2] = static_cast<u8>(H2EnumIndex(CREATURE_ARCHER));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3] = static_cast<u8>(H2EnumIndex(CREATURE_PIKEMAN));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4] = static_cast<u8>(H2EnumIndex(CREATURE_SWORDSMAN));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5] = static_cast<u8>(H2EnumIndex(CREATURE_CAVALRY));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6] = static_cast<u8>(H2EnumIndex(CREATURE_PALADIN));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_2_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_RANGER));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_3_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_VETERAN_PIKEMAN));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_4_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_MASTER_SWORDSMAN));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_5_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_CHAMPION));
  DwellingTypes[H2EnumIndex(FACTION_KNIGHT)][DWELLING_6_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_CRUSADER));

  DwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_1] = static_cast<u8>(H2EnumIndex(CREATURE_GOBLIN));
  DwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2] = static_cast<u8>(H2EnumIndex(CREATURE_ORC));
  DwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_3] = static_cast<u8>(H2EnumIndex(CREATURE_WOLF));
  DwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4] = static_cast<u8>(H2EnumIndex(CREATURE_OGRE));
  DwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5] = static_cast<u8>(H2EnumIndex(CREATURE_TROLL));
  DwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_6] = static_cast<u8>(H2EnumIndex(CREATURE_CYCLOPS));
  DwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_2_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_ORC_CHIEF));
  DwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_4_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_OGRE_LORD));
  DwellingTypes[H2EnumIndex(FACTION_BARBARIAN)][DWELLING_5_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_WAR_TROLL));

  DwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_1] = static_cast<u8>(H2EnumIndex(CREATURE_SPRITE));
  DwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2] = static_cast<u8>(H2EnumIndex(CREATURE_DWARF));
  DwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3] = static_cast<u8>(H2EnumIndex(CREATURE_ELF));
  DwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4] = static_cast<u8>(H2EnumIndex(CREATURE_DRUID));
  DwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_5] = static_cast<u8>(H2EnumIndex(CREATURE_UNICORN));
  DwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_6] = static_cast<u8>(H2EnumIndex(CREATURE_PHOENIX));
  DwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_2_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_BATTLE_DWARF));
  DwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_3_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_GRAND_ELF));
  DwellingTypes[H2EnumIndex(FACTION_SORCERESS)][DWELLING_4_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_GREATER_DRUID));

  DwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_1] = static_cast<u8>(H2EnumIndex(CREATURE_CENTAUR));
  DwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_2] = static_cast<u8>(H2EnumIndex(CREATURE_GARGOYLE));
  DwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_3] = static_cast<u8>(H2EnumIndex(CREATURE_GRIFFIN));
  DwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4] = static_cast<u8>(H2EnumIndex(CREATURE_MINOTAUR));
  DwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_5] = static_cast<u8>(H2EnumIndex(CREATURE_HYDRA));
  DwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6] = static_cast<u8>(H2EnumIndex(CREATURE_GREEN_DRAGON));
  DwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_4_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_MINOTAUR_KING));
  DwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_RED_DRAGON));
  DwellingTypes[H2EnumIndex(FACTION_WARLOCK)][DWELLING_6_UPGRADE2] = static_cast<u8>(H2EnumIndex(CREATURE_BLACK_DRAGON));

  DwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_1] = static_cast<u8>(H2EnumIndex(CREATURE_HALFLING));
  DwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_2] = static_cast<u8>(H2EnumIndex(CREATURE_BOAR));
  DwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_3] = static_cast<u8>(H2EnumIndex(CREATURE_IRON_GOLEM));
  DwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_4] = static_cast<u8>(H2EnumIndex(CREATURE_ROC));
  DwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_5] = static_cast<u8>(H2EnumIndex(CREATURE_MAGE));
  DwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_6] = static_cast<u8>(H2EnumIndex(CREATURE_GIANT));
  DwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_3_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_STEEL_GOLEM));
  DwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_5_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_ARCHMAGE));
  DwellingTypes[H2EnumIndex(FACTION_WIZARD)][DWELLING_6_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_TITAN));

  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_1] = static_cast<u8>(H2EnumIndex(CREATURE_SKELETON));
  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2] = static_cast<u8>(H2EnumIndex(CREATURE_ZOMBIE));
  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3] = static_cast<u8>(H2EnumIndex(CREATURE_MUMMY));
  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4] = static_cast<u8>(H2EnumIndex(CREATURE_VAMPIRE));
  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5] = static_cast<u8>(H2EnumIndex(CREATURE_LICH));
  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_6] = static_cast<u8>(H2EnumIndex(CREATURE_BONE_DRAGON));
  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_2_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_MUTANT_ZOMBIE));
  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_3_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_ROYAL_MUMMY));
  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_4_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_VAMPIRE_LORD));
  DwellingTypes[H2EnumIndex(FACTION_NECROMANCER)][DWELLING_5_UPGRADE] = static_cast<u8>(H2EnumIndex(CREATURE_POWER_LICH));

  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_1] = H2EnumIndex(CREATURE_CYBER_KOBOLD_SPEARMAN);
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_2] = H2EnumIndex(CREATURE_CYBER_PLASMA_BERSERKER);
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_3] = H2EnumIndex(CREATURE_CYBER_PLASMA_LANCER);
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_4] = H2EnumIndex(CREATURE_CYBER_INDIGO_PANTHER);
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_5] = H2EnumIndex(CREATURE_CYBER_SHADOW_ASSASSIN);
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_6] = H2EnumIndex(CREATURE_CYBER_BEHEMOTH);
  // Cyborgs have no upgraded creature variants, so disabled upgrade slots
  // resolve to their corresponding base creatures.
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_2_UPGRADE] = H2EnumIndex(CREATURE_CYBER_PLASMA_BERSERKER);
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_3_UPGRADE] = H2EnumIndex(CREATURE_CYBER_PLASMA_LANCER);
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_4_UPGRADE] = H2EnumIndex(CREATURE_CYBER_INDIGO_PANTHER);
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_5_UPGRADE] = H2EnumIndex(CREATURE_CYBER_SHADOW_ASSASSIN);
  DwellingTypes[H2EnumIndex(FACTION_CYBORG)][DWELLING_6_UPGRADE] = H2EnumIndex(CREATURE_CYBER_BEHEMOTH);
}

u32 TownEligibleBuildMasks[H2EnumIndex(KB_FACTION_TABLE_CAPACITY)] = {
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

const char* TownPrefixNames[H2EnumIndex(KB_FACTION_TABLE_CAPACITY)] = {
  "twnk", "twnb", "twns", "twnw", "twnz", "twnn",
  "", "", "", "", "", "",
  "twnc"
};

// The Cybernetics Lab holds fewer spells per level than a mage guild.
i8 CyborgSpellLimits[KB_MAGE_GUILD_MAX_LEVEL] = {2, 2, 1, 1, 1};

b32 TownBuildingBuilt(const town* t, i32 building) {
    if (building < 0 || building >= H2EnumIndex(BUILDING_SLOT_COUNT)) {
        return false;
    }

    return (t->m_buildings & (1u << building)) != 0;
}

b32 TownDwellingBuilt(const town* t, i32 index) {
    if (index < 0 || index >= KB_DWELLING_TYPE_COUNT) {
        return false;
    }

    return TownBuildingBuilt(t, index + BUILDING_DWELLING_1);
}

i32 TownDwellingIndex(const town* t, i32 tier) {
    if (tier < 0 || tier > 5) {
        return -1;
    }

    i32 dwellingIdx = tier;
    if (tier > 0 && TownBuildingBuilt(t, tier + BUILDING_UPGRADE_1 - 1)) {
        dwellingIdx += 5;
    }
    if (tier == 5 && TownBuildingBuilt(t, BUILDING_UPGRADE_5B)) { // Warlock Black Tower
        dwellingIdx = 11;
    }
    return dwellingIdx;
}

i32 GetDwellingType(i32 faction, i32 dwellingIndex) {
    if (faction < 0 || faction >= KB_FACTION_TABLE_CAPACITY || dwellingIndex < 0
        || dwellingIndex >= KB_DWELLING_TYPE_COUNT) {
        return CREATURE_TABLE_UNUSED;
    }

    return DwellingTypes[faction][dwellingIndex];
}

char* GetDwellingName(i32 faction, i32 dwellingIndex) {
    if (faction >= 0 && faction < KB_FACTION_TABLE_CAPACITY
        && dwellingIndex >= 0 && dwellingIndex < KB_DWELLING_TYPE_COUNT
        && gDwellingNames[faction][dwellingIndex] != NULL) {
        return gDwellingNames[faction][dwellingIndex];
    }
    return const_cast<char*>("INVALID DWELLING");
}

char* GetFirstLevelGrowerName(i32 faction) {
    return &firstLevelGrowerNames[faction][0];
}

char* GetSpecialBuildingName(i32 faction) {
    return &specialBuildingNames[faction][0];
}

char* GetSpecialBuildingDesc(i32 faction) {
    return &specialBuildingDescriptions[faction][0];
}

char* GetCyborgDwellingName(i32 faction, i32 dwelling) {
    if (faction == H2EnumIndex(FACTION_CYBORG)) {
        return &cyborgDwellings[dwelling][0];
    }

    return const_cast<char*>("INVALID DWELLING");
}

void BuildingCostToIntArray(SBuildingCost& cost, i32 (&arr)[H2EnumIndex(RES_COUNT)]) {
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
        gTownEligibleBuildMask[f] = TownEligibleBuildMasks[f];
        gTownPrefixNames[f] = const_cast<char*>(TownPrefixNames[f]);
        if (f == H2EnumIndex(FACTION_CYBORG)) {
            gWellExtraNames[f] = GetFirstLevelGrowerName(f);
            gSpecialBuildingNames[f] = GetSpecialBuildingName(f);
            gBuildingInfoSpecial[f] = GetSpecialBuildingDesc(f);
            for (i32 dwelling = 0; dwelling < KB_DWELLING_TYPE_COUNT; ++dwelling) {
                gDwellingNames[f][dwelling] = &cyborgDwellings[dwelling][0];
            }
        }

        for (i32 d = 0; d < KB_DWELLING_TYPE_COUNT; ++d) {
            gDwellingType[f][d] = static_cast<i8>(DwellingTypes[f][d]);
            i32 costs[H2EnumIndex(RES_COUNT)];
            BuildingCostToIntArray(DwellingCosts[f][d], costs);
            for (i32 r = 0; r < H2EnumIndex(RES_COUNT); ++r) {
                gDwellingCosts[f][d][r] = costs[r];
            }
        }

        i32 specialCosts[H2EnumIndex(RES_COUNT)];
        BuildingCostToIntArray(SpecialBuildingCosts[f], specialCosts);
        for (i32 r = 0; r < H2EnumIndex(RES_COUNT); ++r) {
            gSpecialBuildingCosts[f][r] = specialCosts[r];
        }
    }
}

} // namespace ironfist
