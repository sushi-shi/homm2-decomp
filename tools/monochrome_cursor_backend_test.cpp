#include <PLATFORM/Platform.h>
#include <SDL3/SDL.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>

namespace {
// Dummy SDL video has no hardware cursor. Wrap only the four cursor calls,
// retaining the production Video implementation and the real SDL lifecycle.
int creations = 0, selections = 0, destructions = 0, failures = 0;
bool failCreate = false, failSelect = false;
SDL_Cursor* selected = nullptr;
std::set<SDL_Cursor*> live;
int defaultToken;
platform::MonochromeCursor captured;

void Expect(bool ok, const char* message) {
    if (!ok) {
        std::fprintf(stderr, "cursor backend: %s\n", message);
        ++failures;
    }
}
}

extern "C" SDL_Cursor* __wrap_SDL_CreateCursor(const Uint8* data, const Uint8* mask,
                                               int width, int height, int x, int y) {
    ++creations;
    Expect(width == 32 && height == 32, "cursor dimensions");
    std::memcpy(captured.data.data(), data, captured.data.size());
    std::memcpy(captured.mask.data(), mask, captured.mask.size());
    captured.hotspot = {x, y};
    if (failCreate) return nullptr;
    auto* cursor = reinterpret_cast<SDL_Cursor*>(new int(creations));
    live.insert(cursor);
    return cursor;
}

extern "C" SDL_Cursor* __wrap_SDL_GetDefaultCursor() {
    return reinterpret_cast<SDL_Cursor*>(&defaultToken);
}

extern "C" bool __wrap_SDL_SetCursor(SDL_Cursor* cursor) {
    ++selections;
    if (failSelect) return false;
    Expect(cursor == __wrap_SDL_GetDefaultCursor() || live.contains(cursor), "valid selection");
    selected = cursor;
    return true;
}

extern "C" void __wrap_SDL_DestroyCursor(SDL_Cursor* cursor) {
    Expect(selected != cursor, "deselect before destroy");
    Expect(live.erase(cursor) == 1, "destroy exactly once");
    delete reinterpret_cast<int*>(cursor);
    ++destructions;
}

int main() {
    setenv("SDL_VIDEODRIVER", "dummy", 1);
    setenv("SDL_AUDIODRIVER", "dummy", 1);
    if (!platform::Startup()) return 1;
    auto& video = platform::Video();
    platform::MonochromeCursor first;
    first.data[0] = 0x80;
    first.mask[0] = 0xc0;
    first.hotspot = {2, 3};
    Expect(!video.SetMonochromeCursor(first), "selection before open rejected");
    platform::DisplayMode mode;
    mode.width = mode.height = 32;
    mode.scale = 1;
    Expect(video.Open(mode), "video open");
    video.ShowCursor(false);
    Expect(video.SetMonochromeCursor(first), "select first");
    Expect(captured == first, "pixels and hotspot reach SDL unchanged");
    const auto* firstHandle = selected;
    Expect(!SDL_CursorVisible(), "selection does not unhide cursor");
    Expect(video.SetMonochromeCursor(first) && creations == 1, "identical image reuses handle");
    auto second = first;
    second.data[0] = 0x40;
    Expect(video.SetMonochromeCursor(second) && selected != firstHandle, "shape changes");
    Expect(video.SetMonochromeCursor(first) && selected == firstHandle, "old shape reused");
    second = first;
    second.hotspot = {15, 15};
    Expect(video.SetMonochromeCursor(second) && creations == 3, "hotspot participates in cache key");
    auto invalid = first;
    invalid.hotspot.x = 32;
    Expect(!video.SetMonochromeCursor(invalid) && creations == 3, "invalid hotspot rejected");
    const auto* previous = selected;
    failCreate = true;
    second.mask[1] = 1;
    Expect(!video.SetMonochromeCursor(second) && selected == previous, "creation failure keeps prior cursor");
    failCreate = false;
    failSelect = true;
    Expect(!video.SetMonochromeCursor(first) && selected == previous && live.size() == 3,
           "cached selection failure keeps prior cursor and cache");
    Expect(!video.SetMonochromeCursor(second) && live.size() == 3, "selection failure releases new handle");
    failSelect = false;
    video.ShowCursor(true);
    Expect(SDL_CursorVisible(), "explicit show");
    video.ResetCursor();
    Expect(selected == __wrap_SDL_GetDefaultCursor(), "reset restores desktop cursor");
    video.Close();
    Expect(live.empty() && destructions == 4, "close releases all cached handles");
    Expect(video.Open(mode), "reopen video");
    Expect(video.SetMonochromeCursor(first) && live.size() == 1, "reopen rebuilds cache");
    platform::Shutdown();
    Expect(live.empty(), "shutdown releases cursors");
    std::printf("cursor backend: %d creations, %d selections, %d failures\n", creations, selections, failures);
    return failures != 0;
}
