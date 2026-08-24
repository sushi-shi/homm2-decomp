#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Ints.h>
#include <BASE/INPUTMGR.h>
#include <BASE/Misc.h>
#include <BASE/heroWindowManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>

typedef enum KbWinPrivateConstant {
    TIMER_UPDATE_MIN_INTERVAL = 5
} KbWinPrivateConstant;

extern "C" i32 __stdcall
WinMain(HINSTANCE instance, HINSTANCE previousInstance, char* commandLine, i32 showCommand) {
    DWORD errorLast;
    MSG message;

    hInstApp = instance;
    gEventHandle = CreateEventA(
        NULL,
        0,
        0,
        "\xc3\xe5\xf0\xee\xe8\x20\x49\x49"
    );
    errorLast = GetLastError();
    if (gEventHandle == NULL || errorLast == ERROR_ALREADY_EXISTS) {
        sprintf(
            gText,
            "\xd2\xee\xeb\xfc\xea\xee\x20\xee\xe4\xed\xe0\x20\xea\xee\xef\xe8\xff\x20\x25\x73\x20\xec\xee\xe6\xe5\xf2\x20\xe1\xfb\xf2\xfc\x20\xe7\xe0\xef\xf3\xf9\xe5\xed\xe0\x20\xee\xe4\xed\xee\xe2\xf0\xe5\xec\xe5\xed\xed\xee",
            "\xc3\xe5\xf0\xee\xe8\x20\xcc\xe5\xf7\xe0\x20\xe8\x20\xcc\xe0\xe3\xe8\xe8\x20\x49\x49"
        );
        MessageBoxA(
            NULL,
            gText,
            "\xce\xf8\xe8\xe1\xea\xe0\x20\xe7\xe0\xe3\xf0\xf3\xe7\xea\xe8",
            MB_ICONHAND
        );
        return 0;
    }

    memset(gcCommandLine, 0, KBWIN_COMMAND_LINE_CLEAR_SIZE);
    strncpy(gcCommandLine, commandLine, KBWIN_COMMAND_LINE_LIMIT);
    if (EarlySetup() == 0)
        return 0;
    if (AppInit(instance, previousInstance, showCommand, commandLine) == 0)
        return 0;

    for (;;) {
        if (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE) != 0) {
            if (message.message == WM_QUIT)
                break;
            TranslateMessage(&message);
            DispatchMessageA(&message);
        } else {
            if (AppIdle() != 0)
                WaitMessage();
        }
    }
    ShutDown(NULL);
    return message.wParam;
}

