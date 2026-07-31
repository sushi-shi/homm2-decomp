#include <windows.h>

#include <cerrno>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <vector>

#include <cctype>
#include <dirent.h>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <PLATFORM/Platform.h>

#include "State.h"

namespace {

DWORD gLastError = 0;

enum class HandleKind { File, Event, Find };

struct HandleHeader {
    HandleKind kind;
};

struct Event : HandleHeader {
    std::mutex mutex;
    std::condition_variable changed;
    bool signalled = false;
    bool manualReset = false;
};

struct FileHandle : HandleHeader {
    int descriptor = -1;
};

}

HMODULE LoadLibraryA(LPCSTR name) {

    gLastError = ERROR_FILE_NOT_FOUND;
    return nullptr;
}

BOOL FreeLibrary(HMODULE) { return TRUE; }
FARPROC GetProcAddress(HMODULE, LPCSTR) { return nullptr; }

DWORD GetTickCount() { return platform::Host().Ticks(); }
void Sleep(DWORD milliseconds) { platform::Host().Sleep(milliseconds); }
DWORD GetLastError() { return gLastError; }

void OutputDebugStringA(LPCSTR text) {
    if (text != nullptr) {
        platform::Host().Log(platform::LogLevel::Debug, text);
    }
}

int lstrlenA(LPCSTR text) { return text != nullptr ? static_cast<int>(std::strlen(text)) : 0; }

HANDLE CreateFileA(LPCSTR name, DWORD access, DWORD, LPSECURITY_ATTRIBUTES, DWORD creation, DWORD,
                   HANDLE) {
    if (name == nullptr) {
        return INVALID_HANDLE_VALUE;
    }

    int flags = 0;
    if ((access & GENERIC_WRITE) != 0) {
        flags = (access & GENERIC_READ) != 0 ? O_RDWR : O_WRONLY;
    } else {
        flags = O_RDONLY;
    }
    if (creation == CREATE_ALWAYS) {
        flags |= O_CREAT | O_TRUNC;
    } else if (creation == CREATE_NEW) {
        flags |= O_CREAT | O_EXCL;
    } else if (creation == OPEN_ALWAYS) {
        flags |= O_CREAT;
    }

    const bool writing = (flags & (O_CREAT | O_WRONLY | O_RDWR)) != 0;
    const std::string resolved = platform::win32::ResolvePath(
        name, writing ? platform::win32::PathUse::Write : platform::win32::PathUse::Read);
    const int descriptor = ::open(resolved.c_str(), flags, 0644);
    if (descriptor < 0) {
        gLastError = ERROR_FILE_NOT_FOUND;
        return INVALID_HANDLE_VALUE;
    }

    FileHandle* handle = new FileHandle();
    handle->kind = HandleKind::File;
    handle->descriptor = descriptor;
    return handle;
}

