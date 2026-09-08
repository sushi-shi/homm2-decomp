#ifndef HOMM2_BASE_SAMPLE_H
#define HOMM2_BASE_SAMPLE_H

#include <Ints.h>
#include <BASE/resource.h>
#include <BASE/sampleData.h>

class sample : public resource {
public:
    SamplePlaybackData m_playbackData;
    sample(const char*);
    virtual inline ~sample() override;
};
#endif
