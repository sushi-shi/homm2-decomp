#ifndef HOMM2_BASE_SAMPLEDATA_H
#define HOMM2_BASE_SAMPLEDATA_H

#include <va.h>

#pragma pack(push, 1)
struct SamplePlaybackData {
    struct _SAMPLE *activeSample;
    char *data;
    i32 size;
    i32 channelType;
    i32 sampleRate;
    i32 format;
    i32 volume;
    i32 loopCount;
};
#pragma pack(pop)
SIZE(SamplePlaybackData, 0x20);

#endif // HOMM2_BASE_SAMPLEDATA_H
