#ifndef HOMM2_SOURCE_FINDPATH_TYPES_H
#define HOMM2_SOURCE_FINDPATH_TYPES_H

#include <va.h>

#define FINDPATH_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\FINDPATH.CPP"

struct SFindPathSourceLine {
    i16 value;
    i16 reserved;
};

struct SFindPathSourceLocation {
    SFindPathSourceLine line;
    char sourceFile[sizeof(FINDPATH_SOURCE_FILE)];
};

#endif
