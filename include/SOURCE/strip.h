#ifndef HOMM2_SOURCE_STRIP_H
#define HOMM2_SOURCE_STRIP_H

#include <va.h>

class armyGroup;
class border;
class heroWindow;
class icon;

H2_ENUM_BEGIN(StripConstant)
    STRIP_ARMY_SLOT_COUNT       = 5,
    STRIP_BORDER_COUNT          = 6,
    STRIP_RUNTIME_GAP_SIZE      = 0x18,
    STRIP_WINDOW_WIDTH          = 0x228,
    STRIP_WINDOW_HEIGHT         = 0x69,
    STRIP_WINDOW_FLAGS          = 8,
    STRIP_PORTRAIT_X            = 5,
    STRIP_CONTENT_Y             = 6,
    STRIP_ARMY_FIRST_X          = 0x70,
    STRIP_ARMY_X_STEP           = 0x58,
    STRIP_ARMY_BORDER_WIDTH     = 0x52,
    STRIP_BORDER_HEIGHT         = 0x5d,
    STRIP_PORTRAIT_BORDER_WIDTH = 0x65,
    STRIP_BORDER_FILL_COLOR     = 1,
    STRIP_EMPTY_FRAME           = 2,
    STRIP_NO_ARMY_FRAME         = 0xb,
    STRIP_SELECTED_FRAME        = 1,
    STRIP_RACE_FRAME_OFFSET     = 4,
    STRIP_QUANTITY_Y            = 0x56,
    STRIP_QUANTITY_WIDTH        = 0x4d,
    STRIP_QUANTITY_HEIGHT       = 0xd,
    STRIP_QUANTITY_COLOR        = 1,
    STRIP_QUANTITY_BACKGROUND   = 2
H2_ENUM_END(StripConstant)

#pragma pack(push, 1)
class strip {
public:
    heroWindow* m_window;
    char m_unused04[STRIP_RUNTIME_GAP_SIZE];
    i32 m_x;
    i32 m_y;
    i32 m_stripType;
    i32 m_selectedSlot;
    border* m_borders[STRIP_BORDER_COUNT];
    icon* m_stripIcon;
    icon* m_creatureIcons[STRIP_ARMY_SLOT_COUNT];
    i32 m_cachedCreatureTypes[STRIP_ARMY_SLOT_COUNT];
    icon* m_portraitIcon;
    icon* m_flagIcon;
    i32 m_portraitFrame;
    i32 m_flagFrame;
    armyGroup* m_army;
    strip(
        i32 x,
        i32 y,
        i32 stripType,
        u32l portraitIconId,
        i32 portraitFrame,
        class armyGroup* army,
        i32 firstBorderId,
        i32 drawWindow,
        i32 flagFrame
    );
    ~strip();
    void Draw(void);
    void DrawIcons(i32);
    void DrawFrame(void);
};
#pragma pack(pop)
SIZE(strip, 0x84);
#endif
