// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\soundmgr.obj   from: .\basewin.lib
// functions: 34   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/soundManager.h>
#include <BASE/soundmgr.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <BASE/mss.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <BASE/Misc.h>
VA(0x004cb630, 0x68)
void HandleMCIError(int param_1, char *param_2)
{
    mciGetErrorStringA(param_1, reinterpret_cast<char *>(&lpszReturnString), 0xff);
    sprintf(gText, "CD MUSIC ERROR\nDescription: %s\nCall: %s", reinterpret_cast<char *>(&lpszReturnString), param_2);
    gMciErrorFlag = 1;
    gCdMusic = 0;
    WritePrefs();
    ShutDown(gText);
}

VA(0x004cb6a0, 0xc7)
void soundManager::ValidatePreviousPosition(int) {}

VA(0x004cb770, 0x13c)
void soundManager::CDStop(void) {}

VA(0x004cb8b0, 0xb3)
int soundManager::CDIsPlaying(void)
{
    unsigned int uVar1;
    if (gbNoSound == 0) {
        if (field_0x69a == 0) {
            uVar1 = 0;
        } else {
            wsprintfA(reinterpret_cast<char *>(&CommandString), "status CD mode");
            nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                       reinterpret_cast<char *>(&lpszReturnString), 0xff, 0);
            if (nMCIError != 0)
                HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
            uVar1 = (stricmp(reinterpret_cast<char *>(&lpszReturnString), "playing") == 0);
        }
    } else {
        uVar1 = 0;
    }
    return uVar1;
}

VA(0x004cb970, 0xf3)
void soundManager::CDStartup(void)
{
    if (gbNoSound == 0) {
        field_0x6a2 = 1;
        field_0x69a = 0;
        if (gbNoCDRom == 0 && gMciErrorFlag == 0 && gbDontTryRedbook == 0) {
            wsprintfA(reinterpret_cast<char *>(&CommandString),
                      "open %c: type cdaudio alias CD shareable", gcAnimPath[0]);
            nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                       reinterpret_cast<char *>(&lpszReturnString), 0xff, 0);
            if (nMCIError == 0) {
                field_0x69a = 1;
            } else {
                field_0x69a = 0;
                gMciErrorFlag = 1;
                gCdMusic = 0;
                WritePrefs();
            }
        }
    }
}

VA(0x004cba70, 0xd5)
void soundManager::CDShutdown(void)
{
    if (gbNoSound == 0 && field_0x69a != 0) {
        wsprintfA(reinterpret_cast<char *>(&CommandString), "stop CD");
        nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                   reinterpret_cast<char *>(&lpszReturnString), 0xff, 0);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
        wsprintfA(reinterpret_cast<char *>(&CommandString), "close CD");
        nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                   reinterpret_cast<char *>(&lpszReturnString), 0xff, 0);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
    }
}

VA(0x004cbb50, 0xe5)
void soundManager::CDSetVolume(int, int) {}

VA(0x004cbc40, 0x473)
void soundManager::CDPlay(int, int, int, int) {}

VA(0x004cc0c0, 0xf1)
void soundManager::CDPoll(void)
{
    if (gbNoSound == 0 && gMidiEnabled != 0 && field_0x69a != 0 &&
        CDPlaying != 0 && field_0x578 >= 0 &&
        bMusicIsLooping[field_0x578] != 0 &&
        field_0x6aa + 3000 <= KBTickCount()) {
        field_0x6aa = KBTickCount();
        if (CDIsPlaying() == 0)
            CDPlay(field_0x690, 0, field_0x694, 1);
    }
}

VA(0x004cc1c0, 0xdd)
int soundManager::ConvertVolume(int param_1, int param_2)
{
    int local_8 = 0;
    if (param_2 == 0x65) {
        if (gMidiEnabled > 0 && gMidiEnabled < 0xb &&
            (local_8 = ((0xb - gMidiEnabled) * param_1) / 10, local_8 < 1))
            local_8 = 1;
    } else if (gSampleVolume > 0 && gSampleVolume < 0xb &&
               (local_8 = ((0xb - gSampleVolume) * param_1) / 10, local_8 < 1)) {
        local_8 = 1;
    }
    if (local_8 < 0)
        local_8 = 0;
    if (0x7f < local_8)
        local_8 = 0x7f;
    return local_8;
}

VA(0x004cc2a0, 0x5e)
void __stdcall SetReady2Poll(unsigned long int)
{
    if (gpSoundManager != 0 &&
        (gpSoundManager->field_0x57b ^= 1, gpSoundManager->field_0x57b != 0))
        gpSoundManager->field_0x57a = 1;
}

