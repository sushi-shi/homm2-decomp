#include <Ints.h>
#include <BASE/button.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <PLATFORM/Runtime.h>
#include <BASE/mouseManager.h>
#include <BASE/inputManager.h>
#include <SOURCE/KB.h>

typedef enum ButtonHotkeyConstant {
    NO_HOTKEY = -1
} ButtonHotkeyConstant;

typedef enum ButtonConstant {
    RESOURCE_NAME_CAPACITY = 16,
    REPEAT_DELAY_TICKS     = 60
} ButtonConstant;

button::button(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_iconId = 0;
    m_icon = NULL;
    m_normalFrame = 0;
    m_pressedFrame = 0;
    m_selectMode = BUTTON_SELECT_STANDARD;
    m_hotkey = NO_HOTKEY;
}

button::button(
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
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_normalFrame = normalFrame;
    m_pressedFrame = pressedFrame;
    m_selectMode = selectMode;
    m_hotkey = hotkey;
}

button::button(
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
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    m_normalFrame = normalFrame;
    m_pressedFrame = pressedFrame;
    m_selectMode = selectMode;
    m_hotkey = hotkey;
}

void button::Read(void) {
    char iconName[RESOURCE_NAME_CAPACITY];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<i8*>(iconName));
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

inline button::~button() {
    gpResourceManager->Dispose(m_icon);
}

#define SET_WIDGET_MESSAGE(messageValue, commandValue, idValue)                                  \
    messageValue.type = MESSAGE_WIDGET;                                                          \
    messageValue.payload.widget.command = commandValue;                                          \
    messageValue.payload.widget.id = idValue

MessageDispatchResult button::Main(tag_message& msg) {
    if (m_kind == WIDGET_KIND_AUTO_REPEAT && (H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED)))
        && glTimers[GLOBAL_BUTTON_REPEAT_TIMER_SLOT] < platform::Ticks()) {
        return Deselect(msg);
    }

    if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_ENABLED)))) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return MESSAGE_DISPATCH_CONTINUE;
    }

    switch (msg.type) {
        case MESSAGE_WIDGET:
            switch (msg.payload.widget.command) {
                case WIDGET_COMMAND_REPLACE_ICON:
                    if (m_iconId == msg.payload.widget.id) {
                        m_iconId = msg.payload.widget.data.value;
                        gpResourceManager->Dispose(m_icon);
                        m_icon = gpResourceManager->GetIcon(msg.payload.widget.data.value);
                    }
                    return MESSAGE_DISPATCH_CONTINUE;
            }
            break;

        case MESSAGE_KEY_DOWN:
            if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_ENABLED))))
                break;
            if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_DRAW))))
                break;
            if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_DIMMED))))
                break;
            if (m_hotkey != NO_HOTKEY && m_hotkey == msg.payload.keyboard.keyCode)
                return Select(msg);
            return MESSAGE_DISPATCH_CONTINUE;

        case MESSAGE_KEY_UP:
            if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_ENABLED))))
                break;
            if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_DRAW))))
                break;
            if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_DIMMED))))
                break;
            if (m_hotkey != NO_HOTKEY && m_hotkey == msg.payload.keyboard.keyCode)
                return Deselect(msg);
            return MESSAGE_DISPATCH_CONTINUE;

        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_DRAW))))
                break;

            i16 x = msg.payload.mouse.x - m_owner->m_posX;
            i16 y = msg.payload.mouse.y - m_owner->m_posY;
            if (msg.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                    SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_ALTERNATE_SELECT, m_id);
                    msg.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;
                    return MESSAGE_DISPATCH_FORWARD;
                }
                return MESSAGE_DISPATCH_CONTINUE;
            }

            if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_DIMMED))) && x >= m_x && y >= m_y && x < m_x + m_width
                && y < m_y + m_height) {
                Select(msg);
                while (msg.type != MESSAGE_LEFT_BUTTON_UP && msg.type != MESSAGE_RIGHT_BUTTON_UP) {
                    PollSound();
                    gpMouseManager->Main(msg);
                    if (msg.type == MESSAGE_MOUSE_MOVE) {
                        x = msg.payload.mouse.x - m_owner->m_posX;
                        y = msg.payload.mouse.y - m_owner->m_posY;
                        if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                            if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED)))) {
                                Select(msg);
                            }
                        } else if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED)))) {
                            Deselect(msg);
                        }
                    }
                    platform::PumpEvents();
                    msg = gpInputManager->GetEvent();
                }
                if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED)))) {
                    Deselect(msg);
                    return MESSAGE_DISPATCH_FORWARD;
                }
                return MESSAGE_DISPATCH_CONSUME;
            }
            return MESSAGE_DISPATCH_CONTINUE;
        }

        case MESSAGE_LEFT_BUTTON_UP:
            if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_DRAW))))
                break;
            if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED))))
                return Deselect(msg);
            goto normalEvent;
    }

normalEvent:
    return widget::Main(msg);
}

MessageDispatchResult button::Select(struct tag_message& msg) {
    i16 x = m_owner->m_posX + m_x;
    i16 y = m_owner->m_posY + m_y;
    m_icon->DrawToBuffer(x, y, m_pressedFrame, ICON_DRAW_NORMAL);
    gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
    m_flags |= WIDGET_FLAG_SELECTED;
    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.id = m_id;
    if (m_selectMode == BUTTON_SELECT_DIALOG_RESULT) {
        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
    } else {
        msg.payload.widget.command = WIDGET_COMMAND_SELECT;
    }
    glTimers[GLOBAL_BUTTON_REPEAT_TIMER_SLOT] = platform::Ticks() + REPEAT_DELAY_TICKS;
    iLeftRightSave = msg.payload.widget.modifiers
        & MESSAGE_MODIFIER_BUTTON_MASK;
    return MESSAGE_DISPATCH_FORWARD;
}

MessageDispatchResult button::Deselect(struct tag_message& msg) {
    if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED))))
        return MESSAGE_DISPATCH_CONTINUE;
    m_flags &= ~WIDGET_FLAG_SELECTED;
    Draw();
    gpWindowManager->UpdateScreenRegion(
        m_owner->m_posX + m_x,
        m_owner->m_posY + m_y,
        m_width,
        m_height
    );
    SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_DESELECT, m_id);
    msg.payload.widget.modifiers = iLeftRightSave;
    iLeftRightSave = MESSAGE_MODIFIER_NONE;
    return MESSAGE_DISPATCH_FORWARD;
}

#undef SET_WIDGET_MESSAGE

void button::Draw(void) {
    if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED)))) {
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


MessageModifier iLeftRightSave = MESSAGE_MODIFIER_NONE;
