#include <PLATFORM/Smacker.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <new>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
}

#include <PLATFORM/Platform.h>

#include "State.h"

namespace {

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
    std::array<u8, 768> palette{};
};

SmackerState* State(Smack* smack) {
    return smack != nullptr ? static_cast<SmackerState*>(smack->Reserved) : nullptr;
}

void CloseState(SmackerState* state) {
    if (state == nullptr) {
        return;
    }
    av_packet_free(&state->packet);
    av_frame_free(&state->frame);
    avcodec_free_context(&state->codec);
    avformat_close_input(&state->format);
    delete state;
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
