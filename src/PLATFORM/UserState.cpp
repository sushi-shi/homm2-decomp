#include <PLATFORM/FileSystem.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>
#include <system_error>

namespace platform {
namespace {

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

#endif

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
    if (!Under(path, "DATA") || slash == std::string::npos) {
        return false;
    }

    const std::string name = path.substr(slash + 1);
    return name.ends_with(".HS") || (name.starts_with("RMT") && name.ends_with(".BIN"));
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

}
