#include <va.h>
#include <BASE/sample.h>
#include <BASE/soundBackends.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>

SIZE(AudiereSampleNode, 0xc);

DATA(0x005395c0) static void* gAudiereEffectBuffer;
DATA(0x005395c4) static i32 gAudiereEffectFrameCount;
DATA(0x005395c8) static i32 gAudiereEffectChannelCount;
DATA(0x005395cc) static i32 gAudiereEffectSampleRate;
DATA(0x005395d0) static audiere::SampleFormat gAudiereEffectSampleFormat;
DATA(0x005395d4) static AudiereSampleNode* gAudiereSampleList;
DATA(0x005395d8) static i32 gAudiereSampleIterationDepth;

VA(0x004cc740, 0x162)
void PurgeFinishedAudiereSamples(void) {
    if (gAudiereSampleList == NULL)
        return;
    AudiereSampleNode* head = NULL;
    for (;;) {
        if (!gAudiereSampleList->stream->isPlaying()) {
            head = gAudiereSampleList->next;
            delete gAudiereSampleList;
            gAudiereSampleList = head;
            if (gAudiereSampleList == NULL)
                return;
        } else {
            break;
        }
    }
    AudiereSampleNode* current = gAudiereSampleList->next;
    AudiereSampleNode* cursor = gAudiereSampleList;
    while (current != NULL) {
        if (!current->stream->isPlaying()) {
            cursor->next = current->next;
            delete current;
            current = cursor->next;
        } else {
            cursor = current;
            current = current->next;
        }
    }
}

VA(0x004cc8b0, 0x3a)
AudiereSampleNode* FindAudiereSample(class sample* sampleResource) {
    for (AudiereSampleNode* node = gAudiereSampleList; node != NULL; node = node->next) {
        if (node->sampleResource == sampleResource)
            return node;
    }
    return NULL;
}

VA(0x004cc8f0, 0x39c)
void PlayAudiereSample(class sample* sampleResource, audiere::AudioDevicePtr device) {
    if (device == NULL)
        return;
    if (AudiereSampleIterationActive())
        return;

    AudiereSampleNode* node = FindAudiereSample(sampleResource);
    if (node != NULL) {
        if (node->stream->isPlaying())
            return;
        PurgeFinishedAudiereSamples();
    }

    gAudiereSampleList =
        new AudiereSampleNode(sampleResource, gAudiereSampleList);

    gAudiereEffectBuffer = sampleResource->m_playbackData.data;
    gAudiereEffectSampleRate = IDX(sampleResource->m_playbackData.sampleRate);
    gAudiereEffectFrameCount = sampleResource->m_playbackData.size;
    if (sampleResource->m_playbackData.stereo != 0) {
        gAudiereEffectChannelCount = 2;
        gAudiereEffectFrameCount >>= 1;
    } else {
        gAudiereEffectChannelCount = 1;
    }
    if (sampleResource->m_playbackData.sampleFormat != FORMAT_8_BIT) {
        gAudiereEffectSampleFormat = audiere::SF_S16;
        gAudiereEffectFrameCount >>= 1;
    } else {
        gAudiereEffectSampleFormat = audiere::SF_U8;
    }

    gAudiereSampleList->stream = device->openBuffer(
        gAudiereEffectBuffer,
        gAudiereEffectFrameCount,
        gAudiereEffectChannelCount,
        gAudiereEffectSampleRate,
        gAudiereEffectSampleFormat
    );
    if (!gAudiereSampleList->stream) {
        AudiereSampleNode* deadNode = gAudiereSampleList;
        gAudiereSampleList = gAudiereSampleList->next;
        delete deadNode;
    } else {
        float convertedVolume = gpSoundManager->ConvertVolumeFloat(
            sampleResource->m_playbackData.volume,
            SOUND_VOLUME_EFFECT
        );
        gAudiereSampleList->stream->setVolume(convertedVolume);
        gAudiereSampleList->stream->setRepeat(
            sampleResource->m_playbackData.loopCount != 0 ? true : false
        );
        gAudiereSampleList->stream->play();
    }
    PurgeFinishedAudiereSamples();
}

VA(0x004ccc90, 0x3c)
bool AudiereSamplePlaying(class sample* sampleResource) {
    AudiereSampleNode* node = FindAudiereSample(sampleResource);
    if (node == NULL)
        return false;
    return node->stream->isPlaying();
}

VA(0x004cccd0, 0x70)
void StopAudiereSample(class sample* sampleResource) {
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
void SetAudiereSampleVolume(class sample* sampleResource, i32 volume) {
    if (AudiereSampleIterationActive())
        return;
    float sampleVolume =
        gpSoundManager->ConvertVolumeFloat(volume, SOUND_VOLUME_EFFECT);
    AudiereSampleNode* sampleNode = FindAudiereSample(sampleResource);
    if (sampleNode != NULL)
        sampleNode->stream->setVolume(sampleVolume);
}

VA(0x004ccdb0, 0x62)
void WaitForAudiereSample(class sample* sampleResource) {
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
    for (AudiereSampleNode* node = gAudiereSampleList; node != NULL; node = node->next) {
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
    for (AudiereSampleNode* sampleNode = gAudiereSampleList; sampleNode != NULL;
         sampleNode = sampleNode->next)
        sampleNode->stream->setVolume(sampleVolume);
}

VA(0x004ccf10, 0x12)
void BeginAudiereSampleIteration(void) {
    ++gAudiereSampleIterationDepth;
}

VA(0x004ccf30, 0x12)
void EndAudiereSampleIteration(void) {
    --gAudiereSampleIterationDepth;
}

VA(0x004ccf50, 0x11)
bool AudiereSampleIterationActive(void) {
    return gAudiereSampleIterationDepth > 0;
}
