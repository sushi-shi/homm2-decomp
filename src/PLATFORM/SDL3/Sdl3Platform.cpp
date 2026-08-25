#include <SDL3/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}


namespace platform {
namespace {

class Sdl3Video final : public IVideo {
public:
    bool Open(const DisplayMode& mode) override {
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
            std::fprintf(stderr, "[homm2] SDL_InitSubSystem(video): %s\n", SDL_GetError());
            return false;
        }

        m_size = {mode.width, mode.height};
        const int scale = mode.scale > 0 ? mode.scale : 2;
        int windowWidth = mode.width * scale;
        int windowHeight = mode.height * scale;
        if (const char* configured = SDL_getenv("HOMM2_WINDOW_SIZE");
            configured != nullptr && *configured != '\0') {
            int configuredWidth;
            int configuredHeight;
            char trailing;
            if (SDL_sscanf(
                    configured,
                    "%dx%d%c",
                    &configuredWidth,
                    &configuredHeight,
                    &trailing
                ) == 2
                && configuredWidth > 0 && configuredHeight > 0) {
                windowWidth = configuredWidth;
                windowHeight = configuredHeight;
            } else {
                std::fprintf(
                    stderr,
                    "[homm2] HOMM2_WINDOW_SIZE must be '<width>x<height>'; using %dx%d\n",
                    windowWidth,
                    windowHeight
                );
            }
        }

        m_window = SDL_CreateWindow(mode.title, windowWidth, windowHeight,
                                    SDL_WINDOW_RESIZABLE);
        if (m_window == nullptr) {
            std::fprintf(stderr, "[homm2] SDL_CreateWindow: %s\n", SDL_GetError());
            return false;
        }
        if (mode.fullscreen) {
            SDL_SetWindowFullscreen(m_window, true);
        }

        m_renderer = SDL_CreateRenderer(m_window, nullptr);
        if (m_renderer == nullptr) {
            std::fprintf(stderr, "[homm2] SDL_CreateRenderer: %s\n", SDL_GetError());
            return false;
        }

        SDL_SetRenderLogicalPresentation(m_renderer, mode.width, mode.height,
                                         SDL_LOGICAL_PRESENTATION_LETTERBOX);

        m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_XRGB8888,
                                      SDL_TEXTUREACCESS_STREAMING, mode.width, mode.height);
        if (m_texture == nullptr) {
            std::fprintf(stderr, "[homm2] SDL_CreateTexture: %s\n", SDL_GetError());
            return false;
        }
        SDL_SetTextureScaleMode(m_texture, SDL_SCALEMODE_NEAREST);

        m_indexed.assign(static_cast<std::size_t>(mode.width) * mode.height, 0);
        m_presented.assign(static_cast<std::size_t>(mode.width) * mode.height, 0);
        m_expanded.assign(static_cast<std::size_t>(mode.width) * mode.height, 0);
        return true;
    }

