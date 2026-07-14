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
#include <_carcass_types.h>
// @early-stop
// reloc-masked raw bytes are identical over 0x004cdb50..0x004cde58; objdiff only retains $L jump-table identities instead of the delinked containing-function target
VA(0x004cdb50, 0x308)
int KeyboardMessageHandler(void *, unsigned int message, unsigned int, long int messageData)
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
    event->type = 0;

    switch (message) {
    case WM_KEYDOWN:
        event->type = 1;
        event->payload.keyboard.keyCode = static_cast<unsigned short>(static_cast<unsigned long>(messageData) >> 16) & 0xff;
        event->payload.keyboard.unknown0x08 = 0;
        event->payload.keyboard.modifiers = 0;
        switch (event->payload.keyboard.keyCode) {
        case 0x1d:
            gpInputManager->field_0x85e |= 4;
            break;
        case 0x2a:
            gpInputManager->field_0x85e |= 2;
            break;
        case 0x36:
            gpInputManager->field_0x85e |= 1;
            break;
        case 0x38:
            gpInputManager->field_0x85e |= 0x20;
            break;
        }
        break;
    case WM_KEYUP:
        event->type = 2;
        event->payload.keyboard.keyCode = static_cast<unsigned short>(static_cast<unsigned long>(messageData) >> 16) & 0xff;
        event->payload.keyboard.unknown0x08 = 0;
        event->payload.keyboard.modifiers = 0;
        switch (event->payload.keyboard.keyCode) {
        case 0x1d:
            gpInputManager->field_0x85e &= 0xfffb;
            break;
        case 0x2a:
            gpInputManager->field_0x85e &= 0xfffd;
            break;
        case 0x36:
            gpInputManager->field_0x85e &= 0xfffe;
            break;
        case 0x38:
            gpInputManager->field_0x85e &= 0xffdf;
            break;
        }
        break;
    }

    if (event->type != 0) {
        event->payload.keyboard.modifiers = gpInputManager->field_0x85e;
        gpInputManager->m_writeIndex++;
        gpInputManager->m_writeIndex %= 64;
        if (gpInputManager->m_writeIndex == gpInputManager->m_readIndex) {
            gpInputManager->m_readIndex++;
            gpInputManager->m_readIndex %= 64;
        }
        gpInputManager->m_field_0x85a = 0;

        if (gpWindowManager->m_active == 1) {
            if (event->type == 1 && event->payload.keyboard.keyCode == 0x58 &&
                (event->payload.keyboard.modifiers & 3) != 0)
                gpWindowManager->ScreenShot();
            if (event->type == 1 && event->payload.keyboard.keyCode == 0x3b) {
                SetFullScreenStatus(0);
                AppCommand(hwndApp, 0, 0x9c74, 0);
            }
            if (event->type == 1 && event->payload.keyboard.keyCode == 0x3e)
                SetFullScreenStatus(1 - gConfig.gfx[giCurExe].fullScreen);
        }
    }
    return event->type == 0;
}

