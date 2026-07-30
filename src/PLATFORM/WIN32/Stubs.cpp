#include <mss.h>
#include <smack.h>
#include <windows.h>

#include <Ints.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <sys/stat.h>
#include <cstring>
#include <map>
#include <string>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include <PLATFORM/Platform.h>

#include "State.h"

struct _SEQUENCE {
    int track = -1;
    int loops = 1;
    bool playing = false;
};

extern HSEQUENCE hSequence[60];

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

extern "C" u8 Netbios(void*) { return 0x23; }

namespace {

struct Sample {
    const void* data = nullptr;
    U32 length = 0;
    S32 volume = AIL_MAX_SAMPLE_VOLUME;
    S32 rate = 22050;
    S32 format = 0;
    S32 loops = 1;
    bool playing = false;
    platform::VoiceId voice = 0;
};

std::map<HSAMPLE, Sample*> gSamples;
int gNextSample = 1;

bool AppendAudioFrame(
    AVFrame* frame,
    SwrContext* converter,
    std::vector<std::int16_t>& samples
) {
    const int channels = 2;
    const int capacity = swr_get_out_samples(converter, frame->nb_samples);
    if (capacity < 0) {
        return false;
    }
    const std::size_t first = samples.size();
    samples.resize(first + static_cast<std::size_t>(capacity) * channels);
    std::uint8_t* output =
        reinterpret_cast<std::uint8_t*>(samples.data() + first);
    const int converted = swr_convert(
        converter,
        &output,
        capacity,
        const_cast<const std::uint8_t**>(frame->extended_data),
        frame->nb_samples
    );
    if (converted < 0) {
        return false;
    }
    samples.resize(first + static_cast<std::size_t>(converted) * channels);
    return true;
}

bool ReceiveAudio(
    AVCodecContext* codec,
    AVFrame* frame,
    SwrContext* converter,
    std::vector<std::int16_t>& samples
) {
    for (;;) {
        const int result = avcodec_receive_frame(codec, frame);
        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            return true;
        }
        if (result < 0 || !AppendAudioFrame(frame, converter, samples)) {
            return false;
        }
        av_frame_unref(frame);
    }
}

bool DecodeMusic(
    const std::string& path,
    std::vector<std::int16_t>& samples,
    int& sampleRate
) {
    AVFormatContext* format = nullptr;
    AVCodecContext* codec = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* packet = nullptr;
    SwrContext* converter = nullptr;
    const AVCodec* decoder = nullptr;
    int stream = -1;
    AVChannelLayout stereo = AV_CHANNEL_LAYOUT_STEREO;
    bool success = false;

    if (avformat_open_input(&format, path.c_str(), nullptr, nullptr) < 0
        || avformat_find_stream_info(format, nullptr) < 0) {
        goto done;
    }

    stream = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, &decoder, 0);
    if (stream < 0 || decoder == nullptr) {
        goto done;
    }

    codec = avcodec_alloc_context3(decoder);
    frame = av_frame_alloc();
    packet = av_packet_alloc();
    if (codec == nullptr || frame == nullptr || packet == nullptr
        || avcodec_parameters_to_context(codec, format->streams[stream]->codecpar) < 0
        || avcodec_open2(codec, decoder, nullptr) < 0) {
        goto done;
    }

    sampleRate = codec->sample_rate;
    if (swr_alloc_set_opts2(
            &converter,
            &stereo,
            AV_SAMPLE_FMT_S16,
            sampleRate,
            &codec->ch_layout,
            codec->sample_fmt,
            sampleRate,
            0,
            nullptr
        )
            < 0
        || swr_init(converter) < 0) {
        goto done;
    }

    while (av_read_frame(format, packet) >= 0) {
        if (packet->stream_index == stream
            && (avcodec_send_packet(codec, packet) < 0
                || !ReceiveAudio(codec, frame, converter, samples))) {
            av_packet_unref(packet);
            goto done;
        }
        av_packet_unref(packet);
    }
    if (avcodec_send_packet(codec, nullptr) < 0
        || !ReceiveAudio(codec, frame, converter, samples)) {
        goto done;
    }
    success = !samples.empty();

