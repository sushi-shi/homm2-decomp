#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <va.h>
#include <BASE/INPUTMGR.h>
#include <BASE/Misc.h>
#include <BASE/heroWindowManager.h>
#include <BASE/soundManager.h>
#include <BASE/soundmgr.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>
VA(0x0041bce0, 0x146)
extern "C" i32 __stdcall
WinMain(HINSTANCE instance, HINSTANCE previousInstance, char* commandLine, i32 showCommand) {
    DWORD lastError;
    MSG message;

    hInstApp = instance;
    gEventHandle = CreateEventA(0, 0, 0, "Heroes II");
    lastError = GetLastError();
    if (gEventHandle == 0 || lastError == ERROR_ALREADY_EXISTS) {
        sprintf(gText, "Only one copy of %s may run at a time", "Heroes of Might and Magic II");
        MessageBoxA(0, gText, "Startup Error", MB_ICONHAND);
        return 0;
    }

    memset(gcCommandLine, 0, KBWIN_COMMAND_LINE_CLEAR_SIZE);
    strncpy(gcCommandLine, commandLine, KBWIN_COMMAND_LINE_LIMIT);
    if (EarlySetup() == 0)
        return 0;
    if (AppInit(instance, previousInstance, showCommand, commandLine) == 0)
        return 0;

    for (;;) {
        if (PeekMessageA(&message, 0, 0, 0, PM_REMOVE) != 0) {
            if (message.message == WM_QUIT)
                break;
            TranslateMessage(&message);
            DispatchMessageA(&message);
        } else {
            if (AppIdle() != 0)
                WaitMessage();
        }
    }
    ShutDown(0);
    return message.wParam;
}

VA(0x0041be26, 0x339)
i32 AppInit(HINSTANCE instance, HINSTANCE previousInstance, i32 showCommand, char* commandLine) {
    HMENU windowMenu;
    RECT windowRect;
    WNDCLASSA appClass;

    LogInt("hInstApp", reinterpret_cast<i32>(hInstApp), -999, -999, -999, -999, -999, -999);
    memset(bProcessMessage, 0, sizeof(bProcessMessage));
    bProcessMessage[WM_CREATE] = 1;
    bProcessMessage[WM_KEYDOWN] = 1;
    bProcessMessage[WM_KEYUP] = 1;
    bProcessMessage[WM_MOUSEMOVE] = 1;
    bProcessMessage[WM_LBUTTONDOWN] = 1;
    bProcessMessage[WM_LBUTTONDBLCLK] = 1;
    bProcessMessage[WM_RBUTTONDOWN] = 1;
    bProcessMessage[WM_RBUTTONDBLCLK] = 1;
    bProcessMessage[WM_LBUTTONUP] = 1;
    bProcessMessage[WM_RBUTTONUP] = 1;
    bProcessMessage[WM_TIMER] = 1;
    bProcessMessage[WM_ACTIVATEAPP] = 1;
    bProcessMessage[WM_ERASEBKGND] = 1;
    bProcessMessage[WM_MOVE] = 1;
    bProcessMessage[WM_SIZE] = 1;
    bProcessMessage[WM_COMMAND] = 1;
    bProcessMessage[WM_PALETTECHANGED] = 1;
    bProcessMessage[WM_QUERYNEWPALETTE] = 1;
    bProcessMessage[WM_PAINT] = 1;
    bProcessMessage[WM_DESTROY] = 1;
    bProcessMessage[WM_QUIT] = 1;
    bProcessMessage[WM_CLOSE] = 1;
    bProcessMessage[KBWIN_CUSTOM_CD_MESSAGE] = 1;

    if (previousInstance == 0) {
        appClass.hCursor = 0;
        appClass.hIcon = LoadIconA(instance, "Heroes II");
        appClass.lpszMenuName = 0;
        appClass.lpszClassName = szAppName;
        appClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        appClass.hInstance = instance;
        appClass.style = KBWIN_CLASS_STYLE;
        appClass.lpfnWndProc = AppWndProc;
        appClass.cbWndExtra = 0;
        appClass.cbClsExtra = 0;
        if (RegisterClassA(&appClass) == 0)
            return 0;
    }

    if (gConfig.gfx[giCurExe].showMenu != 0)
        giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
    else
        giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;
    windowRect.left = windowRect.top = 0;
    windowRect.right = gConfig.gfx[giCurExe].width - 1;
    windowRect.bottom = gConfig.gfx[giCurExe].height - 1;
    AdjustWindowRect(&windowRect, giCurWindowsStyleFlags, gConfig.gfx[giCurExe].showMenu);
    if (gConfig.gfx[giCurExe].showMenu != 0)
        windowMenu = reinterpret_cast<HMENU>(hmnuDflt);
    else
        windowMenu = 0;
    hwndApp = CreateWindowExA(
        0,
        szAppName,
        szTitle,
        giCurWindowsStyleFlags,
        gConfig.gfx[giCurExe].x,
        gConfig.gfx[giCurExe].y,
        windowRect.right - windowRect.left + 1,
        windowRect.bottom - windowRect.top + 1,
        0,
        windowMenu,
        instance,
        0
    );
    if (hwndApp != 0) {
        PostMessageA(
            hwndApp,
            WM_SETICON,
            ICON_SMALL,
            reinterpret_cast<LPARAM>(LoadIconA(instance, "Heroes"))
        );
        ShowWindow(hwndApp, showCommand);
        SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
        if (gConfig.gfx[giCurExe].showMenu == 0)
            SetMenuStatus(0);
        InitGraphics();
        SetCursor(LoadCursorA(0, IDC_ARROW));
        oldmain();
        return 1;
    } else {
        return 0;
    }
}

