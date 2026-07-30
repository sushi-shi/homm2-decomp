#include <Ints.h>
#include <string.h>
#include <PLATFORM/Input.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <PLATFORM/Graphics.h>
#include <BASE/inputManager.h>
#include <BASE/INPUTMGR.h>
#include <BASE/INPUTMGR_TYPES.h>
#include <BASE/message.h>

enum class InputManagerScanCodeEncoding : i32 {
    SCAN_CODE_MASK          = 0xff,
    WINDOWS_HIGH_WORD_SHIFT = 16,
    ENCODED_SCAN_CODE_SHIFT = 8,
    ASCII_DELETE_CODE       = 0x7f
};
using enum InputManagerScanCodeEncoding;

static inline u32 EncodeScanCode(u32 scanCode) {
    return scanCode << H2EnumIndex(ENCODED_SCAN_CODE_SHIFT);
}

static inline i32l PackInputPoint(platform::Point point) {
    return static_cast<i32l>(
        static_cast<u16>(point.x)
        | (static_cast<u32l>(static_cast<u16>(point.y))
           << H2EnumIndex(WINDOWS_HIGH_WORD_SHIFT))
    );
}

typedef enum InputManagerCursorBounds {
    CURSOR_INTERIOR_MIN_EXCLUSIVE   = 3,
    CURSOR_INTERIOR_MAX_X_EXCLUSIVE = 636,
    CURSOR_INTERIOR_MAX_Y_EXCLUSIVE = 476
} InputManagerCursorBounds;

typedef enum InputManagerTiming {
    CURSOR_CHECK_DELAY = 500
} InputManagerTiming;

i32 iCurSwapPalette = 0;
i32 bLastMouseOffscreen = 0;
i32 bLastOnscreenMouseColor = 0;
i32 bInCheckChangeCursor = 0;
static SInputManagerText gInputManagerText =
    {"ReleaseCapture Failed", "ReleaseCapture Failed", "inputManager"};

