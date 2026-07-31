#include <windows.h>

#include <stdio.h>
#include <string.h>

#include <Ints.h>
#include <PLATFORM/Entry.h>
#include <PLATFORM/WIN32/Application.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

// Retail's entry, kept where it belongs: only this executable is allowed to
// know what a HINSTANCE is.
namespace {

HANDLE gEventHandle = NULL;

}

extern "C" i32 __stdcall
WinMain(HINSTANCE instance, HINSTANCE previousInstance, char* commandLine, i32 showCommand) {
    gEventHandle = CreateEventA(NULL, 0, 0, "Heroes II");
    if (gEventHandle == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        sprintf(gText, "Only one copy of %s may run at a time", "Heroes of Might and Magic II");
        MessageBoxA(NULL, gText, "Startup Error", MB_ICONHAND);
        return 0;
    }

    return platform::Run(commandLine);
}

// The build is console subsystem, so the runtime calls this one. It hands the
// command line over the way the Windows entry expects it. Once the game stops
// borrowing our own <windows.h>, this can read GetCommandLineA() instead.
int main(int argc, char** argv) {
    char commandLine[KBWIN_COMMAND_LINE_CLEAR_SIZE];
    commandLine[0] = '\0';
    for (int argument = 1; argument < argc; ++argument) {
        if (commandLine[0] != '\0') {
            strncat(commandLine, " ", sizeof(commandLine) - strlen(commandLine) - 1);
        }
        strncat(commandLine, argv[argument], sizeof(commandLine) - strlen(commandLine) - 1);
    }

    return WinMain(NULL, NULL, commandLine, SW_SHOWNORMAL);
}
