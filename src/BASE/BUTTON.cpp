#include <va.h>
#include <BASE/widget.h>
#include <BASE/message.h>
#include <BASE/button.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/kbwin.h>
#include <BASE/mouseManager.h>
#include <BASE/inputManager.h>
#include <SOURCE/KB.h>

H2_ENUM_BEGIN(ButtonHotkeyConstant)
    NO_HOTKEY = -1
H2_ENUM_END(ButtonHotkeyConstant)

H2_ENUM_BEGIN(ButtonConstant)
    RESOURCE_NAME_CAPACITY = 16,
    REPEAT_DELAY_TICKS     = 60
H2_ENUM_END(ButtonConstant)

VA(0x004d34e0, 0x63)
button::button(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_iconId = 0;
    m_icon = NULL;
    m_normalFrame = 0;
    m_pressedFrame = 0;
    m_selectMode = BUTTON_SELECT_STANDARD;
    m_hotkey = NO_HOTKEY;
}

VA(0x004d3580, 0xb6)
button::button(
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
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_normalFrame = normalFrame;
    m_pressedFrame = pressedFrame;
    m_selectMode = selectMode;
    m_hotkey = hotkey;
}

VA(0x004d3640, 0xc7)
button::button(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    H2_CONST char* iconName,
    i16 normalFrame,
    i16 pressedFrame,
    H2_ENUM_PARAM(ButtonSelectMode, i16) selectMode,
    i16 hotkey,
    i16 id,
    H2_ENUM_PARAM(WidgetKind, i16) kind
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    m_normalFrame = normalFrame;
    m_pressedFrame = pressedFrame;
    m_selectMode = selectMode;
    m_hotkey = hotkey;
}

VA(0x004d3710, 0x115)
void button::Read(void) {
    char iconName[RESOURCE_NAME_CAPACITY];
    READ_WIDGET_GEOMETRY(*this, gpResourceManager);
    gpResourceManager->Read13(iconName);
    gpResourceManager->SavePosition();
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    gpResourceManager->RestorePosition();
    m_normalFrame = gpResourceManager->ReadWord();
    m_pressedFrame = gpResourceManager->ReadWord();
    m_selectMode = static_cast<ButtonSelectMode>(gpResourceManager->ReadWord());
    m_hotkey = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
}

VA(0x004d3830, 0x5b)
H2_RETAIL_INLINE button::~button() {
    gpResourceManager->Dispose(m_icon);
}

VA(0x004d3890, 0x4d8)
MessageDispatchResult button::Main(tag_message& message) {
    if (m_kind == WIDGET_KIND_AUTO_REPEAT && HAS(m_flags, WIDGET_FLAG_SELECTED)
        && glTimers[GLOBAL_BUTTON_REPEAT_TIMER_SLOT] < KBTickCount()) {
        return Deselect(message);
    }

    if (!HAS(m_flags, WIDGET_FLAG_ENABLED)) {
        if (message.type == MESSAGE_WIDGET)
            return widget::Main(message);
        return MESSAGE_DISPATCH_CONTINUE;
    }

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_REPLACE_ICON:
                    if (m_iconId == static_cast<u32l>(message.payload.widget.id)) {
                        m_iconId = message.payload.widget.data.value;
                        gpResourceManager->Dispose(m_icon);
                        m_icon = gpResourceManager->GetIcon(message.payload.widget.data.value);
                    }
                    return MESSAGE_DISPATCH_CONTINUE;
            }
            break;

        case MESSAGE_KEY_DOWN:
            if (!HAS(m_flags, WIDGET_FLAG_ENABLED))
                break;
            if (!HAS(m_flags, WIDGET_FLAG_DRAW))
                break;
            if (HAS(m_flags, WIDGET_FLAG_DIMMED))
                break;
            if (m_hotkey != NO_HOTKEY && m_hotkey == message.payload.keyboard.keyCode)
                return Select(message);
            return MESSAGE_DISPATCH_CONTINUE;

        case MESSAGE_KEY_UP:
            if (!HAS(m_flags, WIDGET_FLAG_ENABLED))
                break;
            if (!HAS(m_flags, WIDGET_FLAG_DRAW))
                break;
            if (HAS(m_flags, WIDGET_FLAG_DIMMED))
                break;
            if (m_hotkey != NO_HOTKEY && m_hotkey == message.payload.keyboard.keyCode)
                return Deselect(message);
            return MESSAGE_DISPATCH_CONTINUE;

        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            if (!HAS(m_flags, WIDGET_FLAG_DRAW))
                break;

            i16 x = message.payload.mouse.x - m_owner->m_posX;
            i16 y = message.payload.mouse.y - m_owner->m_posY;
            if (message.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                if (WIDGET_CONTAINS_LOCAL_POINT(*this, x, y)) {
                    SET_WIDGET_MESSAGE(message, WIDGET_NOTIFY_RIGHT_CLICK, m_id);
                    message.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;
                    return MESSAGE_DISPATCH_FORWARD;
                }
                return MESSAGE_DISPATCH_CONTINUE;
            }

            if (!HAS(m_flags, WIDGET_FLAG_DIMMED) && WIDGET_CONTAINS_LOCAL_POINT(*this, x, y)) {
                Select(message);
                while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
                    PollSound();
                    gpMouseManager->Main(message);
                    if (message.type == MESSAGE_MOUSE_MOVE) {
                        x = message.payload.mouse.x - m_owner->m_posX;
                        y = message.payload.mouse.y - m_owner->m_posY;
                        if (WIDGET_CONTAINS_LOCAL_POINT(*this, x, y)) {
                            if (!HAS(m_flags, WIDGET_FLAG_SELECTED)) {
                                Select(message);
                            }
                        } else if (HAS(m_flags, WIDGET_FLAG_SELECTED)) {
                            Deselect(message);
                        }
                    }
                    Process1WindowsMessage();
                    message = gpInputManager->GetEvent();
                }
                if (HAS(m_flags, WIDGET_FLAG_SELECTED)) {
                    Deselect(message);
                    return MESSAGE_DISPATCH_FORWARD;
                }
                return MESSAGE_DISPATCH_CONSUME;
            }
            return MESSAGE_DISPATCH_CONTINUE;
        }

        case MESSAGE_LEFT_BUTTON_UP:
            if (!HAS(m_flags, WIDGET_FLAG_DRAW))
                break;
            if (HAS(m_flags, WIDGET_FLAG_SELECTED))
                return Deselect(message);
            goto normalEvent;
    }

