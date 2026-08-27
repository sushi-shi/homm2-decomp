#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <PLATFORM/Sockets.h>

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

std::vector<platform::Event> gEvents;

void CollectEvent(const platform::Event& event) {
    gEvents.push_back(event);
}

bool Expect(bool condition, const char* description) {
    if (condition) {
        return true;
    }
    std::fprintf(stderr, "platform backend mismatch: %s\n", description);
    return false;
}

bool SetEnvironment(const char* name, const std::string& value) {
#ifdef _WIN32
    return _putenv_s(name, value.c_str()) == 0;
#else
    return setenv(name, value.c_str(), 1) == 0;
#endif
}

}

int main() {
    const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
    const std::filesystem::path root = std::filesystem::temp_directory_path()
        / ("homm2-platform-test-" + std::to_string(nonce));
    const std::filesystem::path data = root / "game";
    const std::filesystem::path state = root / "state";
    std::error_code error;
    std::filesystem::create_directories(data / "DATA", error);
    std::filesystem::create_directories(data / "GAMES", error);
    std::ofstream(data / "DATA" / "HEROES2.AGG", std::ios::binary).put('\0');

    bool valid = true;
    valid &= Expect(SetEnvironment("SDL_VIDEODRIVER", "dummy"), "dummy video environment");
    valid &= Expect(SetEnvironment("SDL_AUDIODRIVER", "dummy"), "dummy audio environment");
    valid &= Expect(SetEnvironment("HOMM2_DATA", data.string()), "data environment");
    valid &= Expect(SetEnvironment("XDG_DATA_HOME", state.string()), "state environment");
    valid &= Expect(platform::Startup(), "startup");
    if (!valid) {
        std::filesystem::remove_all(root, error);
        return 1;
    }

    platform::DisplayMode mode;
    mode.width = 64;
    mode.height = 48;
    mode.scale = 1;
    mode.title = "HoMM2 platform test";
    valid &= Expect(platform::Video().Open(mode), "dummy video open");
    if (platform::Video().Pixels() != nullptr) {
        std::fill_n(
            platform::Video().Pixels(),
            static_cast<std::size_t>(mode.width) * static_cast<std::size_t>(mode.height),
            static_cast<std::uint8_t>(1)
        );
        const platform::Color palette[] = {{0, 0, 0}, {12, 34, 56}};
        platform::Video().SetPalette(palette, 0, 2);
        platform::Video().Blit({-2, -2, mode.width, mode.height}, {2, 2});
        platform::Video().Present();
    } else {
        valid &= Expect(false, "video pixels");
    }

    valid &= Expect(platform::Audio().Open(), "dummy audio open");
    const std::uint8_t silence[32] = {};
    platform::SoundData sound;
    sound.samples = silence;
    sound.byteCount = sizeof(silence);
    const platform::VoiceId voice = platform::Audio().PlaySound(sound, 127, 0);
    valid &= Expect(voice != 0, "dummy sound playback");
    platform::Audio().StopVoice(voice);
    valid &= Expect(platform::Audio().PlaySound(sound, 127, -2) == 0, "invalid loop count");

    const platform::Address address;
    valid &= Expect(
        platform::SendTo(platform::kInvalidSocket, nullptr, -1, address) == -1,
        "negative datagram size"
    );
    valid &= Expect(
        platform::ReceiveFrom(platform::kInvalidSocket, nullptr, 1, nullptr) == -1,
        "null datagram buffer"
    );

    const i32 file = platform::FileOpen("GAMES\\platform.bin", platform::FileMode::Write);
    valid &= Expect(file >= 0, "state file open");
    const char bytes[] = "test";
    valid &= Expect(!platform::FileWriteExact(file, bytes, -1), "negative write count");
    valid &= Expect(!platform::FileWriteExact(file, nullptr, 1), "null write buffer");
    valid &= Expect(platform::FileWriteExact(file, bytes, 0), "zero write count");
    valid &= Expect(platform::FileWriteExact(file, bytes, 4), "state file write");

    std::ofstream(data / "GAMES" / "Shipped.GIC", std::ios::binary).put('d');
    const std::filesystem::path userGames =
        std::filesystem::path(platform::Files().UserRoot()) / "GAMES";
    std::filesystem::create_directories(userGames, error);
    std::ofstream(userGames / "shipped.gic", std::ios::binary).put('u');
    const std::vector<std::string> listed = platform::Files().List("GAMES\\*.GIC");
    valid &= Expect(
        listed.size() == 1 && listed.front() == "shipped.gic",
        "user file shadows installation file case-insensitively"
    );

    platform::SetEventHandler(CollectEvent);
    platform::PumpEvents();
    gEvents.clear();

    SDL_Event key {};
    key.type = SDL_EVENT_KEY_DOWN;
    key.key.scancode = SDL_SCANCODE_LSHIFT;
    key.key.key = SDLK_LSHIFT;
    key.key.mod = SDL_KMOD_LSHIFT;
    SDL_PushEvent(&key);

    SDL_Event button {};
    button.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    button.button.button = SDL_BUTTON_LEFT;
    button.button.x = 4;
    button.button.y = 5;
    SDL_PushEvent(&button);
    platform::PumpEvents();
    valid &= Expect(platform::Input().IsKeyDown(platform::Key::Shift), "held key state");
    valid &= Expect(
        platform::Input().IsMouseButtonDown(platform::MouseButton::Left),
        "held mouse state"
    );

    gEvents.clear();
    SDL_Event focus {};
    focus.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    SDL_PushEvent(&focus);
    platform::PumpEvents();
    valid &= Expect(
        gEvents.size() == 3
            && gEvents[0].type == platform::Event::Type::KeyUp
            && gEvents[0].scanCode == 0x2a
            && gEvents[1].type == platform::Event::Type::MouseUp
            && gEvents[2].type == platform::Event::Type::FocusChanged
            && !gEvents[2].focused,
        "focus loss releases input before notification"
    );
    valid &= Expect(!platform::Input().IsKeyDown(platform::Key::Shift), "released key state");
    valid &= Expect(
        !platform::Input().IsMouseButtonDown(platform::MouseButton::Left),
        "released mouse state"
    );

    gEvents.clear();
    platform::RequestQuit();
    platform::PumpEvents();
    const auto quitCount = [] {
        return std::count_if(gEvents.begin(), gEvents.end(), [](const platform::Event& event) {
            return event.type == platform::Event::Type::Quit;
        });
    };
    valid &= Expect(quitCount() == 1, "quit request delivery");
    platform::RequestQuit();
    platform::PumpEvents();
    valid &= Expect(quitCount() == 1, "quit request delivered once");

    platform::SetEventHandler(nullptr);
    platform::Shutdown();

    std::ifstream persisted(userGames / "platform.bin", std::ios::binary);
    std::string persistedText(4, '\0');
    persisted.read(persistedText.data(), static_cast<std::streamsize>(persistedText.size()));
    valid &= Expect(persistedText == "test", "outstanding stream closed during shutdown");
    valid &= Expect(platform::Startup(), "second startup");
    platform::Shutdown();

    std::filesystem::remove_all(root, error);
    return valid ? 0 : 1;
}
