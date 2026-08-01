#ifndef HOMM2_BASE_DROPLIST_TYPES_H
#define HOMM2_BASE_DROPLIST_TYPES_H

#include <va.h>

#define DROPLIST_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp"

union SDropListSourceFile {
    char text[sizeof(DROPLIST_SOURCE_FILE)];
    u32l alignmentValue;
};

struct SDropListSourceFiles {
    SDropListSourceFile itemDestruction;
    SDropListSourceFile listDestruction;
    SDropListSourceFile finalItemDestruction;
    SDropListSourceFile finalListDestruction;
    SDropListSourceFile resizedListAllocation;
    SDropListSourceFile oldListDestruction;
    SDropListSourceFile replacedItemDestruction;
    SDropListSourceFile replacementItemAllocation;
    SDropListSourceFile appendedListAllocation;
    SDropListSourceFile appendedTextAllocation;
    SDropListSourceFile appendedOldListDestruction;
};

#endif