normalEvent:
    return widget::Main(message);
}

VA(0x004d3d70, 0xe9)
H2_ENUM_RETURN(MessageDispatchResult, i16) button::Select(struct tag_message& message) {
    i16 x = m_owner->m_posX + m_x;
    i16 y = m_owner->m_posY + m_y;
    m_icon->DrawToBuffer(x, y, m_pressedFrame, ICON_DRAW_NORMAL);
    gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
    m_flags |= WIDGET_FLAG_SELECTED;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.id = m_id;
    if (m_selectMode == BUTTON_SELECT_DIALOG_RESULT) {
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
    } else {
        message.payload.widget.command = WIDGET_NOTIFY_SELECT;
    }
    glTimers[GLOBAL_BUTTON_REPEAT_TIMER_SLOT] = KBTickCount() + REPEAT_DELAY_TICKS;
    iLeftRightSave = message.payload.widget.modifiers
        & MESSAGE_MODIFIER_BUTTON_MASK;
    return MESSAGE_DISPATCH_FORWARD;
}

VA(0x004d3e60, 0xb7)
H2_ENUM_RETURN(MessageDispatchResult, i16) button::Deselect(struct tag_message& message) {
    if (!HAS(m_flags, WIDGET_FLAG_SELECTED))
        return MESSAGE_DISPATCH_CONTINUE;
    m_flags &= ~WIDGET_FLAG_SELECTED;
    Draw();
    gpWindowManager->UpdateScreenRegion(
        m_owner->m_posX + m_x,
        m_owner->m_posY + m_y,
        m_width,
        m_height
    );
    SET_WIDGET_MESSAGE(message, WIDGET_NOTIFY_DESELECT, m_id);
    message.payload.widget.modifiers = iLeftRightSave;
    iLeftRightSave = MESSAGE_MODIFIER_NONE;
    return MESSAGE_DISPATCH_FORWARD;
}


VA(0x004d3f20, 0x91)
void button::Draw(void) {
    if (HAS(m_flags, WIDGET_FLAG_SELECTED)) {
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_x,
            m_owner->m_posY + m_y,
            m_pressedFrame,
            ICON_DRAW_NORMAL
        );
        return;
    }
    m_icon->DrawToBuffer(
        m_owner->m_posX + m_x,
        m_owner->m_posY + m_y,
        m_normalFrame,
        ICON_DRAW_NORMAL
    );
}



DATA(0x00539760) MessageModifier iLeftRightSave = MESSAGE_MODIFIER_NONE;

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(button, 0x004eaa10)
