#ifndef HOMM2_BASE_DROPLIST_TYPES_H
#define HOMM2_BASE_DROPLIST_TYPES_H

#include <va.h>

#define DROPLIST_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp"

union SDropListSourceFile {
    char text[sizeof(DROPLIST_SOURCE_FILE)];
    unsigned long alignmentValue;
};
SIZE(SDropListSourceFile, 0x2c);

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
SIZE(SDropListSourceFiles, 0x1e4);

#endif // HOMM2_BASE_DROPLIST_TYPES_H
