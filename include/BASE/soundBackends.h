#ifndef HOMM2_BASE_SOUNDBACKENDS_H
#define HOMM2_BASE_SOUNDBACKENDS_H

#include <va.h>
#include <audiere.h>

class sample;
struct _DIG_DRIVER;

H2_ENUM_BEGIN(MilesSampleStorageConstant)
    MILES_SAMPLE_HANDLE_STORAGE_COUNT = 16
H2_ENUM_END(MilesSampleStorageConstant)

// The Miles DIG_F_* sample format codes AIL_set_sample_type takes: bit 0
// selects 16-bit samples and bit 1 selects stereo.
H2_ENUM_BEGIN(MilesDigitalFormat)
    MILES_DIG_F_MONO_8    = 0,
    MILES_DIG_F_MONO_16   = 1,
    MILES_DIG_F_STEREO_8  = 2,
    MILES_DIG_F_STEREO_16 = 3
H2_ENUM_END(MilesDigitalFormat)

// The interleaved channel count of an Audiere sample buffer.
H2_ENUM_BEGIN(AudiereChannelCount)
    AUDIERE_CHANNELS_MONO   = 1,
    AUDIERE_CHANNELS_STEREO = 2
H2_ENUM_END(AudiereChannelCount)

// Startup clears this complete owner before allocating the handle prefix.
struct MilesSampleState {
    i32 ready;
    struct _SAMPLE* handles[MILES_SAMPLE_HANDLE_STORAGE_COUNT];
    i32 handleCount;
};
SIZE(MilesSampleState, 0x48);

struct AudiereSampleNode {
    audiere::OutputStreamPtr stream;
    class sample* sampleResource;
    AudiereSampleNode* next;

    AudiereSampleNode(class sample* resource, AudiereSampleNode* nextNode) {
        stream = NULL;
        sampleResource = resource;
        next = nextNode;
    }

    ~AudiereSampleNode() {}
};

// AudiereEffects keeps its sample-effect state in one private owner.
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

// Retail keeps AudiereMusic::stream/source as static class members: their atexit
// teardowns carry VC6's member-static destroy-once guard (one flag byte,
// bit per member), which file-scope statics never get.
struct AudiereMusic {
    static audiere::OutputStreamPtr stream;
    static audiere::SampleSourcePtr source;
};

void StartupMilesSamples(struct _DIG_DRIVER* driver);
void AllocateMilesSampleHandles(struct _DIG_DRIVER* driver);
void StopMilesSample(class sample* sampleResource);
void SetMilesSampleVolume(class sample* sampleResource, i32 volume);
bool MilesSamplePlaying(class sample* sampleResource);
void PlayMilesSample(class sample* sampleResource);
void StopAllMilesSamples(void);
void ServiceMilesSamples(void);
void AdjustMilesSampleVolumes(void);

void PlayAudiereSample(class sample* sampleResource, audiere::AudioDevicePtr device);
bool AudiereSamplePlaying(class sample* sampleResource);
void StopAudiereSample(class sample* sampleResource);
void SetAudiereSampleVolume(class sample* sampleResource, i32 volume);
void WaitForAudiereSample(class sample* sampleResource);
void StopAllAudiereSamples(void);
void SetAllAudiereSampleVolumes(i32 volume);
void BeginAudiereSampleIteration(void);
void EndAudiereSampleIteration(void);
bool AudiereSampleIterationActive(void);

void StopAudiereMusic(i32& currentTrack);
bool AudiereMusicAvailable(void);
bool AudiereMusicPlaying(void);
bool StartupAudiereMusic(audiere::AudioDevicePtr device);
void ResetAudiereMusic(void);
void SetAudiereMusicVolume(i32 volume, i32 fading);
void PlayAudiereMusic(
    audiere::AudioDevicePtr device,
    i32& currentTrack,
    i32& fadeSteps,
    i32 track
);

#endif