// @semantic: first residual is retail's mov eax, 0 in the false arm versus this compiler state's xor eax, eax.
VA(0x0041c15f, 0x31)
i32 AppIdle(void) {
    if (gbForegroundApp != 0)
        return 1;
    else
        return 0;
}

// @semantic: first residual is the embedded 0x14-byte jump table at RVA 0x1c61b.
VA(0x0041c190, 0x57e)
LRESULT CALLBACK AppWndProc(HWND window, UINT message, WPARAM messageParam, LPARAM messageData) {
    if (message > KBWIN_PROCESS_MESSAGE_MAX || bProcessMessage[message] == 0) {
        return DefWindowProcA(window, message, messageParam, messageData);
    }

    switch (message) {
        case WM_CREATE:
            srand(KBTickCount());
            SetTimer(window, KBWIN_TIMER_ID, KBWIN_TIMER_INTERVAL, 0);
            GdiSetBatchLimit(1);
            return 0;
        case WM_KEYDOWN:
        case WM_KEYUP:
            if (KeyboardMessageHandler(window, message, messageParam, messageData) == 0)
                return 0;
            break;
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
            if (MouseMessageHandler(window, message, messageParam, messageData) == 0)
                return 0;
            break;
        case WM_TIMER:
            lTemp = KBTickCount();
            if (lLastGTimerTickCount + 5 < lTemp) {
                lLastGTimerTickCount = lTemp;
                UpdateTimers(0);
            }
            return 0;
        case KBWIN_CUSTOM_CD_MESSAGE:
            if (messageParam == 1) {
                gpSoundManager
                    ->CDPlay(gpSoundManager->m_cdTrack, 0, gpSoundManager->m_cdPlayFrame, 1);
            }
            break;
        case WM_ACTIVATEAPP:
            gbForegroundApp = messageParam;
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOVE:
            if (hwndApp == 0)
                return 0;
            lTemp = GetWindowLongA(hwndApp, GWL_STYLE);
            if ((lTemp & (WS_MINIMIZE | WS_MAXIMIZE)) == 0 && gbClosingApp == 0
                && gConfig.gfx[giCurExe].fullScreen == 0) {
                GetWindowRect(window, &rcTemp);
                gConfig.gfx[giCurExe].x = rcTemp.left;
                gConfig.gfx[giCurExe].y = rcTemp.top;
                WritePrefs();
            }
            return 0;
        case WM_SIZE:
            if (hwndApp != 0) {
                lTemp = GetWindowLongA(hwndApp, GWL_STYLE);
                gbMinimized = lTemp & WS_MINIMIZE;
                if ((lTemp & WS_MINIMIZE) == 0)
                    EarlyResizeWindow(0, 0, 0, 0);
                if ((lTemp & (WS_MINIMIZE | WS_MAXIMIZE)) == 0
                    && (LOWORD(messageData) < KBWIN_MIN_WIDTH
                        || HIWORD(messageData) < KBWIN_MIN_HEIGHT)) {
                    iTempX = LOWORD(messageData) > KBWIN_MIN_WIDTH ? LOWORD(messageData)
                                                                   : KBWIN_MIN_WIDTH;
                    iTempY = HIWORD(messageData) > KBWIN_MIN_HEIGHT ? HIWORD(messageData)
                                                                    : KBWIN_MIN_HEIGHT;
                    ResizeWindow(-1, -1, iTempX, iTempY);
                    return 0;
                }
            }
            iMainWinScreenWidth = LOWORD(messageData);
            iMainWinScreenHeight = HIWORD(messageData);
            if (iMainWinScreenWidth < 1)
                iMainWinScreenWidth = 1;
            if (iMainWinScreenHeight < 1)
                iMainWinScreenHeight = 1;
            if (hwndApp != 0 && (lTemp & (WS_MINIMIZE | WS_MAXIMIZE)) == 0 && gbClosingApp == 0
                && gConfig.gfx[giCurExe].fullScreen == 0) {
                gConfig.gfx[giCurExe].width = iMainWinScreenWidth;
                gConfig.gfx[giCurExe].height = iMainWinScreenHeight;
                WritePrefs();
            }
            return 0;
        case WM_COMMAND:
            return AppCommand(window, message, messageParam, messageData);
        case WM_PALETTECHANGED:
            if (reinterpret_cast<u32>(window) == messageParam)
                break;
        case WM_QUERYNEWPALETTE:
            return QueryNewPalette();
        case WM_PAINT:
            AppPaint(window, 0);
            return 0;
        case WM_CLOSE:
            if (hwndApp == window) {
                if (GameUnsaved() != 0) {
                    NormalDialog(
                        "Are you sure you want to quit?",
                        NORMAL_DIALOG_CONFIRM,
                        NORMAL_DIALOG_NO_RESOURCE,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0
                    );
                    if (gpWindowManager->m_dialogResult == APP_MENU_CONFIRM_OK)
                        DestroyWindow(window);
                    return 0;
                }
            }
        case WM_DESTROY:
            gbClosingApp = true;
            PostQuitMessage(0);
        case WM_QUIT:
            ShutDown(0);
            break;
    }
    return DefWindowProcA(window, message, messageParam, messageData);
}

