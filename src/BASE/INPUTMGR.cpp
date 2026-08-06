#include <va.h>
#include <string.h>
#include <windows.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/wingraph.h>
#include <BASE/inputManager.h>
#include <BASE/INPUTMGR.h>
#include <BASE/message.h>

H2_ENUM_CLASS_BEGIN(InputManagerScanCodeEncoding)
    SCAN_CODE_MASK          = 0xff,
    WINDOWS_HIGH_WORD_SHIFT = 16,
    ENCODED_SCAN_CODE_SHIFT = 8,
    ASCII_DELETE_CODE       = 0x7f
H2_ENUM_CLASS_END(InputManagerScanCodeEncoding)

#define EncodeScanCode(scanCode) (IDX(scanCode) << IDX(ENCODED_SCAN_CODE_SHIFT))

H2_ENUM_BEGIN(InputManagerCursorBounds)
    CURSOR_INTERIOR_MIN_EXCLUSIVE   = 3,
    CURSOR_INTERIOR_MAX_X_EXCLUSIVE = 636,
    CURSOR_INTERIOR_MAX_Y_EXCLUSIVE = 476
H2_ENUM_END(InputManagerCursorBounds)

H2_ENUM_BEGIN(InputManagerTiming)
    CURSOR_CHECK_DELAY = 500
H2_ENUM_END(InputManagerTiming)

i32 iCurSwapPalette = 0;
DATA(0x0053607c) i32 bLastMouseOffscreen = 0;
DATA(0x00536080) i32 bLastOnscreenMouseColor = 0;
DATA(0x00536084) i32 bInCheckChangeCursor = 0;

DATA(0x0051e51c) static u8 gInputCharacterMapCp1251[0x80] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
    0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0xdd, 0x23, 0x24, 0x25, 0x26,
    0xfd, 0x28, 0x29, 0x2a, 0x2b, 0xe1, 0x2d, 0xfe, 0xb8, 0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xc6, 0xe6, 0xc1, 0x3d, 0xde, 0xa8, 0x40,
    0xd4, 0xc8, 0xd1, 0xc2, 0xd3, 0xc0, 0xcf, 0xd0, 0xd8, 0xce, 0xcb, 0xc4, 0xdc,
    0xd2, 0xd9, 0xc7, 0xc9, 0xca, 0xdb, 0xc5, 0xc3, 0xcc, 0xd6, 0xd7, 0xcd, 0xdf,
    0xf5, 0x5c, 0xfa, 0x5e, 0x5f, 0x60, 0xf4, 0xe8, 0xf1, 0xe2, 0xf3, 0xe0, 0xef,
    0xf0, 0xf8, 0xee, 0xeb, 0xe4, 0xfc, 0xf2, 0xf9, 0xe7, 0xe9, 0xea, 0xfb, 0xe5,
    0xe3, 0xec, 0xf6, 0xf7, 0xed, 0xff, 0xd5, 0x7c, 0xda, 0x7e, 0x7f
};

