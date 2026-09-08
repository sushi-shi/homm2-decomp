#include <PLATFORM/FileTransaction.h>

namespace platform {

FileTransaction::FileTransaction(IFileSystem& files, const char* retailPath)
    : m_files(files), m_destination(files.Resolve(retailPath, FileMode::Write)) {
    if (!m_destination.empty())
        m_file = m_files.CreateTemporarySibling(m_destination, m_temporary);
}

FileTransaction::~FileTransaction() { Abort(); }

bool FileTransaction::Write(const void* buffer, i32 count) {
    if (m_failed || m_file < 0) return false;
    if (!WriteExact(m_files, m_file, buffer, count)) m_failed = true;
    return !m_failed;
}

void FileTransaction::Abort() {
    if (m_file >= 0) {
        m_files.Close(m_file);
        m_file = -1;
    }
    if (!m_temporary.empty()) {
        m_files.RemoveTemporary(m_temporary);
        m_temporary.clear();
    }
}

bool FileTransaction::Commit() {
    if (m_file < 0 || m_failed) {
        Abort();
        return false;
    }
    const bool flushed = m_files.Flush(m_file);
    const bool closed = m_files.CloseChecked(m_file);
    m_file = -1;
    if (!flushed || !closed || !m_files.ReplaceFile(m_temporary, m_destination)) {
        Abort();
        return false;
    }
    m_temporary.clear();
    return true;
}

}
