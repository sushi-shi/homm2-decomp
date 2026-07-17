#ifndef HOMM2_BASE_BITS_CONSTANTS_H
#define HOMM2_BASE_BITS_CONSTANTS_H

#include <Ints.h>

H2_ENUM_BEGIN(BitIndexConstant)
    BIT_INDEX_BYTE_SHIFT = 3,
    BIT_INDEX_WITHIN_BYTE_MASK = 7
H2_ENUM_END(BitIndexConstant)

typedef u8 BitByte;
typedef u32 BitWord;
typedef u32 BitIndex;

#endif // HOMM2_BASE_BITS_CONSTANTS_H
