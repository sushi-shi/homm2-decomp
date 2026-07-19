#include <va.h>
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

H2_ENUM_CLASS_BEGIN(ButtonSelectMode)
    SELECT_DIALOG_RESULT = 1
H2_ENUM_CLASS_END(ButtonSelectMode)

H2_ENUM_BEGIN(ButtonHotkeyConstant)
    NO_HOTKEY = -1
H2_ENUM_END(ButtonHotkeyConstant)

H2_ENUM_BEGIN(ButtonConstant)
    REPEAT_DELAY_TICKS = 60
H2_ENUM_END(ButtonConstant)

VA(0x004dd440, 0x34)
button::button(void) : widget(0, 0, 0, 0, 0, 0) {
    m_normalFrame = 0;
    m_pressedFrame = 0;
    m_iconId = 0;
    m_selectMode = 0;
    m_hotkey = NO_HOTKEY;
    m_icon = NULL;
}

VA(0x004dd4c0, 0x6e)
button::button(
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
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_normalFrame = normalFrame;
    m_pressedFrame = pressedFrame;
    m_selectMode = selectMode;
    m_hotkey = hotkey;
}

VA(0x004dd530, 0x7c)
button::button(
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
)
    : widget(x, y, width, height, id, kind) {
    u32l iconId = gpResourceManager->MakeId(iconName, 1);
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_normalFrame = normalFrame;
    m_pressedFrame = pressedFrame;
    m_selectMode = selectMode;
    m_hotkey = hotkey;
}

#define RESOURCE_NAME_CAPACITY 16

VA(0x004dd5b0, 0xeb)
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
    m_selectMode = gpResourceManager->ReadWord();
    m_hotkey = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
}

#undef RESOURCE_NAME_CAPACITY

VA(0x004dd6a0, 0x21)
inline button::~button() {
    gpResourceManager->Dispose(m_icon);
}

// Preserve the original statement stream; an inline function perturbs VC4.2 CSE in Main.
#define SET_WIDGET_MESSAGE(messageValue, commandValue, idValue)                                  \
    messageValue.payload.widget.command = commandValue;                                          \
    messageValue.type = MESSAGE_WIDGET;                                                          \
    messageValue.payload.widget.id = idValue

