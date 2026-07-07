// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Overview.obj   from: (directly linked into exe)
// functions: 7   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/game.h>
#include <SOURCE/Overview.h>
VA(0x00407870, 0x223e)
void game::SetupDynamicStuff(int, int, int) {}

VA(0x00409aae, 0x357)
void game::SetupNewOverviewType(int, int) {}

VA(0x00409e05, 0x84)
void game::SetupResources(void) {}

VA(0x00409e89, 0x4c7)
void game::Overview(void) {}

VA(0x0040a350, 0x31f)
void game::DoKnob(void) {}

VA(0x0040a66f, 0x4fd)
int OverviewHandler(struct tag_message &) { return 0; }

VA(0x0040ab6c, 0x4fa)
int game::ProcessIconSelect(int, int) { return 0; }
