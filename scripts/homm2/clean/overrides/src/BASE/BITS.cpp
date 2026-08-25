#include <BASE/BITS.h>

extern "C" i32 __cdecl H2BitTest(const void* bits, u32 bitIndex) {
    const u8* bytes = static_cast<const u8*>(bits);
    const u32* word = reinterpret_cast<const u32*>(
        bytes + (bitIndex >> 3));
    const u32 mask = 1u << (bitIndex & 7);
    return (*word & mask) != 0 ? 1 : 0;
}

extern "C" void __cdecl H2BitSet(void* bits, u32 bitIndex) {
    u8* bytes = static_cast<u8*>(bits);
    u32* word = reinterpret_cast<u32*>(bytes + (bitIndex >> 3));
    const u32 mask = 1u << (bitIndex & 7);
    *word |= mask;
}

extern "C" void __cdecl H2BitClear(void* bits, u32 bitIndex) {
    u8* bytes = static_cast<u8*>(bits);
    u32* word = reinterpret_cast<u32*>(bytes + (bitIndex >> 3));
    const u32 mask = 1u << (bitIndex & 7);
    *word &= ~mask;
}
