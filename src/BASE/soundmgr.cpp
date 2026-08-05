#include <va.h>
#include <BASE/soundManager.h>
#include <BASE/sample.h>
#include <BASE/soundBackends.h>
#include <BASE/Midi.h>
#include <BASE/MusicFlags.h>
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
#include <audiere.h>

H2_ENUM_BEGIN(SoundConstant)
    SAMPLE_VOLUME_MAX            = 0x40,
    MIDI_VOLUME_MAX              = 0x7f,
    CD_VOLUME_SCALE_DIVISOR      = 0x280,
    CD_MUSIC_TRACK_FIRST         = 8,
    CD_MUSIC_TRACK_LAST          = 15,
    FADE_HOLD_STEPS              = 10,
    FADE_TOTAL_STEPS             = IDX(CONFIG_VOLUME_MAX) + 1,
    FADE_SAMPLE_RISE_STEPS       = 6,
    AUX_VOLUME_LEVEL_COUNT       = 12,
    AUX_VOLUME_LEVEL_SHIFT       = 12,
    AUX_STEREO_CHANNEL_SHIFT     = 16,
    MCI_RETURN_CHARACTER_LIMIT   = MCI_RESULT_CAPACITY - 1,
    CD_POSITION_BUFFER_CAPACITY  = 20,
    CD_SINGLE_TRACK              = 43,
    CD_FADE_DELAY_TICKS          = 480,
    CD_POLL_INTERVAL_TICKS       = 3000,
    AMBIENT_FADE_DELAY_TICKS     = 900,
    SOUND_TIMER_INTERVAL         = 16,
    AIL_WAVEOUT_PREFERENCE       = 15,
    PCM_BITS_PER_BYTE_SHIFT      = 3,
    DEFAULT_SAMPLE_RATE          = 22050,
    DEFAULT_SAMPLE_BITS          = 8,
    DEFAULT_SAMPLE_CHANNELS      = 1,
    SAMPLE_STOP_ALL_WAIT_COUNT   = 5,
    SAMPLE_STOP_MUSIC_WAIT_COUNT = 10,
    FADE_STEP_TICKS              = 60,
    NO_SAMPLE_CHANNEL_TYPE       = SOUND_CHANNEL_TYPE_COUNT
H2_ENUM_END(SoundConstant)

H2_ENUM_BEGIN(SoundSampleStatus)
    SAMPLE_STATUS_DONE    = 2,
    SAMPLE_STATUS_PLAYING = 4
H2_ENUM_END(SoundSampleStatus)


DATA(0x005348a8) static WAVEOUTCAPSA gWaveOutCaps;
DATA(0x005348e0) static PCMWAVEFORMAT gWaveFormat;
#define NORMALIZED_VOLUME_MAX 127.0f

DATA(0x005348f0) bool gSoundDisabled = false;
DATA(0x005348f1) bool gSoundBackendsReady = false;

// State of the removed MCI/redbook CD path and of the removed Miles sample
// handle pool: the `@remove` bodies below are the PoL 2.0 sources kept as the
// removed-body record, and this is the state they read. Nothing retail
// references it, so none of it is a soundManager member.
static i32 gCDReady;
static i32 gCDTrack;
static i32 gCDPlayFrame;
static i32 gCDPollTimer;
static i16 gAuxDevice;
static i32 gPollRequested;
static i32 gPollDue;
static i32 gPollToggle;
static struct _SAMPLE* gSampleHandles[SOUND_SAMPLE_HANDLE_CAPACITY];
static i32 gNumSampleHandles;

