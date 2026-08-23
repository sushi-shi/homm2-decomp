#include <va.h>
#include <BASE/sample.h>
#include <BASE/soundBackends.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>

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

DATA(0x005395c0) static AudiereEffectsState gAudiereEffects = {0};

VA(0x004cc740, 0x162)
void PurgeFinishedAudiereSamples(void) {
    if (gAudiereEffects.sampleList == NULL)
        return;
    AudiereSampleNode* head = NULL;
    for (;;) {
        if (!gAudiereEffects.sampleList->stream->isPlaying()) {
            head = gAudiereEffects.sampleList->next;
            delete gAudiereEffects.sampleList;
            gAudiereEffects.sampleList = head;
            if (gAudiereEffects.sampleList == NULL)
                return;
        } else {
            break;
        }
    }
    AudiereSampleNode* current = gAudiereEffects.sampleList->next;
    AudiereSampleNode* cursor = gAudiereEffects.sampleList;
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
    for (AudiereSampleNode* node = gAudiereEffects.sampleList; node != NULL;
         node = node->next) {
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

    gAudiereEffects.sampleList =
        new AudiereSampleNode(sampleResource, gAudiereEffects.sampleList);

    gAudiereEffects.buffer = sampleResource->m_playbackData.data;
    gAudiereEffects.sampleRate = IDX(sampleResource->m_playbackData.sampleRate);
    gAudiereEffects.frameCount = sampleResource->m_playbackData.size;
    if (sampleResource->m_playbackData.stereo != 0) {
        gAudiereEffects.channelCount = 2;
        gAudiereEffects.frameCount >>= 1;
    } else {
        gAudiereEffects.channelCount = 1;
    }
    if (sampleResource->m_playbackData.sampleFormat != FORMAT_8_BIT) {
        gAudiereEffects.sampleFormat = audiere::SF_S16;
        gAudiereEffects.frameCount >>= 1;
    } else {
        gAudiereEffects.sampleFormat = audiere::SF_U8;
    }

    gAudiereEffects.sampleList->stream = device->openBuffer(
        gAudiereEffects.buffer,
        gAudiereEffects.frameCount,
        gAudiereEffects.channelCount,
        gAudiereEffects.sampleRate,
        gAudiereEffects.sampleFormat
    );
    if (!gAudiereEffects.sampleList->stream) {
        AudiereSampleNode* deadNode = gAudiereEffects.sampleList;
        gAudiereEffects.sampleList = gAudiereEffects.sampleList->next;
        delete deadNode;
    } else {
        float convertedVolume = gpSoundManager->ConvertVolumeFloat(
            sampleResource->m_playbackData.volume,
            SOUND_VOLUME_EFFECT
        );
        gAudiereEffects.sampleList->stream->setVolume(convertedVolume);
        gAudiereEffects.sampleList->stream->setRepeat(
            sampleResource->m_playbackData.loopCount != 0 ? true : false
        );
        gAudiereEffects.sampleList->stream->play();
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

inline AudiereSampleNode::~AudiereSampleNode() {}
