#include <BASE/Midi.h>
#include <BASE/soundManager.h>
#include <PLATFORM/Platform.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

#include <cstdio>
#include <vector>

namespace {

class Audio final : public platform::IAudio {
public:
    struct Request { int track; int loops; };
    std::vector<Request> requests;
    bool playing = false;

    bool Open() override { return true; }
    void Close() override { playing = false; }
    platform::VoiceId PlaySound(const platform::SoundData&, int, int) override { return 0; }
    void StopVoice(platform::VoiceId) override {}
    bool IsVoicePlaying(platform::VoiceId) const override { return false; }
    void SetVoiceVolume(platform::VoiceId, int) override {}
    bool PlayMusic(const platform::SoundData&, int) override { return false; }
    bool PlayMusicTrack(int track, int loops) override {
        requests.push_back({track, loops});
        playing = track != 59; // An absent file must leave playback stopped.
        return playing;
    }
    void StopMusic() override { playing = false; }
    bool IsMusicPlaying() const override { return playing; }
    void SetMusicVolume(int) override {}
};

class Input final : public platform::IInput {
public:
    bool Poll(platform::Event&) override { return false; }
    bool IsKeyDown(platform::Key) const override { return false; }
    bool IsMouseButtonDown(platform::MouseButton) const override { return false; }
    platform::Point MousePosition() const override { return {}; }
    void WarpMouse(platform::Point) override {}
    void StartTextInput() override {}
    void StopTextInput() override {}
};

Audio audio;
Input input;

}

configStruct gConfig{};
soundManager* gpSoundManager = nullptr;
u8 bSaveMusicPosition[KB_MUSIC_TRACK_COUNT]{};
u8 bMusicIsLooping[KB_MUSIC_TRACK_COUNT]{};
void WritePrefs() {}

namespace platform {
Backend& Get() {
    static Backend backend{nullptr, &input, &audio, nullptr, nullptr};
    return backend;
}
}

int main() {
    soundManager manager;
    gpSoundManager = &manager;
    gConfig.musicVolume = CONFIG_VOLUME_MIN;
    for (ConfigMusicSource source : {CONFIG_MUSIC_SOURCE_CD, CONFIG_MUSIC_SOURCE_MIDI}) {
        gConfig.musicSource = source;
        manager.Open(0);
        for (int track : {16, 29, 30, 2}) {
            const std::size_t before = audio.requests.size();
            manager.PlayAmbientMusic(track);
            if (!audio.playing || audio.requests.size() != before + 1
                || audio.requests.back().track != track || manager.m_musicTrack != track
                || audio.requests.back().loops != ((track == 16 || track == 2) ? -1 : 0)) {
                std::fprintf(stderr, "numbered track %d did not reach the audio backend\n", track);
                return 1;
            }
            manager.PlayAmbientMusic(track);
            if (audio.requests.size() != before + 1) return 2;
        }
        manager.PlayAmbientMusic(59);
        if (audio.playing || manager.m_musicTrack != MIDI_NO_TRACK) return 3;
        const std::size_t beforeInvalid = audio.requests.size();
        manager.PlayAmbientMusic(MIDI_TRACK_COUNT);
        manager.PlayAmbientMusic(-2);
        if (audio.requests.size() != beforeInvalid || audio.playing) return 4;
        manager.Close();
    }
    return 0;
}
