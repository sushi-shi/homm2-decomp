#include <io.h>

#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

#include <fcntl.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>

#include "State.h"

#undef open
#undef access
#undef tell
#undef filelength

int _open(const char* path, int flags, ...) {
    if (path == nullptr) {
        return -1;
    }

    int mode = 0644;
    if ((flags & O_CREAT) != 0) {
        va_list arguments;
        va_start(arguments, flags);
        mode = va_arg(arguments, int);
        va_end(arguments);
    }

    const std::string resolved = platform::win32::ResolvePath(path);
    return ::open(resolved.c_str(), flags, mode);
}

int _close(int file) { return ::close(file); }

int _read(int file, void* buffer, unsigned count) {
    return static_cast<int>(::read(file, buffer, count));
}

int _write(int file, const void* buffer, unsigned count) {
    return static_cast<int>(::write(file, buffer, count));
}

long _lseek(int file, long offset, int origin) { return ::lseek(file, offset, origin); }
long _tell(int file) { return ::lseek(file, 0, SEEK_CUR); }

long _filelength(int file) {
    const off_t current = ::lseek(file, 0, SEEK_CUR);
    if (current < 0) {
        return -1;
    }
    const off_t end = ::lseek(file, 0, SEEK_END);
    ::lseek(file, current, SEEK_SET);
    return static_cast<long>(end);
}

int _eof(int file) {
    const off_t current = ::lseek(file, 0, SEEK_CUR);
    const off_t end = ::lseek(file, 0, SEEK_END);
    ::lseek(file, current, SEEK_SET);
    return current >= end ? 1 : 0;
}

int _chdir(const char* path) { return ::chdir(platform::win32::ResolvePath(path).c_str()); }
int _mkdir(const char* path) { return ::mkdir(platform::win32::ResolvePath(path).c_str(), 0755); }
int _rmdir(const char* path) { return ::rmdir(platform::win32::ResolvePath(path).c_str()); }
int _unlink(const char* path) { return ::unlink(platform::win32::ResolvePath(path).c_str()); }
char* _getcwd(char* buffer, int length) { return ::getcwd(buffer, static_cast<size_t>(length)); }

int _chdrive(int) { return 0; }
int _getdrive() { return 3; }

int _access(const char* path, int mode) {
    return ::access(platform::win32::ResolvePath(path).c_str(), mode);
}

char* _itoa(int value, char* buffer, int radix) {
    if (buffer == nullptr) {
        return nullptr;
    }
    if (radix == 10) {
        std::sprintf(buffer, "%d", value);
    } else if (radix == 16) {
        std::sprintf(buffer, "%x", value);
    } else if (radix == 8) {
        std::sprintf(buffer, "%o", value);
    } else {
        buffer[0] = '\0';
    }
    return buffer;
}

extern "C" {

char* _strrev(char* text) {
    if (text == nullptr) {
        return nullptr;
    }
    const size_t length = std::strlen(text);
    for (size_t i = 0; i + 1 < length - i; ++i) {
        const char swapped = text[i];
        text[i] = text[length - 1 - i];
        text[length - 1 - i] = swapped;
    }
    return text;
}

char* _strupr(char* text) {
    if (text != nullptr) {
        for (char* cursor = text; *cursor != '\0'; ++cursor) {
            *cursor = static_cast<char>(std::toupper(static_cast<unsigned char>(*cursor)));
        }
    }
    return text;
}

char* _strlwr(char* text) {
    if (text != nullptr) {
        for (char* cursor = text; *cursor != '\0'; ++cursor) {
            *cursor = static_cast<char>(std::tolower(static_cast<unsigned char>(*cursor)));
        }
    }
    return text;
}

char* strrev(char* text) { return _strrev(text); }
char* strupr(char* text) { return _strupr(text); }
char* strlwr(char* text) { return _strlwr(text); }

int stricmp(const char* left, const char* right) { return ::strcasecmp(left, right); }
int strcmpi(const char* left, const char* right) { return ::strcasecmp(left, right); }
int _stricmp(const char* left, const char* right) { return ::strcasecmp(left, right); }
int _strcmpi(const char* left, const char* right) { return ::strcasecmp(left, right); }

int strnicmp(const char* left, const char* right, unsigned count) {
    return ::strncasecmp(left, right, count);
}

int _strnicmp(const char* left, const char* right, unsigned count) {
    return ::strncasecmp(left, right, count);
}

}
