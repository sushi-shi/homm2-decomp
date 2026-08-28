#include "Sdl3Internal.h"

#include "Sdl3Video.h"

#include <SDL3/SDL.h>

#include <PLATFORM/InputReplay.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <cmath>
#include <cstdio>
#include <deque>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <PLATFORM/Runtime.h>

namespace platform::sdl3 {
namespace {

Key TranslateKey(SDL_Scancode code) {
    switch (code) {
    case SDL_SCANCODE_ESCAPE: return Key::Escape;
    case SDL_SCANCODE_RETURN:
    case SDL_SCANCODE_KP_ENTER: return Key::Return;
    case SDL_SCANCODE_SPACE: return Key::Space;
    case SDL_SCANCODE_TAB: return Key::Tab;
    case SDL_SCANCODE_BACKSPACE: return Key::Backspace;
    case SDL_SCANCODE_DELETE: return Key::Delete;
    case SDL_SCANCODE_LEFT: return Key::Left;
    case SDL_SCANCODE_RIGHT: return Key::Right;
    case SDL_SCANCODE_UP: return Key::Up;
    case SDL_SCANCODE_DOWN: return Key::Down;
    case SDL_SCANCODE_HOME: return Key::Home;
    case SDL_SCANCODE_END: return Key::End;
    case SDL_SCANCODE_PAGEUP: return Key::PageUp;
    case SDL_SCANCODE_PAGEDOWN: return Key::PageDown;
    case SDL_SCANCODE_INSERT: return Key::Insert;
    case SDL_SCANCODE_LSHIFT:
    case SDL_SCANCODE_RSHIFT: return Key::Shift;
    case SDL_SCANCODE_LCTRL:
    case SDL_SCANCODE_RCTRL: return Key::Control;
    case SDL_SCANCODE_LALT:
    case SDL_SCANCODE_RALT: return Key::Alt;
    case SDL_SCANCODE_F1: return Key::F1;
    case SDL_SCANCODE_F2: return Key::F2;
    case SDL_SCANCODE_F3: return Key::F3;
    case SDL_SCANCODE_F4: return Key::F4;
    case SDL_SCANCODE_F5: return Key::F5;
    case SDL_SCANCODE_F6: return Key::F6;
    case SDL_SCANCODE_F7: return Key::F7;
    case SDL_SCANCODE_F8: return Key::F8;
    case SDL_SCANCODE_F9: return Key::F9;
    case SDL_SCANCODE_F10: return Key::F10;
    case SDL_SCANCODE_F11: return Key::F11;
    case SDL_SCANCODE_F12: return Key::F12;
    default: break;
    }
    if (code >= SDL_SCANCODE_A && code <= SDL_SCANCODE_Z) {
        return KeyFromOrdinal(static_cast<int>(Key::A) + (code - SDL_SCANCODE_A));
    }
    if (code >= SDL_SCANCODE_1 && code <= SDL_SCANCODE_9) {
        return KeyFromOrdinal(static_cast<int>(Key::Digit1) + (code - SDL_SCANCODE_1));
    }
    if (code == SDL_SCANCODE_0) {
        return Key::Digit0;
    }
    return Key::Unknown;
}

unsigned ToSetOneScanCode(SDL_Scancode code) {
    switch (code) {
    case SDL_SCANCODE_ESCAPE: return 0x01;
    case SDL_SCANCODE_1: return 0x02;
    case SDL_SCANCODE_2: return 0x03;
    case SDL_SCANCODE_3: return 0x04;
    case SDL_SCANCODE_4: return 0x05;
    case SDL_SCANCODE_5: return 0x06;
    case SDL_SCANCODE_6: return 0x07;
    case SDL_SCANCODE_7: return 0x08;
    case SDL_SCANCODE_8: return 0x09;
    case SDL_SCANCODE_9: return 0x0a;
    case SDL_SCANCODE_0: return 0x0b;
    case SDL_SCANCODE_MINUS: return 0x0c;
    case SDL_SCANCODE_EQUALS: return 0x0d;
    case SDL_SCANCODE_BACKSPACE: return 0x0e;
    case SDL_SCANCODE_TAB: return 0x0f;
    case SDL_SCANCODE_Q: return 0x10;
    case SDL_SCANCODE_W: return 0x11;
    case SDL_SCANCODE_E: return 0x12;
    case SDL_SCANCODE_R: return 0x13;
    case SDL_SCANCODE_T: return 0x14;
    case SDL_SCANCODE_Y: return 0x15;
    case SDL_SCANCODE_U: return 0x16;
    case SDL_SCANCODE_I: return 0x17;
    case SDL_SCANCODE_O: return 0x18;
    case SDL_SCANCODE_P: return 0x19;
    case SDL_SCANCODE_LEFTBRACKET: return 0x1a;
    case SDL_SCANCODE_RIGHTBRACKET: return 0x1b;
    case SDL_SCANCODE_RETURN:
    case SDL_SCANCODE_KP_ENTER: return 0x1c;
    case SDL_SCANCODE_LCTRL:
    case SDL_SCANCODE_RCTRL: return 0x1d;
    case SDL_SCANCODE_A: return 0x1e;
    case SDL_SCANCODE_S: return 0x1f;
    case SDL_SCANCODE_D: return 0x20;
    case SDL_SCANCODE_F: return 0x21;
    case SDL_SCANCODE_G: return 0x22;
    case SDL_SCANCODE_H: return 0x23;
    case SDL_SCANCODE_J: return 0x24;
    case SDL_SCANCODE_K: return 0x25;
    case SDL_SCANCODE_L: return 0x26;
    case SDL_SCANCODE_SEMICOLON: return 0x27;
    case SDL_SCANCODE_APOSTROPHE: return 0x28;
    case SDL_SCANCODE_GRAVE: return 0x29;
    case SDL_SCANCODE_LSHIFT: return 0x2a;
    case SDL_SCANCODE_BACKSLASH: return 0x2b;
    case SDL_SCANCODE_Z: return 0x2c;
    case SDL_SCANCODE_X: return 0x2d;
    case SDL_SCANCODE_C: return 0x2e;
    case SDL_SCANCODE_V: return 0x2f;
    case SDL_SCANCODE_B: return 0x30;
    case SDL_SCANCODE_N: return 0x31;
    case SDL_SCANCODE_M: return 0x32;
    case SDL_SCANCODE_COMMA: return 0x33;
    case SDL_SCANCODE_PERIOD: return 0x34;
    case SDL_SCANCODE_SLASH: return 0x35;
    case SDL_SCANCODE_RSHIFT: return 0x36;
    case SDL_SCANCODE_LALT:
    case SDL_SCANCODE_RALT: return 0x38;
    case SDL_SCANCODE_SPACE: return 0x39;
    case SDL_SCANCODE_CAPSLOCK: return 0x3a;
    case SDL_SCANCODE_F1: return 0x3b;
    case SDL_SCANCODE_F2: return 0x3c;
    case SDL_SCANCODE_F3: return 0x3d;
    case SDL_SCANCODE_F4: return 0x3e;
    case SDL_SCANCODE_F5: return 0x3f;
    case SDL_SCANCODE_F6: return 0x40;
    case SDL_SCANCODE_F7: return 0x41;
    case SDL_SCANCODE_F8: return 0x42;
    case SDL_SCANCODE_F9: return 0x43;
    case SDL_SCANCODE_F10: return 0x44;
    case SDL_SCANCODE_HOME: return 0x47;
    case SDL_SCANCODE_UP: return 0x48;
    case SDL_SCANCODE_PAGEUP: return 0x49;
    case SDL_SCANCODE_LEFT: return 0x4b;
    case SDL_SCANCODE_RIGHT: return 0x4d;
    case SDL_SCANCODE_END: return 0x4f;
    case SDL_SCANCODE_DOWN: return 0x50;
    case SDL_SCANCODE_PAGEDOWN: return 0x51;
    case SDL_SCANCODE_INSERT: return 0x52;
    case SDL_SCANCODE_DELETE: return 0x53;
    case SDL_SCANCODE_F11: return 0x57;
    case SDL_SCANCODE_F12: return 0x58;
    default: return 0;
    }
}

bool ResolveReplayKey(
    std::string_view name,
    Key& key,
    unsigned& scanCode,
    unsigned& physicalCode,
    unsigned& character
) {
    const std::string ownedName(name);
    const SDL_Scancode code = SDL_GetScancodeFromName(ownedName.c_str());
    key = TranslateKey(code);
    scanCode = ToSetOneScanCode(code);
    physicalCode = static_cast<unsigned>(code);
    character = name.size() == 1 ? static_cast<unsigned char>(name.front()) : 0;
    return code != SDL_SCANCODE_UNKNOWN && key != Key::Unknown;
}

unsigned TranslateModifiers(SDL_Keymod modifiers) {
    unsigned result = ModNone;
    if ((modifiers & SDL_KMOD_SHIFT) != 0) result |= ModShift;
    if ((modifiers & SDL_KMOD_CTRL) != 0) result |= ModControl;
    if ((modifiers & SDL_KMOD_ALT) != 0) result |= ModAlt;
    return result;
}

SDL_Scancode LogicalScanCode(const SDL_KeyboardEvent& event) {
    switch (event.key) {
    case SDLK_ESCAPE: return SDL_SCANCODE_ESCAPE;
    case SDLK_RETURN: return SDL_SCANCODE_RETURN;
    case SDLK_SPACE: return SDL_SCANCODE_SPACE;
    case SDLK_TAB: return SDL_SCANCODE_TAB;
    case SDLK_BACKSPACE: return SDL_SCANCODE_BACKSPACE;
    case SDLK_DELETE: return SDL_SCANCODE_DELETE;
    case SDLK_MINUS: return SDL_SCANCODE_MINUS;
    case SDLK_EQUALS: return SDL_SCANCODE_EQUALS;
    case SDLK_LEFTBRACKET: return SDL_SCANCODE_LEFTBRACKET;
    case SDLK_RIGHTBRACKET: return SDL_SCANCODE_RIGHTBRACKET;
    case SDLK_SEMICOLON: return SDL_SCANCODE_SEMICOLON;
    case SDLK_APOSTROPHE: return SDL_SCANCODE_APOSTROPHE;
    case SDLK_GRAVE: return SDL_SCANCODE_GRAVE;
    case SDLK_BACKSLASH: return SDL_SCANCODE_BACKSLASH;
    case SDLK_COMMA: return SDL_SCANCODE_COMMA;
    case SDLK_PERIOD: return SDL_SCANCODE_PERIOD;
    case SDLK_SLASH: return SDL_SCANCODE_SLASH;
    default: break;
    }
    if (event.key >= SDLK_A && event.key <= SDLK_Z) {
        return static_cast<SDL_Scancode>(SDL_SCANCODE_A + (event.key - SDLK_A));
    }
    if (event.key >= SDLK_1 && event.key <= SDLK_9) {
        return static_cast<SDL_Scancode>(SDL_SCANCODE_1 + (event.key - SDLK_1));
    }
    if (event.key == SDLK_0) {
        return SDL_SCANCODE_0;
    }
    if ((event.key & SDLK_SCANCODE_MASK) != 0) {
        return static_cast<SDL_Scancode>(
            event.key & ~(SDLK_SCANCODE_MASK | SDLK_EXTENDED_MASK)
        );
    }
    return event.scancode;
}

class Input final : public IInput {
public:
    explicit Input(Video& video) : m_video(video) {}
    ~Input() override { ResetTextInput(); }

