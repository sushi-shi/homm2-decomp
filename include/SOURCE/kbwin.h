#ifndef HOMM2_KBWIN_H
#define HOMM2_KBWIN_H
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

#endif // HOMM2_KBWIN_H