done:
    swr_free(&converter);
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return success;
}

bool StartSequence(_SEQUENCE& sequence) {
    if (sequence.track < 0) {
        return false;
    }
    char relativePath[32];
    std::snprintf(
        relativePath,
        sizeof(relativePath),
        "MUSIC/Track%02d.ogg",
        sequence.track
    );
    std::vector<std::int16_t> samples;
    int sampleRate = 0;
    const std::string path = platform::Files().Resolve(relativePath);
    if (!DecodeMusic(path, samples, sampleRate)) {
        std::fprintf(stderr, "[homm2] unable to decode music: %s\n", path.c_str());
        return false;
    }

    platform::SoundData sound;
    sound.samples = samples.data();
    sound.byteCount = samples.size() * sizeof(samples[0]);
    sound.sampleRate = sampleRate;
    sound.channels = 2;
    sound.bitsPerSample = 16;
    sequence.playing =
        platform::Audio().PlayMusic(sound, sequence.loops == 0 ? -1 : sequence.loops - 1);
    return sequence.playing;
}

}

S32 AIL_startup() {
    platform::Startup();
    return platform::Audio().Open() ? 1 : 0;
}

void AIL_shutdown() {}

S32 AIL_waveOutOpen(HDIGDRIVER* driver, void*, S32, const void*) {
    platform::Startup();
    if (!platform::Audio().Open()) {
        return 1;
    }
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
        platform::Audio().StopVoice(found->second->voice);
        delete found->second;
        gSamples.erase(found);
    }
}

void AIL_init_sample(HSAMPLE sample) {
    if (const auto found = gSamples.find(sample); found != gSamples.end()) {
        platform::Audio().StopVoice(found->second->voice);
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
    sound.channels = (state.format & 2) != 0 ? 2 : 1;
    sound.bitsPerSample = (state.format & 1) != 0 ? 16 : 8;
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

void AIL_set_sample_type(HSAMPLE sample, S32 format, U32) {
    if (const auto found = gSamples.find(sample); found != gSamples.end()) {
        found->second->format = format;
    }
}

HSEQUENCE AIL_allocate_sequence_handle(HMDIDRIVER) { return new _SEQUENCE(); }

void AIL_release_sequence_handle(HSEQUENCE sequence) {
    if (sequence == nullptr) {
        return;
    }
    if (sequence->playing) {
        platform::Audio().StopMusic();
    }
    delete sequence;
}

S32 AIL_init_sequence(HSEQUENCE sequence, void*, S32) {
    if (sequence == nullptr) {
        return 0;
    }
    for (int track = 0; track < 60; ++track) {
        if (hSequence[track] == sequence) {
            sequence->track = track;
            return 1;
        }
    }
    return 0;
}

void AIL_start_sequence(HSEQUENCE sequence) {
    if (sequence != nullptr) {
        sequence->playing = false;
    }
}

void AIL_stop_sequence(HSEQUENCE sequence) {
    if (sequence != nullptr && sequence->playing) {
        platform::Audio().StopMusic();
        sequence->playing = false;
    }
}

void AIL_resume_sequence(HSEQUENCE sequence) {
    if (sequence != nullptr) {
        StartSequence(*sequence);
    }
}

U32 AIL_sequence_status(HSEQUENCE sequence) {
    if (sequence != nullptr && sequence->playing && platform::Audio().IsMusicPlaying()) {
        return SEQ_PLAYING;
    }
    if (sequence != nullptr) {
        sequence->playing = false;
    }
    return SEQ_DONE;
}

void AIL_set_sequence_loop_count(HSEQUENCE sequence, S32 loopCount) {
    if (sequence != nullptr) {
        sequence->loops = loopCount;
        StartSequence(*sequence);
    }
}

u8 SmackSoundUseMSS(void*) { return 1; }
u8 SmackSoundUseDirectSound(void*) { return 0; }
