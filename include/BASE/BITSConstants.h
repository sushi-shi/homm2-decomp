#ifndef HOMM2_BASE_BITS_CONSTANTS_H
#define HOMM2_BASE_BITS_CONSTANTS_H

typedef enum BitIndexConstant {
    BIT_INDEX_BYTE_SHIFT = 3,
    BIT_INDEX_WITHIN_BYTE_MASK = 7
} BitIndexConstant;

typedef unsigned char BitByte;
typedef unsigned int BitWord;
typedef unsigned int BitIndex;

#endif // HOMM2_BASE_BITS_CONSTANTS_H