BOOL ReadFile(HANDLE file, LPVOID buffer, DWORD toRead, LPDWORD read, LPOVERLAPPED) {
    FileHandle* handle = static_cast<FileHandle*>(file);
    if (handle == nullptr || file == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    const ssize_t got = ::read(handle->descriptor, buffer, toRead);
    if (read != nullptr) {
        *read = got > 0 ? static_cast<DWORD>(got) : 0;
    }
    return got >= 0 ? TRUE : FALSE;
}

BOOL WriteFile(HANDLE file, LPCVOID buffer, DWORD toWrite, LPDWORD written, LPOVERLAPPED) {
    FileHandle* handle = static_cast<FileHandle*>(file);
    if (handle == nullptr || file == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    const ssize_t put = ::write(handle->descriptor, buffer, toWrite);
    if (written != nullptr) {
        *written = put > 0 ? static_cast<DWORD>(put) : 0;
    }
    return put >= 0 ? TRUE : FALSE;
}

BOOL CloseHandle(HANDLE object) {
    if (object == nullptr || object == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    HandleHeader* header = static_cast<HandleHeader*>(object);
    switch (header->kind) {
    case HandleKind::File: {
        FileHandle* file = static_cast<FileHandle*>(header);
        if (file->descriptor >= 0) {
            ::close(file->descriptor);
        }
        delete file;
        return TRUE;
    }
    case HandleKind::Event:
        delete static_cast<Event*>(header);
        return TRUE;
    case HandleKind::Find:
        return FindClose(object);
    }
    return FALSE;
}

DWORD GetFileAttributesA(LPCSTR name) {
    if (name == nullptr) {
        return 0xFFFFFFFF;
    }
    struct stat info;
    const std::string resolved = platform::win32::ResolvePath(name);
    if (::stat(resolved.c_str(), &info) != 0) {
        gLastError = ERROR_FILE_NOT_FOUND;
        return 0xFFFFFFFF;
    }
    return S_ISDIR(info.st_mode) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
}

namespace {

struct FindHandle : HandleHeader {
    std::vector<std::string> names;
    std::size_t next = 0;
};

bool MatchesWildcard(const char* pattern, const char* name) {
    if (*pattern == '\0') {
        return *name == '\0';
    }
    if (*pattern == '*') {
        for (const char* at = name;; ++at) {
            if (MatchesWildcard(pattern + 1, at)) {
                return true;
            }
            if (*at == '\0') {
                return false;
            }
        }
    }
    if (*name == '\0') {
        return false;
    }
    if (*pattern != '?'
        && std::tolower(static_cast<unsigned char>(*pattern))
             != std::tolower(static_cast<unsigned char>(*name))) {
        return false;
    }
    return MatchesWildcard(pattern + 1, name + 1);
}

void FillFindData(LPWIN32_FIND_DATAA data, const std::string& name) {
    std::memset(data, 0, sizeof(*data));
    data->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    std::strncpy(data->cFileName, name.c_str(), sizeof(data->cFileName) - 1);
}

}

HANDLE FindFirstFileA(LPCSTR pattern, LPWIN32_FIND_DATAA data) {
    if (pattern == nullptr || data == nullptr) {
        return INVALID_HANDLE_VALUE;
    }

    std::string full(pattern);
    std::string wildcard = full;
    std::string directory;
    const std::size_t slash = full.find_last_of("\\/");
    if (slash != std::string::npos) {
        directory = full.substr(0, slash);
        wildcard = full.substr(slash + 1);
    }

    const std::string resolved =
        directory.empty() ? platform::Files().DataRoot() : platform::win32::ResolvePath(directory.c_str());

    DIR* dir = ::opendir(resolved.c_str());
    if (dir == nullptr) {
        gLastError = ERROR_PATH_NOT_FOUND;
        return INVALID_HANDLE_VALUE;
    }

    FindHandle* handle = new FindHandle();
    handle->kind = HandleKind::Find;
    while (dirent* entry = ::readdir(dir)) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        if (MatchesWildcard(wildcard.c_str(), entry->d_name)) {
            handle->names.emplace_back(entry->d_name);
        }
    }
    ::closedir(dir);

    if (handle->names.empty()) {
        delete handle;
        gLastError = ERROR_FILE_NOT_FOUND;
        return INVALID_HANDLE_VALUE;
    }

    FillFindData(data, handle->names[handle->next++]);
    return handle;
}

BOOL FindNextFileA(HANDLE find, LPWIN32_FIND_DATAA data) {
    FindHandle* handle = static_cast<FindHandle*>(find);
    if (handle == nullptr || find == INVALID_HANDLE_VALUE || data == nullptr
        || handle->next >= handle->names.size()) {
        gLastError = ERROR_NO_MORE_FILES;
        return FALSE;
    }
    FillFindData(data, handle->names[handle->next++]);
    return TRUE;
}

BOOL FindClose(HANDLE find) {
    if (find == nullptr || find == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    delete static_cast<FindHandle*>(find);
    return TRUE;
}

void InitializeCriticalSection(LPCRITICAL_SECTION section) {
    if (section != nullptr) {
        section->opaque = new std::recursive_mutex();
    }
}

void DeleteCriticalSection(LPCRITICAL_SECTION section) {
    if (section != nullptr && section->opaque != nullptr) {
        delete static_cast<std::recursive_mutex*>(section->opaque);
        section->opaque = nullptr;
    }
}

void EnterCriticalSection(LPCRITICAL_SECTION section) {
    if (section != nullptr && section->opaque != nullptr) {
        static_cast<std::recursive_mutex*>(section->opaque)->lock();
    }
}

void LeaveCriticalSection(LPCRITICAL_SECTION section) {
    if (section != nullptr && section->opaque != nullptr) {
        static_cast<std::recursive_mutex*>(section->opaque)->unlock();
    }
}

HANDLE CreateEventA(LPSECURITY_ATTRIBUTES, BOOL manualReset, BOOL initialState, LPCSTR) {
    Event* event = new Event();
    event->kind = HandleKind::Event;
    event->manualReset = manualReset != FALSE;
    event->signalled = initialState != FALSE;
    return event;
}

BOOL SetEvent(HANDLE handle) {
    Event* event = static_cast<Event*>(handle);
    if (event == nullptr) {
        return FALSE;
    }
    {
        std::lock_guard<std::mutex> guard(event->mutex);
        event->signalled = true;
    }
    event->changed.notify_all();
    return TRUE;
}

BOOL ResetEvent(HANDLE handle) {
    Event* event = static_cast<Event*>(handle);
    if (event == nullptr) {
        return FALSE;
    }
    std::lock_guard<std::mutex> guard(event->mutex);
    event->signalled = false;
    return TRUE;
}

DWORD WaitForSingleObject(HANDLE handle, DWORD milliseconds) {
    Event* event = static_cast<Event*>(handle);
    if (event == nullptr) {
        return WAIT_FAILED;
    }

    const std::uint32_t deadline = platform::Host().Ticks() + milliseconds;
    for (;;) {
        {
            std::lock_guard<std::mutex> guard(event->mutex);
            if (event->signalled) {
                if (!event->manualReset) {
                    event->signalled = false;
                }
                return WAIT_OBJECT_0;
            }
        }
        if (milliseconds != INFINITE && platform::Host().Ticks() >= deadline) {
            return WAIT_TIMEOUT;
        }
        platform::Host().Sleep(1);
    }
}

DWORD WaitForMultipleObjects(DWORD count, const HANDLE* objects, BOOL waitAll, DWORD milliseconds) {
    if (objects == nullptr || count == 0) {
        return WAIT_FAILED;
    }
    const std::uint32_t deadline = platform::Host().Ticks() + milliseconds;
    for (;;) {
        for (DWORD i = 0; i < count; ++i) {
            Event* event = static_cast<Event*>(objects[i]);
            if (event == nullptr) {
                continue;
            }
            std::lock_guard<std::mutex> guard(event->mutex);
            if (event->signalled) {
                if (!event->manualReset) {
                    event->signalled = false;
                }
                return WAIT_OBJECT_0 + i;
            }
        }
        if (milliseconds != INFINITE && platform::Host().Ticks() >= deadline) {
            return WAIT_TIMEOUT;
        }
        platform::Host().Sleep(1);
    }
}

BOOL GetCommState(HANDLE, LPDCB) { return FALSE; }
BOOL SetCommState(HANDLE, LPDCB) { return FALSE; }
BOOL GetCommTimeouts(HANDLE, LPCOMMTIMEOUTS) { return FALSE; }
BOOL SetCommTimeouts(HANDLE, LPCOMMTIMEOUTS) { return FALSE; }
BOOL SetupComm(HANDLE, DWORD, DWORD) { return FALSE; }
BOOL SetCommBreak(HANDLE) { return FALSE; }
BOOL ClearCommBreak(HANDLE) { return FALSE; }
BOOL ClearCommError(HANDLE, LPDWORD, LPCOMSTAT) { return FALSE; }
BOOL GetCommModemStatus(HANDLE, LPDWORD) { return FALSE; }
