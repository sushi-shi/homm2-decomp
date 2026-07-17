#ifndef HOMM2_SOURCE_TOWN_H
#define HOMM2_SOURCE_TOWN_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/KB_TYPES.h>
// forward declarations:
class hero;

HOMM2_ENUM_BEGIN(TownBuilding)
    TOWN_BUILDING_MAGE_GUILD = 0x1,
    TOWN_BUILDING_TAVERN = 0x4,
    TOWN_BUILDING_DOCK = 0x8,
    TOWN_BUILDING_TENT = 0x20,
    TOWN_BUILDING_CASTLE = 0x40,
    TOWN_BUILDING_CAPTAIN = 0x80,
    TOWN_BUILDING_LEFT_TURRET = 0x100,
    TOWN_BUILDING_RIGHT_TURRET = 0x200,
    TOWN_BUILDING_MARKETPLACE = 0x400,
    TOWN_BUILDING_MOAT = 0x1000,
    TOWN_BUILDING_FORTIFICATIONS = 0x2000,
    TOWN_BUILDING_COLISEUM = 0x2000,
    TOWN_BUILDING_RAINBOW = 0x2000,
    TOWN_BUILDING_SHRINE = 0x2000,
    TOWN_BUILDING_CAPTAIN_QUARTERS = 0x8000,
    TOWN_BUILDING_LIBRARY = 0x2000,
    TOWN_BUILDING_DWELLING_1 = 0x00080000,
    TOWN_BUILDING_DWELLING_2 = 0x00100000,
    TOWN_BUILDING_DWELLING_3 = 0x00200000,
    TOWN_BUILDING_DWELLING_4 = 0x00400000,
    TOWN_BUILDING_DWELLING_5 = 0x00800000,
    TOWN_BUILDING_DWELLING_6 = 0x01000000,
    TOWN_BUILDING_UPGRADED_DWELLING_2 = 0x02000000,
    TOWN_BUILDING_UPGRADED_DWELLING_3 = 0x04000000,
    TOWN_BUILDING_UPGRADED_DWELLING_4 = 0x08000000,
    TOWN_BUILDING_UPGRADED_DWELLING_5 = 0x10000000,
    TOWN_BUILDING_UPGRADED_DWELLING_6 = 0x20000000,
    TOWN_BUILDING_ALTERNATE_UPGRADED_DWELLING_6 = 0x40000000
HOMM2_ENUM_END(TownBuilding)
HOMM2_ENUM_FLAGS(TownBuilding)

HOMM2_ENUM_VALUES_BEGIN(TownConstant)
    TOWN_OWNER_NONE = -1,
    TOWN_ID_NONE = -1,
    TOWN_OCCUPYING_HERO_NONE = -1,
    TOWN_DOCK_COORDINATE_NONE = 0xFF,
    TOWN_FORMATION_SPREAD = 0,
    TOWN_FORMATION_GROUPED = 1,
    TOWN_PLAYER_WINDOW_SIZE = 5,
    TOWN_MAGE_GUILD_LEVEL_COUNT = 5,
    TOWN_MAGE_GUILD_FIRST_LEVEL = 1,
    TOWN_MAGE_GUILD_WISDOM_LEVEL_BONUS = 2,
    TOWN_MAGE_GUILD_SPELLS_PER_LEVEL = 4,
    TOWN_CONVERT_SOURCE_FRAME = 0x10,
    TOWN_CONVERT_ANY_FRAME = 0xFF,
    TOWN_CONVERT_OBJECT_NONE = 0,
    TOWN_VIEW_MEMORY_REQUIREMENT = 0x514,
    TOWN_VIEW_LOW_MEMORY_LIMIT = 0x320,
    TOWN_VIEW_HIGH_MEMORY_LIMIT = 0xb54,
    TOWN_DISPOSE_NONE = 0,
    TOWN_DISPOSE_PARTIAL = 1,
    TOWN_DISPOSE_FULL = 2
HOMM2_ENUM_VALUES_END(TownConstant)

#pragma pack(push, 1) // recovered layout is byte-packed
class town {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    char m_id;                  // +0x00
    char m_owner;               // +0x01
    i8 m_threat;                // +0x02
    char m_type;                // +0x03
    u8 m_x;                     // +0x04
    u8 m_y;                     // +0x05
    u8 m_boatX;                 // +0x06
    u8 m_boatY;                 // +0x07
    armyGroup m_army;           // +0x08
    char m_occupyingHeroId;     // +0x17
    u32l m_buildings;           // +0x18
    i8 m_buildState;            // +0x1c
    char m_unknown1d;           // +0x1d zero-initialized only, never read
    i16 m_garrison[12];         // +0x1e
    u8 m_onMap;                 // +0x36
    i8 m_mayNotUpgradeToCastle; // +0x37
    i8 m_formation;             // +0x38
    i8 m_originalOwner;         // +0x39
    u16 m_extraIndex;           // +0x3a
    union {
        i8 m_spells[TOWN_MAGE_GUILD_LEVEL_COUNT][TOWN_MAGE_GUILD_SPELLS_PER_LEVEL]; // +0x3c
        i8 m_spellSlots[TOWN_MAGE_GUILD_LEVEL_COUNT * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL];
        struct {
            char m_spellPad[19];
            i8 m_spellCounts[6];
        };
    };
    u16 m_turnsOwned; // +0x55
    char m_name[13];  // +0x57
    // Packed retail stride is 0x64 bytes.
    // --- constructors ---
    town(void);
    // --- methods ---
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
// ---- globals (declarations, RVA order) ----
extern i32 bEnteringTown;

#endif // HOMM2_SOURCE_TOWN_H