VA(0x0041c70e, 0x90)
BOOL CALLBACK AppAbout(HWND dialog, UINT message, WPARAM messageParam, LPARAM messageData) {
    i32 command;
    HWND commandWindow;
    u16 notificationType;

    switch (message) {
        case WM_INITDIALOG:
            return 1;
        case WM_COMMAND:
            command = LOWORD(messageParam);
            commandWindow = reinterpret_cast<HWND>(messageData);
            notificationType = HIWORD(messageParam);
            if (command == IDOK)
                EndDialog(dialog, 1);
            break;
    }
    PollSound();
    return 0;
}

VA(0x0041c79e, 0x1a)
void AppExit(void) {
    CleanUpWinGraphics();
    CleanUpMenus();
}

VA(0x0041c7b8, 0xc8)
void Process1WindowsMessage(void) {
    MSG message;
    i32l currentTick;

    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE) != 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    currentTick = KBTickCount();
    if (currentTick - lLastAilServe > KBWIN_SOUND_SERVICE_INTERVAL) {
        lLastAilServe = currentTick;
        if (gbNoSound == 0)
            gpSoundManager->ServiceSound();
    }
    if (currentTick - lLastGetMessage > KBWIN_GET_MESSAGE_INTERVAL) {
        lLastGetMessage = currentTick;
        if (GetMessageA(&message, 0, 0, 0) != 0) {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }
}