    void Close() override {
        if (m_texture != nullptr) {
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
        }
        if (m_renderer != nullptr) {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
        if (m_window != nullptr) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
    }

    std::uint8_t* Pixels() override { return m_indexed.data(); }
    int Pitch() const override { return m_size.width; }
    Size Resolution() const override { return m_size; }

    void Blit(const Rect& source, Point destination) override {
        if (m_indexed.empty()) {
            return;
        }

        const int width = std::min({source.width, m_size.width - source.x,
                                    m_size.width - destination.x});
        const int height = std::min({source.height, m_size.height - source.y,
                                     m_size.height - destination.y});
        if (width <= 0 || height <= 0 || source.x < 0 || source.y < 0 || destination.x < 0
            || destination.y < 0) {
            return;
        }

        for (int row = 0; row < height; ++row) {
            const std::uint8_t* from =
                m_indexed.data() + static_cast<std::size_t>(source.y + row) * m_size.width + source.x;
            std::uint8_t* to = m_presented.data()
                             + static_cast<std::size_t>(destination.y + row) * m_size.width
                             + destination.x;
            std::memcpy(to, from, static_cast<std::size_t>(width));
        }
    }

    void SetPalette(const Color* entries, int first, int count) override {
        for (int i = 0; i < count; ++i) {
            const Color& color = entries[i];
            const int slot = first + i;
            if (slot < 0 || slot >= 256) {
                continue;
            }
            m_palette[slot] = (static_cast<std::uint32_t>(color.r) << 16)
                            | (static_cast<std::uint32_t>(color.g) << 8)
                            | static_cast<std::uint32_t>(color.b);
        }
        Present();
    }

    void Present() override {
        if (m_renderer == nullptr) {
            return;
        }

        const std::size_t count = m_presented.size();
        const std::uint8_t* source = m_presented.data();
        std::uint32_t* destination = m_expanded.data();
        for (std::size_t i = 0; i < count; ++i) {
            destination[i] = m_palette[source[i]];
        }

        MaybeCapture();

        SDL_UpdateTexture(m_texture, nullptr, m_expanded.data(),
                          m_size.width * static_cast<int>(sizeof(std::uint32_t)));
        SDL_RenderClear(m_renderer);
        SDL_RenderTexture(m_renderer, m_texture, nullptr, nullptr);
        SDL_RenderPresent(m_renderer);
    }

    void ShowCursor(bool visible) override {
        if (visible) {
            SDL_ShowCursor();
        } else {
            SDL_HideCursor();
        }
    }

    void MaybeCapture() {
        const char* pattern = std::getenv("HOMM2_SCREENSHOT");
        if (pattern == nullptr) {
            return;
        }
        ++m_frame;
        const char* everyText = std::getenv("HOMM2_SCREENSHOT_EVERY");
        const int every = everyText != nullptr ? std::atoi(everyText) : 0;
        if (every > 0 ? (m_frame % every) != 0 : m_frame != 1) {
            return;
        }

        char path[512];
        SDL_snprintf(path, sizeof(path), "%s.%06d.ppm", pattern, m_frame);
        SDL_IOStream* stream = SDL_IOFromFile(path, "wb");
        if (stream == nullptr) {
            return;
        }

        char header[64];
        const int headerLength =
            SDL_snprintf(header, sizeof(header), "P6\n%d %d\n255\n", m_size.width, m_size.height);
        SDL_WriteIO(stream, header, static_cast<std::size_t>(headerLength));
        for (std::uint32_t pixel : m_expanded) {
            const std::uint8_t rgb[3] = {
                static_cast<std::uint8_t>((pixel >> 16) & 0xFF),
                static_cast<std::uint8_t>((pixel >> 8) & 0xFF),
                static_cast<std::uint8_t>(pixel & 0xFF),
            };
            SDL_WriteIO(stream, rgb, sizeof(rgb));
        }
        SDL_CloseIO(stream);
    }

    SDL_Window* window() const { return m_window; }
    SDL_Renderer* renderer() const { return m_renderer; }

private:
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_texture = nullptr;
    Size m_size;
    std::vector<std::uint8_t> m_indexed;
    std::vector<std::uint8_t> m_presented;
    std::vector<std::uint32_t> m_expanded;
    std::uint32_t m_palette[256] = {};
    int m_frame = 0;
};

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
        return static_cast<Key>(static_cast<int>(Key::A) + (code - SDL_SCANCODE_A));
    }
    if (code >= SDL_SCANCODE_1 && code <= SDL_SCANCODE_9) {
        return static_cast<Key>(static_cast<int>(Key::Digit1) + (code - SDL_SCANCODE_1));
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
    case SDL_SCANCODE_9: return 0x0A;
    case SDL_SCANCODE_0: return 0x0B;
    case SDL_SCANCODE_MINUS: return 0x0C;
    case SDL_SCANCODE_EQUALS: return 0x0D;
    case SDL_SCANCODE_BACKSPACE: return 0x0E;
    case SDL_SCANCODE_TAB: return 0x0F;
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
    case SDL_SCANCODE_LEFTBRACKET: return 0x1A;
    case SDL_SCANCODE_RIGHTBRACKET: return 0x1B;
    case SDL_SCANCODE_RETURN:
    case SDL_SCANCODE_KP_ENTER: return 0x1C;
    case SDL_SCANCODE_LCTRL:
    case SDL_SCANCODE_RCTRL: return 0x1D;
    case SDL_SCANCODE_A: return 0x1E;
    case SDL_SCANCODE_S: return 0x1F;
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
    case SDL_SCANCODE_LSHIFT: return 0x2A;
    case SDL_SCANCODE_BACKSLASH: return 0x2B;
    case SDL_SCANCODE_Z: return 0x2C;
    case SDL_SCANCODE_X: return 0x2D;
    case SDL_SCANCODE_C: return 0x2E;
    case SDL_SCANCODE_V: return 0x2F;
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
    case SDL_SCANCODE_CAPSLOCK: return 0x3A;
    case SDL_SCANCODE_F1: return 0x3B;
    case SDL_SCANCODE_F2: return 0x3C;
    case SDL_SCANCODE_F3: return 0x3D;
    case SDL_SCANCODE_F4: return 0x3E;
    case SDL_SCANCODE_F5: return 0x3F;
    case SDL_SCANCODE_F6: return 0x40;
    case SDL_SCANCODE_F7: return 0x41;
    case SDL_SCANCODE_F8: return 0x42;
    case SDL_SCANCODE_F9: return 0x43;
    case SDL_SCANCODE_F10: return 0x44;
    case SDL_SCANCODE_HOME: return 0x47;
    case SDL_SCANCODE_UP: return 0x48;
    case SDL_SCANCODE_PAGEUP: return 0x49;
    case SDL_SCANCODE_LEFT: return 0x4B;
    case SDL_SCANCODE_RIGHT: return 0x4D;
    case SDL_SCANCODE_END: return 0x4F;
    case SDL_SCANCODE_DOWN: return 0x50;
    case SDL_SCANCODE_PAGEDOWN: return 0x51;
    case SDL_SCANCODE_INSERT: return 0x52;
    case SDL_SCANCODE_DELETE: return 0x53;
    case SDL_SCANCODE_F11: return 0x57;
    case SDL_SCANCODE_F12: return 0x58;
    default: return 0;
    }
}

