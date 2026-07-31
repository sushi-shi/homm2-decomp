#include <Ints.h>
#include <BASE/Midi.h>
#include <BASE/Misc.h>
#include <BASE/MusicFlags.h>
#include <BASE/sample.h>
#include <BASE/soundManager.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <algorithm>
#include <string.h>
#include <vector>

typedef enum SoundConstant {
    SAMPLE_VOLUME_MAX          = 0x40,
    MIDI_VOLUME_MAX            = 0x7f,
    CD_VOLUME_SCALE_DIVISOR    = 0x280,
    CD_MUSIC_TRACK_FIRST       = 8,
    CD_MUSIC_TRACK_LAST        = 15,
    FADE_HOLD_STEPS            = 10,
    FADE_TOTAL_STEPS           = H2EnumIndex(CONFIG_VOLUME_MAX) + 1,
    FADE_SAMPLE_RISE_STEPS     = 6,
    AMBIENT_FADE_DELAY_TICKS   = 900,
    FADE_STEP_TICKS            = 60
} SoundConstant;

bool gSoundDisabled = false;
bool gSoundBackendsReady = false;

namespace {

std::vector<sample*> gActiveSamples;

void PurgeFinishedSamples(void) {
    auto finished = std::remove_if(
        gActiveSamples.begin(),
        gActiveSamples.end(),
        [](sample* sampleResource) {
            if (sampleResource == NULL || sampleResource->m_playbackData.activeSample == 0)
                return true;
            if (platform::Audio().IsVoicePlaying(sampleResource->m_playbackData.activeSample))
                return false;
            sampleResource->m_playbackData.activeSample = 0;
            return true;
        }
    );
    gActiveSamples.erase(finished, gActiveSamples.end());
}

bool OpenAudioBackend(SoundBackendKind backend) {
    if (gSoundDisabled)
        return false;
    if (!platform::Audio().Open())
        return false;
    gpSoundManager->m_backend = backend;
    gSoundBackendsReady = true;
    return true;
}

}

void soundManager::ShutdownSoundBackends(void) {
    if (m_backend == SOUND_BACKEND_NONE)
        return;
    StopAllSamples(1);
    MIDIShutdown();
    platform::Audio().Close();
    gActiveSamples.clear();
    gSoundBackendsReady = false;
    m_backend = SOUND_BACKEND_NONE;
}

bool soundManager::StartupMilesBackend(void) {
    if (m_backend == SOUND_BACKEND_AUDIO_MIDI)
        return true;
    ShutdownSoundBackends();
    if (!OpenAudioBackend(SOUND_BACKEND_AUDIO_MIDI))
        return false;
    MIDIStartup();
    return true;
}

bool soundManager::CDStartup(void) {
    if (m_backend == SOUND_BACKEND_AUDIO_CD)
        return true;
    ShutdownSoundBackends();
    if (!OpenAudioBackend(SOUND_BACKEND_AUDIO_CD))
        return false;
    MIDIStartup();
    return true;
}

i32 soundManager::ConvertVolume(i32 volume, SoundVolumeConversionMode soundType) {
    i32 converted = 0;
    if (soundType == SOUND_VOLUME_MUSIC) {
        if (gConfig.musicVolume >= CONFIG_VOLUME_MIN
            && gConfig.musicVolume <= CONFIG_VOLUME_MAX) {
            converted = (volume * (FADE_TOTAL_STEPS - H2EnumIndex(gConfig.musicVolume)))
                      / H2EnumIndex(CONFIG_VOLUME_MAX);
            if (converted < 1)
                converted = 1;
        }
    } else if (gConfig.soundVolume >= CONFIG_VOLUME_MIN
               && gConfig.soundVolume <= CONFIG_VOLUME_MAX) {
        converted = (volume * (FADE_TOTAL_STEPS - H2EnumIndex(gConfig.soundVolume)))
                  / H2EnumIndex(CONFIG_VOLUME_MAX);
        if (converted < 1)
            converted = 1;
    }
    if (converted < 0)
        converted = 0;
    if (converted > MIDI_VOLUME_MAX)
        converted = MIDI_VOLUME_MAX;
    return converted;
}

