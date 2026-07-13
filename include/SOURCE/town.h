#ifndef HOMM2_SOURCE_TOWN_H
#define HOMM2_SOURCE_TOWN_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <SOURCE/armyGroup.h>
// forward declarations:
class hero;

typedef enum TownType {
    TOWN_TYPE_KNIGHT = 0,
    TOWN_TYPE_BARBARIAN = 1,
    TOWN_TYPE_SORCERESS = 2,
    TOWN_TYPE_WARLOCK = 3,
    TOWN_TYPE_WIZARD = 4,
    TOWN_TYPE_NECROMANCER = 5
} TownType;

typedef enum TownBuilding {
    TOWN_BUILDING_TAVERN = 0x4,
    TOWN_BUILDING_TENT = 0x20,
    TOWN_BUILDING_CASTLE = 0x40,
    TOWN_BUILDING_CAPTAIN = 0x80,
    TOWN_BUILDING_MARKETPLACE = 0x400,
    TOWN_BUILDING_MOAT = 0x1000,
    TOWN_BUILDING_COLISEUM = 0x2000,
    TOWN_BUILDING_RAINBOW = 0x2000
} TownBuilding;

#pragma pack(push, 1)  // recovered layout is byte-packed
class town {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    char   m_id;  // +0x00
    char   m_owner;  // +0x01
    char _pad_0x2[0x1];
    char   m_type;  // +0x03
    unsigned char m_x;  // +0x04
    unsigned char m_y;  // +0x05
    unsigned char m_boatX;  // +0x06
    unsigned char m_boatY;  // +0x07
    armyGroup m_army;  // +0x08
    char   m_occupyingHeroId;  // +0x17
    int    m_buildings;  // +0x18
    signed char m_buildState;  // +0x1c
    char m_unknown1d;  // +0x1d
    short m_garrison[12];  // +0x1e
    unsigned char m_onMap;  // +0x36
    signed char m_unknown37;  // +0x37
    signed char m_unknown38;  // +0x38
    signed char m_originalOwner;  // +0x39
    unsigned short m_extraIndex;  // +0x3a
    union {
        signed char m_spells[5][4];  // +0x3c
        struct {
            char m_spellPad[19];
            signed char m_spellCounts[6];
        };
    };
    short m_unknown55;  // +0x55
    char m_name[13];  // +0x57
    // Packed retail stride is 0x64 bytes.
    // --- constructors ---
    town(void);
    // --- methods ---
    int HasGarrison(void);
    void GiveSpells(class hero *);
    void XformToCastle(void);
    void View(int);
    void Deallocate(void);
    void BuildBuilding(int);
    int CanBuildDock(void);
    void CalcNumLevelArchers(int *, int *);
};
#pragma pack(pop)
SIZE(town, 100);
// ---- globals (declarations, RVA order) ----
extern int bEnteringTown;

#endif // HOMM2_SOURCE_TOWN_H
