#ifndef HOMM2_BASE_AUDIEREEFFECTS_H
#define HOMM2_BASE_AUDIEREEFFECTS_H

#include <va.h>
#include <audiere.h>

class sample;

struct AudiereSampleNode {
    audiere::OutputStreamPtr stream;
    sample* sampleResource;
    AudiereSampleNode* next;

    AudiereSampleNode(sample* resource, AudiereSampleNode* nextNode)
        : stream(NULL), sampleResource(resource), next(nextNode) {}

};

SIZE(AudiereSampleNode, 0xc);

struct AudiereEffectsState {
    void* buffer;
    i32 frameCount;
    i32 channelCount;
    i32 sampleRate;
    audiere::SampleFormat sampleFormat;
    AudiereSampleNode* sampleList;
    i32 sampleIterationDepth;
};

SIZE(AudiereEffectsState, 0x1c);

#endif
