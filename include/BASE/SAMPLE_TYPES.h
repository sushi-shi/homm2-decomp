#ifndef HOMM2_BASE_SAMPLE_TYPES_H
#define HOMM2_BASE_SAMPLE_TYPES_H

#include <va.h>

#define SAMPLE_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\SAMPLE.CPP"

struct SSampleSourceFiles {
    char sampleAllocation[sizeof(SAMPLE_SOURCE_FILE)];
    char sampleDestruction[sizeof(SAMPLE_SOURCE_FILE)];
    char midiAllocation[sizeof(SAMPLE_SOURCE_FILE)];
    char midiDestruction[sizeof(SAMPLE_SOURCE_FILE)];
};
SIZE(SSampleSourceFiles, 0xa0);

#endif
