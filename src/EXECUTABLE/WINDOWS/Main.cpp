#include <windows.h>

#include <stdio.h>

#include <Ints.h>
#include <BASE/Utf8.h>
#include <PLATFORM/Entry.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

// Retail's entry, kept where it belongs: only this executable is allowed to
// know what a HINSTANCE is.
namespace {

HANDLE gEventHandle = NULL;

}

extern "C" i32 __stdcall
WinMain(HINSTANCE, HINSTANCE, char* commandLine, i32) {
    gEventHandle = CreateEventA(NULL, 0, 0, "Heroes II");
    if (gEventHandle == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "Only one copy of %s may run at a time", "Heroes of Might and Magic II");
        MessageBoxA(NULL, gText, "Startup Error", MB_ICONHAND);
        return 0;
    }

    const i32 result = platform::Run(commandLine);
    CloseHandle(gEventHandle);
    return result;
}
