#include <Ints.h>
#include <BASE/soundBackends.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <stdio.h>

typedef enum AudiereMusicConstant {
    AUDIERE_MUSIC_VOLUME_LEVEL_COUNT = 12,
    AUDIERE_FADE_VOLUME_DIVISOR      = 12,
    AUDIERE_FADE_VOLUME_LIMIT        = 121,
    AUDIERE_FADE_START_LEVEL         = 10,
    AUDIERE_FADE_STEP_COUNT          = 11,
    AUDIERE_FADE_DELAY               = 480
} AudiereMusicConstant;

static float gAudiereMusicVolume = 1.0f;
static float gAudiereMusicVolumes[AUDIERE_MUSIC_VOLUME_LEVEL_COUNT] = {
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
audiere::OutputStreamPtr AudiereMusic::stream;
audiere::SampleSourcePtr AudiereMusic::source;
static i32 gAudiereMusicPositions[MIDI_TRACK_COUNT] = {0};


void StopAudiereMusic(i32& currentTrack) {
    if (AudiereMusic::stream) {
        if (AudiereMusic::stream->isPlaying()) {
            gAudiereMusicPositions[currentTrack] = AudiereMusic::stream->getPosition();
            AudiereMusic::stream->stop();
        }
        AudiereMusic::stream = NULL;
    }
    if (AudiereMusic::source)
        AudiereMusic::source = NULL;
    currentTrack = MIDI_NO_TRACK;
}

bool AudiereMusicAvailable(void) {
    return true;
}

bool AudiereMusicPlaying(void) {
    if (!AudiereMusic::stream)
        return false;
    return AudiereMusic::stream->isPlaying();
}

bool StartupAudiereMusic(audiere::AudioDevicePtr device) {
    ResetAudiereMusic();
    if (device == NULL)
        return false;
    return true;
}

void ResetAudiereMusic(void) {
    if (AudiereMusic::stream) {
        if (AudiereMusic::stream->isPlaying())
            AudiereMusic::stream->stop();
        AudiereMusic::stream = NULL;
    }
    if (AudiereMusic::source)
        AudiereMusic::source = NULL;
    for (i32 track = 0; track < MIDI_TRACK_COUNT; ++track)
        gAudiereMusicPositions[track] = 0;
}

void SetAudiereMusicVolume(i32 volume, i32 fading) {
    i32 volumeLevel;
    if (volume == -1) {
        volumeLevel = (gConfig.musicVolume);
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
    if (AudiereMusic::stream)
        AudiereMusic::stream->setVolume(gAudiereMusicVolume);
}

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
            AudiereMusic::source = source;
            AudiereMusic::stream = stream;
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
