#ifndef HOMM2_SOURCE_SWAPMANAGER_H
#define HOMM2_SOURCE_SWAPMANAGER_H
#include <va.h>
#include "../BASE/baseManager.h"
class hero;
class heroWindow;
class icon;
struct tag_message;

H2_ENUM_BEGIN(SwapManagerSide)
    SWAP_SIDE_NONE = -1,
    SWAP_SIDE_LEFT = 0,
    SWAP_SIDE_RIGHT = 1,
    SWAP_SIDE_COUNT = 2
H2_ENUM_END(SwapManagerSide)

H2_ENUM_CLASS_BEGIN(SwapManagerItemType)
    SWAP_ITEM_NONE = -1,
    SWAP_ITEM_ARMY = 0,
    SWAP_ITEM_ARTIFACT = 1
H2_ENUM_CLASS_END(SwapManagerItemType)

H2_ENUM_CLASS_BEGIN(SwapManagerMessage)
    SWAP_COMMAND_HOVER = 0xc,
    SWAP_COMMAND_SELECT = 0xd,
    SWAP_COMMAND_HELP = 0xe,
    SWAP_CONTROL_CLOSE = 0x7800,
    SWAP_SPLIT_MODIFIER_MASK = 3
H2_ENUM_CLASS_END(SwapManagerMessage)

H2_ENUM_BEGIN(SwapManagerControl)
    SWAP_CONTROL_LEFT_HERO = 0x41,
    SWAP_CONTROL_RIGHT_HERO = 0x42,
    SWAP_CONTROL_LEFT_ARMY_FIRST = 0x4e,
    SWAP_CONTROL_LEFT_ARMY_LAST = 0x52,
    SWAP_CONTROL_RIGHT_ARMY_FIRST = 0x53,
    SWAP_CONTROL_RIGHT_ARMY_LAST = 0x57,
    SWAP_CONTROL_LEFT_ARTIFACT_FIRST = 0x58,
    SWAP_CONTROL_LEFT_ARTIFACT_LAST = 0x65,
    SWAP_CONTROL_RIGHT_ARTIFACT_FIRST = 0x66,
    SWAP_CONTROL_RIGHT_ARTIFACT_LAST = 0x73,
    SWAP_CONTROL_LEFT_SKILL_FIRST = 0xc8,
    SWAP_CONTROL_LEFT_SKILL_LAST = 0xcf,
    SWAP_CONTROL_RIGHT_SKILL_FIRST = 0xd0,
    SWAP_CONTROL_RIGHT_SKILL_LAST = 0xd7,
    SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST = 0xd8,
    SWAP_CONTROL_LEFT_SKILL_LEVEL_LAST = 0xdf,
    SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST = 0xe0,
    SWAP_CONTROL_RIGHT_SKILL_LEVEL_LAST = 0xe7
H2_ENUM_END(SwapManagerControl)

H2_ENUM_BEGIN(SwapManagerConstant)
    SWAP_SECONDARY_SKILL_WIDGET_COUNT = 8,
    SWAP_WINDOW_TEXT_ID = 0x15,
    SWAP_LEFT_PORTRAIT_WIDGET = 0x41,
    SWAP_RIGHT_PORTRAIT_WIDGET = 0x42,
    SWAP_TITLE_WIDGET = 0x4d,
    SWAP_ADVENTURE_WIDGET_FIRST = 1,
    SWAP_ADVENTURE_WIDGET_LAST = 6,
    SWAP_ADVENTURE_DISABLE_VALUE = 2,
    SWAP_EMPTY_SKILL_VALUE = 4,
    SWAP_MONO_ICON_SKIP = 2,
    SWAP_MONO_ICON_DEFAULT = -1,
    SWAP_MANAGER_MESSAGE = 0x100,
    SWAP_SLOT_NONE = -1,
    SWAP_ARTIFACT_COLUMN_COUNT = 7,
    SWAP_FADE_STEPS = 8,
    SWAP_VIEW_FULL = 0,
    SWAP_VIEW_QUICK = 1,
    SWAP_CLOSE_REQUESTED = 1,
    SWAP_RESULT_CONTINUE = 1,
    SWAP_RESULT_CLOSE = 2,
    SWAP_SELECTOR_WIDTH = 0x2e,
    SWAP_SELECTOR_HEIGHT = 0x2e,
    SWAP_ARMY_SELECTOR_FRAME = 3,
    SWAP_ARTIFACT_SELECTOR_FRAME = 2,
    SWAP_EMPTY_ITEM_VALUE = 4,
    SWAP_ARTIFACT_FIRST_ROW_LAST = 6,
    SWAP_PRIMARY_SKILL_COUNT = 4,
    SWAP_LEFT_PRIMARY_SKILL_FIRST = 0x43,
    SWAP_RIGHT_PRIMARY_SKILL_FIRST = 0x48,
    SWAP_LEFT_ARMY_COUNT_FIRST = 0x74,
    SWAP_RIGHT_ARMY_COUNT_FIRST = 0x79,
    SWAP_ARMY_VIEW_X = 0x77,
    SWAP_ARMY_VIEW_Y = 0x14,
    SWAP_SPLIT_WINDOW_X = 0xb1,
    SWAP_SPLIT_WINDOW_Y = 0x14,
    SWAP_SPLIT_TEXT_CONTROL = 1,
    SWAP_SPLIT_AMOUNT_CONTROL = 0x44,
    SWAP_SPLIT_CONFIRM = 0x7802
H2_ENUM_END(SwapManagerConstant)

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
