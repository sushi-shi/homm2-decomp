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
HOMM2_ENUM_BEGIN(ButtonSelectMode)
    BUTTON_SELECT_DIALOG_RESULT = 1
HOMM2_ENUM_END(ButtonSelectMode)

HOMM2_ENUM_VALUES_BEGIN(ButtonHotkeyConstant)
    BUTTON_NO_HOTKEY = -1
HOMM2_ENUM_VALUES_END(ButtonHotkeyConstant)

HOMM2_ENUM_VALUES_BEGIN(ButtonConstant)
    BUTTON_REPEAT_DELAY_TICKS = 60
HOMM2_ENUM_VALUES_END(ButtonConstant)

#pragma pack(push, 1) // recovered layout is byte-packed
class button : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    icon* m_icon;       // +0x20  button icon
    i16 m_normalFrame;  // +0x24  frame drawn while the button is idle
    i16 m_pressedFrame; // +0x26  frame drawn while selected/armed
    i16 m_selectMode;   // +0x28  1 emits dialog-select command 10; other values emit command 12
    i16 m_hotkey;       // +0x2a
    u32l m_iconId;      // +0x2c  aggregate resource id
    // --- constructors ---
    button(void);
    button(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        u32l iconId,
        i16 normalFrame,
        i16 pressedFrame,
        i16 selectMode,
        i16 hotkey,
        i16 id,
        i16 kind
    );
    button(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        char* iconName,
        i16 normalFrame,
        i16 pressedFrame,
        i16 selectMode,
        i16 hotkey,
        i16 id,
        i16 kind
    );
    __declspec(dllexport) virtual inline ~button() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message& message) OVERRIDE;
    // --- methods ---
    void Read(void);
    i16 Select(struct tag_message& message);
    i16 Deselect(struct tag_message& message);
};
#pragma pack(pop)
SIZE(button, 0x30);
// ---- globals (declarations, RVA order) ----
extern i32 iLeftRightSave;

#endif // HOMM2_BASE_BUTTON_H
