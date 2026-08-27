#include <PLATFORM/Binary.h>

#include <array>

constexpr bool RoundTrips() {
    std::array<u8, 8> bytes{};
    u16 word = 0;
    u32 dword = 0;
    return platform::binary::WriteU16(bytes.data(), bytes.size(), 1, 0xa1b2)
        && platform::binary::WriteU32(bytes.data(), bytes.size(), 3, 0xc3d4e5f6)
        && bytes[1] == 0xb2 && bytes[2] == 0xa1
        && bytes[3] == 0xf6 && bytes[4] == 0xe5
        && bytes[5] == 0xd4 && bytes[6] == 0xc3
        && platform::binary::ReadU16(bytes.data(), bytes.size(), 1, word)
        && word == 0xa1b2
        && platform::binary::ReadU32(bytes.data(), bytes.size(), 3, dword)
        && dword == 0xc3d4e5f6;
}

static_assert(RoundTrips());

int main() {
    std::array<u8, 4> bytes{};
    u16 word = 7;
    u32 dword = 7;
    if (platform::binary::ReadU16(bytes.data(), 1, 0, word)
        || platform::binary::ReadU32(bytes.data(), bytes.size(), 1, dword)
        || platform::binary::WriteU16(bytes.data(), bytes.size(), 3, 0)
        || platform::binary::WriteU32(nullptr, bytes.size(), 0, 0)
        || word != 7 || dword != 7) {
        return 1;
    }
    return 0;
}
