#include <PLATFORM/FileSystem.h>

#include <PLATFORM/Platform.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>
#include <vector>

namespace platform {
namespace {

#ifdef _WIN32
constexpr char kSeparator = '\\';

bool IsSeparator(char character) { return character == '\\' || character == '/'; }
#else
constexpr char kSeparator = '/';

bool IsSeparator(char character) { return character == '/'; }
#endif

bool Present(const std::string& path) {
    std::error_code error;
    return !path.empty() && std::filesystem::exists(HostPath(path), error);
}

bool SameIgnoringCase(const std::string& left, const std::string& right) {
    return left.size() == right.size()
        && std::equal(left.begin(), left.end(), right.begin(), [](char a, char b) {
               return std::tolower(static_cast<unsigned char>(a))
                   == std::tolower(static_cast<unsigned char>(b));
           });
}

struct CaseInsensitiveMatch {
    std::string name;
    bool ambiguous = false;
};

CaseInsensitiveMatch MatchIgnoringCase(
    const std::string& directory,
    const std::string& wanted
) {
    CaseInsensitiveMatch match;
    std::error_code error;
    std::filesystem::directory_iterator entry(
        HostPath(directory.empty() ? std::string(".") : directory),
        error
    );
    for (; !error && entry != std::filesystem::directory_iterator(); entry.increment(error)) {
        const std::string name = HostString(entry->path().filename());
        if (!SameIgnoringCase(name, wanted)) {
            continue;
        }
        if (!match.name.empty() && match.name != name) {
            match.ambiguous = true;
            break;
        }
        match.name = name;
    }
    return match;
}

std::vector<std::string> SplitPath(const std::string& path) {
    std::vector<std::string> components;
    std::string current;
    for (const char character : path) {
        if (character == '\\' || character == '/') {
            if (!current.empty()) {
                if (current == "..") {
                    if (!components.empty()) {
                        components.pop_back();
                    }
                } else if (current != ".") {
                    components.push_back(current);
                }
                current.clear();
            }
        } else {
            current.push_back(character);
        }
    }
    if (!current.empty()) {
        if (current == "..") {
            if (!components.empty()) {
                components.pop_back();
            }
        } else if (current != ".") {
            components.push_back(current);
        }
    }
    return components;
}

#ifndef _WIN32

const char* const kStateDirectories[] = {"GAMES", "DATA"};

// Uppercase, forward slashes, no drive or leading ".". Absolute paths, which
// name something outside the installation, come back empty.
std::string Retail(const char* retailPath) {
    std::string path = retailPath != nullptr ? retailPath : "";
    if (path.size() > 2 && path[1] == ':') {
        path.erase(0, 2);
    }
    std::replace(path.begin(), path.end(), '\\', '/');
    if (path.empty() || path.front() == '/') {
        return std::string();
    }

    std::string normalized;
    for (std::string component : SplitPath(path)) {
        for (char& character : component) {
            character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
        }
        if (!normalized.empty()) {
            normalized.push_back('/');
        }
        normalized += component;
    }
    return normalized;
}

bool Under(const std::string& path, const std::string& directory) {
    return path.starts_with(directory)
        && (path.size() == directory.size() || path[directory.size()] == '/');
}

#endif

}

i32 FileOpen(const char* retailPath, FileMode mode) { return Files().Open(retailPath, mode); }

i32 FileOpenLocale(const char* retailPath) { return Files().OpenLocale(retailPath); }

void FileClose(i32 file) { Files().Close(file); }

bool ReadExact(IFileSystem& files, i32 file, void* buffer, i32 count) {
    if (count < 0 || (count > 0 && buffer == nullptr)) {
        return false;
    }
    auto* cursor = static_cast<unsigned char*>(buffer);
    i32 remaining = count;
    while (remaining > 0) {
        const i32 transferred = files.Read(file, cursor, remaining);
        if (transferred <= 0 || transferred > remaining) {
            return false;
        }
        cursor += transferred;
        remaining -= transferred;
    }
    return true;
}

bool WriteExact(IFileSystem& files, i32 file, const void* buffer, i32 count) {
    if (count < 0 || (count > 0 && buffer == nullptr)) {
        return false;
    }
    const auto* cursor = static_cast<const unsigned char*>(buffer);
    i32 remaining = count;
    while (remaining > 0) {
        const i32 transferred = files.Write(file, cursor, remaining);
        if (transferred <= 0 || transferred > remaining) {
            return false;
        }
        cursor += transferred;
        remaining -= transferred;
    }
    return true;
}

bool FileReadExact(i32 file, void* buffer, i32 count) {
    return ReadExact(Files(), file, buffer, count);
}

bool FileWriteExact(i32 file, const void* buffer, i32 count) {
    return WriteExact(Files(), file, buffer, count);
}

i32 FileSeek(i32 file, i32 offset) { return Files().Seek(file, offset); }

i32 FileTell(i32 file) { return Files().Tell(file); }

i32 FileLength(i32 file) { return Files().Length(file); }

bool FileExists(const char* retailPath) { return Files().Exists(retailPath); }

void FileResolve(const char* retailPath, FileMode mode, char* buffer, i32 size) {
    if (buffer == nullptr || size <= 0) {
        return;
    }

    const std::string path = Files().Resolve(retailPath, mode);
    std::strncpy(buffer, path.c_str(), static_cast<std::size_t>(size) - 1);
    buffer[size - 1] = '\0';
}

bool IsUserState(const char* retailPath) {
#ifdef _WIN32
    // The retail layout, where state stays with the installation.
    static_cast<void>(retailPath);
    return false;
#else
    const std::string path = Retail(retailPath);
    if (Under(path, "GAMES")) {
        return true;
    }

    const std::size_t slash = path.find('/');
    if (slash == std::string::npos) {
        // Preferences, screenshots, and the debug log are things the game
        // produces, not things the installation came with.
        return path == "HEROES2.CFG" || path == "HEROES2.DISPLAY" || path.ends_with(".PCX") || path.ends_with(".LOG");
    }

    if (!Under(path, "DATA")) {
        return false;
    }

    const std::string name = path.substr(slash + 1);
    return name.ends_with(".HS") || (name.starts_with("RMT") && name.ends_with(".BIN"));
#endif
}

bool PrepareUserState(const std::string& userRoot) {
#ifdef _WIN32
    static_cast<void>(userRoot);
    return true;
#else
    for (const char* directory : kStateDirectories) {
        std::error_code error;
        std::filesystem::create_directories(userRoot + "/" + directory, error);
        if (error) {
            return false;
        }
    }
    return true;
#endif
}

std::string ResolveIn(const std::string& root, const char* retailPath) {
    if (retailPath == nullptr || *retailPath == '\0') {
        return std::string();
    }

    std::string relative = retailPath;
    if (relative.size() > 2 && relative[1] == ':') {
        relative.erase(0, 2);
    }

    // Retail wrote paths from the installation, so a leading separator means
    // the top of the game directory rather than the top of the host.
    std::string resolved = root;
    for (const std::string& component : SplitPath(relative)) {
        const bool joined = resolved.empty() || IsSeparator(resolved.back());
        const std::string separator = joined ? "" : std::string(1, kSeparator);
        const std::string candidate = resolved + separator + component;
        if (Present(candidate)) {
            resolved = candidate;
            continue;
        }
        const CaseInsensitiveMatch match = MatchIgnoringCase(resolved, component);
        if (match.ambiguous) {
            return std::string();
        }
        resolved += separator + (match.name.empty() ? component : match.name);
    }
    return resolved;
}

std::filesystem::path HostPath(const std::string& utf8) {
    return std::filesystem::path(std::u8string(utf8.begin(), utf8.end()));
}

std::string HostString(const std::filesystem::path& path) {
    const std::u8string text = path.u8string();
    return std::string(text.begin(), text.end());
}

std::string ConfiguredDirectory(const char* value) {
    std::string text = value != nullptr ? value : "";
    const auto trim = [&text] {
        const auto blank = [](char character) {
            return std::isspace(static_cast<unsigned char>(character)) != 0;
        };
        while (!text.empty() && blank(text.back())) {
            text.pop_back();
        }
        const auto first = std::find_if_not(text.begin(), text.end(), blank);
        text.erase(text.begin(), first);
    };
    trim();
    if (text.size() >= 2 && text.front() == '"' && text.back() == '"') {
        text = text.substr(1, text.size() - 2);
        trim();
    }

    // Keep a root: "/" here, "C:\" on Windows.
    std::size_t keep = 1;
#ifdef _WIN32
    if (text.size() >= 3 && text[1] == ':') {
        keep = 3;
    }
#endif
    while (text.size() > keep && IsSeparator(text.back())) {
        text.pop_back();
    }
    return text;
}

bool HoldsGameData(const std::string& directory) {
    return !directory.empty() && Present(ResolveIn(directory, "DATA\\HEROES2.AGG"));
}

std::string FindGameData(const std::vector<std::string>& candidates) {
    for (const std::string& candidate : candidates) {
        if (HoldsGameData(candidate)) {
            return candidate;
        }
    }
    return std::string();
}

}