unsigned TranslateModifiers(SDL_Keymod mod) {
    unsigned modifiers = ModNone;
    if ((mod & SDL_KMOD_SHIFT) != 0) modifiers |= ModShift;
    if ((mod & SDL_KMOD_CTRL) != 0) modifiers |= ModControl;
    if ((mod & SDL_KMOD_ALT) != 0) modifiers |= ModAlt;
    return modifiers;
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
        return static_cast<SDL_Scancode>(
            SDL_SCANCODE_A + (event.key - SDLK_A)
        );
    }
    if (event.key >= SDLK_1 && event.key <= SDLK_9) {
        return static_cast<SDL_Scancode>(
            SDL_SCANCODE_1 + (event.key - SDLK_1)
        );
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

class Sdl3Host;

class Sdl3Input final : public IInput {
public:
    explicit Sdl3Input(Sdl3Video& video) : m_video(video) {}

    bool Poll(Event& event) override {
        if (m_queue.empty()) {
            return false;
        }
        event = m_queue.front();
        m_queue.erase(m_queue.begin());
        if (event.type == Event::Type::MouseMove) {
            SetMouse(event.position);
        } else if (event.type == Event::Type::MouseDown) {
            SetMouse(event.position);
            SetButton(event.button, true);
        } else if (event.type == Event::Type::MouseUp) {
            SetMouse(event.position);
            SetButton(event.button, false);
        } else if (event.type == Event::Type::KeyDown) {
            SetKey(event.key, true);
        } else if (event.type == Event::Type::KeyUp) {
            SetKey(event.key, false);
        }
        return true;
    }

    bool IsKeyDown(Key key) const override {
        return m_keys.count(static_cast<int>(key)) != 0;
    }

    bool IsMouseButtonDown(MouseButton button) const override {
        return (m_buttons & (1u << static_cast<unsigned>(button))) != 0;
    }

    Point MousePosition() const override { return m_mouse; }

    void WarpMouse(Point position) override {
        m_mouse = position;
        if (m_video.window() != nullptr) {
            float windowX = static_cast<float>(position.x);
            float windowY = static_cast<float>(position.y);
            if (m_video.renderer() != nullptr) {
                SDL_RenderCoordinatesToWindow(
                    m_video.renderer(),
                    windowX,
                    windowY,
                    &windowX,
                    &windowY
                );
            }
            SDL_WarpMouseInWindow(m_video.window(), windowX, windowY);
        }
    }

    void StartTextInput() override {
        if (!m_textInputActive && m_video.window() != nullptr) {
            if (!SDL_StartTextInput(m_video.window())) {
                std::fprintf(stderr, "[homm2] SDL_StartTextInput: %s\n", SDL_GetError());
                return;
            }
            m_textInputActive = true;
        }
    }

    void StopTextInput() override {
        if (m_textInputActive && m_video.window() != nullptr) {
            SDL_StopTextInput(m_video.window());
            m_textInputActive = false;
        }
    }

    void Push(const Event& event) { m_queue.push_back(event); }
    void SetKey(Key key, bool down) {
        if (down) {
            m_keys.insert(static_cast<int>(key));
        } else {
            m_keys.erase(static_cast<int>(key));
        }
    }
    void SetButton(MouseButton button, bool down) {
        const unsigned bit = 1u << static_cast<unsigned>(button);
        m_buttons = down ? (m_buttons | bit) : (m_buttons & ~bit);
    }
    void SetMouse(Point position) { m_mouse = position; }

private:
    Sdl3Video& m_video;
    std::vector<Event> m_queue;
    std::set<int> m_keys;
    unsigned m_buttons = 0;
    Point m_mouse;
    bool m_textInputActive = false;
};

class Sdl3Host final : public IHost {
public:
    Sdl3Host(Sdl3Video& video, Sdl3Input& input) : m_video(video), m_input(input) {
        LoadReplay();
    }

    std::uint32_t Ticks() const override { return static_cast<std::uint32_t>(SDL_GetTicks()); }

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

    bool ShouldQuit() const override { return m_quit; }
    void RequestQuit() override { m_quit = true; }

    void Log(LogLevel level, const char* message) override {
        static const char* const kNames[] = {"debug", "info", "warning", "error"};
        std::fprintf(stderr, "[homm2] %s: %s\n", kNames[static_cast<int>(level)], message);
    }

    void ShowMessage(const char* title, const char* message) override {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, m_video.window());
    }

private:
    struct ReplayEvent {
        std::uint32_t milliseconds;
        Event event;
    };

    void LoadReplay() {
        const char* path = SDL_getenv("HOMM2_INPUT_REPLAY");
        if (path == nullptr) {
            return;
        }

        std::ifstream stream(path);
        std::string line;
        while (std::getline(stream, line)) {
            std::istringstream fields(line);
            ReplayEvent replay;
            std::string action;
            fields >> replay.milliseconds >> action;
            if (!fields) {
                continue;
            }
            if (action == "move") {
                replay.event.type = Event::Type::MouseMove;
            } else if (action == "left-down") {
                replay.event.type = Event::Type::MouseDown;
                replay.event.button = MouseButton::Left;
            } else if (action == "left-up") {
                replay.event.type = Event::Type::MouseUp;
                replay.event.button = MouseButton::Left;
            } else if (action == "right-down") {
                replay.event.type = Event::Type::MouseDown;
                replay.event.button = MouseButton::Right;
            } else if (action == "right-up") {
                replay.event.type = Event::Type::MouseUp;
                replay.event.button = MouseButton::Right;
            } else if (action == "key-down" || action == "key-up") {
                std::string name;
                fields >> name;
                const SDL_Scancode code = SDL_GetScancodeFromName(name.c_str());
                replay.event.type =
                    action == "key-down" ? Event::Type::KeyDown : Event::Type::KeyUp;
                replay.event.key = TranslateKey(code);
                replay.event.scanCode = ToSetOneScanCode(code);
                replay.event.character =
                    name.size() == 1 ? static_cast<unsigned char>(name[0]) : 0;
            } else if (action == "text") {
                replay.event.type = Event::Type::TextInput;
                std::getline(fields, replay.event.text);
                if (!replay.event.text.empty() && replay.event.text[0] == ' ') {
                    replay.event.text.erase(0, 1);
                }
            } else {
                continue;
            }
            if (replay.event.type == Event::Type::MouseMove
                || replay.event.type == Event::Type::MouseDown
                || replay.event.type == Event::Type::MouseUp) {
                fields >> replay.event.position.x >> replay.event.position.y;
            }
            if (!fields) {
                continue;
            }
            m_replay.push_back(replay);
        }
        m_replayStart = Ticks();
    }

    void PumpReplay() {
        const std::uint32_t elapsed = Ticks() - m_replayStart;
        while (m_replayIndex < m_replay.size()
               && m_replay[m_replayIndex].milliseconds <= elapsed) {
            const Event& event = m_replay[m_replayIndex].event;
            m_input.Push(event);
            ++m_replayIndex;
        }
    }

    void Pump() {
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            Translate(sdlEvent);
        }
        PumpReplay();
    }

    void Translate(const SDL_Event& sdlEvent) {
        Event event;
        switch (sdlEvent.type) {
        case SDL_EVENT_QUIT:
            m_quit = true;
            event.type = Event::Type::Quit;
            m_input.Push(event);
            return;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            const bool down = sdlEvent.type == SDL_EVENT_KEY_DOWN;
            const SDL_Scancode code = LogicalScanCode(sdlEvent.key);
            event.type = down ? Event::Type::KeyDown : Event::Type::KeyUp;
            event.key = TranslateKey(code);
            event.scanCode = ToSetOneScanCode(code);
            event.modifiers = TranslateModifiers(sdlEvent.key.mod);
            const SDL_Keycode key = sdlEvent.key.key;
            event.character = (key > 0 && key < 128) ? static_cast<unsigned>(key) : 0;
            m_input.SetKey(event.key, down);
            m_input.Push(event);
            return;
        }

        case SDL_EVENT_TEXT_INPUT:
            event.type = Event::Type::TextInput;
            event.text = sdlEvent.text.text != nullptr ? sdlEvent.text.text : "";
            if (!event.text.empty()) {
                m_input.Push(event);
            }
            return;

        case SDL_EVENT_MOUSE_MOTION: {
            const Point position = ToFramebuffer(sdlEvent.motion.x, sdlEvent.motion.y);
            m_input.SetMouse(position);
            event.type = Event::Type::MouseMove;
            event.position = position;
            m_input.Push(event);
            return;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            const bool down = sdlEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
            event.type = down ? Event::Type::MouseDown : Event::Type::MouseUp;
            if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {

                return;
            }
            event.button = sdlEvent.button.button == SDL_BUTTON_RIGHT ? MouseButton::Right
                                                                      : MouseButton::Left;
            event.position = ToFramebuffer(sdlEvent.button.x, sdlEvent.button.y);
            m_input.SetMouse(event.position);
            m_input.SetButton(event.button, down);
            m_input.Push(event);
            return;
        }

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            event.type = Event::Type::FocusChanged;
            event.focused = sdlEvent.type == SDL_EVENT_WINDOW_FOCUS_GAINED;
            m_input.Push(event);
            return;

        default:
            return;
        }
    }

    Point ToFramebuffer(float x, float y) const {
        float logicalX = x;
        float logicalY = y;
        if (m_video.renderer() != nullptr) {
            SDL_RenderCoordinatesFromWindow(m_video.renderer(), x, y, &logicalX, &logicalY);
        }

        const int column = static_cast<int>(std::floor(logicalX));
        const int row = static_cast<int>(std::floor(logicalY));
        // Captured mouse motion and letterbox coordinates are deliberately
        // allowed outside the framebuffer. Win32 delivered those signed
        // client coordinates too; clamping them makes the black bars behave
        // like the nearest edge control and breaks drags leaving the window.
        return {column, row};
    }

    Sdl3Video& m_video;
    Sdl3Input& m_input;
    bool m_quit = false;
    std::vector<ReplayEvent> m_replay;
    std::size_t m_replayIndex = 0;
    std::uint32_t m_replayStart = 0;
};

