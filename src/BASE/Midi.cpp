#include <Ints.h>
#include <BASE/soundManager.h>
#include <BASE/Midi.h>
#include <BASE/MusicFlags.h>
#include <PLATFORM/Miles.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/resourceManager.h>
#include <BASE/MIDIWrap.h>
#include <BASE/Misc.h>
#include <stdio.h>

typedef enum MidiSequenceStatus {
    SEQUENCE_PLAYING = 4
} MidiSequenceStatus;

typedef enum MidiVolumeConstant {
    VOLUME_HIGH_RANGE = 6,
    VOLUME_FADE_SPLIT = 10,
    VOLUME_LOW_RANGE  = 11,
    MAX_VOLUME        = 127
} MidiVolumeConstant;

typedef enum MidiConstant {
    MIDI_FILENAME_CAPACITY = 16
} MidiConstant;

i32 CurrentMidiFile = MIDI_NO_TRACK;
u8 bGotMidi[MIDI_TRACK_COUNT] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,
                                                  1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static char gMidiFilenameFormat[] = "MIDI%04d.XMI";

bool MIDIStartup(void) {
    if (MusicFlagsActive())
        return true;
    MIDIShutdown();
    gMusicFlagA = AIL_midiOutOpen(&hMDI, NULL, MIDI_MAPPER) == 0 ? true : false;
    gMusicFlagB = 1;
    gMusicFeatureEnabled = !gMusicFlagA;
    return true;
}

void MIDIShutdown(void) {
    i32 i;
    if (!GetMusicFlagA())
        return;
    for (i = 0; i < MIDI_TRACK_COUNT; i++) {
        if (hSequence[i] != NULL) {
            if (AIL_sequence_status(hSequence[i]) == SEQUENCE_PLAYING)
                AIL_stop_sequence(hSequence[i]);
            AIL_release_sequence_handle(hSequence[i]);
            hSequence[i] = NULL;
        }
    }
    AIL_midiOutClose(hMDI);
    hMDI = NULL;
    gMusicFlagA = 0;
    for (i = 0; i < MIDI_TRACK_COUNT; i++) {
        if (pMIDIWrap[i] != NULL) {
            gpResourceManager->Dispose(pMIDIWrap[i]);
            pMIDIWrap[i] = NULL;
        }
    }
}

void MIDIPlay(i32& currentTrack, i32& fadeSteps, i32 midiTrack) {
    if (!GetMusicFlagA())
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    if (bGotMidi[midiTrack] == 0)
        midiTrack = MIDI_NO_TRACK;
    if (midiTrack < 0) {
        MIDIStop(currentTrack);
        return;
    }
    if (midiTrack == CurrentMidiFile)
        return;
    MIDIStop(currentTrack);

    char filename[MIDI_FILENAME_CAPACITY];
    sprintf(filename, gMidiFilenameFormat, midiTrack);
    if (hSequence[midiTrack] == NULL) {
        hSequence[midiTrack] = AIL_allocate_sequence_handle(hMDI);
        if (hSequence[midiTrack] == NULL) {
            MIDIShutdown();
            return;
        }
        pMIDIWrap[midiTrack] = gpResourceManager->GetMIDIWrap(filename);
        if (AIL_init_sequence(hSequence[midiTrack], pMIDIWrap[midiTrack]->m_data, 0) == 0)
            return;
        MIDISetVolume(fadeSteps);
        AIL_start_sequence(hSequence[midiTrack]);
        if (bMusicIsLooping[midiTrack] != 0)
            AIL_set_sequence_loop_count(hSequence[midiTrack], 0);
        else
            AIL_set_sequence_loop_count(hSequence[midiTrack], 1);
    } else {
        AIL_resume_sequence(hSequence[midiTrack]);
    }
    CurrentMidiFile = midiTrack;
    currentTrack = midiTrack;
}

void MIDIStop(i32& currentTrack) {
    if (!GetMusicFlagA())
        return;
    if (CurrentMidiFile < 0)
        return;
    if (MIDIIsPlaying() && hSequence[CurrentMidiFile] != NULL) {
        AIL_stop_sequence(hSequence[CurrentMidiFile]);
        if (bSaveMusicPosition[CurrentMidiFile] == 0) {
            AIL_release_sequence_handle(hSequence[CurrentMidiFile]);
            hSequence[CurrentMidiFile] = NULL;
            if (pMIDIWrap[CurrentMidiFile] != NULL)
                gpResourceManager->Dispose(pMIDIWrap[CurrentMidiFile]);
            pMIDIWrap[CurrentMidiFile] = NULL;
        }
    }
    CurrentMidiFile = MIDI_NO_TRACK;
    currentTrack = MIDI_NO_TRACK;
}

bool MIDIIsPlaying(void) {
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED || !GetMusicFlagA()
        || CurrentMidiFile < 0 || hSequence[CurrentMidiFile] == NULL)
        return false;
    return AIL_sequence_status(hSequence[CurrentMidiFile]) == SEQUENCE_PLAYING;
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
    volume = gpSoundManager->ConvertVolume(volume, SOUND_VOLUME_MUSIC);
    AIL_set_XMIDI_master_volume(hMDI, volume);
}

struct _SEQUENCE* hSequence[MIDI_TRACK_COUNT] = {NULL};
class MIDIWrap* pMIDIWrap[MIDI_TRACK_COUNT] = {NULL};
u8 gMusicFlagA = 0;
u8 gMusicFlagB = 0;
struct _MDI_DRIVER* hMDI = NULL;
