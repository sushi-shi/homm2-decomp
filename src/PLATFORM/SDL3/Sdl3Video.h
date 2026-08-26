#ifndef HOMM2_PLATFORM_SDL3_VIDEO_H
#define HOMM2_PLATFORM_SDL3_VIDEO_H

#include <cstdint>
#include <vector>

#include <SDL3/SDL.h>

#include <PLATFORM/Video.h>

namespace platform::sdl3 {

class Video final : public IVideo {
public:
    ~Video() override;

    bool Open(const DisplayMode& mode) override;
    void Close() override;

    std::uint8_t* Pixels() override;
    int Pitch() const override;
    Size Resolution() const override;

    void SetPalette(const Color* entries, int first, int count) override;
    void Blit(const Rect& source, Point destination) override;
    void Present() override;
    void ShowCursor(bool visible) override;

    SDL_Window* Window() const { return m_window; }
    SDL_Renderer* Renderer() const { return m_renderer; }

private:
    void MaybeCapture();
    void LogPresentationFailure();

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_texture = nullptr;
    Size m_size;
    std::vector<std::uint8_t> m_indexed;
    std::vector<std::uint8_t> m_presented;
    std::vector<std::uint32_t> m_expanded;
    std::uint32_t m_palette[256] = {};
    int m_frame = 0;
    bool m_presentationFailureLogged = false;
};

}

#endif