bool AppendAudioFrame(
    AVFrame* frame,
    SwrContext* converter,
    std::vector<std::int16_t>& samples
) {
    const int channels = 2;
    const int capacity = swr_get_out_samples(converter, frame->nb_samples);
    if (capacity < 0) {
        return false;
    }
    const std::size_t first = samples.size();
    samples.resize(first + static_cast<std::size_t>(capacity) * channels);
    std::uint8_t* output =
        reinterpret_cast<std::uint8_t*>(samples.data() + first);
    const int converted = swr_convert(
        converter,
        &output,
        capacity,
        const_cast<const std::uint8_t**>(frame->extended_data),
        frame->nb_samples
    );
    if (converted < 0) {
        return false;
    }
    samples.resize(first + static_cast<std::size_t>(converted) * channels);
    return true;
}

bool ReceiveAudio(
    AVCodecContext* codec,
    AVFrame* frame,
    SwrContext* converter,
    std::vector<std::int16_t>& samples
) {
    for (;;) {
        const int result = avcodec_receive_frame(codec, frame);
        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            return true;
        }
        if (result < 0 || !AppendAudioFrame(frame, converter, samples)) {
            return false;
        }
        av_frame_unref(frame);
    }
}

bool DecodeMusic(
    const std::string& path,
    std::vector<std::int16_t>& samples,
    int& sampleRate
) {
    AVFormatContext* format = nullptr;
    AVCodecContext* codec = nullptr;
    AVFrame* frame = nullptr;
    AVPacket* packet = nullptr;
    SwrContext* converter = nullptr;
    const AVCodec* decoder = nullptr;
    int stream = -1;
    AVChannelLayout stereo = AV_CHANNEL_LAYOUT_STEREO;
    bool success = false;

    if (avformat_open_input(&format, path.c_str(), nullptr, nullptr) < 0
        || avformat_find_stream_info(format, nullptr) < 0) {
        goto done;
    }

    stream = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, &decoder, 0);
    if (stream < 0 || decoder == nullptr) {
        goto done;
    }

    codec = avcodec_alloc_context3(decoder);
    frame = av_frame_alloc();
    packet = av_packet_alloc();
    if (codec == nullptr || frame == nullptr || packet == nullptr
        || avcodec_parameters_to_context(codec, format->streams[stream]->codecpar) < 0
        || avcodec_open2(codec, decoder, nullptr) < 0) {
        goto done;
    }

    sampleRate = codec->sample_rate;
    if (swr_alloc_set_opts2(
            &converter,
            &stereo,
            AV_SAMPLE_FMT_S16,
            sampleRate,
            &codec->ch_layout,
            codec->sample_fmt,
            sampleRate,
            0,
            nullptr
        )
            < 0
        || swr_init(converter) < 0) {
        goto done;
    }

    while (av_read_frame(format, packet) >= 0) {
        if (packet->stream_index == stream
            && (avcodec_send_packet(codec, packet) < 0
                || !ReceiveAudio(codec, frame, converter, samples))) {
            av_packet_unref(packet);
            goto done;
        }
        av_packet_unref(packet);
    }
    if (avcodec_send_packet(codec, nullptr) < 0
        || !ReceiveAudio(codec, frame, converter, samples)) {
        goto done;
    }
    success = !samples.empty();

