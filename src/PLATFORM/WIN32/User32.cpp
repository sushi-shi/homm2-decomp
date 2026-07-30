#include <windows.h>

#include <algorithm>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include <PLATFORM/Platform.h>

#include "State.h"

namespace {

WNDPROC gWindowProc = nullptr;
int gWindowToken = 0;
bool gDisplayOpen = false;
bool gQuitPosted = false;
int gCursorVisibility = 0;

bool gDirty = false;
RECT gDirtyRect = {0, 0, 0, 0};

bool gActivationSent = false;

struct Timer {
    UINT_PTR id = 0;
    UINT interval = 0;
    std::uint32_t due = 0;
};

std::vector<Timer> gTimers;

WPARAM ToVirtualKey(platform::Key key, unsigned character) {
    using platform::Key;
    switch (key) {
    case Key::Escape: return 0x1B;
    case Key::Return: return 0x0D;
    case Key::Space: return 0x20;
    case Key::Tab: return 0x09;
    case Key::Backspace: return 0x08;
    case Key::Delete: return 0x2E;
    case Key::Left: return 0x25;
    case Key::Up: return 0x26;
    case Key::Right: return 0x27;
    case Key::Down: return 0x28;
    case Key::Home: return 0x24;
    case Key::End: return 0x23;
    case Key::PageUp: return 0x21;
    case Key::PageDown: return 0x22;
    case Key::Insert: return 0x2D;
    case Key::Shift: return 0x10;
    case Key::Control: return 0x11;
    case Key::Alt: return 0x12;
    default: break;
    }
    if (key >= Key::F1 && key <= Key::F12) {
        return 0x70 + (static_cast<int>(key) - static_cast<int>(Key::F1));
    }
    if (key >= Key::A && key <= Key::Z) {
        return 'A' + (static_cast<int>(key) - static_cast<int>(Key::A));
    }
    if (key >= Key::Digit0 && key <= Key::Digit9) {
        return '0' + (static_cast<int>(key) - static_cast<int>(Key::Digit0));
    }
    return character;
}

LPARAM PackPoint(platform::Point point) {
    return static_cast<LPARAM>((point.y << 16) | (point.x & 0xFFFF));
}

}

namespace platform::win32 {

bool EnsureDisplay() {
    if (gDisplayOpen) {
        return true;
    }
    if (!platform::Startup()) {
        return false;
    }

    platform::DisplayMode mode;
    mode.width = 640;
    mode.height = 480;
    mode.title = "Heroes of Might and Magic II";
    if (!platform::Video().Open(mode)) {
        return false;
    }
    gDisplayOpen = true;
    return true;
}

void SetWindowProc(WNDPROC proc) { gWindowProc = proc; }
WNDPROC GetWindowProc() { return gWindowProc; }
HWND MainWindow() { return &gWindowToken; }

bool PumpOne(MSG* message) {
    if (!EnsureDisplay()) {
        return false;
    }

    platform::Host().Yield();

    if (!gActivationSent) {
        gActivationSent = true;
        std::memset(message, 0, sizeof(*message));
        message->hwnd = MainWindow();
        message->message = WM_ACTIVATEAPP;
        message->wParam = TRUE;
        return true;
    }

    platform::Event event;
    if (!platform::Input().Poll(event)) {

        const std::uint32_t now = platform::Host().Ticks();
        for (Timer& timer : gTimers) {
            if (static_cast<std::int32_t>(now - timer.due) >= 0) {
                timer.due = now + (timer.interval != 0 ? timer.interval : 1);
                std::memset(message, 0, sizeof(*message));
                message->hwnd = MainWindow();
                message->message = WM_TIMER;
                message->wParam = timer.id;
                return true;
            }
        }

        if (gDirty) {
            std::memset(message, 0, sizeof(*message));
            message->hwnd = MainWindow();
            message->message = WM_PAINT;
            return true;
        }
        if (platform::Host().ShouldQuit() && !gQuitPosted) {
            gQuitPosted = true;
            std::memset(message, 0, sizeof(*message));
            message->hwnd = MainWindow();
            message->message = WM_QUIT;
            return true;
        }
        return false;
    }

    std::memset(message, 0, sizeof(*message));
    message->hwnd = MainWindow();

    using Type = platform::Event::Type;
    switch (event.type) {
    case Type::Quit:

        message->message = WM_CLOSE;
        break;
    case Type::KeyDown:
    case Type::KeyUp:

        message->message = event.type == Type::KeyDown ? WM_KEYDOWN : WM_KEYUP;
        message->wParam = ToVirtualKey(event.key, event.character);
        message->lParam = static_cast<LPARAM>((event.scanCode & 0x7F) << 16);
        break;
    case Type::MouseMove:
        message->message = WM_MOUSEMOVE;
        message->lParam = PackPoint(event.position);
        break;
    case Type::MouseDown:
        message->message = event.button == platform::MouseButton::Right ? WM_RBUTTONDOWN
                                                                        : WM_LBUTTONDOWN;
        message->lParam = PackPoint(event.position);
        break;
    case Type::MouseUp:
        message->message = event.button == platform::MouseButton::Right ? WM_RBUTTONUP
                                                                        : WM_LBUTTONUP;
        message->lParam = PackPoint(event.position);
        break;
    case Type::FocusChanged:
        message->message = WM_ACTIVATEAPP;
        message->wParam = event.focused ? TRUE : FALSE;
        break;
    case Type::Resized: {
        const platform::Size size = platform::Video().Resolution();
        message->message = WM_SIZE;
        message->lParam = MAKELPARAM(size.width, size.height);
        gDirty = true;
        break;
    }
    default:

        return PumpOne(message);
    }
    return true;
}

}

