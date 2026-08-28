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

// Retail stores millisecond deadlines in a wrapping 32-bit counter. Compare
// the signed distance, not the signed counter values, so short waits work on
// both sides of the 0x7fffffff and 0xffffffff boundaries.
constexpr bool TickDeadlinePending(u32 deadline, u32 now) {
    return static_cast<i32>(deadline - now) > 0;
}

constexpr bool TickDeadlineExpired(u32 deadline, u32 now) {
    return static_cast<i32>(now - deadline) > 0;
}

void SetEventHandler(EventHandler handler);
void PumpEvents();
void StartTextInput();
void StopTextInput();

// Retail hung a Windows menu bar off the window, one per screen. The game half
// of that survives whole - HandleAppSpecificMenuCommands still runs every
// command - but no backend draws a menu, so these keep the bookkeeping and
// nothing looks at it. A backend that grew menus would find the game ready.
MenuHandle LoadMenu(const char* name);
void DestroyMenu(MenuHandle menu);
MenuHandle CurrentMenu();
void ChangeMenu(MenuHandle menu);
void SetMenuItemChecked(MenuHandle menu, i32 command, bool checked);
void SetDialogMenusEnabled(b32 enabled);
void SetMenuVisible(b32 visible);
void InitializeVideo();
void ShowHelp();
void ShowMessage(const char* title, const char* message);
void RequestQuit();

}

#endif
