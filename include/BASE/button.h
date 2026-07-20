#ifndef HOMM2_BASE_BUTTON_H
#define HOMM2_BASE_BUTTON_H

#include <va.h>
#include "message.h"
#include "widget.h"

class icon;
H2_ENUM_CLASS_BEGIN_T(ButtonSelectMode, i16)
    BUTTON_SELECT_STANDARD      = 0,
    BUTTON_SELECT_DIALOG_RESULT = 1
H2_ENUM_CLASS_END_T(ButtonSelectMode, i16)

#pragma pack(push, 1)
class button : public widget {
public:
    icon* m_icon;
    i16 m_normalFrame;
    i16 m_pressedFrame;
    H2_ENUM_STORAGE(ButtonSelectMode, i16) m_selectMode;
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
        H2_ENUM_PARAM(ButtonSelectMode, i16) selectMode,
        i16 hotkey,
        i16 id,
        H2_ENUM_PARAM(WidgetKind, i16) kind
    );
    button(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        char* iconName,
        i16 normalFrame,
        i16 pressedFrame,
        H2_ENUM_PARAM(ButtonSelectMode, i16) selectMode,
        i16 hotkey,
        i16 id,
        H2_ENUM_PARAM(WidgetKind, i16) kind
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
extern MessageModifier iLeftRightSave;

#endif