// @remove
void HandleMCIError(i32 errorCode, char* commandString) {
    mciGetErrorStringA(errorCode, lpszReturnString, MCI_RETURN_CHARACTER_LIMIT);
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

// @remove
void soundManager::ValidatePreviousPosition(i32 track) {
    char buf[CD_POSITION_BUFFER_CAPACITY];
    char* cur;
    H2_ASSERT(
        track >= 0 && track < MIDI_TRACK_COUNT,
        NULL,
        66
    );
    if (CDPreviousPosition[track][0] == 0)
        return;
    strcpy(buf, CDPreviousPosition[track]);
    cur = FindToken(buf, ':');
    if (cur != NULL)
        *cur = 0;
    if (atoi(buf) != track)
        CDPreviousPosition[track][0] = 0;
}

// @remove
void soundManager::CDStop(void) {
    char position[CD_POSITION_BUFFER_CAPACITY];
    if (gbNoSound != 0)
        return;
    if (gCDReady == 0)
        return;
    wsprintfA(CommandString, "stop CD wait");
    nMCIError =
        mciSendStringA(CommandString, lpszReturnString, MCI_RETURN_CHARACTER_LIMIT, NULL);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    if (strcmpi(lpszReturnString, "stopped") != 0 && m_musicTrack >= 0) {
        wsprintfA(CommandString, "status CD position");
        nMCIError = mciSendStringA(CommandString, position, sizeof(position), NULL);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
        strcpy(CDPreviousPosition[m_musicTrack], position);
        ValidatePreviousPosition(m_musicTrack);
    }
    CDPlaying = 0;
}

// @remove
i32 soundManager::CDIsPlaying(void) {
    if (gbNoSound != 0)
        return 0;
    if (gCDReady == 0)
        return 0;
    wsprintfA(CommandString, "status CD mode");
    nMCIError =
        mciSendStringA(CommandString, lpszReturnString, MCI_RETURN_CHARACTER_LIMIT, NULL);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    return strcmpi(lpszReturnString, "playing") == 0;
}

VA(0x004b59b0, 0x123)
bool soundManager::CDStartup(void) {
    if (m_backend == SOUND_BACKEND_AUDIERE)
        return true;

    ShutdownSoundBackends();
    if (gSoundDisabled)
        return false;

    m_backend = SOUND_BACKEND_AUDIERE;
    m_audiereDevice = audiere::OpenDevice("winmm");
    if (m_audiereDevice != NULL)
        return StartupAudiereMusic(m_audiereDevice);
    return false;
}

// @remove
void soundManager::CDShutdown(void) {
    if (gbNoSound != 0)
        return;
    if (gCDReady == 0)
        return;
    wsprintfA(CommandString, "stop CD");
    nMCIError =
        mciSendStringA(CommandString, lpszReturnString, MCI_RETURN_CHARACTER_LIMIT, NULL);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    wsprintfA(CommandString, "close CD");
    nMCIError =
        mciSendStringA(CommandString, lpszReturnString, MCI_RETURN_CHARACTER_LIMIT, NULL);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
}

// @remove
void soundManager::CDSetVolume(i32 volume, i32 fadeScale) {
    i32 local_c;
    u32l local_8;
    if (gbNoSound != 0)
        return;
    if (gCDReady == 0)
        return;
    if (gAuxDevice == -1)
        return;
    if (volume == -1)
        local_c = IDX(gConfig.musicVolume);
    else
        local_c = volume;
    if (local_c != 0) {
        i32 local_10;
        if (fadeScale != 0)
            local_10 = AUX_VOLUME_LEVEL_COUNT
                       - (FADE_TOTAL_STEPS - local_c / AUX_VOLUME_LEVEL_COUNT);
        else
            local_10 = AUX_VOLUME_LEVEL_COUNT - local_c;
        local_10 <<= AUX_VOLUME_LEVEL_SHIFT;
        local_8 = local_10 << AUX_STEREO_CHANNEL_SHIFT | local_10;
    } else {
        local_8 = 0;
    }
    auxSetVolume(gAuxDevice, local_8);
}

// @remove
void soundManager::CDPlay(i32 track, i32 resume, i32 volume, i32 restart) {
    i32l t1;
    i32l t2;
    i32l t3;
    char buffer[CD_POSITION_BUFFER_CAPACITY];
    i32 notify;
    HWND wnd;
    HWND wndn;
    if (gbNoSound != 0)
        return;
    if (gCDReady == 0)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    if (track == -1) {
        CDStop();
        return;
    }
    if (m_musicTrack == track && CDPlaying != 0 && restart == 0)
        return;
    gCDTrack = track;
    gCDPlayFrame = volume;
    Process1WindowsMessage();
    ServiceSound();
    t1 = KBTickCount();
    wsprintfA(CommandString, "set CD time format tmsf");
    nMCIError =
        mciSendStringA(CommandString, lpszReturnString, MCI_RETURN_CHARACTER_LIMIT, NULL);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    wsprintfA(CommandString, "status CD mode");
    nMCIError =
        mciSendStringA(CommandString, lpszReturnString, MCI_RETURN_CHARACTER_LIMIT, NULL);
    if (nMCIError != 0)
        HandleMCIError(nMCIError, CommandString);
    if (strcmpi(lpszReturnString, "stopped") != 0) {
        wsprintfA(CommandString, "status CD position");
        nMCIError = mciSendStringA(CommandString, buffer, CD_POSITION_BUFFER_CAPACITY, NULL);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
        strcpy(CDPreviousPosition[m_musicTrack], buffer);
        ValidatePreviousPosition(m_musicTrack);
    }
    t2 = KBTickCount();
    notify = bMusicIsLooping[track];
    Process1WindowsMessage();
    ServiceSound();
    if (restart == 0 && resume != 0 && CDPreviousPosition[track][0] != 0) {
        if (track == CD_SINGLE_TRACK)
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
            wnd = hwndApp;
        else
            wnd = NULL;
        nMCIError =
            mciSendStringA(CommandString, lpszReturnString, MCI_RETURN_CHARACTER_LIMIT, wnd);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
    } else {
        if (track == CD_SINGLE_TRACK)
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
            wndn = hwndApp;
        else
            wndn = NULL;
        nMCIError =
            mciSendStringA(CommandString, lpszReturnString, MCI_RETURN_CHARACTER_LIMIT, wndn);
        if (nMCIError != 0)
            HandleMCIError(nMCIError, CommandString);
    }
    t3 = KBTickCount();
    CDPlaying = 1;
    Process1WindowsMessage();
    ServiceSound();
    if (m_musicFadeSteps > 0) {
        m_musicFadeSteps = FADE_TOTAL_STEPS;
        glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount() + CD_FADE_DELAY_TICKS;
        CDSetVolume(IDX(CONFIG_VOLUME_MAX), 0);
    } else {
        CDSetVolume(volume, 0);
    }
    m_musicTrack = static_cast<char>(track);
}

// @remove
void soundManager::CDPoll(void) {
    if (gbNoSound != 0)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    if (gCDReady == 0)
        return;
    if (CDPlaying == 0 || m_musicTrack < 0)
        return;
    {
        if (bMusicIsLooping[m_musicTrack] == 0)
            return;
        if (KBTickCount() < gCDPollTimer + CD_POLL_INTERVAL_TICKS)
            return;
        gCDPollTimer = KBTickCount();
        if (CDIsPlaying() == 0)
            CDPlay(gCDTrack, 0, gCDPlayFrame, 1);
    }
}

VA(0x004b5710, 0x101)
void soundManager::ShutdownSoundBackends(void) {
    if (IsMilesBackend(this)) {
        MIDIShutdown();
        StopAllMilesSamples();
        AIL_shutdown();
        m_digitalDriver = NULL;
    } else if (IsAudiereBackend(this)) {
        StopAudiereMusic(m_musicTrack);
        StopAllAudiereSamples();
        m_audiereDevice = NULL;
    }
    m_backend = SOUND_BACKEND_NONE;
}

VA(0x004b5820, 0x18f)
bool soundManager::StartupMilesBackend(void) {
    if (m_backend == SOUND_BACKEND_MILES)
        return true;

    ShutdownSoundBackends();
    if (gSoundDisabled)
        return false;

    m_backend = SOUND_BACKEND_MILES;
    if (waveOutGetNumDevs() == 0) {
        m_digitalDriver = NULL;
        return false;
    }
    if (waveOutGetDevCapsA(0, &gWaveOutCaps, sizeof(gWaveOutCaps)) != 0) {
        MessageBoxA(
            hwndApp,
            "\xce\xf8\xe8\xe1\xea\xe0 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xe8 "
            "\xe7\xe2\xf3\xea\xe0!  \xcd\xe5 \xed\xe0\xe9\xe4\xe5\xed\xee "
            "\xf3\xf1\xf2\xf0\xee\xe9\xf1\xf2\xe2\xee.",
            "\xce\xf8\xe8\xe1\xea\xe0 \xe7\xe0\xe3\xf0\xf3\xe7\xea\xe8",
            0
        );
        m_digitalDriver = NULL;
        return false;
    }
    AIL_startup();
    AIL_set_preference(AIL_WAVEOUT_PREFERENCE, 1);

    u32l sampleRate = DEFAULT_SAMPLE_RATE;
    u16 bits = DEFAULT_SAMPLE_BITS;
    u16 channels = DEFAULT_SAMPLE_CHANNELS;
    gWaveFormat.wf.wFormatTag = 1;
    gWaveFormat.wf.nChannels = channels;
    gWaveFormat.wf.nSamplesPerSec = sampleRate;
    gWaveFormat.wf.nAvgBytesPerSec =
        sampleRate * (bits / DEFAULT_SAMPLE_BITS) * channels;
    gWaveFormat.wf.nBlockAlign = (bits / DEFAULT_SAMPLE_BITS) * channels;
    gWaveFormat.wBitsPerSample = bits;
    if (AIL_waveOutOpen(&m_digitalDriver, NULL, 0, &gWaveFormat.wf) != 0) {
        MessageBoxA(
            hwndApp,
            AIL_last_error(),
            "\xce\xf8\xe8\xe1\xea\xe0 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xe8 "
            "\xe7\xe2\xf3\xea\xe0!",
            0
        );
        m_digitalDriver = NULL;
        AIL_shutdown();
        return false;
    }

    StartupMilesSamples(m_digitalDriver);
    gSoundBackendsReady = true;
    return MIDIStartup();
}

VA(0x004b5ae0, 0xb1)
i32 soundManager::ConvertVolume(i32 volume, SoundVolumeConversionMode soundType) {
    i32 local_8 = 0;
    if (soundType == SOUND_VOLUME_MUSIC) {
        if (gConfig.musicVolume >= CONFIG_VOLUME_MIN
            && gConfig.musicVolume <= CONFIG_VOLUME_MAX) {
            local_8 = (volume * (FADE_TOTAL_STEPS - IDX(gConfig.musicVolume)))
                      / IDX(CONFIG_VOLUME_MAX);
            if (local_8 < 1)
                local_8 = 1;
        }
    } else if (gConfig.soundVolume >= CONFIG_VOLUME_MIN
               && gConfig.soundVolume <= CONFIG_VOLUME_MAX) {
        local_8 =
            (volume * (FADE_TOTAL_STEPS - IDX(gConfig.soundVolume))) / IDX(CONFIG_VOLUME_MAX);
        if (local_8 < 1)
            local_8 = 1;
    }
    if (local_8 < 0)
        local_8 = 0;
    if (MIDI_VOLUME_MAX < local_8)
        local_8 = MIDI_VOLUME_MAX;
    return local_8;
}

VA(0x004b5ba0, 0x2b)
float soundManager::ConvertVolumeFloat(i32 volume, SoundVolumeConversionMode soundType) {
    return static_cast<float>(ConvertVolume(volume, soundType)) / NORMALIZED_VOLUME_MAX;
}

#undef NORMALIZED_VOLUME_MAX

// @remove
void __stdcall SetReady2Poll(u32l) {
    if (gpSoundManager == NULL)
        return;
    gPollToggle ^= 1;
    if (gPollToggle != 0)
        gPollDue = 1;
}

// @remove
void __stdcall UpdateTimers(u32l) {
    iCalibrateLoop++;
    glMilliCounter += SOUND_TIMER_INTERVAL;
    SetReady2Poll(0);
}

VA(0x004b5bd0, 0x146)
soundManager::soundManager(void) : baseManager() {
    m_backend = SOUND_BACKEND_NONE;
    m_savedBackend = SOUND_BACKEND_NONE;
    m_active = false;
    gSoundBackendsReady = false;
    m_digitalDriver = NULL;
    m_audiereDevice = NULL;
    m_musicFadeTargetTrack = MIDI_NO_TRACK;
    m_musicFadeSteps = 0;
    m_musicTrack = MIDI_NO_TRACK;
}

// @remove
struct _DIG_DRIVER*
WAVE_init_driver(u32l sampleRate, u16 bitsPerSample, u16 channels, u16 showErrors) {
    u32 numDevs;
    struct _DIG_DRIVER* drvr;
    WAVEOUTCAPSA caps;
    i32 rc;
    numDevs = waveOutGetNumDevs();
    if (numDevs == 0) {
        drvr = NULL;
        return NULL;
    }
    if (waveOutGetDevCapsA(0, &caps, sizeof(caps)) != 0) {
        MessageBoxA(
            hwndApp,
            "Sound initialization error!  No wave devices found.",
            "Startup Error",
            0
        );
        drvr = NULL;
        return NULL;
    }
    if (gbUseWaveout != 0)
        AIL_set_preference(AIL_WAVEOUT_PREFERENCE, 1);
    gWaveFormat.wf.wFormatTag = 1;
    gWaveFormat.wf.nChannels = channels;
    gWaveFormat.wf.nSamplesPerSec = sampleRate;
    gWaveFormat.wf.nAvgBytesPerSec =
        (bitsPerSample >> PCM_BITS_PER_BYTE_SHIFT) * channels * sampleRate;
    gWaveFormat.wf.nBlockAlign = (bitsPerSample >> PCM_BITS_PER_BYTE_SHIFT) * channels;
    gWaveFormat.wBitsPerSample = bitsPerSample;
    rc = AIL_waveOutOpen(&drvr, NULL, 0, &gWaveFormat.wf);
    if (rc != 0) {
        if (showErrors != 0)
            MessageBoxA(
                hwndApp,
                AIL_last_error(),
                "Sound initialization error!",
                0
            );
        drvr = NULL;
        return NULL;
    }
    return drvr;
}

VA(0x004b5d20, 0x2df)
i32 soundManager::Open(i32) {
    i32 asyncState;
    i32 musicTrack;

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

    asyncState = GetAsyncKeyState(VK_F6);
    if (HIBYTE(asyncState)) {
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
        WritePrefs();
    }
    asyncState = GetAsyncKeyState(VK_F7);
    if (HIBYTE(asyncState)) {
        gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
        WritePrefs();
    }

    m_musicTrack = MIDI_NO_TRACK;
    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI) {
        if (!CDStartup()) {
            StartupMilesBackend();
            if (MusicFlagsActive()) {
                gConfig.musicVolume = CONFIG_VOLUME_MUTED;
                WritePrefs();
            } else {
                gConfig.musicSource = CONFIG_MUSIC_SOURCE_MIDI;
                WritePrefs();
            }
        }
    } else {
        StartupMilesBackend();
        if (MusicFlagsActive()) {
            if (CDStartup()) {
                gConfig.musicSource = CONFIG_MUSIC_SOURCE_CD;
                WritePrefs();
            } else {
                gConfig.musicVolume = CONFIG_VOLUME_MUTED;
                WritePrefs();
            }
        }
    }

    if (!IsAudiereBackend(this) && !IsMilesBackend(this)) {
        gConfig.soundVolume = CONFIG_VOLUME_MUTED;
        WritePrefs();
    }

    m_messageMask = BASE_MANAGER_ACCEPT_LEFT_BUTTON_UP;
    m_priority = SOUND_MANAGER_PRIORITY;
    m_active = true;
    strcpy(m_name, "soundManager");
    return 0;
}

