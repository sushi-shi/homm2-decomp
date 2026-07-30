#include <Ints.h>
#include <BASE/soundManager.h>
#include <BASE/sample.h>
#include <BASE/soundmgr.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <mss.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <BASE/Misc.h>

typedef enum SoundConstant {
    MIDI_VOLUME_MAX              = 0x7f,
    FADE_HOLD_STEPS              = 10,
    FADE_TOTAL_STEPS             = H2EnumIndex(CONFIG_VOLUME_MAX) + 1,
    AMBIENT_FADE_DELAY_TICKS     = 900,
    SOUND_TIMER_INTERVAL         = 16,
    AIL_WAVEOUT_PREFERENCE       = 15,
    PCM_BITS_PER_BYTE_SHIFT      = 3,
    DEFAULT_SAMPLE_RATE          = 22050,
    DEFAULT_SAMPLE_BITS          = 8,
    DEFAULT_SAMPLE_CHANNELS      = 1,
    SAMPLE_STOP_ALL_WAIT_COUNT   = 5,
    SAMPLE_STOP_MUSIC_WAIT_COUNT = 10,
    FADE_STEP_TICKS              = 60,
    NO_SAMPLE_CHANNEL_TYPE       = SOUND_CHANNEL_TYPE_COUNT
} SoundConstant;

typedef enum SoundSampleStatus {
    SAMPLE_STATUS_DONE    = 2,
    SAMPLE_STATUS_PLAYING = 4
} SoundSampleStatus;


static PCMWAVEFORMAT gWaveFormat;

enum class SoundStateSpan : i32 {
    SOUND_STATE_RESET_SPAN = 0xae
};
using enum SoundStateSpan;

i32 soundManager::ConvertVolume(i32 volume, SoundVolumeConversionMode soundType) {
    i32 local_8 = 0;
    if (soundType == SOUND_VOLUME_MUSIC) {
        if (gConfig.musicVolume >= CONFIG_VOLUME_MIN
            && gConfig.musicVolume <= CONFIG_VOLUME_MAX) {
            local_8 =
                ((FADE_TOTAL_STEPS - H2EnumIndex(gConfig.musicVolume)) * volume)
                / H2EnumIndex(CONFIG_VOLUME_MAX);
            if (local_8 < 1)
                local_8 = 1;
        }
    } else if (gConfig.soundVolume >= CONFIG_VOLUME_MIN
               && gConfig.soundVolume <= CONFIG_VOLUME_MAX) {
        local_8 =
            ((FADE_TOTAL_STEPS - H2EnumIndex(gConfig.soundVolume)) * volume) / H2EnumIndex(CONFIG_VOLUME_MAX);
        if (local_8 < 1)
            local_8 = 1;
    }
    if (local_8 < 0)
        local_8 = 0;
    if (MIDI_VOLUME_MAX < local_8)
        local_8 = MIDI_VOLUME_MAX;
    return local_8;
}

void __stdcall SetReady2Poll(u32l) {
    if (gpSoundManager == NULL)
        return;
    gpSoundManager->m_pollToggle ^= 1;
    if (gpSoundManager->m_pollToggle != 0)
        gpSoundManager->m_pollDue = 1;
}

void __stdcall UpdateTimers(u32l) {
    iCalibrateLoop++;
    glMilliCounter += SOUND_TIMER_INTERVAL;
    SetReady2Poll(0);
}

soundManager::soundManager(void) : baseManager(), field_0x574(1) {
    i32 local_8;
    m_active = false;
    m_fadeSteps = 0;
    field_0x56c = 0;
    for (local_8 = 0; local_8 < SAVED_SAMPLE_VOLUME_CAPACITY; local_8++)
        iLastVolume[local_8] = 0;
    memset(&m_ready, 0, H2EnumIndex(SOUND_STATE_RESET_SPAN));
    m_samplesReady = 0;
    m_digitalDriver = NULL;
    field_0x3a = 0;
}

