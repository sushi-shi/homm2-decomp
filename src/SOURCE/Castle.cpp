// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Castle.obj   from: (directly linked into exe)
// functions: 2   data: 3
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/Castle.h>
#include <_all.h>
VA(0x0041d040, 0x10bb)
void townManager::SetupCastle(class heroWindow *, int) {}

VA(0x0041e0fb, 0xca3)
int CastleHandler(struct tag_message &) { return 0; }

// ---- data / globals / vtables ----
DATA(0x004ef5e0)  // unsigned char * castleSlotsBase
DATA(0x00525040)  // class heroWindow * casWin
DATA(0x00525048)  // unsigned char * castleSlotsUse
