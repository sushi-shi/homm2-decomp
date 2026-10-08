#ifndef HOMM2_BASE_BITS_H
#define HOMM2_BASE_BITS_H

#include <H2/Ints.h>

extern "C" i32 __cdecl BitTest(const void*, u32);
extern "C" void __cdecl BitSet(void*, u32);
extern "C" void __cdecl BitClear(void*, u32);

#endif // HOMM2_BASE_BITS_H