VA(0x0041c880, 0x147)
void ResizeWindow(i32 x, i32 y, i32 width, i32 height) {
    i32 windowX;
    RECT windowRect;
    i32 targetY;

    if (gConfig.gfx[giCurExe].fullScreen != 0)
        return;
    GetWindowRect(hwndApp, &windowRect);
    if (x == -1)
        windowX = windowRect.left;
    else
        windowX = x;
    if (y == -1)
        targetY = windowRect.top;
    else
        targetY = y;
    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = width - 1;
    windowRect.bottom = height - 1;
    AdjustWindowRect(&windowRect, giCurWindowsStyleFlags, gConfig.gfx[giCurExe].showMenu);
    MoveWindow(
        hwndApp,
        windowX,
        targetY,
        windowRect.right - windowRect.left + 1,
        windowRect.bottom - windowRect.top + 1,
        1
    );
    gConfig.gfx[giCurExe].x = windowX;
    gConfig.gfx[giCurExe].y = targetY;
    gConfig.gfx[giCurExe].width = width;
    gConfig.gfx[giCurExe].height = height;
    WritePrefs();
}

// @early-stop: delinker jump-table artifact.
VA(0x0041c9c7, 0x197)
LRESULT AppCommand(HWND window, UINT message, WPARAM messageParam, LPARAM messageData) {
    i32 command;
    DLGPROC appDialogProc;

    command = LOWORD(messageParam);
    switch (command) {
        case IDX(KBWIN_MENU_ABOUT):
            appDialogProc = reinterpret_cast<DLGPROC>(AppAbout);
            DialogBoxParamA(hInstApp, "HEROES", window, appDialogProc, 0);
            break;
        case IDX(KBWIN_MENU_HELP):
            WinHelpA(hwndApp, ".\\HELP\\HEROES2.HLP", HELP_FINDER, 0);
            break;
        case IDX(KBWIN_MENU_SIZE_640_480):
            ResizeWindow(-1, -1, KBWIN_WIDTH_640, KBWIN_HEIGHT_480);
            break;
        case IDX(KBWIN_MENU_SIZE_800_600):
            ResizeWindow(-1, -1, KBWIN_WIDTH_800, KBWIN_HEIGHT_600);
            break;
        case IDX(KBWIN_MENU_SIZE_1024_768):
            ResizeWindow(-1, -1, KBWIN_WIDTH_1024, KBWIN_HEIGHT_768);
            break;
        case IDX(KBWIN_MENU_SIZE_1280_1024):
            ResizeWindow(-1, -1, KBWIN_WIDTH_1280, KBWIN_HEIGHT_1024);
            break;
        case IDX(KBWIN_MENU_FULLSCREEN):
            SetFullScreenStatus(1 - gConfig.gfx[giCurExe].fullScreen);
            break;
        default:
            return HandleAppSpecificMenuCommands(command);
    }
    return 0;
}

VA(0x0041cb5e, 0xd7)
void UpdateDfltMenu(HMENU menu) {
    i32 result;
    i32 value;

    if (gConfig.gfx[giCurExe].showMenu == 0)
        return;
    if (giMainVideoModeWidth <= 640)
        EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_640_480), MF_GRAYED);
    if (giMainVideoModeWidth <= 800)
        EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_800_600), MF_GRAYED);
    if (giMainVideoModeWidth <= 1024)
        EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_1024_768), MF_GRAYED);
    if (giMainVideoModeWidth <= 1280)
        EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_1280_1024), MF_GRAYED);
    if (gbDDrawAttached == 0)
        EnableMenuItem(menu, IDX(KBWIN_MENU_FULLSCREEN), MF_GRAYED);
}

// @semantic: compiler-shape residual.
VA(0x0041cc35, 0xac)
void KBChangeMenu(HMENU menu) {
    if (menu == 0)
        menu = hmnuCurrent;
    hmnuCurrent = menu;
    hmnuApp = menu;
    if (gConfig.gfx[giCurExe].showMenu && menu != 0) {
        SetMenu(hwndApp, menu);
        UpdateDfltMenu(menu);
        UpdateAppSpecificMenus(menu);
        DrawMenuBar(hwndApp);
    } else {
        SetMenu(hwndApp, 0);
        DrawMenuBar(hwndApp);
    }
}

