#ifndef HOMM2_SOURCE_TOWN_H
#define HOMM2_SOURCE_TOWN_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <SOURCE/armyGroup.h>
// forward declarations:
class hero;

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
    char m_pad_0x6[2];
    armyGroup m_army;  // +0x08
    char   m_occupyingHeroId;  // +0x17
    int    m_buildings;  // +0x18
    char   m_field_0x1c;  // +0x1c
    char   m_field_0x1d;  // +0x1d
    char _pad_0x1e[0x14];
    short  field_0x32;  // +0x32
    short  field_0x34;  // +0x34
    char m_pad_0x36[0x2e];
    // size >= 0x36 (accessed lower bound; no ctor/stride oracle)
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
