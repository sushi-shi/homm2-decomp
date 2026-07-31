#ifndef HOMM2_PLATFORM_WIN32_STATE_H
#define HOMM2_PLATFORM_WIN32_STATE_H

#include <string>

#include <windows.h>

#include <PLATFORM/FileSystem.h>

namespace platform::win32 {

bool EnsureDisplay();

void SetWindowProc(WNDPROC proc);
WNDPROC GetWindowProc();
HWND MainWindow();

bool PumpOne(MSG* message);

std::string ResolvePath(const char* retailPath, FileMode mode = FileMode::Read);

void TrackGdiObject(void* object);
bool ForgetGdiObject(void* object);

}

#endif
