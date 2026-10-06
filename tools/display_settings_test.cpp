#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <PLATFORM/SDL3/Sdl3Video.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

namespace fs = std::filesystem;
namespace {
bool Expect(bool value, const char* what) {
    if (!value) std::fprintf(stderr, "display settings: %s\n", what);
    return value;
}
void Environment(const char* name, const std::string& value) {
#ifdef _WIN32
    _putenv_s(name, value.c_str());
#else
    setenv(name, value.c_str(), 1);
#endif
}
std::string LatestFrame(const fs::path& root) {
    fs::path latest;
    for (const auto& entry : fs::directory_iterator(root))
        if (entry.path().extension() == ".ppm" && entry.path() > latest) latest = entry.path();
    std::ifstream stream(latest, std::ios::binary);
    return {std::istreambuf_iterator<char>(stream), {}};
}
std::vector<platform::Event> events;
void Collect(const platform::Event& event) { events.push_back(event); }
bool Motion(platform::sdl3::Video& video, float x, float y, int expectedX, int expectedY) {
    platform::PumpEvents();
    events.clear();
    SDL_Event event{};
    event.type = SDL_EVENT_MOUSE_MOTION;
    event.motion.windowID = SDL_GetWindowID(video.Window());
    event.motion.x = x;
    event.motion.y = y;
    SDL_PushEvent(&event);
    platform::PumpEvents();
    for (const auto& result : events)
        if (result.type == platform::Event::Type::MouseMove)
            return Expect(result.position.x == expectedX && result.position.y == expectedY,
                          "mouse maps to logical pixels, including outside borders");
    return Expect(false, "mouse event delivered");
}
}

