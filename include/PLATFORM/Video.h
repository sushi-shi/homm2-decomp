#ifndef HOMM2_PLATFORM_VIDEO_H
#define HOMM2_PLATFORM_VIDEO_H

#include "Types.h"

namespace platform {

enum class Scaling { Nearest, Linear, Integer };

struct DisplaySettings {
    bool fullscreen = false;
    Scaling scaling = Scaling::Nearest;
    bool vsync = false;
};

class IFileSystem;

// Separate from the retail binary preferences; malformed files leave defaults intact.
bool ReadDisplaySettings(IFileSystem& files, DisplaySettings& settings);
bool WriteDisplaySettings(IFileSystem& files, const DisplaySettings& settings);

struct DisplayMode {
    int width = 640;
    int height = 480;

    int scale = 0;
    bool fullscreen = false;
    Scaling scaling = Scaling::Nearest;
    bool vsync = false;
    const char* title = "Heroes of Might and Magic II";
};

class IVideo {
public:
    virtual ~IVideo() = default;

    virtual bool Open(const DisplayMode& mode) = 0;
    virtual void Close() = 0;

    virtual std::uint8_t* Pixels() = 0;
    virtual int Pitch() const = 0;
    virtual Size Resolution() const = 0;

    virtual void SetPalette(const Color* entries, int first, int count) = 0;

    virtual void Blit(const Rect& source, Point destination) = 0;

    virtual void Present() = 0;

    virtual void ShowCursor(bool visible) = 0;

    // Change presentation without reallocating the game's indexed screen buffer.
    virtual DisplaySettings Settings() const { return {}; }
    virtual bool SetFullscreen(bool) { return false; }
    virtual bool SetScaling(Scaling) { return false; }
    virtual bool SetVSync(bool) { return false; }
};

}

#endif
