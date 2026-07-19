#ifndef HOMM2_SOURCE_SWAPMANAGER_H
#define HOMM2_SOURCE_SWAPMANAGER_H

#include <va.h>
#include <BASE/baseManager.h>

class hero;
class heroWindow;
class icon;
struct tag_message;

H2_ENUM_BEGIN(SwapManagerSide)
    SWAP_SIDE_NONE  = -1,
    SWAP_SIDE_LEFT  = 0,
    SWAP_SIDE_RIGHT = 1,
    SWAP_SIDE_COUNT = 2
H2_ENUM_END(SwapManagerSide)

H2_ENUM_CLASS_BEGIN(SwapManagerItemType)
    SWAP_ITEM_NONE     = -1,
    SWAP_ITEM_ARMY     = 0,
    SWAP_ITEM_ARTIFACT = 1
H2_ENUM_CLASS_END(SwapManagerItemType)

#pragma pack(push, 1)
class swapManager : public baseManager {
public:
    heroWindow* m_window;
    icon* m_selectorIcon;
    hero* m_heroes[IDX(SWAP_SIDE_COUNT)];
    SwapManagerSide m_selectedSide;
    SwapManagerSide m_targetSide;
    i32 m_selectedSlot;
    i32 m_targetSlot;
    SwapManagerItemType m_itemType;
    swapManager(void);
    swapManager(class hero*, class hero*);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Reset(void);
    i32 DrawSwapWin(void);
    void DrawSelector(void);
    void ViewMon(void);
    void SwapArtifacts(void);
    void SwapMons(void);
    void Update(void);
    void SplitMons(void);
};
#pragma pack(pop)
SIZE(swapManager, 0x5a);
#endif
