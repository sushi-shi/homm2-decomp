#ifndef HOMM2_H2_MACROS_H
#define HOMM2_H2_MACROS_H

// A virtual function that overrides a base-class one. VC6 has no `override`;
// the clang analysis build checks it.
#ifdef __clang__
#define OVERRIDE override
#else
#define OVERRIDE
#endif

#define H2_C_LINKAGE extern "C"

#endif // HOMM2_H2_MACROS_H