// @remove
void soundManager::AllocateSampleHandles(void) {
    i32 local_8;
    if (gbNoSound != 0)
        return;
    if (m_digitalDriver == NULL)
        return;
    for (local_8 = 0; local_8 < SOUND_SAMPLE_HANDLE_CAPACITY; local_8++) {
        gSampleHandles[local_8] =
            AIL_allocate_sample_handle(m_digitalDriver);
        if (gSampleHandles[local_8] == NULL)
            break;
    }
    gNumSampleHandles = local_8;
}

VA(0x004b6000, 0x28)
void soundManager::Close(void) {
    if (m_active != 1)
        return;
    ShutdownSoundBackends();
    m_active = false;
}

VA(0x004b6030, 0xf)
MessageDispatchResult soundManager::Main(struct tag_message&) {
    return MESSAGE_DISPATCH_CONTINUE;
}

// @remove
struct _SAMPLE* soundManager::StartSample(char*, char**, i16, i16, i32, i32, i32l) {
    return NULL;
}

VA(0x004b6040, 0xcb)
void soundManager::StopAllSamples(i32 stopMusic) {
    if (!gSoundBackendsReady)
        return;

    if (IsAudiereBackend(this)) {
        m_musicFadeSteps = 0;
        if (stopMusic != 0)
            StopAudiereMusic(m_musicTrack);
        StopAllAudiereSamples();
        return;
    }
    if (IsMilesBackend(this)) {
        m_musicFadeSteps = 0;
        if (stopMusic != 0)
            MIDIStop(m_musicTrack);
        StopAllMilesSamples();
    }
}