ATOM RegisterClassA(const WNDCLASSA* windowClass) {
    if (windowClass != nullptr) {
        platform::win32::SetWindowProc(windowClass->lpfnWndProc);
    }
    return 1;
}

HWND CreateWindowExA(DWORD, LPCSTR, LPCSTR windowName, DWORD, int, int, int, int, HWND, HMENU,
                     HINSTANCE, LPVOID) {
    if (!platform::win32::EnsureDisplay()) {
        return nullptr;
    }
    HWND window = platform::win32::MainWindow();

    if (WNDPROC proc = platform::win32::GetWindowProc()) {
        proc(window, WM_CREATE, 0, 0);
        const platform::Size size = platform::Video().Resolution();
        proc(window, WM_SIZE, 0, MAKELPARAM(size.width, size.height));
    }
    return window;
}

BOOL DestroyWindow(HWND window) {
    if (WNDPROC proc = platform::win32::GetWindowProc()) {
        proc(window, WM_DESTROY, 0, 0);
    }
    return TRUE;
}

LRESULT DefWindowProcA(HWND, UINT, WPARAM, LPARAM) { return 0; }

BOOL GetMessageA(LPMSG message, HWND, UINT, UINT) {

    while (!platform::win32::PumpOne(message)) {
        platform::Host().Sleep(1);
    }
    return message->message != WM_QUIT ? TRUE : FALSE;
}

BOOL PeekMessageA(LPMSG message, HWND, UINT, UINT, UINT) {
    return platform::win32::PumpOne(message) ? TRUE : FALSE;
}

BOOL TranslateMessage(const MSG*) { return TRUE; }

LRESULT DispatchMessageA(const MSG* message) {
    if (message == nullptr) {
        return 0;
    }
    if (WNDPROC proc = platform::win32::GetWindowProc()) {
        return proc(message->hwnd, message->message, message->wParam, message->lParam);
    }
    return 0;
}

BOOL PostMessageA(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (WNDPROC proc = platform::win32::GetWindowProc()) {
        proc(window, message, wParam, lParam);
    }
    return TRUE;
}

void PostQuitMessage(int) { platform::Host().RequestQuit(); }

BOOL WaitMessage() {
    platform::Host().Sleep(1);
    return TRUE;
}

BOOL ShowWindow(HWND, int) { return TRUE; }
BOOL MoveWindow(HWND, int, int, int, int, BOOL) { return TRUE; }

BOOL InvalidateRect(HWND window, const RECT* rect, BOOL) {
    RECT area;
    if (rect != nullptr) {
        area = *rect;
    } else {
        GetClientRect(window, &area);
    }

    if (!gDirty) {
        gDirtyRect = area;
        gDirty = true;
        return TRUE;
    }

    gDirtyRect.left = area.left < gDirtyRect.left ? area.left : gDirtyRect.left;
    gDirtyRect.top = area.top < gDirtyRect.top ? area.top : gDirtyRect.top;
    gDirtyRect.right = area.right > gDirtyRect.right ? area.right : gDirtyRect.right;
    gDirtyRect.bottom = area.bottom > gDirtyRect.bottom ? area.bottom : gDirtyRect.bottom;
    return TRUE;
}

BOOL UpdateWindow(HWND window) {
    if (!gDirty) {
        return TRUE;
    }
    if (WNDPROC proc = platform::win32::GetWindowProc()) {
        proc(window, WM_PAINT, 0, 0);
    }
    return TRUE;
}

BOOL GetClientRect(HWND, LPRECT rect) {
    if (rect == nullptr) {
        return FALSE;
    }
    const platform::Size size = platform::Video().Resolution();
    rect->left = 0;
    rect->top = 0;
    rect->right = size.width;
    rect->bottom = size.height;
    return TRUE;
}

BOOL GetWindowRect(HWND window, LPRECT rect) { return GetClientRect(window, rect); }
BOOL AdjustWindowRect(LPRECT, DWORD, BOOL) { return TRUE; }

BOOL OffsetRect(LPRECT rect, int dx, int dy) {
    if (rect == nullptr) {
        return FALSE;
    }
    rect->left += dx;
    rect->right += dx;
    rect->top += dy;
    rect->bottom += dy;
    return TRUE;
}

BOOL ClientToScreen(HWND, LPPOINT) { return TRUE; }
BOOL ScreenToClient(HWND, LPPOINT) { return TRUE; }

