#ifndef HOMM2_BASE_SAMPLE_H
#define HOMM2_BASE_SAMPLE_H
#include <va.h>
#include <BASE/resource.h>
#include <BASE/sampleData.h>

H2_ENUM_CLASS_BEGIN(sampleAudioFormat)
    SAMPLE_FORMAT_8_BIT = 0,
    SAMPLE_FORMAT_16_BIT = 1,
    SAMPLE_FORMAT_STEREO = 2
H2_ENUM_CLASS_END(sampleAudioFormat)

H2_ENUM_CLASS_BEGIN(samplePlaybackRate)
    SAMPLE_RATE_11025 = 11025,
    SAMPLE_RATE_22050 = 22050,
    SAMPLE_RATE_44100 = 44100
H2_ENUM_CLASS_END(samplePlaybackRate)

#pragma pack(push, 1)
class sample : public resource {
public:
    SamplePlaybackData m_playbackData;
    sample(char*, i32l, i32l, i32l);
    __declspec(dllexport) virtual inline ~sample() OVERRIDE;
};
#pragma pack(pop)
SIZE(sample, 0x30);
#endif
