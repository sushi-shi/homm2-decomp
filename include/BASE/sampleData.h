#ifndef HOMM2_BASE_SAMPLEDATA_H
#define HOMM2_BASE_SAMPLEDATA_H

#include <va.h>

H2_ENUM_CLASS_BEGIN(SampleAudioFormat)
    FORMAT_MONO   = 0,
    FORMAT_8_BIT  = 0,
    FORMAT_16_BIT = 1,
    FORMAT_STEREO = 2
H2_ENUM_CLASS_END(SampleAudioFormat)
H2_ENUM_FLAGS(SampleAudioFormat)

H2_ENUM_CLASS_BEGIN(SamplePlaybackRate)
    RATE_11025 = 11025,
    RATE_22050 = 22050,
    RATE_44100 = 44100
H2_ENUM_CLASS_END(SamplePlaybackRate)

#pragma pack(push, 1)
struct SamplePlaybackData {
    char* data;
    i32 size;
    SamplePlaybackRate sampleRate;
    i32 volume;
    SampleAudioFormat sampleFormat;
    i32 stereo;
    i32 loopCount;
    struct _SAMPLE* activeSample;
    i32 channelType;
};
#pragma pack(pop)
SIZE(SamplePlaybackData, 0x24);

#endif
