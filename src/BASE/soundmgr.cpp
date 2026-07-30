#include <Ints.h>
#include <BASE/soundManager.h>
#include <BASE/sample.h>
#include <BASE/soundBackends.h>
#include <BASE/Midi.h>
#include <BASE/MusicFlags.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/KB.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/NOOPT.h>
#include <PLATFORM/Miles.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <BASE/Misc.h>
#include <audiere.h>

typedef enum SoundConstant {
    SAMPLE_VOLUME_MAX            = 0x40,
    MIDI_VOLUME_MAX              = 0x7f,
    CD_VOLUME_SCALE_DIVISOR      = 0x280,
    CD_MUSIC_TRACK_FIRST         = 8,
    CD_MUSIC_TRACK_LAST          = 15,
    FADE_HOLD_STEPS              = 10,
    FADE_TOTAL_STEPS             = H2EnumIndex(CONFIG_VOLUME_MAX) + 1,
    FADE_SAMPLE_RISE_STEPS       = 6,
    AMBIENT_FADE_DELAY_TICKS     = 900,
    AIL_WAVEOUT_PREFERENCE       = 15,
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


#define NORMALIZED_VOLUME_MAX 127.0f

bool gSoundDisabled = false;
bool gSoundBackendsReady = false;

void soundManager::ShutdownSoundBackends(void) {
    if (IsMilesBackend(this)) {
        MIDIShutdown();
        StopAllMilesSamples();
        AIL_shutdown();
        m_digitalDriver = NULL;
    } else if (IsAudiereBackend(this)) {
        StopAudiereMusic(m_musicTrack);
        StopAllAudiereSamples();
        m_audiereDevice = NULL;
    }
    m_backend = SOUND_BACKEND_NONE;
}

bool soundManager::StartupMilesBackend(void) {
    if (m_backend == SOUND_BACKEND_MILES)
        return true;

    ShutdownSoundBackends();
    if (gSoundDisabled)
        return false;

    m_backend = SOUND_BACKEND_MILES;
    AIL_startup();
    AIL_set_preference(AIL_WAVEOUT_PREFERENCE, 1);

    if (AIL_waveOutOpen(&m_digitalDriver, NULL, 0, NULL) != 0) {
        platform::ShowMessage(
            "\xce\xf8\xe8\xe1\xea\xe0 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xe8 "
                "\xe7\xe2\xf3\xea\xe0!",
            AIL_last_error()
        );
        m_digitalDriver = NULL;
        AIL_shutdown();
        return false;
    }

    StartupMilesSamples(m_digitalDriver);
    gSoundBackendsReady = true;
    return MIDIStartup();
}

bool soundManager::CDStartup(void) {
    if (m_backend == SOUND_BACKEND_AUDIERE)
        return true;

    ShutdownSoundBackends();
    if (gSoundDisabled)
        return false;

    m_backend = SOUND_BACKEND_AUDIERE;
    m_audiereDevice = audiere::OpenDevice();
    if (m_audiereDevice != NULL)
        return StartupAudiereMusic(m_audiereDevice);
    return false;
}

i32 soundManager::ConvertVolume(i32 volume, SoundVolumeConversionMode soundType) {
    i32 local_8 = 0;
    if (soundType == SOUND_VOLUME_MUSIC) {
        if (gConfig.musicVolume >= CONFIG_VOLUME_MIN
            && gConfig.musicVolume <= CONFIG_VOLUME_MAX) {
            local_8 = (volume * (FADE_TOTAL_STEPS - H2EnumIndex(gConfig.musicVolume)))
                      / H2EnumIndex(CONFIG_VOLUME_MAX);
            if (local_8 < 1)
                local_8 = 1;
        }
    } else if (gConfig.soundVolume >= CONFIG_VOLUME_MIN
               && gConfig.soundVolume <= CONFIG_VOLUME_MAX) {
        local_8 =
            (volume * (FADE_TOTAL_STEPS - H2EnumIndex(gConfig.soundVolume))) / H2EnumIndex(CONFIG_VOLUME_MAX);
        if (local_8 < 1)
            local_8 = 1;
    }
    if (local_8 < 0)
        local_8 = 0;
    if (MIDI_VOLUME_MAX < local_8)
        local_8 = MIDI_VOLUME_MAX;
    return local_8;
}

float soundManager::ConvertVolumeFloat(i32 volume, SoundVolumeConversionMode soundType) {
    return static_cast<float>(ConvertVolume(volume, soundType)) / NORMALIZED_VOLUME_MAX;
}

#undef NORMALIZED_VOLUME_MAX

soundManager::soundManager(void) : baseManager() {
    m_backend = SOUND_BACKEND_NONE;
    m_savedBackend = SOUND_BACKEND_NONE;
    m_active = false;
    gSoundBackendsReady = false;
    m_digitalDriver = NULL;
    m_audiereDevice = NULL;
    m_musicFadeTargetTrack = MIDI_NO_TRACK;
    m_musicFadeSteps = 0;
    m_musicTrack = MIDI_NO_TRACK;
}

i32 soundManager::Open(i32) {
    i32 musicTrack;

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

    if (platform::Input().IsKeyDown(platform::Key::F6)) {
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
        WritePrefs();
    }
    if (platform::Input().IsKeyDown(platform::Key::F7)) {
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
        WritePrefs();
    }

    m_musicTrack = MIDI_NO_TRACK;
    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI) {
        if (!CDStartup()) {
            StartupMilesBackend();
            if (MusicFlagsActive()) {
                gConfig.musicVolume = CONFIG_VOLUME_MUTED;
                WritePrefs();
            } else {
                gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
                WritePrefs();
            }
        }
    } else {
        StartupMilesBackend();
        if (MusicFlagsActive()) {
            if (CDStartup()) {
                gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
                WritePrefs();
            } else {
                gConfig.musicVolume = CONFIG_VOLUME_MUTED;
                WritePrefs();
            }
        }
    }

    if (!IsAudiereBackend(this) && !IsMilesBackend(this)) {
        gConfig.soundVolume = CONFIG_VOLUME_MUTED;
        WritePrefs();
    }

    m_messageMask = BASE_MANAGER_ACCEPT_LEFT_BUTTON_UP;
    m_priority = SOUND_MANAGER_PRIORITY;
    m_active = true;
    strcpy(m_name, "soundManager");
    return 0;
}