i32 KeyboardMessageHandler(void*, u32 message, u32, i32l messageData) {
    if (gpInputManager == NULL)
        return 1;
    if (gpInputManager->m_active != 1)
        return 1;

    tag_message* event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->payload.keyboard.modifiers = MESSAGE_MODIFIER_NONE;
    event->payload.keyboard.unknown0x14 = 0;
    event->payload.keyboard.unknown0x10 = 0;
    event->payload.keyboard.unknown0x08 = 0;
    event->payload.keyboard.keyCode = 0;
    event->type = MESSAGE_NONE;

    switch (message) {
        case platform::INPUT_MESSAGE_KEY_DOWN:
            event->type = MESSAGE_KEY_DOWN;
            event->payload.keyboard.keyCode =
                static_cast<u16>(static_cast<u32l>(messageData) >> H2EnumIndex(WINDOWS_HIGH_WORD_SHIFT))
                & H2EnumIndex(SCAN_CODE_MASK);
            event->payload.keyboard.unknown0x08 = 0;
            event->payload.keyboard.modifiers = MESSAGE_MODIFIER_NONE;
            switch (event->payload.keyboard.keyCode) {
                case INPUT_SCAN_CONTROL:
                    gpInputManager->m_modifiers |= MESSAGE_MODIFIER_CONTROL;
                    break;
                case INPUT_SCAN_LEFT_SHIFT:
                    gpInputManager->m_modifiers |= MESSAGE_MODIFIER_LEFT_SHIFT;
                    break;
                case INPUT_SCAN_RIGHT_SHIFT:
                    gpInputManager->m_modifiers |= MESSAGE_MODIFIER_RIGHT_SHIFT;
                    break;
                case INPUT_SCAN_ALT:
                    gpInputManager->m_modifiers |= MESSAGE_MODIFIER_ALT;
                    break;
            }
            break;
        case platform::INPUT_MESSAGE_KEY_UP:
            event->type = MESSAGE_KEY_UP;
            event->payload.keyboard.keyCode =
                static_cast<u16>(static_cast<u32l>(messageData) >> H2EnumIndex(WINDOWS_HIGH_WORD_SHIFT))
                & H2EnumIndex(SCAN_CODE_MASK);
            event->payload.keyboard.unknown0x08 = 0;
            event->payload.keyboard.modifiers = MESSAGE_MODIFIER_NONE;
            switch (event->payload.keyboard.keyCode) {
                case INPUT_SCAN_CONTROL:
                    gpInputManager->m_modifiers &= MESSAGE_MODIFIER_CLEAR_CONTROL_MASK;
                    break;
                case INPUT_SCAN_LEFT_SHIFT:
                    gpInputManager->m_modifiers &= MESSAGE_MODIFIER_CLEAR_LEFT_SHIFT_MASK;
                    break;
                case INPUT_SCAN_RIGHT_SHIFT:
                    gpInputManager->m_modifiers &= MESSAGE_MODIFIER_CLEAR_RIGHT_SHIFT_MASK;
                    break;
                case INPUT_SCAN_ALT:
                    gpInputManager->m_modifiers &= MESSAGE_MODIFIER_CLEAR_ALT_MASK;
                    break;
            }
            break;
    }

    if (event->type != MESSAGE_NONE) {
        event->payload.keyboard.modifiers = gpInputManager->m_modifiers;
        gpInputManager->m_writeIndex++;
        gpInputManager->m_writeIndex %= H2EnumIndex(INPUT_EVENT_RING_CAPACITY);
        if (gpInputManager->m_writeIndex == gpInputManager->m_readIndex) {
            gpInputManager->m_readIndex++;
            gpInputManager->m_readIndex %= H2EnumIndex(INPUT_EVENT_RING_CAPACITY);
        }
        gpInputManager->m_field_0x85a = 0;

        if (gpWindowManager->m_active == 1) {
            if (event->type == MESSAGE_KEY_DOWN && event->payload.keyboard.keyCode == INPUT_SCAN_F12
                && (event->payload.keyboard.modifiers
                    & (MESSAGE_MODIFIER_RIGHT_SHIFT | MESSAGE_MODIFIER_LEFT_SHIFT))
                       != MESSAGE_MODIFIER_NONE)
                gpWindowManager->ScreenShot();
            if (event->type == MESSAGE_KEY_DOWN
                && event->payload.keyboard.keyCode == INPUT_SCAN_F1) {
                SetFullScreenStatus(0);
                platform::ShowHelp();
            }
            if (event->type == MESSAGE_KEY_DOWN && event->payload.keyboard.keyCode == INPUT_SCAN_F4)
                SetFullScreenStatus(1 - gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen);
        }
    }
    return event->type == MESSAGE_NONE;
}

