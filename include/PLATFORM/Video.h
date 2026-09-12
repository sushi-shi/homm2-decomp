#ifndef HOMM2_PLATFORM_VIDEO_H
#define HOMM2_PLATFORM_VIDEO_H

#include "Types.h"
#include "MonochromeCursor.h"

namespace platform {

struct DisplayMode {
    int width = 640;
    int height = 480;

    int scale = 0;
    bool fullscreen = false;
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
    // Selection does not change visibility. Cursor resources belong to the
    // video backend and are released by Close(). Reset restores the OS default.
    virtual bool SetMonochromeCursor(const MonochromeCursor& cursor) = 0;
    virtual void ResetCursor() = 0;
};

}

#endif
