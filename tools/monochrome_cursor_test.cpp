#include <BASE/MonochromeCursor.h>

#include <array>
#include <cstdio>

int main() {
    std::array<u8, mouse_cursor::ResourceBytes> bytes {};
    bytes[0] = 33;
    bytes[2] = bytes[4] = 32;
    platform::MonochromeCursor cursor;
    int cases = 0;
    // Exercise every value, column, row, and mask-byte boundary. The expected
    // pixel states independently follow retail's AND/XOR mapping.
    for (int value = 0; value < 256; ++value) {
        for (std::size_t i = mouse_cursor::HeaderBytes; i < bytes.size(); ++i)
            bytes[i] = static_cast<u8>((value + i - mouse_cursor::HeaderBytes) % 256);
        if (!mouse_cursor::Decode(bytes, {2, 3}, cursor)) return 1;
        for (std::size_t i = 0; i < 1024; ++i) {
            const auto bit = 0x80u >> (i % 8);
            const bool opaque = (cursor.mask[i / 8] & bit) != 0;
            const bool black = (cursor.data[i / 8] & bit) != 0;
            const auto pixel = bytes[mouse_cursor::HeaderBytes + i];
            if (opaque != (pixel != 0) || black != (pixel > 1)) return 1;
            ++cases;
        }
        if (cursor.hotspot.x != 2 || cursor.hotspot.y != 3) return 1;
    }
    const auto unchanged = cursor;
    for (const auto length : {std::size_t{0}, std::size_t{5}, bytes.size() - 1}) {
        if (mouse_cursor::Decode(std::span(bytes).first(length), {0, 0}, cursor)
            || !(cursor == unchanged)) return 1;
    }
    for (const platform::Point hotspot : {platform::Point{-1, 0}, {0, -1}, {32, 0}, {0, 32}}) {
        if (mouse_cursor::Decode(bytes, hotspot, cursor) || !(cursor == unchanged)) return 1;
    }
    bytes[2] = 31;
    if (mouse_cursor::Decode(bytes, {0, 0}, cursor)) return 1;
    bytes[2] = 32;
    bytes[4] = 31;
    if (mouse_cursor::Decode(bytes, {0, 0}, cursor)) return 1;
    std::printf("monochrome cursor: %d pixel conversions and invalid inputs passed\n", cases);
    return 0;
}