    bool Poll(Event& event) override {
        if (m_queue.empty()) {
            return false;
        }
        event = std::move(m_queue.front());
        m_queue.pop_front();
        return true;
    }

    bool IsKeyDown(Key key) const override {
        const auto found = m_keyCounts.find(static_cast<int>(key));
        return found != m_keyCounts.end() && found->second != 0;
    }

    bool IsMouseButtonDown(MouseButton button) const override {
        return (m_buttons & (1u << static_cast<unsigned>(button))) != 0;
    }

    Point MousePosition() const override { return m_mouse; }

    void WarpMouse(Point position) override {
        m_mouse = position;
        if (m_video.Window() == nullptr) {
            return;
        }
        float windowX = static_cast<float>(position.x);
        float windowY = static_cast<float>(position.y);
        if (m_video.Renderer() != nullptr
            && !SDL_RenderCoordinatesToWindow(
                m_video.Renderer(), windowX, windowY, &windowX, &windowY
            )) {
            std::fprintf(stderr, "[homm2] SDL_RenderCoordinatesToWindow: %s\n", SDL_GetError());
            return;
        }
        SDL_WarpMouseInWindow(m_video.Window(), windowX, windowY);
    }

    void StartTextInput() override {
        if (m_video.Window() == nullptr) {
            return;
        }

        const bool activate = m_textInputDepth.Acquire();
        if (!activate) {
            return;
        }
        if (!SDL_StartTextInput(m_video.Window())) {
            m_textInputDepth.Release();
            std::fprintf(stderr, "[homm2] SDL_StartTextInput: %s\n", SDL_GetError());
        }
    }

