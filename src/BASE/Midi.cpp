// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Midi.obj   from: .\basewin.lib
// functions: 7   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/soundManager.h>
#include <BASE/mss.h>
#include <SOURCE/KB.h>
#include <_globals_model.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/resourceManager.h>
#include <BASE/MIDIWrap.h>
#include <BASE/Misc.h>
#include <stdio.h>
VA(0x004d3850, 0xb8)
void soundManager::MIDIStartup(void)
{
    int i;
    LogStr("MIDIStartup");
    if (gbNoSound == 0 && (field_0x6a6 = 1, gbDontTryMIDI == 0)) {
        LogStr("Clearing MIDI slots");
        for (i = 0; i < MIDI_TRACK_COUNT; i++)
            pMIDIWrap[i] = 0;
        for (i = 0; i < MIDI_TRACK_COUNT; i++)
            hSequence[i] = 0;
        field_0x69e = 1;
        LogStr("Opening MIDI output");
        i = _AIL_midiOutOpen_12(&hMDI, 0, MIDI_DEFAULT_DEVICE_ID);
        LogInt("midiOutOpen = %d", i, -999, -999, -999, -999, -999, -999);
        if (i != 0)
            field_0x69e = 0;
    }
}

VA(0x004d3910, 0x1a9)
void soundManager::MIDIShutdown(void)
{
    int i;
    if (gbNoSound == 0 && field_0x69e != 0) {
        MIDIStop();
        LogStr("MS1");
        for (i = 0; i < MIDI_TRACK_COUNT; i++) {
            if (hSequence[i] != 0)
                _AIL_release_sequence_handle_4(hSequence[i]);
            hSequence[i] = 0;
        }
        LogStr("MS2");
        _AIL_midiOutClose_4(hMDI);
        hMDI = 0;
        field_0x69e = 0;
        for (i = 0; i < MIDI_TRACK_COUNT; i++) {
            if (pMIDIWrap[i] != 0)
                gpResourceManager->Dispose(pMIDIWrap[i]);
            pMIDIWrap[i] = 0;
        }
        LogStr("MS4");
    }
}

// @early-stop
// Reloc/IAT-masked: 0x3aa of 0x3ab bytes are exact; only +0x167 differs (3b c3 vs 3b d8),
// commuting the operands of the CMP immediately consumed by JE. All 66 relocation targets agree.
// Equality/inequality, nested/early-return, subtraction, cast, local-copy, and |0 forms all emit c3.
VA(0x004d3ac0, 0x3ab)
void soundManager::MIDIPlay(int midiTrack)
{
    if (gbNoSound == 0 && field_0x69e != 0 && gMidiEnabled != 0) {
        LogStr("MP1a");
        if (bGotMidi[midiTrack] == 0)
            midiTrack = MIDI_NO_TRACK;
        if (midiTrack == MIDI_NO_TRACK) {
            MIDIStop();
            return;
        }
        if (midiTrack != CurrentMidiFile) {
            MIDIStop();

            char filename[16];
            sprintf(filename, "MIDI%04d.XMI", midiTrack);
            if (hSequence[midiTrack] == 0) {
                hSequence[midiTrack] = _AIL_allocate_sequence_handle_4(hMDI);
                if (hSequence[midiTrack] == 0)
                    MIDIShutdown();
                pMIDIWrap[midiTrack] = gpResourceManager->GetMIDIWrap(filename);
                if (_AIL_init_sequence_12(hSequence[midiTrack], pMIDIWrap[midiTrack]->m_data, 0) == 0)
                    return;
                MIDISetVolume();
                _AIL_start_sequence_4(hSequence[midiTrack]);
                if (bMusicIsLooping[midiTrack] != 0)
                    _AIL_set_sequence_loop_count_8(hSequence[midiTrack], 0);
                else
                    _AIL_set_sequence_loop_count_8(hSequence[midiTrack], 1);
            } else {
                _AIL_resume_sequence_4(hSequence[midiTrack]);
            }
            CurrentMidiFile = midiTrack;
            m_currentTrack = static_cast<char>(midiTrack);
        }
    }
}

VA(0x004d3e70, 0x108)
inline void soundManager::MIDIStop(void)
{
    if (gbNoSound == 0 && field_0x69e != 0 && CurrentMidiFile != MIDI_NO_TRACK) {
        if (MIDIIsPlaying() && hSequence[CurrentMidiFile] != 0) {
            _AIL_stop_sequence_4(hSequence[CurrentMidiFile]);
            if (gbLowMemory != 0 || bSaveMusicPosition[CurrentMidiFile] == 0) {
                _AIL_release_sequence_handle_4(hSequence[CurrentMidiFile]);
                hSequence[CurrentMidiFile] = 0;
                if (pMIDIWrap[CurrentMidiFile] != 0)
                    gpResourceManager->Dispose(pMIDIWrap[CurrentMidiFile]);
                pMIDIWrap[CurrentMidiFile] = 0;
            }
        }
        CurrentMidiFile = MIDI_NO_TRACK;
        m_currentTrack = MIDI_NO_TRACK;
    }
}

VA(0x004d3f80, 0x46)
inline int soundManager::MIDIIsPlaying(void)
{
    if (gbNoSound == 0 && gMidiEnabled != 0 && field_0x69e != 0 &&
        CurrentMidiFile != MIDI_NO_TRACK && hSequence[CurrentMidiFile] != 0) {
        return _AIL_sequence_status_4(hSequence[CurrentMidiFile]) == MIDI_SEQUENCE_PLAYING;
    }
    return 0;
}

VA(0x004d3fd0, 0x68)
inline void soundManager::MIDISetVolume(void)
{
    if (gbNoSound == 0 && field_0x69e != 0) {
        int volume = MIDI_MAX_VOLUME;
        if (m_fadeSteps > 0) {
            if (m_fadeSteps <= MIDI_VOLUME_FADE_SPLIT)
                volume = ((MIDI_VOLUME_LOW_RANGE - m_fadeSteps) * MIDI_MAX_VOLUME) /
                         MIDI_VOLUME_LOW_RANGE;
            else
                volume = ((m_fadeSteps - MIDI_VOLUME_FADE_SPLIT) * MIDI_MAX_VOLUME) /
                         MIDI_VOLUME_HIGH_RANGE;
        }
        _AIL_set_XMIDI_master_volume_8(hMDI, ConvertVolume(volume, MIDI_VOLUME_CONVERSION_MODE));
    }
}

VA(0x004d4040, 0x1)
void soundManager::MIDIPoll(void) {}

// ---- globals (definitions, RVA order) ----
DATA(0x0051fec8) struct _MDI_DRIVER *hMDI;
DATA(0x0051fecc) int CurrentMidiFile;
DATA(0x0051fed0) unsigned char bGotMidi[MIDI_TRACK_COUNT];
DATA(0x0051ff0c) long lLastMIDIPollTickCount;
DATA(0x00534cf0) class MIDIWrap *pMIDIWrap[MIDI_TRACK_COUNT];
DATA(0x00534de0) struct _SEQUENCE *hSequence[MIDI_TRACK_COUNT];
