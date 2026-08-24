#ifndef HOMM2_SOURCE_SWAPMANAGER_H
#define HOMM2_SOURCE_SWAPMANAGER_H

#include <Ints.h>
#include <BASE/baseManager.h>

class hero;
class heroWindow;
class icon;
struct tag_message;

enum {
    SWAP_SIDE_NONE  = -1,
    SWAP_SIDE_LEFT  = 0,
    SWAP_SIDE_RIGHT = 1,
    SWAP_SIDE_COUNT = 2
};
typedef i32 SwapManagerSide;

enum {
    SWAP_ITEM_NONE     = -1,
    SWAP_ITEM_ARMY     = 0,
    SWAP_ITEM_ARTIFACT = 1
};
typedef i32 SwapManagerItemType;
#pragma pack(push, 1)
class swapManager : public baseManager {
public:
    heroWindow* m_window;
    icon* m_selectorIcon;
    hero* m_heroes[(SWAP_SIDE_COUNT)];
    SwapManagerSide m_selectedSide;
    SwapManagerSide m_targetSide;
    i32 m_selectedSlot;
    i32 m_targetSlot;
    SwapManagerItemType m_itemType;
    swapManager(void);
    swapManager(class hero*, class hero*);
    virtual i32 Open(i32) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
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
#endif
