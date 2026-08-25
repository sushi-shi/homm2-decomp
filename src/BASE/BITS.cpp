#include <Ints.h>
#include <BASE/BITSConstants.h>
#include <BASE/BITS.h>

typedef enum BitIndexConstant {
    INDEX_BYTE_SHIFT       = 3,
    INDEX_WITHIN_BYTE_MASK = 7
} BitIndexConstant;

extern "C" i32 __cdecl H2BitTest(const void* bits, BitIndex bitIndex) {
    const BitByte* bytes = static_cast<const BitByte*>(bits);
    const BitWord* word = reinterpret_cast<const BitWord*>(
        bytes + (bitIndex >> INDEX_BYTE_SHIFT)
    );
    const BitWord mask = 1u << (bitIndex & INDEX_WITHIN_BYTE_MASK);
    return (*word & mask) != 0 ? 1 : 0;
}

extern "C" void __cdecl H2BitSet(void* bits, BitIndex bitIndex) {
    BitByte* bytes = static_cast<BitByte*>(bits);
    BitWord* word = reinterpret_cast<BitWord*>(
        bytes + (bitIndex >> INDEX_BYTE_SHIFT)
    );
    const BitWord mask = 1u << (bitIndex & INDEX_WITHIN_BYTE_MASK);
    *word |= mask;
}

extern "C" void __cdecl H2BitClear(void* bits, BitIndex bitIndex) {
    BitByte* bytes = static_cast<BitByte*>(bits);
    BitWord* word = reinterpret_cast<BitWord*>(
        bytes + (bitIndex >> INDEX_BYTE_SHIFT)
    );
    const BitWord mask = 1u << (bitIndex & INDEX_WITHIN_BYTE_MASK);
    *word &= ~mask;
}
