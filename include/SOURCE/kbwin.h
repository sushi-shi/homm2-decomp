#ifndef HOMM2_KBWIN_H
#define HOMM2_KBWIN_H
#include <va.h>
// Declarations of the free functions DEFINED in kbwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

extern "C" int __stdcall WinMain(void *, void *, char *, int);
int AppInit(void *, void *, int, char *);
int AppIdle(void);
long int __stdcall AppWndProc(void *, unsigned int, unsigned int, long int);
int __stdcall AppAbout(void *, unsigned int, unsigned int, long int);
void AppExit(void);
void Process1WindowsMessage(void);
void ResizeWindow(int, int, int, int);
long int AppCommand(void *, unsigned int, unsigned int, long int);
void UpdateDfltMenu(void *);
void KBChangeMenu(void *);
void SetMenuStatus(int);
void SetNoDialogMenus(int);
void SetMenus(void *, int);
long int KBTickCount(void);
void InitVideo(void);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
DATA(0x004ef500) extern void *gEventHandle;
DATA(0x00524c08) extern void *hInstApp;
DATA(0x004ef4fc) extern void *hmnuApp;
DATA(0x004ef4f8) extern void *hwndApp;

// ---- globals (declarations, RVA order) ----
DATA(0x004ef4c8) extern char *szAppName;
DATA(0x004ef4d8) extern char *szTitle;
DATA(0x004ef588) extern long lLastGTimerTickCount;
DATA(0x004ef58c) extern long lLastCycleColorsTickCount;
DATA(0x004ef590) extern int bRestartMusic;
DATA(0x004ef594) extern int iLastMusic;
DATA(0x004ef5b8) extern long lLastGetMessage;
DATA(0x004ef5bc) extern long lLastAilServe;
DATA(0x004ef5dc) extern int gbNoDialogMenusOn;
DATA(0x0051dcf0) extern int giTotalMemAllocated;
DATA(0x0051dcf8) extern unsigned char giChangeThreshold[16];
DATA(0x00524c20) extern int iMainWinScreenHeight;
DATA(0x00524c24) extern void *hmnuCurrent;
DATA(0x00524c28) extern int iTempX;
DATA(0x00524c2c) extern int iTempY;
DATA(0x00524c30) extern long lTemp;
DATA(0x00524c38) extern unsigned char *bProcessMessage;
DATA(0x00525038) extern int iMainWinScreenWidth;

#endif // HOMM2_KBWIN_H
