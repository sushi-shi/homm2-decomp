#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <va.h>
#include <BASE/INPUTMGR.h>
#include <BASE/Misc.h>
#include <BASE/heroWindowManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/KB_TYPES.h>
#include <BASE/dialog.h>
#include <BASE/display.h>

H2_ENUM_BEGIN(KbWinPrivateConstant)
    TIMER_UPDATE_MIN_INTERVAL = 5
H2_ENUM_END(KbWinPrivateConstant)

VA(0x00470e30, 0x112)
extern "C" i32 __stdcall
WinMain(HINSTANCE instance, HINSTANCE previousInstance, char* commandLine, i32 showCommand) {
    DWORD errorLast;
    MSG message;

    hInstApp = instance;
    gEventHandle = CreateEventA(
        NULL,
        0,
        0,
        localization::Tr("system.title.short")
    );
    errorLast = GetLastError();
    if (gEventHandle == NULL || errorLast == ERROR_ALREADY_EXISTS) {
        sprintf(
            gText,
            localization::Tr("system.single_instance"),
            localization::Tr("system.title.full")
        ); /* "Только одна копия %s может быть запущена одновременно", "Герои Меча и Магии II" */
        MessageBoxA(
            NULL,
            gText,
            localization::Tr("system.startup_error.title"),
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

#if H2_RETAIL_COMPILER
#define windowRectangle windowRect
#endif
VA(0x00470f42, 0x2ec)
i32 AppInit(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    i32 showCommand,
    char* H2_UNUSED(commandLine)
) {

    RECT windowRectangle;
    WNDCLASSA appClass;

    LogInt("hInstApp", reinterpret_cast<i32>(hInstApp));
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
            localization::Tr("system.title.short")
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

    if (CURRENT_GRAPHICS_CONFIG.showMenu != 0)
        giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
    else
        giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;
    windowRectangle.left = windowRectangle.top = 0;
    windowRectangle.right = CURRENT_GRAPHICS_CONFIG.width - 1;
    windowRectangle.bottom = CURRENT_GRAPHICS_CONFIG.height - 1;
    AdjustWindowRect(&windowRectangle, giCurWindowsStyleFlags, CURRENT_GRAPHICS_CONFIG.showMenu);
    hwndApp = CreateWindowExA(
        0,
        szAppName,
        szTitle,
        giCurWindowsStyleFlags,
        CURRENT_GRAPHICS_CONFIG.x,
        CURRENT_GRAPHICS_CONFIG.y,
        windowRectangle.right - windowRectangle.left + 1,
        windowRectangle.bottom - windowRectangle.top + 1,
        NULL,
        (CURRENT_GRAPHICS_CONFIG.showMenu != 0 ? hmnuDflt : NULL),
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
        if (CURRENT_GRAPHICS_CONFIG.showMenu == 0)
            SetMenuStatus(0);
        InitGraphics();
        SetCursor(LoadCursorA(NULL, IDC_ARROW));
        oldmain();
        return 1;
    } else {
        return 0;
    }
}
#if H2_RETAIL_COMPILER
#undef windowRectangle
#endif

VA(0x0047122e, 0x1a)
i32 AppIdle(void) {
    if (gbForegroundApp != 0)
        return 1;
    else
        return 1;
}

VA(0x00471248, 0x63b)
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
                && CURRENT_GRAPHICS_CONFIG.fullScreen == 0) {
                GetWindowRect(window, &rcTemp);
                CURRENT_GRAPHICS_CONFIG.x = rcTemp.left;
                CURRENT_GRAPHICS_CONFIG.y = rcTemp.top;
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
                && gbClosingApp == 0 && CURRENT_GRAPHICS_CONFIG.fullScreen == 0) {
                CURRENT_GRAPHICS_CONFIG.width = iMainWinScreenWidth;
                CURRENT_GRAPHICS_CONFIG.height = iMainWinScreenHeight;
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
                        localization::Tr("adventure.confirm.quit"),
                        NORMAL_DIALOG_CONFIRM
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

#if H2_RETAIL_COMPILER
#define commandWindow commandWindow2
#define notificationType notificationType1
#endif
VA(0x00471883, 0x67)
BOOL CALLBACK AppAbout(HWND dialog, UINT message, WPARAM messageParam, LPARAM messageData) {
    i32 command;
    HWND H2_UNUSED(commandWindow);
    u16 H2_UNUSED(notificationType);

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
#if H2_RETAIL_COMPILER
#undef commandWindow
#undef notificationType
#endif

VA(0x004718ea, 0xf)
void AppExit(void) {
    CleanUpWinGraphics();
    CleanUpMenus();
}

VA(0x004718f9, 0xa1)
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

#if H2_RETAIL_COMPILER
#define windowRectangle windowRect
#endif
VA(0x0047199a, 0x125)
void ResizeWindow(i32 x, i32 y, i32 width, i32 height) {
    i32 windowX;
    RECT windowRectangle;
    i32 targetY;

    if (CURRENT_GRAPHICS_CONFIG.fullScreen != 0)
        return;
    GetWindowRect(hwndApp, &windowRectangle);
    windowX = (x == -1 ? windowRectangle.left : x);
    targetY = (y == -1 ? windowRectangle.top : y);
    windowRectangle.left = 0;
    windowRectangle.top = 0;
    windowRectangle.right = width - 1;
    windowRectangle.bottom = height - 1;
    AdjustWindowRect(&windowRectangle, giCurWindowsStyleFlags, CURRENT_GRAPHICS_CONFIG.showMenu);
    MoveWindow(
        hwndApp,
        windowX,
        targetY,
        windowRectangle.right - windowRectangle.left + 1,
        windowRectangle.bottom - windowRectangle.top + 1,
        1
    );
    CURRENT_GRAPHICS_CONFIG.x = windowX;
    CURRENT_GRAPHICS_CONFIG.y = targetY;
    CURRENT_GRAPHICS_CONFIG.width = width;
    CURRENT_GRAPHICS_CONFIG.height = height;
    WritePrefs();
}
#if H2_RETAIL_COMPILER
#undef windowRectangle
#endif

VA(0x00471abf, 0x161)
LRESULT AppCommand(
    HWND window,
    UINT H2_UNUSED(message),
    WPARAM messageParam,
    LPARAM H2_UNUSED(messageData)
) {
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
            ResizeWindow(-1, -1, LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT);
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
            SetFullScreenStatus(1 - CURRENT_GRAPHICS_CONFIG.fullScreen);
            break;
        default:
            return HandleAppSpecificMenuCommands(command);
    }
    return 0;
}

VA(0x00471c20, 0xb1)
void UpdateDfltMenu(HMENU menu) {
    i32 H2_UNUSED(result);
    i32 H2_UNUSED(value);

    if (CURRENT_GRAPHICS_CONFIG.showMenu == 0)
        return;
    if (giMainVideoModeWidth <= LOGICAL_SCREEN_WIDTH)
        EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_640_480), MF_GRAYED);
    if (giMainVideoModeWidth <= KBWIN_WIDTH_800)
        EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_800_600), MF_GRAYED);
    if (giMainVideoModeWidth <= KBWIN_WIDTH_1024)
        EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_1024_768), MF_GRAYED);
    if (giMainVideoModeWidth <= KBWIN_WIDTH_1280)
        EnableMenuItem(menu, IDX(KBWIN_MENU_SIZE_1280_1024), MF_GRAYED);
    if (gbDDrawAttached == 0)
        EnableMenuItem(menu, IDX(KBWIN_MENU_FULLSCREEN), MF_GRAYED);
}

