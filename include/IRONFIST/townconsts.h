#ifndef HOMM2_IRONFIST_TOWNCONSTS_H
#define HOMM2_IRONFIST_TOWNCONSTS_H

#include <string>
#include <vector>

#include <Ints.h>
#include <IRONFIST/creatures.h>
#include <SOURCE/KB_TYPES.h>

class town;

namespace ironfist {

/*
 * Ironfist's faction-generalized town data: dwelling creature types, dwelling
 * and special-building costs, building names, draw orders, eligible-build
 * masks and town graphics prefixes, all sized for the 13-faction table so the
 * Cyborgs (faction 12) fit. InitializeTownConstants fills the retail tables
 * from this data at startup.
 */

enum TownTableConstant {
    // Cyborg towns borrow the seventh slot of the retail town tilesets and
    // race-icon strips.
    CYBORG_SPRITE_SLOT  = 6,
    DWELLING_1          = 0,
    DWELLING_2          = 1,
    DWELLING_3          = 2,
    DWELLING_4          = 3,
    DWELLING_5          = 4,
    DWELLING_6          = 5,
    DWELLING_2_UPGRADE  = 6,
    DWELLING_3_UPGRADE  = 7,
    DWELLING_4_UPGRADE  = 8,
    DWELLING_5_UPGRADE  = 9,
    DWELLING_6_UPGRADE  = 10,
    DWELLING_6_UPGRADE2 = 11
};

enum BuildingCode {
    BUILDING_MAGE_GUILD      = 0x0,
    BUILDING_THIEVES_GUILD   = 0x1,
    BUILDING_TAVERN          = 0x2,
    BUILDING_DOCK            = 0x3,
    BUILDING_WELL            = 0x4,
    BUILDING_TENT            = 0x5,
    BUILDING_CASTLE          = 0x6,
    BUILDING_STATUE          = 0x7,
    BUILDING_LEFT_TURRET     = 0x8,
    BUILDING_RIGHT_TURRET    = 0x9,
    BUILDING_MARKET          = 0xa,
    BUILDING_SPECIAL_GROWTH  = 0xb,
    BUILDING_MOAT            = 0xc,
    BUILDING_SPECIAL         = 0xd,
    BUILDING_BOAT            = 0xe,
    BUILDING_CAPTAIN         = 0xf,
    BUILDING_EXT_0           = 0x10,
    BUILDING_EXT_1           = 0x11,
    BUILDING_EXT_2           = 0x12,
    BUILDING_DWELLING_1      = 0x13,
    BUILDING_DWELLING_2      = 0x14,
    BUILDING_DWELLING_3      = 0x15,
    BUILDING_DWELLING_4      = 0x16,
    BUILDING_DWELLING_5      = 0x17,
    BUILDING_DWELLING_6      = 0x18,
    BUILDING_UPGRADE_1       = 0x19,
    BUILDING_UPGRADE_2       = 0x1a,
    BUILDING_UPGRADE_3       = 0x1b,
    BUILDING_UPGRADE_4       = 0x1c,
    BUILDING_UPGRADE_5       = 0x1d,
    BUILDING_UPGRADE_5B      = 0x1e,
    BUILDING_EXT_3           = 0x1f,
    BUILDING_INVALID         = -1
};

struct SBuildingCost {
    i32 wood;
    i32 mercury;
    i32 ore;
    i32 sulfur;
    i32 crystal;
    i32 gems;
    i32 gold;
};

extern i8 BuildingsToDraw[KB_FACTION_TABLE_CAPACITY][H2EnumIndex(BUILDING_SLOT_COUNT)];
extern SBuildingCost SpecialBuildingCosts[KB_FACTION_TABLE_CAPACITY];
extern SBuildingCost DwellingCosts[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT];
extern u8 DwellingTypes[KB_FACTION_TABLE_CAPACITY][KB_DWELLING_TYPE_COUNT];
extern u32 TownEligibleBuildMasks[KB_FACTION_TABLE_CAPACITY];
extern const char* TownPrefixNames[KB_FACTION_TABLE_CAPACITY];
extern i8 CyborgSpellLimits[KB_MAGE_GUILD_MAX_LEVEL];

b32 TownBuildingBuilt(const town* t, i32 building);
b32 TownDwellingBuilt(const town* t, i32 index);
i32 TownDwellingIndex(const town* t, i32 tier);
i32 GetDwellingType(i32 faction, i32 dwellingIndex);
char* GetDwellingName(i32 faction, i32 dwellingIndex);
char* GetFirstLevelGrowerName(i32 faction);
char* GetSpecialBuildingName(i32 faction);
char* GetSpecialBuildingDesc(i32 faction);
char* GetCyborgDwellingName(i32 faction, i32 dwelling);
void BuildingCostToIntArray(SBuildingCost& cost, i32 (&arr)[H2EnumIndex(RES_COUNT)]);
void InitializeTownConstants();

} // namespace ironfist

#endif
