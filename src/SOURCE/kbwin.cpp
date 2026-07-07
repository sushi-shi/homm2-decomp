// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\kbwin.obj   from: (directly linked into exe)
// functions: 16   data: 21
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_all.h>
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

// ---- data / globals / vtables ----
DATA(0x004ef4c8)  // char * szAppName
DATA(0x004ef4d8)  // char * szTitle
DATA(0x004ef4f8)  // void * hwndApp
DATA(0x004ef4fc)  // void * hmnuApp
DATA(0x004ef500)  // void * gEventHandle
DATA(0x004ef588)  // long int lLastGTimerTickCount
DATA(0x004ef58c)  // long int lLastCycleColorsTickCount
DATA(0x004ef590)  // int bRestartMusic
DATA(0x004ef594)  // int iLastMusic
DATA(0x004ef5b8)  // long int lLastGetMessage
DATA(0x004ef5bc)  // long int lLastAilServe
DATA(0x004ef5dc)  // int gbNoDialogMenusOn
DATA(0x00524c08)  // void * hInstApp
DATA(0x00524c10)  // struct tagRECT rcTemp
DATA(0x00524c20)  // int iMainWinScreenHeight
DATA(0x00524c24)  // void * hmnuCurrent
DATA(0x00524c28)  // int iTempX
DATA(0x00524c2c)  // int iTempY
DATA(0x00524c30)  // long int lTemp
DATA(0x00524c38)  // unsigned char * bProcessMessage
DATA(0x00525038)  // int iMainWinScreenWidth
