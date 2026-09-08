#ifndef HOMM2_PLATFORM_FILESYSTEM_H
#define HOMM2_PLATFORM_FILESYSTEM_H

#include <string>
#include <vector>

#include <PLATFORM/File.h>

namespace platform {

class IFileSystem {
public:
    virtual ~IFileSystem() = default;

    virtual std::string DataRoot() const = 0;

    virtual std::string UserRoot() const = 0;

    // Directory containing the executable (or the virtual filesystem root on
    // WASM). Port-owned data such as translation catalogs lives relative to
    // this path, independently of the user-supplied retail data root.
    virtual std::string ProgramRoot() const = 0;

    // Optional user-supplied installation containing localized retail
    // archives. An empty value means that no asset overlay was configured.
    virtual std::string LocaleDataRoot() const = 0;

    virtual std::string Resolve(const char* retailPath, FileMode mode) const = 0;

    virtual std::vector<std::string> List(const char* pattern) const = 0;

    virtual bool Exists(const char* retailPath) const = 0;

    virtual i32 Open(const char* retailPath, FileMode mode) = 0;

    virtual i32 OpenLocale(const char* retailPath) = 0;

    virtual void Close(i32 file) = 0;

    // The handle is consumed even if closing reports a delayed write error.
    // A backend without checked-close support must not report a successful save.
    virtual bool CloseChecked(i32 file) { Close(file); return false; }

    // Transaction primitives use resolved host paths, preserving the exact
    // destination chosen at transaction creation. Temporary creation must be
    // exclusive. Replacement must not delete the destination before renaming.
    // Backends that do not implement transactions fail before creating a file.
    virtual i32 CreateTemporarySibling(const std::string&, std::string&) { return -1; }
    virtual bool Flush(i32) { return false; }
    virtual bool ReplaceFile(const std::string&, const std::string&) { return false; }
    virtual bool RemoveTemporary(const std::string&) { return false; }

    virtual i32 Read(i32 file, void* buffer, i32 count) = 0;

    virtual i32 Write(i32 file, const void* buffer, i32 count) = 0;

    virtual i32 Seek(i32 file, i32 offset) = 0;

    virtual i32 Tell(i32 file) = 0;

    virtual i32 Length(i32 file) = 0;
};

// Complete a fixed-size record even when a host backend services an operation
// in smaller chunks. EOF, a zero-progress operation, and backend errors fail
// without spinning.
[[nodiscard]] bool ReadExact(IFileSystem& files, i32 file, void* buffer, i32 count);
[[nodiscard]] bool WriteExact(IFileSystem& files, i32 file, const void* buffer, i32 count);

// Saved games, high scores and the network exchange files. Retail kept them
// with the installation, as Windows still does; elsewhere they are the user's.
bool IsUserState(const char* retailPath);

// Creates the user directories that state resolves into. Failure is reported
// so startup can make a missing writable state root visible to the user.
[[nodiscard]] bool PrepareUserState(const std::string& userRoot);

// Names a retail path inside a real directory, taking the spelling that is
// already there over the one asked for. Ambiguous case-folded names fail with
// an empty result instead of depending on host directory enumeration order.
std::string ResolveIn(const std::string& root, const char* retailPath);

}

#endif