VA(0x004bbf40, 0x44c)
i32 KeyboardMessageHandler(void*, u32 message, u32 virtualKey, i32l messageData) {
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
        case WM_KEYDOWN:
            event->type = MESSAGE_KEY_DOWN;
            if (virtualKey == VK_RETURN)
                event->payload.keyboard.keyCode = INPUT_SCAN_ENTER;
            else
                event->payload.keyboard.keyCode = HIWORD(messageData) & IDX(SCAN_CODE_MASK);
            event->payload.keyboard.unknown0x08 = 0;
            event->payload.keyboard.modifiers = MESSAGE_MODIFIER_NONE;
            switch (event->payload.keyboard.keyCode) {
                case INPUT_SCAN_CONTROL:
                    gpInputManager->m_modifiers |= MESSAGE_MODIFIER_CONTROL;
                    break;
                case INPUT_SCAN_ALT:
                    gpInputManager->m_modifiers |= MESSAGE_MODIFIER_ALT;
                    break;
                case INPUT_SCAN_LEFT_SHIFT:
                    gpInputManager->m_modifiers |= MESSAGE_MODIFIER_LEFT_SHIFT;
                    break;
                case INPUT_SCAN_RIGHT_SHIFT:
                    gpInputManager->m_modifiers |= MESSAGE_MODIFIER_RIGHT_SHIFT;
                    break;
            }
            break;
        case WM_KEYUP:
            event->type = MESSAGE_KEY_UP;
            if (virtualKey == VK_RETURN)
                event->payload.keyboard.keyCode = INPUT_SCAN_ENTER;
            else
                event->payload.keyboard.keyCode = HIWORD(messageData) & IDX(SCAN_CODE_MASK);
            event->payload.keyboard.unknown0x08 = 0;
            event->payload.keyboard.modifiers = MESSAGE_MODIFIER_NONE;
            switch (event->payload.keyboard.keyCode) {
                case INPUT_SCAN_CONTROL:
                    gpInputManager->m_modifiers &= MESSAGE_MODIFIER_CLEAR_CONTROL_MASK;
                    break;
                case INPUT_SCAN_ALT:
                    gpInputManager->m_modifiers &= MESSAGE_MODIFIER_CLEAR_ALT_MASK;
                    break;
                case INPUT_SCAN_LEFT_SHIFT:
                    gpInputManager->m_modifiers &= MESSAGE_MODIFIER_CLEAR_LEFT_SHIFT_MASK;
                    break;
                case INPUT_SCAN_RIGHT_SHIFT:
                    gpInputManager->m_modifiers &= MESSAGE_MODIFIER_CLEAR_RIGHT_SHIFT_MASK;
                    break;
            }
            break;
    }

    if (event->type != MESSAGE_NONE) {
        event->payload.keyboard.modifiers = gpInputManager->m_modifiers;
        gpInputManager->m_writeIndex++;
        gpInputManager->m_writeIndex %= IDX(INPUT_EVENT_RING_CAPACITY);
        if (gpInputManager->m_readIndex == gpInputManager->m_writeIndex) {
            gpInputManager->m_readIndex++;
            gpInputManager->m_readIndex %= IDX(INPUT_EVENT_RING_CAPACITY);
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
                AppCommand(hwndApp, 0, IDX(KBWIN_MENU_HELP), 0);
            }
            if (event->type == MESSAGE_KEY_DOWN && event->payload.keyboard.keyCode == INPUT_SCAN_F4)
                SetFullScreenStatus(1 - gConfig.gfx[IDX(giCurExe)].fullScreen);
        }
    }
    return event->type == MESSAGE_NONE;
}

