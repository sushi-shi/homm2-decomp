// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\soundmgr.obj   from: .\basewin.lib
// functions: 34   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/soundManager.h>
#include <BASE/sample.h>
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
    if (gbNoSound != 0)
        return 0;
    if (field_0x69a == 0)
        return 0;
    wsprintfA(reinterpret_cast<char *>(&CommandString), "status CD mode");
    nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                               reinterpret_cast<char *>(&lpszReturnString), 0xff, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
    return stricmp(reinterpret_cast<char *>(&lpszReturnString), "playing") == 0;
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
void soundManager::CDPlay(int param_1, int param_2, int param_3, int param_4)
{
    char cVar1;
    int iVar2;
    long lVar3;
    HWND local_30;
    HWND local_2c;
    unsigned int local_24[5];
    long local_10;
    long local_c;
    long local_8;
    if (gbNoSound == 0 && field_0x69a != 0 && gMidiEnabled != 0) {
        if (param_1 == -1) {
            CDStop();
        } else if (field_0x578 != param_1 || CDPlaying == 0 || param_4 != 0) {
            field_0x690 = param_1;
            field_0x694 = param_3;
            Process1WindowsMessage();
            ServiceSound();
            local_8 = KBTickCount();
            wsprintfA(reinterpret_cast<char *>(&CommandString), "set CD time format tmsf");
            nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                       reinterpret_cast<char *>(&lpszReturnString), 0xff, 0);
            if (nMCIError != 0)
                HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
            wsprintfA(reinterpret_cast<char *>(&CommandString), "status CD mode");
            nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                       reinterpret_cast<char *>(&lpszReturnString), 0xff, 0);
            if (nMCIError != 0)
                HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
            iVar2 = stricmp(reinterpret_cast<char *>(&lpszReturnString), "stopped");
            if (iVar2 != 0) {
                wsprintfA(reinterpret_cast<char *>(&CommandString), "status CD position");
                nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                           reinterpret_cast<char *>(local_24), 0x14, 0);
                if (nMCIError != 0)
                    HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
                strcpy(CDPreviousPosition[field_0x578], reinterpret_cast<char *>(local_24));
                ValidatePreviousPosition(field_0x578);
            }
            local_c = KBTickCount();
            cVar1 = bMusicIsLooping[param_1];
            Process1WindowsMessage();
            ServiceSound();
            if (param_4 == 0 && param_2 != 0 && CDPreviousPosition[param_1][0] != 0) {
                if (param_1 == 0x2b)
                    wsprintfA(reinterpret_cast<char *>(&CommandString), "play CD from %s %s",
                              CDPreviousPosition[param_1], "notify" + (((cVar1 != 0) - 1) & 8));
                else
                    wsprintfA(reinterpret_cast<char *>(&CommandString), "play CD from %s to %d %s",
                              CDPreviousPosition[param_1], param_1 + 1,
                              "notify" + (((cVar1 != 0) - 1) & 8));
                if (cVar1 == 0)
                    local_2c = 0;
                else
                    local_2c = ghWndMain;
                nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                           reinterpret_cast<char *>(&lpszReturnString), 0xff, local_2c);
                if (nMCIError != 0)
                    HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
            } else {
                if (param_1 == 0x2b)
                    wsprintfA(reinterpret_cast<char *>(&CommandString), "play CD from %d %s",
                              0x2b, "notify" + (((cVar1 != 0) - 1) & 8));
                else
                    wsprintfA(reinterpret_cast<char *>(&CommandString), "play CD from %d to %d %s",
                              param_1, param_1 + 1, "notify" + (((cVar1 != 0) - 1) & 8));
                if (cVar1 == 0)
                    local_30 = 0;
                else
                    local_30 = ghWndMain;
                nMCIError = mciSendStringA(reinterpret_cast<char *>(&CommandString),
                                           reinterpret_cast<char *>(&lpszReturnString), 0xff, local_30);
                if (nMCIError != 0)
                    HandleMCIError(nMCIError, reinterpret_cast<char *>(&CommandString));
            }
            local_10 = KBTickCount();
            CDPlaying = 1;
            Process1WindowsMessage();
            ServiceSound();
            if (field_0x688 < 1) {
                CDSetVolume(param_3, 0);
            } else {
                field_0x688 = 0xb;
                lVar3 = KBTickCount();
                gMusicFadeTimer = lVar3 + 0x1e0;
                CDSetVolume(10, 0);
            }
            field_0x578 = static_cast<char>(param_1);
        }
    }
}

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
int soundManager::Open(int param_1)
{
    SHORT SVar1;
    struct _DIG_DRIVER *p_Var2;
    int local_c;
    char cStack_7;
    field_0x6a2 = 0;
    field_0x6a6 = 0;
    field_0x69a = 0;
    field_0x69e = 0;
    memset(bSaveMusicPosition, 0, 0x3c);
    memset(bMusicIsLooping, 0, 0x3c);
    bSaveMusicPosition[0x10] = 1;
    bSaveMusicPosition[0x12] = 1;
    bSaveMusicPosition[0xe] = 1;
    bSaveMusicPosition[0xf] = 1;
    bSaveMusicPosition[0xb] = 1;
    bSaveMusicPosition[0xd] = 1;
    bSaveMusicPosition[0x11] = 1;
    bSaveMusicPosition[0xc] = 1;
    bSaveMusicPosition[0x1c] = 1;
    bSaveMusicPosition[0x2a] = 1;
    bMusicIsLooping[0x10] = 1;
    bMusicIsLooping[0x12] = 1;
    bMusicIsLooping[0xe] = 1;
    bMusicIsLooping[0xf] = 1;
    bMusicIsLooping[0xb] = 1;
    bMusicIsLooping[0xd] = 1;
    bMusicIsLooping[0x11] = 1;
    bMusicIsLooping[0xc] = 1;
    bMusicIsLooping[8] = 1;
    bMusicIsLooping[9] = 1;
    bMusicIsLooping[5] = 1;
    bMusicIsLooping[6] = 1;
    bMusicIsLooping[0xa] = 1;
    bMusicIsLooping[7] = 1;
    bMusicIsLooping[0x2a] = 1;
    bMusicIsLooping[0x1c] = 1;
    bMusicIsLooping[0x2b] = 1;
    bMusicIsLooping[0x16] = 1;
    bMusicIsLooping[0x18] = 1;
    for (local_c = 2; local_c < 5; local_c++)
        bMusicIsLooping[local_c] = 1;
    SVar1 = GetAsyncKeyState(0x75);
    cStack_7 = static_cast<char>(static_cast<unsigned short>(SVar1) >> 8);
    if (cStack_7 != 0) {
        gCdMusic = 0;
        WritePrefs();
    }
    SVar1 = GetAsyncKeyState(0x76);
    cStack_7 = static_cast<char>(static_cast<unsigned short>(SVar1) >> 8);
    if (cStack_7 != 0) {
        gCdMusic = 1;
        WritePrefs();
    }
    field_0x578 = static_cast<char>(0xff);
    if (gbNoSound == 0) {
        field_0x579 = 0;
        field_0x57a = field_0x579;
        field_0x57b = field_0x57a;
        _AIL_startup_0();
        if (gCdMusic == 0) {
            MIDIStartup();
            if (field_0x69e == 0) {
                CDStartup();
                if (field_0x69a == 0) {
                    gMidiEnabled = 0;
                    WritePrefs();
                } else {
                    gCdMusic = 1;
                    WritePrefs();
                }
            }
        } else {
            CDStartup();
            if (field_0x69a == 0) {
                MIDIStartup();
                if (field_0x69e == 0) {
                    gMidiEnabled = 0;
                    WritePrefs();
                } else {
                    gCdMusic = 0;
                    WritePrefs();
                }
            }
        }
        field_0x684 = 1;
        memset(&field_0x3e, 0, 0xae);
        if (gbDontTryDigital == 0 && field_0x36 == 0) {
            p_Var2 = WAVE_init_driver(0x5622, 8, 1, 0);
            field_0x36 = reinterpret_cast<int>(p_Var2);
        }
        if (field_0x36 == 0) {
            gSampleVolume = 0;
            WritePrefs();
        }
        AllocateSampleHandles();
        field_0x3e = 1;
        field_0x50 = 0;
        memset(field_0x590, 0, 0xf0);
        field_0x680 = 1;
    }
    field_0xc = 0x10;
    field_0x10 = -1;
    field_0x32 = 1;
    strcpy(name, "soundManager");
    return 0;
}

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
void soundManager::PollSound(void)
{
    int iVar1;
    long lVar2;
    int local_8;
    if (gbNoSound == 0) {
        if (gCdMusic != 0)
            CDPoll();
        if ((field_0x579 != 0 || field_0x688 != 0) && gMidiEnabled != 0) {
            LogStr("Poll Sound 1");
            if (field_0x688 > 0) {
                LogStr("Poll Sound 1a");
                Process1WindowsMessage();
                if (field_0x578 < 8 || 0xf < field_0x578)
                    gMusicFadeTimer = KBTickCount();
                iVar1 = gMusicFadeTimer;
                lVar2 = KBTickCount();
                field_0x688 = (iVar1 - lVar2) / 0x3c;
                if (field_0x688 < 1)
                    field_0x688 = 0;
                LogStr("Poll Sound 1b");
                if (field_0x688 < 0xb && field_0x578 != field_0x68c) {
                    if (field_0x50 == 0 || bSaveMusicPosition[field_0x578] == 0) {
                        gMusicFadeTimer = KBTickCount();
                    } else if (gCdMusic == 0) {
                        ProcessAssert(field_0x50, __FILE__, __LINE__);
                        field_0x590[field_0x578] = ftell(reinterpret_cast<FILE *>(field_0x50));
                    }
                    field_0x680 = 1;
                    if (bSaveMusicPosition[field_0x68c] == 0)
                        PlayAmbientMusic(field_0x68c, 0, -1);
                    else
                        PlayAmbientMusic(field_0x68c, field_0x590[field_0x68c], -1);
                    iVar1 = gMusicFadeTimer;
                    lVar2 = KBTickCount();
                    field_0x688 = (iVar1 - lVar2) / 0x3c;
                    if (field_0x688 < 1)
                        field_0x688 = 0;
                    field_0x578 = static_cast<char>(field_0x68c);
                }
                if (field_0x688 < 0xb)
                    local_8 = ((0xb - field_0x688) * 0x40) / 0xb;
                else
                    local_8 = ((field_0x688 - 10) * 0x40) / 6;
                if (0x40 < local_8)
                    local_8 = 0x40;
                if (local_8 < 0)
                    local_8 = 0;
                LogStr("Poll Sound 1c");
                if (gCdMusic == 0) {
                    MIDISetVolume();
                } else {
                    local_8 = ((0xb - gMidiEnabled) * local_8 * 0x7f) / 0x280;
                    if (0x7f < local_8)
                        local_8 = 0x7f;
                    if (local_8 < 0)
                        local_8 = 0;
                    CDSetVolume(local_8, 1);
                }
                LogStr("Poll Sound 1d");
            }
            LogStr("Poll Sound 2");
            field_0x579 = 0;
        }
    }
}

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
struct _SAMPLE *soundManager::MemorySample(class sample *param_1)
{
    int iVar1;
    struct _SAMPLE *p_Var2;
    int iVar4;
    short local_10;
    if (gbNoSound == 0) {
        if (field_0x36 == 0) {
            p_Var2 = 0;
        } else if (field_0x684 == 0) {
            p_Var2 = 0;
        } else if (gSampleVolume == 0) {
            p_Var2 = 0;
        } else if (field_0x3e == 0 || param_1->field_0x28 == 0) {
            p_Var2 = 0;
        } else {
            LogStr("Memory Sample 1");
            iVar1 = param_1->field_0x1c;
            iVar4 = iVar1 * 0xc;
            local_10 = static_cast<short>(reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].startChannel);
            while (local_10 < reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].endChannel &&
                   _AIL_sample_status_4(field_0x54[local_10]) != 2)
                local_10++;
            if (reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].endChannel == local_10) {
                if (param_1->field_0x1c == 4) {
                    LogStr("Memory Sample 2a");
                    return 0;
                }
                local_10 = static_cast<short>(reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].currentChannel);
                reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].currentChannel++;
                if (reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].endChannel <=
                    reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].currentChannel) {
                    reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].currentChannel =
                        reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].startChannel;
                    local_10 = static_cast<short>(reinterpret_cast<SampleChannelStruct *>(&SCS)[iVar1].currentChannel);
                }
                StopSample(field_0x54[local_10]);
            }
            p_Var2 = field_0x54[local_10];
            field_0xd8[local_10] = static_cast<char>(param_1->field_0x28);
            reinterpret_cast<short *>(&iLastVolume)[local_10] = static_cast<short>(param_1->field_0x28);
            _AIL_init_sample_4(p_Var2);
            _AIL_set_sample_type_12(p_Var2, param_1->field_0x24, 0);
            _AIL_set_sample_playback_rate_8(p_Var2, param_1->field_0x20);
            _AIL_set_sample_loop_count_8(p_Var2, param_1->field_0x2c);
            _AIL_set_sample_address_12(p_Var2, reinterpret_cast<int>(param_1->field_0x14), param_1->field_0x18);
            if (gSampleVolume == 0) {
                _AIL_set_sample_volume_8(p_Var2, 0);
            } else {
                iVar4 = ConvertVolume(param_1->field_0x28, 100);
                _AIL_set_sample_volume_8(p_Var2, iVar4);
            }
            _AIL_start_sample_4(p_Var2);
            param_1->field_0x10 = p_Var2;
            field_0xec[local_10] = p_Var2;
            field_0x12c[local_10] = reinterpret_cast<int>(param_1->field_0x14);
            field_0x16c[local_10] = param_1->field_0x18;
            LogStr("Memory Sample 2b");
        }
    } else {
        p_Var2 = 0;
    }
    return p_Var2;
}

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
    if (gbNoSound != 0)
        return 0;
    if (gCdMusic == 0) {
        if (field_0x69e == 0)
            return 0;
        return MIDIIsPlaying();
    }
    if (field_0x69a == 0)
        return 0;
    return CDIsPlaying();
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
