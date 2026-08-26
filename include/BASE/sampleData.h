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
    char* data;
    i32 size;
    SamplePlaybackRate sampleRate;
    i32 volume;
    SampleAudioFormat sampleFormat;
    b32 stereo;
    b32 loopCount;
    struct _SAMPLE* activeSample;
    i32 channelType;
};
#pragma pack(pop)

#endif
