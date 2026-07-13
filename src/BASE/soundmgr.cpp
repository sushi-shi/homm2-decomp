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



// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x00528d00) static long gMusicFadeTimer; // ambient-music fade deadline tick (soundManager::SwitchAmbientMusic)
DATA(0x00528db6) static char gMciErrorFlag; // MCI error latch (byte-accessed) (HandleMCIError)
DATA(0x00534970) static WAVEFORMATEX gWaveFormat; // digital-driver PCM format (WAVE_init_driver)

VA(0x004cb630, 0x68)
void HandleMCIError(int param_1, char *param_2)
{
    mciGetErrorStringA(param_1, lpszReturnString, 0xff);
    sprintf(gText, "CD MUSIC ERROR\nDescription: %s\nCall: %s", lpszReturnString, param_2);
    gMciErrorFlag = 1;
    gCdMusic = 0;
    WritePrefs();
    ShutDown(gText);
}

VA(0x004cb6a0, 0xc7)
void soundManager::ValidatePreviousPosition(int param_1)
{
    char buf[20];
    char *cur;
    ProcessAssert(param_1 >= 0 && param_1 < 0x3c, __FILE__, 66);
    if (CDPreviousPosition[param_1][0] == 0)
        return;
    strcpy(buf, CDPreviousPosition[param_1]);
    cur = FindToken(buf, ':');
    if (cur != 0)
        *cur = 0;
    if (atoi(buf) != param_1)
        CDPreviousPosition[param_1][0] = 0;
}

