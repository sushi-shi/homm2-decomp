#include <va.h>
#include <BASE/BITSConstants.h>
#include <BASE/BITS.h>

H2_ENUM_BEGIN(BitIndexConstant)
    INDEX_BYTE_SHIFT       = 3,
    INDEX_WITHIN_BYTE_MASK = 7
H2_ENUM_END(BitIndexConstant)

VA(0x004d1594, 0x2e)
extern "C" i32 __cdecl BitTest(const void* bits, BitIndex bitIndex) {
    const BitByte* bytes = static_cast<const BitByte*>(bits);
    bytes += bitIndex >> INDEX_BYTE_SHIFT;
    const BitWord* word = reinterpret_cast<const BitWord*>(bytes);
    bitIndex &= INDEX_WITHIN_BYTE_MASK;
    BitWord mask = 1U;
    mask <<= bitIndex;
    if ((*word & mask) != 0)
        return 1;
    return 0;
}

VA(0x004d15c2, 0x20)
extern "C" void __cdecl BitSet(void* bits, BitIndex bitIndex) {
    BitByte* bytes = static_cast<BitByte*>(bits);
    bytes += bitIndex >> INDEX_BYTE_SHIFT;
    BitWord* word = reinterpret_cast<BitWord*>(bytes);
    bitIndex &= INDEX_WITHIN_BYTE_MASK;
    BitWord mask = 1U;
    mask <<= bitIndex;
    *word |= mask;
}

VA(0x004d15e2, 0x22)
extern "C" void __cdecl BitClear(void* bits, BitIndex bitIndex) {
    BitByte* bytes = static_cast<BitByte*>(bits);
    bytes += bitIndex >> INDEX_BYTE_SHIFT;
    BitWord* word = reinterpret_cast<BitWord*>(bytes);
    bitIndex &= INDEX_WITHIN_BYTE_MASK;
    BitWord mask = 1U;
    mask <<= bitIndex;
    *word &= ~mask;
}
