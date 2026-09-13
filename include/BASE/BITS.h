#ifndef HOMM2_BITS_H
#define HOMM2_BITS_H

#include <Ints.h>

extern "C" i32 __cdecl H2BitTest(const void* bits, u32 bitIndex);
extern "C" void __cdecl H2BitSet(void* bits, u32 bitIndex);
extern "C" void __cdecl H2BitClear(void* bits, u32 bitIndex);

#endif
