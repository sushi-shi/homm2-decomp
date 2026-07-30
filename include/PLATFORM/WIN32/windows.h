#ifndef HOMM2_PLATFORM_WIN32_WINDOWS_H
#define HOMM2_PLATFORM_WIN32_WINDOWS_H

#include <cstddef>
#include <cstdint>

#ifdef _WIN32
#ifndef __cdecl
#define __cdecl __attribute__((cdecl))
#endif
#ifndef __stdcall
#define __stdcall __attribute__((stdcall))
#endif
#ifndef __fastcall
#define __fastcall __attribute__((fastcall))
#endif
#define WINAPI __stdcall
#define APIENTRY __stdcall
#define CALLBACK __stdcall
#define PASCAL __stdcall
#else
#define __cdecl
#define __stdcall
#define __fastcall
#define WINAPI
#define APIENTRY
#define CALLBACK
#define PASCAL
#endif
#define FAR
#define NEAR
#define WINGAPI WINAPI

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int BOOL;
typedef BOOL WINBOOL;
typedef unsigned int UINT;
typedef int INT;
typedef long LONG;
typedef unsigned long ULONG;
typedef short SHORT;
typedef unsigned short USHORT;
typedef char CHAR;
typedef float FLOAT;
typedef void VOID;

typedef char* LPSTR;
typedef const char* LPCSTR;
typedef void* LPVOID;
typedef void* PVOID;
typedef const void* LPCVOID;
typedef BYTE* LPBYTE;
typedef WORD* LPWORD;
typedef DWORD* LPDWORD;
typedef int* LPINT;
typedef long* LPLONG;
typedef BOOL* LPBOOL;

typedef std::intptr_t INT_PTR;
typedef std::uintptr_t UINT_PTR;
typedef UINT_PTR WPARAM;
typedef INT_PTR LPARAM;
typedef INT_PTR LRESULT;
typedef long HRESULT;

#define TRUE 1
#define FALSE 0
#define MAKEWORD(low, high) ((WORD)(((BYTE)(low)) | (((WORD)((BYTE)(high))) << 8)))

typedef void* HANDLE;
typedef void* HWND;
typedef void* HDC;
typedef void* HINSTANCE;
typedef void* HMENU;
typedef void* HICON;
typedef void* HBITMAP;
typedef void* HPALETTE;
typedef void* HBRUSH;
typedef void* HGDIOBJ;
typedef void* HKEY;

typedef HINSTANCE HMODULE;
typedef HICON HCURSOR;
typedef HKEY* PHKEY;

typedef int (*FARPROC)();

struct _GUID {
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[8];
};
typedef struct _GUID GUID;
typedef GUID* LPGUID;
typedef GUID IID;
typedef GUID CLSID;

struct IUnknown;

#define INVALID_HANDLE_VALUE ((HANDLE)(std::intptr_t)-1)

struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};
typedef tagRECT RECT;
typedef tagRECT* LPRECT;

struct tagPOINT {
    LONG x;
    LONG y;
};
typedef tagPOINT POINT;
typedef tagPOINT* LPPOINT;

struct tagMSG {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
};
typedef tagMSG MSG;
typedef tagMSG* LPMSG;

struct tagPAINTSTRUCT {
    HDC hdc;
    BOOL fErase;
    RECT rcPaint;
    BOOL fRestore;
    BOOL fIncUpdate;
    BYTE rgbReserved[32];
};
typedef tagPAINTSTRUCT PAINTSTRUCT;
typedef tagPAINTSTRUCT* LPPAINTSTRUCT;

struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
};
typedef tagPALETTEENTRY PALETTEENTRY;
typedef tagPALETTEENTRY* LPPALETTEENTRY;

struct tagLOGPALETTE {
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[1];
};
typedef tagLOGPALETTE LOGPALETTE;
typedef tagLOGPALETTE* LPLOGPALETTE;

struct tagRGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
};
typedef tagRGBQUAD RGBQUAD;

#pragma pack(push, 1)
struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};
typedef tagBITMAPINFOHEADER BITMAPINFOHEADER;

struct tagBITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
};
typedef tagBITMAPINFO BITMAPINFO;
#pragma pack(pop)

typedef BITMAPINFO* LPBITMAPINFO;
typedef BITMAPINFOHEADER* LPBITMAPINFOHEADER;

