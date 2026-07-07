// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\FINDPATH.OBJ   from: (directly linked into exe)
// functions: 12   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/FINDPATH.h>
VA(0x004a4a50, 0x10)
searchArray::searchArray(void) {}

VA(0x004a4a60, 0x30)
searchArray::~searchArray() {}

VA(0x004a4a90, 0x60)
void searchArray::Init(void) {}

VA(0x004a4af0, 0x30)
void searchArray::Close(void) {}

VA(0x004a4b20, 0x40)
void searchArray::Clear(void) {}

VA(0x004a4b60, 0x40)
int searchArray::QuickDistance(int, int, int, int) { return 0; }

VA(0x004a4ba0, 0x80)
int CalcTerrainCost(int, int, int, int, int, int) { return 0; }

VA(0x004a4c20, 0x270)
void searchArray::PushPoint(int, int, int, int, int, int, int, int, int, int, int, int) {}

VA(0x004a4e90, 0x36f)
void searchArray::TestPossibleDirections(int, int, signed char * const, signed char * const, int, int) {}

VA(0x004a5200, 0x1f0)
void searchArray::SeedCombatPosition(class army *) {}

VA(0x004a53f0, 0x410)
int searchArray::FindCombatPath(int, int, class army *, int, int) { return 0; }

VA(0x004a5800, 0x100)
void searchArray::PushCombatPoint(int, int, int, int) {}

// ---- data / globals / vtables ----
DATA(0x0052adc8)  // unsigned char * bIsMoatSlowed
