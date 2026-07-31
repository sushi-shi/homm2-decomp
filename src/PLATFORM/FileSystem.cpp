#include <PLATFORM/FileSystem.h>

#include <PLATFORM/Platform.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>
#include <vector>

#include <dirent.h>
#include <strings.h>
#include <sys/stat.h>

namespace platform {
namespace {

bool Present(const std::string& path) {
    struct stat info;
    return ::stat(path.c_str(), &info) == 0;
}

std::string MatchIgnoringCase(const std::string& directory, const std::string& wanted) {
    DIR* handle = ::opendir(directory.empty() ? "." : directory.c_str());
    if (handle == nullptr) {
        return std::string();
    }

    std::string found;
    while (dirent* entry = ::readdir(handle)) {
        if (::strcasecmp(entry->d_name, wanted.c_str()) == 0) {
            found = entry->d_name;
            break;
        }
    }
    ::closedir(handle);
    return found;
}

std::vector<std::string> SplitPath(const std::string& path) {
    std::vector<std::string> components;
    std::string current;
    for (const char character : path) {
        if (character == '\\' || character == '/') {
            if (!current.empty()) {
                components.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(character);
        }
    }
    if (!current.empty()) {
        components.push_back(current);
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
    while (path.starts_with("./")) {
        path.erase(0, 2);
    }
    if (path.empty() || path.front() == '/') {
        return std::string();
    }

    for (char& character : path) {
        character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
    }
    return path;
}

bool Under(const std::string& path, const std::string& directory) {
    return path.starts_with(directory)
        && (path.size() == directory.size() || path[directory.size()] == '/');
}

// The save compressor works through scratch files named H2C plus four random
// letters, and the same again with .nw while compressed. H2CAMP.TXT ships with
// the game and is none of ours.
bool IsScratch(const std::string& name) {
    if (!name.starts_with("H2C")) {
        return false;
    }
    return name.size() == sizeof("H2CABCD") - 1
        || (name.size() == sizeof("H2CABCD.NW") - 1 && name.ends_with(".NW"));
}

#endif

}

i32 FileOpen(const char* retailPath, FileMode mode) { return Files().Open(retailPath, mode); }

void FileClose(i32 file) { Files().Close(file); }

i32 FileRead(i32 file, void* buffer, i32 count) { return Files().Read(file, buffer, count); }

i32 FileWrite(i32 file, const void* buffer, i32 count) {
    return Files().Write(file, buffer, count);
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
        // Screenshots and the debug log are things the game produces, not
        // things the installation came with.
        return path.ends_with(".PCX") || path.ends_with(".LOG");
    }

    if (!Under(path, "DATA")) {
        return false;
    }

    const std::string name = path.substr(slash + 1);
    return name.ends_with(".HS") || IsScratch(name)
        || (name.starts_with("RMT") && name.ends_with(".BIN"));
#endif
}

void PrepareUserState(const std::string& userRoot) {
#ifdef _WIN32
    static_cast<void>(userRoot);
#else
    std::error_code error;
    for (const char* directory : kStateDirectories) {
        std::filesystem::create_directories(userRoot + "/" + directory, error);
    }
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

    const bool absolute = !relative.empty() && (relative[0] == '\\' || relative[0] == '/');
    std::string resolved = absolute ? std::string("/") : root;
    for (const std::string& component : SplitPath(relative)) {
        if (component == ".") {
            continue;
        }
        const std::string separator = (resolved.empty() || resolved.back() == '/') ? "" : "/";
        const std::string candidate = resolved + separator + component;
        if (Present(candidate)) {
            resolved = candidate;
            continue;
        }
        const std::string matched = MatchIgnoringCase(resolved, component);
        resolved += separator + (matched.empty() ? component : matched);
    }
    return resolved;
}

}
