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

// __FILE__ for the NWC memory/assert tracking (reloc-masked path string).
#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\soundmgr.cpp"

// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x00534970) static PCMWAVEFORMAT gWaveFormat; // digital-driver PCM format (WAVE_init_driver)

H2_ENUM_CLASS_BEGIN(SoundStateSpan)
    SOUND_STATE_RESET_SPAN = 0xae
H2_ENUM_CLASS_END(SoundStateSpan)

VA(0x004cb630, 0x68)
void HandleMCIError(i32 errorCode, char* commandString) {
    mciGetErrorStringA(errorCode, lpszReturnString, 0xFF);
    sprintf(
        gText,
        "CD MUSIC ERROR\n\n"
        "Description '%s'\n\n"
        "Command '%s'\n\n\n"
        "Because of this problem running with CD stereo music, Heroes II has been "
        "configured to run with MIDI music in the future.  You can always manually "
        "change this setting in the control panel within the game.",
        lpszReturnString,
        commandString
    );
    gConfig.mciError = 1;
    gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
    WritePrefs();
    ShutDown(gText);
}

VA(0x004cb6a0, 0xc7)
void soundManager::ValidatePreviousPosition(i32 track) {
    char buf[20];
    char* cur;
    H2_ASSERT(track >= 0 && track < 60, RETAIL_FILE, 66);
    if (CDPreviousPosition[track][0] == 0)
        return;
    strcpy(buf, CDPreviousPosition[track]);
    cur = FindToken(buf, ':');
    if (cur != 0)
        *cur = 0;
    if (atoi(buf) != track)
        CDPreviousPosition[track][0] = 0;
}

// @early-stop
// @early-stop-reloc-only
// All 0x13c relocation-masked bytes, the 0x18 frame/slots/CFG, and all 25
// ordered relocation sites/effective targets agree. Residual identities are
// local strings and the _stricmp/_strcmpi alias at the same retail address.
VA(0x004cb770, 0x13c)
void soundManager::CDStop(void) {
    char position[20];
    if (gbNoSound != 0)
        return;
    if (m_cdReady == 0)
        return;
    wsprintfA(CommandString, "stop CD wait");
    nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xFF, 0);
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

// @early-stop
// @early-stop-reloc-only
// All 0xb3 relocation-masked bytes, the 0x04 this slot/CFG, and all 13 ordered
// relocation sites/effective targets agree. Residual identities are local
// strings and the _stricmp/_strcmpi alias at the same retail address.
VA(0x004cb8b0, 0xb3)
i32 soundManager::CDIsPlaying(void) {
    if (gbNoSound != 0)
        return 0;
    if (m_cdReady == 0)
        return 0;
    wsprintfA(CommandString, "status CD mode");
    nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xFF, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    return stricmp(lpszReturnString, "playing") == 0;
}

VA(0x004cb970, 0xf3)
void soundManager::CDStartup(void) {
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
    wsprintfA(CommandString, "open %c: type cdaudio alias CD shareable", gcAnimPath[0]);
    nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xFF, 0);
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
void soundManager::CDShutdown(void) {
    if (gbNoSound != 0)
        return;
    if (m_cdReady == 0)
        return;
    wsprintfA(CommandString, "stop CD");
    nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xFF, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    wsprintfA(CommandString, "close CD");
    nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xFF, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
}

VA(0x004cbb50, 0xe5)
void soundManager::CDSetVolume(i32 volume, i32 fadeScale) {
    i32 local_c;
    u32l local_8;
    if (gbNoSound != 0)
        return;
    if (m_cdReady == 0)
        return;
    if (m_auxDevice == -1)
        return;
    if (volume == -1)
        local_c = gConfig.musicVolume;
    else
        local_c = volume;
    if (local_c != 0) {
        i32 local_10;
        if (fadeScale != 0)
            local_10 = 12 - (11 - local_c / 12);
        else
            local_10 = 12 - local_c;
        local_10 <<= 12;
        local_8 = local_10 << 16 | local_10;
    } else {
        local_8 = 0;
    }
    auxSetVolume(m_auxDevice, local_8);
}