struct tagBITMAP {
    LONG bmType;
    LONG bmWidth;
    LONG bmHeight;
    LONG bmWidthBytes;
    WORD bmPlanes;
    WORD bmBitsPixel;
    LPVOID bmBits;
};
typedef tagBITMAP BITMAP;

struct tagICONINFO {
    BOOL fIcon;
    DWORD xHotspot;
    DWORD yHotspot;
    HBITMAP hbmMask;
    HBITMAP hbmColor;
};
typedef tagICONINFO ICONINFO;

struct CRITICAL_SECTION {
    void* opaque;
};
typedef CRITICAL_SECTION* LPCRITICAL_SECTION;

struct SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
};
typedef SECURITY_ATTRIBUTES* LPSECURITY_ATTRIBUTES;

struct OVERLAPPED {
    UINT_PTR Internal;
    UINT_PTR InternalHigh;
    DWORD Offset;
    DWORD OffsetHigh;
    HANDLE hEvent;
};
typedef OVERLAPPED* LPOVERLAPPED;

struct WNDCLASSA {
    UINT style;
    LRESULT (*lpfnWndProc)(HWND, UINT, WPARAM, LPARAM);
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCSTR lpszMenuName;
    LPCSTR lpszClassName;
};
typedef WNDCLASSA WNDCLASS;

typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef INT_PTR (*DLGPROC)(HWND, UINT, WPARAM, LPARAM);

struct DCB {
    DWORD DCBlength;
    DWORD BaudRate;
    DWORD fBinary : 1;
    DWORD fParity : 1;
    DWORD fOutxCtsFlow : 1;
    DWORD fOutxDsrFlow : 1;
    DWORD fDtrControl : 2;
    DWORD fDsrSensitivity : 1;
    DWORD fTXContinueOnXoff : 1;
    DWORD fOutX : 1;
    DWORD fInX : 1;
    DWORD fErrorChar : 1;
    DWORD fNull : 1;
    DWORD fRtsControl : 2;
    DWORD fAbortOnError : 1;
    DWORD fDummy2 : 17;
    WORD wReserved;
    WORD XonLim;
    WORD XoffLim;
    BYTE ByteSize;
    BYTE Parity;
    BYTE StopBits;
    char XonChar;
    char XoffChar;
    char ErrorChar;
    char EofChar;
    char EvtChar;
    WORD wReserved1;
};
typedef DCB* LPDCB;

struct COMMTIMEOUTS {
    DWORD ReadIntervalTimeout;
    DWORD ReadTotalTimeoutMultiplier;
    DWORD ReadTotalTimeoutConstant;
    DWORD WriteTotalTimeoutMultiplier;
    DWORD WriteTotalTimeoutConstant;
};
typedef COMMTIMEOUTS* LPCOMMTIMEOUTS;

struct COMSTAT {
    DWORD fCtsHold : 1;
    DWORD fDsrHold : 1;
    DWORD fRlsdHold : 1;
    DWORD fXoffHold : 1;
    DWORD fXoffSent : 1;
    DWORD fEof : 1;
    DWORD fTxim : 1;
    DWORD fReserved : 25;
    DWORD cbInQue;
    DWORD cbOutQue;
};
typedef COMSTAT* LPCOMSTAT;

struct FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};

#define MAX_PATH 260

#define LOWORD(value) ((WORD)((UINT_PTR)(value) & 0xFFFF))
#define HIWORD(value) ((WORD)(((UINT_PTR)(value) >> 16) & 0xFFFF))
#define LOBYTE(value) ((BYTE)((UINT_PTR)(value) & 0xFF))
#define HIBYTE(value) ((BYTE)(((UINT_PTR)(value) >> 8) & 0xFF))
#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#define MAKELPARAM(low, high) ((LPARAM)MAKELONG(low, high))
#define RGB(r, g, b) ((DWORD)(((BYTE)(r)) | (((WORD)((BYTE)(g))) << 8) | (((DWORD)((BYTE)(b))) << 16)))

