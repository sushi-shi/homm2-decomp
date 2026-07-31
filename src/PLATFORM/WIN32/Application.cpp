#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Ints.h>
#include <BASE/INPUTMGR.h>
#include <BASE/Misc.h>
#include <BASE/heroWindowManager.h>
#include <BASE/soundManager.h>
#include <BASE/soundmgr.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <PLATFORM/Entry.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <PLATFORM/Graphics.h>
#include <PLATFORM/WIN32/Application.h>

i32 platform::RunApplication(const char* commandLine) {
    memset(gcCommandLine, 0, KBWIN_COMMAND_LINE_CLEAR_SIZE);
    strncpy(gcCommandLine, commandLine, KBWIN_COMMAND_LINE_LIMIT);
    if (EarlySetup() == 0)
        return 0;

    if (gConfig.gfx[H2EnumIndex(giCurExe)].showMenu != 0)
        giCurWindowsStyleFlags = KBWIN_WINDOWED_STYLE;
    else
        giCurWindowsStyleFlags = KBWIN_FULLSCREEN_STYLE;

    srand(static_cast<u32>(platform::Ticks()));
    InitGraphics();

    const platform::Size screen = platform::Video().Resolution();
    iMainWinScreenWidth = screen.width;
    iMainWinScreenHeight = screen.height;

    oldmain();

    ShutDown(NULL);
    return 0;
}

void ResizeWindow(i32 x, i32 y, i32 width, i32 height) {
    if (gConfig.gfx[H2EnumIndex(giCurExe)].fullScreen != 0)
        return;

    if (x != -1)
        gConfig.gfx[H2EnumIndex(giCurExe)].x = x;
    if (y != -1)
        gConfig.gfx[H2EnumIndex(giCurExe)].y = y;
    gConfig.gfx[H2EnumIndex(giCurExe)].width = width;
    gConfig.gfx[H2EnumIndex(giCurExe)].height = height;
    iMainWinScreenWidth = width;
    iMainWinScreenHeight = height;
    WritePrefs();
}

i32l KBTickCount(void) {
    return platform::Ticks();
}

HWND hwndApp = NULL;
HANDLE gEventHandle = NULL;
HINSTANCE hInstApp;
i32 iMainWinScreenHeight;
i32 iMainWinScreenWidth;
