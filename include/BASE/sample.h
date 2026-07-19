#ifndef HOMM2_BASE_SAMPLE_H
#define HOMM2_BASE_SAMPLE_H

#include <va.h>
#include <BASE/resource.h>
#include <BASE/sampleData.h>

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
