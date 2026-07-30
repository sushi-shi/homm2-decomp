#ifndef HOMM2_PLATFORM_WIN32_DIRECT_H
#define HOMM2_PLATFORM_WIN32_DIRECT_H

#include "io.h"

int _rmdir(const char* path);
int _chdrive(int drive);
int _getdrive();

#endif