soundManager::soundManager(void) : baseManager() {
    m_backend = SOUND_BACKEND_NONE;
    m_savedBackend = SOUND_BACKEND_NONE;
    m_active = false;
    m_musicFadeTargetTrack = MIDI_NO_TRACK;
    m_musicFadeSteps = 0;
    m_musicTrack = MIDI_NO_TRACK;
}

i32 soundManager::Open(i32) {
    memset(bSaveMusicPosition, 0, MIDI_TRACK_COUNT);
    memset(bMusicIsLooping, 0, MIDI_TRACK_COUNT);

    const i32 savedTracks[] = {16, 18, 14, 15, 11, 13, 17, 12, 28, 42};
    for (i32 track : savedTracks)
        bSaveMusicPosition[track] = 1;
    const i32 loopingTracks[] = {
        16, 18, 14, 15, 11, 13, 17, 12, 8, 9, 5, 6, 10, 7, 42, 28, 43, 22, 24
    };
    for (i32 track : loopingTracks)
        bMusicIsLooping[track] = 1;
    for (i32 track = 2; track <= 4; ++track)
        bMusicIsLooping[track] = 1;

    if (platform::Input().IsKeyDown(platform::Key::F6)) {
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
        WritePrefs();
    }
    if (platform::Input().IsKeyDown(platform::Key::F7)) {
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
        WritePrefs();
    }

    m_musicTrack = MIDI_NO_TRACK;
    const bool opened = gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI
        ? StartupMilesBackend()
        : CDStartup();
    if (!opened) {
        gConfig.musicVolume = CONFIG_VOLUME_MUTED;
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
    m_musicFadeSteps = 0;
    if (stopMusic != 0)
        MIDIStop(m_musicTrack);
    for (sample* sampleResource : gActiveSamples) {
        if (sampleResource != NULL && sampleResource->m_playbackData.activeSample != 0) {
            platform::Audio().StopVoice(sampleResource->m_playbackData.activeSample);
            sampleResource->m_playbackData.activeSample = 0;
        }
    }
    gActiveSamples.clear();
}

void soundManager::StopSample(class sample* sampleResource) {
    if (sampleResource == NULL || sampleResource->m_playbackData.activeSample == 0)
        return;
    platform::Audio().StopVoice(sampleResource->m_playbackData.activeSample);
    sampleResource->m_playbackData.activeSample = 0;
    PurgeFinishedSamples();
}

void soundManager::ModifySample(class sample* sampleResource, i32 volume) {
    if (!gSoundBackendsReady || sampleResource == NULL)
        return;
    platform::Audio().SetVoiceVolume(
        sampleResource->m_playbackData.activeSample,
        ConvertVolume(volume, SOUND_VOLUME_EFFECT)
    );
}

bool soundManager::DigitalReport(class sample* sampleResource) {
    return gSoundBackendsReady && sampleResource != NULL
        && platform::Audio().IsVoicePlaying(sampleResource->m_playbackData.activeSample);
}

void soundManager::AdjustSoundVolumes(void) {
    if (!gSoundBackendsReady)
        return;
    PurgeFinishedSamples();
    for (sample* sampleResource : gActiveSamples)
        ModifySample(sampleResource, sampleResource->m_playbackData.volume);
}

void soundManager::AdjustMusicVolumes(void) {
    if (gSoundBackendsReady && m_musicTrack >= 0)
        MIDISetVolume(m_musicFadeSteps);
}

void soundManager::SetMusicQuality(i32 musicSource) {
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    const i32 previousTrack = m_musicTrack;
    MIDIStop(m_musicTrack);
    gConfig.musicSource = ConfigMusicSource(musicSource);
    if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
        StartupMilesBackend();
    else
        CDStartup();
    if (previousTrack >= 0)
        PlayAmbientMusic(previousTrack);
}

void soundManager::PlayAmbientMusic(i32 track) {
    if (!gSoundBackendsReady || m_musicTrack == track)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED) {
        m_musicTrack = track;
        return;
    }
    MIDIPlay(m_musicTrack, m_musicFadeSteps, track);
    if (CurrentMidiFile == track)
        m_musicTrack = track;
}