VA(0x004b6110, 0x8a)
void soundManager::StopSample(class sample* sampleResource) {
    if (IsAudiereBackend(this)) {
        StopAudiereSample(sampleResource);
    } else if (IsMilesBackend(this)) {
        StopMilesSample(sampleResource);
    }
}

VA(0x004b61a0, 0xa1)
void soundManager::ModifySample(
    class sample* sampleResource,
    i32 volume
) {
    if (!gSoundBackendsReady)
        return;
    if (IsAudiereBackend(this)) {
        SetAudiereSampleVolume(sampleResource, volume);
    } else if (IsMilesBackend(this)) {
        SetMilesSampleVolume(sampleResource, volume);
    }
}

VA(0x004b6250, 0x8e)
bool soundManager::DigitalReport(class sample* sampleResource) {
    if (IsAudiereBackend(this)) {
        return AudiereSamplePlaying(sampleResource);
    }
    if (IsMilesBackend(this))
        return MilesSamplePlaying(sampleResource);
    return false;
}

VA(0x004b62e0, 0x4d)
void soundManager::AdjustSoundVolumes(void) {
    if (!gSoundBackendsReady)
        return;
    if (IsMilesBackend(this))
        AdjustMilesSampleVolumes();
}

VA(0x004b6330, 0xad)
void soundManager::AdjustMusicVolumes(void) {
    if (!gSoundBackendsReady)
        return;
    if (m_musicTrack < 0)
        return;
    if (IsAudiereBackend(this))
        SetAudiereMusicVolume(-1, false);
    else if (IsMilesBackend(this))
        MIDISetVolume(m_musicFadeSteps);
}

