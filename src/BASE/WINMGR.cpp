// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WINMGR.OBJ   from: .\basewin.lib
// functions: 21   data: 9
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/WINMGR.h>
#include <_all.h>
VA(0x004ca6d0, 0x3a3)
void CycleColors(int) {}

VA(0x004caa80, 0x41)
heroWindowManager::heroWindowManager(void) {}

VA(0x004caad0, 0xd6)
int heroWindowManager::Open(int) { return 0; }

VA(0x004cabb0, 0x45)
void heroWindowManager::Close(void) {}

VA(0x004cac00, 0x2d)
int heroWindowManager::Main(struct tag_message &) { return 0; }

VA(0x004cac30, 0xf)
int heroWindowManager::ConvertToHover(struct tag_message &) { return 0; }

VA(0x004cac40, 0x35)
int heroWindowManager::BroadcastMessage(int, int, int, int) { return 0; }

VA(0x004cac80, 0xbc)
void heroWindowManager::AddWindow(class heroWindow *, int, int) {}

VA(0x004cad40, 0x87)
void heroWindowManager::RemoveWindow(class heroWindow *) {}

VA(0x004cadd0, 0x1cf)
int heroWindowManager::DoDialog(class heroWindow *, int (*)(struct tag_message &), int) { return 0; }

VA(0x004cafa0, 0x17)
void heroWindowManager::UpdateScreen(void) {}

VA(0x004cafc0, 0x4f)
void heroWindowManager::UpdateScreenRegion(int, int, int, int) {}

VA(0x004cb010, 0x18)
void heroWindowManager::RedrawScreen(void) {}

VA(0x004cb030, 0x80)
void heroWindowManager::FadeScreen(int, int, class palette *) {}

VA(0x004cb0b0, 0x53)
void heroWindowManager::ScreenShot(void) {}

VA(0x004cb110, 0xc0)
void heroWindowManager::SaveFizzleSource(int, int, int, int) {}

VA(0x004cb1d0, 0x1)
void CreateFizzleTables(void) {}

VA(0x004cb1e0, 0x402)
void heroWindowManager::FizzleForward(int, int, int, int, int, signed char *, signed char *) {}

VA(0x004cb5f0, 0x19)
void heroWindowManager::ReleaseFizzleSource(void) {}

VA(0x004cb610, 0x1)
void CreateColorTables(void) {}

VA(0x004cb620, 0x1)
void CreateColorLookupTables(void) {}

// ---- data / globals / vtables ----
DATA(0x004eba10)  // const heroWindowManager::vftable
DATA(0x0051ef28)  // int iCombatCycleFrame
DATA(0x0051ef2c)  // int gbEveryOtherCycle
DATA(0x0051ef30)  // int iCycle1Count
DATA(0x0051ef34)  // int iCycle2Count
DATA(0x0051ef38)  // int iCycle3Count
DATA(0x0051ef3c)  // int iDialogNestCount
DATA(0x00534908)  // signed char * gCyclePal
DATA(0x00534968)  // short int memSelector

// ===== vtable heroWindowManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004caad0, 0xd6)  int heroWindowManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004cabb0, 0x45)  void heroWindowManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004cac00, 0x2d)  int heroWindowManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