VA(0x004dd6d0, 0x595)
i32 button::Main(tag_message& msg) {
    if (DecodeWidgetKind(m_kind) == WIDGET_KIND_AUTO_REPEAT && (m_flags & WIDGET_FLAG_SELECTED) != 0
        && KBTickCount() > glTimers[GLOBAL_BUTTON_REPEAT_TIMER_SLOT]) {
        if ((m_flags & WIDGET_FLAG_SELECTED) == 0)
            return WIDGET_DISPATCH_CONTINUE;
        m_flags &= ~WIDGET_FLAG_SELECTED;
        Draw();
        gpWindowManager
            ->UpdateScreenRegion(m_x + m_owner->m_posX, m_y + m_owner->m_posY, m_width, m_height);
        SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_DESELECT, m_id);
        msg.payload.widget.parameter = iLeftRightSave;
        iLeftRightSave = 0;
        return WIDGET_DISPATCH_FORWARD;
    }

    if ((m_flags & WIDGET_FLAG_ENABLED) == 0) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return WIDGET_DISPATCH_CONTINUE;
    }

    i32 eventType = msg.type;
    switch (eventType) {
        case MESSAGE_KEY_DOWN:
            if ((m_flags & WIDGET_FLAG_ENABLED) != 0 && (m_flags & WIDGET_FLAG_DRAW) != 0
                && (m_flags & WIDGET_FLAG_DIMMED) == 0) {
                if (m_hotkey != NO_HOTKEY && m_hotkey == msg.payload.keyboard.keyCode)
                    return Select(msg);
                return WIDGET_DISPATCH_CONTINUE;
            }
            break;

        case MESSAGE_KEY_UP:
            if ((m_flags & WIDGET_FLAG_ENABLED) != 0 && (m_flags & WIDGET_FLAG_DRAW) != 0
                && (m_flags & WIDGET_FLAG_DIMMED) == 0) {
                if (m_hotkey == NO_HOTKEY || m_hotkey != msg.payload.keyboard.keyCode)
                    return WIDGET_DISPATCH_CONTINUE;
                i16 keyFlags = m_flags;
                if ((keyFlags & WIDGET_FLAG_SELECTED) == 0)
                    return WIDGET_DISPATCH_CONTINUE;
                keyFlags &= ~WIDGET_FLAG_SELECTED;
                m_flags = keyFlags;
                Draw();
                gpWindowManager->UpdateScreenRegion(
                    m_x + m_owner->m_posX,
                    m_y + m_owner->m_posY,
                    m_width,
                    m_height
                );
                SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_DESELECT, m_id);
                msg.payload.widget.parameter = iLeftRightSave;
                iLeftRightSave = 0;
                return WIDGET_DISPATCH_FORWARD;
            }
            break;

        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            if ((m_flags & WIDGET_FLAG_DRAW) == 0)
                goto normalEvent;

            i16 relativeX =
                static_cast<i16>(msg.payload.mouse.x) - static_cast<i16>(m_owner->m_posX);
            i16 relativeY =
                static_cast<i16>(msg.payload.mouse.y) - static_cast<i16>(m_owner->m_posY);
            if (eventType == MESSAGE_RIGHT_BUTTON_DOWN) {
                if (m_x <= relativeX && m_y <= relativeY && relativeX < m_x + m_width
                    && relativeY < m_y + m_height) {
                    SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_ALTERNATE_SELECT, m_id);
                    msg.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
                    return WIDGET_DISPATCH_FORWARD;
                }
                return WIDGET_DISPATCH_CONTINUE;
            }

            if ((m_flags & WIDGET_FLAG_DIMMED) == 0 && m_x <= relativeX && m_y <= relativeY
                && relativeX < m_x + m_width && relativeY < m_y + m_height) {
                Select(msg);
                while (msg.type != MESSAGE_LEFT_BUTTON_UP && msg.type != MESSAGE_RIGHT_BUTTON_UP) {
                    PollSound();
                    gpMouseManager->Main(msg);
                    if (msg.type == MESSAGE_MOUSE_MOVE) {
                        relativeX = static_cast<i16>(msg.payload.mouse.x)
                                    - static_cast<i16>(m_owner->m_posX);
                        relativeY = static_cast<i16>(msg.payload.mouse.y)
                                    - static_cast<i16>(m_owner->m_posY);
                        if (m_x > relativeX || m_y > relativeY || relativeX >= m_x + m_width
                            || relativeY >= m_y + m_height) {
                            i16 moveFlags = m_flags;
                            if ((moveFlags & WIDGET_FLAG_SELECTED) != 0) {
                                moveFlags &= ~WIDGET_FLAG_SELECTED;
                                m_flags = moveFlags;
                                Draw();
                                gpWindowManager->UpdateScreenRegion(
                                    m_x + m_owner->m_posX,
                                    m_y + m_owner->m_posY,
                                    m_width,
                                    m_height
                                );
                                SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_DESELECT, m_id);
                                msg.payload.widget.parameter = iLeftRightSave;
                                iLeftRightSave = 0;
                            }
                        } else if ((m_flags & WIDGET_FLAG_SELECTED) == 0) {
                            Select(msg);
                        }
                    }
                    Process1WindowsMessage();
                    msg = gpInputManager->GetEvent();
                }
                i16 releaseFlags = m_flags;
                if ((releaseFlags & WIDGET_FLAG_SELECTED) != 0) {
                    releaseFlags &= ~WIDGET_FLAG_SELECTED;
                    m_flags = releaseFlags;
                    Draw();
                    gpWindowManager->UpdateScreenRegion(
                        m_x + m_owner->m_posX,
                        m_y + m_owner->m_posY,
                        m_width,
                        m_height
                    );
                    SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_DESELECT, m_id);
                    msg.payload.widget.parameter = iLeftRightSave;
                    iLeftRightSave = 0;
                    return WIDGET_DISPATCH_FORWARD;
                }
                return WIDGET_DISPATCH_CONSUME;
            }
            return WIDGET_DISPATCH_CONTINUE;
        }

        case MESSAGE_LEFT_BUTTON_UP: {
            i16 releaseFlags = m_flags;
            if ((releaseFlags & WIDGET_FLAG_DRAW) != 0
                && (releaseFlags & WIDGET_FLAG_SELECTED) != 0) {
                releaseFlags &= ~WIDGET_FLAG_SELECTED;
                m_flags = releaseFlags;
                Draw();
                gpWindowManager->UpdateScreenRegion(
                    m_x + m_owner->m_posX,
                    m_y + m_owner->m_posY,
                    m_width,
                    m_height
                );
                SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_DESELECT, m_id);
                msg.payload.widget.parameter = iLeftRightSave;
                iLeftRightSave = 0;
                return WIDGET_DISPATCH_FORWARD;
            }
            goto normalEvent;
        }

        case MESSAGE_WIDGET:
            if (msg.payload.widget.command == WIDGET_COMMAND_REPLACE_ICON) {
                if (msg.payload.widget.id == m_iconId) {
                    m_iconId = msg.payload.widget.data.value;
                    gpResourceManager->Dispose(m_icon);
                    m_icon = gpResourceManager->GetIcon(msg.payload.widget.data.value);
                }
                return WIDGET_DISPATCH_CONTINUE;
            }
            goto normalEvent;

        default:
            goto normalEvent;
    }