// @remove
void soundManager::ForcePollSound(void) {
    if (gbNoSound != 0)
        return;
    gPollRequested = 1;
    PollSound();
}

VA(0x004b63e0, 0xef)
void soundManager::SetMusicQuality(i32 musicSource) {
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;

    i32 previousTrack;
    if (IsAudiereBackend(this)) {
        previousTrack = m_musicTrack;
        StopAudiereMusic(m_musicTrack);
        m_musicTrack = MIDI_NO_TRACK;
    } else if (IsMilesBackend(this)) {
        previousTrack = m_musicTrack;
        MIDIStop(m_musicTrack);
    }

    gConfig.musicSource = ConfigMusicSource(musicSource);
    if (gConfig.musicSource != CONFIG_MUSIC_SOURCE_MIDI)
        CDStartup();
    else
        StartupMilesBackend();
    if (previousTrack >= 0)
        PlayAmbientMusic(previousTrack);
}

VA(0x004b64d0, 0x106)
void soundManager::PlayAmbientMusic(i32 track) {
    if (!gSoundBackendsReady)
        return;
    if (m_musicTrack == track)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED) {
        m_musicTrack = track;
        return;
    }

    if (IsAudiereBackend(this)) {
        PlayAudiereMusic(
            m_audiereDevice,
            m_musicTrack,
            m_musicFadeSteps,
            track
        );
    } else if (IsMilesBackend(this)) {
        MIDIPlay(m_musicTrack, m_musicFadeSteps, track);
    }
    m_musicTrack = track;
}

