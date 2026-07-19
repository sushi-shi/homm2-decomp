#ifndef HOMM2_SOURCE_TOWN_H
#define HOMM2_SOURCE_TOWN_H

#include <va.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/KB_TYPES.h>

class hero;

H2_ENUM_CLASS_BEGIN(TownBuilding)
    TOWN_BUILDING_MAGE_GUILD                    = 0x1,
    TOWN_BUILDING_TAVERN                        = 0x4,
    TOWN_BUILDING_DOCK                          = 0x8,
    TOWN_BUILDING_TENT                          = 0x20,
    TOWN_BUILDING_CASTLE                        = 0x40,
    TOWN_BUILDING_CAPTAIN                       = 0x80,
    TOWN_BUILDING_LEFT_TURRET                   = 0x100,
    TOWN_BUILDING_RIGHT_TURRET                  = 0x200,
    TOWN_BUILDING_MARKETPLACE                   = 0x400,
    TOWN_BUILDING_MOAT                          = 0x1000,
    TOWN_BUILDING_FORTIFICATIONS                = 0x2000,
    TOWN_BUILDING_COLISEUM                      = 0x2000,
    TOWN_BUILDING_RAINBOW                       = 0x2000,
    TOWN_BUILDING_SHRINE                        = 0x2000,
    TOWN_BUILDING_CAPTAIN_QUARTERS              = 0x8000,
    TOWN_BUILDING_LIBRARY                       = 0x2000,
    TOWN_BUILDING_DWELLING_1                    = 0x00080000,
    TOWN_BUILDING_DWELLING_2                    = 0x00100000,
    TOWN_BUILDING_DWELLING_3                    = 0x00200000,
    TOWN_BUILDING_DWELLING_4                    = 0x00400000,
    TOWN_BUILDING_DWELLING_5                    = 0x00800000,
    TOWN_BUILDING_DWELLING_6                    = 0x01000000,
    TOWN_BUILDING_UPGRADED_DWELLING_2           = 0x02000000,
    TOWN_BUILDING_UPGRADED_DWELLING_3           = 0x04000000,
    TOWN_BUILDING_UPGRADED_DWELLING_4           = 0x08000000,
    TOWN_BUILDING_UPGRADED_DWELLING_5           = 0x10000000,
    TOWN_BUILDING_UPGRADED_DWELLING_6           = 0x20000000,
    TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6 = 0x40000000
H2_ENUM_CLASS_END(TownBuilding)
H2_ENUM_FLAGS(TownBuilding)

H2_ENUM_BEGIN(TownConstant)
    TOWN_GARRISON_SLOT_COUNT           = 12, // slots 19..30: dwellings + upgrades (see BuildingSlotType)
    TOWN_OWNER_NONE                    = -1,
    TOWN_ID_NONE                       = -1,
    TOWN_OCCUPYING_HERO_NONE           = -1,
    TOWN_DOCK_COORDINATE_NONE          = 0xFF,
    TOWN_FORMATION_SPREAD              = 0,
    TOWN_FORMATION_GROUPED             = 1,
    TOWN_PLAYER_WINDOW_SIZE            = 5,
    TOWN_MAGE_GUILD_LEVEL_COUNT        = 5,
    TOWN_MAGE_GUILD_FIRST_LEVEL        = 1,
    TOWN_MAGE_GUILD_WISDOM_LEVEL_BONUS = 2,
    TOWN_MAGE_GUILD_SPELLS_PER_LEVEL   = 4,
    TOWN_CONVERT_SOURCE_FRAME          = 0x10,
    TOWN_CONVERT_ANY_FRAME             = 0xFF,
    TOWN_CONVERT_OBJECT_NONE           = 0,
    TOWN_VIEW_MEMORY_REQUIREMENT       = 0x514,
    TOWN_VIEW_LOW_MEMORY_LIMIT         = 0x320,
    TOWN_VIEW_HIGH_MEMORY_LIMIT        = 0xb54,
    TOWN_DISPOSE_NONE                  = 0,
    TOWN_DISPOSE_PARTIAL               = 1,
    TOWN_DISPOSE_FULL                  = 2
H2_ENUM_END(TownConstant)

#pragma pack(push, 1)
class town {
public:
    char m_id;
    char m_owner;
    i8 m_threat;
    char m_type;
    u8 m_x;
    u8 m_y;
    u8 m_boatX;
    u8 m_boatY;
    armyGroup m_army;
    char m_occupyingHeroId;
    u32l m_buildings;
    i8 m_buildState;
    char m_unknown1d;
    i16 m_garrison[IDX(TOWN_GARRISON_SLOT_COUNT)];
    u8 m_onMap;
    i8 m_mayNotUpgradeToCastle;
    i8 m_formation;
    i8 m_originalOwner;
    u16 m_extraIndex;
    union {
        i8 m_spells[TOWN_MAGE_GUILD_LEVEL_COUNT][TOWN_MAGE_GUILD_SPELLS_PER_LEVEL];
        i8 m_spellSlots[TOWN_MAGE_GUILD_LEVEL_COUNT * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL];
        struct {
            char m_spellPad[19];
            i8 m_spellCounts[6];
        };
    };
    u16 m_turnsOwned;
    char m_name[13];
    town(void);
    i32 HasGarrison(void);
    void GiveSpells(class hero*);
    void XformToCastle(void);
    void View(i32);
    void Deallocate(void);
    void BuildBuilding(i32);
    i32 CanBuildDock(void);
    void CalcNumLevelArchers(i32*, i32*);
};
#pragma pack(pop)
SIZE(town, 100);
extern i32 bEnteringTown;

#endif
