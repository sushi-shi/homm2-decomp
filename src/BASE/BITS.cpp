#include <Ints.h>

#include <BASE/BITS.h>
#include <BASE/BITSConstants.h>

namespace {

constexpr u32 kByteShift = 3;
constexpr u32 kBitWithinByteMask = 7;

}

extern "C" i32 H2BitTest(const void* bits, BitIndex bitIndex) {
    const u8* bytes = static_cast<const u8*>(bits);
    const u32 index = static_cast<u32>(bitIndex);
    const u8 mask =
        static_cast<u8>(1u << (index & kBitWithinByteMask));
    return (bytes[index >> kByteShift] & mask) != 0 ? 1 : 0;
}

extern "C" void H2BitSet(void* bits, BitIndex bitIndex) {
    u8* bytes = static_cast<u8*>(bits);
    const u32 index = static_cast<u32>(bitIndex);
    const u8 mask =
        static_cast<u8>(1u << (index & kBitWithinByteMask));
    bytes[index >> kByteShift] |= mask;
}

extern "C" void H2BitClear(void* bits, BitIndex bitIndex) {
    u8* bytes = static_cast<u8*>(bits);
    const u32 index = static_cast<u32>(bitIndex);
    const u8 mask =
        static_cast<u8>(1u << (index & kBitWithinByteMask));
    bytes[index >> kByteShift] &= static_cast<u8>(~mask);
}
