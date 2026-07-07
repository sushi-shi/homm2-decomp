// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\kbwin.obj   from: (directly linked into exe)
// functions: 16   data: 21
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/kbwin.h>
VA(0x0041bce0, 0x146)
extern "C" int __stdcall WinMain(void *, void *, char *, int) { return 0; }

VA(0x0041be26, 0x339)
int AppInit(void *, void *, int, char *) { return 0; }

VA(0x0041c15f, 0x31)
int AppIdle(void) { return 0; }

VA(0x0041c190, 0x57e)
long int __stdcall AppWndProc(void *, unsigned int, unsigned int, long int) { return 0; }

VA(0x0041c70e, 0x90)
int __stdcall AppAbout(void *, unsigned int, unsigned int, long int) { return 0; }

VA(0x0041c79e, 0x1a)
void AppExit(void) {}

VA(0x0041c7b8, 0xc8)
void Process1WindowsMessage(void) {}

VA(0x0041c880, 0x147)
void ResizeWindow(int, int, int, int) {}

VA(0x0041c9c7, 0x197)
long int AppCommand(void *, unsigned int, unsigned int, long int) { return 0; }

VA(0x0041cb5e, 0xd7)
void UpdateDfltMenu(void *) {}

VA(0x0041cc35, 0xac)
void KBChangeMenu(void *) {}

VA(0x0041cce1, 0x15c)
void SetMenuStatus(int) {}

VA(0x0041ce3d, 0x7b)
void SetNoDialogMenus(int) {}

VA(0x0041ceb8, 0x159)
void SetMenus(void *, int) {}

VA(0x0041d011, 0x16)
long int KBTickCount(void) { return 0; }

VA(0x0041d027, 0x10)
void InitVideo(void) {}

// ---- globals (definitions, RVA order) ----
void *hwndApp;
void *hmnuApp;
void *gEventHandle;
void *hInstApp;
