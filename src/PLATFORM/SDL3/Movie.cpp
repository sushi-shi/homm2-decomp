#include <PLATFORM/Movie.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
}

#include <PLATFORM/Platform.h>

#include "Sdl3Internal.h"

namespace platform {
namespace {

struct Movie {
    AVFormatContext* format = nullptr;
    AVCodecContext* codec = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* packet = nullptr;
    std::int32_t stream = -1;

    u8* destination = nullptr;
    i32 left = 0;
    i32 top = 0;
    i32 pitch = 0;
    i32 destinationHeight = 0;

    i32 width = 0;
    i32 height = 0;
    i32 index = 0;
    i32 count = 1;
    u32 msPerFrame = 1;
    u32 nextFrameTime = 0;

    bool draining = false;
    bool framePending = false;
    bool paletteReady = false;
    bool paletteChanged = false;
    std::array<u8, 768> palette{};
    VoiceId audioVoice = 0;
};

std::map<MovieId, Movie*> gMovies;
std::uint32_t gNextMovie = 1;

MovieId AllocateMovieId() {
    constexpr std::uint32_t maximum = static_cast<std::uint32_t>(
        std::numeric_limits<MovieId>::max()
    );
    for (std::uint32_t attempt = 0; attempt < maximum; ++attempt) {
        if (gNextMovie == 0 || gNextMovie > maximum) {
            gNextMovie = 1;
        }
        const MovieId candidate = static_cast<MovieId>(gNextMovie++);
        if (!gMovies.contains(candidate)) {
            return candidate;
        }
    }
    return kInvalidMovie;
}

Movie* Find(MovieId movie) {
    const auto found = gMovies.find(movie);
    return found != gMovies.end() ? found->second : nullptr;
}

void Release(Movie* movie) {
    if (movie == nullptr) {
        return;
    }
    Audio().StopVoice(movie->audioVoice);
    av_packet_free(&movie->packet);
    av_frame_free(&movie->frame);
    avcodec_free_context(&movie->codec);
    avformat_close_input(&movie->format);
    delete movie;
}

bool ReceiveFrame(Movie& state) {
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

bool LoadFrame(Movie& state) {
    av_frame_unref(state.frame);
    if (!ReceiveFrame(state) || state.frame->format != AV_PIX_FMT_PAL8
        || state.frame->data[0] == nullptr || state.frame->data[1] == nullptr
        || state.frame->linesize[0] < state.width) {
        return false;
    }

    std::array<u8, 768> palette;
    for (std::size_t index = 0; index < 256; ++index) {
        u32 color;
        std::memcpy(&color, state.frame->data[1] + index * sizeof(color), sizeof(color));
        palette[index * 3] = static_cast<u8>(color >> 16);
        palette[index * 3 + 1] = static_cast<u8>(color >> 8);
        palette[index * 3 + 2] = static_cast<u8>(color);
    }

    state.paletteChanged = !state.paletteReady || palette != state.palette;
    if (state.paletteChanged) {
        state.palette = palette;
        state.paletteReady = true;
    }
    state.framePending = true;
    return true;
}

}

MovieId MovieOpen(const char* retailPath, bool withSound) {
    if (retailPath == nullptr || *retailPath == '\0') {
        return kInvalidMovie;
    }
    Movie* state = new Movie();

    const std::string path = Files().Resolve(retailPath, FileMode::Read);
    if (avformat_open_input(&state->format, path.c_str(), nullptr, nullptr) < 0
        || avformat_find_stream_info(state->format, nullptr) < 0) {
        Release(state);
        return kInvalidMovie;
    }

    const AVCodec* decoder = nullptr;
    state->stream = av_find_best_stream(state->format, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (state->stream < 0 || decoder == nullptr) {
        Release(state);
        return kInvalidMovie;
    }

    AVStream* stream = state->format->streams[state->stream];
    state->codec = avcodec_alloc_context3(decoder);
    state->frame = av_frame_alloc();
    state->packet = av_packet_alloc();
    if (state->codec == nullptr || state->frame == nullptr || state->packet == nullptr
        || avcodec_parameters_to_context(state->codec, stream->codecpar) < 0
        || avcodec_open2(state->codec, decoder, nullptr) < 0
        || state->codec->width <= 0 || state->codec->height <= 0) {
        Release(state);
        return kInvalidMovie;
    }

    state->width = state->codec->width;
    state->height = state->codec->height;
    state->count = stream->duration > 0
        ? static_cast<i32>(std::min<std::int64_t>(
              stream->duration,
              std::numeric_limits<i32>::max()
          ))
        : 1;
    const double milliseconds = av_q2d(stream->time_base) * 1000.0;
    if (std::isfinite(milliseconds) && milliseconds > 0.0) {
        state->msPerFrame = static_cast<u32>(std::clamp(
            milliseconds + 0.5,
            1.0,
            static_cast<double>(std::numeric_limits<u32>::max())
        ));
    }

    if (withSound) {
        sdl3::DecodedAudio audio;
        if (sdl3::DecodeAudioFile(path, 0, audio)) {
            SoundData sound;
            sound.samples = audio.samples.data();
            sound.byteCount = audio.samples.size();
            sound.sampleRate = audio.sampleRate;
            sound.channels = audio.channels;
            sound.bitsPerSample = 16;
            state->audioVoice = Audio().PlaySound(sound, 127, 0);
        }
    }

    if (!LoadFrame(*state)) {
        Release(state);
        return kInvalidMovie;
    }

    // Opening decodes the whole sound track, which takes real time. Start the
    // clock once that is done, or the first frames are already overdue and the
    // movie races to catch up.
    state->nextFrameTime = Host().Ticks();

    const MovieId movie = AllocateMovieId();
    if (movie == kInvalidMovie) {
        Release(state);
        return kInvalidMovie;
    }
    gMovies.emplace(movie, state);
    return movie;
}

void MovieClose(MovieId movie) {
    const auto found = gMovies.find(movie);
    if (found == gMovies.end()) {
        return;
    }
    Release(found->second);
    gMovies.erase(found);
}

void MovieTarget(MovieId movie, void* buffer, i32 pitch, i32 height, i32 left, i32 top) {
    Movie* state = Find(movie);
    if (state == nullptr) {
        return;
    }
    state->destination = buffer != nullptr && pitch > 0 && height > 0
        ? static_cast<u8*>(buffer)
        : nullptr;
    state->pitch = pitch;
    state->destinationHeight = height;
    state->left = left;
    state->top = top;
}

bool MovieDraw(MovieId movie, MovieFrame& frame) {
    Movie* state = Find(movie);
    if (state == nullptr || !state->framePending || state->destination == nullptr) {
        return false;
    }

    sdl3::CopyRegion region;
    if (!sdl3::ClipCopyRegion(
            {0, 0, state->width, state->height},
            {state->left, state->top},
            {state->width, state->height},
            {state->pitch, state->destinationHeight},
            region
        )) {
        state->framePending = false;
        state->paletteChanged = false;
        return false;
    }
    for (i32 row = 0; row < region.height; ++row) {
        std::memcpy(
            state->destination
                + static_cast<std::size_t>(region.destinationY + row)
                    * static_cast<std::size_t>(state->pitch)
                + static_cast<std::size_t>(region.destinationX),
            state->frame->data[0]
                + static_cast<std::size_t>(region.sourceY + row)
                    * static_cast<std::size_t>(state->frame->linesize[0])
                + static_cast<std::size_t>(region.sourceX),
            static_cast<std::size_t>(region.width)
        );
    }

    frame.index = state->index;
    frame.count = state->count;
    frame.dirty = Rect{
        region.destinationX,
        region.destinationY,
        region.width,
        region.height,
    };
    frame.palette = state->paletteChanged ? state->palette.data() : nullptr;

    state->framePending = false;
    state->paletteChanged = false;
    return true;
}

void MovieAdvance(MovieId movie) {
    Movie* state = Find(movie);
    if (state == nullptr || state->index >= state->count) {
        return;
    }

    ++state->index;
    state->nextFrameTime += state->msPerFrame;

    // A stall - a slow load, a window drag - must not turn into a fast-forward
    // through the rest of the movie. Give up on the lost time instead.
    constexpr u32 kResyncAfter = 250;
    const u32 now = Host().Ticks();
    if (static_cast<std::int32_t>(now - state->nextFrameTime) > static_cast<std::int32_t>(kResyncAfter)) {
        state->nextFrameTime = now;
    }

    if (state->index < state->count && !LoadFrame(*state)) {
        state->count = state->index;
    }
}

bool MovieWaiting(MovieId movie) {
    Movie* state = Find(movie);
    if (state == nullptr) {
        return false;
    }
    return static_cast<std::int32_t>(Host().Ticks() - state->nextFrameTime) < 0;
}

bool MovieAtEnd(MovieId movie) {
    Movie* state = Find(movie);
    return state == nullptr || state->index >= state->count;
}

i32 MovieFrameIndex(MovieId movie) {
    Movie* state = Find(movie);
    return state != nullptr ? state->index : 0;
}

i32 MovieFrameCount(MovieId movie) {
    Movie* state = Find(movie);
    return state != nullptr ? state->count : 0;
}

const u8* MoviePalette(MovieId movie) {
    Movie* state = Find(movie);
    return state != nullptr && state->paletteReady ? state->palette.data() : nullptr;
}

Size MovieSize(MovieId movie) {
    Movie* state = Find(movie);
    return state != nullptr ? Size{state->width, state->height} : Size{0, 0};
}

namespace sdl3 {

void CloseMovies() {
    for (auto& [id, movie] : gMovies) {
        static_cast<void>(id);
        Release(movie);
    }
    gMovies.clear();
    gNextMovie = 1;
}

}

}
