#include "Sdl3Internal.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

namespace platform::sdl3 {
namespace {

class FileSystem final : public IFileSystem {
public:
    FileSystem() {
#ifdef __EMSCRIPTEN__
        m_programRoot = "/";
        m_dataRoot = "/game";
#else
        if (const char* base = SDL_GetBasePath()) {
            m_programRoot = base;
        }
        if (m_programRoot.empty()) {
            std::error_code error;
            m_programRoot = std::filesystem::current_path(error).string();
            if (error) {
                m_programRoot = ".";
            }
        }
        while (m_programRoot.size() > 1 && m_programRoot.back() == '/') {
            m_programRoot.pop_back();
        }

        if (const char* configured = SDL_getenv("HOMM2_DATA");
            configured != nullptr && *configured != '\0') {
            m_dataRoot = configured;
        } else {
            m_dataRoot = FindDataRoot();
        }
        if (const char* localeData = SDL_getenv("HOMM2_LOCALE_DATA");
            localeData != nullptr && *localeData != '\0') {
            m_localeDataRoot = localeData;
        }
#endif

        if (char* preferences = SDL_GetPrefPath("homm2", "homm2")) {
            m_userRoot = preferences;
            SDL_free(preferences);
        } else {
            m_userRoot = m_dataRoot;
        }
        PrepareUserState(m_userRoot);
    }

    ~FileSystem() override {
        for (auto& [file, stream] : m_streams) {
            static_cast<void>(file);
            if (!SDL_CloseIO(stream)) {
                std::fprintf(stderr, "[homm2] SDL_CloseIO during shutdown: %s\n", SDL_GetError());
            }
        }
    }

    std::string DataRoot() const override { return m_dataRoot; }
    std::string UserRoot() const override { return m_userRoot; }
    std::string ProgramRoot() const override { return m_programRoot; }
    std::string LocaleDataRoot() const override { return m_localeDataRoot; }

    std::string Resolve(const char* retailPath, FileMode mode) const override {
        if (!IsUserState(retailPath)) {
            return ResolveIn(m_dataRoot, retailPath);
        }

        const std::string user = ResolveIn(m_userRoot, retailPath);
        if (mode != FileMode::Read || Present(user)) {
            return user;
        }
        const std::string data = ResolveIn(m_dataRoot, retailPath);
        return Present(data) ? data : user;
    }

    std::vector<std::string> List(const char* pattern) const override {
        std::string retail = pattern != nullptr ? pattern : "";
        std::replace(retail.begin(), retail.end(), '\\', '/');
        const std::size_t slash = retail.find_last_of('/');
        const std::string directory = slash == std::string::npos ? "" : retail.substr(0, slash);
        const std::string wildcard =
            slash == std::string::npos ? retail : retail.substr(slash + 1);

        std::vector<std::string> names;
        if (IsUserState(pattern)) {
            Collect(Directory(m_userRoot, directory), wildcard, names);
        }
        Collect(Directory(m_dataRoot, directory), wildcard, names);
        return names;
    }

    bool Exists(const char* retailPath) const override {
        return Present(Resolve(retailPath, FileMode::Read));
    }

    i32 Open(const char* retailPath, FileMode mode) override {
        const char* how = "wb";
        if (mode == FileMode::Read) {
            how = "rb";
        } else if (mode == FileMode::Append) {
            how = "ab";
        }
        return OpenPath(Resolve(retailPath, mode), how);
    }

    i32 OpenLocale(const char* retailPath) override {
        if (m_localeDataRoot.empty()) {
            return -1;
        }
        return OpenPath(ResolveIn(m_localeDataRoot, retailPath), "rb");
    }

    void Close(i32 file) override {
        const auto found = m_streams.find(file);
        if (found == m_streams.end()) {
            return;
        }
        if (!SDL_CloseIO(found->second)) {
            std::fprintf(stderr, "[homm2] SDL_CloseIO: %s\n", SDL_GetError());
        }
        m_streams.erase(found);
    }

    i32 Read(i32 file, void* buffer, i32 count) override {
        SDL_IOStream* stream = Stream(file);
        if (stream == nullptr || count < 0 || (count > 0 && buffer == nullptr)) {
            return -1;
        }
        if (count == 0) {
            return 0;
        }
        return static_cast<i32>(
            SDL_ReadIO(stream, buffer, static_cast<std::size_t>(count))
        );
    }

    i32 Write(i32 file, const void* buffer, i32 count) override {
        SDL_IOStream* stream = Stream(file);
        if (stream == nullptr || count < 0 || (count > 0 && buffer == nullptr)) {
            return -1;
        }
        if (count == 0) {
            return 0;
        }
        return static_cast<i32>(
            SDL_WriteIO(stream, buffer, static_cast<std::size_t>(count))
        );
    }

    i32 Seek(i32 file, i32 offset) override {
        SDL_IOStream* stream = Stream(file);
        if (stream == nullptr) {
            return -1;
        }
        return NarrowPosition(SDL_SeekIO(stream, offset, SDL_IO_SEEK_SET));
    }