done:
    swr_free(&converter);
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    return success;
}

class Sdl3Audio final : public IAudio {
public:
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
            SDL_DestroyAudioStream(voice.stream);
        }
        m_voices.clear();
        if (m_device != 0) {
            SDL_CloseAudioDevice(m_device);
            m_device = 0;
        }
    }

    VoiceId PlaySound(const SoundData& sound, int volume, int loops) override {
        if (!Open() || sound.samples == nullptr || sound.byteCount == 0) {
            return 0;
        }

        SDL_AudioSpec source {};
        source.format = sound.bitsPerSample == 16 ? SDL_AUDIO_S16 : SDL_AUDIO_U8;
        source.channels = std::clamp(sound.channels, 1, 2);
        source.freq = sound.sampleRate;

        SDL_AudioSpec destination {};
        if (!SDL_GetAudioDeviceFormat(m_device, &destination, nullptr)) {
            return 0;
        }

        Voice voice;
        voice.stream = SDL_CreateAudioStream(&source, &destination);
        if (voice.stream == nullptr || !SDL_BindAudioStream(m_device, voice.stream)) {
            SDL_DestroyAudioStream(voice.stream);
            return 0;
        }

        const auto* first = static_cast<const std::uint8_t*>(sound.samples);
        voice.samples.assign(first, first + sound.byteCount);
        voice.infinite = loops < 0;
        SDL_SetAudioStreamGain(
            voice.stream,
            static_cast<float>(std::clamp(volume, 0, 127)) / 127.0f
        );

        const int copies = voice.infinite ? 2 : loops + 1;
        for (int copy = 0; copy < copies; ++copy) {
            if (!Queue(voice)) {
                SDL_DestroyAudioStream(voice.stream);
                return 0;
            }
        }
        if (!voice.infinite) {
            SDL_FlushAudioStream(voice.stream);
        }

        VoiceId id = m_nextVoice++;
        if (id == 0) {
            id = m_nextVoice++;
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
            SDL_SetAudioStreamGain(
                found->second.stream,
                static_cast<float>(std::clamp(volume, 0, 127)) / 127.0f
            );
        }
    }

    // Retail shipped MIDI; this port ships the Ogg recordings next to it.
    bool PlayMusicTrack(int track, int loops) override {
        if (track < 0) {
            return false;
        }

        std::vector<std::int16_t> samples;
        int sampleRate = 0;
        char relativePath[64];
        std::string path;
        const char* formats[] = {
            "MUSIC/%02d-AudioTrack %02d.ogg",
            "MUSIC/Track%02d.ogg"
        };
        for (const char* format : formats) {
            SDL_snprintf(relativePath, sizeof(relativePath), format, track, track);
            path = Files().Resolve(relativePath, FileMode::Read);
            if (DecodeMusic(path, samples, sampleRate))
                break;
            samples.clear();
        }
        if (samples.empty()) {
            std::fprintf(stderr, "[homm2] unable to decode music: %s\n", path.c_str());
            return false;
        }

        SoundData sound;
        sound.samples = samples.data();
        sound.byteCount = samples.size() * sizeof(samples[0]);
        sound.sampleRate = sampleRate;
        sound.channels = 2;
        sound.bitsPerSample = 16;
        return PlayMusic(sound, loops);
    }

    bool PlayMusic(const SoundData& sound, int loops) override {
        StopMusic();
        if (!Open() || sound.samples == nullptr || sound.byteCount == 0) {
            return false;
        }

        SDL_AudioSpec source {};
        source.format = sound.bitsPerSample == 16 ? SDL_AUDIO_S16 : SDL_AUDIO_U8;
        source.channels = std::clamp(sound.channels, 1, 2);
        source.freq = sound.sampleRate;

        SDL_AudioSpec destination {};
        if (!SDL_GetAudioDeviceFormat(m_device, &destination, nullptr)) {
            return false;
        }

        m_music.stream = SDL_CreateAudioStream(&source, &destination);
        if (m_music.stream == nullptr || !SDL_BindAudioStream(m_device, m_music.stream)) {
            SDL_DestroyAudioStream(m_music.stream);
            m_music.stream = nullptr;
            return false;
        }

        const auto* first = static_cast<const std::uint8_t*>(sound.samples);
        m_music.samples.assign(first, first + sound.byteCount);
        m_music.infinite = loops < 0;
        SDL_SetAudioStreamGain(
            m_music.stream,
            static_cast<float>(std::clamp(m_musicVolume, 0, 127)) / 127.0f
        );

        const int copies = m_music.infinite ? 2 : loops + 1;
        for (int copy = 0; copy < copies; ++copy) {
            if (!Queue(m_music)) {
                StopMusic();
                return false;
            }
        }
        if (!m_music.infinite) {
            SDL_FlushAudioStream(m_music.stream);
        }
        return true;
    }

    void StopMusic() override {
        if (m_music.stream != nullptr) {
            SDL_DestroyAudioStream(m_music.stream);
            m_music = Voice();
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
            SDL_SetAudioStreamGain(
                m_music.stream,
                static_cast<float>(m_musicVolume) / 127.0f
            );
        }
    }

    void Service() override {
        for (auto iterator = m_voices.begin(); iterator != m_voices.end();) {
            Voice& voice = iterator->second;
            if (voice.infinite) {
                if (SDL_GetAudioStreamQueued(voice.stream)
                    < static_cast<int>(voice.samples.size() * 2)) {
                    Queue(voice);
                }
                ++iterator;
            } else if (SDL_GetAudioStreamQueued(voice.stream) == 0
                       && SDL_GetAudioStreamAvailable(voice.stream) == 0) {
                SDL_DestroyAudioStream(voice.stream);
                iterator = m_voices.erase(iterator);
            } else {
                ++iterator;
            }
        }
        if (m_music.stream != nullptr && m_music.infinite
            && SDL_GetAudioStreamQueued(m_music.stream)
                   < static_cast<int>(m_music.samples.size() * 2)) {
            Queue(m_music);
        }
    }

