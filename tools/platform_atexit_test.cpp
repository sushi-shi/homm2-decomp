#include <PLATFORM/Platform.h>

#include <SDL3/SDL.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

std::filesystem::path gRoot;
std::filesystem::path gOutput;

void VerifyCleanup() {
    std::ifstream stream(gOutput, std::ios::binary);
    std::string contents(4, '\0');
    stream.read(contents.data(), static_cast<std::streamsize>(contents.size()));
    std::error_code error;
    std::filesystem::remove_all(gRoot, error);
    if (contents != "exit") {
        std::fprintf(stderr, "platform atexit cleanup did not flush the open stream\n");
        std::_Exit(1);
    }
}

bool SetEnvironment(const char* name, const std::string& value) {
    return setenv(name, value.c_str(), 1) == 0;
}

}

int main() {
    const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
    gRoot = std::filesystem::temp_directory_path()
        / ("homm2-platform-atexit-" + std::to_string(nonce));
    const std::filesystem::path data = gRoot / "game";
    const std::filesystem::path state = gRoot / "state";
    std::error_code error;
    std::filesystem::create_directories(data / "DATA", error);
    std::ofstream(data / "DATA" / "HEROES2.AGG", std::ios::binary).put('\0');

    if (!SetEnvironment("SDL_VIDEODRIVER", "dummy")
        || !SetEnvironment("SDL_AUDIODRIVER", "dummy")
        || !SetEnvironment("HOMM2_DATA", data.string())
        || !SetEnvironment("XDG_DATA_HOME", state.string())
        || std::atexit(VerifyCleanup) != 0
        || !platform::Startup()) {
        std::filesystem::remove_all(gRoot, error);
        return 1;
    }

    gOutput = std::filesystem::path(platform::Files().UserRoot()) / "GAMES" / "atexit.bin";
    const i32 file = platform::Files().Open("GAMES\\atexit.bin", platform::FileMode::Write);
    const char contents[] = "exit";
    if (file < 0 || platform::Files().Write(file, contents, 4) != 4) {
        std::filesystem::remove_all(gRoot, error);
        return 1;
    }

    std::exit(0);
}
