#ifndef HOMM2_BASE_BUTTON_H
#define HOMM2_BASE_BUTTON_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 10 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
class icon;
// forward declarations:
struct tag_message;

// m_selectMode is open-ended; retail proves only this distinguished value.
typedef enum ButtonSelectMode {
    BUTTON_SELECT_DIALOG_RESULT = 1
} ButtonSelectMode;

typedef enum ButtonHotkeyConstant {
    BUTTON_NO_HOTKEY = -1
} ButtonHotkeyConstant;

typedef enum ButtonConstant {
    BUTTON_REPEAT_DELAY_TICKS = 60
} ButtonConstant;

#pragma pack(push, 1)  // recovered layout is byte-packed
class button : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    icon  *m_icon;  // +0x20  button icon
    short  m_normalFrame;  // +0x24  frame drawn while the button is idle
    short  m_pressedFrame;  // +0x26  frame drawn while selected/armed
    short  m_selectMode;  // +0x28  1 emits dialog-select command 10; other values emit command 12
    short  m_hotkey;  // +0x2a
    int    m_iconId;  // +0x2c
    // --- constructors ---
    button(void);
    button(short int x, short int y, short int width, short int height,
           unsigned long int iconId, short int normalFrame, short int pressedFrame,
           short int selectMode, short int hotkey, short int id, short int kind);
    button(short int x, short int y, short int width, short int height,
           char *iconName, short int normalFrame, short int pressedFrame,
           short int selectMode, short int hotkey, short int id, short int kind);
    __declspec(dllexport) virtual inline ~button() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &message) OVERRIDE;
    // --- methods ---
    void Read(void);
    short int Select(struct tag_message &message);
    short int Deselect(struct tag_message &message);
};
#pragma pack(pop)
SIZE(button, 0x30);
// ---- globals (declarations, RVA order) ----
extern int iLeftRightSave;

#endif // HOMM2_BASE_BUTTON_H