    i32 Tell(i32 file) override {
        SDL_IOStream* stream = Stream(file);
        return stream != nullptr ? NarrowPosition(SDL_TellIO(stream)) : -1;
    }

    i32 Length(i32 file) override {
        SDL_IOStream* stream = Stream(file);
        return stream != nullptr ? NarrowPosition(SDL_GetIOSize(stream)) : -1;
    }

private:
    static bool Present(const std::string& path) {
        std::error_code error;
        return !path.empty() && std::filesystem::exists(path, error);
    }

    static i32 NarrowPosition(Sint64 position) {
        return position >= 0 && position <= std::numeric_limits<i32>::max()
            ? static_cast<i32>(position)
            : -1;
    }

    i32 OpenPath(const std::string& path, const char* mode) {
        if (path.empty()) {
            return -1;
        }
        SDL_IOStream* stream = SDL_IOFromFile(path.c_str(), mode);
        if (stream == nullptr) {
            return -1;
        }
        const i32 file = AllocateHandle();
        if (file < 0) {
            SDL_CloseIO(stream);
            return -1;
        }
        m_streams.emplace(file, stream);
        return file;
    }

    i32 AllocateHandle() {
        const std::size_t attempts = m_streams.size() + 1;
        for (std::size_t attempt = 0; attempt < attempts; ++attempt) {
            if (m_nextFile == 0
                || m_nextFile > static_cast<std::uint32_t>(std::numeric_limits<i32>::max())) {
                m_nextFile = 1;
            }
            const i32 candidate = static_cast<i32>(m_nextFile++);
            if (!m_streams.contains(candidate)) {
                return candidate;
            }
        }
        return -1;
    }

    SDL_IOStream* Stream(i32 file) const {
        const auto found = m_streams.find(file);
        return found != m_streams.end() ? found->second : nullptr;
    }

    static std::string FindDataRoot() {
        std::vector<std::string> candidates;
        if (const char* base = SDL_GetBasePath()) {
            candidates.emplace_back(base);
        }
        std::error_code error;
        const std::filesystem::path working = std::filesystem::current_path(error);
        candidates.emplace_back(error ? std::string(".") : working.string());

        std::string dataHome;
        if (const char* data = SDL_getenv("XDG_DATA_HOME"); data != nullptr && *data != '\0') {
            dataHome = data;
        } else if (const char* home = SDL_getenv("HOME"); home != nullptr && *home != '\0') {
            dataHome = std::string(home) + "/.local/share";
        }
        if (!dataHome.empty()) {
            candidates.emplace_back(dataHome + "/homm2");
            candidates.emplace_back(dataHome + "/homm2/data");
        }
        if (const char* home = SDL_getenv("HOME"); home != nullptr && *home != '\0') {
            candidates.emplace_back(std::string(home) + "/games/homm2");
        }

        for (const std::string& candidate : candidates) {
            if (HoldsGameData(candidate)) {
                return candidate;
            }
        }
        std::fprintf(
            stderr,
            "[homm2] no game data found, set HOMM2_DATA to the directory holding "
            "DATA/HEROES2.AGG\n"
        );
        return candidates.front();
    }

    static bool HoldsGameData(const std::string& directory) {
        std::error_code error;
        return std::filesystem::exists(
            ResolveIn(directory, "DATA\\HEROES2.AGG"),
            error
        );
    }

    static std::filesystem::path Directory(
        const std::string& root,
        const std::string& directory
    ) {
        return directory.empty() ? std::filesystem::path(root)
                                 : std::filesystem::path(ResolveIn(root, directory.c_str()));
    }

    static void Collect(
        const std::filesystem::path& root,
        const std::string& wildcard,
        std::vector<std::string>& names
    ) {
        std::error_code error;
        for (const auto& entry : std::filesystem::directory_iterator(root, error)) {
            const std::string name = entry.path().filename().string();
            if (!Matches(wildcard.c_str(), name.c_str())) {
                continue;
            }
            const auto known = [&name](const std::string& seen) {
                return SDL_strcasecmp(seen.c_str(), name.c_str()) == 0;
            };
            if (std::none_of(names.begin(), names.end(), known)) {
                names.push_back(name);
            }
        }
    }

    static bool Matches(const char* pattern, const char* name) {
        const char* star = nullptr;
        const char* retry = nullptr;
        while (*name != '\0') {
            if (*pattern == '?'
                || std::tolower(static_cast<unsigned char>(*pattern))
                    == std::tolower(static_cast<unsigned char>(*name))) {
                ++pattern;
                ++name;
            } else if (*pattern == '*') {
                star = pattern++;
                retry = name;
            } else if (star != nullptr) {
                pattern = star + 1;
                name = ++retry;
            } else {
                return false;
            }
        }
        while (*pattern == '*') {
            ++pattern;
        }
        return *pattern == '\0';
    }

    std::string m_programRoot;
    std::string m_dataRoot;
    std::string m_userRoot;
    std::string m_localeDataRoot;
    std::map<i32, SDL_IOStream*> m_streams;
    std::uint32_t m_nextFile = 1;
};

}

std::unique_ptr<IFileSystem> CreateFileSystem() {
    return std::make_unique<FileSystem>();
}

}
