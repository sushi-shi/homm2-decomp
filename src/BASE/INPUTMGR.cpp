// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\INPUTMGR.OBJ   from: .\basewin.lib
// functions: 15   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <string.h>
#include <BASE/mouseManager.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/KB.h>
#include <BASE/inputManager.h>
#include <BASE/INPUTMGR.h>
#include <_carcass_types.h>
VA(0x004cdb50, 0x308)
int KeyboardMessageHandler(void *, unsigned int, unsigned int, long int) { return 0; }

VA(0x004cde60, 0x36c)
int MouseMessageHandler(void *, unsigned int, unsigned int, long int) { return 0; }

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
    tag_message local_1c;
    PollSound();
    if (gpInputManager->m_active == 1 && m_readIndex != m_writeIndex) {
        int iVar3 = m_readIndex;
        local_1c = m_eventRing[iVar3];
        m_readIndex = iVar3 + 1;
        m_readIndex = m_readIndex % 0x40;
        if (local_1c.type == 1 && field_0x856 == 0)
            AsciiConvert(local_1c);
    } else {
        local_1c.type = 0;
        local_1c.field8 = 0;
        local_1c.field4 = 0;
        local_1c.fieldC = 0;
    }
    return local_1c;
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
        local_1c.field8 = 0;
        local_1c.field4 = 0;
        local_1c.fieldC = 0;
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

VA(0x004ce480, 0x1cb)
void inputManager::AsciiConvert(struct tag_message &) {}

VA(0x004ce650, 0x33c)
void inputManager::MakeScanCodeTable(void) {}

VA(0x004ce990, 0xe4)
void CheckChangeCursor(int, int, int) {}

VA(0x004cea80, 0xe9)
void inputManager::ForceMouseMove(void)
{
    if (gpInputManager->field_0x73e == 0) {
        gpInputManager->field_0x73e = 1;
        int iVar4 = gpInputManager->m_writeIndex;
        tag_message *ev = &gpInputManager->m_eventRing[iVar4];
        ev->type = 4;
        gpMouseManager->MouseCoords(ev->field4, ev->field8);
        ev->field10 = ev->field4;
        ev->field14 = ev->field8;
        ev->fieldC = gpInputManager->field_0x85e;
        gpInputManager->m_writeIndex = gpInputManager->m_writeIndex + 1;
        gpInputManager->m_writeIndex = gpInputManager->m_writeIndex % 0x40;
        if (gpInputManager->m_writeIndex == gpInputManager->m_readIndex) {
            gpInputManager->m_readIndex = gpInputManager->m_readIndex + 1;
            gpInputManager->m_readIndex = gpInputManager->m_readIndex % 0x40;
        }
        gpInputManager->field_0x73e = 0;
    }
}


// ===== vtable inputManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004ce230, 0x78)  int inputManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004ce2b0, 0x20)  void inputManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004ce2d0, 0x5)  int inputManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(inputManager, 0x004eba30);

// ---- globals (definitions, RVA order) ----
int iCurSwapPalette;
int bLastMouseOffscreen;
int bLastOnscreenMouseColor;
int bInCheckChangeCursor;
int iLastBWOnScreenCheck;