// @early-stop
// @early-stop-reloc-only
// All 0x473 relocation-masked bytes, the 0x30 frame/slots/CFG, and all 86
// ordered relocation sites/effective targets agree. Residual identities are
// local strings and the _stricmp/_strcmpi alias at the same retail address.
VA(0x004cbc40, 0x473)
void soundManager::CDPlay(i32 track, i32 resume, i32 volume, i32 restart) {
    i32l t1;
    i32l t2;
    i32l t3;
    char buffer[20];
    i32 notify;
    HWND wnd;
    HWND wndn;
    if (gbNoSound != 0)
        return;
    if (m_cdReady == 0)
        return;
    if (gConfig.musicVolume == 0)
        return;
    if (track == -1) {
        CDStop();
        return;
    }
    if (m_currentTrack == track && CDPlaying != 0 && restart == 0)
        return;
    m_cdTrack = track;
    m_cdPlayFrame = volume;
    Process1WindowsMessage();
    ServiceSound();
    t1 = KBTickCount();
    wsprintfA(CommandString, "set CD time format tmsf");
    nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xFF, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    wsprintfA(CommandString, "status CD mode");
    nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xFF, 0);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    if (stricmp(lpszReturnString, "stopped") != 0) {
        wsprintfA(CommandString, "status CD position");
        nMCIError = mciSendStringA(CommandString, buffer, 20, 0);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
        strcpy(CDPreviousPosition[m_currentTrack], buffer);
        ValidatePreviousPosition(m_currentTrack);
    }
    t2 = KBTickCount();
    notify = bMusicIsLooping[track];
    Process1WindowsMessage();
    ServiceSound();
    if (restart == 0 && resume != 0 && CDPreviousPosition[track][0] != 0) {
        if (track == 43)
            wsprintfA(
                CommandString,
                "play CD from %s %s",
                CDPreviousPosition[track],
                notify ? " notify" : ""
            );
        else
            wsprintfA(
                CommandString,
                "play CD from %s to %d%s",
                CDPreviousPosition[track],
                track + 1,
                notify ? " notify" : ""
            );
        if (notify != 0)
            wnd = static_cast<HWND>(hwndApp);
        else
            wnd = 0;
        nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xFF, wnd);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
    } else {
        if (track == 43)
            wsprintfA(CommandString, "play CD from %d %s", track, notify ? " notify" : "");
        else
            wsprintfA(
                CommandString,
                "play CD from %d to %d%s",
                track,
                track + 1,
                notify ? " notify" : ""
            );
        if (notify != 0)
            wndn = static_cast<HWND>(hwndApp);
        else
            wndn = 0;
        nMCIError = mciSendStringA(CommandString, lpszReturnString, 0xFF, wndn);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
    }
    t3 = KBTickCount();
    CDPlaying = 1;
    Process1WindowsMessage();
    ServiceSound();
    if (m_fadeSteps > 0) {
        m_fadeSteps = 11;
        glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount() + 480;
        CDSetVolume(10, 0);
    } else {
        CDSetVolume(volume, 0);
    }
    m_currentTrack = static_cast<char>(track);
}

