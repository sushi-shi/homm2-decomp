// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\RECRUIT.OBJ   from: (directly linked into exe)
// functions: 8   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/RECRUIT.h>
VA(0x0048b310, 0x18c)
void SetupRecruitWin(class heroWindow *, int, int, int, int, int) {}

VA(0x0048b49c, 0x24b)
int recruitUnit::Open(int) { return 0; }

VA(0x0048b6e7, 0xe7)
void recruitUnit::Close(void) {}

VA(0x0048b7ce, 0x122)
void recruitUnit::Update(void) {}

VA(0x0048b8f0, 0x41b)
int recruitUnit::Main(struct tag_message &) { return 0; }

VA(0x0048bd0b, 0xdf)
recruitUnit::recruitUnit(class armyGroup *, int, short int *) {}

VA(0x0048bdea, 0xfb)
recruitUnit::recruitUnit(class town *, int, int) {}

VA(0x0048bee5, 0x14f)
void QuickViewRecruit(class town *, int) {}

// ---- data / globals / vtables ----
DATA(0x004eb848)  // const recruitUnit::vftable
DATA(0x00528578)  // void * hmnuRecruitSave

// ===== vtable recruitUnit : public baseManager  (3 slots) =====
//  [ 0] VA(0x0048b49c, 0x24b)  int recruitUnit::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x0048b6e7, 0xe7)  void recruitUnit::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0048b8f0, 0x41b)  int recruitUnit::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