i32 MouseMessageHandler(void*, u32 message, u32, i32l messageData) {
    if (gpInputManager == NULL)
        return 1;
    if (gpInputManager->m_active != 1)
        return 1;
    if (gpInputManager->m_mouseMessageActive != 0)
        return 1;
    gpInputManager->m_mouseMessageActive = 1;

    tag_message* event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->payload.mouse.modifiers = MESSAGE_MODIFIER_NONE;
    event->payload.mouse.screenY = 0;
    event->payload.mouse.screenX = 0;
    event->payload.mouse.y = 0;
    event->payload.mouse.x = 0;
    event->type = MESSAGE_NONE;

    switch (message - platform::INPUT_MESSAGE_MOUSE_MOVE) {
        case platform::INPUT_MESSAGE_MOUSE_MOVE - platform::INPUT_MESSAGE_MOUSE_MOVE:
            event->type = MESSAGE_MOUSE_MOVE;
            break;
        case platform::INPUT_MESSAGE_LEFT_DOWN - platform::INPUT_MESSAGE_MOUSE_MOVE:
            event->type = MESSAGE_LEFT_BUTTON_DOWN;
            break;
        case platform::INPUT_MESSAGE_LEFT_UP - platform::INPUT_MESSAGE_MOUSE_MOVE:
            event->type = MESSAGE_LEFT_BUTTON_UP;
            break;
        case platform::INPUT_MESSAGE_LEFT_DOUBLE - platform::INPUT_MESSAGE_MOUSE_MOVE:
            event->type = MESSAGE_LEFT_BUTTON_DOWN;
            break;
        case platform::INPUT_MESSAGE_RIGHT_DOWN - platform::INPUT_MESSAGE_MOUSE_MOVE:
            event->type = MESSAGE_RIGHT_BUTTON_DOWN;
            break;
        case platform::INPUT_MESSAGE_RIGHT_UP - platform::INPUT_MESSAGE_MOUSE_MOVE:
            event->type = MESSAGE_RIGHT_BUTTON_UP;
            break;
        case platform::INPUT_MESSAGE_RIGHT_DOUBLE - platform::INPUT_MESSAGE_MOUSE_MOVE:
            event->type = MESSAGE_RIGHT_BUTTON_DOWN;
            break;
        default:
            goto afterMouseCoordinates;
    }

    event->payload.mouse.x = static_cast<i16>(messageData);
    event->payload.mouse.y =
        static_cast<i16>(static_cast<u32l>(messageData) >> H2EnumIndex(WINDOWS_HIGH_WORD_SHIFT));
    event->payload.mouse.screenX = event->payload.mouse.x;
    event->payload.mouse.screenY = event->payload.mouse.y;

    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen == 0 && gConfig.gfx[H2EnumIndex(giCurExe)].colorMouseCursor == 0
        && platform::Ticks() > iLastBWOnScreenCheck
        && event->payload.mouse.x > CURSOR_INTERIOR_MIN_EXCLUSIVE
        && event->payload.mouse.x < CURSOR_INTERIOR_MAX_X_EXCLUSIVE
        && event->payload.mouse.y > CURSOR_INTERIOR_MIN_EXCLUSIVE
        && event->payload.mouse.y < CURSOR_INTERIOR_MAX_Y_EXCLUSIVE) {
        iLastBWOnScreenCheck = platform::Ticks() + CURSOR_CHECK_DELAY;
        gpMouseManager->SetPointer(MOUSE_KEEP_CURRENT_FRAME);
    }

afterMouseCoordinates:
    if (message == platform::INPUT_MESSAGE_MOUSE_MOVE && gpMouseManager != NULL) {
        i32 y = event->payload.mouse.y;
        i32 x = event->payload.mouse.x;
        if (bInCheckChangeCursor == 0 && gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen == 0
            && gConfig.gfx[H2EnumIndex(giCurExe)].colorMouseCursor != 0) {
            bInCheckChangeCursor = 1;
            if (x >= 0 && x < MOUSE_SCREEN_WIDTH && y >= 0 && y < MOUSE_SCREEN_HEIGHT) {
                if (bLastMouseOffscreen != 0) {
                    bLastMouseOffscreen = 0;
                    gpMouseManager->SetPointer(MOUSE_KEEP_CURRENT_FRAME);
                }
                if (bLastOnscreenMouseColor != gbColorMice)
                    gpMouseManager->SetColorMice(1);
            } else if (bLastMouseOffscreen == 0) {
                bLastMouseOffscreen = 1;
                bLastOnscreenMouseColor = gbColorMice;
                if (gbColorMice != 0)
                    gpMouseManager->SetColorMice(0);
            }
            bInCheckChangeCursor = 0;
        }
    }

    if (event->type != MESSAGE_NONE) {
        event->payload.mouse.modifiers = gpInputManager->m_modifiers;
        gpInputManager->m_writeIndex++;
        gpInputManager->m_writeIndex %= H2EnumIndex(INPUT_EVENT_RING_CAPACITY);
        i32 readIndex = gpInputManager->m_readIndex;
        if (gpInputManager->m_writeIndex == readIndex) {
            gpInputManager->m_readIndex = readIndex + 1;
            gpInputManager->m_readIndex %= H2EnumIndex(INPUT_EVENT_RING_CAPACITY);
        }
    }
    gpInputManager->m_mouseMessageActive = 0;
    return event->type == MESSAGE_NONE;
}

