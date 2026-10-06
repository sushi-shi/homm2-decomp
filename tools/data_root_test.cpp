// Finding the game data the way a player sets it up: the program copied into
// the game folder and started from anywhere, or HOMM2_DATA typed into a shell
// (cmd keeps the quotes). Runs natively and, for the Windows build, under Wine.

#include <PLATFORM/FileSystem.h>

#include "src/PLATFORM/SDL3/Sdl3Internal.h"

#include <SDL3/SDL.h>

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <system_error>

namespace {

namespace fs = std::filesystem;

#ifdef _WIN32
constexpr char kSeparator = '\\';
#else
constexpr char kSeparator = '/';
#endif

bool gValid = true;

void Expect(bool condition, const std::string& description) {
    if (!condition) {
        std::fprintf(stderr, "data root mismatch: %s\n", description.c_str());
        gValid = false;
    }
}

void ExpectRoot(const platform::IFileSystem& files, const std::string& expected, const char* what) {
    Expect(
        files.DataRoot() == expected,
        std::string(what) + ": got '" + files.DataRoot() + "', expected '" + expected + "'"
    );
}

void Touch(const fs::path& path) {
    std::error_code error;
    fs::create_directories(path.parent_path(), error);
    std::ofstream(path, std::ios::binary).put('x');
}

void SetVariable(const char* name, const std::string* value) {
    SDL_Environment* environment = SDL_GetEnvironment();
    if (value != nullptr) {
        SDL_SetEnvironmentVariable(environment, name, value->c_str(), true);
    } else {
        SDL_UnsetEnvironmentVariable(environment, name);
    }
}

// The expansion archive as the game asks for it: lowercase, retail separators.
void ExpectArchive(platform::IFileSystem& files, const char* what) {
    const char* archive = ".\\DATA\\heroes2x.agg";
    Expect(files.Exists(archive), std::string(what) + ": archive exists");
    const i32 file = files.Open(archive, platform::FileMode::Read);
    Expect(file >= 0, std::string(what) + ": archive opens at " + files.Resolve(archive, platform::FileMode::Read));
    if (file >= 0) {
        files.Close(file);
    }
}

}

int main() {
    const std::string program = platform::ConfiguredDirectory(SDL_GetBasePath());
    Expect(!program.empty(), "program directory is known");
    const fs::path programPath = platform::HostPath(program);

    // A player's folder: the program beside DATA, archives in uppercase.
    Touch(programPath / "DATA" / "HEROES2.AGG");
    Touch(programPath / "DATA" / "HEROES2X.AGG");

    const std::string stamp =
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    const fs::path scratch =
        fs::temp_directory_path() / platform::HostPath("homm2-data-root-тест-" + stamp);
    const fs::path elsewhere = scratch / "elsewhere";
    // Another installation, below a non-ASCII name and spelled in lowercase.
    const fs::path other = scratch / platform::HostPath("Герои Might and Magic II");
    Touch(other / "data" / "heroes2.agg");
    Touch(other / "data" / "heroes2x.agg");
    std::error_code error;
    fs::create_directories(elsewhere, error);
    const std::string otherText = platform::HostString(other);

    // Keep preferences out of the real user directory.
    const std::string state = platform::HostString(scratch / "state");
    SetVariable("XDG_DATA_HOME", &state);
    SetVariable("HOMM2_DATA", nullptr);

    fs::current_path(elsewhere, error);
    {
        auto files = platform::sdl3::CreateFileSystem();
        ExpectRoot(*files, program, "started from another directory");
        ExpectArchive(*files, "started from another directory");
    }

    fs::current_path(programPath, error);
    {
        auto files = platform::sdl3::CreateFileSystem();
        ExpectRoot(*files, program, "started from the game folder");
        ExpectArchive(*files, "started from the game folder");
    }

    fs::current_path(elsewhere, error);
    const std::string quoted = "\"" + otherText + "\"";
    SetVariable("HOMM2_DATA", &quoted);
    {
        auto files = platform::sdl3::CreateFileSystem();
        ExpectRoot(*files, otherText, "quoted HOMM2_DATA");
        ExpectArchive(*files, "quoted HOMM2_DATA");
    }

    const std::string trailing = otherText + kSeparator + " ";
    SetVariable("HOMM2_DATA", &trailing);
    {
        auto files = platform::sdl3::CreateFileSystem();
        ExpectRoot(*files, otherText, "HOMM2_DATA with a trailing separator");
    }

    fs::current_path(programPath, error);
    fs::remove_all(scratch, error);
    fs::remove_all(programPath / "DATA", error);
    return gValid ? 0 : 1;
}
