#ifndef HOMM2_KBWIN_H
#define HOMM2_KBWIN_H
#include <_types.h>
#include <va.h>
#include <windows.h>
// Declarations of the free functions DEFINED in kbwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

enum {
    KBWIN_PROCESS_MESSAGE_MAX = 0x3ff,
    KBWIN_PROCESS_MESSAGE_COUNT = KBWIN_PROCESS_MESSAGE_MAX + 1,
    KBWIN_COMMAND_LINE_LIMIT = 0x3c,
    KBWIN_COMMAND_LINE_CLEAR_SIZE = KBWIN_COMMAND_LINE_LIMIT + 1,
    KBWIN_SOUND_SERVICE_INTERVAL = 20,
    KBWIN_GET_MESSAGE_INTERVAL = 150,
    KBWIN_TIMER_ID = 1,
    KBWIN_TIMER_INTERVAL = 10,
    KBWIN_MIN_WIDTH = 240,
    KBWIN_MIN_HEIGHT = 160,
    KBWIN_WINDOWED_STYLE = 0x14cf0000,
    KBWIN_FULLSCREEN_STYLE = 0x14000000,
    KBWIN_CLASS_STYLE = 0x100b,
    KBWIN_CUSTOM_CD_MESSAGE = 0x3b9,
    KBWIN_MENU_ENTRY_COUNT = MENU_ENABLE_STATUS_COUNT,
    KBWIN_WIDTH_640 = 640,
    KBWIN_HEIGHT_480 = 480,
    KBWIN_WIDTH_800 = 800,
    KBWIN_HEIGHT_600 = 600,
    KBWIN_WIDTH_1024 = 1024,
    KBWIN_HEIGHT_768 = 768,
    KBWIN_WIDTH_1280 = 1280,
    KBWIN_HEIGHT_1024 = 1024
};

typedef enum KbWinMenuCommand {
    KBWIN_MENU_SIZE_640_480 = 0x9c45,
    KBWIN_MENU_SIZE_800_600 = 0x9c46,
    KBWIN_MENU_SIZE_1024_768 = 0x9c47,
    KBWIN_MENU_SIZE_1280_1024 = 0x9c48,
    KBWIN_MENU_FULLSCREEN = 0x9c49,
    KBWIN_MENU_HELP = 0x9c74,
    KBWIN_MENU_ABOUT = 0x9c75
} KbWinMenuCommand;

extern "C" int __stdcall WinMain(HINSTANCE, HINSTANCE, char *, int);
int AppInit(HINSTANCE, HINSTANCE, int, char *);
int AppIdle(void);
long int __stdcall AppWndProc(HWND, unsigned int, unsigned int, long int);
int __stdcall AppAbout(HWND, unsigned int, unsigned int, long int);
void AppExit(void);
void Process1WindowsMessage(void);
void ResizeWindow(int, int, int, int);
long int AppCommand(HWND, unsigned int, unsigned int, long int);
void UpdateDfltMenu(HMENU);
void KBChangeMenu(HMENU);
void SetMenuStatus(int);
void SetNoDialogMenus(int);
void SetMenus(HMENU, int);
long int KBTickCount(void);
void InitVideo(void);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern HANDLE gEventHandle;
extern HINSTANCE hInstApp;
extern HMENU hmnuApp;
extern HWND hwndApp;

// ---- globals (declarations, RVA order) ----
extern char szAppName[16];
extern char szTitle[32];
extern long lLastGTimerTickCount;
extern long lLastCycleColorsTickCount;
extern int bRestartMusic;
extern int iLastMusic;
extern long lLastGetMessage;
extern long lLastAilServe;
extern int gbNoDialogMenusOn;
extern int giTotalMemAllocated;
extern unsigned char giChangeThreshold[16];
extern RECT rcTemp;
extern int iMainWinScreenHeight;
extern HMENU hmnuCurrent;
extern int iTempX;
extern int iTempY;
extern long lTemp;
extern unsigned char bProcessMessage[KBWIN_PROCESS_MESSAGE_COUNT];
extern int iMainWinScreenWidth;

#endif // HOMM2_KBWIN_H