struct _DIG_DRIVER*
WAVE_init_driver(u32l sampleRate, u16 bitsPerSample, u16 channels, u16 showErrors) {
    u32 numDevs;
    struct _DIG_DRIVER* drvr;
    WAVEOUTCAPSA caps;
    i32 rc;
    numDevs = waveOutGetNumDevs();
    if (numDevs == 0) {
        drvr = NULL;
        return NULL;
    }
    if (waveOutGetDevCapsA(0, &caps, sizeof(caps)) != 0) {
        MessageBoxA(
            hwndApp,
            "Sound initialization error!  No wave devices found.",
            "Startup Error",
            0
        );
        drvr = NULL;
        return NULL;
    }
    if (gbUseWaveout != 0)
        AIL_set_preference(AIL_WAVEOUT_PREFERENCE, 1);
    gWaveFormat.wf.wFormatTag = 1;
    gWaveFormat.wf.nChannels = channels;
    gWaveFormat.wf.nSamplesPerSec = sampleRate;
    gWaveFormat.wf.nAvgBytesPerSec =
        (bitsPerSample >> PCM_BITS_PER_BYTE_SHIFT) * channels * sampleRate;
    gWaveFormat.wf.nBlockAlign = (bitsPerSample >> PCM_BITS_PER_BYTE_SHIFT) * channels;
    gWaveFormat.wBitsPerSample = bitsPerSample;
    rc = AIL_waveOutOpen(&drvr, NULL, 0, &gWaveFormat.wf);
    if (rc != 0) {
        if (showErrors != 0)
            MessageBoxA(
                hwndApp,
                AIL_last_error(),
                "Sound initialization error!",
                0
            );
        drvr = NULL;
        return NULL;
    }
    return drvr;
}

i32 soundManager::Open(i32) {
    i32 musicTrack;

    m_midiStarted = 0;
    m_midiReady = 0;

    memset(bSaveMusicPosition, 0, MIDI_TRACK_COUNT);
    memset(bMusicIsLooping, 0, MIDI_TRACK_COUNT);


    bSaveMusicPosition[16] = 1;
    bSaveMusicPosition[18] = 1;
    bSaveMusicPosition[14] = 1;
    bSaveMusicPosition[15] = 1;
    bSaveMusicPosition[11] = 1;
    bSaveMusicPosition[13] = 1;
    bSaveMusicPosition[17] = 1;
    bSaveMusicPosition[12] = 1;
    bSaveMusicPosition[28] = 1;
    bSaveMusicPosition[42] = 1;

    bMusicIsLooping[16] = 1;
    bMusicIsLooping[18] = 1;
    bMusicIsLooping[14] = 1;
    bMusicIsLooping[15] = 1;
    bMusicIsLooping[11] = 1;
    bMusicIsLooping[13] = 1;
    bMusicIsLooping[17] = 1;
    bMusicIsLooping[12] = 1;
    bMusicIsLooping[8] = 1;
    bMusicIsLooping[9] = 1;
    bMusicIsLooping[5] = 1;
    bMusicIsLooping[6] = 1;
    bMusicIsLooping[10] = 1;
    bMusicIsLooping[7] = 1;
    bMusicIsLooping[42] = 1;
    bMusicIsLooping[28] = 1;
    bMusicIsLooping[43] = 1;
    bMusicIsLooping[22] = 1;
    bMusicIsLooping[24] = 1;
    for (musicTrack = 2; musicTrack <= 4; musicTrack++)
        bMusicIsLooping[musicTrack] = 1;


    m_currentTrack = -1;
    if (gbNoSound != 0) {
        goto managerReady;
    }
    m_pollToggle = m_pollDue = m_pollRequested = 0;
    AIL_startup();

    MIDIStartup();
    if (m_midiReady == 0) {
        gConfig.musicVolume = CONFIG_VOLUME_MUTED;
        WritePrefs();
    }

    m_samplesReady = 1;
    memset(&m_ready, 0, H2EnumIndex(SOUND_STATE_RESET_SPAN));
    if (gbDontTryDigital == 0 && m_digitalDriver == NULL)
        m_digitalDriver = WAVE_init_driver(
            DEFAULT_SAMPLE_RATE,
            DEFAULT_SAMPLE_BITS,
            DEFAULT_SAMPLE_CHANNELS,
            0
        );
    if (m_digitalDriver == NULL) {
        gConfig.soundVolume = CONFIG_VOLUME_MUTED;
        WritePrefs();
    }
    AllocateSampleHandles();
    m_ready = 1;
    m_midiFile = NULL;
    memset(m_savedTrackPositions, 0, sizeof(m_savedTrackPositions));
    m_fading = 1;

managerReady:
    m_messageMask = BASE_MANAGER_ACCEPT_LEFT_BUTTON_UP;
    m_priority = SOUND_MANAGER_PRIORITY;
    m_active = true;
    strcpy(m_name, "soundManager");
    return 0;
}

