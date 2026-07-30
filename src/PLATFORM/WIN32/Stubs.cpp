#include <mss.h>
#include <smack.h>
#include <windows.h>

#include <Ints.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <sys/stat.h>
#include <cstring>
#include <map>
#include <string>

#include <PLATFORM/Platform.h>

#include "State.h"

namespace {

int gRegistryKey = 0;

}

LONG RegOpenKeyExA(HKEY, LPCSTR, DWORD, DWORD, PHKEY result) {
    if (result != nullptr) {
        *result = &gRegistryKey;
    }
    return ERROR_SUCCESS;
}

LONG RegCreateKeyA(HKEY, LPCSTR, PHKEY result) {
    if (result != nullptr) {
        *result = &gRegistryKey;
    }
    return ERROR_SUCCESS;
}

LONG RegQueryValueExA(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD) {
    return ERROR_FILE_NOT_FOUND;
}

LONG RegSetValueExA(HKEY, LPCSTR, DWORD, DWORD, const BYTE*, DWORD) { return ERROR_SUCCESS; }
LONG RegCloseKey(HKEY) { return ERROR_SUCCESS; }

BOOL GetVersionExA(LPOSVERSIONINFOA info) {
    if (info == nullptr) {
        return FALSE;
    }
    info->dwMajorVersion = 10;
    info->dwMinorVersion = 0;
    info->dwBuildNumber = 19045;
    info->dwPlatformId = VER_PLATFORM_WIN32_NT;
    info->szCSDVersion[0] = '\0';
    return TRUE;
}

DWORD GetVersion() { return 0x0A00; }

UINT waveOutGetNumDevs() { return 1; }

MMRESULT waveOutGetDevCapsA(UINT_PTR, LPWAVEOUTCAPSA caps, UINT) {
    if (caps != nullptr) {
        std::memset(caps, 0, sizeof(*caps));
        std::strcpy(caps->szPname, "Platform audio");
        caps->wChannels = 2;
    }
    return 0;
}

MMRESULT auxSetVolume(UINT, DWORD) { return 0; }

DWORD mciSendStringA(LPCSTR, LPSTR returnString, UINT returnLength, HWND) {
    if (returnString != nullptr && returnLength > 0) {
        returnString[0] = '\0';
    }
    return 1;
}

BOOL mciGetErrorStringA(DWORD, LPSTR buffer, UINT length) {
    if (buffer != nullptr && length > 0) {
        std::strncpy(buffer, "no CD audio device", length - 1);
        buffer[length - 1] = '\0';
    }
    return TRUE;
}

extern "C" u8 Netbios(void*) { return 0x23; }

namespace {

struct Sample {
    const void* data = nullptr;
    U32 length = 0;
    S32 volume = AIL_MAX_SAMPLE_VOLUME;
    S32 rate = 22050;
    S32 loops = 1;
    bool playing = false;
    platform::VoiceId voice = 0;
};

std::map<HSAMPLE, Sample*> gSamples;
int gNextSample = 1;

}

S32 AIL_startup() {
    platform::Startup();
    platform::Audio().Open();
    return 1;
}

void AIL_shutdown() {}

S32 AIL_waveOutOpen(HDIGDRIVER* driver, void*, S32, const void*) {
    platform::Startup();
    platform::Audio().Open();
    if (driver != nullptr) {
        *driver = reinterpret_cast<HDIGDRIVER>(static_cast<std::uintptr_t>(1));
    }
    return 0;
}

void AIL_waveOutClose(HDIGDRIVER) {}

S32 AIL_midiOutOpen(HMDIDRIVER* driver, void*, S32) {
    if (driver != nullptr) {
        *driver = reinterpret_cast<HMDIDRIVER>(static_cast<std::uintptr_t>(1));
    }
    return 0;
}

void AIL_midiOutClose(HMDIDRIVER) {}

void AIL_set_XMIDI_master_volume(HMDIDRIVER, S32 volume) {
    platform::Audio().SetMusicVolume(volume);
}

S32 AIL_XMIDI_master_volume(HMDIDRIVER) { return 0; }
void AIL_serve() { platform::Audio().Service(); }
char* AIL_last_error() { return const_cast<char*>(""); }
S32 AIL_set_preference(U32, S32) { return 0; }
S32 AIL_get_preference(U32 number) {

    return number == 15 ? 1 : 0;
}

HSAMPLE AIL_allocate_sample_handle(HDIGDRIVER) {
    HSAMPLE handle = reinterpret_cast<HSAMPLE>(static_cast<std::uintptr_t>(gNextSample++));
    gSamples[handle] = new Sample();
    return handle;
}

