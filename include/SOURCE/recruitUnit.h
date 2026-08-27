#ifndef HOMM2_SOURCE_RECRUITUNIT_H
#define HOMM2_SOURCE_RECRUITUNIT_H

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>
#include <windows.h>
#include <BASE/baseManager.h>

class armyGroup;
class town;
class heroWindow;
struct tag_message;

enum {
    RECRUIT_SOURCE_EVENT = -1,
    RECRUIT_SOURCE_TOWN  = 0x23
};
typedef i32 RecruitSourceType;
#pragma pack(push, 1)
class recruitUnit H2_FINAL : public baseManager {
public:
    RecruitSourceType m_sourceType;
    i32 m_creatureType;
    char m_padding3e[4];
    i32 m_goldCost;
    i32 m_resourceType;
    i32 m_resourceCost;
    i32 m_refreshTown;
    heroWindow* m_window;
    char m_padding56[4];
    armyGroup* m_army;
    char m_padding5e[4];
    b32 m_recruited;
    b32 m_noRoom;
    i16* m_available;
    i32 m_maximum;
    i32 m_goldTotal;
    i32 m_resourceTotal;
    i32 m_quantity;
    recruitUnit(class armyGroup*, CreatureType, i16*);
    recruitUnit(class town*, i32, i32);
    virtual i32 Open(i32) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
    void Update(void);
};
#pragma pack(pop)
extern HMENU hmnuRecruitSave;

#endif