VA(0x004cb770, 0x13c)
void soundManager::CDStop(void)
{
    unsigned int local_18[5];
    if (gbNoSound != 0)
        return;
    if (m_cdReady == 0)
        return;
    wsprintfA(CommandString, "stop CD wait");
    nMCIError = mciSendStringA(CommandString,
                               lpszReturnString, 0xff, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    if (stricmp(lpszReturnString, "stopped") != 0 && m_currentTrack >= 0) {
        wsprintfA(CommandString, "status CD position");
        nMCIError = mciSendStringA(CommandString,
                                   reinterpret_cast<char *>(local_18), 0x14, 0);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
        strcpy(CDPreviousPosition[m_currentTrack], reinterpret_cast<char *>(local_18));
        ValidatePreviousPosition(m_currentTrack);
    }
    CDPlaying = 0;
}

VA(0x004cb8b0, 0xb3)
int soundManager::CDIsPlaying(void)
{
    if (gbNoSound != 0)
        return 0;
    if (m_cdReady == 0)
        return 0;
    wsprintfA(CommandString, "status CD mode");
    nMCIError = mciSendStringA(CommandString,
                               lpszReturnString, 0xff, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    return stricmp(lpszReturnString, "playing") == 0;
}

VA(0x004cb970, 0xf3)
void soundManager::CDStartup(void)
{
    if (gbNoSound != 0)
        return;
    m_cdStarted = 1;
    m_cdReady = 0;
    if (gbNoCDRom != 0)
        return;
    if (gMciErrorFlag != 0)
        return;
    if (gbDontTryRedbook != 0)
        return;
    wsprintfA(CommandString,
              "open %c: type cdaudio alias CD shareable", gcAnimPath[0]);
    nMCIError = mciSendStringA(CommandString,
                               lpszReturnString, 0xff, 0);
    if (nMCIError != 0) {
        m_cdReady = 0;
        gMciErrorFlag = 1;
        gCdMusic = 0;
        WritePrefs();
    } else {
        m_cdReady = 1;
    }
}

VA(0x004cba70, 0xd5)
void soundManager::CDShutdown(void)
{
    if (gbNoSound != 0)
        return;
    if (m_cdReady == 0)
        return;
    wsprintfA(CommandString, "stop CD");
    nMCIError = mciSendStringA(CommandString,
                               lpszReturnString, 0xff, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    wsprintfA(CommandString, "close CD");
    nMCIError = mciSendStringA(CommandString,
                               lpszReturnString, 0xff, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
}

VA(0x004cbb50, 0xe5)
void soundManager::CDSetVolume(int param_1, int param_2)
{
    int local_c;
    unsigned long local_8;
    if (gbNoSound != 0)
        return;
    if (m_cdReady == 0)
        return;
    if (m_auxDevice == -1)
        return;
    if (param_1 == -1)
        local_c = gMidiEnabled;
    else
        local_c = param_1;
    if (local_c != 0) {
        if (param_2 != 0)
            local_c = 0xb - (0xc - local_c / 0xc);
        int local_10 = 0xc - local_c;
        local_8 = local_10 << 0x1c | local_10 << 0xc;
    } else {
        local_8 = 0;
    }
    auxSetVolume(m_auxDevice, local_8);
}

VA(0x004cbc40, 0x473)
void soundManager::CDPlay(int param_1, int param_2, int param_3, int param_4)
{
    long t1;
    long t2;
    long t3;
    char buffer[20];
    int notify;
    HWND wnd;
    HWND wndn;
    if (gbNoSound != 0)
        return;
    if (m_cdReady == 0)
        return;
    if (gMidiEnabled == 0)
        return;
    if (param_1 == -1) {
        CDStop();
        return;
    }
    if (m_currentTrack == param_1 && CDPlaying != 0 && param_4 == 0)
        return;
    m_cdTrack = param_1;
    m_cdPlayFrame = param_3;
    Process1WindowsMessage();
    ServiceSound();
    t1 = KBTickCount();
    wsprintfA(CommandString, "set CD time format tmsf");
    nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xff, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    wsprintfA(CommandString, "status CD mode");
    nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xff, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    if (stricmp(lpszReturnString, "stopped") != 0) {
        wsprintfA(CommandString, "status CD position");
        nMCIError = mciSendStringA(CommandString, buffer, 0x14, 0);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
        strcpy(CDPreviousPosition[m_currentTrack], buffer);
        ValidatePreviousPosition(m_currentTrack);
    }
    t2 = KBTickCount();
    notify = bMusicIsLooping[param_1];
    Process1WindowsMessage();
    ServiceSound();
    if (param_4 == 0 && param_2 != 0 && CDPreviousPosition[param_1][0] != 0) {
        if (param_1 == 0x2b)
            wsprintfA(CommandString, "play CD from %s %s",
                      CDPreviousPosition[param_1], notify ? " notify" : "");
        else
            wsprintfA(CommandString, "play CD from %s to %d%s",
                      CDPreviousPosition[param_1], param_1 + 1,
                      notify ? " notify" : "");
        if (notify != 0)
            wnd = static_cast<HWND>(hwndApp);
        else
            wnd = 0;
        nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xff, wnd);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
    } else {
        if (param_1 == 0x2b)
            wsprintfA(CommandString, "play CD from %d %s",
                      param_1, notify ? " notify" : "");
        else
            wsprintfA(CommandString, "play CD from %d to %d%s",
                      param_1, param_1 + 1, notify ? " notify" : "");
        if (notify != 0)
            wndn = static_cast<HWND>(hwndApp);
        else
            wndn = 0;
        nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xff, wndn);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
    }
    t3 = KBTickCount();
    CDPlaying = 1;
    Process1WindowsMessage();
    ServiceSound();
    if (m_fadeSteps > 0) {
        m_fadeSteps = 0xb;
        gMusicFadeTimer = KBTickCount() + 0x1e0;
        CDSetVolume(0xa, 0);
    } else {
        CDSetVolume(param_3, 0);
    }
    m_currentTrack = static_cast<char>(param_1);
}

VA(0x004cc0c0, 0xf1)
void soundManager::CDPoll(void)
{
    if (gbNoSound != 0)
        return;
    if (gMidiEnabled == 0)
        return;
    if (m_cdReady == 0)
        return;
    if (CDPlaying == 0)
        return;
    if (m_currentTrack < 0)
        return;
    if (bMusicIsLooping[m_currentTrack] == 0)
        return;
    if (m_pollTimer + 3000 > KBTickCount())
        return;
    m_pollTimer = KBTickCount();
    if (CDIsPlaying() == 0)
        CDPlay(m_cdTrack, 0, m_cdPlayFrame, 1);
}

VA(0x004cc1c0, 0xdd)
int soundManager::ConvertVolume(int param_1, int param_2)
{
    int local_8 = 0;
    if (param_2 == 0x65) {
        if (gMidiEnabled >= 1 && gMidiEnabled <= 0xa) {
            local_8 = ((0xb - gMidiEnabled) * param_1) / 10;
            if (local_8 < 1)
                local_8 = 1;
        }
    } else if (gSampleVolume >= 1 && gSampleVolume <= 0xa) {
        local_8 = ((0xb - gSampleVolume) * param_1) / 10;
        if (local_8 < 1)
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
    if (gpSoundManager == 0)
        return;
    gpSoundManager->m_pollToggle ^= 1;
    if (gpSoundManager->m_pollToggle != 0)
        gpSoundManager->m_pollDue = 1;
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
    m_active = 0;
    m_fadeSteps = 0;
    field_0x56c = 0;
    for (local_8 = 0; local_8 < 0x20; local_8++)
        iLastVolume[local_8] = 0;
    memset(&m_ready, 0, 0xae);
    m_samplesReady = 0;
    m_digitalDriver = 0;
    field_0x3a = 0;
    m_cdTrack = 0;
    m_cdPlayFrame = 0;
}

VA(0x004cc410, 0x14a)
struct _DIG_DRIVER *WAVE_init_driver(unsigned long param_1, unsigned short param_2,
                                     unsigned short param_3, unsigned short param_4)
{
    unsigned int numDevs;
    struct _DIG_DRIVER *drvr;
    WAVEOUTCAPSA caps;
    int rc;
    numDevs = waveOutGetNumDevs();
    if (numDevs == 0) {
        drvr = 0;
        return 0;
    }
    if (waveOutGetDevCapsA(0, &caps, 0x34) != 0) {
        MessageBoxA(static_cast<HWND>(hwndApp),
                    "Sound initialization error!  No wave devices found.",
                    "Startup Error", 0);
        drvr = 0;
        return 0;
    }
    if (gbUseWaveout != 0)
        _AIL_set_preference_8(0xf, 1);
    gWaveFormat.wFormatTag = 1;
    gWaveFormat.nChannels = param_3;
    gWaveFormat.nSamplesPerSec = param_1;
    gWaveFormat.nAvgBytesPerSec = (param_2 >> 3) * param_3 * param_1;
    gWaveFormat.nBlockAlign = (param_2 >> 3) * param_3;
    gWaveFormat.wBitsPerSample = param_2;
    rc = _AIL_waveOutOpen_16(&drvr, 0, 0, &gWaveFormat);
    if (rc != 0) {
        if (param_4 != 0)
            MessageBoxA(static_cast<HWND>(hwndApp), _AIL_last_error_0(),
                        "Sound initialization error!", 0);
        drvr = 0;
        return 0;
    }
    return drvr;
}

VA(0x004cc560, 0x3a8)
int soundManager::Open(int param_1)
{
    union { int i; unsigned char b[4]; } SVar1;
    struct _DIG_DRIVER *p_Var2;
    int local_c;
    m_cdStarted = 0;
    m_midiStarted = 0;
    m_cdReady = 0;
    m_midiReady = 0;
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
    SVar1.i = GetAsyncKeyState(0x75);
    if (SVar1.b[1] != 0) {
        gCdMusic = 0;
        WritePrefs();
    }
    SVar1.i = GetAsyncKeyState(0x76);
    if (SVar1.b[1] != 0) {
        gCdMusic = 1;
        WritePrefs();
    }
    m_currentTrack = static_cast<char>(0xff);
    if (gbNoSound == 0) {
        m_pollRequested = 0;
        m_pollDue = m_pollRequested;
        m_pollToggle = m_pollDue;
        _AIL_startup_0();
        if (gCdMusic == 0) {
            MIDIStartup();
            if (m_midiReady == 0) {
                CDStartup();
                if (m_cdReady == 0) {
                    gMidiEnabled = 0;
                    WritePrefs();
                } else {
                    gCdMusic = 1;
                    WritePrefs();
                }
            }
        } else {
            CDStartup();
            if (m_cdReady == 0) {
                MIDIStartup();
                if (m_midiReady == 0) {
                    gMidiEnabled = 0;
                    WritePrefs();
                } else {
                    gCdMusic = 0;
                    WritePrefs();
                }
            }
        }
        m_samplesReady = 1;
        memset(&m_ready, 0, 0xae);
        if (gbDontTryDigital == 0 && m_digitalDriver == 0) {
            p_Var2 = WAVE_init_driver(0x5622, 8, 1, 0);
            m_digitalDriver = reinterpret_cast<int>(p_Var2);
        }
        if (m_digitalDriver == 0) {
            gSampleVolume = 0;
            WritePrefs();
        }
        AllocateSampleHandles();
        m_ready = 1;
        m_midiFile = 0;
        memset(m_savedTrackPositions, 0, 0xf0);
        m_fading = 1;
    }
    field_0xc = 0x10;
    field_0x10 = -1;
    m_active = 1;
    strcpy(name, "soundManager");
    return 0;
}

VA(0x004cc910, 0x91)
void soundManager::AllocateSampleHandles(void)
{
    int local_8;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    for (local_8 = 0; local_8 < 0xe; local_8++) {
        m_sampleHandles[local_8] = _AIL_allocate_sample_handle_4(m_digitalDriver);
        if (m_sampleHandles[local_8] == 0)
            break;
    }
    m_numSampleHandles = local_8;
}

VA(0x004cc9b0, 0x96)
void soundManager::Close(void)
{
    if (m_active != 1)
        return;
    if (gbNoSound == 0) {
        LogStr("Shutting down CD audio");
        CDShutdown();
        LogStr("Shutting down MIDI");
        MIDIShutdown();
        LogStr("Shutting down AIL");
        _AIL_shutdown_0();
        LogStr("Sound shut down");
    }
    m_active = 0;
    gbNoSound = 1;
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
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    if (m_samplesReady == 0)
        return;
    LogStr("SAS 1");
    for (local_8 = 0; local_8 < m_numSampleHandles; local_8++) {
        if (_AIL_sample_status_4(m_sampleHandles[local_8]) == 4)
            _AIL_end_sample_4(m_sampleHandles[local_8]);
    }
    m_fadeSteps = 0;
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

VA(0x004ccbc0, 0xb1)
void soundManager::StopSample(struct _SAMPLE *param_1)
{
    int local_c;
    int local_10;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    local_10 = 0;
    LogStr("Stop Sample 1");
    if (m_sampleHandles[0] == param_1)
        local_10 = 1;
    _AIL_end_sample_4(param_1);
    if (local_10 != 0) {
        for (local_c = 0; local_c < 10; local_c++) {
            ServiceSound();
            DelayMilli(1);
        }
    }
    LogStr("Stop Sample 2");
}

VA(0x004ccc80, 0x202)
void soundManager::ModifySample(struct _SAMPLE *param_1, short param_2, long param_3)
{
    int iVar1;
    int local_10;
    int local_8;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (m_ready == 0)
        return;
    LogStr("Modify Sample 1");
    local_8 = -1;
    for (local_10 = 0; local_10 < m_numSampleHandles; local_10++) {
        if (m_sampleHandles[local_10] == param_1)
            local_8 = local_10;
    }
    switch (param_2) {
    case 1:
    case 100:
        iVar1 = ConvertVolume(param_3, 100);
        _AIL_set_sample_volume_8(param_1, iVar1);
        if (local_8 >= 0)
            iLastVolume[local_8] = static_cast<short>(param_3);
        break;
    case 5:
        _AIL_start_sample_4(param_1);
        break;
    case 0x65:
        ProcessAssert(gCdMusic == 0, __FILE__, __LINE__);
        iVar1 = ConvertVolume(param_3, 0x65);
        _AIL_set_sample_volume_8(param_1, iVar1);
        if (local_8 >= 0)
            iLastVolume[local_8] = static_cast<short>(param_3);
    }
    Process1WindowsMessage();
    LogStr("Modify Sample 2");
}

VA(0x004cce90, 0xa3)
long soundManager::DigitalReport(struct _SAMPLE *param_1, short param_2)
{
    if (gbNoSound != 0)
        return 0;
    if (m_digitalDriver == 0)
        return 0;
    switch (param_2) {
    case 1:
        return _AIL_sample_volume_4(param_1);
    case 4:
        return _AIL_sample_status_4(param_1) == 4;
    }
    return 0;
}

VA(0x004ccf40, 0xe8)
void soundManager::AdjustSoundVolumes(void)
{
    int local_c;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    if (m_samplesReady == 0)
        return;
    LogStr("Adjust Sound Volumes 1");
    for (local_c = 1; local_c < m_numSampleHandles; local_c++) {
        struct _SAMPLE *p_Var1 = m_sampleHandles[local_c];
        if (gSampleVolume != 0) {
            if (DigitalReport(p_Var1, 4) != 0)
                ModifySample(p_Var1, 100, iLastVolume[local_c]);
        } else {
            ModifySample(p_Var1, 1, 0);
        }
    }
    LogStr("Adjust Sound Volumes 2");
}

VA(0x004cd030, 0xee)
void soundManager::AdjustMusicVolumes(void)
{
    if (gbNoSound != 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (m_currentTrack < 0)
        return;
    LogStr("Adjust Music Volumes 1");
    int local_4 = 0;
    if (bSaveMusicPosition[m_currentTrack] != 0)
        local_4 = 1;
    if (gMidiEnabled != 0) {
        if (gCdMusic != 0)
            CDSetVolume(-1, 0);
        else
            MIDISetVolume();
    } else {
        if (gCdMusic != 0)
            CDSetVolume(-1, 0);
        else
            MIDISetVolume();
    }
    LogStr("Adjust Music Volumes 2");
}

VA(0x004cd120, 0x3a)
void soundManager::ForcePollSound(void)
{
    if (gbNoSound != 0)
        return;
    m_pollRequested = 1;
    PollSound();
}

VA(0x004cd160, 0xe3)
void soundManager::SetMusicQuality(int param_1)
{
    int local_8;
    if (gbNoSound != 0)
        return;
    if (m_ready == 0)
        return;
    if (gMidiEnabled == 0)
        return;
    if (m_cdReady == 0)
        return;
    if (gCdMusic != 0) {
        local_8 = m_currentTrack;
        CDStop();
        m_currentTrack = static_cast<char>(0xff);
    } else {
        local_8 = m_currentTrack;
        MIDIStop();
    }
    memset(m_savedTrackPositions, 0, 0xf0);
    gCdMusic = param_1;
    if (local_8 >= 0)
        PlayAmbientMusic(local_8, 0, -1);
}

VA(0x004cd250, 0xc5)
void soundManager::PlayAmbientMusic(int param_1, long param_2, int param_3)
{
    if (gbNoSound != 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (m_ready == 0)
        return;
    if (m_currentTrack == param_1)
        return;
    if (gMidiEnabled == 0) {
        m_currentTrack = static_cast<char>(param_1);
    } else {
        if (gCdMusic != 0)
            CDPlay(param_1, param_2, -1, 0);
        else
            MIDIPlay(param_1);
        m_currentTrack = static_cast<char>(param_1);
    }
}

VA(0x004cd320, 0x38f)
void soundManager::PollSound(void)
{
    int volume;
    struct _SAMPLE *smp;
    long delta;
    int snap;
    long now;
    if (gbNoSound != 0)
        return;
    if (gCdMusic != 0)
        CDPoll();
    if (m_pollRequested == 0 && m_fadeSteps == 0)
        return;
    if (gMidiEnabled == 0)
        return;
    LogStr("Poll Sound 1");
    if (m_fadeSteps > 0) {
        LogStr("Poll Sound 1a");
        Process1WindowsMessage();
        if (m_currentTrack < 8 || m_currentTrack > 0xf)
            gMusicFadeTimer = KBTickCount();
        delta = gMusicFadeTimer - KBTickCount();
        m_fadeSteps = delta / 0x3c;
        if (m_fadeSteps < 1)
            m_fadeSteps = 0;
        LogStr("Poll Sound 1b");
        if (m_fadeSteps <= 0xa && m_currentTrack != m_fadeTargetTrack) {
            if (m_midiFile != 0 && bSaveMusicPosition[m_currentTrack] != 0) {
                if (gCdMusic == 0) {
                    ProcessAssert(m_midiFile, __FILE__, 0x61a);
                    m_savedTrackPositions[m_currentTrack] =
                        ftell(reinterpret_cast<FILE *>(m_midiFile));
                }
            } else {
                gMusicFadeTimer = KBTickCount();
            }
            m_fading = 1;
            if (bSaveMusicPosition[m_fadeTargetTrack] != 0)
                PlayAmbientMusic(m_fadeTargetTrack,
                                 m_savedTrackPositions[m_fadeTargetTrack], -1);
            else
                PlayAmbientMusic(m_fadeTargetTrack, 0, -1);
            now = gMusicFadeTimer - KBTickCount();
            m_fadeSteps = now / 0x3c;
            if (m_fadeSteps < 1)
                m_fadeSteps = 0;
            m_currentTrack = static_cast<char>(m_fadeTargetTrack);
        }
        snap = m_fadeSteps;
        if (m_fadeSteps <= 0xa)
            volume = (0xb - m_fadeSteps) * 0x40 / 0xb;
        else
            volume = (m_fadeSteps - 0xa) * 0x40 / 6;
        if (volume > 0x40)
            volume = 0x40;
        if (volume < 0)
            volume = 0;
        LogStr("Poll Sound 1c");
        smp = m_sampleHandles[0];
        if (gCdMusic != 0) {
            volume = (0xb - gMidiEnabled) * volume * 0x7f / 0x280;
            if (volume > 0x7f)
                volume = 0x7f;
            if (volume < 0)
                volume = 0;
            CDSetVolume(volume, 1);
        } else {
            MIDISetVolume();
        }
        LogStr("Poll Sound 1d");
    }
    LogStr("Poll Sound 2");
    m_pollRequested = 0;
}

VA(0x004cd6b0, 0x138)
void soundManager::SwitchAmbientMusic(int param_1)
{
    if (gbNoSound != 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (gMidiEnabled == 0) {
        m_currentTrack = static_cast<char>(param_1);
    } else if (MusicPlaying() == 0) {
        PlayAmbientMusic(param_1, 0, -1);
    } else if (m_currentTrack != param_1) {
        LogStr("Switch Ambient Music 1");
        Process1WindowsMessage();
        if ((m_fadeSteps != 0 && m_fadeTargetTrack != param_1) ||
            (m_fadeSteps == 0 && m_currentTrack != param_1)) {
            if (m_fadeSteps < 0xb) {
                m_fadeSteps = 0xb;
                gMusicFadeTimer = KBTickCount() + 900;
            }
            m_fadeTargetTrack = param_1;
            PollSound();
        }
        LogStr("Switch Ambient Music 2");
    }
}

VA(0x004cd7f0, 0x28f)
struct _SAMPLE *soundManager::MemorySample(class sample *param_1)
{
    struct _SAMPLE *smp;
    int *params;
    short ch;
    SampleChannelStruct *scs;
    if (gbNoSound != 0)
        return 0;
    if (m_digitalDriver == 0)
        return 0;
    if (m_samplesReady == 0)
        return 0;
    if (gSampleVolume == 0)
        return 0;
    params = reinterpret_cast<int *>(param_1) + 4;
    if (m_ready == 0 || params[6] == 0)
        return 0;
    LogStr("Memory Sample 1");
    scs = &SCS[params[3]];
    for (ch = static_cast<short>(scs->startChannel); scs->endChannel > ch; ch++) {
        if (_AIL_sample_status_4(m_sampleHandles[ch]) == 2)
            break;
    }
    if (scs->endChannel == ch) {
        if (params[3] == 4) {
            LogStr("Memory Sample 2a");
            return 0;
        }
        ch = static_cast<short>(scs->currentChannel);
        scs->currentChannel++;
        if (scs->endChannel <= scs->currentChannel) {
            scs->currentChannel = scs->startChannel;
            ch = static_cast<short>(scs->currentChannel);
        }
        StopSample(m_sampleHandles[ch]);
    }
    smp = m_sampleHandles[ch];
    m_channelVolumes[ch] = static_cast<char>(params[6]);
    iLastVolume[ch] = static_cast<short>(params[6]);
    _AIL_init_sample_4(smp);
    _AIL_set_sample_type_12(smp, params[5], 0);
    _AIL_set_sample_playback_rate_8(smp, params[4]);
    _AIL_set_sample_loop_count_8(smp, params[7]);
    _AIL_set_sample_address_12(smp, params[1], params[2]);
    if (gSampleVolume != 0)
        _AIL_set_sample_volume_8(smp, ConvertVolume(params[6], 0x64));
    else
        _AIL_set_sample_volume_8(smp, 0);
    _AIL_start_sample_4(smp);
    params[0] = reinterpret_cast<int>(smp);
    m_channelSamples[ch] = smp;
    m_sampleAddrLow[ch] = params[1];
    m_sampleAddrHigh[ch] = params[2];
    LogStr("Memory Sample 2b");
    return smp;
}

VA(0x004cda80, 0x16)
void soundManager::GetNumberCDDrives(void) {}

VA(0x004cdaa0, 0x2e)
void soundManager::ServiceSound(void)
{
    if (gbNoSound != 0)
        return;
    _AIL_serve_0();
}

VA(0x004cdad0, 0x7f)
int soundManager::MusicPlaying(void)
{
    if (gbNoSound != 0)
        return 0;
    if (gCdMusic == 0) {
        if (m_midiReady == 0)
            return 0;
        return MIDIIsPlaying();
    }
    if (m_cdReady == 0)
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
DATA(0x0051f018) char *digitalDriverNames[14];
DATA(0x0051f050) SampleChannelStruct SCS[4];
DATA(0x0051f080) char CDPreviousPosition[60][15];
DATA(0x0051f404) int CDWaiting;
DATA(0x0051f408) int CDPlaying;
DATA(0x0051f40c) int iCalibrateLoop;
DATA(0x00534980) char lpszReturnString[0x100];
DATA(0x00534a80) unsigned long nMCIError;
DATA(0x00534a88) short iLastVolume[0x20];
DATA(0x00534ac8) char CommandString[0x100];