void soundManager::Close(void) {
    if (m_active != 1)
        return;
    ShutdownSoundBackends();
    m_active = false;
}

MessageDispatchResult soundManager::Main(struct tag_message&) {
    return MESSAGE_DISPATCH_CONTINUE;
}

void soundManager::StopAllSamples(i32 stopMusic) {
    if (!gSoundBackendsReady)
        return;

    if (IsAudiereBackend(this)) {
        m_musicFadeSteps = 0;
        if (stopMusic != 0)
            StopAudiereMusic(m_musicTrack);
        StopAllAudiereSamples();
        return;
    }
    if (IsMilesBackend(this)) {
        m_musicFadeSteps = 0;
        if (stopMusic != 0)
            MIDIStop(m_musicTrack);
        StopAllMilesSamples();
    }
}

void soundManager::StopSample(class sample* sampleResource) {
    if (IsAudiereBackend(this)) {
        StopAudiereSample(sampleResource);
    } else if (IsMilesBackend(this)) {
        StopMilesSample(sampleResource);
    }
}

void soundManager::ModifySample(
    class sample* sampleResource,
    i32 volume
) {
    if (!gSoundBackendsReady)
        return;
    if (IsAudiereBackend(this)) {
        SetAudiereSampleVolume(sampleResource, volume);
    } else if (IsMilesBackend(this)) {
        SetMilesSampleVolume(sampleResource, volume);
    }
}

bool soundManager::DigitalReport(class sample* sampleResource) {
    if (IsAudiereBackend(this)) {
        return AudiereSamplePlaying(sampleResource);
    }
    if (IsMilesBackend(this))
        return MilesSamplePlaying(sampleResource);
    return false;
}

void soundManager::AdjustSoundVolumes(void) {
    if (!gSoundBackendsReady)
        return;
    if (IsMilesBackend(this))
        AdjustMilesSampleVolumes();
}

void soundManager::AdjustMusicVolumes(void) {
    if (!gSoundBackendsReady)
        return;
    if (m_musicTrack < 0)
        return;
    if (IsAudiereBackend(this))
        SetAudiereMusicVolume(-1, false);
    else if (IsMilesBackend(this))
        MIDISetVolume(m_musicFadeSteps);
}

void soundManager::SetMusicQuality(i32 musicSource) {
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;

    i32 previousTrack;
    if (IsAudiereBackend(this)) {
        previousTrack = m_musicTrack;
        StopAudiereMusic(m_musicTrack);
        m_musicTrack = MIDI_NO_TRACK;
    } else if (IsMilesBackend(this)) {
        previousTrack = m_musicTrack;
        MIDIStop(m_musicTrack);
    }

    gConfig.musicSource = ConfigMusicSource(musicSource);
    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI)
        CDStartup();
    else
        StartupMilesBackend();
    if (previousTrack >= 0)
        PlayAmbientMusic(previousTrack);
}

