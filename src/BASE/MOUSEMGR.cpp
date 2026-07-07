// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\MOUSEMGR.OBJ   from: .\basewin.lib
// functions: 17   data: 12
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/mouseManager.h>
VA(0x004c9270, 0xd9)
mouseManager::mouseManager(void) {}

VA(0x004c9350, 0x94)
int mouseManager::Open(int) { return 0; }

VA(0x004c93f0, 0xed)
void mouseManager::Close(void) {}

VA(0x004c94e0, 0x5)
int mouseManager::Main(struct tag_message &) { return 0; }

VA(0x004c94f0, 0x135)
void mouseManager::SetPointer(char *, int, int) {}

VA(0x004c9630, 0x405)
void mouseManager::SetPointer(int) {}

VA(0x004c9a40, 0x47a)
void mouseManager::NewUpdate(int) {}

VA(0x004c9ec0, 0x56)
void mouseManager::MouseCoords(int &, int &) {}

VA(0x004c9f20, 0xa2)
void mouseManager::SaveAndDraw(void) {}

VA(0x004c9fd0, 0x29)
void mouseManager::RestoreUnderlying(void) {}

VA(0x004ca000, 0x2c)
void mouseManager::ReallyHidePointer(void) {}

VA(0x004ca030, 0xb9)
void mouseManager::ReallyShowPointer(void) {}

VA(0x004ca0f0, 0x1a)
void mouseManager::HideColorPointer(void)
{
    int old = field_0x86;
    field_0x86 = old + 1;
    if (old + 1 == 1)
        NewUpdate(1);
}

VA(0x004ca110, 0x98)
void mouseManager::ShowColorPointer(void) {}

VA(0x004ca1b0, 0xc)
int mouseManager::IsVis(void) { return field_0x86 == 0; }

VA(0x004ca1c0, 0x66)
void mouseManager::CheckUpdateMousePos(void) {}

VA(0x004ca230, 0x191)
void mouseManager::SetColorMice(int) {}


// ===== vtable mouseManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004c9350, 0x94)  int mouseManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004c93f0, 0xed)  void mouseManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004c94e0, 0x5)  int mouseManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(mouseManager, 0x004eba00);
