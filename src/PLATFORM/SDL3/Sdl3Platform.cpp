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

        m_window = SDL_CreateWindow(mode.title, mode.width * scale, mode.height * scale,
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
            SDL_WarpMouseInWindow(m_video.window(), static_cast<float>(position.x),
                                  static_cast<float>(position.y));
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
        } while (Ticks() < deadline);
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

        case SDL_EVENT_MOUSE_WHEEL:
            event.type = Event::Type::MouseWheel;
            event.wheel = static_cast<int>(sdlEvent.wheel.y);
            m_input.Push(event);
            return;

        case SDL_EVENT_WINDOW_RESIZED:
            event.type = Event::Type::Resized;
            m_input.Push(event);
            return;

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

        const Size size = m_video.Resolution();
        const int column = static_cast<int>(std::floor(logicalX));
        const int row = static_cast<int>(std::floor(logicalY));
        return {std::clamp(column, 0, size.width - 1), std::clamp(row, 0, size.height - 1)};
    }

    Sdl3Video& m_video;
    Sdl3Input& m_input;
    bool m_quit = false;
    std::vector<ReplayEvent> m_replay;
    std::size_t m_replayIndex = 0;
    std::uint32_t m_replayStart = 0;
};

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
        m_dataRoot = "/game";
#else
        if (const char* fromEnvironment = SDL_getenv("HOMM2_DATA")) {
            m_dataRoot = fromEnvironment;
        } else if (const char* base = SDL_GetBasePath()) {
            m_dataRoot = base;
        } else {
            m_dataRoot = ".";
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

    std::string Resolve(const char* retailPath) const override {
        const std::string& root = IsUserState(retailPath) ? m_userRoot : m_dataRoot;
        return root + "/" + (retailPath != nullptr ? retailPath : "");
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
    static std::filesystem::path Directory(const std::string& root, const std::string& directory) {
        return directory.empty() ? std::filesystem::path(root)
                                 : std::filesystem::path(root) / directory;
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
    gConcrete->video.Close();
    gConcrete->audio.Close();
    delete gConcrete;
    gConcrete = nullptr;
    SDL_Quit();
}

}
