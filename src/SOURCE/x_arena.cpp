// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\x_arena.obj   from: (directly linked into exe)
// functions: 3   data: 3
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/x_arena.h>
VA(0x004c0080, 0x35c)
int DoArenaDialog(void) { return 0; }

VA(0x004c03dc, 0x25f)
int ArenaWindowHandler(struct tag_message &) { return 0; }

VA(0x004c063b, 0x150)
void UpdateArenaIcons(void) {}

// ---- globals (definitions, RVA order) ----
int choice;
class iconWidget *skillWidget[3];
class heroWindow *arenaWinPtr;
