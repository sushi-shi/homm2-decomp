#ifndef HOMM2_BASE_MONOCHROME_CURSOR_H
#define HOMM2_BASE_MONOCHROME_CURSOR_H

#include <Ints.h>
#include <PLATFORM/MonochromeCursor.h>

#include <span>

namespace mouse_cursor {

inline constexpr std::size_t HeaderBytes = 6;
inline constexpr std::size_t ResourceBytes = HeaderBytes
    + platform::MonochromeCursor::Width * platform::MonochromeCursor::Height;

// HoMM2's .BMP cursor entries are six-byte bitmap headers followed by 32x32
// top-down byte pixels, not Windows BMP files. Retail maps 0 to transparent,
// 1 to white, and every other value to black in its AND/XOR cursor mask.
inline bool Decode(std::span<const u8> bytes, platform::Point hotspot,
                   platform::MonochromeCursor& output) {
    using Cursor = platform::MonochromeCursor;
    if (bytes.size() != ResourceBytes || bytes[2] != Cursor::Width || bytes[3] != 0
        || bytes[4] != Cursor::Height || bytes[5] != 0
        || hotspot.x < 0 || hotspot.x >= Cursor::Width
        || hotspot.y < 0 || hotspot.y >= Cursor::Height)
        return false;

    Cursor cursor;
    cursor.hotspot = hotspot;
    for (std::size_t pixel = 0; pixel < ResourceBytes - HeaderBytes; ++pixel) {
        const u8 value = bytes[HeaderBytes + pixel];
        const auto bit = static_cast<u8>(0x80u >> (pixel % 8));
        if (value != 0)
            cursor.mask[pixel / 8] |= bit;
        if (value > 1)
            cursor.data[pixel / 8] |= bit;
    }
    output = cursor;
    return true;
}

}

#endif
