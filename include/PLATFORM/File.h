#ifndef HOMM2_PLATFORM_FILE_H
#define HOMM2_PLATFORM_FILE_H

#include <Ints.h>

namespace platform {

enum class FileMode {
    Read,
    Write,
    Append,
};

// The game reads and writes its files through these. An open gives back -1
// when the file is not there.
i32 FileOpen(const char* retailPath, FileMode mode);
i32 FileOpenLocale(const char* retailPath);
void FileClose(i32 file);
[[nodiscard]] bool FileReadExact(i32 file, void* buffer, i32 count);
[[nodiscard]] bool FileWriteExact(i32 file, const void* buffer, i32 count);
i32 FileSeek(i32 file, i32 offset);
i32 FileTell(i32 file);
i32 FileLength(i32 file);
bool FileExists(const char* retailPath);

// For the few places that still hand a path to the C runtime themselves.
// Writes the real path for retailPath into buffer.
void FileResolve(const char* retailPath, FileMode mode, char* buffer, i32 size);

}

#endif
