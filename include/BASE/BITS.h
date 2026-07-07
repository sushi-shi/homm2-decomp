#ifndef HOMM2_BITS_H
#define HOMM2_BITS_H
// Declarations of the free functions DEFINED in BITS.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

extern "C" int __cdecl BitTest(void *, int);
extern "C" void __cdecl BitSet(void *, int);
extern "C" void __cdecl BitClear(void *, int);

#endif // HOMM2_BITS_H
