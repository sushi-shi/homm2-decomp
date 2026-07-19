#ifndef HOMM2_SOURCE_RECRUITUNIT_H
#define HOMM2_SOURCE_RECRUITUNIT_H

#include <va.h>
#include <SOURCE/KB_TYPES.h>
#include <windows.h>
#include <BASE/baseManager.h>

class armyGroup;
class town;
class heroWindow;
struct tag_message;

#pragma pack(push, 1)
class recruitUnit : public baseManager {
public:
    i32 m_sourceType;
    i32 m_creatureType;
    char m_padding3e[4];
    i32 m_goldCost;
    ResourceType m_resourceType;
    i32 m_resourceCost;
    i32 m_refreshTown;
    heroWindow* m_window;
    char m_padding56[4];
    armyGroup* m_army;
    char m_padding5e[4];
    i32 m_recruited;
    i32 m_noRoom;
    i16* m_available;
    i32 m_maximum;
    i32 m_goldTotal;
    i32 m_resourceTotal;
    i32 m_quantity;
    recruitUnit(class armyGroup*, i32, i16*);
    recruitUnit(class town*, i32, i32);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Update(void);
};
#pragma pack(pop)
SIZE(recruitUnit, 0x7e);
extern HMENU hmnuRecruitSave;

#endif
