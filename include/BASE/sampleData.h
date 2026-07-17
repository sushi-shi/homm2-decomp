#ifndef HOMM2_BASE_SAMPLEDATA_H
#define HOMM2_BASE_SAMPLEDATA_H

#include <va.h>

#pragma pack(push, 1)
struct SamplePlaybackData {
    struct _SAMPLE *activeSample;
    char *data;
    int size;
    int channelType;
    int sampleRate;
    int format;
    int volume;
    int loopCount;
};
#pragma pack(pop)
SIZE(SamplePlaybackData, 0x20);

#endif // HOMM2_BASE_SAMPLEDATA_H