VA(0x004cc0c0, 0xf1)
void soundManager::CDPoll(void) {
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
i32 soundManager::ConvertVolume(i32 volume, SoundVolumeConversionMode soundType) {
    i32 local_8 = 0;
    if (soundType == SOUND_VOLUME_MUSIC) {
        if (gConfig.musicVolume >= 1 && gConfig.musicVolume <= SOUND_VOLUME_STEPS) {
            local_8 = ((11 - gConfig.musicVolume) * volume) / 10;
            if (local_8 < 1)
                local_8 = 1;
        }
    } else if (gConfig.soundVolume >= 1 && gConfig.soundVolume <= SOUND_VOLUME_STEPS) {
        local_8 = ((11 - gConfig.soundVolume) * volume) / 10;
        if (local_8 < 1)
            local_8 = 1;
    }
    if (local_8 < 0)
        local_8 = 0;
    if (SOUND_MIDI_VOLUME_MAX < local_8)
        local_8 = 127;
    return local_8;
}

VA(0x004cc2a0, 0x5e)
void __stdcall SetReady2Poll(u32l) {
    if (gpSoundManager == 0)
        return;
    gpSoundManager->m_pollToggle ^= 1;
    if (gpSoundManager->m_pollToggle != 0)
        gpSoundManager->m_pollDue = 1;
}

VA(0x004cc300, 0x26)
void __stdcall UpdateTimers(u32l) {
    iCalibrateLoop++;
    glMilliCounter += 16;
    SetReady2Poll(0);
}

VA(0x004cc330, 0xd7)
soundManager::soundManager(void) : baseManager(), field_0x574(1) {
    i32 local_8;
    m_active = false;
    m_fadeSteps = 0;
    field_0x56c = 0;
    for (local_8 = 0; local_8 < 32; local_8++)
        iLastVolume[local_8] = 0;
    memset(&m_ready, 0, SOUND_STATE_RESET_SPAN);
    m_samplesReady = 0;
    m_digitalDriver = 0;
    field_0x3a = 0;
    m_cdTrack = 0;
    m_cdPlayFrame = 0;
}

VA(0x004cc410, 0x14a)
struct _DIG_DRIVER*
WAVE_init_driver(u32l sampleRate, u16 bitsPerSample, u16 channels, u16 showErrors) {
    u32 numDevs;
    struct _DIG_DRIVER* drvr;
    WAVEOUTCAPSA caps;
    i32 rc;
    numDevs = waveOutGetNumDevs();
    if (numDevs == 0) {
        drvr = 0;
        return 0;
    }
    if (waveOutGetDevCapsA(0, &caps, sizeof(caps)) != 0) {
        MessageBoxA(
            static_cast<HWND>(hwndApp),
            "Sound initialization error!  No wave devices found.",
            "Startup Error",
            0
        );
        drvr = 0;
        return 0;
    }
    if (gbUseWaveout != 0)
        AIL_set_preference(15, 1);
    gWaveFormat.wf.wFormatTag = 1;
    gWaveFormat.wf.nChannels = channels;
    gWaveFormat.wf.nSamplesPerSec = sampleRate;
    gWaveFormat.wf.nAvgBytesPerSec = (bitsPerSample >> 3) * channels * sampleRate;
    gWaveFormat.wf.nBlockAlign = (bitsPerSample >> 3) * channels;
    gWaveFormat.wBitsPerSample = bitsPerSample;
    rc = AIL_waveOutOpen(&drvr, 0, 0, &gWaveFormat.wf);
    if (rc != 0) {
        if (showErrors != 0)
            MessageBoxA(
                static_cast<HWND>(hwndApp),
                AIL_last_error(),
                "Sound initialization error!",
                0
            );
        drvr = 0;
        return 0;
    }
    return drvr;
}

VA(0x004cc560, 0x3a8)
i32 soundManager::Open(i32) {
    i32 keyState;
    i32 musicTrack;

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
    memset(&m_ready, 0, SOUND_STATE_RESET_SPAN);
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
    m_messageMask = BASE_MANAGER_ACCEPT_LEFT_BUTTON_UP;
    m_priority = SOUND_MANAGER_PRIORITY;
    m_active = true;
    strcpy(m_name, "soundManager");
    return 0;
}

VA(0x004cc910, 0x91)
void soundManager::AllocateSampleHandles(void) {
    i32 local_8;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    for (local_8 = 0; local_8 < 14; local_8++) {
        m_sampleHandles[local_8] = AIL_allocate_sample_handle(m_digitalDriver);
        if (m_sampleHandles[local_8] == 0)
            break;
    }
    m_numSampleHandles = local_8;
}

VA(0x004cc9b0, 0x96)
void soundManager::Close(void) {
    if (!m_active)
        return;
    if (gbNoSound != 0)
        goto soundClosed;
    LogStr("SD1");
    CDShutdown();
    LogStr("SD2");
    MIDIShutdown();
    LogStr("SD3");
    AIL_shutdown();
    LogStr("SD4");
soundClosed:
    m_active = false;
    gbNoSound = true;
}

VA(0x004cca50, 0x1a)
i32 soundManager::Main(struct tag_message&) {
    return 0;
}

VA(0x004cca70, 0x1a)
struct _SAMPLE* soundManager::StartSample(char*, char**, i16, i16, i32, i32, i32l) {
    return 0;
}

VA(0x004cca90, 0x126)
void soundManager::StopAllSamples(i32 stopMusic) {
    i16 sampleIdx;
    i32 waitCounter;
    i32 sampleStatus;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    if (m_samplesReady == 0)
        return;
    LogStr("SAS 1");
    for (sampleIdx = 0; sampleIdx < m_numSampleHandles; sampleIdx++) {
        sampleStatus = AIL_sample_status(m_sampleHandles[sampleIdx]);
        if (sampleStatus == IDX(SOUND_SAMPLE_STATUS_PLAYING))
            AIL_end_sample(m_sampleHandles[sampleIdx]);
    }
    m_fadeSteps = 0;
    if (stopMusic != 0) {
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
void soundManager::StopSample(struct _SAMPLE* sample) {
    i32 local_c;
    i32 local_10;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == 0)
        return;
    local_10 = 0;
    LogStr("Stop Sample 1");
    if (m_sampleHandles[0] == sample)
        local_10 = 1;
    AIL_end_sample(sample);
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
void soundManager::ModifySample(struct _SAMPLE* sampleHandle, i16 operation, i32l value) {
    i32 foundChannel;
    i32 unused;
    i32 sampleIndex;

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
        case IDX(SOUND_SAMPLE_OPERATION_VOLUME):
        case IDX(SOUND_SAMPLE_OPERATION_EFFECT_VOLUME):
            AIL_set_sample_volume(sampleHandle, ConvertVolume(value, IDX(SOUND_VOLUME_EFFECT)));
            if (foundChannel >= 0)
                iLastVolume[foundChannel] = static_cast<i16>(value);
            break;
        case IDX(SOUND_SAMPLE_OPERATION_MUSIC_VOLUME):
            H2_ASSERT(gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI, RETAIL_FILE, 1327);
            AIL_set_sample_volume(sampleHandle, ConvertVolume(value, IDX(SOUND_VOLUME_MUSIC)));
            if (foundChannel >= 0)
                iLastVolume[foundChannel] = static_cast<i16>(value);
            break;
        case IDX(SOUND_SAMPLE_OPERATION_START):
            AIL_start_sample(sampleHandle);
            break;
    }

    Process1WindowsMessage();
    LogStr("Modify Sample 2");
}

VA(0x004cce90, 0xa3)
i32l soundManager::DigitalReport(struct _SAMPLE* sample, i16 reportType) {
    i32 sampleStatus;

    if (gbNoSound != 0)
        return 0;
    if (m_digitalDriver == 0)
        return 0;
    switch (reportType) {
        case IDX(SOUND_DIGITAL_REPORT_VOLUME):
            return AIL_sample_volume(sample);
        case IDX(SOUND_DIGITAL_REPORT_PLAYING):
            sampleStatus = AIL_sample_status(sample);
            return sampleStatus == IDX(SOUND_SAMPLE_STATUS_PLAYING);
    }
    return 0;
}

VA(0x004ccf40, 0xe8)
void soundManager::AdjustSoundVolumes(void) {
    i32 sampleIndex;
    struct _SAMPLE* sampleHandle;

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
            if (DigitalReport(sampleHandle, IDX(SOUND_DIGITAL_REPORT_PLAYING)) != 0)
                ModifySample(
                    sampleHandle,
                    IDX(SOUND_SAMPLE_OPERATION_EFFECT_VOLUME),
                    iLastVolume[sampleIndex]
                );
        } else {
            ModifySample(sampleHandle, IDX(SOUND_SAMPLE_OPERATION_VOLUME), 0);
        }
    }
    LogStr("Adjust Sound Volumes 2");
}

