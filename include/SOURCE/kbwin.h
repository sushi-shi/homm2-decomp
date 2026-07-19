#ifndef HOMM2_KBWIN_H
#define HOMM2_KBWIN_H

#include <SOURCE/KB_TYPES.h>
#include <va.h>
#include <windows.h>

H2_ENUM_BEGIN(KbWinConstant)
    KBWIN_PROCESS_MESSAGE_MAX     = 0x3ff,
    KBWIN_PROCESS_MESSAGE_COUNT   = KBWIN_PROCESS_MESSAGE_MAX + 1,
    KBWIN_COMMAND_LINE_LIMIT      = 0x3c,
    KBWIN_COMMAND_LINE_CLEAR_SIZE = KBWIN_COMMAND_LINE_LIMIT + 1,
    KBWIN_SOUND_SERVICE_INTERVAL  = 20,
    KBWIN_GET_MESSAGE_INTERVAL    = 150,
    KBWIN_TIMER_ID                = 1,
    KBWIN_TIMER_INTERVAL          = 10,
    KBWIN_MIN_WIDTH               = 240,
    KBWIN_MIN_HEIGHT              = 160,
    KBWIN_WINDOWED_STYLE          = 0x14cf0000,
    KBWIN_FULLSCREEN_STYLE        = 0x14000000,
    KBWIN_CLASS_STYLE             = 0x100b,
    KBWIN_CUSTOM_CD_MESSAGE       = 0x3b9,
    KBWIN_MENU_ENTRY_COUNT        = MENU_ENABLE_STATUS_COUNT,
    KBWIN_WIDTH_640               = 640,
    KBWIN_HEIGHT_480              = 480,
    KBWIN_WIDTH_800               = 800,
    KBWIN_HEIGHT_600              = 600,
    KBWIN_WIDTH_1024              = 1024,
    KBWIN_HEIGHT_768              = 768,
    KBWIN_WIDTH_1280              = 1280,
    KBWIN_HEIGHT_1024             = 1024
H2_ENUM_END(KbWinConstant)

H2_ENUM_CLASS_BEGIN(KbWinMenuCommand)
    KBWIN_MENU_SIZE_640_480   = 0x9c45,
    KBWIN_MENU_SIZE_800_600   = 0x9c46,
    KBWIN_MENU_SIZE_1024_768  = 0x9c47,
    KBWIN_MENU_SIZE_1280_1024 = 0x9c48,
    KBWIN_MENU_FULLSCREEN     = 0x9c49,
    KBWIN_MENU_HELP           = 0x9c74,
    KBWIN_MENU_ABOUT          = 0x9c75
H2_ENUM_CLASS_END(KbWinMenuCommand)

extern "C" i32 __stdcall WinMain(HINSTANCE, HINSTANCE, char*, i32);
i32 AppInit(HINSTANCE, HINSTANCE, i32, char*);
i32 AppIdle(void);
LRESULT CALLBACK AppWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AppAbout(HWND, UINT, WPARAM, LPARAM);
void AppExit(void);
void Process1WindowsMessage(void);
void ResizeWindow(i32, i32, i32, i32);
LRESULT AppCommand(HWND, UINT, WPARAM, LPARAM);
void UpdateDfltMenu(HMENU);
void KBChangeMenu(HMENU);
void SetMenuStatus(i32);
void SetNoDialogMenus(i32);
void SetMenus(HMENU, i32);
i32l KBTickCount(void);
void InitVideo(void);

extern HANDLE gEventHandle;
extern HINSTANCE hInstApp;
extern HMENU hmnuApp;
extern HWND hwndApp;

extern char szAppName[16];
extern char szTitle[32];
extern i32l lLastGTimerTickCount;
extern i32l lLastCycleColorsTickCount;
extern i32 bRestartMusic;
extern i32 iLastMusic;
extern i32l lLastGetMessage;
extern i32l lLastAilServe;
extern b32 gbNoDialogMenusOn;
extern i32 giTotalMemAllocated;
extern u8 giChangeThreshold[];
extern RECT rcTemp;
extern i32 iMainWinScreenHeight;
extern HMENU hmnuCurrent;
extern i32 iTempX;
extern i32 iTempY;
extern i32l lTemp;
extern u8 bProcessMessage[KBWIN_PROCESS_MESSAGE_COUNT];
extern i32 iMainWinScreenWidth;

#endif