void soundManager::AllocateSampleHandles(void) {
    i32 local_8;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == NULL)
        return;
    for (local_8 = 0; local_8 < SOUND_SAMPLE_HANDLE_CAPACITY; local_8++) {
        m_sampleHandles[local_8] = AIL_allocate_sample_handle(m_digitalDriver);
        if (m_sampleHandles[local_8] == NULL)
            break;
    }
    m_numSampleHandles = local_8;
}

void soundManager::Close(void) {
    if (m_active != 1)
        return;
    if (gbNoSound != 0)
        goto soundClosed;
    LogStr("SD1");
    MIDIShutdown();
    LogStr("SD2");
    AIL_shutdown();
    LogStr("SD3");
soundClosed:
    m_active = false;
    gbNoSound = true;
}

MessageDispatchResult soundManager::Main(struct tag_message&) {
    return MESSAGE_DISPATCH_CONTINUE;
}

struct _SAMPLE* soundManager::StartSample(char*, char**, i16, i16, i32, i32, i32l) {
    return NULL;
}

void soundManager::StopAllSamples(i32 stopMusic) {
    i16 sampleIdx;
    i32 waitCounter;
    i32 sampleStatus;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == NULL)
        return;
    if (m_samplesReady == 0)
        return;
    LogStr("SAS 1");
    for (sampleIdx = 0; sampleIdx < m_numSampleHandles; sampleIdx++) {
        sampleStatus = AIL_sample_status(m_sampleHandles[sampleIdx]);
        if (sampleStatus == SAMPLE_STATUS_PLAYING)
            AIL_end_sample(m_sampleHandles[sampleIdx]);
    }
    m_fadeSteps = 0;
    if (stopMusic != 0) {
        MIDIStop();
    }
    for (waitCounter = 0; waitCounter < SAMPLE_STOP_ALL_WAIT_COUNT; waitCounter++) {
        ServiceSound();
        DelayMilli(1);
    }
    LogStr("SAS 2");
}

void soundManager::StopSample(struct _SAMPLE* sample) {
    i32 local_c;
    i32 local_10;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == NULL)
        return;
    local_10 = 0;
    LogStr("Stop Sample 1");
    if (m_sampleHandles[0] == sample)
        local_10 = 1;
    AIL_end_sample(sample);
    if (local_10 != 0) {
        for (local_c = 0; local_c < SAMPLE_STOP_MUSIC_WAIT_COUNT; local_c++) {
            ServiceSound();
            DelayMilli(1);
        }
    }
    LogStr("Stop Sample 2");
}

void soundManager::ModifySample(
    struct _SAMPLE* sampleHandle,
    SoundSampleOperation operation,
    i32l value
) {
    i32 foundChannel;
    i32 unused;
    i32 sampleIndex;

    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == NULL)
        return;
    if (m_samplesReady == 0)
        return;
    if (m_ready == 0)
        return;

    LogStr("Modify Sample 1");
    foundChannel = -1;
    for (sampleIndex = 0; sampleIndex < m_numSampleHandles; sampleIndex++) {
        if (m_sampleHandles[sampleIndex] == sampleHandle)
            foundChannel = sampleIndex;
    }

    switch (operation) {
        case SOUND_SAMPLE_OPERATION_VOLUME:
        case SOUND_SAMPLE_OPERATION_EFFECT_VOLUME:
            AIL_set_sample_volume(sampleHandle, ConvertVolume(value, SOUND_VOLUME_EFFECT));
            if (foundChannel >= 0)
                iLastVolume[foundChannel] = static_cast<i16>(value);
            break;
        case SOUND_SAMPLE_OPERATION_MUSIC_VOLUME:
            AIL_set_sample_volume(sampleHandle, ConvertVolume(value, SOUND_VOLUME_MUSIC));
            if (foundChannel >= 0)
                iLastVolume[foundChannel] = static_cast<i16>(value);
            break;
        case SOUND_SAMPLE_OPERATION_START:
            AIL_start_sample(sampleHandle);
            break;
    }

    Process1WindowsMessage();
    LogStr("Modify Sample 2");
}

