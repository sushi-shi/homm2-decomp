#include <PLATFORM/Runtime.h>

#include <PLATFORM/Platform.h>

#include <cstdint>

namespace platform {

namespace {

MenuHandle gCurrentMenu;
EventHandler gEventHandler;

}

i32l Ticks() {
    return static_cast<i32l>(Host().Ticks());
}

void SetEventHandler(EventHandler handler) {
    gEventHandler = handler;
}

void PumpEvents() {
    Host().Yield();
    Audio().Service();
    Event event;
    while (Input().Poll(event)) {
        if (gEventHandler != nullptr) {
            gEventHandler(event);
        }
    }
}

void StartTextInput() {
    Input().StartTextInput();
}

void StopTextInput() {
    Input().StopTextInput();
}

MenuHandle LoadMenu(const char* name) {
    std::uintptr_t id = 1;
    for (const unsigned char* byte = reinterpret_cast<const unsigned char*>(name); *byte; ++byte) {
        id = id * 33 + *byte;
    }
    return reinterpret_cast<MenuHandle>(id);
}

void DestroyMenu(MenuHandle) {}

MenuHandle CurrentMenu() {
    return gCurrentMenu;
}

void ChangeMenu(MenuHandle menu) {
    if (menu != nullptr) {
        gCurrentMenu = menu;
    }
}

void SetMenuItemChecked(MenuHandle, i32, bool) {}

void SetDialogMenusEnabled(i32) {}

void SetMenuVisible(i32) {}

void InitializeVideo() {}

void ShowHelp() {
    Host().ShowMessage("Heroes II", "Help is not available in this build.");
}

void ShowMessage(const char* title, const char* message) {
    Host().ShowMessage(title, message);
}

void RequestQuit() {
    Host().RequestQuit();
}

}
