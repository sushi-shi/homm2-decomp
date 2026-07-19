#ifndef HOMM2_BASE_BUTTON_H
#define HOMM2_BASE_BUTTON_H
#include <va.h>
#include "widget.h"
class icon;
struct tag_message;

H2_ENUM_CLASS_BEGIN(ButtonSelectMode)
    BUTTON_SELECT_DIALOG_RESULT = 1
H2_ENUM_CLASS_END(ButtonSelectMode)

H2_ENUM_BEGIN(ButtonHotkeyConstant)
    BUTTON_NO_HOTKEY = -1
H2_ENUM_END(ButtonHotkeyConstant)

H2_ENUM_BEGIN(ButtonConstant)
    BUTTON_REPEAT_DELAY_TICKS = 60
H2_ENUM_END(ButtonConstant)

#pragma pack(push, 1)
class button : public widget {
public:
    icon* m_icon;
    i16 m_normalFrame;
    i16 m_pressedFrame;
    i16 m_selectMode;
    i16 m_hotkey;
    u32l m_iconId;
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
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message& message) OVERRIDE;
    void Read(void);
    i16 Select(struct tag_message& message);
    i16 Deselect(struct tag_message& message);
};
#pragma pack(pop)
SIZE(button, 0x30);
extern i32 iLeftRightSave;

#endif