int main() {
    const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
    const fs::path root = fs::temp_directory_path() / ("homm2-display-" + std::to_string(nonce));
    fs::create_directories(root / "data" / "DATA");
    std::ofstream(root / "data" / "DATA" / "HEROES2.AGG").put('\0');
    Environment("HOMM2_DATA", (root / "data").string());
    Environment("XDG_DATA_HOME", (root / "state").string());
    const char* driver = std::getenv("HOMM2_DISPLAY_TEST_DRIVER");
    Environment("SDL_VIDEODRIVER", driver != nullptr ? driver : "dummy");
    Environment("SDL_AUDIODRIVER", "dummy");
    Environment("HOMM2_SCREENSHOT", (root / "frame").string());
    Environment("HOMM2_SCREENSHOT_EVERY", "1");
    if (!Expect(platform::Startup(), "startup")) return 1;
    auto& video = static_cast<platform::sdl3::Video&>(platform::Video());
    bool valid = true;
    valid &= Expect(!video.SetFullscreen(true) && !video.SetScaling(platform::Scaling::Linear)
                    && !video.SetVSync(true), "closed display rejects changes");
    platform::DisplayMode mode;
    mode.width = 64;
    mode.height = 48;
    mode.scale = 2;
    if (!Expect(video.Open(mode), "open")) return 1;
    auto* const pixels = video.Pixels();
    SDL_Window* const window = video.Window();
    SDL_Renderer* const renderer = video.Renderer();
    const platform::Color colors[] = {{0, 0, 0}, {12, 34, 56}, {200, 123, 67}};
    video.SetPalette(colors, 0, 3);
    std::fill_n(pixels, 64 * 48, 1);
    video.Blit({0, 0, 64, 48}, {0, 0});
    pixels[64 + 1] = 2;
    video.Blit({1, 1, 1, 1}, {1, 1});
    pixels[64 + 1] = 1; // The cursor is restored in the work buffer after composition.
    video.Present();
    const std::string reference = LatestFrame(root);
    const std::string header = "P6\n64 48\n255\n";
    valid &= Expect(reference.size() == header.size() + 64 * 48 * 3
        && reference.starts_with(header) && reference[header.size()] == 12
        && static_cast<unsigned char>(reference[header.size() + 65 * 3]) == 200,
        "capture contains palette and composed cursor, not restored working pixels");
    platform::SetEventHandler(Collect);
    for (const auto scaling : {platform::Scaling::Nearest, platform::Scaling::Linear,
                               platform::Scaling::Integer}) {
        valid &= Expect(video.SetScaling(scaling), "change scaling");
        valid &= Expect(video.Settings().scaling == scaling, "reported scaling");
        int width = 0, height = 0;
        SDL_RendererLogicalPresentation policy{};
        valid &= Expect(SDL_GetRenderLogicalPresentation(renderer, &width, &height, &policy)
            && width == 64 && height == 48
            && policy == (scaling == platform::Scaling::Integer
                ? SDL_LOGICAL_PRESENTATION_INTEGER_SCALE : SDL_LOGICAL_PRESENTATION_LETTERBOX),
            "renderer applies the requested aspect and integer policy");
        for (const bool fullscreen : {true, false, true, false}) {
            valid &= Expect(video.SetFullscreen(fullscreen), "toggle fullscreen");
            valid &= Expect(video.Settings().fullscreen == fullscreen, "actual fullscreen state");
            valid &= Expect(video.Pixels() == pixels && video.Window() == window
                && video.Renderer() == renderer && pixels[65] == 1,
                "in-place changes preserve the working buffer, window, and renderer");
            valid &= Expect(LatestFrame(root) == reference, "palette and composed cursor survive mode switch");
        }
        for (const auto size : {platform::Size{170, 140}, platform::Size{32, 24}}) {
            valid &= Expect(SDL_SetWindowSize(window, size.width, size.height), "resize window");
            SDL_SyncWindow(window);
            platform::PumpEvents();
            float x = 0, y = 0;
            valid &= Expect(SDL_RenderCoordinatesToWindow(renderer, 10.25f, 15.25f, &x, &y), "map logical point to window");
            valid &= Motion(video, x, y, 10, 15);
            valid &= Expect(SDL_RenderCoordinatesToWindow(renderer, -0.25f, -0.25f, &x, &y), "map outside logical point");
            valid &= Motion(video, x, y, -1, -1);
        }
        valid &= Expect(video.HandleRenderEvent(SDL_EVENT_RENDER_DEVICE_RESET, SDL_GetWindowID(window)), "recreate lost texture");
        valid &= Expect(video.Settings().scaling == scaling && LatestFrame(root) == reference,
                        "device reset preserves settings, pixels, palette, cursor");
    }
    valid &= Motion(video, std::numeric_limits<float>::quiet_NaN(), 0, -1, -1);
    platform::PumpEvents();
    events.clear();
    for (const bool repeat : {false, true}) {
        SDL_Event key{};
        key.type = SDL_EVENT_KEY_DOWN;
        key.key.scancode = SDL_SCANCODE_F4;
        key.key.key = SDLK_F4;
        key.key.repeat = repeat;
        SDL_PushEvent(&key);
    }
    SDL_Event release{};
    release.type = SDL_EVENT_KEY_UP;
    release.key.scancode = SDL_SCANCODE_F4;
    release.key.key = SDLK_F4;
    SDL_PushEvent(&release);
    platform::PumpEvents();
    const auto downs = std::count_if(events.begin(), events.end(), [](const auto& event) {
        return event.type == platform::Event::Type::KeyDown && event.key == platform::Key::F4;
    });
    valid &= Expect(downs == 1 && !platform::Input().IsKeyDown(platform::Key::F4),
                    "holding F4 does not repeat mode switches and key release still arrives");
    const auto before = video.Settings();
    valid &= Expect(!video.SetScaling(static_cast<platform::Scaling>(99))
                    && video.Settings().scaling == before.scaling, "invalid scaling leaves current mode intact");
    // Some render drivers cannot enable VSync; failure must leave its previous state intact.
    const bool enabled = video.SetVSync(true);
    valid &= Expect(video.Settings().vsync == (enabled ? true : before.vsync), "VSync reports actual driver state");
    valid &= Expect(video.SetVSync(false) && !video.Settings().vsync, "disable VSync");
    valid &= Expect(std::string(SDL_GetWindowTitle(window)).ends_with("integer | VSync off"),
                    "window title exposes current presentation settings");

    auto& files = platform::Files();
    const platform::DisplaySettings saved{true, platform::Scaling::Linear, true};
    valid &= Expect(platform::WriteDisplaySettings(files, saved), "write preferences");
    platform::DisplaySettings loaded;
    valid &= Expect(platform::ReadDisplaySettings(files, loaded) && loaded.fullscreen
        && loaded.scaling == platform::Scaling::Linear && loaded.vsync, "read preference round trip");
    const std::string path = files.Resolve("HEROES2.DISPLAY", platform::FileMode::Write);
#ifndef _WIN32
    valid &= Expect(path.starts_with(files.UserRoot()), "preferences are user state");
#endif
    for (const std::string malformed : {"", "H2DISPLAY 2\nfullscreen 1\nscaling linear\nvsync 1\n",
            "H2DISPLAY 1\nfullscreen 2\nscaling nearest\nvsync 0\n",
            "H2DISPLAY 1\nfullscreen 1\nscaling bad\nvsync 0\n",
            "H2DISPLAY 1\nfullscreen 1\nscaling linear\nvsync 0\nextra", "H2DISPLAY 1"}) {
        std::ofstream(path, std::ios::binary | std::ios::trunc) << malformed;
        loaded = saved;
        valid &= Expect(!platform::ReadDisplaySettings(files, loaded) && loaded.fullscreen
            && loaded.scaling == saved.scaling && loaded.vsync, "malformed preferences leave all defaults unchanged");
    }
    std::ofstream(path, std::ios::binary | std::ios::trunc) << std::string(256, 'a');
    valid &= Expect(!platform::ReadDisplaySettings(files, loaded), "oversize preference file rejected");
    video.Close();
    mode.fullscreen = true;
    mode.scaling = platform::Scaling::Linear;
    mode.vsync = false;
    valid &= Expect(video.Open(mode) && video.Settings().fullscreen
        && video.Settings().scaling == mode.scaling, "open honors initial settings");
    platform::Shutdown();
    std::error_code error;
    fs::remove_all(root, error);
    return valid ? 0 : 1;
}
