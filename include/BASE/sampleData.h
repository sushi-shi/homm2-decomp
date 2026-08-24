#ifndef HOMM2_BASE_SAMPLEDATA_H
#define HOMM2_BASE_SAMPLEDATA_H

#include <Ints.h>

enum {
    FORMAT_MONO   = 0,
    FORMAT_8_BIT  = 0,
    FORMAT_16_BIT = 1,
    FORMAT_STEREO = 2
};
typedef i32 SampleAudioFormat;

enum {
    RATE_11025 = 11025,
    RATE_22050 = 22050,
    RATE_44100 = 44100
};
typedef i32 SamplePlaybackRate;
#pragma pack(push, 1)
struct SamplePlaybackData {
    struct _SAMPLE* activeSample;
    char* data;
    i32 size;
    i32 channelType;
    SamplePlaybackRate sampleRate;
    SampleAudioFormat format;
    i32 volume;
    i32 loopCount;
};
#pragma pack(pop)

#endif
