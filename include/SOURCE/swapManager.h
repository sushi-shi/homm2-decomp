#ifndef HOMM2_SOURCE_SWAPMANAGER_H
#define HOMM2_SOURCE_SWAPMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
class hero;
class heroWindow;
class icon;
struct tag_message;

typedef enum SwapManagerSide {
    SWAP_SIDE_NONE = -1,
    SWAP_SIDE_LEFT = 0,
    SWAP_SIDE_RIGHT = 1,
    SWAP_SIDE_COUNT = 2
} SwapManagerSide;

typedef enum SwapManagerItemType {
    SWAP_ITEM_NONE = -1,
    SWAP_ITEM_ARMY = 0,
    SWAP_ITEM_ARTIFACT = 1
} SwapManagerItemType;

typedef enum SwapManagerMessage {
    SWAP_MESSAGE_REDRAW = 0x20,
    SWAP_MESSAGE_WIDGET = 0x200,
    SWAP_MESSAGE_EXIT = 0x4000,
    SWAP_COMMAND_HOVER = 0xc,
    SWAP_COMMAND_SELECT = 0xd,
    SWAP_COMMAND_HELP = 0xe,
    SWAP_COMMAND_EXIT = 4,
    SWAP_CONTROL_CLOSE = 0x7800,
    SWAP_QUICK_VIEW_MODIFIER = 0x200,
    SWAP_SPLIT_MODIFIER_MASK = 3
} SwapManagerMessage;

typedef enum SwapManagerControl {
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
} SwapManagerControl;

typedef enum SwapManagerConstant {
    SWAP_SECONDARY_SKILL_WIDGET_COUNT = 8,
    SWAP_WINDOW_TEXT_ID = 0x15,
    SWAP_MESSAGE_SET_PORTRAIT = 9,
    SWAP_MESSAGE_SET_TEXT = 3,
    SWAP_MESSAGE_SET_ICON = 4,
    SWAP_MESSAGE_ENABLE = 5,
    SWAP_MESSAGE_DISABLE = 6,
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
    SWAP_ARTIFACT_NONE = -1,
    SWAP_CREATURE_NONE = -1,
    SWAP_ARTIFACT_COLUMN_COUNT = 7,
    SWAP_FADE_STEPS = 8,
    SWAP_VIEW_FULL = 0,
    SWAP_VIEW_QUICK = 1,
    SWAP_RESULT_CONTINUE = 1,
    SWAP_RESULT_CLOSE = 2,
    SWAP_SELECTOR_WIDTH = 0x2e,
    SWAP_SELECTOR_HEIGHT = 0x2e,
    SWAP_ARMY_SELECTOR_FRAME = 3,
    SWAP_ARTIFACT_SELECTOR_FRAME = 2,
    SWAP_ARTIFACT_FIRST_ROW_LAST = 6,
    SWAP_ARTIFACT_MAGIC_BOOK = 0x51,
    SWAP_ARMY_VIEW_X = 0x77,
    SWAP_ARMY_VIEW_Y = 0x14
} SwapManagerConstant;

#pragma pack(push, 1)  // recovered layout is byte-packed
class swapManager : public baseManager {
public:
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    heroWindow *m_window;  // +0x36
    icon *m_selectorIcon;  // +0x3a
    hero *m_heroes[SWAP_SIDE_COUNT];  // +0x3e
    int m_selectedSide;  // +0x46
    int m_targetSide;  // +0x4a
    int m_selectedSlot;  // +0x4e
    int m_targetSlot;  // +0x52
    int m_itemType;  // +0x56
    // --- constructors ---
    swapManager(void);
    swapManager(class hero *, class hero *);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Reset(void);
    int DrawSwapWin(void);
    void DrawSelector(void);
    void ViewMon(void);
    void SwapArtifacts(void);
    void SwapMons(void);
    void Update(void);
    void SplitMons(void);
};
#pragma pack(pop)
SIZE(swapManager, 0x5a);
#endif // HOMM2_SOURCE_SWAPMANAGER_H
