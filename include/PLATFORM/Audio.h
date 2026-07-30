#ifndef HOMM2_PLATFORM_AUDIO_H
#define HOMM2_PLATFORM_AUDIO_H

#include <cstddef>

#include "Types.h"

namespace platform {

struct SoundData {
    const void* samples = nullptr;
    std::size_t byteCount = 0;
    int sampleRate = 22050;
    int channels = 1;
    int bitsPerSample = 8;
};

using VoiceId = std::uint32_t;

class IAudio {
public:
    virtual ~IAudio() = default;

    virtual bool Open() = 0;
    virtual void Close() = 0;

    virtual VoiceId PlaySound(const SoundData& sound, int volume, int loops) = 0;
    virtual void StopVoice(VoiceId voice) = 0;
    virtual bool IsVoicePlaying(VoiceId voice) const = 0;
    virtual void SetVoiceVolume(VoiceId voice, int volume) = 0;

    virtual bool PlayMusic(const void* data, std::size_t byteCount, int loops) = 0;
    virtual void StopMusic() = 0;
    virtual bool IsMusicPlaying() const = 0;
    virtual void SetMusicVolume(int volume) = 0;

    virtual void Service() {}
};

}

#endif