    void StopTextInput() override {
        if (m_textInputDepth.Release() && m_video.Window() != nullptr) {
            SDL_StopTextInput(m_video.Window());
        }
    }

    void Push(Event event) {
        UpdateState(event);
        m_queue.push_back(std::move(event));
    }

    void ReleaseAll() {
        std::vector<Event> releases;
        releases.reserve(m_pressedKeys.size() + 2);
        for (const auto& [scanCode, pressed] : m_pressedKeys) {
            Event release = pressed;
            release.type = Event::Type::KeyUp;
            release.character = 0;
            releases.push_back(std::move(release));
        }
        if (IsMouseButtonDown(MouseButton::Left)) {
            Event release;
            release.type = Event::Type::MouseUp;
            release.button = MouseButton::Left;
            release.position = m_mouse;
            releases.push_back(release);
        }
        if (IsMouseButtonDown(MouseButton::Right)) {
            Event release;
            release.type = Event::Type::MouseUp;
            release.button = MouseButton::Right;
            release.position = m_mouse;
            releases.push_back(release);
        }
        for (Event& release : releases) {
            Push(std::move(release));
        }
    }

private:
    void ResetTextInput() {
        if (m_textInputDepth.Reset() && m_video.Window() != nullptr) {
            SDL_StopTextInput(m_video.Window());
        }
    }

