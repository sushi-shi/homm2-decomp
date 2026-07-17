#ifndef HOMM2_SOURCE_FINDPATH_TYPES_H
#define HOMM2_SOURCE_FINDPATH_TYPES_H

#include <va.h>

#define FINDPATH_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\FINDPATH.CPP"

#define FINDPATH_ALLOCATION_SOURCE_LINE_BASE 20
#define FINDPATH_DESTRUCTION_SOURCE_LINE_BASE 26

struct SFindPathSourceLine {
    i16 value;
    i16 reserved;
};
SIZE(SFindPathSourceLine, 0x4);

struct SFindPathSourceLocation {
    SFindPathSourceLine line;
    char sourceFile[sizeof(FINDPATH_SOURCE_FILE)];
};
SIZE(SFindPathSourceLocation, 0x30);

#endif // HOMM2_SOURCE_FINDPATH_TYPES_H
