// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Viewwrld.obj   from: (directly linked into exe)
// functions: 5   data: 17
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/advManager.h>
#include <SOURCE/Viewwrld.h>
VA(0x004333c0, 0x267)
void advManager::ViewWorld(int, int, int) {}

VA(0x00433627, 0x5a)
void advManager::VWCleanup(void) {}

VA(0x00433681, 0x253)
void advManager::VWInit(int, int) {}

VA(0x004338d4, 0x1346)
void advManager::VWCompleteDraw(void) {}

VA(0x00434c1a, 0x5e2)
int ViewWorldDialogHandler(struct tag_message &) { return 0; }
