#ifndef HOMM2_PLATFORM_FILE_H
#define HOMM2_PLATFORM_FILE_H

#include <Ints.h>

namespace platform {

enum class FileMode {
    Read,
    Write,
};

// The game reads and writes its files through these. An open gives back -1
// when the file is not there.
i32 FileOpen(const char* retailPath, FileMode mode);
void FileClose(i32 file);
i32 FileRead(i32 file, void* buffer, i32 count);
i32 FileWrite(i32 file, const void* buffer, i32 count);
i32 FileSeek(i32 file, i32 offset);
i32 FileTell(i32 file);
bool FileExists(const char* retailPath);

}

#endif
