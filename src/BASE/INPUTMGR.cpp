// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\INPUTMGR.OBJ   from: .\basewin.lib
// functions: 15   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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
#include <BASE/INPUTMGR_TYPES.h>
#include <BASE/message.h>

DATA(0x0051f980) i32 iCurSwapPalette = 0;
DATA(0x0051f984) i32 bLastMouseOffscreen = 0;
DATA(0x0051f988) i32 bLastOnscreenMouseColor = 0;
DATA(0x0051f98c) i32 bInCheckChangeCursor = 0;
DATA(0x0051f990) static SInputManagerText gInputManagerText = {
    "ReleaseCapture Failed",
    "ReleaseCapture Failed",
    "inputManager"
};

// @data-layout-note NB09 assigns INPUTMGR one 0x50 initialized-data
// contribution at 0x11f980. Retail stores the four public integers first,
// followed by two independent 0x18 ReleaseCapture diagnostic slots and one
// 0x10 manager-name slot. The two mouse-button release paths reference owner
// addends 0x10 and 0x28; Open references addend 0x40. Together the definitions
// above reproduce the complete contribution. INPUTMGR separately owns the
// four-byte loader-zero iLastBWOnScreenCheck contribution at 0x134bc8.
// @early-stop
// The explicit 0x308-byte range is raw-exact after relocation-union masking, proving
// frame/slots and CFG. All 40 ordered sites/types and every nonlocal runtime address
// agree. Residual identities/addends are 14 local dispatch/table $L symbols rewritten
// by the delinker as this function plus local offsets, and gConfig+0x30, whose retail
// interior label is the same VA 0x00528d50.
VA(0x004cdb50, 0x308)
i32 KeyboardMessageHandler(void *, u32 message, u32, i32l messageData)
{
    if (gpInputManager == 0)
        return 1;
    if (gpInputManager->m_active != 1)
        return 1;

    tag_message *event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->payload.keyboard.modifiers = 0;
    event->payload.keyboard.unknown0x14 = 0;
    event->payload.keyboard.unknown0x10 = 0;
    event->payload.keyboard.unknown0x08 = 0;
    event->payload.keyboard.keyCode = 0;
    event->type = MESSAGE_NONE;

    switch (message) {
    case WM_KEYDOWN:
        event->type = MESSAGE_KEY_DOWN;
        event->payload.keyboard.keyCode = static_cast<u16>(static_cast<u32l>(messageData) >> 16) & INPUT_SCAN_CODE_MASK;
        event->payload.keyboard.unknown0x08 = 0;
        event->payload.keyboard.modifiers = 0;
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
    case WM_KEYUP:
        event->type = MESSAGE_KEY_UP;
        event->payload.keyboard.keyCode = static_cast<u16>(static_cast<u32l>(messageData) >> 16) & INPUT_SCAN_CODE_MASK;
        event->payload.keyboard.unknown0x08 = 0;
        event->payload.keyboard.modifiers = 0;
        switch (event->payload.keyboard.keyCode) {
        case INPUT_SCAN_CONTROL:
            gpInputManager->m_modifiers &= INPUT_CLEAR_CONTROL_MASK;
            break;
        case INPUT_SCAN_LEFT_SHIFT:
            gpInputManager->m_modifiers &= INPUT_CLEAR_LEFT_SHIFT_MASK;
            break;
        case INPUT_SCAN_RIGHT_SHIFT:
            gpInputManager->m_modifiers &= INPUT_CLEAR_RIGHT_SHIFT_MASK;
            break;
        case INPUT_SCAN_ALT:
            gpInputManager->m_modifiers &= INPUT_CLEAR_ALT_MASK;
            break;
        }
        break;
    }

    if (event->type != MESSAGE_NONE) {
        event->payload.keyboard.modifiers = gpInputManager->m_modifiers;
        gpInputManager->m_writeIndex++;
        gpInputManager->m_writeIndex %= INPUT_EVENT_RING_CAPACITY;
        if (gpInputManager->m_writeIndex == gpInputManager->m_readIndex) {
            gpInputManager->m_readIndex++;
            gpInputManager->m_readIndex %= INPUT_EVENT_RING_CAPACITY;
        }
        gpInputManager->m_field_0x85a = 0;

        if (gpWindowManager->m_active == 1) {
            if (event->type == MESSAGE_KEY_DOWN && event->payload.keyboard.keyCode == INPUT_SCAN_F12 &&
                (event->payload.keyboard.modifiers &
                 (MESSAGE_MODIFIER_RIGHT_SHIFT | MESSAGE_MODIFIER_LEFT_SHIFT)) != 0)
                gpWindowManager->ScreenShot();
            if (event->type == MESSAGE_KEY_DOWN && event->payload.keyboard.keyCode == INPUT_SCAN_F1) {
                SetFullScreenStatus(0);
                AppCommand(hwndApp, 0, KBWIN_MENU_HELP, 0);
            }
            if (event->type == MESSAGE_KEY_DOWN && event->payload.keyboard.keyCode == INPUT_SCAN_F4)
                SetFullScreenStatus(1 - gConfig.gfx[giCurExe].fullScreen);
        }
    }
    return event->type == MESSAGE_NONE;
}

