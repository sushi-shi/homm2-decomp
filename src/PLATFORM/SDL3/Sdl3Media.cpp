#include "Sdl3Internal.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

namespace platform::sdl3 {
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

bool AppendAudioFrame(AudioDecoder& decoder, DecodedAudio& audio) {
    const int capacity = swr_get_out_samples(decoder.resampler, decoder.frame->nb_samples);
    if (capacity < 0 || audio.channels <= 0) {
        return false;
    }

    const std::size_t channels = static_cast<std::size_t>(audio.channels);
    const std::size_t sampleBytes = channels * sizeof(std::int16_t);
    const std::size_t convertedCapacity = static_cast<std::size_t>(capacity);
    if (convertedCapacity > (std::numeric_limits<std::size_t>::max() - audio.samples.size())
            / sampleBytes) {
        return false;
    }

    const std::size_t first = audio.samples.size();
    audio.samples.resize(first + convertedCapacity * sampleBytes);
    std::uint8_t* output = audio.samples.data() + first;
    const std::uint8_t* const* input =
        const_cast<const std::uint8_t* const*>(decoder.frame->extended_data);
    const int converted = swr_convert(
        decoder.resampler,
        &output,
        capacity,
        input,
        decoder.frame->nb_samples
    );
    if (converted < 0) {
        return false;
    }
    audio.samples.resize(first + static_cast<std::size_t>(converted) * sampleBytes);
    return true;
}

bool FlushAudio(AudioDecoder& decoder, DecodedAudio& audio) {
    for (;;) {
        const int capacity = swr_get_out_samples(decoder.resampler, 0);
        if (capacity < 0 || audio.channels <= 0) {
            return false;
        }
        if (capacity == 0) {
            return true;
        }

        const std::size_t sampleBytes = static_cast<std::size_t>(audio.channels)
            * sizeof(std::int16_t);
        const std::size_t convertedCapacity = static_cast<std::size_t>(capacity);
        if (convertedCapacity > (std::numeric_limits<std::size_t>::max() - audio.samples.size())
                / sampleBytes) {
            return false;
        }

        const std::size_t first = audio.samples.size();
        audio.samples.resize(first + convertedCapacity * sampleBytes);
        std::uint8_t* output = audio.samples.data() + first;
        const int converted = swr_convert(
            decoder.resampler,
            &output,
            capacity,
            nullptr,
            0
        );
        if (converted < 0) {
            return false;
        }
        audio.samples.resize(first + static_cast<std::size_t>(converted) * sampleBytes);
        if (converted == 0) {
            return true;
        }
    }
}

bool ReceiveAudio(AudioDecoder& decoder, DecodedAudio& audio) {
    for (;;) {
        const int received = avcodec_receive_frame(decoder.codec, decoder.frame);
        if (received == AVERROR(EAGAIN) || received == AVERROR_EOF) {
            return true;
        }
        if (received < 0 || !AppendAudioFrame(decoder, audio)) {
            return false;
        }
        av_frame_unref(decoder.frame);
    }
}

}

bool DecodeAudioFile(const std::string& path, int requestedChannels, DecodedAudio& audio) {
    audio = {};
    AudioDecoder decoder;
    if (path.empty()
        || avformat_open_input(&decoder.format, path.c_str(), nullptr, nullptr) < 0
        || avformat_find_stream_info(decoder.format, nullptr) < 0) {
        return false;
    }

    const AVCodec* codec = nullptr;
    const int stream = av_find_best_stream(
        decoder.format,
        AVMEDIA_TYPE_AUDIO,
        -1,
        -1,
        &codec,
        0
    );
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
        || avcodec_open2(decoder.codec, codec, nullptr) < 0
        || decoder.codec->sample_rate <= 0) {
        return false;
    }

    AVChannelLayout inputLayout {};
    if (decoder.codec->ch_layout.nb_channels > 0) {
        if (av_channel_layout_copy(&inputLayout, &decoder.codec->ch_layout) < 0) {
            return false;
        }
    } else {
        av_channel_layout_default(&inputLayout, 1);
    }
    audio.channels = requestedChannels >= 1 && requestedChannels <= 2
        ? requestedChannels
        : std::clamp(inputLayout.nb_channels, 1, 2);
    audio.sampleRate = decoder.codec->sample_rate;

    AVChannelLayout outputLayout {};
    av_channel_layout_default(&outputLayout, audio.channels);
    const int configured = swr_alloc_set_opts2(
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

    int read = 0;
    while ((read = av_read_frame(decoder.format, decoder.packet)) >= 0) {
        if (decoder.packet->stream_index == stream
            && (avcodec_send_packet(decoder.codec, decoder.packet) < 0
                || !ReceiveAudio(decoder, audio))) {
            av_packet_unref(decoder.packet);
            return false;
        }
        av_packet_unref(decoder.packet);
    }
    if (read != AVERROR_EOF
        || avcodec_send_packet(decoder.codec, nullptr) < 0
        || !ReceiveAudio(decoder, audio)
        || !FlushAudio(decoder, audio)) {
        return false;
    }
    return !audio.samples.empty();
}

}