VA(0x004cd030, 0xee)
void soundManager::AdjustMusicVolumes(void) {
    if (gbNoSound != 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (m_currentTrack < 0)
        return;
    LogStr("Adjust Music Volumes 1");
    i32 local_4 = 0;
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
void soundManager::ForcePollSound(void) {
    if (gbNoSound != 0)
        return;
    m_pollRequested = 1;
    PollSound();
}

VA(0x004cd160, 0xe3)
void soundManager::SetMusicQuality(i32 musicSource) {
    i32 local_8;
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
    memset(m_savedTrackPositions, 0, 240);
    gConfig.musicSource = musicSource;
    if (local_8 >= 0)
        PlayAmbientMusic(local_8, 0, -1);
}

VA(0x004cd250, 0xc5)
void soundManager::PlayAmbientMusic(i32 track, i32l resume, i32 unused) {
    if (gbNoSound != 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (m_ready == 0)
        return;
    if (m_currentTrack == track)
        return;
    if (gConfig.musicVolume == 0) {
        m_currentTrack = static_cast<char>(track);
        return;
    }
    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI)
        CDPlay(track, resume, -1, 0);
    else
        MIDIPlay(track);
    m_currentTrack = static_cast<char>(track);
}

VA(0x004cd320, 0x38f)
void soundManager::PollSound(void) {
    i32 volume;
    struct _SAMPLE* smp;
    i32l delta;
    i32 snap;
    i32l now;
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
        if (m_currentTrack < SOUND_CD_MUSIC_TRACK_FIRST
            || m_currentTrack > SOUND_CD_MUSIC_TRACK_LAST)
            glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount();
        delta = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - KBTickCount();
        m_fadeSteps = delta / 60;
        if (m_fadeSteps < 1)
            m_fadeSteps = 0;
        LogStr("Poll Sound 1b");
        if (m_fadeSteps <= SOUND_FADE_HOLD_STEPS && m_currentTrack != m_fadeTargetTrack) {
            if (m_midiFile != 0 && bSaveMusicPosition[m_currentTrack] != 0) {
                if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
                    H2_ASSERT(reinterpret_cast<i32>(m_midiFile), RETAIL_FILE, 0x61a);
                    m_savedTrackPositions[m_currentTrack] = ftell(m_midiFile);
                }
            } else {
                glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount();
            }
            m_fading = 1;
            if (bSaveMusicPosition[m_fadeTargetTrack] != 0)
                PlayAmbientMusic(m_fadeTargetTrack, m_savedTrackPositions[m_fadeTargetTrack], -1);
            else
                PlayAmbientMusic(m_fadeTargetTrack, 0, -1);
            now = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - KBTickCount();
            m_fadeSteps = now / 60;
            if (m_fadeSteps < 1)
                m_fadeSteps = 0;
            m_currentTrack = static_cast<char>(m_fadeTargetTrack);
        }
        snap = m_fadeSteps;
        if (m_fadeSteps <= SOUND_FADE_HOLD_STEPS)
            volume = (11 - m_fadeSteps) * 64 / 11;
        else
            volume = (m_fadeSteps - SOUND_FADE_HOLD_STEPS) * SOUND_SAMPLE_VOLUME_MAX / 6;
        if (volume > SOUND_SAMPLE_VOLUME_MAX)
            volume = SOUND_SAMPLE_VOLUME_MAX;
        if (volume < 0)
            volume = 0;
        LogStr("Poll Sound 1c");
        smp = m_sampleHandles[0];
        if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI) {
            volume = (SOUND_VOLUME_STEPS + 1 - gConfig.musicVolume) * volume * SOUND_MIDI_VOLUME_MAX
                     / SOUND_CD_VOLUME_SCALE_DIVISOR;
            if (volume > SOUND_MIDI_VOLUME_MAX)
                volume = SOUND_MIDI_VOLUME_MAX;
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
void soundManager::SwitchAmbientMusic(i32 track) {
    if (gbNoSound != 0)
        return;
    if (m_samplesReady == 0)
        return;
    if (gConfig.musicVolume == 0) {
        m_currentTrack = static_cast<char>(track);
        return;
    }
    if (MusicPlaying() == 0) {
        PlayAmbientMusic(track, 0, -1);
        return;
    }
    if (m_currentTrack == track)
        return;
    LogStr("Switch Ambient Music 1");
    Process1WindowsMessage();
    if ((m_fadeSteps != 0 && m_fadeTargetTrack != track)
        || (m_fadeSteps == 0 && m_currentTrack != track)) {
        if (m_fadeSteps <= SOUND_FADE_HOLD_STEPS) {
            m_fadeSteps = 11;
            glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount() + 900;
        }
        m_fadeTargetTrack = track;
        PollSound();
    }
    LogStr("Switch Ambient Music 2");
}

VA(0x004cd7f0, 0x28f)
struct _SAMPLE* soundManager::MemorySample(class sample* sampleResource) {
    struct _SAMPLE* smp;
    i16 ch;
    SampleChannelStruct* scs;
    SamplePlaybackData* playbackData;
    if (gbNoSound != 0)
        return 0;
    if (m_digitalDriver == 0)
        return 0;
    if (m_samplesReady == 0)
        return 0;
    if (gConfig.soundVolume == 0)
        return 0;
    playbackData = &sampleResource->m_playbackData;
    if (m_ready == 0 || playbackData->volume == 0)
        return 0;
    LogStr("Memory Sample 1");
    scs = &SCS[playbackData->channelType];
    for (ch = static_cast<i16>(scs->startChannel); scs->endChannel > ch; ch++) {
        if (AIL_sample_status(m_sampleHandles[ch]) == IDX(SOUND_SAMPLE_STATUS_DONE))
            break;
    }
    if (scs->endChannel == ch) {
        if (playbackData->channelType == 4) {
            LogStr("Memory Sample 2a");
            return 0;
        }
        ch = static_cast<i16>(scs->currentChannel);
        scs->currentChannel++;
        if (scs->endChannel <= scs->currentChannel) {
            scs->currentChannel = scs->startChannel;
            ch = static_cast<i16>(scs->currentChannel);
        }
        StopSample(m_sampleHandles[ch]);
    }
    smp = m_sampleHandles[ch];
    m_channelVolumes[ch] = static_cast<char>(playbackData->volume);
    iLastVolume[ch] = static_cast<i16>(playbackData->volume);
    AIL_init_sample(smp);
    AIL_set_sample_type(smp, playbackData->format, 0);
    AIL_set_sample_playback_rate(smp, playbackData->sampleRate);
    AIL_set_sample_loop_count(smp, playbackData->loopCount);
    AIL_set_sample_address(smp, playbackData->data, playbackData->size);
    if (gConfig.soundVolume != 0)
        AIL_set_sample_volume(smp, ConvertVolume(playbackData->volume, IDX(SOUND_VOLUME_EFFECT)));
    else
        AIL_set_sample_volume(smp, 0);
    AIL_start_sample(smp);
    playbackData->activeSample = smp;
    m_channelSamples[ch] = smp;
    m_channelSampleData[ch] = playbackData->data;
    m_channelSampleSizes[ch] = playbackData->size;
    LogStr("Memory Sample 2b");
    return smp;
}

VA(0x004cda80, 0x16)
void soundManager::GetNumberCDDrives(void) {}

VA(0x004cdaa0, 0x2e)
void soundManager::ServiceSound(void) {
    if (gbNoSound != 0)
        return;
    AIL_serve();
}

VA(0x004cdad0, 0x7f)
i32 soundManager::MusicPlaying(void) {
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

// @data-layout-note Retail's initialized soundmgr contribution is
// RVA 0x11f018..0x11f980 (0x968). Candidate section 2 is one align-eight
// ordinary .data section of 0x965 bytes. Its public owners are exact at offsets
// 0, 0x38, 0x68, 0x3ec, 0x3f0, and 0x3f4. Seventy compiler-local $SG owners
// cover 0x3f8..0x965: thirteen driver-name strings, the 0x100-byte MCI error
// format, and every later diagnostic literal. Bytes 0x38..0x964 are directly
// identical to retail. The thirteen pointer slots at 0..0x30 relocate in order
// to retail VAs 0x0051f410..0x0051f4a4, and the final three contribution bytes
// are zero alignment. Those thirteen data relocations plus 57 code references
// cover every local owner; all owner-relative addends are zero. The only rdata
// owner is the reviewed 0xc soundManager vtable at RVA 0x0eba20; its 0x10
// contribution includes four bytes of natural alignment.
// Retail loader-zero storage is 0x134970..0x134bc8 (0x258), ordered as
// gWaveFormat, lpszReturnString, nMCIError, iLastVolume, and CommandString at
// offsets 0, 0x10, 0x110, 0x118, and 0x158. Candidate BSS has the exact size and
// align-eight class, but emits those owners at 0x208, 0x108, 0, 0x218, and 0x8.
// Their definitions already occur in retail order. Keep the resulting
// inconsistent-anchor-bases residual instead of inventing aliases, aggregates,
// padding, pragmas, or fake owners.

// ---- globals (definitions, RVA order) ----
DATA(0x0051f018) char* digitalDriverNames[14] = {
    "",
    "ultra.dig",
    "sndsys.dig",
    "sndscape.dig",
    "jammer.dig",
    "audiodrv.dig",
    "proaudio.dig",
    "rap10.dig",
    "iwav.dig",
    "nvdig.dig",
    "sb16.dig",
    "sbpro.dig",
    "sblaster.dig",
    0
};
DATA(0x0051f050) SampleChannelStruct SCS[4] = {{0, 1, 0}, {1, 2, 1}, {2, 6, 2}, {6, 16, 6}};
DATA(0x0051f080) char CDPreviousPosition[60][15] = {0};
DATA(0x0051f404) i32 CDWaiting = -1;
DATA(0x0051f408) i32 CDPlaying = 0;
DATA(0x0051f40c) i32 iCalibrateLoop = 0;
DATA(0x00534980) char lpszReturnString[0x100];
DATA(0x00534a80) u32l nMCIError;
DATA(0x00534a88) i16 iLastVolume[0x20];
DATA(0x00534ac8) char CommandString[0x100];

#undef RETAIL_FILE