i32l soundManager::DigitalReport(struct _SAMPLE* sample, SoundDigitalReportQuery reportType) {
    i32 sampleStatus;

    if (gbNoSound != 0)
        return 0;
    if (m_digitalDriver == NULL)
        return 0;
    switch (reportType) {
        case SOUND_DIGITAL_REPORT_VOLUME:
            return AIL_sample_volume(sample);
        case SOUND_DIGITAL_REPORT_PLAYING:
            sampleStatus = AIL_sample_status(sample);
            return sampleStatus == SAMPLE_STATUS_PLAYING;
    }
    return 0;
}

void soundManager::AdjustSoundVolumes(void) {
    i32 sampleIndex;
    struct _SAMPLE* sampleHandle;

    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == NULL)
        return;
    if (m_samplesReady == 0)
        return;

    LogStr("Adjust Sound Volumes 1");
    for (sampleIndex = 1; sampleIndex < m_numSampleHandles; sampleIndex++) {
        sampleHandle = m_sampleHandles[sampleIndex];
        if (gConfig.soundVolume != CONFIG_VOLUME_MUTED) {
            if (DigitalReport(sampleHandle, SOUND_DIGITAL_REPORT_PLAYING) != 0)
                ModifySample(
                    sampleHandle,
                    SOUND_SAMPLE_OPERATION_EFFECT_VOLUME,
                    iLastVolume[sampleIndex]
                );
        } else {
            ModifySample(sampleHandle, SOUND_SAMPLE_OPERATION_VOLUME, 0);
        }
    }
    LogStr("Adjust Sound Volumes 2");
}

void soundManager::AdjustMusicVolumes(void) {
    if (gbNoSound != 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (m_currentTrack < 0)
        return;
    LogStr("Adjust Music Volumes 1");
    MIDISetVolume();
    LogStr("Adjust Music Volumes 2");
}

void soundManager::ForcePollSound(void) {
    if (gbNoSound != 0)
        return;
    m_pollRequested = 1;
    PollSound();
}

void soundManager::PlayAmbientMusic(i32 track, i32l, i32) {
    if (gbNoSound != 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (m_ready == 0)
        return;
    if (m_currentTrack == track)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED) {
        m_currentTrack = static_cast<char>(track);
        return;
    }
    MIDIPlay(track);
    m_currentTrack = static_cast<char>(track);
}

void soundManager::PollSound(void) {
    i32l delta;
    i32l now;
    if (gbNoSound != 0)
        return;
    if (m_pollRequested == 0 && m_fadeSteps == 0)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    LogStr("Poll Sound 1");
    if (m_fadeSteps > 0) {
        LogStr("Poll Sound 1a");
        Process1WindowsMessage();
        glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount();
        delta = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - KBTickCount();
        m_fadeSteps = delta / FADE_STEP_TICKS;
        if (m_fadeSteps < 1)
            m_fadeSteps = 0;
        LogStr("Poll Sound 1b");
        if (m_fadeSteps <= FADE_HOLD_STEPS && m_currentTrack != m_fadeTargetTrack) {
            if (m_midiFile != NULL && bSaveMusicPosition[m_currentTrack] != 0) {
                H2_ASSERT(reinterpret_cast<i32>(m_midiFile));
                m_savedTrackPositions[m_currentTrack] = ftell(m_midiFile);
            } else {
                glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount();
            }
            m_fading = 1;
            if (bSaveMusicPosition[m_fadeTargetTrack] != 0)
                PlayAmbientMusic(m_fadeTargetTrack, m_savedTrackPositions[m_fadeTargetTrack], -1);
            else
                PlayAmbientMusic(m_fadeTargetTrack, 0, -1);
            now = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - KBTickCount();
            m_fadeSteps = now / FADE_STEP_TICKS;
            if (m_fadeSteps < 1)
                m_fadeSteps = 0;
            m_currentTrack = static_cast<char>(m_fadeTargetTrack);
        }
        LogStr("Poll Sound 1c");
        MIDISetVolume();
        LogStr("Poll Sound 1d");
    }
    LogStr("Poll Sound 2");
    m_pollRequested = 0;
}

void soundManager::SwitchAmbientMusic(i32 track) {
    if (gbNoSound != 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED) {
        m_currentTrack = static_cast<char>(track);
        return;
    }
    if (MusicPlaying() == 0) {
        PlayAmbientMusic(track, 0, -1);
        return;
    }
    if (m_currentTrack == track)
        return;
    LogStr("Switch Ambient Music 1");
    Process1WindowsMessage();
    if ((m_fadeSteps != 0 && m_fadeTargetTrack != track)
        || (m_fadeSteps == 0 && m_currentTrack != track)) {
        if (m_fadeSteps <= FADE_HOLD_STEPS) {
            m_fadeSteps = FADE_TOTAL_STEPS;
            glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] =
                KBTickCount() + AMBIENT_FADE_DELAY_TICKS;
        }
        m_fadeTargetTrack = track;
        PollSound();
    }
    LogStr("Switch Ambient Music 2");
}

