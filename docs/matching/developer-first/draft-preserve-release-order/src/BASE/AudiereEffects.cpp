#include <va.h>
#include <BASE/sample.h>
#include <BASE/soundBackends.h>
#include <BASE/AudiereEffects.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>

DATA(0x005395c0) static AudiereEffectsState gAudiereEffects = H2_ZERO_INIT;

VA(0x004cc740, 0x162)
static void PurgeFinishedAudiereSamples(void) {
    while (gAudiereEffects.sampleList != NULL
           && !gAudiereEffects.sampleList->stream->isPlaying()) {
        AudiereSampleNode* next = gAudiereEffects.sampleList->next;
        delete gAudiereEffects.sampleList;
        gAudiereEffects.sampleList = next;
    }
    if (gAudiereEffects.sampleList == NULL)
        return;

    AudiereSampleNode* previous = gAudiereEffects.sampleList;
    while (previous->next != NULL) {
        AudiereSampleNode* node = previous->next;
        if (node->stream->isPlaying()) {
            previous = node;
        } else {
            previous->next = node->next;
            delete node;
        }
    }
}

VA(0x004cc8b0, 0x3a)
static AudiereSampleNode* FindAudiereSample(sample* sampleResource) {
    for (AudiereSampleNode* node = gAudiereEffects.sampleList; node != NULL;
         node = node->next) {
        if (node->sampleResource == sampleResource)
            return node;
    }
    return NULL;
}

VA(0x004cc8f0, 0x39c)
void PlayAudiereSample(sample* sampleResource, audiere::AudioDevicePtr device) {
    if (device == NULL || AudiereSampleIterationActive())
        return;

    AudiereSampleNode* node = FindAudiereSample(sampleResource);
    if (node != NULL) {
        if (node->stream->isPlaying())
            return;
        PurgeFinishedAudiereSamples();
    }

    gAudiereEffects.sampleList =
        new AudiereSampleNode(sampleResource, gAudiereEffects.sampleList);

    const SamplePlaybackData& playback = sampleResource->m_playbackData;
    gAudiereEffects.buffer = playback.data;
    gAudiereEffects.sampleRate = IDX(playback.sampleRate);
    gAudiereEffects.frameCount = playback.size;
    gAudiereEffects.channelCount = playback.stereo ? 2 : 1;
    if (playback.stereo)
        gAudiereEffects.frameCount >>= 1;
    gAudiereEffects.sampleFormat =
        playback.sampleFormat == FORMAT_8_BIT ? audiere::SF_U8 : audiere::SF_S16;
    if (playback.sampleFormat != FORMAT_8_BIT)
        gAudiereEffects.frameCount >>= 1;

    gAudiereEffects.sampleList->stream = device->openBuffer(
        gAudiereEffects.buffer,
        gAudiereEffects.frameCount,
        gAudiereEffects.channelCount,
        gAudiereEffects.sampleRate,
        gAudiereEffects.sampleFormat
    );
    if (!gAudiereEffects.sampleList->stream) {
        AudiereSampleNode* failedNode = gAudiereEffects.sampleList;
        gAudiereEffects.sampleList = failedNode->next;
        delete failedNode;
    } else {
        const float volume = gpSoundManager->ConvertVolumeFloat(
            playback.volume, SOUND_VOLUME_EFFECT
        );
        gAudiereEffects.sampleList->stream->setVolume(volume);
        gAudiereEffects.sampleList->stream->setRepeat(playback.looping != 0);
        gAudiereEffects.sampleList->stream->play();
    }
    PurgeFinishedAudiereSamples();
}

VA(0x004ccc90, 0x3c)
bool AudiereSamplePlaying(sample* sampleResource) {
    AudiereSampleNode* node = FindAudiereSample(sampleResource);
    if (node == NULL)
        return false;
    return node->stream->isPlaying();
}

VA(0x004cccd0, 0x70)
void StopAudiereSample(sample* sampleResource) {
    if (AudiereSampleIterationActive())
        return;
    AudiereSampleNode* node = FindAudiereSample(sampleResource);
    if (node != NULL) {
        if (node->stream->isPlaying())
            node->stream->stop();
        PurgeFinishedAudiereSamples();
    }
}

VA(0x004ccd40, 0x63)
void SetAudiereSampleVolume(sample* sampleResource, i32 volume) {
    if (AudiereSampleIterationActive())
        return;
    float sampleVolume =
        gpSoundManager->ConvertVolumeFloat(volume, SOUND_VOLUME_EFFECT);
    AudiereSampleNode* sampleNode = FindAudiereSample(sampleResource);
    if (sampleNode != NULL)
        sampleNode->stream->setVolume(sampleVolume);
}

VA(0x004ccdb0, 0x62)
void WaitForAudiereSample(sample* sampleResource) {
    if (AudiereSampleIterationActive())
        return;
    AudiereSampleNode* node = FindAudiereSample(sampleResource);
    if (node != NULL) {
        while (node->stream->isPlaying())
            DelayMilli(10);
        PurgeFinishedAudiereSamples();
    }
}

VA(0x004cce20, 0x77)
void StopAllAudiereSamples(void) {
    if (AudiereSampleIterationActive())
        return;
    for (AudiereSampleNode* node = gAudiereEffects.sampleList; node != NULL;
         node = node->next) {
        if (node->stream->isPlaying())
            node->stream->stop();
    }
    PurgeFinishedAudiereSamples();
}

VA(0x004ccea0, 0x6b)
void SetAllAudiereSampleVolumes(i32 volume) {
    if (AudiereSampleIterationActive())
        return;
    float sampleVolume =
        gpSoundManager->ConvertVolumeFloat(volume, SOUND_VOLUME_EFFECT);
    for (AudiereSampleNode* sampleNode = gAudiereEffects.sampleList; sampleNode != NULL;
         sampleNode = sampleNode->next)
        sampleNode->stream->setVolume(sampleVolume);
}

VA(0x004ccf10, 0x12)
void BeginAudiereSampleIteration(void) {
    ++gAudiereEffects.sampleIterationDepth;
}

VA(0x004ccf30, 0x12)
void EndAudiereSampleIteration(void) {
    --gAudiereEffects.sampleIterationDepth;
}

VA(0x004ccf50, 0x11)
bool AudiereSampleIterationActive(void) {
    return gAudiereEffects.sampleIterationDepth > 0;
}
