#ifndef HOMM2_VA_H
#define HOMM2_VA_H

#include <Ints.h>

#ifdef __clang__

#define VA(addr, size) __attribute__((annotate("va:" #addr " size:" #size)))
#define VA_COMPGEN(addr, size, kind, owner)
#define DATA_COMPGEN(addr, name, value) value
#define DATA_COMPGEN_GUARD(addr, name, owner)
#define OVERRIDE override
#define SIZE(type, bytes) static_assert(sizeof(type) == (bytes), "sizeof(" #type ") != " #bytes)

// Where the same thing sits in the Buka target. Gold 2.1 moved code, data and
// at least one struct's fields, so the 2.1 fact is recorded beside the 2.0 one
// rather than replacing it.
#define NEW_VA(addr, size) __attribute__((annotate("new_va:" #addr " size:" #size)))
#define NEW_SIZE(type, bytes)

#else

#define VA(addr, size)
#define VA_COMPGEN(addr, size, kind, owner)
#define DATA_COMPGEN(addr, name, value) value
#define DATA_COMPGEN_GUARD(addr, name, owner)
#define OVERRIDE

#define SIZE(type, bytes)

#define NEW_VA(addr, size)
#define NEW_SIZE(type, bytes)

#endif

#endif