void AIL_release_sample_handle(HSAMPLE sample) {
    if (const auto found = gSamples.find(sample); found != gSamples.end()) {
        delete found->second;
        gSamples.erase(found);
    }
}

void AIL_init_sample(HSAMPLE sample) {
    if (const auto found = gSamples.find(sample); found != gSamples.end()) {
        *found->second = Sample();
    }
}

void AIL_start_sample(HSAMPLE sample) {
    const auto found = gSamples.find(sample);
    if (found == gSamples.end() || found->second->data == nullptr) {
        return;
    }
    Sample& state = *found->second;

    platform::SoundData sound;
    sound.samples = state.data;
    sound.byteCount = state.length;
    sound.sampleRate = state.rate;
    sound.channels = 1;
    sound.bitsPerSample = 8;
    state.voice = platform::Audio().PlaySound(sound, state.volume, state.loops - 1);
    state.playing = state.voice != 0;
}

void AIL_end_sample(HSAMPLE sample) {
    if (const auto found = gSamples.find(sample); found != gSamples.end()) {
        platform::Audio().StopVoice(found->second->voice);
        found->second->playing = false;
    }
}

U32 AIL_sample_status(HSAMPLE sample) {
    const auto found = gSamples.find(sample);
    if (found == gSamples.end()) {
        return SMP_FREE;
    }
    if (found->second->playing && platform::Audio().IsVoicePlaying(found->second->voice)) {
        return SMP_PLAYING;
    }
    found->second->playing = false;
    return SMP_DONE;
}

S32 AIL_sample_volume(HSAMPLE sample) {
    const auto found = gSamples.find(sample);
    return found != gSamples.end() ? found->second->volume : 0;
}

void AIL_set_sample_address(HSAMPLE sample, void* start, U32 length) {
    if (const auto found = gSamples.find(sample); found != gSamples.end()) {
        found->second->data = start;
        found->second->length = length;
    }
}

void AIL_set_sample_volume(HSAMPLE sample, S32 volume) {
    if (const auto found = gSamples.find(sample); found != gSamples.end()) {
        found->second->volume = volume;
        platform::Audio().SetVoiceVolume(found->second->voice, volume);
    }
}

void AIL_set_sample_loop_count(HSAMPLE sample, S32 loopCount) {
    if (const auto found = gSamples.find(sample); found != gSamples.end()) {
        found->second->loops = loopCount;
    }
}

void AIL_set_sample_playback_rate(HSAMPLE sample, S32 rate) {
    if (const auto found = gSamples.find(sample); found != gSamples.end()) {
        found->second->rate = rate;
    }
}

void AIL_set_sample_type(HSAMPLE, S32, U32) {}

HSEQUENCE AIL_allocate_sequence_handle(HMDIDRIVER) {
    return reinterpret_cast<HSEQUENCE>(static_cast<std::uintptr_t>(1));
}
void AIL_release_sequence_handle(HSEQUENCE) {}
S32 AIL_init_sequence(HSEQUENCE, void*, S32) { return 1; }
void AIL_start_sequence(HSEQUENCE) {}
void AIL_stop_sequence(HSEQUENCE) {}
void AIL_resume_sequence(HSEQUENCE) {}
U32 AIL_sequence_status(HSEQUENCE) { return SEQ_DONE; }
void AIL_set_sequence_loop_count(HSEQUENCE, S32) {}

HREDBOOK AIL_redbook_open(U32) { return nullptr; }
void AIL_redbook_close(HREDBOOK) {}
U32 AIL_redbook_tracks(HREDBOOK) { return 0; }
void AIL_redbook_track_info(HREDBOOK, U32, U32* start, U32* end) {
    if (start != nullptr) *start = 0;
    if (end != nullptr) *end = 0;
}

Smack* SmackOpen(const char*, u32, u32) {
    Smack* smack = new Smack{};
    smack->Width = 640;
    smack->Height = 480;
    smack->Frames = 1;
    return smack;
}
void SmackClose(Smack* smack) { delete smack; }
u32 SmackDoFrame(Smack*) { return 0; }
void SmackNextFrame(Smack* smack) {
    if (smack != nullptr && smack->FrameNum < smack->Frames) {
        ++smack->FrameNum;
    }
}
u32 SmackWait(Smack*) { return 0; }
void SmackSummary(Smack*, SmackSum* summary) {
    if (summary != nullptr) {
        *summary = {};
    }
}
void SmackToBuffer(Smack*, u32, u32, u32, u32, void*, u32) {}
u32 SmackToBufferRect(Smack*, u32) { return 0; }
u8 SmackSoundUseMSS(void*) { return 1; }
u8 SmackSoundUseDirectSound(void*) { return 0; }
