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
extern void *gEventHandle;
extern void *hInstApp;
extern void *hmnuApp;
extern void *hwndApp;

// ---- globals (declarations, RVA order) ----
extern char *szAppName;
extern char *szTitle;
extern long lLastGTimerTickCount;
extern long lLastCycleColorsTickCount;
extern int bRestartMusic;
extern int iLastMusic;
extern long lLastGetMessage;
extern long lLastAilServe;
extern int gbNoDialogMenusOn;
extern int giTotalMemAllocated;
extern unsigned char giChangeThreshold[16];
extern int iMainWinScreenHeight;
extern void *hmnuCurrent;
extern int iTempX;
extern int iTempY;
extern long lTemp;
extern unsigned char *bProcessMessage;
extern int iMainWinScreenWidth;

#endif // HOMM2_KBWIN_H
