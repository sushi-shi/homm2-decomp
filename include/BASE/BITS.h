#ifndef HOMM2_BITS_H
#define HOMM2_BITS_H

#include <Ints.h>
// Declarations of the free functions DEFINED in BITS.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

extern "C" i32 __cdecl BitTest(const void *, u32);
extern "C" void __cdecl BitSet(void *, u32);
extern "C" void __cdecl BitClear(void *, u32);

#endif // HOMM2_BITS_H
