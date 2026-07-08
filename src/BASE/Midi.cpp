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
VA(0x004d3850, 0xb8)
void soundManager::MIDIStartup(void) {}

VA(0x004d3910, 0x1a9)
void soundManager::MIDIShutdown(void) {}

VA(0x004d3ac0, 0x3ab)
void soundManager::MIDIPlay(int) {}

VA(0x004d3e70, 0x108)
void soundManager::MIDIStop(void)
{
    if (gbNoSound == 0 && field_0x69e != 0 && CurrentMidiFile != -1) {
        if (gMidiEnabled != 0 && field_0x69e != 0 && CurrentMidiFile != -1 &&
            hSequence[CurrentMidiFile] != 0 &&
            _AIL_sequence_status_4(hSequence[CurrentMidiFile]) == 4 &&
            hSequence[CurrentMidiFile] != 0 &&
            (_AIL_stop_sequence_4(hSequence[CurrentMidiFile]),
             gbLowMemory != 0 || bSaveMusicPosition[CurrentMidiFile] == 0)) {
            _AIL_release_sequence_handle_4(hSequence[CurrentMidiFile]);
            int iVar1 = CurrentMidiFile;
            hSequence[CurrentMidiFile] = 0;
            if (pMIDIWrap[iVar1] != 0)
                gpResourceManager->Dispose(pMIDIWrap[iVar1]);
            pMIDIWrap[CurrentMidiFile] = 0;
        }
        CurrentMidiFile = -1;
        field_0x578 = 0xff;
    }
}

VA(0x004d3f80, 0x46)
int soundManager::MIDIIsPlaying(void)
{
    if (gbNoSound == 0 && gMidiEnabled != 0 && field_0x69e != 0 &&
        CurrentMidiFile != -1 && hSequence[CurrentMidiFile] != 0) {
        return _AIL_sequence_status_4(hSequence[CurrentMidiFile]) == 4;
    }
    return 0;
}

VA(0x004d3fd0, 0x68)
void soundManager::MIDISetVolume(void)
{
    if (gbNoSound == 0 && field_0x69e != 0) {
        int iVar1 = 0x7f;
        int iVar2 = field_0x688;
        if (iVar2 > 0) {
            if (iVar2 < 0xb) {
                iVar1 = 0xb;
                iVar2 = 0xb - iVar2;
            } else {
                iVar2 = iVar2 - 10;
                iVar1 = 6;
            }
            iVar1 = (iVar2 * 0x7f) / iVar1;
        }
        iVar2 = ConvertVolume(iVar1, 0x65);
        _AIL_set_XMIDI_master_volume_8(hMDI, iVar2);
    }
}

VA(0x004d4040, 0x1)
void soundManager::MIDIPoll(void) {}

// ---- globals (definitions, RVA order) ----
struct _MDI_DRIVER *hMDI;
int CurrentMidiFile;
unsigned char *bGotMidi;
long lLastMIDIPollTickCount;
class MIDIWrap *pMIDIWrap[60];
struct _SEQUENCE *hSequence[60];