VA(0x004cc300, 0x26)
void __stdcall UpdateTimers(unsigned long int)
{
    iCalibrateLoop++;
    glMilliCounter += 0x10;
    SetReady2Poll(0);
}

VA(0x004cc330, 0xd7)
soundManager::soundManager(void) : baseManager()
{
    int local_8;
    field_0x574 = 1;
    field_0x32 = 0;
    field_0x688 = 0;
    field_0x56c = 0;
    for (local_8 = 0; local_8 < 0x20; local_8++)
        reinterpret_cast<short *>(&iLastVolume)[local_8] = 0;
    memset(&field_0x3e, 0, 0xae);
    field_0x684 = 0;
    field_0x36 = 0;
    field_0x3a = 0;
    field_0x690 = 0;
    field_0x694 = 0;
}

VA(0x004cc410, 0x14a)
struct _DIG_DRIVER * WAVE_init_driver(unsigned long int, unsigned short int, unsigned short int, unsigned short int) { return 0; }

VA(0x004cc560, 0x3a8)
int soundManager::Open(int) { return 0; }

VA(0x004cc910, 0x91)
void soundManager::AllocateSampleHandles(void)
{
    int local_8;
    if (gbNoSound == 0 && field_0x36 != 0) {
        for (local_8 = 0; local_8 < 0xe; local_8++) {
            field_0x54[local_8] = _AIL_allocate_sample_handle_4(field_0x36);
            if (field_0x54[local_8] == 0)
                break;
        }
        field_0x94 = local_8;
    }
}

VA(0x004cc9b0, 0x96)
void soundManager::Close(void)
{
    if (field_0x32 == 1) {
        if (gbNoSound == 0) {
            LogStr("Shutting down CD audio");
            CDShutdown();
            LogStr("Shutting down MIDI");
            MIDIShutdown();
            LogStr("Shutting down AIL");
            _AIL_shutdown_0();
            LogStr("Sound shut down");
        }
        field_0x32 = 0;
        gbNoSound = 1;
    }
}

VA(0x004cca50, 0x1a)
int soundManager::Main(struct tag_message &) { return 0; }

VA(0x004cca70, 0x1a)
struct _SAMPLE * soundManager::StartSample(char *, char * *, short int, short int, int, int, long int) { return 0; }

VA(0x004cca90, 0x126)
void soundManager::StopAllSamples(int) {}

VA(0x004ccbc0, 0xb1)
void soundManager::StopSample(struct _SAMPLE *) {}

VA(0x004ccc80, 0x202)
void soundManager::ModifySample(struct _SAMPLE *, short int, long int) {}

VA(0x004cce90, 0xa3)
long int soundManager::DigitalReport(struct _SAMPLE *, short int) { return 0; }

VA(0x004ccf40, 0xe8)
void soundManager::AdjustSoundVolumes(void) {}

VA(0x004cd030, 0xee)
void soundManager::AdjustMusicVolumes(void) {}

VA(0x004cd120, 0x3a)
void soundManager::ForcePollSound(void)
{
    if (gbNoSound == 0) {
        field_0x579 = 1;
        PollSound();
    }
}

VA(0x004cd160, 0xe3)
void soundManager::SetMusicQuality(int) {}

VA(0x004cd250, 0xc5)
void soundManager::PlayAmbientMusic(int, long int, int) {}

VA(0x004cd320, 0x38f)
void soundManager::PollSound(void) {}

VA(0x004cd6b0, 0x138)
void soundManager::SwitchAmbientMusic(int) {}

VA(0x004cd7f0, 0x28f)
struct _SAMPLE * soundManager::MemorySample(class sample *) { return 0; }

VA(0x004cda80, 0x16)
void soundManager::GetNumberCDDrives(void) {}

VA(0x004cdaa0, 0x2e)
void soundManager::ServiceSound(void)
{
    if (gbNoSound == 0)
        _AIL_serve_0();
}

VA(0x004cdad0, 0x7f)
int soundManager::MusicPlaying(void)
{
    if (gbNoSound == 0) {
        if (gCdMusic == 0) {
            if (field_0x69e == 0)
                return 0;
            return MIDIIsPlaying();
        }
        if (field_0x69a == 0)
            return 0;
        return CDIsPlaying();
    }
    return 0;
}


// ===== vtable soundManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004cc560, 0x3a8)  int soundManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004cc9b0, 0x96)  void soundManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004cca50, 0x1a)  int soundManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(soundManager, 0x004eba20);

// ---- globals (definitions, RVA order) ----
char *digitalDriverNames[14];
struct SampleChannelStruct *SCS;
char CDPreviousPosition[60][15];
int CDWaiting;
int CDPlaying;
int iCalibrateLoop;
char *lpszReturnString;
unsigned long nMCIError;
short *iLastVolume;
char *CommandString;
