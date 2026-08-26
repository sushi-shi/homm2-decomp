#ifndef HOMM2_BASE_BZIP2_H
#define HOMM2_BASE_BZIP2_H

#include <Ints.h>

namespace compression {

// libbz2 requires callers to provide the output buffer. This helper computes
// its documented worst-case capacity without overflowing the 32-bit API.
bool Bzip2CompressBound(u32 sourceSize, u32& destinationCapacity);

bool Bzip2Compress(
    u8* destination,
    u32 destinationCapacity,
    const u8* source,
    u32 sourceSize,
    u32& compressedSize
);

bool Bzip2Decompress(
    u8* destination,
    u32 destinationCapacity,
    const u8* source,
    u32 sourceSize,
    u32& decompressedSize
);

} // namespace compression

#endif