struct WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[MAX_PATH];
    CHAR cAlternateFileName[14];
};
typedef WIN32_FIND_DATAA* LPWIN32_FIND_DATAA;

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_MOVE 0x0003
#define WM_SIZE 0x0005
#define WM_ACTIVATE 0x0006
#define WM_SETFOCUS 0x0007
#define WM_KILLFOCUS 0x0008
#define WM_PAINT 0x000F
#define WM_CLOSE 0x0010
#define WM_QUIT 0x0012
#define WM_ERASEBKGND 0x0014
#define WM_QUERYNEWPALETTE 0x030F
#define WM_PALETTECHANGED 0x0311
#define WM_ACTIVATEAPP 0x001C
#define WM_SETICON 0x0080
#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111
#define WM_TIMER 0x0113
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_CHAR 0x0102
#define WM_SYSKEYDOWN 0x0104
#define WM_SYSKEYUP 0x0105
#define WM_MOUSEMOVE 0x0200
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_LBUTTONDBLCLK 0x0203
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP 0x0205
#define WM_RBUTTONDBLCLK 0x0206
#define WM_USER 0x0400

#define VK_F6 0x75
#define VK_F7 0x76

#define PM_NOREMOVE 0x0000
#define PM_REMOVE 0x0001

#define SW_HIDE 0
#define SW_SHOWNORMAL 1
#define SW_SHOW 5
#define SW_SHOWNA 8

#define WS_OVERLAPPED 0x00000000L
#define WS_POPUP 0x80000000L
#define WS_VISIBLE 0x10000000L
#define WS_CAPTION 0x00C00000L
#define WS_SYSMENU 0x00080000L
#define WS_MINIMIZEBOX 0x00020000L
#define WS_MINIMIZE 0x20000000L
#define WS_MAXIMIZE 0x01000000L
#define WS_OVERLAPPEDWINDOW 0x00CF0000L

#define CS_HREDRAW 0x0002
#define CS_VREDRAW 0x0001
#define CS_DBLCLKS 0x0008
#define CS_OWNDC 0x0020

#define GWL_STYLE (-16)
#define GWL_EXSTYLE (-20)

#define MB_OK 0x0000
#define MB_OKCANCEL 0x0001
#define MB_YESNO 0x0004
#define MB_ICONHAND 0x0010
#define MB_ICONQUESTION 0x0020
#define MB_ICONEXCLAMATION 0x0030
#define IDOK 1
#define IDCANCEL 2
#define IDYES 6
#define IDNO 7

#define MF_ENABLED 0x0000
#define MF_GRAYED 0x0001
#define MF_CHECKED 0x0008
#define MF_UNCHECKED 0x0000
#define MF_BYCOMMAND 0x0000
#define MF_BYPOSITION 0x0400

#define IDC_ARROW ((LPCSTR)32512)
#define IDI_APPLICATION ((LPCSTR)32512)
#define COLOR_WINDOW 5

#define SRCCOPY 0x00CC0020L
#define PATCOPY 0x00F00021L
#define BLACKNESS 0x00000042L

#define HORZSIZE 4
#define VERTSIZE 6
#define HORZRES 8
#define VERTRES 10
#define BITSPIXEL 12
#define PLANES 14
#define NUMCOLORS 24
#define RASTERCAPS 38
#define SIZEPALETTE 104

#define ICON_SMALL 0
#define ICON_BIG 1

#define HELP_CONTEXT 0x0001
#define HELP_QUIT 0x0002
#define HELP_INDEX 0x0003
#define HELP_CONTENTS 0x0003
#define HELP_HELPONHELP 0x0004
#define HELP_KEY 0x0101
#define HELP_FINDER 0x000B

#define DIB_RGB_COLORS 0
#define DIB_PAL_COLORS 1
#define BI_RGB 0
#define PC_NOCOLLAPSE 0x04
#define PC_RESERVED 0x01

#define GENERIC_READ 0x80000000L
#define GENERIC_WRITE 0x40000000L
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010

#define INFINITE 0xFFFFFFFF
#define WAIT_OBJECT_0 0x00000000L
#define WAIT_TIMEOUT 0x00000102L
#define WAIT_FAILED 0xFFFFFFFF