    void UpdateState(const Event& event) {
        if (event.type == Event::Type::MouseMove) {
            m_mouse = event.position;
            return;
        }
        if (event.type == Event::Type::MouseDown || event.type == Event::Type::MouseUp) {
            m_mouse = event.position;
            const unsigned bit = 1u << static_cast<unsigned>(event.button);
            m_buttons = event.type == Event::Type::MouseDown ? (m_buttons | bit)
                                                              : (m_buttons & ~bit);
            return;
        }
        if (event.type == Event::Type::KeyDown && event.scanCode != 0) {
            const unsigned identity = event.physicalCode != 0 ? event.physicalCode
                                                               : event.scanCode;
            const auto [iterator, inserted] = m_pressedKeys.emplace(identity, event);
            if (inserted) {
                ++m_keyCounts[static_cast<int>(event.key)];
            }
            return;
        }
        if (event.type == Event::Type::KeyUp && event.scanCode != 0) {
            const unsigned identity = event.physicalCode != 0 ? event.physicalCode
                                                               : event.scanCode;
            const auto pressed = m_pressedKeys.find(identity);
            if (pressed == m_pressedKeys.end()) {
                return;
            }
            const int key = static_cast<int>(pressed->second.key);
            const auto count = m_keyCounts.find(key);
            if (count != m_keyCounts.end() && --count->second == 0) {
                m_keyCounts.erase(count);
            }
            m_pressedKeys.erase(pressed);
        }
    }

    Video& m_video;
    std::deque<Event> m_queue;
    std::map<unsigned, Event> m_pressedKeys;
    std::map<int, unsigned> m_keyCounts;
    unsigned m_buttons = 0;
    Point m_mouse;
    TextInputDepth m_textInputDepth;
};

class Host final : public IHost {
public:
    Host(Video& video, Input& input) : m_video(video), m_input(input) { LoadReplay(); }

    std::uint32_t Ticks() const override {
        return static_cast<std::uint32_t>(SDL_GetTicks());
    }

    void Sleep(std::uint32_t milliseconds) override {
#ifdef __EMSCRIPTEN__
        Pump();
        emscripten_sleep(milliseconds);
#else
        const std::uint32_t deadline = Ticks() + milliseconds;
        do {
            Yield();
            SDL_Delay(1);
        } while (TickDeadlinePending(deadline, Ticks()));
#endif
    }

    void Yield() override {
        Pump();
#ifdef __EMSCRIPTEN__
        emscripten_sleep(0);
#endif
    }

    void RequestQuit() override {
        if (m_quitQueued) {
            return;
        }
        m_quitQueued = true;
        Event event;
        event.type = Event::Type::Quit;
        m_input.Push(std::move(event));
    }

    void Log(LogLevel level, const char* message) override {
        static const char* const names[] = {"debug", "info", "warning", "error"};
        const int index = static_cast<int>(level);
        const char* name = index >= 0 && index < 4 ? names[index] : "unknown";
        std::fprintf(stderr, "[homm2] %s: %s\n", name, message != nullptr ? message : "");
    }

