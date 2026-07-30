#ifndef HOMM2_BASE_BUTTON_H
#define HOMM2_BASE_BUTTON_H

#include <Ints.h>
#include "message.h"
#include "widget.h"

class icon;
enum class ButtonSelectMode : i16 {
    BUTTON_SELECT_STANDARD      = 0,
    BUTTON_SELECT_DIALOG_RESULT = 1
};
using enum ButtonSelectMode;

#pragma pack(push, 1)
class button : public widget {
public:
    icon* m_icon;
    i16 m_normalFrame;
    i16 m_pressedFrame;
    ButtonSelectMode m_selectMode;
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
        ButtonSelectMode selectMode,
        i16 hotkey,
        i16 id,
        WidgetKind kind
    );
    button(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        char* iconName,
        i16 normalFrame,
        i16 pressedFrame,
        ButtonSelectMode selectMode,
        i16 hotkey,
        i16 id,
        WidgetKind kind
    );
    virtual inline ~button() override;
    virtual void Draw(void) override;
    virtual MessageDispatchResult Main(struct tag_message& message) override;
    void Read(void);
    MessageDispatchResult Select(struct tag_message& message);
    MessageDispatchResult Deselect(struct tag_message& message);

private:
    inline MessageDispatchResult
        DeselectSelected(struct tag_message& message);
};
#pragma pack(pop)
extern MessageModifier iLeftRightSave;

#endif