void soundManager::PlayAmbientMusic(i32 track) {
    if (!gSoundBackendsReady)
        return;
    if (m_musicTrack == track)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED) {
        m_musicTrack = track;
        return;
    }

    if (IsAudiereBackend(this)) {
        PlayAudiereMusic(
            m_audiereDevice,
            m_musicTrack,
            m_musicFadeSteps,
            track
        );
    } else if (IsMilesBackend(this)) {
        MIDIPlay(m_musicTrack, m_musicFadeSteps, track);
    }
    m_musicTrack = track;
}

void soundManager::PollSound(void) {
    i32 musicFadeStep;
    i32 volume;
    i32l delta;
    i32l switchDelta;
    if (m_musicFadeSteps == 0)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    if (m_musicFadeSteps > 0) {
        platform::PumpEvents();
        if (m_musicTrack < CD_MUSIC_TRACK_FIRST
            || m_musicTrack > CD_MUSIC_TRACK_LAST)
            glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = platform::Ticks();
        delta = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - platform::Ticks();
        m_musicFadeSteps = delta / FADE_STEP_TICKS;
        if (m_musicFadeSteps < 1)
            m_musicFadeSteps = 0;

        if (m_musicFadeSteps <= FADE_HOLD_STEPS
            && m_musicTrack != m_musicFadeTargetTrack) {
            if (bSaveMusicPosition[m_musicTrack] == 0)
                glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = platform::Ticks();
            PlayAmbientMusic(m_musicFadeTargetTrack);
            switchDelta = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - platform::Ticks();
            m_musicFadeSteps = switchDelta / FADE_STEP_TICKS;
            if (m_musicFadeSteps < 1)
                m_musicFadeSteps = 0;
            m_musicTrack = m_musicFadeTargetTrack;
        }

        musicFadeStep = m_musicFadeSteps;
        if (m_musicFadeSteps <= FADE_HOLD_STEPS)
            volume = (FADE_TOTAL_STEPS - m_musicFadeSteps)
                   * SAMPLE_VOLUME_MAX / FADE_TOTAL_STEPS;
        else
            volume = (m_musicFadeSteps - FADE_HOLD_STEPS)
                   * SAMPLE_VOLUME_MAX / FADE_SAMPLE_RISE_STEPS;
        if (volume > SAMPLE_VOLUME_MAX)
            volume = SAMPLE_VOLUME_MAX;
        if (volume < 0)
            volume = 0;

        if (IsAudiereBackend(this)) {
            volume = (FADE_TOTAL_STEPS - H2EnumIndex(gConfig.musicVolume)) * volume
                   * MIDI_VOLUME_MAX / CD_VOLUME_SCALE_DIVISOR;
            if (volume > MIDI_VOLUME_MAX)
                volume = MIDI_VOLUME_MAX;
            if (volume < 0)
                volume = 0;
            SetAudiereMusicVolume(volume, true);
        } else if (IsMilesBackend(this)) {
            MIDISetVolume(m_musicFadeSteps);
        }
    }
}

void soundManager::SwitchAmbientMusic(i32 track) {
    if (!gSoundBackendsReady)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED) {
        m_musicTrack = track;
        return;
    }
    if (MusicPlaying() == 0) {
        PlayAmbientMusic(track);
        return;
    }
    if (m_musicTrack == track)
        return;
    platform::PumpEvents();
    if ((m_musicFadeSteps != 0
         && track != m_musicFadeTargetTrack)
        || (m_musicFadeSteps == 0
            && track != m_musicTrack)) {
        if (m_musicFadeSteps <= FADE_HOLD_STEPS) {
            m_musicFadeSteps = FADE_TOTAL_STEPS;
            glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] =
                platform::Ticks() + AMBIENT_FADE_DELAY_TICKS;
        }
        m_musicFadeTargetTrack = track;
        PollSound();
    }
}

void soundManager::MemorySample(class sample* sampleResource) {
    if (sampleResource == NULL)
        return;
    if (!gSoundBackendsReady)
        return;
    if (gConfig.soundVolume == CONFIG_VOLUME_MUTED)
        return;
    if (IsAudiereBackend(this)) {
        PlayAudiereSample(sampleResource, m_audiereDevice);
    } else if (IsMilesBackend(this)) {
        PlayMilesSample(sampleResource);
    }
}

void soundManager::ServiceSound(void) {
    if (IsMilesBackend(this))
        ServiceMilesSamples();
}

i32 soundManager::MusicPlaying(void) {
    if (m_backend == SOUND_BACKEND_AUDIERE)
        return AudiereMusicPlaying();
    if (m_backend == SOUND_BACKEND_MILES) {
        if (MusicFlagsActive())
            return false;
        return MIDIIsPlaying();
    }
    return false;
}