void soundManager::PollSound(void) {
    if (m_musicFadeSteps == 0 || gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    platform::PumpEvents();
    if (m_musicTrack < CD_MUSIC_TRACK_FIRST || m_musicTrack > CD_MUSIC_TRACK_LAST)
        glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = platform::Ticks();
    i32l delta = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - platform::Ticks();
    m_musicFadeSteps = delta / FADE_STEP_TICKS;
    if (m_musicFadeSteps < 1)
        m_musicFadeSteps = 0;

    if (m_musicFadeSteps <= FADE_HOLD_STEPS && m_musicTrack != m_musicFadeTargetTrack) {
        if (m_musicTrack < 0 || bSaveMusicPosition[m_musicTrack] == 0)
            glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = platform::Ticks();
        PlayAmbientMusic(m_musicFadeTargetTrack);
        delta = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - platform::Ticks();
        m_musicFadeSteps = delta / FADE_STEP_TICKS;
        if (m_musicFadeSteps < 1)
            m_musicFadeSteps = 0;
        m_musicTrack = m_musicFadeTargetTrack;
    }

    i32 volume;
    if (m_musicFadeSteps <= FADE_HOLD_STEPS)
        volume = (FADE_TOTAL_STEPS - m_musicFadeSteps) * SAMPLE_VOLUME_MAX / FADE_TOTAL_STEPS;
    else
        volume = (m_musicFadeSteps - FADE_HOLD_STEPS) * SAMPLE_VOLUME_MAX / FADE_SAMPLE_RISE_STEPS;
    volume = std::clamp(volume, 0, H2EnumIndex(SAMPLE_VOLUME_MAX));
    volume = (FADE_TOTAL_STEPS - H2EnumIndex(gConfig.musicVolume)) * volume
           * MIDI_VOLUME_MAX / CD_VOLUME_SCALE_DIVISOR;
    platform::Audio().SetMusicVolume(std::clamp(volume, 0, H2EnumIndex(MIDI_VOLUME_MAX)));
}

void soundManager::SwitchAmbientMusic(i32 track) {
    if (!gSoundBackendsReady)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED) {
        m_musicTrack = track;
        return;
    }
    if (!MusicPlaying()) {
        PlayAmbientMusic(track);
        return;
    }
    if (m_musicTrack == track)
        return;
    platform::PumpEvents();
    if ((m_musicFadeSteps != 0 && track != m_musicFadeTargetTrack)
        || (m_musicFadeSteps == 0 && track != m_musicTrack)) {
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
    if (sampleResource == NULL || !gSoundBackendsReady
        || gConfig.soundVolume == CONFIG_VOLUME_MUTED
        || sampleResource->m_playbackData.volume == 0)
        return;
    if (platform::Audio().IsVoicePlaying(sampleResource->m_playbackData.activeSample))
        return;

    PurgeFinishedSamples();
    SamplePlaybackData& playback = sampleResource->m_playbackData;
    platform::SoundData sound;
    sound.samples = playback.data;
    sound.byteCount = playback.size;
    sound.sampleRate = H2EnumIndex(playback.sampleRate);
    sound.channels = playback.stereo != 0 ? 2 : 1;
    sound.bitsPerSample = playback.sampleFormat == FORMAT_16_BIT ? 16 : 8;
    const i32 loops = playback.loopCount != 0 ? -1 : 0;
    playback.activeSample = platform::Audio().PlaySound(
        sound,
        ConvertVolume(playback.volume, SOUND_VOLUME_EFFECT),
        loops
    );
    if (playback.activeSample != 0)
        gActiveSamples.push_back(sampleResource);
}

void soundManager::ServiceSound(void) {
    if (!gSoundBackendsReady)
        return;
    platform::Audio().Service();
    PurgeFinishedSamples();
}

i32 soundManager::MusicPlaying(void) {
    return gSoundBackendsReady && MIDIIsPlaying();
}

SampleChannelStruct SCS[SOUND_CHANNEL_TYPE_COUNT] = {
    {0, 1, 0}, {1, 2, 1}, {2, 6, 2}, {6, 16, 6}
};