VA(0x004bc390, 0x383)
i32 MouseMessageHandler(void*, u32 message, u32, i32l messageData) {
    if (gpInputManager == NULL)
        return 1;
    if (gpInputManager->m_active != 1)
        return 1;
    if (gpInputManager->m_mouseMessageActive != 0)
        return 1;
    gpInputManager->m_mouseMessageActive = 1;

    i32 captureReleased;
    tag_message* event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->payload.mouse.modifiers = MESSAGE_MODIFIER_NONE;
    event->payload.mouse.screenY = 0;
    event->payload.mouse.screenX = 0;
    event->payload.mouse.y = 0;
    event->payload.mouse.x = 0;
    event->type = MESSAGE_NONE;

    switch (message) {
        case WM_MOUSEMOVE:
            event->type = MESSAGE_MOUSE_MOVE;
            goto mouseCoordinates;
        case WM_LBUTTONDBLCLK:
            event->type = MESSAGE_LEFT_BUTTON_DOWN;
            goto mouseCoordinates;
        case WM_LBUTTONDOWN:
            event->type = MESSAGE_LEFT_BUTTON_DOWN;
            SetCapture(hwndApp);
            goto mouseCoordinates;
        case WM_RBUTTONDOWN:
            event->type = MESSAGE_RIGHT_BUTTON_DOWN;
            SetCapture(hwndApp);
            goto mouseCoordinates;
        case WM_RBUTTONDBLCLK:
            event->type = MESSAGE_RIGHT_BUTTON_DOWN;
            goto mouseCoordinates;
        case WM_LBUTTONUP:
            event->type = MESSAGE_LEFT_BUTTON_UP;
            captureReleased = ReleaseCapture();
            if (captureReleased == 0)
                LogStr(
                    DATA_COMPGEN(
                        0x0051e59c,
                        inputLeftReleaseCaptureFailure,
                        "ReleaseCapture Failed"
                    )
                );
            goto mouseCoordinates;
        case WM_RBUTTONUP:
            event->type = MESSAGE_RIGHT_BUTTON_UP;
            captureReleased = ReleaseCapture();
            if (captureReleased == 0)
                LogStr(
                    DATA_COMPGEN(
                        0x0051e5b4,
                        inputRightReleaseCaptureFailure,
                        "ReleaseCapture Failed"
                    )
                );
        mouseCoordinates:
            event->payload.mouse.x =
                (static_cast<i16>(messageData) * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
            event->payload.mouse.y =
                (static_cast<i16>(HIWORD(messageData)) * MOUSE_SCREEN_HEIGHT)
                / iMainWinScreenHeight;
            event->payload.mouse.screenX = event->payload.mouse.x;
            event->payload.mouse.screenY = event->payload.mouse.y;

            if (gConfig.gfx[IDX(giCurExe)].fullScreen == 0
                && gConfig.gfx[IDX(giCurExe)].colorMouseCursor == 0
                && iLastBWOnScreenCheck < KBTickCount()
                && event->payload.mouse.x > CURSOR_INTERIOR_MIN_EXCLUSIVE
                && event->payload.mouse.x < CURSOR_INTERIOR_MAX_X_EXCLUSIVE
                && event->payload.mouse.y > CURSOR_INTERIOR_MIN_EXCLUSIVE
                && event->payload.mouse.y < CURSOR_INTERIOR_MAX_Y_EXCLUSIVE) {
                iLastBWOnScreenCheck = KBTickCount() + CURSOR_CHECK_DELAY;
                gpMouseManager->SetPointer(MOUSE_KEEP_CURRENT_FRAME);
            }
            break;
        default:
            break;
    }

    if (message == WM_MOUSEMOVE && gpMouseManager != NULL) {
        CheckChangeCursor(event->payload.mouse.x, event->payload.mouse.y, 0);
    }

    if (event->type != MESSAGE_NONE) {
        event->payload.mouse.modifiers = gpInputManager->m_modifiers;
        gpInputManager->m_writeIndex++;
        gpInputManager->m_writeIndex %= IDX(INPUT_EVENT_RING_CAPACITY);
        if (gpInputManager->m_readIndex == gpInputManager->m_writeIndex) {
            gpInputManager->m_readIndex++;
            gpInputManager->m_readIndex %= IDX(INPUT_EVENT_RING_CAPACITY);
        }
    }
    gpInputManager->m_mouseMessageActive = 0;
    return event->type == MESSAGE_NONE;
}

VA(0x004bc720, 0x9e)
inputManager::inputManager(void) : baseManager() {
    m_active = false;
    m_mouseMessageActive = 0;
    m_requestedPriority = 1;
    field_0x84e = 0;
    field_0x742 = 0;
    field_0x746 = 0;
    field_0x74a = 1;
    m_keyCodeType = INPUT_KEY_CODE_SCAN;
    field_0x862 = -1;
    field_0x866 = 0;
}

static inline void ResetEventQueue(inputManager* manager) {
    manager->m_writeIndex = 0;
    manager->m_readIndex = 0;
}

VA(0x004bc7c0, 0x92)
i32 inputManager::Open(i32 priority) {
    memset(m_eventRing, 0, sizeof(m_eventRing));
    ResetEventQueue(this);
    m_requestedPriority = priority;
    m_modifiers = MESSAGE_MODIFIER_NONE;
    MakeScanCodeTable();
    m_messageMask = BASE_MANAGER_ACCEPT_MOUSE_MOVE;
    m_priority = INPUT_MANAGER_PRIORITY;
    m_active = true;
    strcpy(
        m_name,
        DATA_COMPGEN(0x0051e5cc, inputManagerName, "inputManager")
    );
    return 0;
}

VA(0x004bc860, 0x47)
void inputManager::Close(void) {
    if (m_active != true)
        return;

    ResetEventQueue(this);
    m_requestedPriority = 0;
    m_active = false;
}

VA(0x004bc8b0, 0xf)
MessageDispatchResult inputManager::Main(struct tag_message&) {
    return MESSAGE_DISPATCH_CONTINUE;
}

VA(0x004bc8c0, 0x25)
void inputManager::Flush(void) {
    ResetEventQueue(this);
}

static inline void InitializeEmptyEvent(tag_message& event) {
    event.type = MESSAGE_NONE;
    event.payload.widget.id = 0;
    event.payload.widget.command = BaseWidgetCommand(event.payload.widget.id);
    event.payload.widget.parameter = IDX(event.payload.widget.command);
}

VA(0x004bc8f0, 0xd2)
tag_message inputManager::GetEvent(void) {
    tag_message event;
    PollSound();
    if (gpInputManager->m_active != 1 || m_readIndex == m_writeIndex) {
        InitializeEmptyEvent(event);
    } else {
        event = m_eventRing[m_readIndex];
        m_readIndex++;
        m_readIndex %= IDX(INPUT_EVENT_RING_CAPACITY);
        if (event.type == MESSAGE_KEY_DOWN && m_keyCodeType == INPUT_KEY_CODE_ASCII)
            AsciiConvert(event);
    }
    return event;
}

VA(0x004bc9d0, 0xbd)
tag_message inputManager::PeekEvent(void) {
    tag_message event;
    PollSound();
    if (gpInputManager->m_active != 1 || m_readIndex == m_writeIndex) {
        InitializeEmptyEvent(event);
    } else {
        event = m_eventRing[m_readIndex];
        m_readIndex %= IDX(INPUT_EVENT_RING_CAPACITY);
        if (event.type == MESSAGE_KEY_DOWN && m_keyCodeType == INPUT_KEY_CODE_ASCII)
            AsciiConvert(event);
    }
    return event;
}

VA(0x004bca90, 0xd)
void inputManager::SetMouseCoords(i32, i32) {}

VA(0x004bcaa0, 0x21)
void inputManager::SetKeyCodeType(
    H2_ENUM_PARAM(InputManagerKeyCodeType, i32) keyCodeType
) {
    m_keyCodeType = keyCodeType;
    Flush();
}

VA(0x004bcad0, 0x33)
static void TranslateInputCharacterCp1251(tag_message& event) {
    if (event.payload.keyboard.keyCode >= 0
        && event.payload.keyboard.keyCode < static_cast<i32>(sizeof(gInputCharacterMapCp1251)))
        event.payload.keyboard.keyCode =
            static_cast<i8>(gInputCharacterMapCp1251[event.payload.keyboard.keyCode]);
}

VA(0x004bcb10, 0x27e)
void inputManager::AsciiConvert(tag_message& event) {
    if ((event.payload.keyboard.keyCode >= INPUT_SCAN_F1
         && event.payload.keyboard.keyCode <= INPUT_SCAN_F10)
        || event.payload.keyboard.keyCode == INPUT_SCAN_F11
        || event.payload.keyboard.keyCode == INPUT_SCAN_F12)
        event.payload.keyboard.keyCode = m_keyState[event.payload.keyboard.keyCode];
    else
        event.payload.keyboard.keyCode =
            m_keyState[event.payload.keyboard.keyCode] & IDX(SCAN_CODE_MASK);

    if ((event.payload.keyboard.modifiers & MESSAGE_MODIFIER_SHIFT_KEYS) == MESSAGE_MODIFIER_NONE
        && event.payload.keyboard.keyCode > 'A' - 1 && event.payload.keyboard.keyCode < 'Z' + 1)
        event.payload.keyboard.keyCode += 'a' - 'A';

    if ((event.payload.keyboard.modifiers & MESSAGE_MODIFIER_SHIFT_KEYS) != MESSAGE_MODIFIER_NONE) {
        switch (event.payload.keyboard.keyCode) {
            case '1':
                event.payload.keyboard.keyCode = '!';
                break;
            case '2':
                event.payload.keyboard.keyCode = '@';
                break;
            case '3':
                event.payload.keyboard.keyCode = '#';
                break;
            case '4':
                event.payload.keyboard.keyCode = '$';
                break;
            case '5':
                event.payload.keyboard.keyCode = '%';
                break;
            case '6':
                event.payload.keyboard.keyCode = '^';
                break;
            case '7':
                event.payload.keyboard.keyCode = '&';
                break;
            case '8':
                event.payload.keyboard.keyCode = '*';
                break;
            case '9':
                event.payload.keyboard.keyCode = '(';
                break;
            case '0':
                event.payload.keyboard.keyCode = ')';
                break;
            case '-':
                event.payload.keyboard.keyCode = '_';
                break;
            case '=':
                event.payload.keyboard.keyCode = '+';
                break;
            case '[':
                event.payload.keyboard.keyCode = '{';
                break;
            case ']':
                event.payload.keyboard.keyCode = '}';
                break;
            case '\\':
                event.payload.keyboard.keyCode = '|';
                break;
            case ';':
                event.payload.keyboard.keyCode = ':';
                break;
            case '\'':
                event.payload.keyboard.keyCode = '"';
                break;
            case ',':
                event.payload.keyboard.keyCode = '<';
                break;
            case '.':
                event.payload.keyboard.keyCode = '>';
                break;
            case '/':
                event.payload.keyboard.keyCode = '?';
                break;
        }
    }
    if ((event.payload.keyboard.modifiers & MESSAGE_MODIFIER_CONTROL_KEYS) == 0)
        TranslateInputCharacterCp1251(event);
}

VA(0x004bcd90, 0x46a)
void inputManager::MakeScanCodeTable(void) {
    for (u32 scanCode = 0; scanCode < INPUT_SCAN_CODE_CAPACITY; scanCode++)
        m_keyState[scanCode] = EncodeScanCode(scanCode);

    m_keyState[IDX(INPUT_SCAN_NONE)] = 0;
    m_keyState[IDX(INPUT_SCAN_ESCAPE)] = '\x1b';
    m_keyState[IDX(INPUT_SCAN_1)] = '1';
    m_keyState[IDX(INPUT_SCAN_2)] = '2';
    m_keyState[IDX(INPUT_SCAN_3)] = '3';
    m_keyState[IDX(INPUT_SCAN_4)] = '4';
    m_keyState[IDX(INPUT_SCAN_5)] = '5';
    m_keyState[IDX(INPUT_SCAN_6)] = '6';
    m_keyState[IDX(INPUT_SCAN_7)] = '7';
    m_keyState[IDX(INPUT_SCAN_8)] = '8';
    m_keyState[IDX(INPUT_SCAN_9)] = '9';
    m_keyState[IDX(INPUT_SCAN_0)] = '0';
    m_keyState[IDX(INPUT_SCAN_MINUS)] = '-';
    m_keyState[IDX(INPUT_SCAN_EQUALS)] = '=';
    m_keyState[IDX(INPUT_SCAN_BACKSPACE)] = IDX(ASCII_DELETE_CODE);
    m_keyState[IDX(INPUT_SCAN_TAB)] = '\t';
    m_keyState[IDX(INPUT_SCAN_Q)] = 'Q';
    m_keyState[IDX(INPUT_SCAN_W)] = 'W';
    m_keyState[IDX(INPUT_SCAN_E)] = 'E';
    m_keyState[IDX(INPUT_SCAN_R)] = 'R';
    m_keyState[IDX(INPUT_SCAN_T)] = 'T';
    m_keyState[IDX(INPUT_SCAN_Y)] = 'Y';
    m_keyState[IDX(INPUT_SCAN_U)] = 'U';
    m_keyState[IDX(INPUT_SCAN_I)] = 'I';
    m_keyState[IDX(INPUT_SCAN_O)] = 'O';
    m_keyState[IDX(INPUT_SCAN_P)] = 'P';
    m_keyState[IDX(INPUT_SCAN_LEFT_BRACKET)] = '[';
    m_keyState[IDX(INPUT_SCAN_RIGHT_BRACKET)] = ']';
    m_keyState[IDX(INPUT_SCAN_ENTER)] = '\n';
    m_keyState[IDX(INPUT_SCAN_CONTROL)] = EncodeScanCode(INPUT_SCAN_CONTROL);
    m_keyState[IDX(INPUT_SCAN_A)] = 'A';
    m_keyState[IDX(INPUT_SCAN_S)] = 'S';
    m_keyState[IDX(INPUT_SCAN_D)] = 'D';
    m_keyState[IDX(INPUT_SCAN_F)] = 'F';
    m_keyState[IDX(INPUT_SCAN_G)] = 'G';
    m_keyState[IDX(INPUT_SCAN_H)] = 'H';
    m_keyState[IDX(INPUT_SCAN_J)] = 'J';
    m_keyState[IDX(INPUT_SCAN_K)] = 'K';
    m_keyState[IDX(INPUT_SCAN_L)] = 'L';
    m_keyState[IDX(INPUT_SCAN_SEMICOLON)] = ';';
    m_keyState[IDX(INPUT_SCAN_APOSTROPHE)] = '\'';
    m_keyState[IDX(INPUT_SCAN_GRAVE)] = EncodeScanCode(INPUT_SCAN_GRAVE);
    m_keyState[IDX(INPUT_SCAN_LEFT_SHIFT)] = EncodeScanCode(INPUT_SCAN_LEFT_SHIFT);
    m_keyState[IDX(INPUT_SCAN_BACKSLASH)] = '\\';
    m_keyState[IDX(INPUT_SCAN_Z)] = 'Z';
    m_keyState[IDX(INPUT_SCAN_X)] = 'X';
    m_keyState[IDX(INPUT_SCAN_C)] = 'C';
    m_keyState[IDX(INPUT_SCAN_V)] = 'V';
    m_keyState[IDX(INPUT_SCAN_B)] = 'B';
    m_keyState[IDX(INPUT_SCAN_N)] = 'N';
    m_keyState[IDX(INPUT_SCAN_M)] = 'M';
    m_keyState[IDX(INPUT_SCAN_COMMA)] = ',';
    m_keyState[IDX(INPUT_SCAN_PERIOD)] = '.';
    m_keyState[IDX(INPUT_SCAN_SLASH)] = '/';
    m_keyState[IDX(INPUT_SCAN_RIGHT_SHIFT)] = EncodeScanCode(INPUT_SCAN_RIGHT_SHIFT);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_MULTIPLY)] = '*';
    m_keyState[IDX(INPUT_SCAN_ALT)] = EncodeScanCode(INPUT_SCAN_ALT);
    m_keyState[IDX(INPUT_SCAN_SPACE)] = ' ';
    m_keyState[IDX(INPUT_SCAN_CAPS_LOCK)] = EncodeScanCode(INPUT_SCAN_CAPS_LOCK);
    m_keyState[IDX(INPUT_SCAN_F1)] = EncodeScanCode(INPUT_SCAN_F1);
    m_keyState[IDX(INPUT_SCAN_F2)] = EncodeScanCode(INPUT_SCAN_F2);
    m_keyState[IDX(INPUT_SCAN_F3)] = EncodeScanCode(INPUT_SCAN_F3);
    m_keyState[IDX(INPUT_SCAN_F4)] = EncodeScanCode(INPUT_SCAN_F4);
    m_keyState[IDX(INPUT_SCAN_F5)] = EncodeScanCode(INPUT_SCAN_F5);
    m_keyState[IDX(INPUT_SCAN_F6)] = EncodeScanCode(INPUT_SCAN_F6);
    m_keyState[IDX(INPUT_SCAN_F7)] = EncodeScanCode(INPUT_SCAN_F7);
    m_keyState[IDX(INPUT_SCAN_F8)] = EncodeScanCode(INPUT_SCAN_F8);
    m_keyState[IDX(INPUT_SCAN_F9)] = EncodeScanCode(INPUT_SCAN_F9);
    m_keyState[IDX(INPUT_SCAN_F10)] = EncodeScanCode(INPUT_SCAN_F10);
    m_keyState[IDX(INPUT_SCAN_NUM_LOCK)] = EncodeScanCode(INPUT_SCAN_NUM_LOCK);
    m_keyState[IDX(INPUT_SCAN_SCROLL_LOCK)] = EncodeScanCode(INPUT_SCAN_SCROLL_LOCK);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_7)] = EncodeScanCode(INPUT_SCAN_NUMPAD_7);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_8)] = EncodeScanCode(INPUT_SCAN_NUMPAD_8);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_9)] = EncodeScanCode(INPUT_SCAN_NUMPAD_9);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_MINUS)] = '-';
    m_keyState[IDX(INPUT_SCAN_NUMPAD_4)] = EncodeScanCode(INPUT_SCAN_NUMPAD_4);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_5)] = EncodeScanCode(INPUT_SCAN_NUMPAD_5);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_6)] = EncodeScanCode(INPUT_SCAN_NUMPAD_6);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_PLUS)] = '+';
    m_keyState[IDX(INPUT_SCAN_NUMPAD_1)] = EncodeScanCode(INPUT_SCAN_NUMPAD_1);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_2)] = EncodeScanCode(INPUT_SCAN_NUMPAD_2);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_3)] = EncodeScanCode(INPUT_SCAN_NUMPAD_3);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_0)] = EncodeScanCode(INPUT_SCAN_NUMPAD_0);
    m_keyState[IDX(INPUT_SCAN_NUMPAD_DELETE)] = EncodeScanCode(INPUT_SCAN_NUMPAD_DELETE);
    m_keyState[IDX(INPUT_SCAN_SYSREQ)] = EncodeScanCode(INPUT_SCAN_SYSREQ);
    m_keyState[IDX(INPUT_SCAN_RESERVED_55)] = EncodeScanCode(INPUT_SCAN_RESERVED_55);
    m_keyState[IDX(INPUT_SCAN_ISO_BACKSLASH)] = EncodeScanCode(INPUT_SCAN_ISO_BACKSLASH);
    m_keyState[IDX(INPUT_SCAN_F11)] = EncodeScanCode(INPUT_SCAN_F11);
    m_keyState[IDX(INPUT_SCAN_F12)] = EncodeScanCode(INPUT_SCAN_F12);
}

