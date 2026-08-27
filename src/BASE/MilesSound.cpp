#include <Ints.h>
#include <BASE/sample.h>
#include <BASE/soundBackends.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/kbwin.h>
#include <mss.h>
#include <string.h>

typedef enum MilesSampleConstant {
    MILES_SAMPLE_HANDLE_COUNT         = 14,
    MILES_SAMPLE_HANDLE_STORAGE_COUNT = 16,
    MILES_SAMPLE_VOLUME_COUNT         = 32,
    MILES_SAMPLE_STOP_WAIT_COUNT      = 10,
    MILES_STOP_ALL_WAIT_COUNT         = 5,
    MILES_SAMPLE_STATUS_DONE          = 2,
    MILES_SAMPLE_STATUS_PLAYING       = 4,
    MILES_VOLUME_CONVERSION_MODE      = 100
} MilesSampleConstant;

SampleChannelStruct SCS[SOUND_CHANNEL_TYPE_COUNT] = {
    {0, 1, 0},
    {1, 2, 1},
    {2, 6, 2},
    {6, 16, 6}
};

static i32 gMilesSamplesReady = 0;
static struct _SAMPLE* gMilesSampleHandles[MILES_SAMPLE_HANDLE_STORAGE_COUNT] =
    {};
static i32 gMilesSampleHandleCount = 0;
static i16 gMilesSampleVolumes[MILES_SAMPLE_VOLUME_COUNT] = {};

namespace {


    inline i32 MilesSampleFormat(SamplePlaybackData* sampleData) {
        if (sampleData->sampleFormat != FORMAT_8_BIT) {
            if (sampleData->stereo != 0)
                return 3;
            else
                return 1;
        } else if (sampleData->stereo != 0) {
            return 2;
        } else {
            return 0;
        }
    }

}

void StartupMilesSamples(struct _DIG_DRIVER* driver) {
    for (i32 index = 0; index < MILES_SAMPLE_VOLUME_COUNT; ++index)
        gMilesSampleVolumes[index] = 0;
    memset(&gMilesSamplesReady, 0, 0x48);
    AllocateMilesSampleHandles(driver);
    gMilesSamplesReady = true;
}

void StopMilesSampleHandle(struct _SAMPLE* sampleHandle) {
    if (sampleHandle == NULL)
        return;
    bool waitForStop = false;
    if (gMilesSampleHandles[0] == sampleHandle)
        waitForStop = true;
    AIL_end_sample(sampleHandle);
    if (waitForStop) {
        for (i32 index = 0; index < MILES_SAMPLE_STOP_WAIT_COUNT; ++index) {
            ServiceMilesSamples();
            DelayMilli(1);
        }
    }
}

void AllocateMilesSampleHandles(struct _DIG_DRIVER* driver) {
    if (driver == NULL)
        return;
    i32 index;
    for (index = 0; index < MILES_SAMPLE_HANDLE_COUNT; ++index) {
        gMilesSampleHandles[index] = AIL_allocate_sample_handle(driver);
        if (gMilesSampleHandles[index] == NULL)
            break;
    }
    gMilesSampleHandleCount = index;
}

void SetMilesSampleHandleVolume(struct _SAMPLE* sampleHandle, i32 volume) {
    if (gMilesSamplesReady == 0)
        return;
    i32 index = 0;
    i32 foundIndex = -1;
    for (; index < gMilesSampleHandleCount; ++index) {
        if (sampleHandle == gMilesSampleHandles[index])
            foundIndex = index;
    }
    AIL_set_sample_volume(
        sampleHandle,
        gpSoundManager->ConvertVolume(volume, SOUND_VOLUME_EFFECT)
    );
    if (foundIndex >= 0)
        gMilesSampleVolumes[foundIndex] = static_cast<i16>(volume);
    Process1WindowsMessage();
}

void StopAllMilesSamples(void) {
    for (i32 idx = 0; idx < gMilesSampleHandleCount; ++idx) {
        if (AIL_sample_status(gMilesSampleHandles[idx]) == MILES_SAMPLE_STATUS_PLAYING)
            AIL_end_sample(gMilesSampleHandles[idx]);
    }
    for (i32 waitIndex = 0; waitIndex < MILES_STOP_ALL_WAIT_COUNT; ++waitIndex) {
        ServiceMilesSamples();
        DelayMilli(1);
    }
}

void StopMilesSample(class sample* sampleResource) {
    if (sampleResource != NULL)
        StopMilesSampleHandle(sampleResource->m_playbackData.activeSample);
}

void SetMilesSampleVolume(class sample* sampleResource, i32 volume) {
    if (sampleResource != NULL)
        SetMilesSampleHandleVolume(sampleResource->m_playbackData.activeSample, volume);
}

bool MilesSamplePlaying(class sample* sampleResource) {
    if (sampleResource == NULL)
        return false;
    return AIL_sample_status(sampleResource->m_playbackData.activeSample)
        == MILES_SAMPLE_STATUS_PLAYING;
}

void PlayMilesSample(class sample* sampleResource) {
    if (sampleResource == NULL)
        return;
    SamplePlaybackData* sampleData = &sampleResource->m_playbackData;
    if (gMilesSamplesReady == 0 || sampleData->volume == 0)
        return;
    {
        SampleChannelStruct* channelData = &SCS[sampleData->channelType];
        i32 channelIndex;
        for (channelIndex = channelData->startChannel;
             channelIndex < channelData->endChannel;
             ++channelIndex) {
            if (AIL_sample_status(gMilesSampleHandles[channelIndex])
                == MILES_SAMPLE_STATUS_DONE)
                break;
        }
        if (channelIndex == channelData->endChannel) {
            if (sampleData->channelType == SOUND_CHANNEL_TYPE_COUNT - 1)
                return;
            channelIndex = channelData->currentChannel;
            ++channelData->currentChannel;
            if (channelData->currentChannel >= channelData->endChannel) {
                channelData->currentChannel = channelData->startChannel;
                channelIndex = channelData->currentChannel;
            }
            StopMilesSampleHandle(gMilesSampleHandles[channelIndex]);
        }

        struct _SAMPLE* handle = gMilesSampleHandles[channelIndex];
        gMilesSampleVolumes[channelIndex] =
            static_cast<i16>(static_cast<i8>(sampleData->volume));
        AIL_init_sample(handle);
        i32 formatMode = MilesSampleFormat(sampleData);
        AIL_set_sample_type(handle, formatMode, 0);
        AIL_set_sample_playback_rate(handle, (sampleData->sampleRate));
        AIL_set_sample_loop_count(handle, sampleData->loopCount == 0);
        AIL_set_sample_address(handle, sampleData->data, sampleData->size);
        AIL_set_sample_volume(
            handle,
            gpSoundManager->ConvertVolume(sampleData->volume, SOUND_VOLUME_EFFECT)
        );
        AIL_start_sample(handle);
        sampleData->activeSample = handle;
    }
}

void ServiceMilesSamples(void) {
    AIL_serve();
}

void AdjustMilesSampleVolumes(void) {
    for (i32 index = 1; index < gMilesSampleHandleCount; ++index) {
        struct _SAMPLE* sampleHandle = gMilesSampleHandles[index];
        if (gConfig.soundVolume != CONFIG_VOLUME_MUTED) {
            if (AIL_sample_status(sampleHandle) == MILES_SAMPLE_STATUS_PLAYING)
                SetMilesSampleHandleVolume(sampleHandle, gMilesSampleVolumes[index]);
        } else {
            SetMilesSampleHandleVolume(sampleHandle, 0);
        }
    }
}