// @early-stop
// The complete 0x36c-byte code-and-switch-table range is raw-exact after relocation-
// union masking, proving stack/CFG and payload accesses. Both objects now expose 55
// ordered sites. The residual is limited to delinked local switch labels and the
// compiler-local name of gInputManagerText; its two references retain owner addends
// +0/+0x18. All nonlocal runtime addresses and owner-relative addends agree,
// including gConfig +0x30/+0x34, SetCapture, and ReleaseCapture.
VA(0x004cde60, 0x36c)
i32 MouseMessageHandler(void *, u32 message, u32, i32l messageData)
{
    if (gpInputManager == 0)
        return 1;
    if (gpInputManager->m_active != 1)
        return 1;
    if (gpInputManager->m_mouseMessageActive != 0)
        return 1;
    gpInputManager->m_mouseMessageActive = 1;

    tag_message *event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->payload.mouse.modifiers = 0;
    event->payload.mouse.screenY = 0;
    event->payload.mouse.screenX = 0;
    event->payload.mouse.y = 0;
    event->payload.mouse.x = 0;
    event->type = MESSAGE_NONE;

    switch (message - WM_MOUSEMOVE) {
    case WM_MOUSEMOVE - WM_MOUSEMOVE:
        event->type = MESSAGE_MOUSE_MOVE;
        break;
    case WM_LBUTTONDOWN - WM_MOUSEMOVE:
        event->type = MESSAGE_LEFT_BUTTON_DOWN;
        SetCapture(hwndApp);
        break;
    case WM_LBUTTONUP - WM_MOUSEMOVE:
        event->type = MESSAGE_LEFT_BUTTON_UP;
        if (ReleaseCapture() == 0)
            LogStr(gInputManagerText.leftReleaseCaptureFailure);
        break;
    case WM_LBUTTONDBLCLK - WM_MOUSEMOVE:
        event->type = MESSAGE_LEFT_BUTTON_DOWN;
        break;
    case WM_RBUTTONDOWN - WM_MOUSEMOVE:
        event->type = MESSAGE_RIGHT_BUTTON_DOWN;
        SetCapture(hwndApp);
        break;
    case WM_RBUTTONUP - WM_MOUSEMOVE:
        event->type = MESSAGE_RIGHT_BUTTON_UP;
        if (ReleaseCapture() == 0)
            LogStr(gInputManagerText.rightReleaseCaptureFailure);
        break;
    case WM_RBUTTONDBLCLK - WM_MOUSEMOVE:
        event->type = MESSAGE_RIGHT_BUTTON_DOWN;
        break;
    default:
        goto afterMouseCoordinates;
    }

    event->payload.mouse.x =
        (static_cast<i16>(messageData) * MOUSE_SCREEN_WIDTH) / iMainWinScreenWidth;
    event->payload.mouse.y =
        (static_cast<i16>(static_cast<u32l>(messageData) >> 16) * MOUSE_SCREEN_HEIGHT) /
        iMainWinScreenHeight;
    event->payload.mouse.screenX = event->payload.mouse.x;
    event->payload.mouse.screenY = event->payload.mouse.y;

    if (gConfig.gfx[giCurExe].fullScreen == 0 &&
        gConfig.gfx[giCurExe].colorMouseCursor == 0 &&
        KBTickCount() > iLastBWOnScreenCheck &&
        event->payload.mouse.x > INPUT_CURSOR_INTERIOR_MIN_EXCLUSIVE &&
        event->payload.mouse.x < INPUT_CURSOR_INTERIOR_MAX_X_EXCLUSIVE &&
        event->payload.mouse.y > INPUT_CURSOR_INTERIOR_MIN_EXCLUSIVE &&
        event->payload.mouse.y < INPUT_CURSOR_INTERIOR_MAX_Y_EXCLUSIVE) {
        iLastBWOnScreenCheck = KBTickCount() + INPUT_CURSOR_CHECK_DELAY;
        gpMouseManager->SetPointer(MOUSE_KEEP_CURRENT_FRAME);
    }

afterMouseCoordinates:
    if (message == WM_MOUSEMOVE && gpMouseManager != 0) {
        i32 y = event->payload.mouse.y;
        i32 x = event->payload.mouse.x;
        if (bInCheckChangeCursor == 0 && gConfig.gfx[giCurExe].fullScreen == 0 &&
            gConfig.gfx[giCurExe].colorMouseCursor != 0) {
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
        gpInputManager->m_writeIndex %= INPUT_EVENT_RING_CAPACITY;
        i32 readIndex = gpInputManager->m_readIndex;
        if (gpInputManager->m_writeIndex == readIndex) {
            gpInputManager->m_readIndex = readIndex + 1;
            gpInputManager->m_readIndex %= INPUT_EVENT_RING_CAPACITY;
        }
    }
    gpInputManager->m_mouseMessageActive = 0;
    return event->type == MESSAGE_NONE;
}

VA(0x004ce1d0, 0x56)
inputManager::inputManager(void) : baseManager()
{
    m_active = 0;
    m_mouseMessageActive = 0;
    field_0x852 = 1;
    field_0x84e = 0;
    field_0x742 = 0;
    field_0x746 = 0;
    field_0x74a = 1;
    m_keyCodeType = INPUT_KEY_CODE_SCAN;
    field_0x866 = 0;
    field_0x862 = -1;
}

VA(0x004ce230, 0x78)
i32 inputManager::Open(i32 param_1)
{
    memset(m_eventRing, 0, sizeof(m_eventRing));
    m_writeIndex = 0;
    m_readIndex = 0;
    field_0x852 = param_1;
    m_modifiers = 0;
    MakeScanCodeTable();
    m_messageMask = 4;
    m_priority = -1;
    m_active = 1;
    strcpy(m_name, gInputManagerText.managerName);
    return 0;
}

VA(0x004ce2b0, 0x20)
void inputManager::Close(void)
{
    if (m_active == 1) {
        m_writeIndex = 0;
        m_readIndex = 0;
        field_0x852 = 0;
        m_active = 0;
    }
}

VA(0x004ce2d0, 0x5)
i32 inputManager::Main(struct tag_message &) { return 0; }

VA(0x004ce2e0, 0xf)
void inputManager::Flush(void)
{
    m_writeIndex = 0;
    m_readIndex = 0;
}

VA(0x004ce2f0, 0xa8)
tag_message inputManager::GetEvent(void)
{
    tag_message event;
    PollSound();
    if (gpInputManager->m_active == 1 && m_readIndex != m_writeIndex) {
        event = m_eventRing[m_readIndex];
        m_readIndex++;
        m_readIndex %= INPUT_EVENT_RING_CAPACITY;
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

VA(0x004ce3a0, 0xa1)
tag_message inputManager::PeekEvent(void)
{
    tag_message event;
    PollSound();
    if (gpInputManager->m_active == 1 && m_readIndex != m_writeIndex) {
        event = m_eventRing[m_readIndex];
        m_readIndex = m_readIndex % INPUT_EVENT_RING_CAPACITY;
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

VA(0x004ce450, 0x3)
void inputManager::SetMouseCoords(i32, i32) {}

VA(0x004ce460, 0x1b)
void inputManager::SetKeyCodeType(i32 keyCodeType)
{
    m_keyCodeType = static_cast<InputManagerKeyCodeType>(keyCodeType);
    m_writeIndex = 0;
    m_readIndex = 0;
}

// @semantic
// The authoritative 0x1cb-byte CodeView span and all 23 relocation occurrences align.
// After relocation masking, exactly 12 bytes differ, all at +0x32..+0x51: retail keeps
// the converted key in EAX and modifiers in ECX, while base stores the key first and
// then uses EAX for modifiers and ECX for the key. The operations, predicates, values,
// CFG, and every byte after +0x51 agree. Preloaded/split modifiers, converted-key
// local/ternary forms, their combined saved-key/saved-modifier form, duplicated
// branch-local loads/stores, and const/name variants regressed or were byte-neutral.
// The libclang AST pass has no valid mutations; 80 guarded TU-state trials across the
// prior and integrated declaration states produced no exact closure. Jump-table
// identities are delinked $L labels versus this function. Revisit on a genuine
// predecessor/header/TU-state change; register scheduling is not a permitted artifact.
VA(0x004ce480, 0x1cb)
void inputManager::AsciiConvert(tag_message &event)
{
    if ((event.payload.keyboard.keyCode >= 0x3b &&
         event.payload.keyboard.keyCode <= 0x44) ||
        event.payload.keyboard.keyCode == 0x57 ||
        event.payload.keyboard.keyCode == 0x58)
        event.payload.keyboard.keyCode =
            m_keyState[event.payload.keyboard.keyCode];
    else
        event.payload.keyboard.keyCode =
            m_keyState[event.payload.keyboard.keyCode] & 0xff;

    i32 modifiers = event.payload.keyboard.modifiers &
                    (MESSAGE_MODIFIER_RIGHT_SHIFT | MESSAGE_MODIFIER_LEFT_SHIFT);
    if (modifiers == 0) {
        i32 value = event.payload.keyboard.keyCode;
        if (value > 'A' - 1 && value < 'Z' + 1) {
            value += 'a' - 'A';
            event.payload.keyboard.keyCode = value;
        }
    }
    if (modifiers != 0) {
        switch (event.payload.keyboard.keyCode) {
        case '\'': event.payload.keyboard.keyCode = '"'; return;
        case ',': event.payload.keyboard.keyCode = '<'; return;
        case '-': event.payload.keyboard.keyCode = '_'; return;
        case '.': event.payload.keyboard.keyCode = '>'; return;
        case '/': event.payload.keyboard.keyCode = '?'; return;
        case '0': event.payload.keyboard.keyCode = ')'; return;
        case '1': event.payload.keyboard.keyCode = '!'; return;
        case '2': event.payload.keyboard.keyCode = '@'; return;
        case '3': event.payload.keyboard.keyCode = '#'; return;
        case '4': event.payload.keyboard.keyCode = '$'; return;
        case '5': event.payload.keyboard.keyCode = '%'; return;
        case '6': event.payload.keyboard.keyCode = '^'; return;
        case '7': event.payload.keyboard.keyCode = '&'; return;
        case '8': event.payload.keyboard.keyCode = '*'; return;
        case '9': event.payload.keyboard.keyCode = '('; return;
        case ';': event.payload.keyboard.keyCode = ':'; return;
        case '=': event.payload.keyboard.keyCode = '+'; return;
        case '[': event.payload.keyboard.keyCode = '{'; return;
        case '\\': event.payload.keyboard.keyCode = '|'; return;
        case ']': event.payload.keyboard.keyCode = '}'; return;
        }
    }
}

VA(0x004ce650, 0x33c)
void inputManager::MakeScanCodeTable(void)
{
    for (u32 scanCode = 0; scanCode < INPUT_SCAN_CODE_CAPACITY; scanCode++)
        m_keyState[scanCode] = scanCode << 8;

    m_keyState[INPUT_SCAN_NONE] = 0;
    m_keyState[INPUT_SCAN_ESCAPE] = '\x1b';
    m_keyState[INPUT_SCAN_1] = '1';
    m_keyState[INPUT_SCAN_2] = '2';
    m_keyState[INPUT_SCAN_3] = '3';
    m_keyState[INPUT_SCAN_4] = '4';
    m_keyState[INPUT_SCAN_5] = '5';
    m_keyState[INPUT_SCAN_6] = '6';
    m_keyState[INPUT_SCAN_7] = '7';
    m_keyState[INPUT_SCAN_8] = '8';
    m_keyState[INPUT_SCAN_9] = '9';
    m_keyState[INPUT_SCAN_0] = '0';
    m_keyState[INPUT_SCAN_MINUS] = '-';
    m_keyState[INPUT_SCAN_EQUALS] = '=';
    m_keyState[INPUT_SCAN_BACKSPACE] = 0x7f;
    m_keyState[INPUT_SCAN_TAB] = '\t';
    m_keyState[INPUT_SCAN_Q] = 'Q';
    m_keyState[INPUT_SCAN_W] = 'W';
    m_keyState[INPUT_SCAN_E] = 'E';
    m_keyState[INPUT_SCAN_R] = 'R';
    m_keyState[INPUT_SCAN_T] = 'T';
    m_keyState[INPUT_SCAN_Y] = 'Y';
    m_keyState[INPUT_SCAN_U] = 'U';
    m_keyState[INPUT_SCAN_I] = 'I';
    m_keyState[INPUT_SCAN_O] = 'O';
    m_keyState[INPUT_SCAN_P] = 'P';
    m_keyState[INPUT_SCAN_LEFT_BRACKET] = '[';
    m_keyState[INPUT_SCAN_RIGHT_BRACKET] = ']';
    m_keyState[INPUT_SCAN_ENTER] = '\n';
    m_keyState[INPUT_SCAN_CONTROL] = INPUT_SCAN_CONTROL << 8;
    m_keyState[INPUT_SCAN_A] = 'A';
    m_keyState[INPUT_SCAN_S] = 'S';
    m_keyState[INPUT_SCAN_D] = 'D';
    m_keyState[INPUT_SCAN_F] = 'F';
    m_keyState[INPUT_SCAN_G] = 'G';
    m_keyState[INPUT_SCAN_H] = 'H';
    m_keyState[INPUT_SCAN_J] = 'J';
    m_keyState[INPUT_SCAN_K] = 'K';
    m_keyState[INPUT_SCAN_L] = 'L';
    // Retail deliberately maps both physical scan keys 0x27 and 0x28 to apostrophe.
    m_keyState[INPUT_SCAN_SEMICOLON] = '\'';
    m_keyState[INPUT_SCAN_APOSTROPHE] = '\'';
    m_keyState[INPUT_SCAN_GRAVE] = INPUT_SCAN_GRAVE << 8;
    m_keyState[INPUT_SCAN_LEFT_SHIFT] = INPUT_SCAN_LEFT_SHIFT << 8;
    m_keyState[INPUT_SCAN_BACKSLASH] = '\\';
    m_keyState[INPUT_SCAN_Z] = 'Z';
    m_keyState[INPUT_SCAN_X] = 'X';
    m_keyState[INPUT_SCAN_C] = 'C';
    m_keyState[INPUT_SCAN_V] = 'V';
    m_keyState[INPUT_SCAN_B] = 'B';
    m_keyState[INPUT_SCAN_N] = 'N';
    m_keyState[INPUT_SCAN_M] = 'M';
    m_keyState[INPUT_SCAN_COMMA] = ',';
    m_keyState[INPUT_SCAN_PERIOD] = '.';
    m_keyState[INPUT_SCAN_SLASH] = '/';
    m_keyState[INPUT_SCAN_RIGHT_SHIFT] = INPUT_SCAN_RIGHT_SHIFT << 8;
    m_keyState[INPUT_SCAN_NUMPAD_MULTIPLY] = '*';
    m_keyState[INPUT_SCAN_ALT] = INPUT_SCAN_ALT << 8;
    m_keyState[INPUT_SCAN_SPACE] = ' ';
    m_keyState[INPUT_SCAN_CAPS_LOCK] = INPUT_SCAN_CAPS_LOCK << 8;
    m_keyState[INPUT_SCAN_F1] = INPUT_SCAN_F1 << 8;
    m_keyState[INPUT_SCAN_F2] = INPUT_SCAN_F2 << 8;
    m_keyState[INPUT_SCAN_F3] = INPUT_SCAN_F3 << 8;
    m_keyState[INPUT_SCAN_F4] = INPUT_SCAN_F4 << 8;
    m_keyState[INPUT_SCAN_F5] = INPUT_SCAN_F5 << 8;
    m_keyState[INPUT_SCAN_F6] = INPUT_SCAN_F6 << 8;
    m_keyState[INPUT_SCAN_F7] = INPUT_SCAN_F7 << 8;
    m_keyState[INPUT_SCAN_F8] = INPUT_SCAN_F8 << 8;
    m_keyState[INPUT_SCAN_F9] = INPUT_SCAN_F9 << 8;
    m_keyState[INPUT_SCAN_F10] = INPUT_SCAN_F10 << 8;
    m_keyState[INPUT_SCAN_NUM_LOCK] = INPUT_SCAN_NUM_LOCK << 8;
    m_keyState[INPUT_SCAN_SCROLL_LOCK] = INPUT_SCAN_SCROLL_LOCK << 8;
    m_keyState[INPUT_SCAN_NUMPAD_7] = INPUT_SCAN_NUMPAD_7 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_8] = INPUT_SCAN_NUMPAD_8 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_9] = INPUT_SCAN_NUMPAD_9 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_MINUS] = '-';
    m_keyState[INPUT_SCAN_NUMPAD_4] = INPUT_SCAN_NUMPAD_4 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_5] = INPUT_SCAN_NUMPAD_5 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_6] = INPUT_SCAN_NUMPAD_6 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_PLUS] = '+';
    m_keyState[INPUT_SCAN_NUMPAD_1] = INPUT_SCAN_NUMPAD_1 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_2] = INPUT_SCAN_NUMPAD_2 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_3] = INPUT_SCAN_NUMPAD_3 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_0] = INPUT_SCAN_NUMPAD_0 << 8;
    m_keyState[INPUT_SCAN_NUMPAD_DELETE] = INPUT_SCAN_NUMPAD_DELETE << 8;
    m_keyState[INPUT_SCAN_SYSREQ] = INPUT_SCAN_SYSREQ << 8;
    m_keyState[INPUT_SCAN_RESERVED_55] = INPUT_SCAN_RESERVED_55 << 8;
    m_keyState[INPUT_SCAN_ISO_BACKSLASH] = INPUT_SCAN_ISO_BACKSLASH << 8;
    m_keyState[INPUT_SCAN_F11] = INPUT_SCAN_F11 << 8;
    m_keyState[INPUT_SCAN_F12] = INPUT_SCAN_F12 << 8;
}