i32 AppInit(HINSTANCE instance, HINSTANCE previousInstance, i32 showCommand, char* commandLine) {

    RECT windowRect;
    WNDCLASSA appClass;

    LogInt(
        "hInstApp",
        reinterpret_cast<i32>(hInstApp),
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
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

    if (previousInstance == NULL) {
        appClass.hCursor = NULL;
        appClass.hIcon = LoadIconA(
            instance,
            "\xc3\xe5\xf0\xee\xe8\x20\x49\x49"
        );
        appClass.lpszMenuName = NULL;
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

    if (gConfig.gfx[H2EnumIndex(giCurExe)].showMenu != 0)
        giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
    else
        giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;
    windowRect.left = windowRect.top = 0;
    windowRect.right = gConfig.gfx[H2EnumIndex(giCurExe)].width - 1;
    windowRect.bottom = gConfig.gfx[H2EnumIndex(giCurExe)].height - 1;
    AdjustWindowRect(&windowRect, giCurWindowsStyleFlags, gConfig.gfx[H2EnumIndex(giCurExe)].showMenu);
    hwndApp = CreateWindowExA(
        0,
        szAppName,
        szTitle,
        giCurWindowsStyleFlags,
        gConfig.gfx[H2EnumIndex(giCurExe)].x,
        gConfig.gfx[H2EnumIndex(giCurExe)].y,
        windowRect.right - windowRect.left + 1,
        windowRect.bottom - windowRect.top + 1,
        NULL,
        (gConfig.gfx[H2EnumIndex(giCurExe)].showMenu != 0 ? reinterpret_cast<HMENU>(hmnuDflt) : NULL),
        instance,
        NULL
    );
    if (hwndApp != NULL) {
        PostMessageA(
            hwndApp,
            WM_SETICON,
            ICON_SMALL,
            reinterpret_cast<LPARAM>(LoadIconA(
                instance,
                "Heroes"
            ))
        );
        ShowWindow(hwndApp, showCommand);
        SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
        if (gConfig.gfx[H2EnumIndex(giCurExe)].showMenu == 0)
            SetMenuStatus(0);
        InitGraphics();
        SetCursor(LoadCursorA(NULL, IDC_ARROW));
        oldmain();
        return 1;
    } else {
        return 0;
    }
}

i32 AppIdle(void) {
    if (gbForegroundApp != 0)
        return 1;
    else
        return 1;
}

LRESULT CALLBACK AppWndProc(HWND window, UINT message, WPARAM messageParam, LPARAM messageData) {
    if (message > KBWIN_PROCESS_MESSAGE_MAX || bProcessMessage[message] == 0) {
        return DefWindowProcA(window, message, messageParam, messageData);
    }

    switch (message) {
        case WM_CREATE:
            srand(KBTickCount());
            SetTimer(window, KBWIN_TIMER_ID, KBWIN_TIMER_INTERVAL, NULL);
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
            if (lTemp > lLastGTimerTickCount + TIMER_UPDATE_MIN_INTERVAL) {
                lLastGTimerTickCount = lTemp;
            }
            return 0;
        case WM_ACTIVATEAPP:
            gbForegroundApp = messageParam;
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOVE:
            if (hwndApp == NULL)
                return 0;
            lTemp = GetWindowLongA(hwndApp, GWL_STYLE);
            if ((lTemp & WS_MAXIMIZE) == 0 && (lTemp & WS_MINIMIZE) == 0 && gbClosingApp == 0
                && gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen == 0) {
                GetWindowRect(window, &rcTemp);
                gConfig.gfx[H2EnumIndex(giCurExe)].x = rcTemp.left;
                gConfig.gfx[H2EnumIndex(giCurExe)].y = rcTemp.top;
                WritePrefs();
            }
            return 0;
        case WM_SIZE:
            if (hwndApp != NULL) {
                lTemp = GetWindowLongA(hwndApp, GWL_STYLE);
                gbMinimized = lTemp & WS_MINIMIZE;
                if ((lTemp & WS_MINIMIZE) == 0)
                    EarlyResizeWindow(0, 0, 0, 0);
                if ((lTemp & WS_MAXIMIZE) == 0 && (lTemp & WS_MINIMIZE) == 0
                    && (LOWORD(messageData) < KBWIN_MIN_WIDTH
                        || HIWORD(messageData) < KBWIN_MIN_HEIGHT)) {
                    iTempX = LOWORD(messageData) < KBWIN_MIN_WIDTH ? KBWIN_MIN_WIDTH
                                                                   : LOWORD(messageData);
                    iTempY = HIWORD(messageData) < KBWIN_MIN_HEIGHT ? KBWIN_MIN_HEIGHT
                                                                    : HIWORD(messageData);
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
            if (hwndApp != NULL && (lTemp & WS_MAXIMIZE) == 0 && (lTemp & WS_MINIMIZE) == 0
                && gbClosingApp == 0 && gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen == 0) {
                gConfig.gfx[H2EnumIndex(giCurExe)].width = iMainWinScreenWidth;
                gConfig.gfx[H2EnumIndex(giCurExe)].height = iMainWinScreenHeight;
                WritePrefs();
            }
            return 0;
        case WM_COMMAND:
            return AppCommand(window, message, messageParam, messageData);
        case WM_PALETTECHANGED:
            if (messageParam == reinterpret_cast<u32>(window))
                break;
        case WM_QUERYNEWPALETTE:
            return QueryNewPalette();
        case WM_PAINT:
            AppPaint(window, NULL);
            return 0;
        case WM_CLOSE:
            if (window == hwndApp) {
                if (GameUnsaved() != 0) {
                    NormalDialog(
                        "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xe2\xfb\xe9\xf2\xe8?",
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
            ShutDown(NULL);
            break;
    }
    return DefWindowProcA(window, message, messageParam, messageData);
}

BOOL CALLBACK AppAbout(HWND dialog, UINT message, WPARAM messageParam, LPARAM messageData) {
    i32 command;
    HWND commandWindow2;
    u16 notificationType1;

    switch (message) {
        case WM_INITDIALOG:
            return 1;
        case WM_COMMAND:
            command = LOWORD(messageParam);
            commandWindow2 = reinterpret_cast<HWND>(messageData);
            notificationType1 = HIWORD(messageParam);
            if (command == IDOK)
                EndDialog(dialog, 1);
            break;
    }
    PollSound();
    return 0;
}

void AppExit(void) {
    CleanUpWinGraphics();
    CleanUpMenus();
}

void Process1WindowsMessage(void) {
    MSG message;
    i32l currentTick;

    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE) != 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    currentTick = KBTickCount();
    if (currentTick - lLastAilServe > KBWIN_SOUND_SERVICE_INTERVAL) {
        lLastAilServe = currentTick;
        gpSoundManager->ServiceSound();
    }
    if (currentTick - lLastGetMessage > KBWIN_GET_MESSAGE_INTERVAL) {
        lLastGetMessage = currentTick;
        if (GetMessageA(&message, NULL, 0, 0) != 0) {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }
}

void ResizeWindow(i32 x, i32 y, i32 width, i32 height) {
    i32 windowX;
    RECT windowRect;
    i32 targetY;

    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen != 0)
        return;
    GetWindowRect(hwndApp, &windowRect);
    windowX = (x == -1 ? windowRect.left : x);
    targetY = (y == -1 ? windowRect.top : y);
    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = width - 1;
    windowRect.bottom = height - 1;
    AdjustWindowRect(&windowRect, giCurWindowsStyleFlags, gConfig.gfx[H2EnumIndex(giCurExe)].showMenu);
    MoveWindow(
        hwndApp,
        windowX,
        targetY,
        windowRect.right - windowRect.left + 1,
        windowRect.bottom - windowRect.top + 1,
        1
    );
    gConfig.gfx[H2EnumIndex(giCurExe)].x = windowX;
    gConfig.gfx[H2EnumIndex(giCurExe)].y = targetY;
    gConfig.gfx[H2EnumIndex(giCurExe)].width = width;
    gConfig.gfx[H2EnumIndex(giCurExe)].height = height;
    WritePrefs();
}

LRESULT AppCommand(HWND window, UINT message, WPARAM messageParam, LPARAM messageData) {
    i32 command;
    DLGPROC lpfnDlgProc;

    command = LOWORD(messageParam);
    switch (static_cast<KbWinMenuCommand>(command)) {
        case KBWIN_MENU_ABOUT:
            lpfnDlgProc = reinterpret_cast<DLGPROC>(AppAbout);
            DialogBoxParamA(
                hInstApp,
                "HEROES",
                window,
                lpfnDlgProc,
                0
            );
            break;
        case KBWIN_MENU_HELP:
            WinHelpA(
                hwndApp,
                ".\\HELP\\HEROES2.HLP",
                HELP_FINDER,
                0
            );
            break;
        case KBWIN_MENU_SIZE_640_480:
            ResizeWindow(-1, -1, KBWIN_WIDTH_640, KBWIN_HEIGHT_480);
            break;
        case KBWIN_MENU_SIZE_800_600:
            ResizeWindow(-1, -1, KBWIN_WIDTH_800, KBWIN_HEIGHT_600);
            break;
        case KBWIN_MENU_SIZE_1024_768:
            ResizeWindow(-1, -1, KBWIN_WIDTH_1024, KBWIN_HEIGHT_768);
            break;
        case KBWIN_MENU_SIZE_1280_1024:
            ResizeWindow(-1, -1, KBWIN_WIDTH_1280, KBWIN_HEIGHT_1024);
            break;
        case KBWIN_MENU_FULLSCREEN:
            SetFullScreenStatus(1 - gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen);
            break;
        default:
            return HandleAppSpecificMenuCommands(command);
    }
    return 0;
}

void UpdateDfltMenu(HMENU menu) {
    i32 result;
    i32 value;

    if (gConfig.gfx[H2EnumIndex(giCurExe)].showMenu == 0)
        return;
    if (giMainVideoModeWidth <= KBWIN_WIDTH_640)
        EnableMenuItem(menu, H2EnumIndex(KBWIN_MENU_SIZE_640_480), MF_GRAYED);
    if (giMainVideoModeWidth <= KBWIN_WIDTH_800)
        EnableMenuItem(menu, H2EnumIndex(KBWIN_MENU_SIZE_800_600), MF_GRAYED);
    if (giMainVideoModeWidth <= KBWIN_WIDTH_1024)
        EnableMenuItem(menu, H2EnumIndex(KBWIN_MENU_SIZE_1024_768), MF_GRAYED);
    if (giMainVideoModeWidth <= KBWIN_WIDTH_1280)
        EnableMenuItem(menu, H2EnumIndex(KBWIN_MENU_SIZE_1280_1024), MF_GRAYED);
    if (gbDDrawAttached == 0)
        EnableMenuItem(menu, H2EnumIndex(KBWIN_MENU_FULLSCREEN), MF_GRAYED);
}

void KBChangeMenu(HMENU menu) {
    if (menu == NULL)
        menu = hmnuCurrent;
    else
        hmnuCurrent = menu;
    hmnuApp = menu;
    if (gConfig.gfx[H2EnumIndex(giCurExe)].showMenu) {
        if (menu != NULL) {
            SetMenu(hwndApp, menu);
            UpdateDfltMenu(menu);
            UpdateAppSpecificMenus(menu);
            DrawMenuBar(hwndApp);
        }
    } else {
        SetMenu(hwndApp, NULL);
        DrawMenuBar(hwndApp);
    }
}

void SetMenuStatus(i32 showMenu) {
    i32 winWidth;
    i32 height;
    i32l windowStyle;
    i32l replacedStyle;

    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen && showMenu)
        return;
    {
        winWidth = gConfig.gfx[H2EnumIndex(giCurExe)].width;
        height = gConfig.gfx[H2EnumIndex(giCurExe)].height;
        gConfig.gfx[H2EnumIndex(giCurExe)].showMenu = showMenu;
        KBChangeMenu(NULL);
        gConfig.gfx[H2EnumIndex(giCurExe)].width = winWidth;
        gConfig.gfx[H2EnumIndex(giCurExe)].height = height;
        WritePrefs();
        windowStyle = GetWindowLongA(hwndApp, GWL_STYLE);
        if (gConfig.gfx[H2EnumIndex(giCurExe)].showMenu)
            giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
        else
            giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;
        replacedStyle = SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
        ShowWindow(hwndApp, SW_SHOWNA);
        ResizeWindow(-1, -1, gConfig.gfx[H2EnumIndex(giCurExe)].width, gConfig.gfx[H2EnumIndex(giCurExe)].height);
    }
}

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

void SetMenus(HMENU menu, i32 enabled) {
    i32 count;
    u32 id;
    i32 match;
    i32 pos;
    i32 disabled;
    i32 index;

    count = GetMenuItemCount(menu);
    for (index = 0; index < count; index++) {
        id = GetMenuItemID(menu, index);
        if (id == static_cast<u32>(-1)) {
            SetMenus(GetSubMenu(menu, index), enabled);
            disabled = 0;
        } else {
            disabled = 0;
            if (enabled) {
                disabled = 1;
            } else {
                match = 0;
                for (pos = 0; pos < KBWIN_MENU_ENTRY_COUNT; pos++) {
                    if (gsMenuEnableStatus[pos].command == id) {
                        match = pos;
                    }
                }
                if (gbInSetupDialog)
                    disabled = 1 - gsMenuEnableStatus[match].setupEnabled;
                else
                    disabled = 1 - gsMenuEnableStatus[match].normalEnabled;
            }
        }
        if (disabled != 0) {
            EnableMenuItem(menu, id, enabled == 0 ? MF_GRAYED : MF_ENABLED);
        }
    }
    UpdateDfltMenu(menu);
}

i32l KBTickCount(void) {
    return GetTickCount();
}

void InitVideo(void) {
    return;
}

char szAppName[] = "\xc3\xe5\xf0\xee\xe8 II";
char szTitle[] =
    "\xc3\xe5\xf0\xee\xe8 \xcc\xe5\xf7\xe0 \xe8 \xcc\xe0\xe3\xe8\xe8 II";
HWND hwndApp = NULL;
HMENU hmnuApp = NULL;
HANDLE gEventHandle = NULL;
i32l lLastGTimerTickCount = 0;
i32l lLastCycleColorsTickCount = 0;
i32 bRestartMusic = 0;
i32 iLastMusic = -1;
i32l lLastGetMessage = 0;
i32l lLastAilServe = 0;
b32 gbNoDialogMenusOn = false;
HINSTANCE hInstApp;
RECT rcTemp;
i32 iMainWinScreenHeight;
HMENU hmnuCurrent;
i32 iTempX;
i32 iTempY;
i32l lTemp;
u8 bProcessMessage[KBWIN_PROCESS_MESSAGE_COUNT];
i32 iMainWinScreenWidth;