static void PlatformEventHandler(const platform::Event& event) {
    using Type = platform::Event::Type;

    switch (event.type) {
        case Type::KeyDown:
        case Type::KeyUp:
            KeyboardMessageHandler(
                nullptr,
                event.type == Type::KeyDown
                    ? platform::INPUT_MESSAGE_KEY_DOWN
                    : platform::INPUT_MESSAGE_KEY_UP,
                0,
                static_cast<i32l>(
                    event.scanCode << H2EnumIndex(WINDOWS_HIGH_WORD_SHIFT)
                )
            );
            break;
        case Type::MouseMove:
            MouseMessageHandler(
                nullptr,
                platform::INPUT_MESSAGE_MOUSE_MOVE,
                0,
                PackInputPoint(event.position)
            );
            break;
        case Type::MouseDown:
        case Type::MouseUp: {
            u32 message;
            if (event.button == platform::MouseButton::Right) {
                message = event.type == Type::MouseDown
                    ? platform::INPUT_MESSAGE_RIGHT_DOWN
                    : platform::INPUT_MESSAGE_RIGHT_UP;
            } else {
                message = event.type == Type::MouseDown
                    ? platform::INPUT_MESSAGE_LEFT_DOWN
                    : platform::INPUT_MESSAGE_LEFT_UP;
            }
            MouseMessageHandler(nullptr, message, 0, PackInputPoint(event.position));
            break;
        }
        default:
            break;
    }
}

inputManager::inputManager(void) : baseManager() {
    m_active = false;
    m_mouseMessageActive = 0;
    m_requestedPriority = 1;
    field_0x84e = 0;
    field_0x742 = 0;
    field_0x746 = 0;
    field_0x74a = 1;
    m_keyCodeType = INPUT_KEY_CODE_SCAN;
    field_0x866 = 0;
    field_0x862 = -1;
}

static inline void ResetEventQueue(inputManager* manager) {
    manager->m_writeIndex = 0;
    manager->m_readIndex = 0;
}

i32 inputManager::Open(i32 priority) {
    memset(m_eventRing, 0, sizeof(m_eventRing));
    ResetEventQueue(this);
    m_requestedPriority = priority;
    m_modifiers = MESSAGE_MODIFIER_NONE;
    MakeScanCodeTable();
    m_messageMask = BASE_MANAGER_ACCEPT_MOUSE_MOVE;
    m_priority = INPUT_MANAGER_PRIORITY;
    m_active = true;
    platform::SetEventHandler(PlatformEventHandler);
    strcpy(m_name, gInputManagerText.managerName);
    return 0;
}

void inputManager::Close(void) {
    if (m_active == true) {
        platform::SetEventHandler(nullptr);
        ResetEventQueue(this);
        m_requestedPriority = 0;
        m_active = false;
    }
}

MessageDispatchResult inputManager::Main(struct tag_message&) {
    return MESSAGE_DISPATCH_CONTINUE;
}

void inputManager::Flush(void) {
    ResetEventQueue(this);
}

static inline void InitializeEmptyEvent(tag_message& event) {
    event.type = MESSAGE_NONE;
    event.payload.unknown.unknown0x08 = 0;
    event.payload.unknown.unknown0x04 = 0;
    event.payload.unknown.unknown0x0c = 0;
}

tag_message inputManager::GetEvent(void) {
    tag_message event;
    PollSound();
    if (gpInputManager->m_active == 1 && m_readIndex != m_writeIndex) {
        event = m_eventRing[m_readIndex];
        m_readIndex++;
        m_readIndex %= H2EnumIndex(INPUT_EVENT_RING_CAPACITY);
        if (event.type == MESSAGE_KEY_DOWN && m_keyCodeType == INPUT_KEY_CODE_ASCII)
            AsciiConvert(event);
    } else {
        event.type = MESSAGE_NONE;
        event.payload.unknown.unknown0x08 = 0;
        event.payload.unknown.unknown0x04 = 0;
        event.payload.unknown.unknown0x0c = 0;
    }
    return event;
}