normalEvent:
    return widget::Main(msg);
}

VA(0x004ddc70, 0x96)
i16 button::Select(struct tag_message& msg) {
    heroWindow* window = m_owner;
    i16 x = static_cast<i16>(window->m_posX + m_x);
    i16 y = static_cast<i16>(window->m_posY + m_y);
    m_icon->DrawToBuffer(x, y, m_pressedFrame, 0);
    gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
    m_flags |= WIDGET_FLAG_SELECTED;
    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.id = m_id;
    if (m_selectMode == IDX(SELECT_DIALOG_RESULT)) {
        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
    } else {
        msg.payload.widget.command = WIDGET_COMMAND_SELECT;
    }
    glTimers[GLOBAL_BUTTON_REPEAT_TIMER_SLOT] = KBTickCount() + REPEAT_DELAY_TICKS;
    iLeftRightSave = msg.payload.widget.parameter & MESSAGE_MODIFIER_BUTTON_MASK;
    return WIDGET_DISPATCH_FORWARD;
}

VA(0x004ddd10, 0x83)
i16 button::Deselect(struct tag_message& msg) {
    i16 flags = m_flags;
    if ((flags & WIDGET_FLAG_SELECTED) == 0)
        return WIDGET_DISPATCH_CONTINUE;
    flags &= ~WIDGET_FLAG_SELECTED;
    m_flags = flags;
    Draw();
    gpWindowManager
        ->UpdateScreenRegion(m_x + m_owner->m_posX, m_y + m_owner->m_posY, m_width, m_height);
    SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_DESELECT, m_id);
    msg.payload.widget.parameter = iLeftRightSave;
    iLeftRightSave = 0;
    return WIDGET_DISPATCH_FORWARD;
}

#undef SET_WIDGET_MESSAGE

VA(0x004ddda0, 0x55)
void button::Draw(void) {
    heroWindow* win = m_owner;
    if ((m_flags & WIDGET_FLAG_SELECTED) != 0) {
        m_icon->DrawToBuffer(m_x + win->m_posX, m_y + win->m_posY, m_pressedFrame, 0);
        return;
    }
    m_icon->DrawToBuffer(m_x + win->m_posX, m_y + win->m_posY, m_normalFrame, 0);
}


VTBL(button, 0x004ebaf0);

DATA(0x0052125c) i32 iLeftRightSave = 0;