#define HKEY_CLASSES_ROOT ((HKEY)(UINT_PTR)0x80000000)
#define HKEY_CURRENT_USER ((HKEY)(UINT_PTR)0x80000001)
#define HKEY_LOCAL_MACHINE ((HKEY)(UINT_PTR)0x80000002)
#define KEY_READ 0x20019
#define KEY_WRITE 0x20006
#define KEY_ALL_ACCESS 0xF003F
#define REG_SZ 1
#define REG_BINARY 3
#define REG_DWORD 4
#define ERROR_SUCCESS 0L
#define ERROR_INVALID_FUNCTION 1L
#define ERROR_TOO_MANY_OPEN_FILES 4L
#define ERROR_INVALID_HANDLE 6L
#define ERROR_NOT_ENOUGH_MEMORY 8L
#define ERROR_INVALID_ACCESS 12L
#define ERROR_INVALID_DATA 13L
#define ERROR_SHARING_VIOLATION 32L
#define ERROR_INVALID_PARAMETER 87L
#define ERROR_IO_PENDING 997L
#define ERROR_ARENA_TRASHED 7L
#define ERROR_BAD_FORMAT 11L
#define ERROR_INVALID_DRIVE 15L
#define ERROR_BAD_ENVIRONMENT 10L
#define ERROR_NOT_SAME_DEVICE 17L
#define ERROR_NO_MORE_FILES 18L
#define ERROR_WRITE_PROTECT 19L
#define ERROR_BAD_UNIT 20L
#define ERROR_NOT_READY 21L
#define ERROR_BAD_COMMAND 22L
#define ERROR_CRC 23L
#define ERROR_BAD_LENGTH 24L
#define ERROR_SEEK 25L
#define ERROR_NOT_DOS_DISK 26L
#define ERROR_SECTOR_NOT_FOUND 27L
#define ERROR_OUT_OF_PAPER 28L
#define ERROR_WRITE_FAULT 29L
#define ERROR_READ_FAULT 30L
#define ERROR_GEN_FAILURE 31L
#define ERROR_LOCK_VIOLATION 33L
#define ERROR_HANDLE_EOF 38L
#define ERROR_HANDLE_DISK_FULL 39L
#define ERROR_HANDLE_NO_MEMORY 8L
#define ERROR_OPERATION_ABORTED 995L
#define ERROR_COUNTER_TIMEOUT 1121L
#define ERROR_INVALID_BLOCK 9L
#define ERROR_BAD_ARGUMENTS 160L
#define ERROR_CURRENT_DIRECTORY 16L
#define ERROR_NO_MORE_SEARCH_HANDLES 113L
#define ERROR_INVALID_TARGET_HANDLE 114L
#define ERROR_INVALID_CATEGORY 117L
#define ERROR_INVALID_LEVEL 124L
#define ERROR_NO_VOLUME_LABEL 125L
#define MAXDWORD 0xFFFFFFFF

#define DTR_CONTROL_DISABLE 0x00
#define DTR_CONTROL_ENABLE 0x01
#define DTR_CONTROL_HANDSHAKE 0x02
#define RTS_CONTROL_DISABLE 0x00
#define RTS_CONTROL_ENABLE 0x01
#define RTS_CONTROL_HANDSHAKE 0x02
#define RTS_CONTROL_TOGGLE 0x03
#define NOPARITY 0
#define ODDPARITY 1
#define EVENPARITY 2
#define ONESTOPBIT 0
#define TWOSTOPBITS 2
#define CBR_9600 9600
#define CBR_19200 19200
#define CBR_38400 38400
#define CBR_57600 57600
#define CBR_115200 115200
#define MS_CTS_ON 0x0010
#define MS_DSR_ON 0x0020
#define MS_RING_ON 0x0040
#define MS_RLSD_ON 0x0080
#define CE_RXOVER 0x0001
#define CE_OVERRUN 0x0002
#define CE_FRAME 0x0008
#define CE_BREAK 0x0010
#define ERROR_FILE_NOT_FOUND 2L
#define ERROR_PATH_NOT_FOUND 3L
#define ERROR_ACCESS_DENIED 5L
#define ERROR_ALREADY_EXISTS 183L

HMODULE LoadLibraryA(LPCSTR name);
BOOL FreeLibrary(HMODULE module);
FARPROC GetProcAddress(HMODULE module, LPCSTR name);

DWORD GetTickCount();
void Sleep(DWORD milliseconds);
DWORD GetLastError();
void OutputDebugStringA(LPCSTR text);
int lstrlenA(LPCSTR text);

HANDLE CreateFileA(LPCSTR name, DWORD access, DWORD share,
                   LPSECURITY_ATTRIBUTES security, DWORD creation,
                   DWORD attributes, HANDLE templateFile);
BOOL ReadFile(HANDLE file, LPVOID buffer, DWORD toRead, LPDWORD read,
              LPOVERLAPPED overlapped);
BOOL WriteFile(HANDLE file, LPCVOID buffer, DWORD toWrite, LPDWORD written,
               LPOVERLAPPED overlapped);