tag_message inputManager::PeekEvent(void) {
    tag_message event;
    PollSound();
    if (gpInputManager->m_active == 1 && m_readIndex != m_writeIndex) {
        event = m_eventRing[m_readIndex];
        m_readIndex = m_readIndex % H2EnumIndex(INPUT_EVENT_RING_CAPACITY);
        if (event.type == MESSAGE_KEY_DOWN && m_keyCodeType == INPUT_KEY_CODE_ASCII)
            AsciiConvert(event);
    } else {
        event.type = MESSAGE_NONE;
        event.payload.unknown.unknown0x08 = 0;
        event.payload.unknown.unknown0x04 = 0;
        event.payload.unknown.unknown0x0c = 0;
    }
    return event;
}

void inputManager::SetMouseCoords(i32, i32) {}

void inputManager::SetKeyCodeType(
    InputManagerKeyCodeType keyCodeType
) {
    m_keyCodeType = keyCodeType;
    ResetEventQueue(this);
}

void inputManager::AsciiConvert(tag_message& event) {
    if ((event.payload.keyboard.keyCode >= INPUT_SCAN_F1
         && event.payload.keyboard.keyCode <= INPUT_SCAN_F10)
        || event.payload.keyboard.keyCode == INPUT_SCAN_F11
        || event.payload.keyboard.keyCode == INPUT_SCAN_F12)
        event.payload.keyboard.keyCode = m_keyState[event.payload.keyboard.keyCode];
    else
        event.payload.keyboard.keyCode =
            m_keyState[event.payload.keyboard.keyCode] & H2EnumIndex(SCAN_CODE_MASK);

    MessageModifier modifiers = event.payload.keyboard.modifiers & MESSAGE_MODIFIER_SHIFT_KEYS;
    if (modifiers == MESSAGE_MODIFIER_NONE) {
        i32 value = event.payload.keyboard.keyCode;
        if (value > 'A' - 1 && value < 'Z' + 1) {
            value += 'a' - 'A';
            event.payload.keyboard.keyCode = value;
        }
    }
    if (modifiers != MESSAGE_MODIFIER_NONE) {
        switch (event.payload.keyboard.keyCode) {
            case '\'':
                event.payload.keyboard.keyCode = '"';
                return;
            case ',':
                event.payload.keyboard.keyCode = '<';
                return;
            case '-':
                event.payload.keyboard.keyCode = '_';
                return;
            case '.':
                event.payload.keyboard.keyCode = '>';
                return;
            case '/':
                event.payload.keyboard.keyCode = '?';
                return;
            case '0':
                event.payload.keyboard.keyCode = ')';
                return;
            case '1':
                event.payload.keyboard.keyCode = '!';
                return;
            case '2':
                event.payload.keyboard.keyCode = '@';
                return;
            case '3':
                event.payload.keyboard.keyCode = '#';
                return;
            case '4':
                event.payload.keyboard.keyCode = '$';
                return;
            case '5':
                event.payload.keyboard.keyCode = '%';
                return;
            case '6':
                event.payload.keyboard.keyCode = '^';
                return;
            case '7':
                event.payload.keyboard.keyCode = '&';
                return;
            case '8':
                event.payload.keyboard.keyCode = '*';
                return;
            case '9':
                event.payload.keyboard.keyCode = '(';
                return;
            case ';':
                event.payload.keyboard.keyCode = ':';
                return;
            case '=':
                event.payload.keyboard.keyCode = '+';
                return;
            case '[':
                event.payload.keyboard.keyCode = '{';
                return;
            case '\\':
                event.payload.keyboard.keyCode = '|';
                return;
            case ']':
                event.payload.keyboard.keyCode = '}';
                return;
        }
    }
}

