// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\TOWN.OBJ   from: (directly linked into exe)
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/town.h>
VA(0x00432c00, 0x65)
town::town(void) {}

VA(0x00432c65, 0x54)
int town::HasGarrison(void) { return 0; }

VA(0x00432cb9, 0x111)
void town::GiveSpells(class hero *) {}

VA(0x00432dca, 0xaa)
void town::XformToCastle(void) {}

VA(0x00432e74, 0xe0)
void town::View(int) {}

VA(0x00432f54, 0x14d)
void town::Deallocate(void) {}

VA(0x004330a1, 0x23e)
void town::BuildBuilding(int) {}

VA(0x004332df, 0x36)
int town::CanBuildDock(void) { return 0; }

VA(0x00433315, 0x9f)
void town::CalcNumLevelArchers(int *, int *) {}

// ---- globals (definitions, RVA order) ----
DATA(0x004f11b0) int bEnteringTown;