VA(0x004b65e0, 0x227)
void soundManager::PollSound(void) {
    i32 musicFadeStep;
    i32 volume;
    i32l delta;
    i32l switchDelta;
    if (m_musicFadeSteps == 0)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED)
        return;
    if (m_musicFadeSteps > 0) {
        Process1WindowsMessage();
        if (m_musicTrack < CD_MUSIC_TRACK_FIRST
            || m_musicTrack > CD_MUSIC_TRACK_LAST)
            glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount();
        delta = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - KBTickCount();
        m_musicFadeSteps = delta / FADE_STEP_TICKS;
        if (m_musicFadeSteps < 1)
            m_musicFadeSteps = 0;

        if (m_musicFadeSteps <= FADE_HOLD_STEPS
            && m_musicTrack != m_musicFadeTargetTrack) {
            if (bSaveMusicPosition[m_musicTrack] == 0)
                glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] = KBTickCount();
            PlayAmbientMusic(m_musicFadeTargetTrack);
            switchDelta = glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] - KBTickCount();
            m_musicFadeSteps = switchDelta / FADE_STEP_TICKS;
            if (m_musicFadeSteps < 1)
                m_musicFadeSteps = 0;
            m_musicTrack = m_musicFadeTargetTrack;
        }

        musicFadeStep = m_musicFadeSteps;
        if (m_musicFadeSteps <= FADE_HOLD_STEPS)
            volume = (FADE_TOTAL_STEPS - m_musicFadeSteps)
                   * SAMPLE_VOLUME_MAX / FADE_TOTAL_STEPS;
        else
            volume = (m_musicFadeSteps - FADE_HOLD_STEPS)
                   * SAMPLE_VOLUME_MAX / FADE_SAMPLE_RISE_STEPS;
        if (volume > SAMPLE_VOLUME_MAX)
            volume = SAMPLE_VOLUME_MAX;
        if (volume < 0)
            volume = 0;

        if (IsAudiereBackend(this)) {
            volume = (FADE_TOTAL_STEPS - IDX(gConfig.musicVolume)) * volume
                   * MIDI_VOLUME_MAX / CD_VOLUME_SCALE_DIVISOR;
            if (volume > MIDI_VOLUME_MAX)
                volume = MIDI_VOLUME_MAX;
            if (volume < 0)
                volume = 0;
            SetAudiereMusicVolume(volume, true);
        } else if (IsMilesBackend(this)) {
            MIDISetVolume(m_musicFadeSteps);
        }
    }
}