void inputManager::MakeScanCodeTable(void) {
    for (u32 scanCode = 0; scanCode < INPUT_SCAN_CODE_CAPACITY; scanCode++)
        m_keyState[scanCode] = EncodeScanCode(scanCode);

    m_keyState[H2EnumIndex(INPUT_SCAN_NONE)] = 0;
    m_keyState[H2EnumIndex(INPUT_SCAN_ESCAPE)] = '\x1b';
    m_keyState[H2EnumIndex(INPUT_SCAN_1)] = '1';
    m_keyState[H2EnumIndex(INPUT_SCAN_2)] = '2';
    m_keyState[H2EnumIndex(INPUT_SCAN_3)] = '3';
    m_keyState[H2EnumIndex(INPUT_SCAN_4)] = '4';
    m_keyState[H2EnumIndex(INPUT_SCAN_5)] = '5';
    m_keyState[H2EnumIndex(INPUT_SCAN_6)] = '6';
    m_keyState[H2EnumIndex(INPUT_SCAN_7)] = '7';
    m_keyState[H2EnumIndex(INPUT_SCAN_8)] = '8';
    m_keyState[H2EnumIndex(INPUT_SCAN_9)] = '9';
    m_keyState[H2EnumIndex(INPUT_SCAN_0)] = '0';
    m_keyState[H2EnumIndex(INPUT_SCAN_MINUS)] = '-';
    m_keyState[H2EnumIndex(INPUT_SCAN_EQUALS)] = '=';
    m_keyState[H2EnumIndex(INPUT_SCAN_BACKSPACE)] = H2EnumIndex(ASCII_DELETE_CODE);
    m_keyState[H2EnumIndex(INPUT_SCAN_TAB)] = '\t';
    m_keyState[H2EnumIndex(INPUT_SCAN_Q)] = 'Q';
    m_keyState[H2EnumIndex(INPUT_SCAN_W)] = 'W';
    m_keyState[H2EnumIndex(INPUT_SCAN_E)] = 'E';
    m_keyState[H2EnumIndex(INPUT_SCAN_R)] = 'R';
    m_keyState[H2EnumIndex(INPUT_SCAN_T)] = 'T';
    m_keyState[H2EnumIndex(INPUT_SCAN_Y)] = 'Y';
    m_keyState[H2EnumIndex(INPUT_SCAN_U)] = 'U';
    m_keyState[H2EnumIndex(INPUT_SCAN_I)] = 'I';
    m_keyState[H2EnumIndex(INPUT_SCAN_O)] = 'O';
    m_keyState[H2EnumIndex(INPUT_SCAN_P)] = 'P';
    m_keyState[H2EnumIndex(INPUT_SCAN_LEFT_BRACKET)] = '[';
    m_keyState[H2EnumIndex(INPUT_SCAN_RIGHT_BRACKET)] = ']';
    m_keyState[H2EnumIndex(INPUT_SCAN_ENTER)] = '\n';
    m_keyState[H2EnumIndex(INPUT_SCAN_CONTROL)] = EncodeScanCode(INPUT_SCAN_CONTROL);
    m_keyState[H2EnumIndex(INPUT_SCAN_A)] = 'A';
    m_keyState[H2EnumIndex(INPUT_SCAN_S)] = 'S';
    m_keyState[H2EnumIndex(INPUT_SCAN_D)] = 'D';
    m_keyState[H2EnumIndex(INPUT_SCAN_F)] = 'F';
    m_keyState[H2EnumIndex(INPUT_SCAN_G)] = 'G';
    m_keyState[H2EnumIndex(INPUT_SCAN_H)] = 'H';
    m_keyState[H2EnumIndex(INPUT_SCAN_J)] = 'J';
    m_keyState[H2EnumIndex(INPUT_SCAN_K)] = 'K';
    m_keyState[H2EnumIndex(INPUT_SCAN_L)] = 'L';
    m_keyState[H2EnumIndex(INPUT_SCAN_SEMICOLON)] = '\'';
    m_keyState[H2EnumIndex(INPUT_SCAN_APOSTROPHE)] = '\'';
    m_keyState[H2EnumIndex(INPUT_SCAN_GRAVE)] = EncodeScanCode(INPUT_SCAN_GRAVE);
    m_keyState[H2EnumIndex(INPUT_SCAN_LEFT_SHIFT)] = EncodeScanCode(INPUT_SCAN_LEFT_SHIFT);
    m_keyState[H2EnumIndex(INPUT_SCAN_BACKSLASH)] = '\\';
    m_keyState[H2EnumIndex(INPUT_SCAN_Z)] = 'Z';
    m_keyState[H2EnumIndex(INPUT_SCAN_X)] = 'X';
    m_keyState[H2EnumIndex(INPUT_SCAN_C)] = 'C';
    m_keyState[H2EnumIndex(INPUT_SCAN_V)] = 'V';
    m_keyState[H2EnumIndex(INPUT_SCAN_B)] = 'B';
    m_keyState[H2EnumIndex(INPUT_SCAN_N)] = 'N';
    m_keyState[H2EnumIndex(INPUT_SCAN_M)] = 'M';
    m_keyState[H2EnumIndex(INPUT_SCAN_COMMA)] = ',';
    m_keyState[H2EnumIndex(INPUT_SCAN_PERIOD)] = '.';
    m_keyState[H2EnumIndex(INPUT_SCAN_SLASH)] = '/';
    m_keyState[H2EnumIndex(INPUT_SCAN_RIGHT_SHIFT)] = EncodeScanCode(INPUT_SCAN_RIGHT_SHIFT);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_MULTIPLY)] = '*';
    m_keyState[H2EnumIndex(INPUT_SCAN_ALT)] = EncodeScanCode(INPUT_SCAN_ALT);
    m_keyState[H2EnumIndex(INPUT_SCAN_SPACE)] = ' ';
    m_keyState[H2EnumIndex(INPUT_SCAN_CAPS_LOCK)] = EncodeScanCode(INPUT_SCAN_CAPS_LOCK);
    m_keyState[H2EnumIndex(INPUT_SCAN_F1)] = EncodeScanCode(INPUT_SCAN_F1);
    m_keyState[H2EnumIndex(INPUT_SCAN_F2)] = EncodeScanCode(INPUT_SCAN_F2);
    m_keyState[H2EnumIndex(INPUT_SCAN_F3)] = EncodeScanCode(INPUT_SCAN_F3);
    m_keyState[H2EnumIndex(INPUT_SCAN_F4)] = EncodeScanCode(INPUT_SCAN_F4);
    m_keyState[H2EnumIndex(INPUT_SCAN_F5)] = EncodeScanCode(INPUT_SCAN_F5);
    m_keyState[H2EnumIndex(INPUT_SCAN_F6)] = EncodeScanCode(INPUT_SCAN_F6);
    m_keyState[H2EnumIndex(INPUT_SCAN_F7)] = EncodeScanCode(INPUT_SCAN_F7);
    m_keyState[H2EnumIndex(INPUT_SCAN_F8)] = EncodeScanCode(INPUT_SCAN_F8);
    m_keyState[H2EnumIndex(INPUT_SCAN_F9)] = EncodeScanCode(INPUT_SCAN_F9);
    m_keyState[H2EnumIndex(INPUT_SCAN_F10)] = EncodeScanCode(INPUT_SCAN_F10);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUM_LOCK)] = EncodeScanCode(INPUT_SCAN_NUM_LOCK);
    m_keyState[H2EnumIndex(INPUT_SCAN_SCROLL_LOCK)] = EncodeScanCode(INPUT_SCAN_SCROLL_LOCK);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_7)] = EncodeScanCode(INPUT_SCAN_NUMPAD_7);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_8)] = EncodeScanCode(INPUT_SCAN_NUMPAD_8);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_9)] = EncodeScanCode(INPUT_SCAN_NUMPAD_9);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_MINUS)] = '-';
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_4)] = EncodeScanCode(INPUT_SCAN_NUMPAD_4);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_5)] = EncodeScanCode(INPUT_SCAN_NUMPAD_5);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_6)] = EncodeScanCode(INPUT_SCAN_NUMPAD_6);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_PLUS)] = '+';
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_1)] = EncodeScanCode(INPUT_SCAN_NUMPAD_1);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_2)] = EncodeScanCode(INPUT_SCAN_NUMPAD_2);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_3)] = EncodeScanCode(INPUT_SCAN_NUMPAD_3);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_0)] = EncodeScanCode(INPUT_SCAN_NUMPAD_0);
    m_keyState[H2EnumIndex(INPUT_SCAN_NUMPAD_DELETE)] = EncodeScanCode(INPUT_SCAN_NUMPAD_DELETE);
    m_keyState[H2EnumIndex(INPUT_SCAN_SYSREQ)] = EncodeScanCode(INPUT_SCAN_SYSREQ);
    m_keyState[H2EnumIndex(INPUT_SCAN_RESERVED_55)] = EncodeScanCode(INPUT_SCAN_RESERVED_55);
    m_keyState[H2EnumIndex(INPUT_SCAN_ISO_BACKSLASH)] = EncodeScanCode(INPUT_SCAN_ISO_BACKSLASH);
    m_keyState[H2EnumIndex(INPUT_SCAN_F11)] = EncodeScanCode(INPUT_SCAN_F11);
    m_keyState[H2EnumIndex(INPUT_SCAN_F12)] = EncodeScanCode(INPUT_SCAN_F12);
}

