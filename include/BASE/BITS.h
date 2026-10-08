#ifndef HOMM2_BASE_BITS_H
#define HOMM2_BASE_BITS_H

#include <H2/Ints.h>

extern "C" i32 __cdecl H2BitTest(const void*, u32);
extern "C" void __cdecl H2BitSet(void*, u32);
extern "C" void __cdecl H2BitClear(void*, u32);

#endif
