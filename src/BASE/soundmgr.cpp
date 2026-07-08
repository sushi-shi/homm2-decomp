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
#include <SOURCE/NOOPT.h>
#include <BASE/mss.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
void soundManager::ValidatePreviousPosition(int param_1)
{
    unsigned int local_18[5];
    int local_20;
    if (param_1 < 0 || 0x3b < param_1)
        local_20 = 0;
    else
        local_20 = 1;
    ProcessAssert(local_20, __FILE__, __LINE__);
    if (CDPreviousPosition[param_1][0] != 0) {
        strcpy(reinterpret_cast<char *>(local_18), CDPreviousPosition[param_1]);
        char *pcVar1 = FindToken(reinterpret_cast<char *>(local_18), ':');
        if (pcVar1 != 0)
            *pcVar1 = 0;
        int iVar2 = atoi(reinterpret_cast<char *>(local_18));
        if (iVar2 != param_1)
            CDPreviousPosition[param_1][0] = 0;
    }
}

VA(0x004cb770, 0x13c)
void soundManager::CDStop(void)
{
    unsigned int local_18[5];
    if (gbNoSound == 0 && field_0x69a != 0) {
        wsprintfA(reinterpret_cast<char *>(&CommandString), "stop CD wait");
        nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                   reinterpret_cast<char *>(&lpszReturnString), 0xff, 0);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
        if (stricmp(reinterpret_cast<char *>(&lpszReturnString), "stopped") != 0 && field_0x578 >= 0) {
            wsprintfA(reinterpret_cast<char *>(&CommandString), "status CD position");
            nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                       reinterpret_cast<char *>(local_18), 0x14, 0);
            if (nMCIError != 0)
                HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
            strcpy(CDPreviousPosition[field_0x578], reinterpret_cast<char *>(local_18));
            ValidatePreviousPosition(field_0x578);
        }
        CDPlaying = 0;
    }
}

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
void soundManager::CDSetVolume(int param_1, int param_2)
{
    int local_c;
    unsigned long local_8;
    if (gbNoSound == 0 && field_0x69a != 0 && field_0x698 != -1) {
        if (param_1 == -1)
            local_c = gMidiEnabled;
        else
            local_c = param_1;
        if (local_c == 0) {
            local_8 = 0;
        } else {
            if (param_2 != 0)
                local_c = 0xb - local_c / 0xc;
            int local_10 = 0xc - local_c;
            local_8 = local_10 * 0x10000000 | local_10 * 0x1000;
        }
        auxSetVolume(field_0x698, local_8);
    }
}

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
struct _DIG_DRIVER *WAVE_init_driver(unsigned long param_1, unsigned short param_2,
                                     unsigned short param_3, unsigned short param_4)
{
    struct _DIG_DRIVER *local_40;
    tagWAVEOUTCAPSA local_3c;
    int local_8;
    if (waveOutGetNumDevs() == 0) {
        local_40 = 0;
    } else if (waveOutGetDevCapsA(0, &local_3c, 0x34) == 0) {
        if (gbUseWaveout != 0)
            _AIL_set_preference_8(0xf, 1);
        gWaveFormat.wFormatTag = 1;
        gWaveFormat.nChannels = param_3;
        gWaveFormat.nAvgBytesPerSec = (param_2 >> 3) * param_3 * param_1;
        gWaveFormat.nBlockAlign = (param_2 >> 3) * param_3;
        gWaveFormat.nSamplesPerSec = param_1;
        gWaveFormat.wBitsPerSample = param_2;
        local_8 = _AIL_waveOutOpen_16(&local_40, 0, 0, &gWaveFormat);
        if (local_8 != 0) {
            if (param_4 != 0)
                MessageBoxA(ghWndMain, _AIL_last_error_0(), "Sound initialization error:", 0);
            local_40 = 0;
        }
    } else {
        MessageBoxA(ghWndMain, "Sound initialization error: No wave device", "Startup Error", 0);
        local_40 = 0;
    }
    return local_40;
}

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
void soundManager::StopAllSamples(int param_1)
{
    short local_8;
    int local_c;
    if (gbNoSound == 0 && field_0x36 != 0 && field_0x684 != 0) {
        LogStr("SAS 1");
        for (local_8 = 0; local_8 < field_0x94; local_8++) {
            if (_AIL_sample_status_4(field_0x54[local_8]) == 4)
                _AIL_end_sample_4(field_0x54[local_8]);
        }
        field_0x688 = 0;
        if (param_1 != 0) {
            if (gCdMusic == 0)
                MIDIStop();
            else
                CDStop();
        }
        for (local_c = 0; local_c < 5; local_c++) {
            ServiceSound();
            DelayMilli(1);
        }
        LogStr("SAS 2");
    }
}

VA(0x004ccbc0, 0xb1)
void soundManager::StopSample(struct _SAMPLE *param_1)
{
    int local_c;
    if (gbNoSound == 0 && field_0x36 != 0) {
        LogStr("Stop Sample 1");
        struct _SAMPLE *p_Var1 = field_0x54[0];
        _AIL_end_sample_4(param_1);
        if (p_Var1 == param_1) {
            for (local_c = 0; local_c < 10; local_c++) {
                ServiceSound();
                DelayMilli(1);
            }
        }
        LogStr("Stop Sample 2");
    }
}

