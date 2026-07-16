// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\soundmgr.obj   from: .\basewin.lib
// functions: 34   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/soundManager.h>
#include <BASE/sample.h>
#include <BASE/soundmgr.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <mss.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <BASE/Misc.h>

#define MUSIC_FADE_TIMER_SLOT 4


// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x00534970) static PCMWAVEFORMAT gWaveFormat; // digital-driver PCM format (WAVE_init_driver)

VA(0x004cb630, 0x68)
void HandleMCIError(int param_1, char *param_2)
{
    mciGetErrorStringA(param_1, lpszReturnString, 0xff);
    sprintf(gText, "CD MUSIC ERROR\nDescription: %s\nCall: %s", lpszReturnString, param_2);
    gConfig.mciError = 1;
    gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
    WritePrefs();
    ShutDown(gText);
}

VA(0x004cb6a0, 0xc7)
void soundManager::ValidatePreviousPosition(int param_1)
{
    char buf[20];
    char *cur;
    H2_ASSERT(param_1 >= 0 && param_1 < 0x3c,
              "I:\\Projects\\Heroes\\Prog\\BASE\\soundmgr.cpp", 66);
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
    char position[20];
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
        nMCIError = mciSendStringA(CommandString, position, sizeof(position), 0);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
        strcpy(CDPreviousPosition[m_currentTrack], position);
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
    if (gConfig.mciError != 0)
        return;
    if (gbDontTryRedbook != 0)
        return;
    wsprintfA(CommandString,
              "open %c: type cdaudio alias CD shareable", gcAnimPath[0]);
    nMCIError = mciSendStringA(CommandString,
                               lpszReturnString, 0xff, 0);
    if (nMCIError != 0) {
        m_cdReady = 0;
        gConfig.mciError = 1;
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
        WritePrefs();
        return;
    }
    m_cdReady = 1;
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
        local_c = gConfig.musicVolume;
    else
        local_c = param_1;
    if (local_c != 0) {
        int local_10;
        if (param_2 != 0)
            local_10 = 0xc - (0xb - local_c / 0xc);
        else
            local_10 = 0xc - local_c;
        local_10 <<= 0xc;
        local_8 = local_10 << 0x10 | local_10;
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
    if (gConfig.musicVolume == 0)
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
        glTimers[MUSIC_FADE_TIMER_SLOT] = KBTickCount() + 0x1e0;
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
    if (gConfig.musicVolume == 0)
        return;
    if (m_cdReady == 0)
        return;
    if (CDPlaying == 0 || m_currentTrack < 0)
        return;
    {
        if (bMusicIsLooping[m_currentTrack] == 0)
            return;
        if ((KBTickCount() | 0) < m_pollTimer + 3000)
            return;
        m_pollTimer = KBTickCount();
        if (CDIsPlaying() == 0)
            CDPlay(m_cdTrack, 0, m_cdPlayFrame, 1);
    }
}

VA(0x004cc1c0, 0xdd)
int soundManager::ConvertVolume(int param_1, int param_2)
{
    int local_8 = 0;
    if (param_2 == SOUND_VOLUME_MUSIC) {
        if (gConfig.musicVolume >= 1 && gConfig.musicVolume <= 0xa) {
            local_8 = ((0xb - gConfig.musicVolume) * param_1) / 10;
            if (local_8 < 1)
                local_8 = 1;
        }
    } else if (gConfig.soundVolume >= 1 && gConfig.soundVolume <= 0xa) {
        local_8 = ((0xb - gConfig.soundVolume) * param_1) / 10;
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
soundManager::soundManager(void) : baseManager(), field_0x574(1)
{
    int local_8;
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
        AIL_set_preference(0xf, 1);
    gWaveFormat.wf.wFormatTag = 1;
    gWaveFormat.wf.nChannels = param_3;
    gWaveFormat.wf.nSamplesPerSec = param_1;
    gWaveFormat.wf.nAvgBytesPerSec = (param_2 >> 3) * param_3 * param_1;
    gWaveFormat.wf.nBlockAlign = (param_2 >> 3) * param_3;
    gWaveFormat.wBitsPerSample = param_2;
    rc = AIL_waveOutOpen(&drvr, 0, 0, &gWaveFormat.wf);
    if (rc != 0) {
        if (param_4 != 0)
            MessageBoxA(static_cast<HWND>(hwndApp), AIL_last_error(),
                        "Sound initialization error!", 0);
        drvr = 0;
        return 0;
    }
    return drvr;
}

VA(0x004cc560, 0x3a8)
int soundManager::Open(int)
{
    int keyState;
    int musicTrack;

    m_cdStarted = 0;
    m_midiStarted = 0;
    m_cdReady = 0;
    m_midiReady = 0;

    memset(bSaveMusicPosition, 0, MIDI_TRACK_COUNT);
    memset(bMusicIsLooping, 0, MIDI_TRACK_COUNT);

    bSaveMusicPosition[16] = 1;
    bSaveMusicPosition[18] = 1;
    bSaveMusicPosition[14] = 1;
    bSaveMusicPosition[15] = 1;
    bSaveMusicPosition[11] = 1;
    bSaveMusicPosition[13] = 1;
    bSaveMusicPosition[17] = 1;
    bSaveMusicPosition[12] = 1;
    bSaveMusicPosition[28] = 1;
    bSaveMusicPosition[42] = 1;

    bMusicIsLooping[16] = 1;
    bMusicIsLooping[18] = 1;
    bMusicIsLooping[14] = 1;
    bMusicIsLooping[15] = 1;
    bMusicIsLooping[11] = 1;
    bMusicIsLooping[13] = 1;
    bMusicIsLooping[17] = 1;
    bMusicIsLooping[12] = 1;
    bMusicIsLooping[8] = 1;
    bMusicIsLooping[9] = 1;
    bMusicIsLooping[5] = 1;
    bMusicIsLooping[6] = 1;
    bMusicIsLooping[10] = 1;
    bMusicIsLooping[7] = 1;
    bMusicIsLooping[42] = 1;
    bMusicIsLooping[28] = 1;
    bMusicIsLooping[43] = 1;
    bMusicIsLooping[22] = 1;
    bMusicIsLooping[24] = 1;
    for (musicTrack = 2; musicTrack <= 4; musicTrack++)
        bMusicIsLooping[musicTrack] = 1;

    keyState = GetAsyncKeyState(VK_F6);
    if (HIBYTE(keyState)) {
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
        WritePrefs();
    }
    keyState = GetAsyncKeyState(VK_F7);
    if (HIBYTE(keyState)) {
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
        WritePrefs();
    }

    m_currentTrack = -1;
    if (gbNoSound != 0) {
        goto managerReady;
    }
    m_pollToggle = m_pollDue = m_pollRequested = 0;
    AIL_startup();

    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI) {
        CDStartup();
        if (m_cdReady == 0) {
            MIDIStartup();
            if (m_midiReady != 0) {
                gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
                WritePrefs();
            } else {
                gConfig.musicVolume = 0;
                WritePrefs();
            }
        }
    } else {
        MIDIStartup();
        if (m_midiReady == 0) {
            CDStartup();
            if (m_cdReady != 0) {
                gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
                WritePrefs();
            } else {
                gConfig.musicVolume = 0;
                WritePrefs();
            }
        }
    }

    m_samplesReady = 1;
    memset(&m_ready, 0, 0xae);
    if (gbDontTryDigital == 0 && m_digitalDriver == 0)
        m_digitalDriver = WAVE_init_driver(22050, 8, 1, 0);
    if (m_digitalDriver == 0) {
        gConfig.soundVolume = 0;
        WritePrefs();
    }
    AllocateSampleHandles();
    m_ready = 1;
    m_midiFile = 0;
    memset(m_savedTrackPositions, 0, sizeof(m_savedTrackPositions));
    m_fading = 1;

managerReady:
    m_messageMask = 0x10;
    m_priority = -1;
    m_active = 1;
    strcpy(m_name, "soundManager");
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
        m_sampleHandles[local_8] = AIL_allocate_sample_handle(m_digitalDriver);
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
    if (gbNoSound != 0)
        goto soundClosed;
    LogStr("Shutting down CD audio");
    CDShutdown();
    LogStr("Shutting down MIDI");
    MIDIShutdown();
    LogStr("Shutting down AIL");
    AIL_shutdown();
    LogStr("Sound shut down");
soundClosed:
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
    short sampleIdx;
    int waitCounter;
    int sampleStatus;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    if (m_samplesReady == 0)
        return;
    LogStr("SAS 1");
    for (sampleIdx = 0; sampleIdx < m_numSampleHandles; sampleIdx++) {
        sampleStatus = AIL_sample_status(m_sampleHandles[sampleIdx]);
        if (sampleStatus == SOUND_SAMPLE_STATUS_PLAYING)
            AIL_end_sample(m_sampleHandles[sampleIdx]);
    }
    m_fadeSteps = 0;
    if (param_1 != 0) {
        if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI)
            CDStop();
        else
            MIDIStop();
    }
    for (waitCounter = 0; waitCounter < 5; waitCounter++) {
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
    AIL_end_sample(param_1);
    if (local_10 != 0) {
        for (local_c = 0; local_c < 10; local_c++) {
            ServiceSound();
            DelayMilli(1);
        }
    }
    LogStr("Stop Sample 2");
}

// @early-stop
// The explicit 0x202-byte CodeView range is raw-exact after relocation-union masking;
// retail's enclosing row has two trailing padding bytes. Frame/slots and CFG are exact.
// Candidate has 23 relocations versus retail's 20: candidate-only calls +0xc8/+0x11a
// resolve to the linked AIL_set_sample_volume IAT VA 0x0053a78c, and +0x141 resolves
// to AIL_start_sample at 0x0053a780, exactly the immediates embedded by retail. Other
// residual identities are local switch labels, three $SG versus delinker-named string
// constants with the same bytes, and gConfig+0xaa at the same retail VA 0x00528dca.
VA(0x004ccc80, 0x202)
void soundManager::ModifySample(struct _SAMPLE *sampleHandle, short operation, long value)
{
    int foundChannel;
    int unused;
    int sampleIndex;

    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (m_ready == 0)
        return;

    LogStr("Modify Sample 1");
    foundChannel = -1;
    for (sampleIndex = 0; sampleIndex < m_numSampleHandles; sampleIndex++) {
        if (m_sampleHandles[sampleIndex] == sampleHandle)
            foundChannel = sampleIndex;
    }

    switch (operation) {
    case SOUND_SAMPLE_OPERATION_VOLUME:
    case SOUND_SAMPLE_OPERATION_EFFECT_VOLUME:
        AIL_set_sample_volume(sampleHandle, ConvertVolume(value, SOUND_VOLUME_EFFECT));
        if (foundChannel >= 0)
            iLastVolume[foundChannel] = static_cast<short>(value);
        break;
    case SOUND_SAMPLE_OPERATION_MUSIC_VOLUME:
        H2_ASSERT(gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI, "I:\\Projects\\Heroes\\Prog\\BASE\\soundmgr.cpp", 0x52f);
        AIL_set_sample_volume(sampleHandle, ConvertVolume(value, SOUND_VOLUME_MUSIC));
        if (foundChannel >= 0)
            iLastVolume[foundChannel] = static_cast<short>(value);
        break;
    case SOUND_SAMPLE_OPERATION_START:
        AIL_start_sample(sampleHandle);
        break;
    }

    Process1WindowsMessage();
    LogStr("Modify Sample 2");
}

VA(0x004cce90, 0xa3)
long soundManager::DigitalReport(struct _SAMPLE *param_1, short param_2)
{
    int sampleStatus;

    if (gbNoSound != 0)
        return 0;
    if (m_digitalDriver == 0)
        return 0;
    switch (param_2) {
    case SOUND_DIGITAL_REPORT_VOLUME:
        return AIL_sample_volume(param_1);
    case SOUND_DIGITAL_REPORT_PLAYING:
        sampleStatus = AIL_sample_status(param_1);
        return sampleStatus == SOUND_SAMPLE_STATUS_PLAYING;
    }
    return 0;
}

VA(0x004ccf40, 0xe8)
void soundManager::AdjustSoundVolumes(void)
{
    int sampleIndex;
    struct _SAMPLE *sampleHandle;

    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    if (m_samplesReady == 0)
        return;

    LogStr("Adjust Sound Volumes 1");
    for (sampleIndex = 1; sampleIndex < m_numSampleHandles; sampleIndex++) {
        sampleHandle = m_sampleHandles[sampleIndex];
        if (gConfig.soundVolume != 0) {
            if (DigitalReport(sampleHandle, SOUND_DIGITAL_REPORT_PLAYING) != 0)
                ModifySample(sampleHandle, SOUND_SAMPLE_OPERATION_EFFECT_VOLUME,
                             iLastVolume[sampleIndex]);
        } else {
            ModifySample(sampleHandle, SOUND_SAMPLE_OPERATION_VOLUME, 0);
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
    if (gConfig.musicVolume != 0) {
        if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI)
            CDSetVolume(-1, 0);
        else
            MIDISetVolume();
    } else {
        if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI)
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
    if (gConfig.musicVolume == 0)
        return;
    if (m_cdReady == 0)
        return;
    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI) {
        local_8 = m_currentTrack;
        CDStop();
        m_currentTrack = MIDI_NO_TRACK;
    } else {
        local_8 = m_currentTrack;
        MIDIStop();
    }
    memset(m_savedTrackPositions, 0, 0xf0);
    gConfig.musicSource = param_1;
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
    if (gConfig.musicVolume == 0) {
        m_currentTrack = static_cast<char>(param_1);
        return;
    }
    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI)
        CDPlay(param_1, param_2, -1, 0);
    else
        MIDIPlay(param_1);
    m_currentTrack = static_cast<char>(param_1);
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
    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI)
        CDPoll();
    if (m_pollRequested == 0 && m_fadeSteps == 0)
        return;
    if (gConfig.musicVolume == 0)
        return;
    LogStr("Poll Sound 1");
    if (m_fadeSteps > 0) {
        LogStr("Poll Sound 1a");
        Process1WindowsMessage();
        if (m_currentTrack < 8 || m_currentTrack > 0xf)
            glTimers[MUSIC_FADE_TIMER_SLOT] = KBTickCount();
        delta = glTimers[MUSIC_FADE_TIMER_SLOT] - KBTickCount();
        m_fadeSteps = delta / 0x3c;
        if (m_fadeSteps < 1)
            m_fadeSteps = 0;
        LogStr("Poll Sound 1b");
        if (m_fadeSteps <= 0xa && m_currentTrack != m_fadeTargetTrack) {
            if (m_midiFile != 0 && bSaveMusicPosition[m_currentTrack] != 0) {
                if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
                    H2_ASSERT(reinterpret_cast<int>(m_midiFile), "I:\\Projects\\Heroes\\Prog\\BASE\\soundmgr.cpp",
                              0x61a);
                    m_savedTrackPositions[m_currentTrack] =
                        ftell(m_midiFile);
                }
            } else {
                glTimers[MUSIC_FADE_TIMER_SLOT] = KBTickCount();
            }
            m_fading = 1;
            if (bSaveMusicPosition[m_fadeTargetTrack] != 0)
                PlayAmbientMusic(m_fadeTargetTrack,
                                 m_savedTrackPositions[m_fadeTargetTrack], -1);
            else
                PlayAmbientMusic(m_fadeTargetTrack, 0, -1);
            now = glTimers[MUSIC_FADE_TIMER_SLOT] - KBTickCount();
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
        if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI) {
            volume = (0xb - gConfig.musicVolume) * volume * 0x7f / 0x280;
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
    if (gConfig.musicVolume == 0) {
        m_currentTrack = static_cast<char>(param_1);
        return;
    }
    if (MusicPlaying() == 0) {
        PlayAmbientMusic(param_1, 0, -1);
        return;
    }
    if (m_currentTrack == param_1)
        return;
    LogStr("Switch Ambient Music 1");
    Process1WindowsMessage();
    if ((m_fadeSteps != 0 && m_fadeTargetTrack != param_1) ||
        (m_fadeSteps == 0 && m_currentTrack != param_1)) {
        if (m_fadeSteps <= 0xa) {
            m_fadeSteps = 0xb;
            glTimers[MUSIC_FADE_TIMER_SLOT] = KBTickCount() + 900;
        }
        m_fadeTargetTrack = param_1;
        PollSound();
    }
    LogStr("Switch Ambient Music 2");
}

// @semantic
// The sample class fields are accessed directly; the former `int *` overlay at +0x10
// was a decompiler artifact. Semantics and CFG are complete and all 13 external
// relocations agree with no base-only target. Candidate uses a 0x10-byte frame and
// ends at +0x280; retail uses a 0x14-byte frame and ends at +0x288. The first code
// divergence is retail +0x5f, which materializes `sample + 0x10` in a stack local,
// while the typed source reloads the sample argument for each field. Revisit only
// with evidence for a genuine named payload subobject; do not restore the raw overlay.
VA(0x004cd7f0, 0x28f)
struct _SAMPLE *soundManager::MemorySample(class sample *sampleResource)
{
    struct _SAMPLE *smp;
    short ch;
    SampleChannelStruct *scs;
    if (gbNoSound != 0)
        return 0;
    if (m_digitalDriver == 0)
        return 0;
    if (m_samplesReady == 0)
        return 0;
    if (gConfig.soundVolume == 0)
        return 0;
    if (m_ready == 0 || sampleResource->m_volume == 0)
        return 0;
    LogStr("Memory Sample 1");
    scs = &SCS[sampleResource->m_channelType];
    for (ch = static_cast<short>(scs->startChannel); scs->endChannel > ch; ch++) {
        if (AIL_sample_status(m_sampleHandles[ch]) == SOUND_SAMPLE_STATUS_DONE)
            break;
    }
    if (scs->endChannel == ch) {
        if (sampleResource->m_channelType == 4) {
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
    m_channelVolumes[ch] = static_cast<char>(sampleResource->m_volume);
    iLastVolume[ch] = static_cast<short>(sampleResource->m_volume);
    AIL_init_sample(smp);
    AIL_set_sample_type(smp, sampleResource->m_format, 0);
    AIL_set_sample_playback_rate(smp, sampleResource->m_sampleRate);
    AIL_set_sample_loop_count(smp, sampleResource->m_loopCount);
    AIL_set_sample_address(smp, sampleResource->m_data, sampleResource->m_size);
    if (gConfig.soundVolume != 0)
        AIL_set_sample_volume(smp, ConvertVolume(sampleResource->m_volume, SOUND_VOLUME_EFFECT));
    else
        AIL_set_sample_volume(smp, 0);
    AIL_start_sample(smp);
    sampleResource->m_activeSample = smp;
    m_channelSamples[ch] = smp;
    m_channelSampleData[ch] = sampleResource->m_data;
    m_channelSampleSizes[ch] = sampleResource->m_size;
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
    AIL_serve();
}

VA(0x004cdad0, 0x7f)
int soundManager::MusicPlaying(void)
{
    if (gbNoSound != 0)
        return 0;
    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI) {
        if (m_cdReady == 0)
            return 0;
        return CDIsPlaying();
    } else {
        if (m_midiReady == 0)
            return 0;
        return MIDIIsPlaying();
    }
}


// ===== vtable soundManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004cc560, 0x3a8)  int soundManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004cc9b0, 0x96)  void soundManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004cca50, 0x1a)  int soundManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(soundManager, 0x004eba20);

// ---- globals (definitions, RVA order) ----
DATA(0x0051f018) char *digitalDriverNames[14] = {
    "", "ultra.dig", "sndsys.dig", "sndscape.dig", "jammer.dig",
    "audiodrv.dig", "proaudio.dig", "rap10.dig", "iwav.dig",
    "nvdig.dig", "sb16.dig", "sbpro.dig", "sblaster.dig", 0
};
DATA(0x0051f050) SampleChannelStruct SCS[4] = {
    { 0, 1, 0 },
    { 1, 2, 1 },
    { 2, 6, 2 },
    { 6, 16, 6 }
};
DATA(0x0051f080) char CDPreviousPosition[60][15] = { 0 };
DATA(0x0051f404) int CDWaiting = -1;
DATA(0x0051f408) int CDPlaying = 0;
DATA(0x0051f40c) int iCalibrateLoop = 0;
DATA(0x00534980) char lpszReturnString[0x100];
DATA(0x00534a80) unsigned long nMCIError;
DATA(0x00534a88) short iLastVolume[0x20];
DATA(0x00534ac8) char CommandString[0x100];