VA(0x004b6810, 0xbb)
void soundManager::SwitchAmbientMusic(i32 track) {
    if (!gSoundBackendsReady)
        return;
    if (gConfig.musicVolume == CONFIG_VOLUME_MUTED) {
        m_musicTrack = track;
        return;
    }
    if (MusicPlaying() == 0) {
        PlayAmbientMusic(track);
        return;
    }
    if (m_musicTrack == track)
        return;
    Process1WindowsMessage();
    if ((m_musicFadeSteps != 0
         && track != m_musicFadeTargetTrack)
        || (m_musicFadeSteps == 0
            && track != m_musicTrack)) {
        if (m_musicFadeSteps <= FADE_HOLD_STEPS) {
            m_musicFadeSteps = FADE_TOTAL_STEPS;
            glTimers[GLOBAL_MUSIC_FADE_TIMER_SLOT] =
                KBTickCount() + AMBIENT_FADE_DELAY_TICKS;
        }
        m_musicFadeTargetTrack = track;
        PollSound();
    }
}

VA(0x004b68d0, 0x115)
void soundManager::MemorySample(class sample* sampleResource) {
    if (sampleResource == NULL)
        return;
    if (!gSoundBackendsReady)
        return;
    if (gConfig.soundVolume == CONFIG_VOLUME_MUTED)
        return;
    if (IsAudiereBackend(this)) {
        PlayAudiereSample(sampleResource, m_audiereDevice);
    } else if (IsMilesBackend(this)) {
        PlayMilesSample(sampleResource);
    }
}

// @remove
void soundManager::GetNumberCDDrives(void) {}

VA(0x004b69f0, 0x41)
void soundManager::ServiceSound(void) {
    if (IsMilesBackend(this))
        ServiceMilesSamples();
}

VA(0x004b6a40, 0x49)
i32 soundManager::MusicPlaying(void) {
    if (m_backend == SOUND_BACKEND_AUDIERE)
        return AudiereMusicPlaying();
    if (m_backend == SOUND_BACKEND_MILES) {
        if (MusicFlagsActive())
            return false;
        return MIDIIsPlaying();
    }
    return false;
}




char* digitalDriverNames[DIGITAL_DRIVER_NAME_COUNT] = {
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
    NULL
};
DATA(0x00520188) SampleChannelStruct SCS[SOUND_CHANNEL_TYPE_COUNT] = {{0, 1, 0}, {1, 2, 1}, {2, 6, 2}, {6, 16, 6}};
char CDPreviousPosition[MIDI_TRACK_COUNT][CD_POSITION_CAPACITY] = {0};
i32 CDWaiting = -1;
i32 CDPlaying = 0;
i32 iCalibrateLoop = 0;
char lpszReturnString[MCI_RESULT_CAPACITY];
u32l nMCIError;
i16 iLastVolume[SAVED_SAMPLE_VOLUME_CAPACITY];
char CommandString[MCI_COMMAND_CAPACITY];