void CheckChangeCursor(i32 x, i32 y, i32 force) {
    if (bInCheckChangeCursor != 0)
        return;
    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen != 0 && force == 0)
        return;
    if (gConfig.gfx[H2EnumIndex(giCurExe)].colorMouseCursor == 0)
        return;

    bInCheckChangeCursor = 1;
    if (force != 0 || (x >= 0 && x < MOUSE_SCREEN_WIDTH && y >= 0 && y < MOUSE_SCREEN_HEIGHT)) {
        if (bLastMouseOffscreen != 0) {
            bLastMouseOffscreen = 0;
            gpMouseManager->SetPointer(MOUSE_KEEP_CURRENT_FRAME);
        }
        if (gbColorMice != bLastOnscreenMouseColor)
            gpMouseManager->SetColorMice(1);
    } else if (bLastMouseOffscreen == 0) {
        bLastMouseOffscreen = 1;
        if ((bLastOnscreenMouseColor = gbColorMice) != 0)
            gpMouseManager->SetColorMice(0);
    }
    bInCheckChangeCursor = 0;
}

void inputManager::ForceMouseMove(void) {
    i32 mouseMessageActive = gpInputManager->m_mouseMessageActive;
    if (mouseMessageActive != 0)
        return;
    gpInputManager->m_mouseMessageActive = 1;

    tag_message* event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->type = MESSAGE_MOUSE_MOVE;
    gpMouseManager->MouseCoords(event->payload.mouse.x, event->payload.mouse.y);
    event->payload.mouse.screenX = event->payload.mouse.x;
    event->payload.mouse.screenY = event->payload.mouse.y;
    event->payload.mouse.modifiers = gpInputManager->m_modifiers;
    gpInputManager->m_writeIndex++;
    gpInputManager->m_writeIndex %= H2EnumIndex(INPUT_EVENT_RING_CAPACITY);
    if (gpInputManager->m_writeIndex == gpInputManager->m_readIndex) {
        gpInputManager->m_readIndex++;
        gpInputManager->m_readIndex %= H2EnumIndex(INPUT_EVENT_RING_CAPACITY);
    }
    gpInputManager->m_mouseMessageActive = 0;
}


i32 iLastBWOnScreenCheck;