private:
    struct Voice {
        SDL_AudioStream* stream = nullptr;
        std::vector<std::uint8_t> samples;
        bool infinite = false;
    };

    static bool Queue(Voice& voice) {
        return SDL_PutAudioStreamData(
            voice.stream,
            voice.samples.data(),
            static_cast<int>(voice.samples.size())
        );
    }

    SDL_AudioDeviceID m_device = 0;
    VoiceId m_nextVoice = 1;
    std::map<VoiceId, Voice> m_voices;
    Voice m_music;
    int m_musicVolume = 127;
};

class Sdl3FileSystem final : public IFileSystem {
public:
    Sdl3FileSystem() {
#ifdef __EMSCRIPTEN__
        m_programRoot = "/";
        m_dataRoot = "/game";
#else
        if (const char* base = SDL_GetBasePath()) {
            m_programRoot = base;
        }
        if (m_programRoot.empty()) {
            std::error_code error;
            m_programRoot = std::filesystem::current_path(error).string();
            if (error) {
                m_programRoot = ".";
            }
        }
        while (m_programRoot.size() > 1 && m_programRoot.back() == '/') {
            m_programRoot.pop_back();
        }

        if (const char* fromEnvironment = SDL_getenv("HOMM2_DATA")) {
            m_dataRoot = fromEnvironment;
        } else {
            m_dataRoot = FindDataRoot();
        }
        if (const char* localeData = SDL_getenv("HOMM2_LOCALE_DATA")) {
            m_localeDataRoot = localeData;
        }
#endif

        if (char* preferences = SDL_GetPrefPath("homm2", "homm2")) {
            m_userRoot = preferences;
            SDL_free(preferences);
        } else {
            m_userRoot = m_dataRoot;
        }

        PrepareUserState(m_userRoot);
    }

