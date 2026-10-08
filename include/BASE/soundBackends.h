#ifndef HOMM2_BASE_SOUNDBACKENDS_H
#define HOMM2_BASE_SOUNDBACKENDS_H

#include <H2/Ints.h>
#include <Domains.h>
#include <audiere.h>

class sample;
struct _DIG_DRIVER;

typedef enum MilesSampleStorageConstant {
    MILES_SAMPLE_HANDLE_STORAGE_COUNT = 16
} MilesSampleStorageConstant;


typedef enum MilesDigitalFormat {
    MILES_DIG_F_MONO_8    = 0,
    MILES_DIG_F_MONO_16   = 1,
    MILES_DIG_F_STEREO_8  = 2,
    MILES_DIG_F_STEREO_16 = 3
} MilesDigitalFormat;


typedef enum AudiereChannelCount {
    AUDIERE_CHANNELS_MONO   = 1,
    AUDIERE_CHANNELS_STEREO = 2
} AudiereChannelCount;


struct MilesSampleState {
    i32 ready;
    struct _SAMPLE* handles[MILES_SAMPLE_HANDLE_STORAGE_COUNT];
    i32 handleCount;
};

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


struct AudiereEffectsState {
    void* buffer;
    i32 frameCount;
    i32 channelCount;
    i32 sampleRate;
    audiere::SampleFormat sampleFormat;
    AudiereSampleNode* sampleList;
    i32 sampleIterationDepth;
};


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
