#ifndef HOMM2_SOURCE_STRIP_H
#define HOMM2_SOURCE_STRIP_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 5 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class armyGroup;
class border;
class heroWindow;
class icon;

typedef enum StripConstant {
    STRIP_ARMY_SLOT_COUNT = 5,
    STRIP_BORDER_COUNT = 6,
    STRIP_WINDOW_WIDTH = 0x228,
    STRIP_WINDOW_HEIGHT = 0x69,
    STRIP_WINDOW_FLAGS = 8,
    STRIP_PORTRAIT_X = 5,
    STRIP_CONTENT_Y = 6,
    STRIP_ARMY_FIRST_X = 0x70,
    STRIP_ARMY_X_STEP = 0x58,
    STRIP_ARMY_BORDER_WIDTH = 0x52,
    STRIP_BORDER_HEIGHT = 0x5d,
    STRIP_PORTRAIT_BORDER_WIDTH = 0x65,
    STRIP_BORDER_FILL_COLOR = 1,
    STRIP_EMPTY_FRAME = 2,
    STRIP_NO_ARMY_FRAME = 0xb,
    STRIP_SELECTED_FRAME = 1,
    STRIP_RACE_FRAME_OFFSET = 4,
    STRIP_QUANTITY_Y = 0x56,
    STRIP_QUANTITY_WIDTH = 0x4d,
    STRIP_QUANTITY_HEIGHT = 0xd,
    STRIP_QUANTITY_COLOR = 1,
    STRIP_QUANTITY_BACKGROUND = 2
} StripConstant;

#pragma pack(push, 1)  // recovered layout is byte-packed
class strip {
public:
    heroWindow *m_window;  // +0x00
    char m_unused04[0x18];
    int m_x;  // +0x1c
    int m_y;  // +0x20
    int m_stripType;  // +0x24
    int m_selectedSlot;  // +0x28
    border *m_borders[STRIP_BORDER_COUNT];  // +0x2c
    icon *m_stripIcon;  // +0x44
    icon *m_creatureIcons[STRIP_ARMY_SLOT_COUNT];  // +0x48
    int m_cachedCreatureTypes[STRIP_ARMY_SLOT_COUNT];  // +0x5c
    icon *m_portraitIcon;  // +0x70
    icon *m_flagIcon;  // +0x74
    int m_portraitFrame;  // +0x78
    int m_flagFrame;  // +0x7c
    armyGroup *m_army;  // +0x80
    // --- constructors ---
    strip(int x, int y, int stripType, unsigned long portraitIconId,
          int portraitFrame, class armyGroup *army, int firstBorderId,
          int drawWindow, int flagFrame);
    ~strip();
    // --- methods ---
    void Draw(void);
    void DrawIcons(int);
    void DrawFrame(void);
};
#pragma pack(pop)
SIZE(strip, 0x84);
#endif // HOMM2_SOURCE_STRIP_H
