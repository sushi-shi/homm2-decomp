#ifndef HOMM2_PLATFORM_MONOCHROME_CURSOR_H
#define HOMM2_PLATFORM_MONOCHROME_CURSOR_H

#include <array>
#include <cstdint>

#include "Types.h"

namespace platform {

// Fixed-size, top-down, MSB-first hardware cursor. For each pixel:
// mask=0/data=0 is transparent; mask=1 selects white (data=0) or black (data=1).
struct MonochromeCursor {
    static constexpr int Width = 32;
    static constexpr int Height = 32;
    static constexpr int RowBytes = Width / 8;
    static constexpr int PlaneBytes = RowBytes * Height;
    std::array<std::uint8_t, PlaneBytes> data {};
    std::array<std::uint8_t, PlaneBytes> mask {};
    Point hotspot;

    bool operator==(const MonochromeCursor& other) const {
        return data == other.data && mask == other.mask
            && hotspot.x == other.hotspot.x && hotspot.y == other.hotspot.y;
    }
};

}

#endif
