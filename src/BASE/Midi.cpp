#include <Ints.h>
#include <BASE/Midi.h>
#include <BASE/MusicFlags.h>
#include <BASE/Misc.h>
#include <BASE/soundManager.h>
#include <PLATFORM/Platform.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

typedef enum MidiVolumeConstant {
    VOLUME_HIGH_RANGE = 6,
    VOLUME_FADE_SPLIT = 10,
    VOLUME_LOW_RANGE  = 11,
    MAX_VOLUME        = 127
} MidiVolumeConstant;

i32 CurrentMidiFile = MIDI_NO_TRACK;
// Track 44 is Ironfist's Cyborg town theme (data/MUSIC/Track44.ogg).
u8 bGotMidi[MIDI_TRACK_COUNT] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,
                                 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
u8 gMusicFlagA = 0;
u8 gMusicFlagB = 0;

bool MIDIStartup(void) {
    if (MusicFlagsActive())
        return true;
    gMusicFlagA = platform::Audio().Open() ? 1 : 0;
    gMusicFlagB = 1;
    gMusicFeatureEnabled = !gMusicFlagA;
    return gMusicFlagA != 0;
}

void MIDIShutdown(void) {
    if (!GetMusicFlagA())
        return;
    platform::Audio().StopMusic();
    CurrentMidiFile = MIDI_NO_TRACK;
    gMusicFlagA = 0;
}

void MIDIPlay(i32& currentTrack, i32& fadeSteps, i32 midiTrack) {
    if (!GetMusicFlagA() || gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    if (midiTrack < 0 || bGotMidi[midiTrack] == 0) {
        MIDIStop(currentTrack);
        return;
    }
    if (midiTrack == CurrentMidiFile && platform::Audio().IsMusicPlaying())
        return;

    MIDIStop(currentTrack);
    MIDISetVolume(fadeSteps);
    if (!platform::Audio().PlayMusicTrack(midiTrack, bMusicIsLooping[midiTrack] ? -1 : 0))
        return;
    CurrentMidiFile = midiTrack;
    currentTrack = midiTrack;
}

void MIDIStop(i32& currentTrack) {
    if (GetMusicFlagA())
        platform::Audio().StopMusic();
    CurrentMidiFile = MIDI_NO_TRACK;
    currentTrack = MIDI_NO_TRACK;
}

bool MIDIIsPlaying(void) {
    return gConfig.musicVolume != CONFIG_VOLUME_MUTED && GetMusicFlagA()
        && CurrentMidiFile >= 0 && platform::Audio().IsMusicPlaying();
}

void MIDISetVolume(i32& fadeSteps) {
    if (!GetMusicFlagA())
        return;
    i32 volume = MAX_VOLUME;
    if (fadeSteps > 0) {
        if (fadeSteps <= VOLUME_FADE_SPLIT)
            volume = (volume * (VOLUME_LOW_RANGE - fadeSteps)) / VOLUME_LOW_RANGE;
        else
            volume = (volume * (fadeSteps - VOLUME_FADE_SPLIT)) / VOLUME_HIGH_RANGE;
    }
    platform::Audio().SetMusicVolume(
        gpSoundManager->ConvertVolume(volume, SOUND_VOLUME_MUSIC)
    );
}