LONG GetWindowLongA(HWND, int) { return 0; }
LONG SetWindowLongA(HWND, int, LONG) { return 0; }

HDC GetDC(HWND) { return platform::win32::MainWindow(); }
int ReleaseDC(HWND, HDC) { return 1; }

HDC BeginPaint(HWND window, LPPAINTSTRUCT paint) {
    if (paint != nullptr) {
        std::memset(paint, 0, sizeof(*paint));
        paint->hdc = platform::win32::MainWindow();
        if (gDirty) {
            paint->rcPaint = gDirtyRect;
        } else {
            GetClientRect(window, &paint->rcPaint);
        }
    }

    gDirty = false;
    return platform::win32::MainWindow();
}

BOOL EndPaint(HWND, const PAINTSTRUCT*) { return TRUE; }

HCURSOR LoadCursorA(HINSTANCE, LPCSTR) { return nullptr; }
HCURSOR SetCursor(HCURSOR) { return nullptr; }

int ShowCursor(BOOL show) {
    gCursorVisibility += show ? 1 : -1;
    platform::Video().ShowCursor(gCursorVisibility >= 0);
    return gCursorVisibility;
}

BOOL GetCursorPos(LPPOINT point) {
    if (point == nullptr) {
        return FALSE;
    }
    const platform::Point position = platform::Input().MousePosition();
    point->x = position.x;
    point->y = position.y;
    return TRUE;
}

HWND SetCapture(HWND) { return nullptr; }
BOOL ReleaseCapture() { return TRUE; }

SHORT GetAsyncKeyState(int key) {
    using platform::Key;
    platform::Key mapped = Key::Unknown;
    if (key >= 'A' && key <= 'Z') {
        mapped = static_cast<Key>(static_cast<int>(Key::A) + (key - 'A'));
    } else if (key >= 0x70 && key <= 0x7B) {
        mapped = static_cast<Key>(static_cast<int>(Key::F1) + (key - 0x70));
    } else {
        switch (key) {
        case 0x10: mapped = Key::Shift; break;
        case 0x11: mapped = Key::Control; break;
        case 0x12: mapped = Key::Alt; break;
        case 0x1B: mapped = Key::Escape; break;
        case 0x20: mapped = Key::Space; break;
        case 0x0D: mapped = Key::Return; break;
        default: return 0;
        }
    }

    return platform::Input().IsKeyDown(mapped) ? static_cast<SHORT>(0x8000) : 0;
}

HICON LoadIconA(HINSTANCE, LPCSTR) { return nullptr; }

HICON CreateIconIndirect(ICONINFO* info) {
    if (info == nullptr) {
        return nullptr;
    }
    ICONINFO* copy = new ICONINFO(*info);
    platform::win32::TrackGdiObject(copy);
    return copy;
}

BOOL DestroyIcon(HICON icon) {
    if (icon == nullptr || !platform::win32::ForgetGdiObject(icon)) {
        return TRUE;
    }
    delete static_cast<ICONINFO*>(icon);
    return TRUE;
}

HMENU LoadMenuA(HINSTANCE, LPCSTR) { return nullptr; }
BOOL SetMenu(HWND, HMENU) { return TRUE; }
BOOL DestroyMenu(HMENU) { return TRUE; }
BOOL DrawMenuBar(HWND) { return TRUE; }
HMENU GetSubMenu(HMENU, int) { return nullptr; }
int GetMenuItemCount(HMENU) { return 0; }
UINT GetMenuItemID(HMENU, int) { return 0; }
BOOL EnableMenuItem(HMENU, UINT, UINT) { return TRUE; }
DWORD CheckMenuItem(HMENU, UINT, UINT) { return 0; }

int MessageBoxA(HWND, LPCSTR text, LPCSTR caption, UINT) {
    platform::Host().ShowMessage(caption != nullptr ? caption : "Heroes of Might and Magic II",
                                 text != nullptr ? text : "");
    return IDOK;
}

BOOL MessageBeep(UINT) { return TRUE; }
UINT SetTimer(HWND, UINT_PTR id, UINT elapse, void*) {
    const std::uint32_t now = platform::Host().Ticks();
    for (Timer& timer : gTimers) {
        if (timer.id == id) {
            timer.interval = elapse;
            timer.due = now + elapse;
            return static_cast<UINT>(id);
        }
    }
    gTimers.push_back(Timer{id, elapse, now + elapse});
    return static_cast<UINT>(id);
}

BOOL KillTimer(HWND, UINT_PTR id) {
    std::erase_if(gTimers, [id](const Timer& timer) { return timer.id == id; });
    return TRUE;
}
INT_PTR DialogBoxParamA(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM) { return IDCANCEL; }
BOOL EndDialog(HWND, INT_PTR) { return TRUE; }
BOOL WinHelpA(HWND, LPCSTR, UINT, DWORD) { return TRUE; }

int wsprintfA(LPSTR output, LPCSTR format, ...) {
    va_list arguments;
    va_start(arguments, format);
    const int written = std::vsprintf(output, format, arguments);
    va_end(arguments);
    return written;
}
