#ifndef HOMM2_PLATFORM_RUNTIME_H
#define HOMM2_PLATFORM_RUNTIME_H

#include <Ints.h>
#include <PLATFORM/Input.h>

namespace platform {

using MenuHandle = void*;
using EventHandler = void (*)(const Event&);

enum class SystemMenuCommand : i32 {
    Size640x480 = 0x9c45,
    Size800x600 = 0x9c46,
    Size1024x768 = 0x9c47,
    Size1280x1024 = 0x9c48,
    Fullscreen = 0x9c49,
    Help = 0x9c74,
    About = 0x9c75,
};

i32l Ticks();
void SetEventHandler(EventHandler handler);
void PumpEvents();

// Retail hung a Windows menu bar off the window, one per screen. The game half
// of that survives whole - HandleAppSpecificMenuCommands still runs every
// command - but no backend draws a menu, so these keep the bookkeeping and
// nothing looks at it. A backend that grew menus would find the game ready.
MenuHandle LoadMenu(const char* name);
void DestroyMenu(MenuHandle menu);
MenuHandle CurrentMenu();
void ChangeMenu(MenuHandle menu);
void SetMenuItemChecked(MenuHandle menu, i32 command, bool checked);
void SetDialogMenusEnabled(i32 enabled);
void SetMenuVisible(i32 visible);
void InitializeVideo();
void ShowHelp();
void ShowMessage(const char* title, const char* message);
void RequestQuit();

}

#endif