struct _SAMPLE* soundManager::MemorySample(class sample* sampleResource) {
    struct _SAMPLE* smp;
    i16 ch;
    SampleChannelStruct* scs;
    SamplePlaybackData* playbackData;
    if (gbNoSound != 0)
        return NULL;
    if (m_digitalDriver == NULL)
        return NULL;
    if (m_samplesReady == 0)
        return NULL;
    if (gConfig.soundVolume == CONFIG_VOLUME_MUTED)
        return NULL;
    playbackData = &sampleResource->m_playbackData;
    if (m_ready == 0 || playbackData->volume == 0)
        return NULL;
    LogStr("Memory Sample 1");

    scs = &SCS[playbackData->channelType];
    for (ch = static_cast<i16>(scs->startChannel); scs->endChannel > ch; ch++) {
        if (AIL_sample_status(m_sampleHandles[ch]) == SAMPLE_STATUS_DONE)
            break;
    }
    if (scs->endChannel == ch) {
        if (playbackData->channelType == NO_SAMPLE_CHANNEL_TYPE) {
            LogStr("Memory Sample 2a");
            return NULL;
        }
        ch = static_cast<i16>(scs->currentChannel);
        scs->currentChannel++;
        if (scs->endChannel <= scs->currentChannel) {
            scs->currentChannel = scs->startChannel;
            ch = static_cast<i16>(scs->currentChannel);
        }
        StopSample(m_sampleHandles[ch]);
    }
    smp = m_sampleHandles[ch];
    m_channelVolumes[ch] = static_cast<char>(playbackData->volume);
    iLastVolume[ch] = static_cast<i16>(playbackData->volume);
    AIL_init_sample(smp);
    AIL_set_sample_type(smp, H2EnumIndex(playbackData->format), 0);
    AIL_set_sample_playback_rate(smp, H2EnumIndex(playbackData->sampleRate));
    AIL_set_sample_loop_count(smp, playbackData->loopCount);
    AIL_set_sample_address(smp, playbackData->data, playbackData->size);
    if (gConfig.soundVolume != CONFIG_VOLUME_MUTED)
        AIL_set_sample_volume(smp, ConvertVolume(playbackData->volume, SOUND_VOLUME_EFFECT));
    else
        AIL_set_sample_volume(smp, 0);
    AIL_start_sample(smp);
    playbackData->activeSample = smp;
    m_channelSamples[ch] = smp;
    m_channelSampleData[ch] = playbackData->data;
    m_channelSampleSizes[ch] = playbackData->size;
    LogStr("Memory Sample 2b");
    return smp;
}

void soundManager::ServiceSound(void) {
    if (gbNoSound != 0)
        return;
    AIL_serve();
}

i32 soundManager::MusicPlaying(void) {
    if (gbNoSound != 0)
        return 0;
    if (m_midiReady == 0)
        return 0;
    return MIDIIsPlaying();
}


char* digitalDriverNames[DIGITAL_DRIVER_NAME_COUNT] = {
    "",
    "ultra.dig",
    "sndsys.dig",
    "sndscape.dig",
    "jammer.dig",
    "audiodrv.dig",
    "proaudio.dig",
    "rap10.dig",
    "iwav.dig",
    "nvdig.dig",
    "sb16.dig",
    "sbpro.dig",
    "sblaster.dig",
    NULL
};
SampleChannelStruct SCS[SOUND_CHANNEL_TYPE_COUNT] = {{0, 1, 0}, {1, 2, 1}, {2, 6, 2}, {6, 16, 6}};
i32 iCalibrateLoop = 0;
i16 iLastVolume[SAVED_SAMPLE_VOLUME_CAPACITY];
