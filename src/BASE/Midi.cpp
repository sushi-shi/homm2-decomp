#include <Ints.h>
#include <BASE/soundManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
#include <BASE/MIDI_TYPES.h>
#include <PLATFORM/Platform.h>

typedef enum MidiVolumeConstant {
    VOLUME_HIGH_RANGE = 6,
    VOLUME_FADE_SPLIT = 10,
    VOLUME_LOW_RANGE  = 11,
    MAX_VOLUME        = 127
} MidiVolumeConstant;

i32 CurrentMidiFile = MIDI_NO_TRACK;
u8 bGotMidi[MIDI_TRACK_COUNT] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,
                                                  1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
i32l lLastMIDIPollTickCount = 0;
static SMidiText gMidiText = {"MS1", "MS6b", "MS1", "MS4", "MP1a"};

void soundManager::MIDIStartup(void) {
    LogStr(gMidiText.startupBegin);
    if (gbNoSound == 0 && (m_midiStarted = 1, gbDontTryMIDI == 0)) {
        LogStr(gMidiText.startupOpen);
        m_midiReady = platform::Audio().Open() ? 1 : 0;
    }
}

void soundManager::MIDIShutdown(void) {
    if (gbNoSound == 0 && m_midiReady != 0) {
        MIDIStop();
        LogStr(gMidiText.shutdownBegin);
        m_midiReady = 0;
        LogStr(gMidiText.shutdownComplete);
    }
}

void soundManager::MIDIPlay(i32 midiTrack) {
    if (gbNoSound == 0 && m_midiReady != 0 && gConfig.musicVolume != CONFIG_VOLUME_MUTED) {
        LogStr(gMidiText.playBegin);
        if (bGotMidi[midiTrack] == 0)
            midiTrack = MIDI_NO_TRACK;
        if (midiTrack == MIDI_NO_TRACK) {
            MIDIStop();
            return;
        }
        if (midiTrack != CurrentMidiFile) {
            MIDIStop();
            MIDISetVolume();
            if (!platform::Audio().PlayMusicTrack(midiTrack, bMusicIsLooping[midiTrack] ? -1 : 0))
                return;
            CurrentMidiFile = midiTrack;
            m_currentTrack = static_cast<char>(midiTrack);
        }
    }
}

void soundManager::MIDIStop(void) {
    if (gbNoSound == 0 && m_midiReady != 0 && CurrentMidiFile != MIDI_NO_TRACK) {
        platform::Audio().StopMusic();
        CurrentMidiFile = MIDI_NO_TRACK;
        m_currentTrack = MIDI_NO_TRACK;
    }
}

i32 soundManager::MIDIIsPlaying(void) {
    if (gbNoSound == 0 && gConfig.musicVolume != CONFIG_VOLUME_MUTED && m_midiReady != 0
        && CurrentMidiFile != MIDI_NO_TRACK) {
        return platform::Audio().IsMusicPlaying();
    }
    return 0;
}

void soundManager::MIDISetVolume(void) {
    if (gbNoSound == 0 && m_midiReady != 0) {
        i32 volume = MAX_VOLUME;
        if (m_fadeSteps > 0) {
            if (m_fadeSteps <= VOLUME_FADE_SPLIT)
                volume = ((VOLUME_LOW_RANGE - m_fadeSteps) * MAX_VOLUME)
                         / VOLUME_LOW_RANGE;
            else
                volume = ((m_fadeSteps - VOLUME_FADE_SPLIT) * MAX_VOLUME)
                         / VOLUME_HIGH_RANGE;
        }
        platform::Audio().SetMusicVolume(ConvertVolume(volume, SOUND_VOLUME_MUSIC));
    }
}

void soundManager::MIDIPoll(void) {}
