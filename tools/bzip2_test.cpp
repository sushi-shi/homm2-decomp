#include <BASE/Bzip2.h>

#include <array>
#include <cstring>
#include <limits>
#include <vector>

namespace {

bool RoundTrip() {
    std::array<u8, 4096> source{};
    for (u32 index = 0; index < source.size(); ++index)
        source[index] = static_cast<u8>((index * 37U + index / 11U) & 0xFFU);

    u32 compressionCapacity = 0;
    if (!compression::Bzip2CompressBound(
            static_cast<u32>(source.size()),
            compressionCapacity
        )) {
        return false;
    }

    std::vector<u8> compressed(compressionCapacity);
    u32 compressedSize = 0;
    if (!compression::Bzip2Compress(
            compressed.data(),
            compressionCapacity,
            source.data(),
            static_cast<u32>(source.size()),
            compressedSize
        )) {
        return false;
    }
    if (compressedSize < 4 || std::memcmp(compressed.data(), "BZh", 3) != 0)
        return false;

    std::array<u8, source.size()> decompressed{};
    u32 decompressedSize = 0;
    return compression::Bzip2Decompress(
               decompressed.data(),
               static_cast<u32>(decompressed.size()),
               compressed.data(),
               compressedSize,
               decompressedSize
           )
        && decompressedSize == source.size()
        && decompressed == source;
}

bool RejectsInvalidBuffers() {
    std::array<u8, 256> source{};
    std::array<u8, 1> tooSmall{};
    u32 outputSize = 123;
    if (compression::Bzip2Compress(
            tooSmall.data(),
            static_cast<u32>(tooSmall.size()),
            source.data(),
            static_cast<u32>(source.size()),
            outputSize
        )) {
        return false;
    }
    if (outputSize != 0)
        return false;

    std::array<u8, 8> corrupt{{'B', 'Z', 'h', '9', 0, 1, 2, 3}};
    outputSize = 123;
    return !compression::Bzip2Decompress(
               source.data(),
               static_cast<u32>(source.size()),
               corrupt.data(),
               static_cast<u32>(corrupt.size()),
               outputSize
           )
        && outputSize == 0;
}

} // namespace

int main() {
    u32 capacity = 0;
    if (compression::Bzip2CompressBound(std::numeric_limits<u32>::max(), capacity))
        return 1;
    if (!RoundTrip())
        return 2;
    if (!RejectsInvalidBuffers())
        return 3;
    return 0;
}
