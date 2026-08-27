#ifndef HOMM2_PLATFORM_BINARY_H
#define HOMM2_PLATFORM_BINARY_H

#include <Ints.h>

#include <cstddef>

namespace platform::binary {

constexpr bool ReadU16(
    const u8* data,
    std::size_t size,
    std::size_t offset,
    u16& value
) {
    if (data == nullptr || offset > size || size - offset < 2) {
        return false;
    }
    value = static_cast<u16>(data[offset])
        | static_cast<u16>(static_cast<u16>(data[offset + 1]) << 8);
    return true;
}

constexpr bool ReadU32(
    const u8* data,
    std::size_t size,
    std::size_t offset,
    u32& value
) {
    if (data == nullptr || offset > size || size - offset < 4) {
        return false;
    }
    value = static_cast<u32>(data[offset])
        | (static_cast<u32>(data[offset + 1]) << 8)
        | (static_cast<u32>(data[offset + 2]) << 16)
        | (static_cast<u32>(data[offset + 3]) << 24);
    return true;
}

constexpr bool WriteU16(
    u8* data,
    std::size_t size,
    std::size_t offset,
    u16 value
) {
    if (data == nullptr || offset > size || size - offset < 2) {
        return false;
    }
    data[offset] = static_cast<u8>(value & 0xff);
    data[offset + 1] = static_cast<u8>(value >> 8);
    return true;
}

constexpr bool WriteU32(
    u8* data,
    std::size_t size,
    std::size_t offset,
    u32 value
) {
    if (data == nullptr || offset > size || size - offset < 4) {
        return false;
    }
    data[offset] = static_cast<u8>(value & 0xff);
    data[offset + 1] = static_cast<u8>((value >> 8) & 0xff);
    data[offset + 2] = static_cast<u8>((value >> 16) & 0xff);
    data[offset + 3] = static_cast<u8>(value >> 24);
    return true;
}

}

#endif
