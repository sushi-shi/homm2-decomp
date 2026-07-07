#ifndef HOMM2_WIN_WINDOWS_H
#define HOMM2_WIN_WINDOWS_H
// Minimal custom Win32 API declarations (Gruntz-style) — only the imports retail TUs use.
// Opaque HANDLE/HWND/HMENU/HINSTANCE are modelled as void*. Staging: extend as needed.

extern "C" __declspec(dllimport) int   __stdcall MessageBoxA(void *, const char *, const char *, unsigned int);
extern "C" __declspec(dllimport) int   __stdcall SetMenu(void *, void *);
extern "C" __declspec(dllimport) int   __stdcall DestroyMenu(void *);
extern "C" __declspec(dllimport) void *__stdcall LoadMenuA(void *, const char *);
extern "C" __declspec(dllimport) int   __stdcall CloseHandle(void *);

#endif // HOMM2_WIN_WINDOWS_H
