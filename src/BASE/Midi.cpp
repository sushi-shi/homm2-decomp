// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Midi.obj   from: .\basewin.lib
// functions: 7   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/soundManager.h>
#include <mss.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/resourceManager.h>
#include <BASE/MIDIWrap.h>
#include <BASE/MIDI_TYPES.h>
#include <BASE/Misc.h>
#include <stdio.h>

DATA(0x0051fec8) struct _MDI_DRIVER *hMDI = 0;
DATA(0x0051fecc) i32 CurrentMidiFile = MIDI_NO_TRACK;
DATA(0x0051fed0) u8 bGotMidi[MIDI_TRACK_COUNT] = {
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
DATA(0x0051ff0c) i32l lLastMIDIPollTickCount = 0;
DATA(0x0051ff10) static SMidiText gMidiText = {
    "MS1",
    "MS2",
    "MS6b",
    "MS6c",
    "MS1",
    "MS2",
    "MS4",
    "MP1a",
    "MIDI%04d.XMI"
};

VA(0x004d3850, 0xb8)
void soundManager::MIDIStartup(void)
{
    i32 i;
    LogStr(gMidiText.startupBegin);
    if (gbNoSound == 0 && (m_midiStarted = 1, gbDontTryMIDI == 0)) {
        LogStr(gMidiText.startupDriver);
        for (i = 0; i < MIDI_TRACK_COUNT; i++)
            pMIDIWrap[i] = 0;
        for (i = 0; i < MIDI_TRACK_COUNT; i++)
            hSequence[i] = 0;
        m_midiReady = 1;
        LogStr(gMidiText.startupOpen);
        i = AIL_midiOutOpen(&hMDI, 0, MIDI_MAPPER);
        LogInt(gMidiText.startupOpenResult, i, -999, -999, -999, -999, -999, -999);
        if (i != 0)
            m_midiReady = 0;
    }
}

VA(0x004d3910, 0x1a9)
void soundManager::MIDIShutdown(void)
{
    i32 i;
    if (gbNoSound == 0 && m_midiReady != 0) {
        MIDIStop();
        LogStr(gMidiText.shutdownBegin);
        for (i = 0; i < MIDI_TRACK_COUNT; i++) {
            if (hSequence[i] != 0)
                AIL_release_sequence_handle(hSequence[i]);
            hSequence[i] = 0;
        }
        LogStr(gMidiText.shutdownDriver);
        AIL_midiOutClose(hMDI);
        hMDI = 0;
        m_midiReady = 0;
        for (i = 0; i < MIDI_TRACK_COUNT; i++) {
            if (pMIDIWrap[i] != 0)
                gpResourceManager->Dispose(pMIDIWrap[i]);
            pMIDIWrap[i] = 0;
        }
        LogStr(gMidiText.shutdownComplete);
    }
}

// @early-stop
// Reloc/IAT-masked: 0x3aa of 0x3ab bytes are exact; only +0x167 differs (3b c3 vs 3b d8),
// commuting the operands of the CMP immediately consumed by JE. All 66 relocation targets agree.
// Equality/inequality, nested/early-return, subtraction, cast, local-copy, and |0 forms all emit c3.
VA(0x004d3ac0, 0x3ab)
void soundManager::MIDIPlay(i32 midiTrack)
{
    if (gbNoSound == 0 && m_midiReady != 0 && gConfig.musicVolume != 0) {
        LogStr(gMidiText.playBegin);
        if (bGotMidi[midiTrack] == 0)
            midiTrack = MIDI_NO_TRACK;
        if (midiTrack == MIDI_NO_TRACK) {
            MIDIStop();
            return;
        }
        if (midiTrack != CurrentMidiFile) {
            MIDIStop();

            char filename[16];
            sprintf(filename, gMidiText.filenameFormat, midiTrack);
            if (hSequence[midiTrack] == 0) {
                hSequence[midiTrack] = AIL_allocate_sequence_handle(hMDI);
                if (hSequence[midiTrack] == 0)
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

VA(0x004d3e70, 0x108)
inline void soundManager::MIDIStop(void)
{
    if (gbNoSound == 0 && m_midiReady != 0 && CurrentMidiFile != MIDI_NO_TRACK) {
        if (MIDIIsPlaying() && hSequence[CurrentMidiFile] != 0) {
            AIL_stop_sequence(hSequence[CurrentMidiFile]);
            if (gbLowMemory != 0 || bSaveMusicPosition[CurrentMidiFile] == 0) {
                AIL_release_sequence_handle(hSequence[CurrentMidiFile]);
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
inline i32 soundManager::MIDIIsPlaying(void)
{
    if (gbNoSound == 0 && gConfig.musicVolume != 0 && m_midiReady != 0 &&
        CurrentMidiFile != MIDI_NO_TRACK && hSequence[CurrentMidiFile] != 0) {
        return AIL_sequence_status(hSequence[CurrentMidiFile]) == MIDI_SEQUENCE_PLAYING;
    }
    return 0;
}

VA(0x004d3fd0, 0x68)
inline void soundManager::MIDISetVolume(void)
{
    if (gbNoSound == 0 && m_midiReady != 0) {
        i32 volume = MIDI_MAX_VOLUME;
        if (m_fadeSteps > 0) {
            if (m_fadeSteps <= MIDI_VOLUME_FADE_SPLIT)
                volume = ((MIDI_VOLUME_LOW_RANGE - m_fadeSteps) * MIDI_MAX_VOLUME) /
                         MIDI_VOLUME_LOW_RANGE;
            else
                volume = ((m_fadeSteps - MIDI_VOLUME_FADE_SPLIT) * MIDI_MAX_VOLUME) /
                         MIDI_VOLUME_HIGH_RANGE;
        }
        AIL_set_XMIDI_master_volume(hMDI, ConvertVolume(volume, SOUND_VOLUME_MUSIC));
    }
}

VA(0x004d4040, 0x1)
void soundManager::MIDIPoll(void) {}

// @data-layout-note
// Midi has no candidate `.rdata`. Its ordinary initialized `.data` is byte-exact
// over retail 0x11fec8..0x11ff10 (0x48 bytes), and its zero-fill contribution is
// exactly two 0xf0-byte arrays over 0x134cf0..0x134ed0. All 89 relevant candidate
// DIR32 sites are retail HIGHLOW sites. Global references use owner addend zero,
// apart from the proven pMIDIWrap+0xf0 and hSequence+0xf0 end sentinels.
//
// Retail appends one 0x40 text bank to the ordinary initialized-data prefix.
// Its nine owners begin at addends 0x48, 0x4c, 0x50, 0x58, 0x60, 0x64, 0x68,
// 0x6c, and 0x74. In particular, startup and shutdown use independent MS1/MS2
// copies; pooled compiler literals cannot represent those relocation addends.
// The typed gMidiText bank reproduces the complete 0x88 contribution and keeps
// every source use attached to its retail owner without synthetic identities or
// padding allocations.
// ---- globals (definitions, RVA order) ----
DATA(0x00534cf0) class MIDIWrap *pMIDIWrap[MIDI_TRACK_COUNT];
DATA(0x00534de0) struct _SEQUENCE *hSequence[MIDI_TRACK_COUNT];
