#include <va.h>
#include <BASE/sample.h>
#include <BASE/soundBackends.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/kbwin.h>
#include <mss.h>
#include <string.h>

H2_ENUM_BEGIN(MilesSampleConstant)
    MILES_SAMPLE_HANDLE_COUNT         = 14,
    MILES_SAMPLE_HANDLE_STORAGE_COUNT = 16,
    MILES_SAMPLE_VOLUME_COUNT         = 32,
    MILES_SAMPLE_STOP_WAIT_COUNT      = 10,
    MILES_STOP_ALL_WAIT_COUNT         = 5,
    MILES_SAMPLE_STATUS_DONE          = 2,
    MILES_SAMPLE_STATUS_PLAYING       = 4,
    MILES_VOLUME_CONVERSION_MODE      = 100
H2_ENUM_END(MilesSampleConstant)

DATA(0x00520188) SampleChannelStruct SCS[SOUND_CHANNEL_TYPE_COUNT] = {
    {0, 1, 0},
    {1, 2, 1},
    {2, 6, 2},
    {6, 16, 6}
};

DATA(0x005396d8) static i32 gMilesSamplesReady = 0;
DATA(0x005396dc) static struct _SAMPLE* gMilesSampleHandles[MILES_SAMPLE_HANDLE_STORAGE_COUNT] =
    H2_ZERO_INIT;
DATA(0x0053971c) static i32 gMilesSampleHandleCount = 0;
DATA(0x00539720) static i16 gMilesSampleVolumes[MILES_SAMPLE_VOLUME_COUNT] = H2_ZERO_INIT;

namespace {

    // The Miles DIG_F_* sample format code: bit 0 selects 16-bit samples and
    // bit 1 selects stereo.
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

} // namespace

VA(0x004cdb30, 0x57)
void StartupMilesSamples(struct _DIG_DRIVER* driver) {
    for (i32 index = 0; index < MILES_SAMPLE_VOLUME_COUNT; ++index)
        gMilesSampleVolumes[index] = 0;
    memset(&gMilesSamplesReady, 0, 0x48);
    AllocateMilesSampleHandles(driver);
    gMilesSamplesReady = true;
}

VA(0x004cdb90, 0x67)
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

VA(0x004cdc00, 0x5b)
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

VA(0x004cdc60, 0x8c)
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

VA(0x004cdcf0, 0x79)
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

VA(0x004cdd70, 0x1c)
void StopMilesSample(class sample* sampleResource) {
    if (sampleResource != NULL)
        StopMilesSampleHandle(sampleResource->m_playbackData.activeSample);
}

VA(0x004cdd90, 0x24)
void SetMilesSampleVolume(class sample* sampleResource, i32 volume) {
    if (sampleResource != NULL)
        SetMilesSampleHandleVolume(sampleResource->m_playbackData.activeSample, volume);
}

VA(0x004cddc0, 0x2c)
bool MilesSamplePlaying(class sample* sampleResource) {
    if (sampleResource == NULL)
        return false;
    return AIL_sample_status(sampleResource->m_playbackData.activeSample)
        == MILES_SAMPLE_STATUS_PLAYING;
}

VA(0x004cddf0, 0x1cb)
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
        AIL_set_sample_playback_rate(handle, IDX(sampleData->sampleRate));
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

VA(0x004cdfc0, 0xb)
void ServiceMilesSamples(void) {
    AIL_serve();
}

VA(0x004cdfd0, 0x6d)
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
