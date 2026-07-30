#ifndef HOMM2_BASE_SAMPLE_H
#define HOMM2_BASE_SAMPLE_H

#include <Ints.h>
#include <BASE/resource.h>
#include <BASE/sampleData.h>

#pragma pack(push, 1)
class sample : public resource {
public:
    SamplePlaybackData m_playbackData;
    sample(char*, i32l, i32l, i32l);
    virtual inline ~sample() override;
};
#pragma pack(pop)
#endif
