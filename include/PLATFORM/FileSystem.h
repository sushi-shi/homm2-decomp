#ifndef HOMM2_PLATFORM_FILESYSTEM_H
#define HOMM2_PLATFORM_FILESYSTEM_H

#include <string>

namespace platform {

class IFileSystem {
public:
    virtual ~IFileSystem() = default;

    virtual std::string DataRoot() const = 0;

    virtual std::string UserRoot() const = 0;

    virtual std::string Resolve(const char* retailPath) const = 0;
};

}

#endif
