// Optional production-path integration test: requires installed assets and a
// video driver with hardware cursor support (e.g. X11 under Xvfb, not dummy).
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <BASE/executive.h>
#include <BASE/mouseManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/X_GLOBAL.h>

#include <SDL3/SDL.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>

int main() {
    if (std::getenv("HOMM2_DATA") == nullptr) {
        std::fputs("Set HOMM2_DATA to installed assets; use X11/Wayland, not SDL dummy video.\n", stderr);
        return 2;
    }
    if (!platform::Startup()) return 2;
    localization::Initialize("/i0");
    std::strcpy(gcCommandLine, "/i0");
    if (!EarlySetup()) return 2;
    InitGraphics();
    gpExec->InitSystem();

    auto& mouse = *gpMouseManager;
    mouse.SetPointer("ADVMCO.ICN", 0, MOUSE_CURSOR_ADVENTURE);
    mouse.SetColorMice(false);
    SDL_Cursor* const system = SDL_GetDefaultCursor();
    if (system == nullptr || SDL_GetCursor() == system) {
        std::fputs("Hardware cursor creation unavailable or game cursor not selected.\n", stderr);
        return 1;
    }
    std::set<SDL_Cursor*> handles;
    int cases = 0, failures = 0;
    for (const auto type : {MOUSE_CURSOR_ADVENTURE, MOUSE_CURSOR_COMBAT, MOUSE_CURSOR_SPELL}) {
        const int count = type == MOUSE_CURSOR_ADVENTURE ? 41 : type == MOUSE_CURSOR_COMBAT ? 16 : 39;
        for (int frame = 0; frame < count; ++frame) {
            mouse.SetPointer("", frame, type);
            SDL_Cursor* const handle = SDL_GetCursor();
            if (handle == nullptr || handle == system) ++failures;
            handles.insert(handle);
            mouse.ReallyHidePointer();
            // A failed later selection must not pass merely because an earlier
            // game cursor is still active. Force each cached shape from default.
            platform::Video().ResetCursor();
            mouse.SetPointer(MOUSE_KEEP_CURRENT_FRAME);
            if (SDL_GetCursor() != handle || SDL_CursorVisible()) ++failures;
            mouse.ReallyShowPointer();
            if (!SDL_CursorVisible()) ++failures;
            ++cases;
        }
    }
    mouse.SetColorMice(true);
    if (SDL_CursorVisible() || mouse.m_cursorType != MOUSE_CURSOR_SPELL
        || mouse.m_cursorFrame != 38) ++failures;
    mouse.SetColorMice(false);
    if (!SDL_CursorVisible() || SDL_GetCursor() == system
        || mouse.m_cursorType != MOUSE_CURSOR_SPELL || mouse.m_cursorFrame != 38) ++failures;
    mouse.Close();
    if (SDL_GetCursor() != system) ++failures;
    std::fprintf(stderr, "real cursor assets: %d selections, %zu distinct native shapes, %d failures\n",
                 cases, handles.size(), failures);
    std::fflush(nullptr);
    // This fixture does not own a fully initialized game session.
    std::_Exit(failures != 0 ? 1 : 0);
}
