#ifndef HOMM2_SOURCE_ARMYGROUP_H
#define HOMM2_SOURCE_ARMYGROUP_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class hero;
class town;

#define ARMY_GROUP_SLOT_COUNT 5
#define ARMY_GROUP_EMPTY_SLOT (-1)

#pragma pack(push, 1)  // recovered layout is byte-packed
class armyGroup {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // NOTE: sparse this-access recovery (few/no this-relative probes).
    union {
        signed char m_creatureTypes[ARMY_GROUP_SLOT_COUNT];  // +0x00
        signed char m_troopTypes[ARMY_GROUP_SLOT_COUNT];
    };
    union {
        short m_creatureCounts[ARMY_GROUP_SLOT_COUNT];       // +0x05
        unsigned short m_troopCounts[ARMY_GROUP_SLOT_COUNT];
        signed short m_quantities[ARMY_GROUP_SLOT_COUNT];
    };
    // --- constructors ---
    armyGroup(void);
    // --- methods ---
    void View(int);
    int HasAllUndead(void);
    int HasSomeUndead(void);
    int GetMorale(class hero *, class town *, class armyGroup *);
    void Dismiss(int);
    int IsMember(int);
    int IsHomogeneous(int);
    int CanJoin(int);
    int GetNumArmies(void);
    int Add(int, int, int);
    void Swap(int, class armyGroup *, int);
    void DamageGroup(float);
};
#pragma pack(pop)
SIZE(armyGroup, 0xf);
#endif // HOMM2_SOURCE_ARMYGROUP_H
