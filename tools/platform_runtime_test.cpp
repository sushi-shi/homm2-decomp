#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>

#include <cstdio>
#include <deque>

namespace {

class TestHost final : public platform::IHost {
public:
    std::uint32_t Ticks() const override { return 0; }
    void Sleep(std::uint32_t) override {}
    void Yield() override { ++yields; }
    void RequestQuit() override {}
    void Log(platform::LogLevel, const char*) override {}
    void ShowMessage(const char*, const char*) override {}

    unsigned yields = 0;
};

class TestInput final : public platform::IInput {
public:
    bool Poll(platform::Event& event) override {
        if (events.empty()) {
            return false;
        }
        event = events.front();
        events.pop_front();
        return true;
    }
    bool IsKeyDown(platform::Key) const override { return false; }
    bool IsMouseButtonDown(platform::MouseButton) const override { return false; }
    platform::Point MousePosition() const override { return {}; }
    void WarpMouse(platform::Point) override {}
    void StartTextInput() override {}
    void StopTextInput() override {}

    std::deque<platform::Event> events;
};

class TestAudio final : public platform::IAudio {
public:
    bool Open() override { return true; }
    void Close() override {}
    platform::VoiceId PlaySound(const platform::SoundData&, int, int) override { return 0; }
    void StopVoice(platform::VoiceId) override {}
    bool IsVoicePlaying(platform::VoiceId) const override { return false; }
    void SetVoiceVolume(platform::VoiceId, int) override {}
    bool PlayMusic(const platform::SoundData&, int) override { return false; }
    bool PlayMusicTrack(int, int) override { return false; }
    void StopMusic() override {}
    bool IsMusicPlaying() const override { return false; }
    void SetMusicVolume(int) override {}
    void Service() override { ++services; }

    unsigned services = 0;
};

TestHost gHost;
TestInput gInput;
TestAudio gAudio;
platform::Backend gBackend {
    nullptr,
    &gInput,
    &gAudio,
    &gHost,
    nullptr,
};
unsigned gDelivered = 0;

void CollectEvent(const platform::Event&) {
    ++gDelivered;
}

}

namespace platform {

Backend& Get() {
    return gBackend;
}

}

int main() {
    platform::Event event;
    event.type = platform::Event::Type::MouseMove;
    gInput.events.push_back(event);
    platform::SetEventHandler(CollectEvent);
    platform::PumpEvents();

    const bool valid = gHost.yields == 1 && gAudio.services == 1 && gDelivered == 1;
    if (!valid) {
        std::fprintf(
            stderr,
            "platform runtime mismatch: yields=%u services=%u events=%u\n",
            gHost.yields,
            gAudio.services,
            gDelivered
        );
    }
    return valid ? 0 : 1;
}
