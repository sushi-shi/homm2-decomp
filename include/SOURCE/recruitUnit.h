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
    int m_sourceType;                 // +0x36
    int m_creatureType;               // +0x3a
    char m_padding3e[4];               // +0x3e
    int m_goldCost;                    // +0x42
    int m_resourceType;                // +0x46
    int m_resourceCost;                // +0x4a
    int m_refreshTown;                 // +0x4e
    heroWindow *m_window;              // +0x52
    char m_padding56[4];               // +0x56
    armyGroup *m_army;                 // +0x5a
    char m_padding5e[4];               // +0x5e
    int m_recruited;                   // +0x62
    int m_noRoom;                      // +0x66
    short *m_available;                // +0x6a
    int m_maximum;                     // +0x6e
    int m_goldTotal;                   // +0x72
    int m_resourceTotal;               // +0x76
    int m_quantity;                    // +0x7a
    // --- constructors ---
    recruitUnit(class armyGroup *, int, short int *);
    recruitUnit(class town *, int, int);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Update(void);
};
#pragma pack(pop)
SIZE(recruitUnit, 0x7e);
// ---- globals (declarations, RVA order) ----
extern HMENU hmnuRecruitSave;

#endif // HOMM2_SOURCE_RECRUITUNIT_H
