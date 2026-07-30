#include <Ints.h>
#include <BASE/soundManager.h>
#include <PLATFORM/Miles.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/resourceManager.h>
#include <BASE/MIDIWrap.h>
#include <BASE/MIDI_TYPES.h>
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

struct _MDI_DRIVER* hMDI = NULL;
i32 CurrentMidiFile = MIDI_NO_TRACK;
u8 bGotMidi[MIDI_TRACK_COUNT] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,
                                                  1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
                                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
i32l lLastMIDIPollTickCount = 0;
static SMidiText gMidiText =
    {"MS1", "MS2", "MS6b", "MS6c", "MS1", "MS2", "MS4", "MP1a", "MIDI%04d.XMI"};

void soundManager::MIDIStartup(void) {
    i32 i;
    LogStr(gMidiText.startupBegin);
    if (gbNoSound == 0 && (m_midiStarted = 1, gbDontTryMIDI == 0)) {
        LogStr(gMidiText.startupDriver);
        for (i = 0; i < MIDI_TRACK_COUNT; i++)
            pMIDIWrap[i] = NULL;
        for (i = 0; i < MIDI_TRACK_COUNT; i++)
            hSequence[i] = NULL;
        m_midiReady = 1;
        LogStr(gMidiText.startupOpen);
        i = AIL_midiOutOpen(&hMDI, NULL, MIDI_MAPPER);
        LogInt(gMidiText.startupOpenResult, i, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE,
               LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE, LOG_UNUSED_VALUE);
        if (i != 0)
            m_midiReady = 0;
    }
}

void soundManager::MIDIShutdown(void) {
    i32 i;
    if (gbNoSound == 0 && m_midiReady != 0) {
        MIDIStop();
        LogStr(gMidiText.shutdownBegin);
        for (i = 0; i < MIDI_TRACK_COUNT; i++) {
            if (hSequence[i] != NULL)
                AIL_release_sequence_handle(hSequence[i]);
            hSequence[i] = NULL;
        }
        LogStr(gMidiText.shutdownDriver);
        AIL_midiOutClose(hMDI);
        hMDI = NULL;
        m_midiReady = 0;
        for (i = 0; i < MIDI_TRACK_COUNT; i++) {
            if (pMIDIWrap[i] != NULL)
                gpResourceManager->Dispose(pMIDIWrap[i]);
            pMIDIWrap[i] = NULL;
        }
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

            char filename[MIDI_FILENAME_CAPACITY];
            sprintf(filename, gMidiText.filenameFormat, midiTrack);
            if (hSequence[midiTrack] == NULL) {
                hSequence[midiTrack] = AIL_allocate_sequence_handle(hMDI);
                if (hSequence[midiTrack] == NULL)
                    MIDIShutdown();
                pMIDIWrap[midiTrack] = gpResourceManager->GetMIDIWrap(filename);
                if (AIL_init_sequence(hSequence[midiTrack], pMIDIWrap[midiTrack]->m_data, 0) == 0)
                    return;
                MIDISetVolume();
                AIL_start_sequence(hSequence[midiTrack]);
                if (bMusicIsLooping[midiTrack] != 0)
                    AIL_set_sequence_loop_count(hSequence[midiTrack], 0);
                else
                    AIL_set_sequence_loop_count(hSequence[midiTrack], 1);
            } else {
                AIL_resume_sequence(hSequence[midiTrack]);
            }
            CurrentMidiFile = midiTrack;
            m_currentTrack = static_cast<char>(midiTrack);
        }
    }
}

void soundManager::MIDIStop(void) {
    if (gbNoSound == 0 && m_midiReady != 0 && CurrentMidiFile != MIDI_NO_TRACK) {
        if (MIDIIsPlaying() && hSequence[CurrentMidiFile] != NULL) {
            AIL_stop_sequence(hSequence[CurrentMidiFile]);
            if (gbLowMemory != 0 || bSaveMusicPosition[CurrentMidiFile] == 0) {
                AIL_release_sequence_handle(hSequence[CurrentMidiFile]);
                hSequence[CurrentMidiFile] = NULL;
                if (pMIDIWrap[CurrentMidiFile] != NULL)
                    gpResourceManager->Dispose(pMIDIWrap[CurrentMidiFile]);
                pMIDIWrap[CurrentMidiFile] = NULL;
            }
        }
        CurrentMidiFile = MIDI_NO_TRACK;
        m_currentTrack = MIDI_NO_TRACK;
    }
}

i32 soundManager::MIDIIsPlaying(void) {
    if (gbNoSound == 0 && gConfig.musicVolume != CONFIG_VOLUME_MUTED && m_midiReady != 0
        && CurrentMidiFile != MIDI_NO_TRACK && hSequence[CurrentMidiFile] != NULL) {
        return AIL_sequence_status(hSequence[CurrentMidiFile]) == SEQUENCE_PLAYING;
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
        AIL_set_XMIDI_master_volume(hMDI, ConvertVolume(volume, SOUND_VOLUME_MUSIC));
    }
}

void soundManager::MIDIPoll(void) {}

class MIDIWrap* pMIDIWrap[MIDI_TRACK_COUNT];
struct _SEQUENCE* hSequence[MIDI_TRACK_COUNT];