    std::string DataRoot() const override { return m_dataRoot; }
    std::string UserRoot() const override { return m_userRoot; }
    std::string ProgramRoot() const override { return m_programRoot; }
    std::string LocaleDataRoot() const override { return m_localeDataRoot; }

    std::string Resolve(const char* retailPath, FileMode mode) const override {
        if (!IsUserState(retailPath)) {
            return ResolveIn(m_dataRoot, retailPath);
        }

        // Writes go to the user directory, reads fall back to the installation
        // so that shipped saves and score tables stay in reach.
        const std::string user = ResolveIn(m_userRoot, retailPath);
        if (mode != FileMode::Read || Present(user)) {
            return user;
        }
        const std::string data = ResolveIn(m_dataRoot, retailPath);
        return Present(data) ? data : user;
    }

    bool Exists(const char* retailPath) const override {
        return Present(Resolve(retailPath, FileMode::Read));
    }

    i32 Open(const char* retailPath, FileMode mode) override {
        const char* how = "wb";
        if (mode == FileMode::Read) {
            how = "rb";
        } else if (mode == FileMode::Append) {
            how = "ab";
        }

        const std::string path = Resolve(retailPath, mode);
        SDL_IOStream* stream = SDL_IOFromFile(path.c_str(), how);
        if (stream == nullptr) {
            return -1;
        }

        const i32 file = m_nextFile++;
        m_streams.emplace(file, stream);
        return file;
    }

    i32 OpenLocale(const char* retailPath) override {
        if (m_localeDataRoot.empty()) {
            return -1;
        }
        const std::string path = ResolveIn(m_localeDataRoot, retailPath);
        SDL_IOStream* stream = SDL_IOFromFile(path.c_str(), "rb");
        if (stream == nullptr) {
            return -1;
        }
        const i32 file = m_nextFile++;
        m_streams.emplace(file, stream);
        return file;
    }

    void Close(i32 file) override {
        const auto found = m_streams.find(file);
        if (found == m_streams.end()) {
            return;
        }
        SDL_CloseIO(found->second);
        m_streams.erase(found);
    }

    i32 Read(i32 file, void* buffer, i32 count) override {
        SDL_IOStream* stream = Stream(file);
        if (stream == nullptr) {
            return -1;
        }
        return static_cast<i32>(SDL_ReadIO(stream, buffer, static_cast<std::size_t>(count)));
    }

    i32 Write(i32 file, const void* buffer, i32 count) override {
        SDL_IOStream* stream = Stream(file);
        if (stream == nullptr) {
            return -1;
        }
        return static_cast<i32>(SDL_WriteIO(stream, buffer, static_cast<std::size_t>(count)));
    }

    i32 Seek(i32 file, i32 offset) override {
        SDL_IOStream* stream = Stream(file);
        if (stream == nullptr) {
            return -1;
        }
        return static_cast<i32>(SDL_SeekIO(stream, offset, SDL_IO_SEEK_SET));
    }

    i32 Length(i32 file) override {
        SDL_IOStream* stream = Stream(file);
        if (stream == nullptr) {
            return -1;
        }
        return static_cast<i32>(SDL_GetIOSize(stream));
    }

    i32 Tell(i32 file) override {
        SDL_IOStream* stream = Stream(file);
        if (stream == nullptr) {
            return -1;
        }
        return static_cast<i32>(SDL_TellIO(stream));
    }

    std::vector<std::string> List(const char* pattern) const override {
        std::string retail = pattern != nullptr ? pattern : "";
        std::replace(retail.begin(), retail.end(), '\\', '/');
        const std::size_t slash = retail.find_last_of('/');
        const std::string directory = slash == std::string::npos ? "" : retail.substr(0, slash);
        const std::string wildcard = slash == std::string::npos ? retail : retail.substr(slash + 1);

        // A browser wants both what the game wrote and what shipped.
        std::vector<std::string> names;
        if (IsUserState(pattern)) {
            Collect(Directory(m_userRoot, directory), wildcard, names);
        }
        Collect(Directory(m_dataRoot, directory), wildcard, names);
        return names;
    }

private:
    SDL_IOStream* Stream(i32 file) const {
        const auto found = m_streams.find(file);
        return found != m_streams.end() ? found->second : nullptr;
    }