VA(0x004bd200, 0xfe)
void CheckChangeCursor(i32 x, i32 y, i32 force) {
    if (bInCheckChangeCursor != 0)
        return;
    if (gConfig.gfx[IDX(giCurExe)].fullScreen != 0 && force == 0)
        return;
    if (gConfig.gfx[IDX(giCurExe)].colorMouseCursor == 0)
        return;

    bInCheckChangeCursor = 1;
    if (force != 0 || (x >= 0 && x < MOUSE_SCREEN_WIDTH && y >= 0 && y < MOUSE_SCREEN_HEIGHT)) {
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

VA(0x004bd300, 0x151)
void inputManager::ForceMouseMove(void) {
    if (gpInputManager->m_mouseMessageActive != 0)
        return;
    // Retail repeats the re-entrancy guard verbatim before claiming the flag.
    if (gpInputManager->m_mouseMessageActive != 0)
        return;
    gpInputManager->m_mouseMessageActive = 1;

    tag_message* event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->type = MESSAGE_MOUSE_MOVE;
    gpMouseManager->MouseCoords(event->payload.mouse.x, event->payload.mouse.y);
    event->payload.mouse.screenX = event->payload.mouse.x;
    event->payload.mouse.screenY = event->payload.mouse.y;
    event->payload.mouse.modifiers = gpInputManager->m_modifiers;
    gpInputManager->m_writeIndex++;
    gpInputManager->m_writeIndex %= IDX(INPUT_EVENT_RING_CAPACITY);
    if (gpInputManager->m_readIndex == gpInputManager->m_writeIndex) {
        gpInputManager->m_readIndex++;
        gpInputManager->m_readIndex %= IDX(INPUT_EVENT_RING_CAPACITY);
    }
    gpInputManager->m_mouseMessageActive = 0;
}



DATA(0x00536074) i32 iLastBWOnScreenCheck;

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(inputManager, 0x004ea990)