// @semantic
// Complete 0xe4 retail CFG and all 18 relocation occurrences align. Candidate is
// two bytes shorter because +0x82 loads/stores/tests gbColorMice through EAX rather
// than retail ECX; at +0xb7 candidate then loads gbColorMice before
// bLastOnscreenMouseColor, while retail loads the commutative operands oppositely.
// Reversing that inequality and splitting the assignment through a scoped local are
// code-neutral. Payload-independent cursor state and owner-relative gConfig +0x30/
// +0x34 addends are confirmed; revisit only after a genuine predecessor/TU-state
// change, since the residual is register coloring rather than missing behavior.
VA(0x004ce990, 0xe4)
void CheckChangeCursor(i32 x, i32 y, i32 force)
{
    if (bInCheckChangeCursor != 0)
        return;
    if (gConfig.gfx[giCurExe].fullScreen != 0 && force == 0)
        return;
    if (gConfig.gfx[giCurExe].colorMouseCursor == 0)
        return;

    bInCheckChangeCursor = 1;
    if (force != 0 ||
        (x >= 0 && x < MOUSE_SCREEN_WIDTH && y >= 0 && y < MOUSE_SCREEN_HEIGHT)) {
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

// @semantic
// Base is 0xe6 bytes versus retail's 0xe9. At +0x01 retail forms the mouse-active-field
// pointer in ECX, loads it into EAX, and tests EAX; base forms it in EAX and folds the
// load/test into a memory compare. From base +0x1d / retail +0x20 through RET, all 0xc9
// remaining bytes are identical. All 11 relocation identities/addends agree: the first
// gpInputManager operand is at +0x02/+0x03 and every later retail site is base +3.
// Direct and positive-guard forms, pointer/value/reference combinations, manager-field
// pointers, value-dependent stores, and their combined form were tried. The libclang
// AST exposes no valid mutation, and 90 guarded TU-state trials across the prior and
// integrated declaration states produced no exact closure. Revisit on a genuine
// predecessor/header/TU-state change; load folding is not a permitted artifact.
VA(0x004cea80, 0xe9)
void inputManager::ForceMouseMove(void)
{
    i32 mouseMessageActive = gpInputManager->m_mouseMessageActive;
    if (mouseMessageActive != 0)
        return;
    gpInputManager->m_mouseMessageActive = 1;

    tag_message *event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->type = MESSAGE_MOUSE_MOVE;
    gpMouseManager->MouseCoords(event->payload.mouse.x, event->payload.mouse.y);
    event->payload.mouse.screenX = event->payload.mouse.x;
    event->payload.mouse.screenY = event->payload.mouse.y;
    event->payload.mouse.modifiers = gpInputManager->m_modifiers;
    gpInputManager->m_writeIndex++;
    gpInputManager->m_writeIndex %= INPUT_EVENT_RING_CAPACITY;
    if (gpInputManager->m_writeIndex == gpInputManager->m_readIndex) {
        gpInputManager->m_readIndex++;
        gpInputManager->m_readIndex %= INPUT_EVENT_RING_CAPACITY;
    }
    gpInputManager->m_mouseMessageActive = 0;
}


// ===== vtable inputManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004ce230, 0x78)  int inputManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004ce2b0, 0x20)  void inputManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004ce2d0, 0x5)  int inputManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(inputManager, 0x004eba30);

// ---- globals (definitions, RVA order) ----
DATA(0x00534bc8) i32 iLastBWOnScreenCheck;
