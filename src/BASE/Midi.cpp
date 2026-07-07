// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Midi.obj   from: .\basewin.lib
// functions: 7   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/soundManager.h>
VA(0x004d3850, 0xb8)
void soundManager::MIDIStartup(void) {}

VA(0x004d3910, 0x1a9)
void soundManager::MIDIShutdown(void) {}

VA(0x004d3ac0, 0x3ab)
void soundManager::MIDIPlay(int) {}

VA(0x004d3e70, 0x108)
void soundManager::MIDIStop(void) {}

VA(0x004d3f80, 0x46)
int soundManager::MIDIIsPlaying(void) { return 0; }

VA(0x004d3fd0, 0x68)
void soundManager::MIDISetVolume(void) {}

VA(0x004d4040, 0x1)
void soundManager::MIDIPoll(void) {}

// ---- globals (definitions, RVA order) ----
struct _MDI_DRIVER *hMDI;
int CurrentMidiFile;
unsigned char *bGotMidi;
long lLastMIDIPollTickCount;
class MIDIWrap **pMIDIWrap;
struct _SEQUENCE **hSequence;
