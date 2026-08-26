#include <BASE/Bzip2.h>

#include <bzlib.h>

#include <limits>

namespace compression {

bool Bzip2CompressBound(u32 sourceSize, u32& destinationCapacity) {
    // BZ2_bzBuffToBuffCompress documents source size + 1% + 600 bytes.
    // Round the percentage up and retain one extra byte at exact boundaries.
    const u64 capacity = static_cast<u64>(sourceSize) + (sourceSize + 99ULL) / 100ULL + 601ULL;
    if (capacity > std::numeric_limits<u32>::max())
        return false;

    destinationCapacity = static_cast<u32>(capacity);
    return true;
}

bool Bzip2Compress(
    u8* destination,
    u32 destinationCapacity,
    const u8* source,
    u32 sourceSize,
    u32& compressedSize
) {
    compressedSize = 0;
    if (destination == nullptr || source == nullptr)
        return false;

    unsigned int outputSize = destinationCapacity;
    const i32 result = BZ2_bzBuffToBuffCompress(
        reinterpret_cast<char*>(destination),
        &outputSize,
        const_cast<char*>(reinterpret_cast<const char*>(source)),
        sourceSize,
        9,
        0,
        30
    );
    if (result != BZ_OK)
        return false;

    compressedSize = outputSize;
    return true;
}

bool Bzip2Decompress(
    u8* destination,
    u32 destinationCapacity,
    const u8* source,
    u32 sourceSize,
    u32& decompressedSize
) {
    decompressedSize = 0;
    if (destination == nullptr || source == nullptr)
        return false;

    unsigned int outputSize = destinationCapacity;
    const i32 result = BZ2_bzBuffToBuffDecompress(
        reinterpret_cast<char*>(destination),
        &outputSize,
        const_cast<char*>(reinterpret_cast<const char*>(source)),
        sourceSize,
        0,
        0
    );
    if (result != BZ_OK)
        return false;

    decompressedSize = outputSize;
    return true;
}

} // namespace compression
