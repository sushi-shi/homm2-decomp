// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\VIEW.OBJ   from: (directly linked into exe)
// functions: 3   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/VIEW.h>
VA(0x0040bd60, 0x6d3)
int combatManager::ViewGeneral(int, int, int) { return 0; }

VA(0x0040c433, 0x351)
int HandleViewGeneral(struct tag_message &) { return 0; }

VA(0x0040c784, 0x165)
void combatManager::ViewArmy(class army *, int) {}