BOOL CloseHandle(HANDLE object);
DWORD GetFileAttributesA(LPCSTR name);
HANDLE FindFirstFileA(LPCSTR pattern, LPWIN32_FIND_DATAA data);
BOOL FindNextFileA(HANDLE find, LPWIN32_FIND_DATAA data);
BOOL FindClose(HANDLE find);
void InitializeCriticalSection(LPCRITICAL_SECTION section);
void DeleteCriticalSection(LPCRITICAL_SECTION section);
void EnterCriticalSection(LPCRITICAL_SECTION section);
void LeaveCriticalSection(LPCRITICAL_SECTION section);

HANDLE CreateEventA(LPSECURITY_ATTRIBUTES security, BOOL manualReset,
                    BOOL initialState, LPCSTR name);
BOOL SetEvent(HANDLE event);
BOOL ResetEvent(HANDLE event);
DWORD WaitForSingleObject(HANDLE object, DWORD milliseconds);
DWORD WaitForMultipleObjects(DWORD count, const HANDLE* objects, BOOL waitAll,
                             DWORD milliseconds);

BOOL GetCommState(HANDLE file, LPDCB dcb);
BOOL SetCommState(HANDLE file, LPDCB dcb);
BOOL GetCommTimeouts(HANDLE file, LPCOMMTIMEOUTS timeouts);
BOOL SetCommTimeouts(HANDLE file, LPCOMMTIMEOUTS timeouts);
BOOL SetupComm(HANDLE file, DWORD inQueue, DWORD outQueue);
BOOL SetCommBreak(HANDLE file);
BOOL ClearCommBreak(HANDLE file);
BOOL ClearCommError(HANDLE file, LPDWORD errors, LPCOMSTAT status);
BOOL GetCommModemStatus(HANDLE file, LPDWORD status);

typedef WORD ATOM;

ATOM RegisterClassA(const WNDCLASSA* windowClass);
HWND CreateWindowExA(DWORD exStyle, LPCSTR className, LPCSTR windowName,
                     DWORD style, int x, int y, int width, int height,
                     HWND parent, HMENU menu, HINSTANCE instance, LPVOID param);
