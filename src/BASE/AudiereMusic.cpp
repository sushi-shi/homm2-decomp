#include <va.h>
#include <BASE/soundBackends.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <stdio.h>

H2_ENUM_BEGIN(AudiereMusicConstant)
    AUDIERE_MUSIC_VOLUME_LEVEL_COUNT = 12,
    AUDIERE_FADE_VOLUME_DIVISOR      = 12,
    AUDIERE_FADE_VOLUME_LIMIT        = 121,
    AUDIERE_FADE_START_LEVEL         = 10,
    AUDIERE_FADE_STEP_COUNT          = 11,
    AUDIERE_FADE_DELAY               = 480
H2_ENUM_END(AudiereMusicConstant)

DATA(0x00520130) static float gAudiereMusicVolume = 1.0f;
DATA(0x00520134) static float gAudiereMusicVolumes[AUDIERE_MUSIC_VOLUME_LEVEL_COUNT] = {
    0.0f,
    1.0f,
    0.8f,
    0.65f,
    0.5f,
    0.4f,
    0.3f,
    0.2f,
    0.15f,
    0.1f,
    0.05f,
    0.0f
};
DATA(0x005395e0) audiere::OutputStreamPtr AudiereMusicState::stream;
DATA(0x005395dc) audiere::SampleSourcePtr AudiereMusicState::source;
DATA_COMPGEN_GUARD(
    0x005395e4, audiereMusicStateInitializationGuard, AudiereMusicState
)
DATA(0x005395e8) static i32 gAudiereMusicPositions[MIDI_TRACK_COUNT];

VA_COMPGEN(0x004cd080, 0xf, STATIC_INIT_DISPATCH, AudiereMusicState::stream)
VA_COMPGEN(0x004cd090, 0x61, STATIC_CTOR, AudiereMusicState::stream)
VA_COMPGEN(0x004cd100, 0x12, STATIC_ATEXIT, AudiereMusicState::stream)
VA_COMPGEN(0x004cd120, 0x46, STATIC_DTOR, AudiereMusicState::stream)
VA_COMPGEN(0x004cd170, 0xf, STATIC_INIT_DISPATCH, AudiereMusicState::source)
VA_COMPGEN(0x004cd180, 0x61, STATIC_CTOR, AudiereMusicState::source)
VA_COMPGEN(0x004cd1f0, 0x12, STATIC_ATEXIT, AudiereMusicState::source)
VA_COMPGEN(0x004cd210, 0x46, STATIC_DTOR, AudiereMusicState::source)

VA(0x004cd260, 0x133)
void StopAudiereMusic(i32& currentTrack) {
    if (AudiereMusicState::stream) {
        if (AudiereMusicState::stream->isPlaying()) {
            gAudiereMusicPositions[currentTrack] = AudiereMusicState::stream->getPosition();
            AudiereMusicState::stream->stop();
        }
        AudiereMusicState::stream = NULL;
    }
    if (AudiereMusicState::source)
        AudiereMusicState::source = NULL;
    currentTrack = MIDI_NO_TRACK;
}

VA(0x004cd3a0, 0x7)
bool AudiereMusicAvailable(void) {
    return true;
}

VA(0x004cd3b0, 0x36)
bool AudiereMusicPlaying(void) {
    if (!AudiereMusicState::stream)
        return false;
    return AudiereMusicState::stream->isPlaying();
}

VA(0x004cd3f0, 0x99)
bool StartupAudiereMusic(audiere::AudioDevicePtr device) {
    ResetAudiereMusic();
    if (device == NULL)
        return false;
    return true;
}

VA(0x004cd490, 0x132)
void ResetAudiereMusic(void) {
    if (AudiereMusicState::stream) {
        if (AudiereMusicState::stream->isPlaying())
            AudiereMusicState::stream->stop();
        AudiereMusicState::stream = NULL;
    }
    if (AudiereMusicState::source)
        AudiereMusicState::source = NULL;
    for (i32 track = 0; track < MIDI_TRACK_COUNT; ++track)
        gAudiereMusicPositions[track] = 0;
}

VA(0x004cd5d0, 0xad)
void SetAudiereMusicVolume(i32 volume, i32 fading) {
    i32 volumeLevel;
    if (volume == -1) {
        volumeLevel = IDX(gConfig.musicVolume);
    } else if (fading != 0) {
        if (volume > AUDIERE_FADE_VOLUME_LIMIT - 1)
            volumeLevel = 1;
        else
            volumeLevel = AUDIERE_FADE_STEP_COUNT
                        - volume / AUDIERE_FADE_VOLUME_DIVISOR;
    } else {
        volumeLevel = volume;
    }
    if (volumeLevel < 0)
        volumeLevel = 0;
    if (volumeLevel > AUDIERE_FADE_STEP_COUNT)
        volumeLevel = AUDIERE_FADE_STEP_COUNT;
    gAudiereMusicVolume = gAudiereMusicVolumes[volumeLevel];
    if (AudiereMusicState::stream)
        AudiereMusicState::stream->setVolume(gAudiereMusicVolume);
}

VA(0x004cd680, 0x3c7)
void PlayAudiereMusic(
    audiere::AudioDevicePtr device,
    i32& currentTrack,
    i32& fadeSteps,
    i32 track
) {
    char filename[0x160];
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    if (track == MIDI_NO_TRACK) {
        StopAudiereMusic(currentTrack);
        return;
    }
    if (track == currentTrack && AudiereMusicPlaying())
        return;

    StopAudiereMusic(currentTrack);
    sprintf(
        filename,
        "%s%s%02d-AudioTrack %02d.ogg",
        gcRegCDRomPath,
        gcMusicPath,
        track,
        track
    );
    audiere::SampleSourcePtr source = audiere::OpenSampleSource(filename);
    if (source && device) {
        audiere::OutputStreamPtr stream = device->openStream(source.get());
        if (stream) {
            i32 repeatMusic = bMusicIsLooping[track];
            stream->setRepeat(repeatMusic != 0 ? true : false);
            stream->setVolume(gAudiereMusicVolume);
            stream->play();
            AudiereMusicState::source = source;
            AudiereMusicState::stream = stream;
        }
    }
    if (fadeSteps > 0) {
        fadeSteps = AUDIERE_FADE_STEP_COUNT;
        glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount() + AUDIERE_FADE_DELAY;
        SetAudiereMusicVolume(AUDIERE_FADE_START_LEVEL, false);
    } else {
        SetAudiereMusicVolume(-1, false);
    }
    currentTrack = track;
}

VA_COMPGEN(0x004cdae0, 0x27, LOCALE_FACET_ID_INIT, WCharCtypeId)
VA_COMPGEN(0x004cdb10, 0x12, LOCALE_FACET_ID_ATEXIT, WCharCtypeId)