    std::string m_programRoot;
    std::string m_localeDataRoot;

    static bool Present(const std::string& path) {
        std::error_code error;
        return std::filesystem::exists(path, error);
    }

    // Where an installed copy of the game tends to sit.
    static std::string FindDataRoot() {
        std::vector<std::string> candidates;
        if (const char* base = SDL_GetBasePath()) {
            candidates.emplace_back(base);
        }
        std::error_code error;
        const std::filesystem::path working = std::filesystem::current_path(error);
        candidates.emplace_back(error ? std::string(".") : working.string());
        // XDG_DATA_HOME falls back to ~/.local/share, so the two spellings are
        // one place and everything below it follows whichever is in effect.
        std::string dataHome;
        if (const char* data = SDL_getenv("XDG_DATA_HOME"); data != nullptr && *data != '\0') {
            dataHome = data;
        } else if (const char* home = SDL_getenv("HOME")) {
            dataHome = std::string(home) + "/.local/share";
        }
        if (!dataHome.empty()) {
            candidates.emplace_back(dataHome + "/homm2");
            candidates.emplace_back(dataHome + "/homm2/data");
        }
        if (const char* home = SDL_getenv("HOME")) {
            candidates.emplace_back(std::string(home) + "/games/homm2");
        }

        for (const std::string& candidate : candidates) {
            if (HoldsGameData(candidate)) {
                return candidate;
            }
        }

        std::fprintf(
            stderr,
            "[homm2] no game data found, set HOMM2_DATA to the directory holding "
            "DATA/HEROES2.AGG\n"
        );
        return candidates.front();
    }

    // An installation carries the retail spelling, a copied one anything, which
    // is what resolving a retail path already knows how to handle.
    static bool HoldsGameData(const std::string& directory) {
        std::error_code error;
        return std::filesystem::exists(ResolveIn(directory, "DATA\\HEROES2.AGG"), error);
    }

    static std::filesystem::path Directory(const std::string& root, const std::string& directory) {
        return directory.empty() ? std::filesystem::path(root)
                                 : std::filesystem::path(ResolveIn(root, directory.c_str()));
    }

    static void Collect(
        const std::filesystem::path& root,
        const std::string& wildcard,
        std::vector<std::string>& names
    ) {
        std::error_code error;
        for (const auto& entry : std::filesystem::directory_iterator(root, error)) {
            const std::string name = entry.path().filename().string();
            if (!Matches(wildcard.c_str(), name.c_str())) {
                continue;
            }
            const auto known = [&name](const std::string& seen) {
                return SDL_strcasecmp(seen.c_str(), name.c_str()) == 0;
            };
            if (std::none_of(names.begin(), names.end(), known)) {
                names.push_back(name);
            }
        }
    }

    static bool Matches(const char* pattern, const char* name) {
        if (*pattern == '\0') {
            return *name == '\0';
        }
        if (*pattern == '*') {
            for (const char* at = name;; ++at) {
                if (Matches(pattern + 1, at)) {
                    return true;
                }
                if (*at == '\0') {
                    return false;
                }
            }
        }
        if (*name == '\0') {
            return false;
        }
        if (*pattern != '?'
            && std::tolower(static_cast<unsigned char>(*pattern))
                != std::tolower(static_cast<unsigned char>(*name))) {
            return false;
        }
        return Matches(pattern + 1, name + 1);
    }

    std::string m_dataRoot;
    std::string m_userRoot;
    std::map<i32, SDL_IOStream*> m_streams;
    i32 m_nextFile = 1;
};

struct Sdl3Backend {
    Sdl3Video video;
    Sdl3Input input{video};
    Sdl3Host host{video, input};
    Sdl3Audio audio;
    Sdl3FileSystem files;
    Backend facade;
};

Sdl3Backend* gConcrete = nullptr;

}

Backend* CreateBackend() {
    if (!SDL_Init(0)) {
        std::fprintf(stderr, "[homm2] SDL_Init: %s\n", SDL_GetError());
        return nullptr;
    }

    gConcrete = new Sdl3Backend();
    gConcrete->facade.video = &gConcrete->video;
    gConcrete->facade.input = &gConcrete->input;
    gConcrete->facade.audio = &gConcrete->audio;
    gConcrete->facade.host = &gConcrete->host;
    gConcrete->facade.files = &gConcrete->files;
    return &gConcrete->facade;
}

void DestroyBackend(Backend* backend) {
    if (backend == nullptr || gConcrete == nullptr) {
        return;
    }
    gConcrete->input.StopTextInput();
    gConcrete->video.Close();
    gConcrete->audio.Close();
    delete gConcrete;
    gConcrete = nullptr;
    SDL_Quit();
}

}