BOOL DestroyWindow(HWND window);
LRESULT DefWindowProcA(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

BOOL GetMessageA(LPMSG message, HWND window, UINT filterMin, UINT filterMax);
BOOL PeekMessageA(LPMSG message, HWND window, UINT filterMin, UINT filterMax,
                  UINT remove);
BOOL TranslateMessage(const MSG* message);
LRESULT DispatchMessageA(const MSG* message);
BOOL PostMessageA(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
void PostQuitMessage(int exitCode);
BOOL WaitMessage();

BOOL ShowWindow(HWND window, int command);
BOOL UpdateWindow(HWND window);
BOOL MoveWindow(HWND window, int x, int y, int width, int height, BOOL repaint);
BOOL InvalidateRect(HWND window, const RECT* rect, BOOL erase);
BOOL GetClientRect(HWND window, LPRECT rect);
BOOL GetWindowRect(HWND window, LPRECT rect);
BOOL AdjustWindowRect(LPRECT rect, DWORD style, BOOL menu);
BOOL OffsetRect(LPRECT rect, int dx, int dy);
BOOL ClientToScreen(HWND window, LPPOINT point);
BOOL ScreenToClient(HWND window, LPPOINT point);
LONG GetWindowLongA(HWND window, int index);
LONG SetWindowLongA(HWND window, int index, LONG value);

HDC GetDC(HWND window);
int ReleaseDC(HWND window, HDC dc);
HDC BeginPaint(HWND window, LPPAINTSTRUCT paint);
BOOL EndPaint(HWND window, const PAINTSTRUCT* paint);

HCURSOR LoadCursorA(HINSTANCE instance, LPCSTR name);
HCURSOR SetCursor(HCURSOR cursor);
int ShowCursor(BOOL show);
BOOL GetCursorPos(LPPOINT point);
HWND SetCapture(HWND window);
BOOL ReleaseCapture();
SHORT GetAsyncKeyState(int key);

HICON LoadIconA(HINSTANCE instance, LPCSTR name);
HICON CreateIconIndirect(ICONINFO* info);
BOOL DestroyIcon(HICON icon);

HMENU LoadMenuA(HINSTANCE instance, LPCSTR name);
BOOL SetMenu(HWND window, HMENU menu);
BOOL DestroyMenu(HMENU menu);
BOOL DrawMenuBar(HWND window);
HMENU GetSubMenu(HMENU menu, int position);
int GetMenuItemCount(HMENU menu);
UINT GetMenuItemID(HMENU menu, int position);
BOOL EnableMenuItem(HMENU menu, UINT item, UINT flags);
DWORD CheckMenuItem(HMENU menu, UINT item, UINT flags);

int MessageBoxA(HWND window, LPCSTR text, LPCSTR caption, UINT type);
BOOL MessageBeep(UINT type);
UINT SetTimer(HWND window, UINT_PTR id, UINT elapse, void* callback);
BOOL KillTimer(HWND window, UINT_PTR id);
INT_PTR DialogBoxParamA(HINSTANCE instance, LPCSTR templateName, HWND parent,
                        DLGPROC dialogFunc, LPARAM initParam);
BOOL EndDialog(HWND dialog, INT_PTR result);
BOOL WinHelpA(HWND window, LPCSTR help, UINT command, DWORD data);
int wsprintfA(LPSTR output, LPCSTR format, ...);

HPALETTE CreatePalette(const LOGPALETTE* palette);
HPALETTE SelectPalette(HDC dc, HPALETTE palette, BOOL forceBackground);
UINT RealizePalette(HDC dc);
BOOL AnimatePalette(HPALETTE palette, UINT start, UINT count,
                    const PALETTEENTRY* entries);
UINT GetSystemPaletteEntries(HDC dc, UINT start, UINT count,
                             LPPALETTEENTRY entries);
HBITMAP CreateBitmapIndirect(const BITMAP* bitmap);
HGDIOBJ SelectObject(HDC dc, HGDIOBJ object);
BOOL DeleteObject(HGDIOBJ object);
BOOL DeleteDC(HDC dc);
int GetDeviceCaps(HDC dc, int index);
BOOL PatBlt(HDC dc, int x, int y, int width, int height, DWORD rop);
DWORD GdiSetBatchLimit(DWORD limit);

LONG RegOpenKeyExA(HKEY key, LPCSTR subKey, DWORD options, DWORD desired,
                   PHKEY result);
LONG RegCreateKeyA(HKEY key, LPCSTR subKey, PHKEY result);
LONG RegQueryValueExA(HKEY key, LPCSTR valueName, LPDWORD reserved,
                      LPDWORD type, LPBYTE data, LPDWORD size);
LONG RegSetValueExA(HKEY key, LPCSTR valueName, DWORD reserved, DWORD type,
                    const BYTE* data, DWORD size);
LONG RegCloseKey(HKEY key);

typedef UINT MMRESULT;

struct WAVEOUTCAPSA {
    WORD wMid;
    WORD wPid;
    UINT vDriverVersion;
    CHAR szPname[32];
    DWORD dwFormats;
    WORD wChannels;
    WORD wReserved1;
    DWORD dwSupport;
};
typedef WAVEOUTCAPSA* LPWAVEOUTCAPSA;

#define MIDI_MAPPER ((UINT)-1)

#define WAVE_FORMAT_PCM 1

#pragma pack(push, 1)
struct WAVEFORMAT {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
};

struct PCMWAVEFORMAT {
    WAVEFORMAT wf;
    WORD wBitsPerSample;
};
#pragma pack(pop)

typedef WAVEFORMAT* LPWAVEFORMAT;
typedef PCMWAVEFORMAT* LPPCMWAVEFORMAT;

struct OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR szCSDVersion[128];
};
typedef OSVERSIONINFOA OSVERSIONINFO;
typedef OSVERSIONINFOA* LPOSVERSIONINFOA;
typedef OSVERSIONINFOA* LPOSVERSIONINFO;

#define VER_PLATFORM_WIN32s 0
#define VER_PLATFORM_WIN32_WINDOWS 1
#define VER_PLATFORM_WIN32_NT 2

BOOL GetVersionExA(LPOSVERSIONINFOA info);
DWORD GetVersion();

UINT waveOutGetNumDevs();
MMRESULT waveOutGetDevCapsA(UINT_PTR device, LPWAVEOUTCAPSA caps, UINT size);
#define FindFirstFile FindFirstFileA
#define FindNextFile FindNextFileA
#define CreateFile CreateFileA
#define GetFileAttributes GetFileAttributesA
#define GetVersionEx GetVersionExA
#define LoadLibrary LoadLibraryA
#define MessageBox MessageBoxA
#define OutputDebugString OutputDebugStringA
#define wsprintf wsprintfA
#define WIN32_FIND_DATA WIN32_FIND_DATAA

#endif