// @early-stop
// /O2 wall: 0x004ce150..0x004ce191 is the same read-index collision update with eax/ecx exchanged (direct, pointer, and reference spellings exhausted); jump-table relocs are $L vs containing function
VA(0x004cde60, 0x36c)
int MouseMessageHandler(void *, unsigned int message, unsigned int, long int messageData)
{
    if (gpInputManager == 0)
        return 1;
    if (gpInputManager->m_active != 1)
        return 1;
    if (gpInputManager->field_0x73e != 0)
        return 1;
    gpInputManager->field_0x73e = 1;

    tag_message *event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->payload.mouse.modifiers = 0;
    event->payload.mouse.screenY = 0;
    event->payload.mouse.screenX = 0;
    event->payload.mouse.y = 0;
    event->payload.mouse.x = 0;
    event->type = 0;

    switch (message - WM_MOUSEMOVE) {
    case WM_MOUSEMOVE - WM_MOUSEMOVE:
        event->type = 4;
        break;
    case WM_LBUTTONDOWN - WM_MOUSEMOVE:
        event->type = 8;
        SetCapture(hwndApp);
        break;
    case WM_LBUTTONUP - WM_MOUSEMOVE:
        event->type = 0x10;
        if (ReleaseCapture() == 0)
            LogStr("ReleaseCapture Failed");
        break;
    case WM_LBUTTONDBLCLK - WM_MOUSEMOVE:
        event->type = 8;
        break;
    case WM_RBUTTONDOWN - WM_MOUSEMOVE:
        event->type = 0x20;
        SetCapture(hwndApp);
        break;
    case WM_RBUTTONUP - WM_MOUSEMOVE:
        event->type = 0x40;
        if (ReleaseCapture() == 0)
            LogStr("ReleaseCapture Failed");
        break;
    case WM_RBUTTONDBLCLK - WM_MOUSEMOVE:
        event->type = 0x20;
        break;
    default:
        goto afterMouseCoordinates;
    }

    event->payload.mouse.x =
        (static_cast<short>(messageData) * 640) / iMainWinScreenWidth;
    event->payload.mouse.y =
        (static_cast<short>(static_cast<unsigned long>(messageData) >> 16) * 480) /
        iMainWinScreenHeight;
    event->payload.mouse.screenX = event->payload.mouse.x;
    event->payload.mouse.screenY = event->payload.mouse.y;

    if (gConfig.gfx[giCurExe].fullScreen == 0 &&
        gConfig.gfx[giCurExe].colorMouseCursor == 0 &&
        KBTickCount() > iLastBWOnScreenCheck &&
        event->payload.mouse.x > 3 && event->payload.mouse.x < 636 &&
        event->payload.mouse.y > 3 && event->payload.mouse.y < 476) {
        iLastBWOnScreenCheck = KBTickCount() + 500;
        gpMouseManager->SetPointer(1000);
    }

afterMouseCoordinates:
    if (message == WM_MOUSEMOVE && gpMouseManager != 0) {
        int y = event->payload.mouse.y;
        int x = event->payload.mouse.x;
        if (bInCheckChangeCursor == 0 && gConfig.gfx[giCurExe].fullScreen == 0 &&
            gConfig.gfx[giCurExe].colorMouseCursor != 0) {
            bInCheckChangeCursor = 1;
            if (x >= 0 && x < 640 && y >= 0 && y < 480) {
                if (bLastMouseOffscreen != 0) {
                    bLastMouseOffscreen = 0;
                    gpMouseManager->SetPointer(1000);
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

    if (event->type != 0) {
        event->payload.mouse.modifiers = gpInputManager->field_0x85e;
        gpInputManager->m_writeIndex++;
        gpInputManager->m_writeIndex %= 64;
        if (gpInputManager->m_writeIndex == gpInputManager->m_readIndex) {
            gpInputManager->m_readIndex++;
            gpInputManager->m_readIndex %= 64;
        }
    }
    gpInputManager->field_0x73e = 0;
    return event->type == 0;
}

VA(0x004ce1d0, 0x56)
inputManager::inputManager(void) : baseManager()
{
    m_active = 0;
    field_0x73e = 0;
    field_0x852 = 1;
    field_0x84e = 0;
    field_0x742 = 0;
    field_0x746 = 0;
    field_0x74a = 1;
    field_0x856 = 1;
    field_0x866 = 0;
    field_0x862 = -1;
}

VA(0x004ce230, 0x78)
int inputManager::Open(int param_1)
{
    memset(m_eventRing, 0, sizeof(m_eventRing));
    m_writeIndex = 0;
    m_readIndex = 0;
    field_0x852 = param_1;
    field_0x85e = 0;
    MakeScanCodeTable();
    field_0xc = 4;
    field_0x10 = -1;
    m_active = 1;
    strcpy(name, "inputManager");
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
int inputManager::Main(struct tag_message &) { return 0; }

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
        m_readIndex %= 64;
        if (event.type == 1 && field_0x856 == 0)
            AsciiConvert(event);
    } else {
        event.type = 0;
        event.payload.unknown.unknown0x08 = 0;
        event.payload.unknown.unknown0x04 = 0;
        event.payload.unknown.unknown0x0c = 0;
    }
    return event;
}

VA(0x004ce3a0, 0xa1)
tag_message inputManager::PeekEvent(void)
{
    tag_message local_1c;
    PollSound();
    if (gpInputManager->m_active == 1 && m_readIndex != m_writeIndex) {
        local_1c = m_eventRing[m_readIndex];
        m_readIndex = m_readIndex % 0x40;
        if (local_1c.type == 1 && field_0x856 == 0)
            AsciiConvert(local_1c);
    } else {
        local_1c.type = 0;
        local_1c.payload.unknown.unknown0x08 = 0;
        local_1c.payload.unknown.unknown0x04 = 0;
        local_1c.payload.unknown.unknown0x0c = 0;
    }
    return local_1c;
}

VA(0x004ce450, 0x3)
void inputManager::SetMouseCoords(int, int) {}

VA(0x004ce460, 0x1b)
void inputManager::SetKeyCodeType(int param_1)
{
    field_0x856 = param_1;
    m_writeIndex = 0;
    m_readIndex = 0;
}

// @early-stop
// /O2 wall: 0x004ce4b2..0x004ce4d1 caches modifiers/code in eax/ecx oppositely; direct, split, converted, volatile, and SIB spellings exhausted; jump-table relocs are $L vs containing function
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

    int modifiers = event.payload.keyboard.modifiers & 3;
    if (modifiers == 0) {
        int value = event.payload.keyboard.keyCode;
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
    for (unsigned int i = 0; i < 128; i++)
        m_keyState[i] = i << 8;

    m_keyState[0] = 0;
    m_keyState[1] = 0x1b;
    m_keyState[2] = '1';
    m_keyState[3] = '2';
    m_keyState[4] = '3';
    m_keyState[5] = '4';
    m_keyState[6] = '5';
    m_keyState[7] = '6';
    m_keyState[8] = '7';
    m_keyState[9] = '8';
    m_keyState[10] = '9';
    m_keyState[11] = '0';
    m_keyState[12] = '-';
    m_keyState[13] = '=';
    m_keyState[14] = 0x7f;
    m_keyState[15] = '\t';
    m_keyState[16] = 'Q';
    m_keyState[17] = 'W';
    m_keyState[18] = 'E';
    m_keyState[19] = 'R';
    m_keyState[20] = 'T';
    m_keyState[21] = 'Y';
    m_keyState[22] = 'U';
    m_keyState[23] = 'I';
    m_keyState[24] = 'O';
    m_keyState[25] = 'P';
    m_keyState[26] = '[';
    m_keyState[27] = ']';
    m_keyState[28] = '\n';
    m_keyState[29] = 0x1d00;
    m_keyState[30] = 'A';
    m_keyState[31] = 'S';
    m_keyState[32] = 'D';
    m_keyState[33] = 'F';
    m_keyState[34] = 'G';
    m_keyState[35] = 'H';
    m_keyState[36] = 'J';
    m_keyState[37] = 'K';
    m_keyState[38] = 'L';
    m_keyState[39] = '\'';
    m_keyState[40] = '\'';
    m_keyState[41] = 0x2900;
    m_keyState[42] = 0x2a00;
    m_keyState[43] = '\\';
    m_keyState[44] = 'Z';
    m_keyState[45] = 'X';
    m_keyState[46] = 'C';
    m_keyState[47] = 'V';
    m_keyState[48] = 'B';
    m_keyState[49] = 'N';
    m_keyState[50] = 'M';
    m_keyState[51] = ',';
    m_keyState[52] = '.';
    m_keyState[53] = '/';
    m_keyState[54] = 0x3600;
    m_keyState[55] = '*';
    m_keyState[56] = 0x3800;
    m_keyState[57] = ' ';
    m_keyState[58] = 0x3a00;
    m_keyState[59] = 0x3b00;
    m_keyState[60] = 0x3c00;
    m_keyState[61] = 0x3d00;
    m_keyState[62] = 0x3e00;
    m_keyState[63] = 0x3f00;
    m_keyState[64] = 0x4000;
    m_keyState[65] = 0x4100;
    m_keyState[66] = 0x4200;
    m_keyState[67] = 0x4300;
    m_keyState[68] = 0x4400;
    m_keyState[69] = 0x4500;
    m_keyState[70] = 0x4600;
    m_keyState[71] = 0x4700;
    m_keyState[72] = 0x4800;
    m_keyState[73] = 0x4900;
    m_keyState[74] = '-';
    m_keyState[75] = 0x4b00;
    m_keyState[76] = 0x4c00;
    m_keyState[77] = 0x4d00;
    m_keyState[78] = '+';
    m_keyState[79] = 0x4f00;
    m_keyState[80] = 0x5000;
    m_keyState[81] = 0x5100;
    m_keyState[82] = 0x5200;
    m_keyState[83] = 0x5300;
    m_keyState[84] = 0x5400;
    m_keyState[85] = 0x5500;
    m_keyState[86] = 0x5600;
    m_keyState[87] = 0x5700;
    m_keyState[88] = 0x5800;
}

// @early-stop
// /O2 wall: 0x004cea12..0x004cea22 and 0x004cea49..0x004cea5c exchange eax/ecx for identical color-state loads; local/reversed/combined assignment spellings exhausted
VA(0x004ce990, 0xe4)
void CheckChangeCursor(int x, int y, int force)
{
    if (bInCheckChangeCursor != 0)
        return;
    if (gConfig.gfx[giCurExe].fullScreen != 0 && force == 0)
        return;
    if (gConfig.gfx[giCurExe].colorMouseCursor == 0)
        return;

    bInCheckChangeCursor = 1;
    if (force != 0 || (x >= 0 && x < 640 && y >= 0 && y < 480)) {
        if (bLastMouseOffscreen != 0) {
            bLastMouseOffscreen = 0;
            gpMouseManager->SetPointer(1000);
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

// @early-stop
// /O2 wall: 0x004cea81..0x004cea9e uses cmp [eax],0 instead of load/test via ecx/eax; pointer/value/reference and value-dependent store spellings exhausted; remaining body is identical
VA(0x004cea80, 0xe9)
void inputManager::ForceMouseMove(void)
{
    int *busy = &gpInputManager->field_0x73e;
    int busyValue = *busy;
    if (busyValue != 0)
        return;
    *busy = 1;

    tag_message *event = &gpInputManager->m_eventRing[gpInputManager->m_writeIndex];
    event->type = 4;
    gpMouseManager->MouseCoords(event->payload.mouse.x, event->payload.mouse.y);
    event->payload.mouse.screenX = event->payload.mouse.x;
    event->payload.mouse.screenY = event->payload.mouse.y;
    event->payload.mouse.modifiers = gpInputManager->field_0x85e;
    gpInputManager->m_writeIndex++;
    gpInputManager->m_writeIndex %= 64;
    if (gpInputManager->m_writeIndex == gpInputManager->m_readIndex) {
        gpInputManager->m_readIndex++;
        gpInputManager->m_readIndex %= 64;
    }
    gpInputManager->field_0x73e = 0;
}


// ===== vtable inputManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004ce230, 0x78)  int inputManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004ce2b0, 0x20)  void inputManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004ce2d0, 0x5)  int inputManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(inputManager, 0x004eba30);

// ---- globals (definitions, RVA order) ----
DATA(0x0051f980) int iCurSwapPalette;
DATA(0x0051f984) int bLastMouseOffscreen;
DATA(0x0051f988) int bLastOnscreenMouseColor;
DATA(0x0051f98c) int bInCheckChangeCursor;
DATA(0x00534bc8) int iLastBWOnScreenCheck;
