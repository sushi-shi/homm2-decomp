// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\INPUTMGR.OBJ   from: .\basewin.lib
// functions: 15   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/inputManager.h>
#include <BASE/INPUTMGR.h>
#include <_carcass_types.h>
VA(0x004cdb50, 0x308)
int KeyboardMessageHandler(void *, unsigned int, unsigned int, long int) { return 0; }

VA(0x004cde60, 0x36c)
int MouseMessageHandler(void *, unsigned int, unsigned int, long int) { return 0; }

VA(0x004ce1d0, 0x56)
inputManager::inputManager(void) {}

VA(0x004ce230, 0x78)
int inputManager::Open(int) { return 0; }

VA(0x004ce2b0, 0x20)
void inputManager::Close(void) {}

VA(0x004ce2d0, 0x5)
int inputManager::Main(struct tag_message &) { return 0; }

VA(0x004ce2e0, 0xf)
void inputManager::Flush(void) {}

VA(0x004ce2f0, 0xa8)
struct tag_message inputManager::GetEvent(void) { return *(struct tag_message *)0; }

VA(0x004ce3a0, 0xa1)
struct tag_message inputManager::PeekEvent(void) { return *(struct tag_message *)0; }

VA(0x004ce450, 0x3)
void inputManager::SetMouseCoords(int, int) {}

VA(0x004ce460, 0x1b)
void inputManager::SetKeyCodeType(int) {}

VA(0x004ce480, 0x1cb)
void inputManager::AsciiConvert(struct tag_message &) {}

VA(0x004ce650, 0x33c)
void inputManager::MakeScanCodeTable(void) {}

VA(0x004ce990, 0xe4)
void CheckChangeCursor(int, int, int) {}

VA(0x004cea80, 0xe9)
void inputManager::ForceMouseMove(void) {}


// ===== vtable inputManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004ce230, 0x78)  int inputManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004ce2b0, 0x20)  void inputManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004ce2d0, 0x5)  int inputManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(inputManager, 0x004eba30);
