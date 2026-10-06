#ifndef HOMM2_PLATFORM_FILETRANSACTION_H
#define HOMM2_PLATFORM_FILETRANSACTION_H

#include <PLATFORM/FileSystem.h>

namespace platform {

// Build a complete replacement without truncating the current destination.
// Destruction aborts an uncommitted transaction; process termination can leave
// an unused temporary sibling, but never publishes its partial contents.
class FileTransaction final {
public:
    FileTransaction(IFileSystem& files, const char* retailPath);
    ~FileTransaction();
    FileTransaction(const FileTransaction&) = delete;
    FileTransaction& operator=(const FileTransaction&) = delete;

    i32 Handle() const { return m_file; }
    [[nodiscard]] bool Write(const void* buffer, i32 count);
    [[nodiscard]] bool Commit();

private:
    void Abort();
    IFileSystem& m_files;
    std::string m_destination;
    std::string m_temporary;
    i32 m_file = -1;
    bool m_failed = false;
};

}

#endif
