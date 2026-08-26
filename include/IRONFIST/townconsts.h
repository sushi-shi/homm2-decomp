#ifndef HOMM2_IRONFIST_TOWNCONSTS_H
#define HOMM2_IRONFIST_TOWNCONSTS_H

#include <Ints.h>
#include <IRONFIST/creatures.h>
#include <SOURCE/KB_TYPES.h>

namespace ironfist {

/*
 * Ironfist's faction-generalized town data is sized for the 13-faction table
 * so the Cyborgs (faction 12) fit. InitializeTownConstants fills the recovered
 * engine tables from that private extension data at startup.
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

extern i8 CyborgSpellLimits[KB_MAGE_GUILD_MAX_LEVEL];

char* GetDwellingName(i32 faction, i32 dwellingIndex);
void InitializeTownConstants();

} // namespace ironfist

#endif
