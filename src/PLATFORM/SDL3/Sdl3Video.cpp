#include "Sdl3Video.h"

#include "Sdl3Internal.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

namespace platform::sdl3 {

Video::~Video() {
    Close();
}

bool Video::Open(const DisplayMode& mode) {
    Close();
    if (mode.width <= 0 || mode.height <= 0) {
        std::fprintf(stderr, "[homm2] invalid display size: %dx%d\n", mode.width, mode.height);
        return false;
    }

    const int scale = mode.scale > 0 ? mode.scale : 2;
    if (mode.width > std::numeric_limits<int>::max() / scale
        || mode.height > std::numeric_limits<int>::max() / scale
        || static_cast<std::size_t>(mode.height)
               > std::numeric_limits<std::size_t>::max()
                   / static_cast<std::size_t>(mode.width)) {
        std::fprintf(stderr, "[homm2] display size is too large\n");
        return false;
    }

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        std::fprintf(stderr, "[homm2] SDL_InitSubSystem(video): %s\n", SDL_GetError());
        return false;
    }

    m_size = {mode.width, mode.height};
    const int windowWidth = mode.width * scale;
    const int windowHeight = mode.height * scale;
    m_window = SDL_CreateWindow(
        mode.title != nullptr ? mode.title : "Heroes of Might and Magic II",
        windowWidth,
        windowHeight,
        SDL_WINDOW_RESIZABLE
    );
    if (m_window == nullptr) {
        std::fprintf(stderr, "[homm2] SDL_CreateWindow: %s\n", SDL_GetError());
        Close();
        return false;
    }
    if (mode.fullscreen && !SDL_SetWindowFullscreen(m_window, true)) {
        std::fprintf(stderr, "[homm2] SDL_SetWindowFullscreen: %s\n", SDL_GetError());
        Close();
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (m_renderer == nullptr) {
        std::fprintf(stderr, "[homm2] SDL_CreateRenderer: %s\n", SDL_GetError());
        Close();
        return false;
    }
    if (!SDL_SetRenderLogicalPresentation(
            m_renderer,
            mode.width,
            mode.height,
            SDL_LOGICAL_PRESENTATION_LETTERBOX
        )) {
        std::fprintf(
            stderr,
            "[homm2] SDL_SetRenderLogicalPresentation: %s\n",
            SDL_GetError()
        );
        Close();
        return false;
    }

    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        mode.width,
        mode.height
    );
    if (m_texture == nullptr) {
        std::fprintf(stderr, "[homm2] SDL_CreateTexture: %s\n", SDL_GetError());
        Close();
        return false;
    }
    if (!SDL_SetTextureScaleMode(m_texture, SDL_SCALEMODE_NEAREST)) {
        std::fprintf(stderr, "[homm2] SDL_SetTextureScaleMode: %s\n", SDL_GetError());
        Close();
        return false;
    }

    const std::size_t pixelCount = static_cast<std::size_t>(mode.width)
        * static_cast<std::size_t>(mode.height);
    m_indexed.assign(pixelCount, 0);
    m_presented.assign(pixelCount, 0);
    m_expanded.assign(pixelCount, 0);
    std::fill_n(m_palette, 256, 0u);
    m_frame = 0;
    m_presentationFailureLogged = false;
    return true;
}

void Video::Close() {
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
    m_indexed.clear();
    m_presented.clear();
    m_expanded.clear();
    m_size = {};
}

std::uint8_t* Video::Pixels() {
    return m_indexed.data();
}

int Video::Pitch() const {
    return m_size.width;
}

Size Video::Resolution() const {
    return m_size;
}

void Video::Blit(const Rect& source, Point destination) {
    if (m_indexed.empty()) {
        return;
    }

    CopyRegion region;
    if (!ClipCopyRegion(source, destination, m_size, m_size, region)) {
        return;
    }

    const std::size_t pitch = static_cast<std::size_t>(m_size.width);
    for (int row = 0; row < region.height; ++row) {
        const std::uint8_t* from = m_indexed.data()
            + static_cast<std::size_t>(region.sourceY + row) * pitch
            + static_cast<std::size_t>(region.sourceX);
        std::uint8_t* to = m_presented.data()
            + static_cast<std::size_t>(region.destinationY + row) * pitch
            + static_cast<std::size_t>(region.destinationX);
        std::memcpy(to, from, static_cast<std::size_t>(region.width));
    }
}

void Video::SetPalette(const Color* entries, int first, int count) {
    if (entries == nullptr || count <= 0) {
        return;
    }
    for (int index = 0; index < count; ++index) {
        const std::int64_t slot = static_cast<std::int64_t>(first) + index;
        if (slot < 0 || slot >= 256) {
            continue;
        }
        const Color& color = entries[index];
        m_palette[slot] = (static_cast<std::uint32_t>(color.r) << 16)
            | (static_cast<std::uint32_t>(color.g) << 8)
            | static_cast<std::uint32_t>(color.b);
    }
    Present();
}

void Video::Present() {
    if (m_renderer == nullptr || m_texture == nullptr) {
        return;
    }

    const std::size_t count = m_presented.size();
    const std::uint8_t* source = m_presented.data();
    std::uint32_t* destination = m_expanded.data();
    for (std::size_t index = 0; index < count; ++index) {
        destination[index] = m_palette[source[index]];
    }

    MaybeCapture();
    if (!SDL_UpdateTexture(
            m_texture,
            nullptr,
            m_expanded.data(),
            m_size.width * static_cast<int>(sizeof(std::uint32_t))
        )
        || !SDL_RenderClear(m_renderer)
        || !SDL_RenderTexture(m_renderer, m_texture, nullptr, nullptr)
        || !SDL_RenderPresent(m_renderer)) {
        LogPresentationFailure();
    }
}

void Video::ShowCursor(bool visible) {
    if (visible) {
        SDL_ShowCursor();
    } else {
        SDL_HideCursor();
    }
}

void Video::MaybeCapture() {
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
    bool written = headerLength > 0
        && SDL_WriteIO(stream, header, static_cast<std::size_t>(headerLength))
            == static_cast<std::size_t>(headerLength);
    for (std::uint32_t pixel : m_expanded) {
        const std::uint8_t rgb[3] = {
            static_cast<std::uint8_t>((pixel >> 16) & 0xff),
            static_cast<std::uint8_t>((pixel >> 8) & 0xff),
            static_cast<std::uint8_t>(pixel & 0xff),
        };
        written = written && SDL_WriteIO(stream, rgb, sizeof(rgb)) == sizeof(rgb);
    }
    if (!SDL_CloseIO(stream)) {
        written = false;
    }
    if (!written) {
        std::fprintf(stderr, "[homm2] unable to write screenshot: %s\n", path);
    }
}

void Video::LogPresentationFailure() {
    if (!m_presentationFailureLogged) {
        std::fprintf(stderr, "[homm2] SDL presentation failed: %s\n", SDL_GetError());
        m_presentationFailureLogged = true;
    }
}

}
