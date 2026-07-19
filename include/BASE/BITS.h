#ifndef HOMM2_BITS_H
#define HOMM2_BITS_H

#include <Ints.h>

extern "C" i32 __cdecl BitTest(const void*, u32);
extern "C" void __cdecl BitSet(void*, u32);
extern "C" void __cdecl BitClear(void*, u32);

#endif