VA(0x004ccc80, 0x202)
void soundManager::ModifySample(struct _SAMPLE *param_1, short param_2, long param_3)
{
    int iVar1;
    int local_10;
    int local_8;
    if (gbNoSound == 0 && field_0x36 != 0 && field_0x684 != 0 && field_0x3e != 0) {
        LogStr("Modify Sample 1");
        local_8 = -1;
        for (local_10 = 0; local_10 < field_0x94; local_10++) {
            if (field_0x54[local_10] == param_1)
                local_8 = local_10;
        }
        switch (param_2) {
        case 1:
        case 100:
            iVar1 = ConvertVolume(param_3, 100);
            _AIL_set_sample_volume_8(param_1, iVar1);
            if (local_8 >= 0)
                reinterpret_cast<short *>(&iLastVolume)[local_8] = static_cast<short>(param_3);
            break;
        case 5:
            _AIL_start_sample_4(param_1);
            break;
        case 0x65:
            ProcessAssert(gCdMusic == 0, __FILE__, __LINE__);
            iVar1 = ConvertVolume(param_3, 0x65);
            _AIL_set_sample_volume_8(param_1, iVar1);
            if (local_8 >= 0)
                reinterpret_cast<short *>(&iLastVolume)[local_8] = static_cast<short>(param_3);
        }
        Process1WindowsMessage();
        LogStr("Modify Sample 2");
    }
}

VA(0x004cce90, 0xa3)
long soundManager::DigitalReport(struct _SAMPLE *param_1, short param_2)
{
    unsigned int uVar1;
    if (gbNoSound == 0) {
        if (field_0x36 == 0)
            uVar1 = 0;
        else if (param_2 == 1)
            uVar1 = _AIL_sample_volume_4(param_1);
        else if (param_2 == 4)
            uVar1 = _AIL_sample_status_4(param_1) == 4;
        else
            uVar1 = 0;
    } else {
        uVar1 = 0;
    }
    return uVar1;
}

VA(0x004ccf40, 0xe8)
void soundManager::AdjustSoundVolumes(void)
{
    int local_c;
    if (gbNoSound == 0 && field_0x36 != 0 && field_0x684 != 0) {
        LogStr("Adjust Sound Volumes 1");
        for (local_c = 1; local_c < field_0x94; local_c++) {
            struct _SAMPLE *p_Var1 = field_0x54[local_c];
            if (gSampleVolume == 0) {
                ModifySample(p_Var1, 1, 0);
            } else {
                if (DigitalReport(p_Var1, 4) != 0)
                    ModifySample(p_Var1, 100, reinterpret_cast<short *>(&iLastVolume)[local_c]);
            }
        }
        LogStr("Adjust Sound Volumes 2");
    }
}

VA(0x004cd030, 0xee)
void soundManager::AdjustMusicVolumes(void)
{
    if (gbNoSound == 0 && field_0x684 != 0 && field_0x578 >= 0) {
        LogStr("Adjust Music Volumes 1");
        if (gMidiEnabled == 0) {
            if (gCdMusic == 0)
                MIDISetVolume();
            else
                CDSetVolume(-1, 0);
        } else if (gCdMusic == 0) {
            MIDISetVolume();
        } else {
            CDSetVolume(-1, 0);
        }
        LogStr("Adjust Music Volumes 2");
    }
}

VA(0x004cd120, 0x3a)
void soundManager::ForcePollSound(void)
{
    if (gbNoSound == 0) {
        field_0x579 = 1;
        PollSound();
    }
}

VA(0x004cd160, 0xe3)
void soundManager::SetMusicQuality(int param_1)
{
    char sVar1;
    int local_8;
    if (gbNoSound == 0 && field_0x3e != 0 && gMidiEnabled != 0 && field_0x69a != 0) {
        if (gCdMusic == 0) {
            sVar1 = field_0x578;
            MIDIStop();
        } else {
            sVar1 = field_0x578;
            CDStop();
            field_0x578 = static_cast<char>(0xff);
        }
        local_8 = sVar1;
        memset(field_0x590, 0, 0xf0);
        gCdMusic = param_1;
        if (local_8 >= 0)
            PlayAmbientMusic(local_8, 0, -1);
    }
}

VA(0x004cd250, 0xc5)
void soundManager::PlayAmbientMusic(int param_1, long param_2, int param_3)
{
    if (gbNoSound == 0 && field_0x684 != 0 && field_0x3e != 0 && field_0x578 != param_1) {
        if (gMidiEnabled == 0) {
            field_0x578 = static_cast<char>(param_1);
        } else {
            if (gCdMusic == 0)
                MIDIPlay(param_1);
            else
                CDPlay(param_1, param_2, -1, 0);
            field_0x578 = static_cast<char>(param_1);
        }
    }
}

VA(0x004cd320, 0x38f)
void soundManager::PollSound(void) {}

VA(0x004cd6b0, 0x138)
void soundManager::SwitchAmbientMusic(int param_1)
{
    if (gbNoSound == 0 && field_0x684 != 0) {
        if (gMidiEnabled == 0) {
            field_0x578 = static_cast<char>(param_1);
        } else if (MusicPlaying() == 0) {
            PlayAmbientMusic(param_1, 0, -1);
        } else if (field_0x578 != param_1) {
            LogStr("Switch Ambient Music 1");
            Process1WindowsMessage();
            if ((field_0x688 != 0 && field_0x68c != param_1) ||
                (field_0x688 == 0 && field_0x578 != param_1)) {
                if (field_0x688 < 0xb) {
                    field_0x688 = 0xb;
                    gMusicFadeTimer = KBTickCount() + 900;
                }
                field_0x68c = param_1;
                PollSound();
            }
            LogStr("Switch Ambient Music 2");
        }
    }
}

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
