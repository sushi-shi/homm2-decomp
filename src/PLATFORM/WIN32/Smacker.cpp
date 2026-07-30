#include <PLATFORM/Smacker.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <new>
#include <string>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include <PLATFORM/Platform.h>

#include "State.h"

namespace {

constexpr u32 kAudioTrackFlags = 0xfe000;

struct AudioDecoder {
    AVFormatContext* format = nullptr;
    AVCodecContext* codec = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* packet = nullptr;
    SwrContext* resampler = nullptr;

    ~AudioDecoder() {
        swr_free(&resampler);
        av_packet_free(&packet);
        av_frame_free(&frame);
        avcodec_free_context(&codec);
        avformat_close_input(&format);
    }
};

struct DecodedAudio {
    std::vector<u8> samples;
    std::int32_t sampleRate = 0;
    std::int32_t channels = 0;
};

struct SmackerState {
    AVFormatContext* format = nullptr;
    AVCodecContext* codec = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* packet = nullptr;
    std::int32_t stream = -1;
    u8* destination = nullptr;
    u32 left = 0;
    u32 top = 0;
    u32 pitch = 0;
    u32 destinationHeight = 0;
    u32 nextFrameTime = 0;
    bool draining = false;
    bool rectPending = false;
    bool paletteReady = false;
    platform::VoiceId audioVoice = 0;
    std::array<u8, 768> palette{};
};

SmackerState* State(Smack* smack) {
    return smack != nullptr ? static_cast<SmackerState*>(smack->Reserved) : nullptr;
}

void CloseState(SmackerState* state) {
    if (state == nullptr) {
        return;
    }
    platform::Audio().StopVoice(state->audioVoice);
    av_packet_free(&state->packet);
    av_frame_free(&state->frame);
    avcodec_free_context(&state->codec);
    avformat_close_input(&state->format);
    delete state;
}

bool AppendAudioFrame(AudioDecoder& decoder, DecodedAudio& audio) {
    const std::int32_t capacity =
        swr_get_out_samples(decoder.resampler, decoder.frame->nb_samples);
    if (capacity < 0) {
        return false;
    }

    const std::size_t first = audio.samples.size();
    audio.samples.resize(
        first + static_cast<std::size_t>(capacity) * audio.channels * sizeof(std::int16_t)
    );
    u8* output = audio.samples.data() + first;
    const u8* const* input =
        const_cast<const u8* const*>(decoder.frame->extended_data);
    const std::int32_t converted = swr_convert(
        decoder.resampler,
        &output,
        capacity,
        input,
        decoder.frame->nb_samples
    );
    if (converted < 0) {
        return false;
    }
    audio.samples.resize(
        first + static_cast<std::size_t>(converted) * audio.channels * sizeof(std::int16_t)
    );
    return true;
}

bool ReceiveAudio(AudioDecoder& decoder, DecodedAudio& audio) {
    for (;;) {
        const std::int32_t received =
            avcodec_receive_frame(decoder.codec, decoder.frame);
        if (received == AVERROR(EAGAIN) || received == AVERROR_EOF) {
            return true;
        }
        if (received < 0 || !AppendAudioFrame(decoder, audio)) {
            return false;
        }
        av_frame_unref(decoder.frame);
    }
}

bool DecodeAudio(const std::string& path, DecodedAudio& audio) {
    AudioDecoder decoder;
    if (avformat_open_input(&decoder.format, path.c_str(), nullptr, nullptr) < 0
        || avformat_find_stream_info(decoder.format, nullptr) < 0) {
        return false;
    }

    const AVCodec* codec = nullptr;
    const std::int32_t stream =
        av_find_best_stream(decoder.format, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
    if (stream < 0 || codec == nullptr) {
        return false;
    }

    decoder.codec = avcodec_alloc_context3(codec);
    decoder.frame = av_frame_alloc();
    decoder.packet = av_packet_alloc();
    if (decoder.codec == nullptr || decoder.frame == nullptr || decoder.packet == nullptr
        || avcodec_parameters_to_context(
               decoder.codec,
               decoder.format->streams[stream]->codecpar
           ) < 0
        || avcodec_open2(decoder.codec, codec, nullptr) < 0) {
        return false;
    }

    AVChannelLayout inputLayout{};
    if (decoder.codec->ch_layout.nb_channels > 0) {
        if (av_channel_layout_copy(&inputLayout, &decoder.codec->ch_layout) < 0) {
            return false;
        }
    } else {
        av_channel_layout_default(&inputLayout, 1);
    }
    audio.channels = std::clamp(inputLayout.nb_channels, 1, 2);
    audio.sampleRate = decoder.codec->sample_rate;
    AVChannelLayout outputLayout{};
    av_channel_layout_default(&outputLayout, audio.channels);
    const std::int32_t configured = swr_alloc_set_opts2(
        &decoder.resampler,
        &outputLayout,
        AV_SAMPLE_FMT_S16,
        audio.sampleRate,
        &inputLayout,
        decoder.codec->sample_fmt,
        audio.sampleRate,
        0,
        nullptr
    );
    av_channel_layout_uninit(&outputLayout);
    av_channel_layout_uninit(&inputLayout);
    if (configured < 0 || swr_init(decoder.resampler) < 0) {
        return false;
    }

    while (av_read_frame(decoder.format, decoder.packet) >= 0) {
        if (decoder.packet->stream_index == stream
            && avcodec_send_packet(decoder.codec, decoder.packet) >= 0
            && !ReceiveAudio(decoder, audio)) {
            av_packet_unref(decoder.packet);
            return false;
        }
        av_packet_unref(decoder.packet);
    }
    if (avcodec_send_packet(decoder.codec, nullptr) < 0
        || !ReceiveAudio(decoder, audio)) {
        return false;
    }
    return !audio.samples.empty();
}

bool ReceiveFrame(SmackerState& state) {
    for (;;) {
        const std::int32_t received = avcodec_receive_frame(state.codec, state.frame);
        if (received == 0) {
            return true;
        }
        if (received != AVERROR(EAGAIN)) {
            return false;
        }

        bool sent = false;
        while (!sent) {
            const std::int32_t read = av_read_frame(state.format, state.packet);
            if (read < 0) {
                if (state.draining) {
                    return false;
                }
                state.draining = true;
                if (avcodec_send_packet(state.codec, nullptr) < 0) {
                    return false;
                }
                sent = true;
            } else {
                if (state.packet->stream_index == state.stream) {
                    sent = avcodec_send_packet(state.codec, state.packet) >= 0;
                }
                av_packet_unref(state.packet);
            }
        }
    }
}

bool LoadFrame(Smack& smack, SmackerState& state) {
    av_frame_unref(state.frame);
    if (!ReceiveFrame(state) || state.frame->format != AV_PIX_FMT_PAL8
        || state.frame->data[0] == nullptr || state.frame->data[1] == nullptr) {
        return false;
    }

    std::array<u8, 768> palette;
    for (std::int32_t index = 0; index < 256; ++index) {
        u32 color;
        std::memcpy(&color, state.frame->data[1] + index * sizeof(color), sizeof(color));
        palette[index * 3] = static_cast<u8>(color >> 16);
        palette[index * 3 + 1] = static_cast<u8>(color >> 8);
        palette[index * 3 + 2] = static_cast<u8>(color);
    }

    smack.NewPalette = !state.paletteReady || palette != state.palette;
    if (smack.NewPalette) {
        state.palette = palette;
        std::copy(palette.begin(), palette.end(), smack.Palette);
        state.paletteReady = true;
    }
    state.rectPending = true;
    return true;
}

}

Smack* SmackOpen(const char* name, u32 flags, u32) {
    Smack* smack = new (std::nothrow) Smack{};
    SmackerState* state = new (std::nothrow) SmackerState{};
    if (smack == nullptr || state == nullptr) {
        delete smack;
        delete state;
        return nullptr;
    }

    const std::string path = platform::win32::ResolvePath(name);
    if (avformat_open_input(&state->format, path.c_str(), nullptr, nullptr) < 0
        || avformat_find_stream_info(state->format, nullptr) < 0) {
        CloseState(state);
        delete smack;
        return nullptr;
    }

    const AVCodec* decoder = nullptr;
    state->stream =
        av_find_best_stream(state->format, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (state->stream < 0 || decoder == nullptr) {
        CloseState(state);
        delete smack;
        return nullptr;
    }

    AVStream* stream = state->format->streams[state->stream];
    state->codec = avcodec_alloc_context3(decoder);
    state->frame = av_frame_alloc();
    state->packet = av_packet_alloc();
    if (state->codec == nullptr || state->frame == nullptr || state->packet == nullptr
        || avcodec_parameters_to_context(state->codec, stream->codecpar) < 0
        || avcodec_open2(state->codec, decoder, nullptr) < 0) {
        CloseState(state);
        delete smack;
        return nullptr;
    }

    smack->Width = static_cast<u32>(state->codec->width);
    smack->Height = static_cast<u32>(state->codec->height);
    smack->Frames = stream->duration > 0 ? static_cast<u32>(stream->duration) : 1;
    smack->MSPerFrame =
        std::max(1u, static_cast<u32>(av_q2d(stream->time_base) * 1000.0 + 0.5));
    smack->OpenFlags = flags;
    smack->Reserved = state;
    state->nextFrameTime = platform::Host().Ticks();

    if ((flags & kAudioTrackFlags) != 0) {
        DecodedAudio audio;
        if (DecodeAudio(path, audio)) {
            platform::SoundData sound;
            sound.samples = audio.samples.data();
            sound.byteCount = audio.samples.size();
            sound.sampleRate = audio.sampleRate;
            sound.channels = audio.channels;
            sound.bitsPerSample = 16;
            state->audioVoice = platform::Audio().PlaySound(sound, 127, 0);
        }
    }

    if (!LoadFrame(*smack, *state)) {
        SmackClose(smack);
        return nullptr;
    }
    return smack;
}

void SmackClose(Smack* smack) {
    if (smack == nullptr) {
        return;
    }
    CloseState(State(smack));
    delete smack;
}

u32 SmackDoFrame(Smack* smack) {
    if (smack != nullptr) {
        smack->NewPalette = 0;
    }
    return 0;
}

void SmackNextFrame(Smack* smack) {
    SmackerState* state = State(smack);
    if (smack == nullptr || state == nullptr || smack->FrameNum >= smack->Frames) {
        return;
    }

    ++smack->FrameNum;
    state->nextFrameTime += smack->MSPerFrame;
    if (smack->FrameNum < smack->Frames && !LoadFrame(*smack, *state)) {
        smack->Frames = smack->FrameNum;
    }
}

u32 SmackWait(Smack* smack) {
    SmackerState* state = State(smack);
    if (state == nullptr) {
        return 0;
    }
    return static_cast<std::int32_t>(platform::Host().Ticks() - state->nextFrameTime) < 0;
}

void SmackSummary(Smack*, SmackSum* summary) {
    if (summary != nullptr) {
        *summary = {};
    }
}

void SmackToBuffer(
    Smack* smack,
    u32 left,
    u32 top,
    u32 pitch,
    u32 destinationHeight,
    void* buffer,
    u32
) {
    SmackerState* state = State(smack);
    if (state == nullptr) {
        return;
    }
    state->destination = static_cast<u8*>(buffer);
    state->left = left;
    state->top = top;
    state->pitch = pitch;
    state->destinationHeight = destinationHeight;
}

u32 SmackToBufferRect(Smack* smack, u32) {
    SmackerState* state = State(smack);
    if (smack == nullptr || state == nullptr || !state->rectPending || state->destination == nullptr) {
        return 0;
    }

    const u32 width = std::min(smack->Width, state->pitch - std::min(state->left, state->pitch));
    const u32 height =
        std::min(smack->Height, state->destinationHeight - std::min(state->top, state->destinationHeight));
    for (u32 row = 0; row < height; ++row) {
        std::memcpy(
            state->destination + (state->top + row) * state->pitch + state->left,
            state->frame->data[0] + row * state->frame->linesize[0],
            width
        );
    }

    smack->LastRectx = state->left;
    smack->LastRecty = state->top;
    smack->LastRectw = width;
    smack->LastRecth = height;
    state->rectPending = false;
    return 1;
}
