#include <PLATFORM/Movie.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <map>
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

namespace platform {
namespace {

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
        if (decoder.packet->stream_index == stream) {
            if (avcodec_send_packet(decoder.codec, decoder.packet) < 0
                || !ReceiveAudio(decoder, audio)) {
                av_packet_unref(decoder.packet);
                return false;
            }
        }
        av_packet_unref(decoder.packet);
    }
    if (avcodec_send_packet(decoder.codec, nullptr) < 0
        || !ReceiveAudio(decoder, audio)) {
        return false;
    }
    return !audio.samples.empty();
}

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
MovieId gNextMovie = 1;

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
        || avcodec_open2(state->codec, decoder, nullptr) < 0) {
        Release(state);
        return kInvalidMovie;
    }

    state->width = state->codec->width;
    state->height = state->codec->height;
    state->count = stream->duration > 0 ? static_cast<i32>(stream->duration) : 1;
    state->msPerFrame = std::max(1u, static_cast<u32>(av_q2d(stream->time_base) * 1000.0 + 0.5));

    if (withSound) {
        DecodedAudio audio;
        if (DecodeAudio(path, audio)) {
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

    const MovieId movie = gNextMovie++;
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
    state->destination = static_cast<u8*>(buffer);
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

    const i32 width = std::min(state->width, state->pitch - std::min(state->left, state->pitch));
    const i32 height =
        std::min(state->height, state->destinationHeight - std::min(state->top, state->destinationHeight));
    for (i32 row = 0; row < height; ++row) {
        std::memcpy(
            state->destination + (state->top + row) * state->pitch + state->left,
            state->frame->data[0] + row * state->frame->linesize[0],
            static_cast<std::size_t>(width)
        );
    }

    frame.index = state->index;
    frame.count = state->count;
    frame.dirty = Rect{state->left, state->top, width, height};
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

}
