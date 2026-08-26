#include "Sdl3Internal.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <PLATFORM/Platform.h>

namespace platform::sdl3 {
namespace {

bool ValidSound(const SoundData& sound, int loops) {
    if (sound.samples == nullptr || sound.byteCount == 0 || sound.sampleRate <= 0
        || sound.channels < 1 || sound.channels > 2
        || (sound.bitsPerSample != 8 && sound.bitsPerSample != 16)
        || sound.byteCount > static_cast<std::size_t>(std::numeric_limits<int>::max())
        || loops < -1 || loops == std::numeric_limits<int>::max()) {
        return false;
    }
    const std::size_t frameBytes = static_cast<std::size_t>(sound.channels)
        * static_cast<std::size_t>(sound.bitsPerSample / 8);
    return sound.byteCount % frameBytes == 0;
}

class Audio final : public IAudio {
public:
    ~Audio() override { Close(); }

    bool Open() override {
        if (m_device != 0) {
            return true;
        }
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
            std::fprintf(stderr, "[homm2] SDL_InitSubSystem(audio): %s\n", SDL_GetError());
            return false;
        }
        m_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (m_device == 0) {
            std::fprintf(stderr, "[homm2] SDL_OpenAudioDevice: %s\n", SDL_GetError());
            return false;
        }
        return true;
    }

    void Close() override {
        StopMusic();
        for (auto& [id, voice] : m_voices) {
            static_cast<void>(id);
            SDL_DestroyAudioStream(voice.stream);
        }
        m_voices.clear();
        if (m_device != 0) {
            SDL_CloseAudioDevice(m_device);
            m_device = 0;
        }
    }

    VoiceId PlaySound(const SoundData& sound, int volume, int loops) override {
        if (!ValidSound(sound, loops) || !Open()) {
            return 0;
        }

        Voice voice;
        if (!PrepareVoice(voice, sound, volume)) {
            return 0;
        }
        voice.infinite = loops < 0;
        const int copies = voice.infinite ? 2 : loops + 1;
        for (int copy = 0; copy < copies; ++copy) {
            if (!Queue(voice)) {
                SDL_DestroyAudioStream(voice.stream);
                return 0;
            }
        }
        if (!voice.infinite && !SDL_FlushAudioStream(voice.stream)) {
            SDL_DestroyAudioStream(voice.stream);
            return 0;
        }

        VoiceId id = NextVoiceId();
        if (id == 0) {
            SDL_DestroyAudioStream(voice.stream);
            return 0;
        }
        m_voices.emplace(id, std::move(voice));
        return id;
    }

    void StopVoice(VoiceId id) override {
        const auto found = m_voices.find(id);
        if (found == m_voices.end()) {
            return;
        }
        SDL_DestroyAudioStream(found->second.stream);
        m_voices.erase(found);
    }

    bool IsVoicePlaying(VoiceId id) const override {
        const auto found = m_voices.find(id);
        if (found == m_voices.end()) {
            return false;
        }
        const Voice& voice = found->second;
        return voice.infinite || SDL_GetAudioStreamQueued(voice.stream) > 0
            || SDL_GetAudioStreamAvailable(voice.stream) > 0;
    }

    void SetVoiceVolume(VoiceId id, int volume) override {
        const auto found = m_voices.find(id);
        if (found != m_voices.end()) {
            SDL_SetAudioStreamGain(found->second.stream, Gain(volume));
        }
    }

    bool PlayMusicTrack(int track, int loops) override {
        if (track < 0) {
            return false;
        }

        DecodedAudio decoded;
        char relativePath[64];
        std::string path;
        const char* formats[] = {
            "MUSIC/%02d-AudioTrack %02d.ogg",
            "MUSIC/Track%02d.ogg",
        };
        for (const char* format : formats) {
            SDL_snprintf(relativePath, sizeof(relativePath), format, track, track);
            path = Files().Resolve(relativePath, FileMode::Read);
            if (DecodeAudioFile(path, 2, decoded)) {
                break;
            }
        }
        if (decoded.samples.empty()) {
            std::fprintf(stderr, "[homm2] unable to decode music: %s\n", path.c_str());
            return false;
        }

        SoundData sound;
        sound.samples = decoded.samples.data();
        sound.byteCount = decoded.samples.size();
        sound.sampleRate = decoded.sampleRate;
        sound.channels = decoded.channels;
        sound.bitsPerSample = 16;
        return PlayMusic(sound, loops);
    }

    bool PlayMusic(const SoundData& sound, int loops) override {
        StopMusic();
        if (!ValidSound(sound, loops) || !Open()
            || !PrepareVoice(m_music, sound, m_musicVolume)) {
            return false;
        }

        m_music.infinite = loops < 0;
        const int copies = m_music.infinite ? 2 : loops + 1;
        for (int copy = 0; copy < copies; ++copy) {
            if (!Queue(m_music)) {
                StopMusic();
                return false;
            }
        }
        if (!m_music.infinite && !SDL_FlushAudioStream(m_music.stream)) {
            StopMusic();
            return false;
        }
        return true;
    }