VA(0x00471cd1, 0x8f)
void KBChangeMenu(HMENU menu) {
    if (menu == NULL)
        menu = hmnuCurrent;
    else
        hmnuCurrent = menu;
    hmnuApp = menu;
    if (CURRENT_GRAPHICS_CONFIG.showMenu) {
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

VA(0x00471d60, 0x117)
void SetMenuStatus(i32 showMenu) {
    i32 winWidth;
    i32 height;
    i32l H2_UNUSED(windowStyle);
    i32l H2_UNUSED(replacedStyle);

    if (CURRENT_GRAPHICS_CONFIG.fullScreen && showMenu)
        return;
    {
        winWidth = CURRENT_GRAPHICS_CONFIG.width;
        height = CURRENT_GRAPHICS_CONFIG.height;
        CURRENT_GRAPHICS_CONFIG.showMenu = showMenu;
        KBChangeMenu(NULL);
        CURRENT_GRAPHICS_CONFIG.width = winWidth;
        CURRENT_GRAPHICS_CONFIG.height = height;
        WritePrefs();
        windowStyle = GetWindowLongA(hwndApp, GWL_STYLE);
        if (CURRENT_GRAPHICS_CONFIG.showMenu)
            giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
        else
            giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;
        replacedStyle = SetWindowLongA(hwndApp, GWL_STYLE, giCurWindowsStyleFlags);
        ShowWindow(hwndApp, SW_SHOWNA);
        ResizeWindow(-1, -1, CURRENT_GRAPHICS_CONFIG.width, CURRENT_GRAPHICS_CONFIG.height);
    }
}

VA(0x00471e77, 0x53)
void SetNoDialogMenus(b32 menusEnabled) {
    if (gbNoDialogMenusOn && !menusEnabled)
        return;
    if (!gbNoDialogMenusOn && menusEnabled)
        return;
    if (!hmnuApp)
        return;
    gbNoDialogMenusOn = 1 - menusEnabled;
    SetMenus(hmnuApp, menusEnabled);
}

#if H2_RETAIL_COMPILER
#define position pos
#endif
VA(0x00471eca, 0x12e)
void SetMenus(HMENU menu, b32 enabled) {
    i32 count;
    u32 id;
    i32 match;
    i32 position;
    i32 disabled;
    i32 index;

    count = GetMenuItemCount(menu);
    for (index = 0; index < count; index++) {
        id = GetMenuItemID(menu, index);
        if (id == -1) {
            SetMenus(GetSubMenu(menu, index), enabled);
            disabled = 0;
        } else {
            disabled = 0;
            if (enabled) {
                disabled = 1;
            } else {
                match = 0;
                for (position = 0; position < MENU_ENABLE_STATUS_COUNT; position++) {
                    if (gsMenuEnableStatus[position].command == id) {
                        match = position;
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
#if H2_RETAIL_COMPILER
#undef position
#endif

VA(0x00471ff8, 0xb)
i32l KBTickCount(void) {
    return GetTickCount();
}

VA(0x00472003, 0x5)
void InitVideo(void) {
    return;
}

DATA(0x005157dc) char szAppName[] = localization::Tr("system.title.short");
DATA(0x005157e8) char szTitle[] =
    localization::Tr("system.title.full");
DATA(0x00526cd4) HWND hwndApp = NULL;
DATA(0x00526cd8) HMENU hmnuApp = NULL;
DATA(0x00526cdc) HANDLE gEventHandle = NULL;
DATA(0x00526ce0) i32l lLastGTimerTickCount = 0;
DATA(0x00526ce4) i32l lLastCycleColorsTickCount = 0;
DATA(0x00526ce8) i32 bRestartMusic = 0;
DATA(0x00515800) i32 iLastMusic = -1;
DATA(0x00526cec) i32l lLastGetMessage = 0;
DATA(0x00526cf0) i32l lLastAilServe = 0;
DATA(0x00526cf4) b32 gbNoDialogMenusOn = false;
DATA(0x005268c8) HINSTANCE hInstApp;
DATA(0x005268a8) RECT rcTemp;
DATA(0x005268bc) i32 iMainWinScreenHeight;
DATA(0x00526ccc) HMENU hmnuCurrent;
DATA(0x005268c0) i32 iTempX;
DATA(0x005268c4) i32 iTempY;
DATA(0x005268b8) i32l lTemp;
DATA(0x005268cc) u8 bProcessMessage[KBWIN_PROCESS_MESSAGE_COUNT];
DATA(0x00526cd0) i32 iMainWinScreenWidth;
