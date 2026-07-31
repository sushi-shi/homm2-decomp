#include <windows.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <dirent.h>
#include <strings.h>
#include <sys/stat.h>

#include <PLATFORM/Platform.h>

#include "State.h"

namespace platform::win32 {
namespace {

std::set<void*> gGdiObjects;

std::map<std::string, std::string> gCache;

bool Exists(const std::string& path) {
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

std::string Walk(const std::string& base, const std::vector<std::string>& components) {
    std::string resolved = base;
    for (const std::string& component : components) {
        if (component == ".") {
            continue;
        }
        const std::string separator = (resolved.empty() || resolved.back() == '/') ? "" : "/";
        std::string candidate = resolved + separator + component;
        if (Exists(candidate)) {
            resolved = candidate;
            continue;
        }
        const std::string matched = MatchIgnoringCase(resolved, component);
        resolved += separator + (matched.empty() ? component : matched);
    }
    return resolved;
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

}

void TrackGdiObject(void* object) { gGdiObjects.insert(object); }

bool ForgetGdiObject(void* object) { return gGdiObjects.erase(object) != 0; }

std::string ResolvePath(const char* retailPath, PathUse use) {
    if (retailPath == nullptr || *retailPath == '\0') {
        return std::string();
    }

    const std::string original(retailPath);
    if (const auto cached = gCache.find(original); cached != gCache.end()) {
        return cached->second;
    }

    std::string relative = original;
    if (relative.size() > 2 && relative[1] == ':') {
        relative.erase(0, 2);
    }

    const bool absolute = !relative.empty() && (relative[0] == '\\' || relative[0] == '/');
    const std::vector<std::string> components = SplitPath(relative);

    // Writes go to the user directory, reads fall back to the installation so
    // that shipped saves and score tables stay in reach. Not cached: the
    // answer changes as files appear.
    if (!absolute && platform::IsUserState(original.c_str())) {
        const std::string user = Walk(platform::Files().UserRoot(), components);
        if (use == PathUse::Write || Exists(user)) {
            return user;
        }
        const std::string data = Walk(platform::Files().DataRoot(), components);
        return Exists(data) ? data : user;
    }

    const std::string base = absolute ? std::string("/") : platform::Files().DataRoot();
    const std::string resolved = Walk(base, components);

    gCache.emplace(original, resolved);
    return resolved;
}

}