    void StopMusic() override {
        if (m_music.stream != nullptr) {
            SDL_DestroyAudioStream(m_music.stream);
            m_music = {};
        }
    }

    bool IsMusicPlaying() const override {
        return m_music.stream != nullptr
            && (m_music.infinite || SDL_GetAudioStreamQueued(m_music.stream) > 0
                || SDL_GetAudioStreamAvailable(m_music.stream) > 0);
    }

    void SetMusicVolume(int volume) override {
        m_musicVolume = std::clamp(volume, 0, 127);
        if (m_music.stream != nullptr) {
            SDL_SetAudioStreamGain(m_music.stream, Gain(m_musicVolume));
        }
    }

    void Service() override {
        for (auto iterator = m_voices.begin(); iterator != m_voices.end();) {
            Voice& voice = iterator->second;
            if (voice.infinite) {
                Refill(voice);
                ++iterator;
            } else if (SDL_GetAudioStreamQueued(voice.stream) == 0
                       && SDL_GetAudioStreamAvailable(voice.stream) == 0) {
                SDL_DestroyAudioStream(voice.stream);
                iterator = m_voices.erase(iterator);
            } else {
                ++iterator;
            }
        }
        if (m_music.stream != nullptr && m_music.infinite) {
            Refill(m_music);
        }
    }

private:
    struct Voice {
        SDL_AudioStream* stream = nullptr;
        std::vector<std::uint8_t> samples;
        bool infinite = false;
    };

    static float Gain(int volume) {
        return static_cast<float>(std::clamp(volume, 0, 127)) / 127.0f;
    }

    bool PrepareVoice(Voice& voice, const SoundData& sound, int volume) const {
        SDL_AudioSpec source {};
        source.format = sound.bitsPerSample == 16 ? SDL_AUDIO_S16 : SDL_AUDIO_U8;
        source.channels = sound.channels;
        source.freq = sound.sampleRate;

        SDL_AudioSpec destination {};
        if (!SDL_GetAudioDeviceFormat(m_device, &destination, nullptr)) {
            std::fprintf(stderr, "[homm2] SDL_GetAudioDeviceFormat: %s\n", SDL_GetError());
            return false;
        }

        voice.stream = SDL_CreateAudioStream(&source, &destination);
        if (voice.stream == nullptr) {
            std::fprintf(stderr, "[homm2] SDL_CreateAudioStream: %s\n", SDL_GetError());
            return false;
        }
        if (!SDL_BindAudioStream(m_device, voice.stream)) {
            std::fprintf(stderr, "[homm2] SDL_BindAudioStream: %s\n", SDL_GetError());
            SDL_DestroyAudioStream(voice.stream);
            voice.stream = nullptr;
            return false;
        }

        const auto* first = static_cast<const std::uint8_t*>(sound.samples);
        voice.samples.assign(first, first + sound.byteCount);
        if (!SDL_SetAudioStreamGain(voice.stream, Gain(volume))) {
            std::fprintf(stderr, "[homm2] SDL_SetAudioStreamGain: %s\n", SDL_GetError());
            SDL_DestroyAudioStream(voice.stream);
            voice = {};
            return false;
        }
        return true;
    }

    static bool Queue(Voice& voice) {
        return SDL_PutAudioStreamData(
            voice.stream,
            voice.samples.data(),
            static_cast<int>(voice.samples.size())
        );
    }

    static void Refill(Voice& voice) {
        const std::int64_t queued = SDL_GetAudioStreamQueued(voice.stream);
        const std::int64_t target = static_cast<std::int64_t>(voice.samples.size()) * 2;
        if (queued >= 0 && queued < target && !Queue(voice)) {
            std::fprintf(stderr, "[homm2] SDL_PutAudioStreamData: %s\n", SDL_GetError());
        }
    }

    VoiceId NextVoiceId() {
        for (std::uint64_t attempt = 0;
             attempt <= static_cast<std::uint64_t>(std::numeric_limits<VoiceId>::max());
             ++attempt) {
            const VoiceId id = m_nextVoice++;
            if (id != 0 && !m_voices.contains(id)) {
                return id;
            }
        }
        return 0;
    }

    SDL_AudioDeviceID m_device = 0;
    VoiceId m_nextVoice = 1;
    std::map<VoiceId, Voice> m_voices;
    Voice m_music;
    int m_musicVolume = 127;
};

}

std::unique_ptr<IAudio> CreateAudio() {
    return std::make_unique<Audio>();
}

}