    void ShowMessage(const char* title, const char* message) override {
        const char* safeTitle = title != nullptr ? title : "Heroes II";
        const char* safeMessage = message != nullptr ? message : "";
        if (!SDL_ShowSimpleMessageBox(
                SDL_MESSAGEBOX_ERROR,
                safeTitle,
                safeMessage,
                m_video.Window()
            )) {
            std::fprintf(
                stderr,
                "[homm2] %s: %s (message box unavailable: %s)\n",
                safeTitle,
                safeMessage,
                SDL_GetError()
            );
        }
    }

private:
    void LoadReplay() {
        const char* path = SDL_getenv("HOMM2_INPUT_REPLAY");
        if (path == nullptr) {
            return;
        }
        std::ifstream stream(path);
        if (!stream) {
            std::fprintf(stderr, "[homm2] input replay: cannot open %s\n", path);
            return;
        }
        ReplayParseError error;
        if (!m_replay.Load(stream, ResolveReplayKey, error)) {
            std::fprintf(
                stderr,
                "[homm2] input replay: %s:%zu: %s\n",
                path,
                error.line,
                error.message.c_str()
            );
            return;
        }
        m_replay.Start(Ticks());
    }

    void PumpReplay() {
        Event event;
        const std::uint32_t now = Ticks();
        while (m_replay.NextDue(now, event)) {
            m_input.Push(std::move(event));
        }
    }

    void Pump() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            Translate(event);
        }
        PumpReplay();
    }

    void Translate(const SDL_Event& sdlEvent) {
        Event event;
        switch (sdlEvent.type) {
        case SDL_EVENT_QUIT:
            RequestQuit();
            return;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            const bool down = sdlEvent.type == SDL_EVENT_KEY_DOWN;
            const SDL_Scancode code = LogicalScanCode(sdlEvent.key);
            event.type = down ? Event::Type::KeyDown : Event::Type::KeyUp;
            event.key = TranslateKey(code);
            event.scanCode = ToSetOneScanCode(code);
            event.physicalCode = static_cast<unsigned>(sdlEvent.key.scancode);
            event.modifiers = TranslateModifiers(sdlEvent.key.mod);
            const SDL_Keycode key = sdlEvent.key.key;
            event.character = key > 0 && key < 128 ? static_cast<unsigned>(key) : 0;
            m_input.Push(std::move(event));
            return;
        }

        case SDL_EVENT_RENDER_TARGETS_RESET:
        case SDL_EVENT_RENDER_DEVICE_RESET:
        case SDL_EVENT_RENDER_DEVICE_LOST:
            if (!m_video.HandleRenderEvent(sdlEvent.type, sdlEvent.render.windowID)) {
                RequestQuit();
            }
            return;

        case SDL_EVENT_TEXT_INPUT:
            event.type = Event::Type::TextInput;
            event.text = sdlEvent.text.text != nullptr ? sdlEvent.text.text : "";
            if (!event.text.empty()) {
                m_input.Push(std::move(event));
            }
            return;

        case SDL_EVENT_MOUSE_MOTION:
            event.type = Event::Type::MouseMove;
            event.position = ToFramebuffer(sdlEvent.motion.x, sdlEvent.motion.y);
            m_input.Push(std::move(event));
            return;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                event.button = MouseButton::Left;
            } else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                event.button = MouseButton::Right;
            } else {
                return;
            }
            event.type = sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN
                ? Event::Type::MouseDown
                : Event::Type::MouseUp;
            event.position = ToFramebuffer(sdlEvent.button.x, sdlEvent.button.y);
            m_input.Push(std::move(event));
            return;

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            if (sdlEvent.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
                m_input.ReleaseAll();
            }
            event.type = Event::Type::FocusChanged;
            event.focused = sdlEvent.type == SDL_EVENT_WINDOW_FOCUS_GAINED;
            m_input.Push(std::move(event));
            return;

        default:
            return;
        }
    }

    Point ToFramebuffer(float x, float y) const {
        float logicalX = x;
        float logicalY = y;
        if (m_video.Renderer() != nullptr
            && !SDL_RenderCoordinatesFromWindow(
                m_video.Renderer(), x, y, &logicalX, &logicalY
            )) {
            std::fprintf(
                stderr,
                "[homm2] SDL_RenderCoordinatesFromWindow: %s\n",
                SDL_GetError()
            );
        }
        return {
            static_cast<int>(std::floor(logicalX)),
            static_cast<int>(std::floor(logicalY)),
        };
    }

    Video& m_video;
    Input& m_input;
    bool m_quitQueued = false;
    InputReplay m_replay;
};

}

InputHostServices CreateInputHost(Video& video) {
    InputHostServices services;
    auto input = std::make_unique<Input>(video);
    services.host = std::make_unique<Host>(video, *input);
    services.input = std::move(input);
    return services;
}

}
