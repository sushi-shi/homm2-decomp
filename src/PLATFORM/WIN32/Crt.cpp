#include <string.h>

#include <cctype>
#include <cstring>

#include <strings.h>

#ifndef _WIN32

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

#endif
