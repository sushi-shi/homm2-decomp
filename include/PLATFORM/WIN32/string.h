#ifndef HOMM2_PLATFORM_WIN32_STRING_H
#define HOMM2_PLATFORM_WIN32_STRING_H

#include_next <string.h>

#ifdef _WIN32

#define strrev _strrev
#define strupr _strupr
#define strlwr _strlwr
#define stricmp _stricmp
#define strcmpi _strcmpi
#define strnicmp _strnicmp

#else

#ifdef __cplusplus
extern "C" {
#endif

char* _strrev(char* text);
char* _strupr(char* text);
char* _strlwr(char* text);
char* strrev(char* text);
char* strupr(char* text);
char* strlwr(char* text);

int stricmp(const char* left, const char* right);
int strcmpi(const char* left, const char* right);
int strnicmp(const char* left, const char* right, unsigned count);
int _stricmp(const char* left, const char* right);
int _strcmpi(const char* left, const char* right);
int _strnicmp(const char* left, const char* right, unsigned count);

#ifdef __cplusplus
}
#endif

#endif

#endif