VA(0x0041cce1, 0x15c)
void SetMenuStatus(i32 showMenu) {
    i32 width;
    i32 height;
    i32l windowStyle;
    i32l replacedStyle;

    if (gConfig.gfx[giCurExe].fullScreen && showMenu)
        return;
    {
        width = gConfig.gfx[giCurExe].width;
        height = gConfig.gfx[giCurExe].height;
        gConfig.gfx[giCurExe].showMenu = showMenu;
        KBChangeMenu(0);
        gConfig.gfx[giCurExe].width = width;
        gConfig.gfx[giCurExe].height = height;
        WritePrefs();
        windowStyle = GetWindowLongA(hwndApp, GWL_STYLE);
        if (gConfig.gfx[giCurExe].showMenu)
            giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
        else
            giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;
        replacedStyle = SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
        ShowWindow(hwndApp, SW_SHOWNA);
        ResizeWindow(-1, -1, gConfig.gfx[giCurExe].width, gConfig.gfx[giCurExe].height);
    }
}

VA(0x0041ce3d, 0x7b)
void SetNoDialogMenus(i32 menusEnabled) {
    if (gbNoDialogMenusOn && !menusEnabled)
        return;
    if (!gbNoDialogMenusOn && menusEnabled)
        return;
    if (!hmnuApp)
        return;
    gbNoDialogMenusOn = 1 - menusEnabled;
    SetMenus(hmnuApp, menusEnabled);
}

// @semantic: First residual +0x2b: retail loads the loop index and compares count with jle.
VA(0x0041ceb8, 0x159)
void SetMenus(HMENU menu, i32 enabled) {
    i32 count;
    u32 commandId;
    i32 scanPosition;
    i32 commandPosition;
    i32 disableFlag;
    i32 index;

    count = GetMenuItemCount(menu);
    for (index = 0; index < count; index++) {
        commandId = GetMenuItemID(menu, index);
        if (commandId == static_cast<u32>(-1)) {
            SetMenus(GetSubMenu(menu, index), enabled);
            disableFlag = 0;
        } else {
            disableFlag = 0;
            if (enabled) {
                disableFlag = 1;
            } else {
                scanPosition = 0;
                for (commandPosition = 0; commandPosition < KBWIN_MENU_ENTRY_COUNT;
                     commandPosition++) {
                    if (gsMenuEnableStatus[commandPosition].command == commandId) {
                        scanPosition = commandPosition;
                    }
                }
                if (gbInSetupDialog)
                    disableFlag = 1 - gsMenuEnableStatus[scanPosition].setupEnabled;
                else
                    disableFlag = 1 - gsMenuEnableStatus[scanPosition].normalEnabled;
            }
        }
        if (disableFlag != 0) {
            EnableMenuItem(menu, commandId, enabled == 0 ? MF_GRAYED : MF_ENABLED);
        }
    }
    UpdateDfltMenu(menu);
}

VA(0x0041d011, 0x16)
i32l KBTickCount(void) {
    return GetTickCount();
}

VA(0x0041d027, 0x10)
void InitVideo(void) {
    return;
}

DATA(0x004ef4c8) char szAppName[16] = "Heroes II";
DATA(0x004ef4d8) char szTitle[32] = "Heroes of Might and Magic II";
DATA(0x004ef4f8) HWND hwndApp = 0;
DATA(0x004ef4fc) HMENU hmnuApp = 0;
DATA(0x004ef500) HANDLE gEventHandle = 0;
DATA(0x004ef588) i32l lLastGTimerTickCount = 0;
DATA(0x004ef58c) i32l lLastCycleColorsTickCount = 0;
DATA(0x004ef590) i32 bRestartMusic = 0;
DATA(0x004ef594) i32 iLastMusic = -1;
DATA(0x004ef5b8) i32l lLastGetMessage = 0;
DATA(0x004ef5bc) i32l lLastAilServe = 0;
DATA(0x004ef5dc) b32 gbNoDialogMenusOn = false;
DATA(0x00524c08) HINSTANCE hInstApp;
DATA(0x00524c10) RECT rcTemp;
DATA(0x00524c20) i32 iMainWinScreenHeight;
DATA(0x00524c24) HMENU hmnuCurrent;
DATA(0x00524c28) i32 iTempX;
DATA(0x00524c2c) i32 iTempY;
DATA(0x00524c30) i32l lTemp;
DATA(0x00524c38) u8 bProcessMessage[KBWIN_PROCESS_MESSAGE_COUNT];
DATA(0x00525038) i32 iMainWinScreenWidth;
