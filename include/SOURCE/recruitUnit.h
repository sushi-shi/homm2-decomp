#ifndef HOMM2_SOURCE_RECRUITUNIT_H
#define HOMM2_SOURCE_RECRUITUNIT_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include <windows.h>
#include "../BASE/baseManager.h"
// forward declarations:
class armyGroup;
class town;
class heroWindow;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class recruitUnit : public baseManager {
public:
    i32 m_sourceType;                 // +0x36
    i32 m_creatureType;               // +0x3a
    char m_padding3e[4];               // +0x3e
    i32 m_goldCost;                    // +0x42
    i32 m_resourceType;                // +0x46
    i32 m_resourceCost;                // +0x4a
    i32 m_refreshTown;                 // +0x4e
    heroWindow *m_window;              // +0x52
    char m_padding56[4];               // +0x56
    armyGroup *m_army;                 // +0x5a
    char m_padding5e[4];               // +0x5e
    i32 m_recruited;                   // +0x62
    i32 m_noRoom;                      // +0x66
    i16 *m_available;                // +0x6a
    i32 m_maximum;                     // +0x6e
    i32 m_goldTotal;                   // +0x72
    i32 m_resourceTotal;               // +0x76
    i32 m_quantity;                    // +0x7a
    // --- constructors ---
    recruitUnit(class armyGroup *, i32, i16 *);
    recruitUnit(class town *, i32, i32);
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Update(void);
};
#pragma pack(pop)
SIZE(recruitUnit, 0x7e);
// ---- globals (declarations, RVA order) ----
extern HMENU hmnuRecruitSave;

#endif // HOMM2_SOURCE_RECRUITUNIT_H
