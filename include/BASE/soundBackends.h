#ifndef HOMM2_BASE_SOUNDBACKENDS_H
#define HOMM2_BASE_SOUNDBACKENDS_H

#include <va.h>
#include <audiere.h>

class sample;
struct _DIG_DRIVER;

struct AudiereSampleNode {
    audiere::OutputStreamPtr stream;
    class sample* sampleResource;
    AudiereSampleNode* next;

    AudiereSampleNode(class sample* resource, AudiereSampleNode* nextNode) {
        stream = NULL;
        sampleResource = resource;
        next = nextNode;
    }
};

// Retail keeps the music stream/source as static class members: their atexit
// teardowns carry VC6's member-static destroy-once guard (one flag byte,
// bit per member), which file-scope statics never get.
struct AudiereMusicState {
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
