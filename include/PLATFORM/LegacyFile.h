#ifndef HOMM2_PLATFORM_LEGACYFILE_H
#define HOMM2_PLATFORM_LEGACYFILE_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef _WIN32

#include <direct.h>
#include <io.h>

#define open(...) _open(__VA_ARGS__)
#define close _close
#define read _read
#define write _write
#define lseek _lseek
#define access(path, mode) _access(path, mode)
#define tell(file) _tell(file)
#define filelength(file) _filelength(file)

#else

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define _O_RDONLY O_RDONLY
#define _O_WRONLY O_WRONLY
#define _O_RDWR O_RDWR
#define _O_CREAT O_CREAT
#define _O_TRUNC O_TRUNC
#define _O_APPEND O_APPEND
#define _O_BINARY O_BINARY
#define _O_TEXT 0
#define O_TEXT 0

int _open(const char* path, int flags, ...);

#define open(...) _open(__VA_ARGS__)
int _close(int file);
int _read(int file, void* buffer, unsigned count);
int _write(int file, const void* buffer, unsigned count);
long _lseek(int file, long offset, int origin);
long _tell(int file);
long _filelength(int file);
int _eof(int file);
int _access(const char* path, int mode);

int _chdir(const char* path);
int _mkdir(const char* path);
char* _getcwd(char* buffer, int length);
int _unlink(const char* path);
int _rmdir(const char* path);
int _chdrive(int drive);
int _getdrive();

char* _itoa(int value, char* buffer, int radix);

#define access(path, mode) _access(path, mode)
#define tell(file) _tell(file)
#define filelength(file) _filelength(file)

#define _S_IREAD S_IRUSR
#define _S_IWRITE (S_IRUSR | S_IWUSR)
#define _S_IEXEC S_IXUSR

#endif

#endif
