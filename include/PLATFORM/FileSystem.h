#ifndef HOMM2_PLATFORM_FILESYSTEM_H
#define HOMM2_PLATFORM_FILESYSTEM_H

#include <string>
#include <vector>

namespace platform {

class IFileSystem {
public:
    virtual ~IFileSystem() = default;

    virtual std::string DataRoot() const = 0;

    virtual std::string UserRoot() const = 0;

    virtual std::string Resolve(const char* retailPath) const = 0;

    virtual std::vector<std::string> List(const char* pattern) const = 0;
};

// Saved games, high scores and the network exchange files. Retail kept them
// with the installation, as Windows still does; elsewhere they are the user's.
bool IsUserState(const char* retailPath);

// Creates the user directories that state resolves into.
void PrepareUserState(const std::string& userRoot);

}

#endif
